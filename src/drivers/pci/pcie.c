/*
    * pcie.c
    * 
    * ABSTRACT:
    * 
    *   -> PCI-e driver for the system/14 kernel.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/
#include "../../system/panic.h"
#include "pcie.h"
#include "../acpi/acpi.h"
#include "../../system14.h"
#include "../../util/string.h"
#include "../../util/print.h"
#include "../../system/error.h"

void PCIEEnumerateFunction(uintptr_t base, uint64_t functionid)
{
    int off = functionid << 12;
    uintptr_t addr = base + off;
    
    struct PCIDeviceStruct* device = (struct PCIDeviceStruct*)addr;

    /* Confirm device is valid */
    if (device->VendorID == 0xFFFF) return;

    printf("Device:\n");
    printf("    - Vendor: %s\n", IdentifyVendor(device->VendorID));
    printf("    - Device type: %s\n", ClassCodeType(device->ClassCode));
    printf("    - Device subtype: %s\n", SubclassType(device->ClassCode, device->SubClass));
}

void PCIEEnumerateDevice(uintptr_t base, uint64_t deviceid)
{
    int off = deviceid << 15;
    uintptr_t addr = base + off;
    
    struct PCIDeviceStruct* device = (struct PCIDeviceStruct*)addr;

    /* Confirm device is valid */
    if (device->VendorID == 0xFFFF) return;

    if (!(device->HdrType & (1 << 7)))
    {
        printf("Device:\n");
        printf("    - Vendor: %s\n", IdentifyVendor(device->VendorID));
        printf("    - Device type: %s\n", ClassCodeType(device->ClassCode));
        printf("    - Device subtype: %s\n", SubclassType(device->ClassCode, device->SubClass));
        
        if (device->HdrType == 0)
        {
            struct PCIHdrType0* newStruct = (struct PCIHdrType0*)device;

            printf("    - BAR 0: 0x%x\n", newStruct->BAR0);
            printf("    - BAR 1: 0x%x\n", newStruct->BAR1);
            printf("    - BAR 2: 0x%x\n", newStruct->BAR2);
            printf("    - BAR 3: 0x%x\n", newStruct->BAR3);
            printf("    - BAR 4: 0x%x\n", newStruct->BAR4);
            printf("    - BAR 5: 0x%x\n", newStruct->BAR5);
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            PCIEEnumerateFunction(addr, i);
        }
    }

}

void PCIEEnumerateBus(uintptr_t base, uint64_t busid)
{
    int off = busid << 20;
    uintptr_t addr = base + off;

    struct PCIDeviceStruct* device = (struct PCIDeviceStruct*)addr;

    /* Confirm device is valid */
    if (device->VendorID == 0xFFFF) return;

    for (int i = 0; i < 32; i++)
    {
        PCIEEnumerateDevice(addr, i);
    }
}

void InitializePCIE()
{
    printf("PCI driver initializing...\n");
    printf("------------------------------\n");
    uint64_t mcfg = GetMCFG();

    struct ACPISystemTableHeader* mcfgTable = (struct ACPISystemTableHeader*)mcfg;

    if (!mcfgTable)
    {
        KernelSoftError("System ACPI tables do not contain MCFG -- unable to initialize PCI-e.");
        return;
    }
    
    // MCFG has 8 bytes of reserved data before the entries start
    int entryCount = (mcfgTable->Length - (sizeof(struct ACPISystemTableHeader) + 8)) / sizeof(struct PCIConfigEntry);
    
    for (int i = 0; i < entryCount; i++)
    {
        uint64_t off = mcfg + (44 + (16 * i));
        
        struct PCIConfigEntry* entry = (struct PCIConfigEntry*)off;
        
        for (int i = entry->startPCIBus; i < entry->endPCIBus; i++)
        {
            PCIEEnumerateBus((uintptr_t)entry->base, i);
        }
    }
    printf("------------------------------\n");
}

char* IdentifyVendor(int VendorCode)
{
    switch (VendorCode)
    {
        case 0x8086:
        {
            return "Intel Corporation";
        }
        case 0x1022:
        {
            return "Advanced Micro Devices, Inc. [AMD]";
        }
        case 0x1414:
        {
            return "Microsoft Corporation";
        }
        case 0x1234:
        {
            return "Generic/Emulator";
        }
        case 0x144D:
        {
            return "Samsung Electronics Co Ltd";
        }
        /* TODO implement a bunch of other vendors here */
    }

    return "Unknown";
}

char* ClassCodeType(int ClassCode)
{
    switch (ClassCode)
    {
        case 0: return "Unclassified";
        case 1: return "Mass Storage Controller";
        case 2: return "Network Controller";
        case 3: return "Display Controller";
        case 4: return "Multimedia Controller";
        case 5: return "Memory Controller";
        case 6: return "Bridge";
        case 7: return "Communication Controller";
        case 8: return "Generic System Peripheral";
        case 9: return "Input Device Controller";
        case 10: return "Docking Station";
        case 11: return "Processor";
        case 12: return "Serial Bus Controller";
        case 13: return "Wireless Controller";
        case 14: return "Intelligent Controller";
        case 15: return "Satellite Communications Controller";
        case 16: return "Encryption Controller";
        case 17: return "Signal Processing Controller";
        case 18: return "Processing Accelerators";
        case 19: return "Non-Essential Instrumentation";
        default: return "???";
    }
}

