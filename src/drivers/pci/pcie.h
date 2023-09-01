#pragma once
#include <stdint.h>

struct PCIConfigEntry
{
    uint64_t base; // 8 bytes
    uint16_t groupNumber; // 2 bytes
    uint8_t startPCIBus; // the start pci bus number. 1 byte
    uint8_t endPCIBus; // the end pci bus number. 1 byte.
    uint32_t reserved; // 4 bytes
}__attribute__((packed));

// Based on https://wiki.osdev.org/PCI
struct PCIDeviceStruct
{
    uint16_t VendorID; // Identifies the OEM of the device.
    uint16_t DeviceID; // Identifies the particular device, allocated by vendor.
    uint16_t Command; // Allows control over PCI device.
    uint16_t Status; // Used to record status for PCI-bus related events.
    uint8_t RevisionID; // Revision of the device.
    uint8_t ProgIF; // Specifies (if any) the register-level programming interface used by the device.
    uint8_t SubClass; // Specifies the exact function it performs.
    uint8_t ClassCode; // This specifies the type of function that the specific device performs.
    uint8_t CacheLineSize; // Specifies the cache line size.
    uint8_t LatencyTimer; // Specifies the latency timer of the device.
    uint8_t HdrType; // Identifies the layout of the header.
    uint8_t BIST; // Allows status & control of a device's built in self test -- BIST.
}__attribute__((packed));

struct PCIHdrType0
{
    struct PCIDeviceStruct DefaultHeader;
    
    // Base addresses
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t CardbusCISPtr;
    uint16_t SubsystemVendorId;
    uint16_t SubsystemId;
    uint32_t ExpansionROMAddress;
    uint8_t CapabilitiesPtr;
    uint32_t Reserved : 24;
    uint8_t InterruptLine;
    uint8_t InterruptPIN;
    uint8_t MinGrant;
    uint8_t MaxLatency;
}__attribute__((packed));

// TODO add more header types

char* IdentifyVendor(int);
char* ClassCodeType(int);
char* SubclassType(int ClassCode, int SubclassCode);

void InitializePCIE();
