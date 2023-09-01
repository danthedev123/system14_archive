#include "acpi.h"
#include "../../util/ioports.h"
#include "../../system14.h"
#include "../../system/panic.h"
#include "../../util/string.h"
#include "../../system14.h"
#include "../../util/print.h"

/*
    * acpi.c
    * 
    * ABSTRACT:
    * 
    *   -> ACPI "driver" for System/14. Currently used for getting the MCFG (PCI configuration space).
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

// References:
//  * UEFI-ACPI Spec https://uefi.org/specs/ACPI/6.5/
uintptr_t mcfgPtr = {0};
uintptr_t madtPtr = {0};

/*
    * SUBROUTINE EnumerateACPITables(uint32_t)
    * Enumerates ACPI tables and prints them, also fills the mcfgPtr for the PCI driver.
*/
void EnumerateACPITables(uint32_t root)
{
    struct ACPISystemTableHeader* header = (struct ACPISystemTableHeader*)(uintptr_t)root;
    int entries = (header->Length - sizeof(struct ACPISystemTableHeader)) / 4;

    uintptr_t first = (uintptr_t)(root + 36);

    printf("ACPI system table entries: \n");

    for (int i = 0; i < entries; i++)
    {
        printf("    - ");

        uintptr_t entry = first + (i * 4);
        uint32_t* entryPtr = (uint32_t*)entry;

        struct ACPISystemTableHeader* hdr = (struct ACPISystemTableHeader*)(uintptr_t)*entryPtr;

        if (!strncmp(hdr->Signature, "MCFG", 4))
        {
            mcfgPtr = (uintptr_t)hdr;
        }
        else if (!strncmp(hdr->Signature, "APIC", 4))
        {
            madtPtr = (uintptr_t)hdr;
        }

        lprint(hdr->Signature, 4);
        printf("\n");
    }
}

/*
    * SUBROUTINE InitializeACPI(uint64_t)
    * Initializes the ACPI driver.
*/
void InitializeACPI(uint64_t rsdp)
{
    if (!rsdp)
    {
        panic("RSD PTR == 0. Unable to initialize ACPI.");
    }

    struct RSDPDescriptor* rsdpDescriptor = (struct RSDPDescriptor*)rsdp;

    printf("ACPI Driver: Initializing ACPI.\n");
    printf("    - OEM: ");
    lprint((char*)rsdpDescriptor->OEMID, 6); // OEM ID is 6 bytes
    printf("\n");
    printf("    - ACPI Revision: %s\n\n", rsdpDescriptor->Revision ? "> 1.0 (versions 2.0 through 6.1)" : "1.0");
    
    // Get RSDP
    uint32_t rsdtPtr = rsdpDescriptor->RsdtAddress;
    struct ACPISystemTableHeader* rsdtHdr = (struct ACPISystemTableHeader*)(uintptr_t)rsdtPtr;

    if (!rsdtPtr) panic("System unsupported -- No ACPI support. Please make sure your hardware or virtualizer has proper support for ACPI.");

    if (strncmp(rsdtHdr->Signature, "RSDT", 4)) panic("RSDT signature failed.");

    EnumerateACPITables(rsdtPtr);
}

/*
    * SUBROUTINE GetMCFG()
    * Returns the mcfgPtr global variable.
*/
uint64_t GetMCFG()
{
    return mcfgPtr;
}

uint64_t GetMADT()
{
    return madtPtr;
}