char* subclassMassStorageController[] =
{
    "SCSI Storage controller", // 00
    "IDE interface", // 01
    "Floppy disk controller", // 02
    "IPI bus controller", // 03
    "RAID bus controller", // 04
    "ATA controller", // 05
    "SATA controller", // 06
    "Serial Attached SCSI controller", // 07
    "Non-Volatile memory controller", // 08
    "Universal Flash Storage controller", // 09
};

char* subclassNetworkController[] =
{
    "Ethernet controller",
    "Token ring network controller",
    "FDDI network controller",
    "ATM network controller",
    "ISDN controller",
    "WorldFip controller",
    "PICMG controller",
    "Infiniband controller",
    "Fabric controller"
};

char* subclassDisplayController[] =
{
    "VGA compatible controller",
    "XGA compatible controller",
    "3D controller"
};

char* subclassMultimediaController[] =
{
    "Multimedia video controller",
    "Multimedia audio controller",
    "Computer telephony device",
    "Audio device"
};

char* subclassMemoryController[] =
{
    "RAM memory",
    "FLASH memory",
    "CXL"
};

char* subclassBridge[] =
{
    "Host bridge",
    "ISA bridge",
    "EISA bridge",
    "MicroChannel bridge",
    "PCI bridge",
    "PCMCIA bridge",
    "NuBus bridge",
    "CardBus bridge",
    "RACEway bridge",
    "Semi-transparent PCI-to-PCI bridge",
    "InfiniBand to PCI host bridge"
};

char* subclassCommunicationController[] =
{
    "Serial controller",
    "Parallel controller",
    "Multiport serial controller",
    "Modem",
    "GPIB controller",
    "Smart Card controller"
};

char* subclassGenericSystemPeripheral[] =
{
    "PIC",
    "DMA controller",
    "Timer",
    "RTC",
    "PCI Hot-plug controller",
    "SD Host controller",
    "IOMMU",
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    "Timing card"
};

char* subclassInputDeviceController[] =
{
    "Keyboard controller",
    "Digitizer pen",
    "Mouse controller",
    "Scanner controller",
    "Gameport controller"
};

char* subclassProcessor[] =
{
    "386",
    "486",
    "Pentium",
    0, 0, 0, 0, 0, 0, 0,
    "Alpha",
    0, 0, 0, 0, 0, 0, 0,
    "Power PC",
    0, 0, 0, 0, 0, 0, 0,
    "MIPS",
    0, 0, 0, 0, 0, 0, 0,
    "Co-processor"
};

char* subclassSerialBusController[] =
{
    "FireWire (IEEE 1394)",
    "ACCESS Bus",
    "SSA",
    "USB controller",
    "Fibre Channel",
    "SMBus",
    "InfiniBand",
    "IPMI Interface",
    "SERCOS interface",
    "CANBUS"
};

char* subclassWirelessController[] =
{
    "IRDA Controller",
    "Consumer IR controller",
    "RF controller",
    "Bluetooth",
    "Broadband",
    "802.1a controller",
    "802.1b controller"
};

char* subclassSatelliteCommunicationsController[] =
{
    "Satellite TV controller",
    "Satellite audio communication controller",
    "Satellite voice communication controller",
    "Satellite data communication controller"
};

char* subclassEncryptionController[] =
{
    "Network and computing encryption device",
    "Entertainment encryption device"
};

char* subclassSignalProcessingController[] =
{
    "DPIO module",
    "Performance counters",
    "Communication synchronizer",
    "Signal processing management"
};

char* subclassProcessingAccelerators[] =
{
    "Processing accelerators",
    "SNIA Smart Data Accelerator Interface (SDXI) controller"
};

char* SubclassType(int ClassCode, int SubclassCode)
{
    if (SubclassCode == 0x80) return "N/A";

    switch (ClassCode)
    {
        case 1: return subclassMassStorageController[SubclassCode];
        case 2: return subclassNetworkController[SubclassCode];
        case 3: return subclassDisplayController[SubclassCode];
        case 4: return subclassMultimediaController[SubclassCode];
        case 5: return subclassMemoryController[SubclassCode];
        case 6: return subclassBridge[SubclassCode];
        case 7: return subclassCommunicationController[SubclassCode];
        case 8: return subclassGenericSystemPeripheral[SubclassCode];
        case 9: return subclassInputDeviceController[SubclassCode];
        case 10: return "Generic Docking Station"; // There is only one subclass of docking station, which is this one.
        case 11: return subclassProcessor[SubclassCode];
        case 12: return subclassSerialBusController[SubclassCode];
        case 13: return subclassWirelessController[SubclassCode];
        case 14: return "I20"; // Also only one subclass
        case 15: return subclassSatelliteCommunicationsController[SubclassCode];
        case 16: return subclassEncryptionController[SubclassCode];
        case 17: return subclassSignalProcessingController[SubclassCode];
        case 18: return subclassProcessingAccelerators[SubclassCode];

        default: return "???";
    }
}
