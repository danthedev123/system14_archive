#pragma once

#include <stdint.h>
#include <stddef.h>

struct Bootloader
{
    struct limine_framebuffer_response fb;
    struct limine_memmap_response mmap;
    uint64_t rsdp;
    uint64_t ka_phys_base;
    uint64_t ka_virt_base;
    uint64_t hhdm_base;
    uint64_t kernelSize;
    struct limine_module_response* mod;
    void* efiSystemTable;
};

struct Bootloader InitializeBootloader();