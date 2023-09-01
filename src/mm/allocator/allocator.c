/*
    * allocator.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements a simple page frame allocation. In this case the size is 4096 (0x1000).
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    *   -> Aug 27 2023 DanielH refactored
    *
*/

#include "allocator.h"
#include <stddef.h>
#include "../../util/memutil.h"
#include "../../system/panic.h"

struct Page
{
    void* ptr;
    bool free : 1;
};

struct Page* FrameList = {0};
uint64_t LargestMemSegSize = 0;

void InitializeAllocator(struct limine_memmap_response mmap)
{
    uint64_t largestEntryLength = 0;
    struct limine_memmap_entry largestEntry = {0};

    for (size_t i = 0; i < mmap.entry_count; i++)
    {        
        switch (mmap.entries[i]->type)
        {
            case LIMINE_MEMMAP_USABLE:
            {
                if (mmap.entries[i]->length > largestEntryLength)
                {
                    largestEntryLength = mmap.entries[i]->length;
                    largestEntry = *mmap.entries[i];
                }
            }
        }
    }

    if (!largestEntry.length) panic("Detected no free memory!");

    FrameList = (struct Page*)largestEntry.base;
    uint64_t nextAddr = ALIGN_UP(largestEntry.base + (sizeof(struct Page) * (largestEntry.length / 0x1000)), 0x1000);
 
    for (size_t i = 0; i < largestEntry.length / 0x1000; i++)
    {
        struct Page newPage =
        {
            .ptr = (void*)nextAddr,
            .free = true,
        };
        FrameList[i] = newPage;
        nextAddr += 0x1000;
    }

    LargestMemSegSize = largestEntry.length;
}

void* PageAlloc()
{
    for (size_t i = 0; i < LargestMemSegSize / 0x1000; i++)
    {
        if (FrameList[i].free == true)
        {
            FrameList[i].free = false;

            memset(FrameList[i].ptr, 0, 0x1000);
            return FrameList[i].ptr;
        }
    }

    panic("No free mem left!");
    return NULL;
}

void PageFree(void* addr)
{
    for (size_t i = 0; i < LargestMemSegSize / 0x1000; i++)
    {
        if (FrameList[i].ptr == addr)
        {
            FrameList[i].free = true;
        }
    }
}
