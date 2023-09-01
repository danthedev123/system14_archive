/*
    * print.h
    * 
    * ABSTRACT:
    * 
    *   -> Declares printf()
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    *
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/
#include <stdarg.h>
#include <stdint.h>

void printf(const char* format, ...);
void lprint(const char* str, int n);

/*
    MACRO:

    * assert(bool) macro
*/
#define assert(condition) \
    if (!(condition)) { \
        printf("\033[31m[!!!] Assertion failed at line %d in %s\033[0m\n", __LINE__, __FILE__); \
    }

void InitializeTerminal(uint32_t* framebuffer_ptr, int width, int height, int pitch);
