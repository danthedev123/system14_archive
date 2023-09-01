#include "../limine.h"
#include "bootloader.h"

/*
 * bootloader.c
 *
 * ABSTRACT:
 *
 *   -> Abstracts data passed by bootloader.
 *
 * COPYRIGHT (C) 2023 DanielH
 * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
 *
 * HISTORY
 *   -> 2023 DanielH created
 *
 */

static volatile struct limine_framebuffer_request framebuffer_request =
{
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile struct limine_memmap_request mm_request =
{
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_rsdp_request rsdp_request =
{
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request =
{
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_address_request ka_request =
{
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_file_request kf_request =
{
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0
};

static volatile struct limine_module_request module_request =
{
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
};

static volatile struct limine_efi_system_table_request efi_request =
{
    .id = LIMINE_EFI_SYSTEM_TABLE_REQUEST,
    .revision = 0
};

struct Bootloader InitializeBootloader()
{
    struct Bootloader retBootloader = {
        .fb = 0
    };

    struct limine_memmap_response empty = {
        0
    };

    if (framebuffer_request.response)
    {
        retBootloader.fb = *framebuffer_request.response;
    }

    if (!mm_request.response == 0)
    {
        retBootloader.mmap = *mm_request.response;
    }
    else
    {
        retBootloader.mmap = empty;
    }

    if (!rsdp_request.response == 0)
    {
        retBootloader.rsdp = (uint64_t)rsdp_request.response->address;
    }
    else
    {
        retBootloader.rsdp = 0;
    }

    if (!ka_request.response == 0)
    {
        retBootloader.ka_phys_base = ka_request.response->physical_base;
        retBootloader.ka_virt_base = ka_request.response->virtual_base;
    }
    else
    {
        retBootloader.ka_phys_base = 0;
        retBootloader.ka_virt_base = 0;
    }
    if (hhdm_request.response->offset) retBootloader.hhdm_base = hhdm_request.response->offset;
    if (kf_request.response->kernel_file->size) retBootloader.kernelSize = kf_request.response->kernel_file->size;

    if (module_request.response) retBootloader.mod = module_request.response; 
    if (efi_request.response) retBootloader.efiSystemTable = efi_request.response->address;

    return retBootloader;
}
