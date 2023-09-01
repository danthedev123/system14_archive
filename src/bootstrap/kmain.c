/*
                  /\
                 /  \            /\
                /    \          /  \
               /      \        /    \
              /        \      /      \
             /__________\    /        \
    /\      /____________\  /          \
   /  \    /______________\/____________\
  /    \  /________________\_____________\
 /      \/__________________\_____________\
/__________________________________________\
            system 14 - kmain file
*/

/*
    * kmain.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements the kernel entry point (_kmain), the handoff function from the bootloader.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include "../limine.h"
#include "bootloader.h"
#include "../gdt/gdt.h"
#include "../mm/allocator/allocator.h"
#include "../interrupts/idt.h"
#include "../mm/paging/paging.h"
#include "../drivers/pit.h"
#include "../mm/heapalloc/heap.h"
#include "../drivers/acpi/acpi.h"
#include "../drivers/pci/pcie.h"
#include "../util/print.h"
#include "../ramdisk/tar.h"
#include "../elf/elf.h"

extern uint8_t kernelStart[];
extern uint8_t kernelEnd[];

void _kmain()
{
    InitializeGDT();

    struct Bootloader bootloader = InitializeBootloader();

    InitializeTerminal((uint32_t*)bootloader.fb.framebuffers[0]->address,
                       bootloader.fb.framebuffers[0]->width,
                       bootloader.fb.framebuffers[0]->height,
                       bootloader.fb.framebuffers[0]->pitch);

    InitializeACPI(bootloader.rsdp);

    uintptr_t framebuffer_base = (uintptr_t)bootloader.fb.framebuffers[0]->address;
    uintptr_t framebuffer_size = (bootloader.fb.framebuffers[0]->width * bootloader.fb.framebuffers[0]->height * bootloader.fb.framebuffers[0]->bpp) + 0x1000;

    InitializeAllocator(bootloader.mmap);

    InitializePaging(bootloader.mmap,
                     bootloader.hhdm_base, kernelEnd - kernelStart,
                     bootloader.ka_phys_base, bootloader.ka_virt_base, 
                     framebuffer_base, framebuffer_size);

    InitializeIDT();

    printf("System/14 kernel, compiled on %s\n", __DATE__);

    InitializePIT();
    InitializePCIE();

    if (bootloader.mod) InitializeRamdisk((uintptr_t)bootloader.mod->modules[0]->address);

    //MarkSchedulingActive();

    char* elfTargetFileName = "a.out";

    uint32_t sz = RdFileGetSz(elfTargetFileName);
    uint8_t* buffer = NULL;

    printf("Found a.out!\nFile size = %d bytes\nLoading file into buffer...\n", sz);
    if (sz)
    {
        /* Now we get the file stream itself */
        buffer = malloc(sz); // We have the buffer of that file, from here the ELF loading code should take over.
        if (buffer)
        {
            RdFileGetStream(elfTargetFileName, buffer, sz);

            printf("    - ELF executable loaded at 0x%x\n", (uintptr_t)buffer);

            struct ElfHeader64* hdr = (struct ElfHeader64*)buffer;

            /* Test the signature */
            if (hdr->e_ident[0] != ELF_MAGIC0 ||
                hdr->e_ident[1] != ELF_MAGIC1 ||
                hdr->e_ident[2] != ELF_MAGIC2 ||
                hdr->e_ident[3] != ELF_MAGIC3)
            {
                /* Test failed */
                printf("Error: Elf signature verification failed!\n");
            }
            else
            {
                if (hdr->e_machine != EM_X86_64)
                {
                    printf("Error: Elf machine type is not x86_64!\n");
                }
                else
                {
                    printf("    - Verified machine type as x86_64\n");
                    printf("    - ELF requested virtual entry point 0x%x\n", hdr->e_entry);
                    printf("    - Enumerating program headers...\n");

                    struct ElfPheader64* firstPhdr = (struct ElfPheader64*)((uint8_t*)hdr + hdr->e_phoff);

                    int i;
                    for (i = 0; i < hdr->e_phnum; i++)
                    {
                        struct ElfPheader64* phdr = &firstPhdr[i];

                        printf("Found program header:\n");
                        printf("    - Type: 0x%x\n", phdr->p_type);
                        printf("    - File size: %d\n", phdr->p_filesz);
                        printf("    - Memory size: %d\n", phdr->p_memsz);
                        
                        if (phdr->p_filesz != phdr->p_memsz) printf("Warning: Unimplemented case (p_filesz != p_memsz).\n");
                        if (phdr->p_type == 0x1)
                        {
                            // Based on https://wiki.osdev.org/ELF
                            // Type 0x1 == loadable, time to load this segment
                            
                            // 1. Allocate virtual memory for this segment at virtual address `p_vaddr`
                            //    with size 'p_memsz', 'p_align' alignment.
                            uintptr_t align = phdr->p_align;
                            if (!align) align++; // alignment of 0 essentially means alignment to 1, or no alignment needed.

                            uintptr_t vmem_seg = (uintptr_t)aligned_alloc(0x1000, phdr->p_memsz);
                            printf("0x%x", vmem_seg);
                            // uintptr_t pmem_seg = (uintptr_t)((uint8_t*)hdr + phdr->p_offset);

                            // printf("vmmem_seg: 0x%x, aligned down to 4K: 0x%x\n", vmem_seg, ALIGN_DOWN(vmem_seg, 0x1000));
                            // printf("pmmem_seg: 0x%x, aligned down to 4K: 0x%x\n", pmem_seg, ALIGN_DOWN(pmem_seg, 0x1000));
                            (void)vmem_seg;
                        }
                    }
                }

            }
        }
        else
        {
            printf("Unable to allocate space to copy ELF to, refusing to load!\n");
        }
    }

    while(1)
    {
        asm ("hlt");
    }
}
