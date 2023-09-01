#include <stddef.h>
#include "vmm.h"
#include "../paging/paging.h"
#include "../../multitasking/scheduler.h"
#include "../allocator/allocator.h"
#include "../heapalloc/heap.h"
#include "../../util/memutil.h"

void* AllocatePage()
{
    struct ProcessFrame process = GetCurrentProcess();
    uintptr_t page = (uintptr_t)PageAlloc();

    MapMemory((void*)process.cr3, page, page, _4K, true);
    return (void*)page;
}

/* TODO continue this function */
void* AllocateBlock(size_t)
{
    // struct ProcessFrame process = GetCurrentProcess();
    // uintptr_t block = (uintptr_t)malloc(n);

    return NULL;
}
