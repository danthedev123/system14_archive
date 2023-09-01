/*
    * apic.h
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

#pragma once
#include <stdint.h>

struct ICStructure
{
    uint8_t Type;
    uint8_t Length;
}__attribute__((packed));

struct ICStructureLAPIC
{
    struct ICStructure global;
    uint8_t ACPIProcessorUID;
    uint8_t APICId;
    uint32_t Flags;
}__attribute__((packed));

struct ICStructureIOAPIC
{
    struct ICStructure global;
    uint8_t IOAPICId;
    uint8_t Reserved;
    uint32_t IOAPICAddress; // I/O APIC MMIO base
    uint32_t GlobalSystemInterruptBase; // 'Global system interrupt number where I/O APIC interrupt inputs start'
}__attribute__((packed));

struct MADT
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMId[6];
    char OEMTableId[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
    uint32_t LAPICBase;
    uint32_t Flags;
    struct ICStructure ICStructBase;
}__attribute__((packed));

void InitializeAPIC();
void LAPIC_EOI();
void IOAPICRegisterIRQ(uint8_t vector, uint8_t pin);
