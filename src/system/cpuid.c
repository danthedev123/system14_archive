/*
    * cpuid.c
    * 
    * ABSTRACT:
    * 
    *   -> Abstracts the CPU 'cpuid' information.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include <cpuid.h>
#include <stdbool.h>
#include <stdint.h>
#include "../util/string.h"
#include "../util/print.h"

#define HUGE_PAGES_1G (1 << 26)

struct CPUIDRegisters
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

struct CPUIDRegisters getCpuidData()
{
    uint32_t eax, ebx, ecx, edx;

    __cpuid(0x80000001, eax, ebx, ecx, edx);

    struct CPUIDRegisters ret =
    {
        .eax = eax,
        .ebx = ebx,
        .ecx = ecx,
        .edx = edx
    };

    return ret;

}