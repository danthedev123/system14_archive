#include <stdint.h>
#include "../util/ioports.h"

/*
    * pit.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements the programmable interrupt timer (PIT)
    *   -> Also implements sleep() function which could be used to implement that syscall.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * Resources used:
    *   -> Reference implementation "osdev": https://github.com/pdoane/osdev/blob/master/time/pit.c
    *   -> Thanks pdoane!
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include <stdbool.h>

#define DIVISOR (uint8_t)11931 // Pings every ~1ms

int tensMsSinceBoot = 0;

void InitializePIT()
{
    asm volatile ("cli"); // Ensure no interrupts are called during this

    outb(0x48, 0x00 | 0x06 | 0x30 | 0x00);
    io_wait();

    outb(0x40, DIVISOR);
    io_wait();

    outb(0x40, DIVISOR >> 8);
    io_wait();

    asm volatile ("sti"); // Re-enable them
}

void PITDriver() // Handles a tick
{
    tensMsSinceBoot++;
}