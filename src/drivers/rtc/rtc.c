/*
    * rtc.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements the real time clock (RTC).
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include "rtc.h"
#include "../../util/ioports.h"

void InitializeRTC()
{
    asm ("cli");

    outb(0x70, 0x8B);
    uint8_t current = inb(0x71);
    outb(0x70, 0x8B);
    outb(0x71, current | 0x40);

    /* Change frequency to our preferred value */
    int rate = 15; // 15 is the slowest rate possible. Fastest is 3.
    outb(0x70, 0x8A);
    uint8_t current2 = inb(0x71);
    outb(0x70, 0x8A);
    outb(0x71, (current2 & 0xF0) | rate);

    asm ("sti");
}

void RTC_Check()
{
    outb(0x70, 0x0C);
    inb(0x71);
}
