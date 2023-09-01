/*
    * panic.c
    * 
    * ABSTRACT:
    * 
    *   -> Exports a panic() kernel function for kernel components to use in case of a fatal exception.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/
#include "../util/print.h"

void panic(const char* customStr)
{
    printf("\n\n----------------------------------------------------------------------\n");
    printf("KERNEL PANIC\n");
    printf("A serious error has been encountered in the kernel (kernel panic).\nDebug string: %s", customStr);
    printf("\n\n\n\n");

    printf("System halt.\n");
    printf("----------------------------------------------------------------------");

    asm ("cli");

    while(1)
    {
        asm("HLT");
    }
}