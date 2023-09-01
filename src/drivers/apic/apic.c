/*
    * apic.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements the Intel Advanced Programmable Interrupt Controller
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include "apic.h"
#include "../acpi/acpi.h"
#include "../../util/print.h"
#include <stddef.h>
#include <stdbool.h>
#include "../../system14.h"

void* localAPIC = {0};
void* ioAPIC = {0};

enum
{
    IOAPICId = 0x00, // This register contains the IO APIC's id in bits 24-27. All the other bits are reserved.
    IOAPICVer_MaxRedir = 0x01, // Contains the version in bits 0-7. Maximum amount of redirection entries in bits 16-23.
    IOAPICPriority = 0x02, // Contains the arbitration priority in bits 24-27
    IOAPICRedirEntriesBase = 0x10, // 0x10 - 0x3F contains redirection entries
};

struct IOAPICRedirectionEntry
{
    uint8_t InterruptVector;
    uint8_t DeliveryMode: 3;
    uint8_t DestinationMode : 1;
    uint8_t Busy : 1;
    uint8_t Polarity : 1;
    uint8_t LTIStatus : 1;
    uint8_t TriggerMode : 1;
    uint8_t InterruptMask : 1;
    uint64_t Reserved : 39;
    uint8_t Destination;
}__attribute__((packed));

uint32_t IOAPICRead(void* ioapicBase, uint32_t reg)
{
    uint32_t volatile* target = (uint32_t volatile*)ioapicBase;

    target[0] = (reg & 0xff);
    return target[4];
}

void IOAPICWrite(void* ioapicBase, uint32_t reg, uint32_t value)
{
    uint32_t volatile* target = (uint32_t volatile*)ioapicBase;

    target[0] = (reg & 0xff);
    target[4] = value;    
}

struct IOAPICRedirectionEntry IOAPICReadRedirectionEntry(void* ioapicBase, uint32_t offset)
{
    struct IOAPICRedirectionEntry empty = {0};
    if ((offset % 2) != 0) return empty; // This is because it is split into 2 'u32's, example 0x10 and 0x11.

    uint32_t spl0 = IOAPICRead(ioapicBase, IOAPICRedirEntriesBase + offset);
    uint32_t spl1 = IOAPICRead(ioapicBase, IOAPICRedirEntriesBase + (offset + 1));

    uint64_t fullValue = ((uint64_t)spl1 << 32) | ((uint64_t)spl0);

    struct IOAPICRedirectionEntry* ret = (struct IOAPICRedirectionEntry*)&fullValue;
    return *ret;
}

KSTATUS IOAPICWriteRedirectionEntry(void* ioapicBase, uint32_t offset, struct IOAPICRedirectionEntry value)
{
    if ((offset % 2) != 0) return KSTATUS_FAIL;

    uint64_t raw = *(uint64_t*)&value;

    uint32_t spl0 = (uint32_t)raw;
    uint32_t spl1 = raw >> 32;

    IOAPICWrite(ioapicBase, IOAPICRedirEntriesBase + offset, spl0);
    IOAPICWrite(ioapicBase, IOAPICRedirEntriesBase + (offset + 1), spl1);

    return KSTATUS_SUCCESS;
}

void IOAPICRegisterIRQ(uint8_t vector, uint8_t pin)
{
    struct IOAPICRedirectionEntry entry;

    entry.InterruptVector = vector;
    entry.DeliveryMode = 0;
    entry.DestinationMode = 0;
    entry.Busy = 0;
    entry.Polarity = 0;
    entry.LTIStatus = 0;
    entry.TriggerMode = 0;
    entry.InterruptMask = 0;
    entry.Reserved = 0;
    entry.Destination = 0;

    IOAPICWriteRedirectionEntry(ioAPIC, pin * 2, entry);
}

void LAPIC_EOI()
{
    *((uint32_t volatile*)(localAPIC + 0x0B0)) = 0;
}

void InitializeAPIC()
{
    struct MADT* madt = (struct MADT*)GetMADT();
    
    localAPIC = (void*)(uintptr_t)madt->LAPICBase;

    struct ICStructure* current = &madt->ICStructBase;
    struct ICStructureIOAPIC* ioapic = NULL;

    uint32_t length = madt->Length - sizeof(struct MADT) + 2;
    /* Go through each entry */
    while (length > 0)
    {
        switch (current->Type)
        {
            case 0x1: // I/O APIC
            {
                printf("[DEBUG] Found I/O APIC\n");
                ioapic = (struct ICStructureIOAPIC*)current;
                ioAPIC = (void*)(uintptr_t)ioapic->IOAPICAddress;

                printf("[DEBUG] I/O APIC Base: 0x%x\n", ioapic->IOAPICAddress);
                printf("[DEBUG] System interrupt base: 0x%x\n", ioapic->GlobalSystemInterruptBase);

                //foundIoApic = true;    
                length -= current->Length;
                current = (struct ICStructure*)((uintptr_t)current + current->Length);   

                break;
            }
            default:
            {
                printf("[DEBUG] MADT entry: Type: 0x%x\n", current->Type);

                length -= current->Length;
                current = (struct ICStructure*)((uintptr_t)current + current->Length);
            }
        }
    }
    
    if (!ioapic)
    {
        printf("[DEBUG] I/O APIC not available\n");
        return;
    }

    uint32_t entries = IOAPICRead((void*)(uintptr_t)ioapic->IOAPICAddress, IOAPICVer_MaxRedir);
    uint32_t pin_count = ((entries >> 16) & 0xff) + 1;

    printf("[DEBUG] I/O APIC pin count: %d\n", pin_count);
}