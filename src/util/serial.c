/* serial console */
#include "../util/ioports.h"

/*
    * serial.c
    * 
    * ABSTRACT:
    * 
    *   -> Enables serial bus and allows printing to serial buffer.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#define COM1 0x3F8 // see https://wiki.osdev.org/Serial_COM1s

int is_transmit_empty()
{
    return inb(COM1 + 5) & 0x20;
}

void write_serial(char c)
{
    while (is_transmit_empty() == 0); // wait until COM1 is free

    outb(COM1, c);
}
