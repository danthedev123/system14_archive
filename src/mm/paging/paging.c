/*
    * paging.c
    * 
    * ABSTRACT:
    * 
    *   -> Exhibits the ability to map physical addresses to virtual addresses
    *   -> in the future we can swap to disk?
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * Resources used:
    *   -> Reference implementation ilobilix: https://github.com/ilobilo/ilobilix/blob/master/kernel/arch/x86_64/mm/vmm.cpp
    *   -> Thanks ilobilo!
    * 
    * Macros KSTATUS, KSTATUS_FAIL, KSTATUS_SUCCESS:
    *   -> Signal to kernel caller wether operation has been successful or not.
    *   -> Defined in system14.h
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
    * EXTERNAL COMPONENTS
    *   -> CR3 loading function (see pageloading.asm)
*/

#include "paging.h"
#include "../allocator/allocator.h"
#include "../heapalloc/heap.h"
#include "../../util/memutil.h"
#include "../../system14.h"
#include "../../util/string.h"
#include "../../system/cpuid_.h"
#include "../../util/print.h"

struct PT* pml4 = { 0 };

/* 
    * STRUCTURE GlobalPagingInfo
    * Stores global data including kernel bases, hhdm, memory map, and more.
    * Used for replicating kernel mappings in multitasking.
*/
struct
{
    struct limine_memmap_response mmap;
    uint64_t hhdm_base;
    uint64_t kernelSize;
    uint64_t kernelPhysBase;
    uint64_t kernelVirtBase;
    uintptr_t fbBase;
    uintptr_t fbSize;
} GlobalPagingInfo;

/*
    SUBROUTINE:

    * is_aligned(uint64_t, uint16_t)
    * Tests for alignment.
*/
int is_aligned(uint64_t addr, uint64_t boundary)
{
    if ((addr % boundary) == 0) return 1;
    return 0;
}

/*
    SUBROUTINE:

    * GetEntryNextLevel()
    * Allocates and gives you a pointer to the next level down in the page tables.
*/
struct PT* GetEntryNextLevel(struct PT *curr_level, size_t entry)
{
    if (curr_level == NULL)
    {
        return NULL;
    }

    if (!curr_level->values[entry].Present)
    {
        void* newalloc = PageAlloc();
        assert(is_aligned((uintptr_t)newalloc, 4096));

        curr_level->values[entry].PhysAddr = ((uint64_t)newalloc >> 12);
        curr_level->values[entry].Present = 0b1;
        curr_level->values[entry].RW = 0b1;

        return (struct PT*)newalloc;
    }
    else
    {
        return (struct PT*)(uint64_t)(curr_level->values[entry].PhysAddr << 12);
    }

    /* unreachable */
    return NULL;
}

/*
    SUBROUTINE:

    * MapMemory()
    * Maps a physical address to a virtual address.
*/
KSTATUS MapMemory(struct PT* tgtPml4, uint64_t virt, uint64_t phys, PageSizes size, bool user)
{
    assert(is_aligned(virt, 4096));
    assert(is_aligned(phys, 4096));

    size_t pml4_entry = (virt & ((uint64_t)0x1FF << 39)) >> 39;
    size_t pdpt_entry = (virt & ((uint64_t)0x1FF << 30)) >> 30;
    size_t pd_entry = (virt & ((uint64_t)0x1FF << 21)) >> 21;
    size_t pt_entry = (virt & ((uint64_t)0x1FF << 12)) >> 12;

    struct PT* pdpt = GetEntryNextLevel(tgtPml4, pml4_entry);

    struct PT* t;
    size_t index;

    if (size == _2M)
    {
        t = GetEntryNextLevel(pdpt, pdpt_entry);
        index = pd_entry;
    }
    else
    {
        struct PT* pd = GetEntryNextLevel(pdpt, pdpt_entry);
        struct PT* pt = GetEntryNextLevel(pd, pd_entry);
        
        t = pt;
        index = pt_entry;
    }

    if (t == NULL)
    {
        /* Failed to get page table */
        return KSTATUS_FAIL;
    }

    t->values[index].Present = 0b1;
    t->values[index].RW = 0b1;
    t->values[index].PhysAddr = ((uint64_t)phys >> 12);
    
    if (size == _2M) t->values[index].PageSize = 0b1;
    if (user)        t->values[index].UserSupervisor = 0b1;

    return KSTATUS_SUCCESS;
}

