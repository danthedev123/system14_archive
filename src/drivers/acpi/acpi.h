#pragma once
#include <stdint.h>

struct RSDPDescriptor
{
    uint8_t Signature[8];
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;

    /* Start XSDT -- this bit is only supported on newer systems */
    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
}__attribute__((packed));

struct ACPISystemTableHeader
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
}__attribute__((packed));

void InitializeACPI(uint64_t rsdp);
uint64_t GetMCFG();
uint64_t GetMADT();
