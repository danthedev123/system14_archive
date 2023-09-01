/*
    * rtc.c
    * 
    * ABSTRACT:
    * 
    *   -> Exports the soft error function.
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

void KernelSoftError(const char* error)
{
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("Soft error occured in kernel: %s\n", error);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n");
}