/*
    * SUBROUTINE

    * CreateDefaultMappings(struct PT*)
    * Creates default kernel/hhdm/framebuffer mappings
*/
void CreateDefaultMappings(struct PT* targetPML4)
{
    // Map the kernel
    for (uintptr_t i = 0; i < GlobalPagingInfo.kernelSize; i += 4096)
    {
        MapMemory(targetPML4, GlobalPagingInfo.kernelVirtBase + i, GlobalPagingInfo.kernelPhysBase + i, _4K, false);
    }

    // 0-2MiB
    for (uint64_t i = 0; i < 0x200000; i += 4096)
    {
        if (i)
        {
            MapMemory(targetPML4, i, i, _4K, false);
        }

        MapMemory(targetPML4, GlobalPagingInfo.hhdm_base + i, i, _4K, false);
    }

    // 2MiB-4GiB
    for (uint64_t i = 0x200000; i < 0x40000000; i += 0x200000)
    {
        MapMemory(targetPML4, i, i, _2M, false);
        MapMemory(targetPML4, GlobalPagingInfo.hhdm_base + i, i, _2M, false);
    }

    for (uint64_t i = 0x40000000; i < 0x100000000; i += 0x1000)
    {
        MapMemory(targetPML4, i, i, _4K, false);
        MapMemory(targetPML4, GlobalPagingInfo.hhdm_base + i, i, _4K, false);
    }

    // Map the other memory sections (fix for ACPI)
    for (size_t i = 0; i < GlobalPagingInfo.mmap.entry_count; i++)
    {
        for (uintptr_t j = GlobalPagingInfo.mmap.entries[i]->base; j < GlobalPagingInfo.mmap.entries[i]->base + GlobalPagingInfo.mmap.entries[i]->length; j += 4096)
        {
            if (j % 4096 != 0) continue;
            MapMemory(targetPML4, j, j, _4K, false);
        }
    }

    // Map framebuffer
    for (uintptr_t i = GlobalPagingInfo.fbBase; 
                   i < GlobalPagingInfo.fbBase + GlobalPagingInfo.fbSize;
                   i += 4096)
    {
        MapMemory(targetPML4, i, i - GlobalPagingInfo.hhdm_base, _4K, false);
    }
}

struct PT* CreateProcessPML4(void* start, void* end, bool inKernelCode)
{
    struct PT* newpml = PageAlloc();

    uintptr_t offset = 0;
    if (inKernelCode) offset = GlobalPagingInfo.kernelVirtBase;

    for (uint64_t i = 0x200000; i < 0x40000000; i += 0x200000)
    {
        MapMemory(newpml, i, i, _2M, false);
        MapMemory(newpml, GlobalPagingInfo.hhdm_base + i, i, _2M, false);
    }

    for (uintptr_t i = 0; i < GlobalPagingInfo.kernelSize; i += 4096)
    {
        MapMemory(
            newpml,
            GlobalPagingInfo.kernelVirtBase + i,
            GlobalPagingInfo.kernelPhysBase + i,
            _4K,
            false
        );
    }

    if (!inKernelCode)
    {
        for (void* addr = start; addr < end; addr += 4096)
        {
            MapMemory(newpml, (uintptr_t)addr + offset, (uintptr_t)addr, _4K, true);
        }
    }

    // TODO Load program at designated virtual addre

    return newpml;
}

/* 
    SUBROUTINE

    * LoadKernelPML4()
    * Loads the global kernel PML4.
*/
void LoadKernelPML4()
{
    /*
        EXTERNAL SUBROUTINE:

        * cr3load()
        * implemented in pageloading.asm
        * loads a page map level 4 (PML4) into the appropriate register.
    */
    cr3load((uint64_t)pml4);
}

/*
    SUBROUTINE:

    * InitializePaging()
    * Sets up paging for x86_64.
*/
void InitializePaging(struct limine_memmap_response mmap, uint64_t hhdm_base, uint64_t kernelSize, uint64_t kernelPhysBase, uint64_t kernelVirtBase, uintptr_t fbBase, uintptr_t fbSize)
{
    pml4 = PageAlloc();

    GlobalPagingInfo.mmap = mmap;
    GlobalPagingInfo.hhdm_base = hhdm_base;
    GlobalPagingInfo.kernelSize = kernelSize;
    GlobalPagingInfo.kernelPhysBase = kernelPhysBase;
    GlobalPagingInfo.kernelVirtBase = kernelVirtBase;
    GlobalPagingInfo.fbBase = fbBase;
    GlobalPagingInfo.fbSize = fbSize;

    CreateDefaultMappings(pml4);
    LoadKernelPML4();
}
