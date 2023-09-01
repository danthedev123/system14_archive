/*
    * string.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements string functions
    * 
    * TODO: itoa() function returns weird NaN string when number is excessively large. We could add a check for too large numbers?
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * Resources used:
    *   -> StackOverflow
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include "../mm/heapalloc/heap.h"
#include "memutil.h"

// Thanks stackoverflow
const char *itoa(int res)
{
    int size = 0;
    int t = res;

    while (t / 10 != 0)
    {
        t = t / 10;
        size++;
    }
    static char ret[64];
    size++;
    ret[size] = '\0';
    t = res;
    int i = size - 1;
    while (i >= 0)
    {
        ret[i] = (t % 10) + '0';
        t = t / 10;
        i--;
    }

    return ret;
}

int strlen(char* str)
{
    int i = 0;

    while (str[i] != '\0')
    {
        i++;
    }

    return i;
}

int strnlen(char* str, int n)
{
    if (!n) return 0;

    int i = 0;

    while (i != n && str[i] != '\0')
    {
        i++;
    }

    return i;
}

int strcmp(char* str1, char* str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

int strncmp(char* str1, char* str2, int n)
{
    int i = 0;

    while (i != n && *str1 && (*str1 == *str2))
    {
        i++;
        str1++;
        str2++;
    }

    if (i == n) return 0;
    
    return *str1 - *str2;
}

void strcpy(char dest[], const char src[])
{
    int i = 0;

    while (src[i])
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

char hexString[128];
char* hexToString(uint64_t value)
{
    char* hexChars = "0123456789ABCDEF";
    char* ptr = &hexString[127];
    *ptr = '\0';

    if (value == 0)
    {
        *--ptr = '0';
    }
    else
    {
        while (value > 0)
        {
            *--ptr = hexChars[value & 0xF];
            value >>= 4;
        }
    }

    return ptr;
}
