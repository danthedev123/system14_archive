#include "memutil.h"

/*
    * memutil.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements standard memory utility functions.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

void memset(void* memtoset, int value, size_t buffsize)
{
    unsigned char* k = memtoset;

    for (size_t i = 0; i < buffsize; i++)
    {
        k[i] = value;
    }
}

void memcpy(void* dest, void* src, size_t n)
{
    char* srca = (char*)src;
    char* desta = (char*)dest;

    for (size_t i = 0; i < n; i++)
    {
        desta[i] = srca[i];
    }
}

uintptr_t KVaddrToPaddr(uintptr_t vaddr, uintptr_t kernelVirtStart, uintptr_t kernelPhysStart)
{
    uintptr_t off = vaddr - kernelVirtStart;
    uintptr_t paddr = kernelPhysStart + off;

    return paddr;
}