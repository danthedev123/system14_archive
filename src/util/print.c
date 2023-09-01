/*
    * print.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements printf()
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    *
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/
#include "string.h"
#include "print.h"
#include "../util/serial.h"
#include "../flanterm/flanterm.h"
#include "../flanterm/backends/fb.h"

bool fbSetup = false;
struct flanterm_context* ft_ctx = { 0 };

void InitializeTerminal(uint32_t* framebuffer_ptr, int width, int height, int pitch)
{
    ft_ctx = flanterm_fb_simple_init(
        framebuffer_ptr, width, height, pitch
    );
    fbSetup = true;
}

void putchar(char c)
{
    if (c == '\n')
        write_serial('\r');
    write_serial(c);
    if (fbSetup) flanterm_write(ft_ctx, &c, 1);
}

void print(const char* str)
{
    while (*str != '\0')
    {
        putchar(*str);

        str++;
    }
}

/*
    * SUBROUTINE lprint(char*, int)
    * "Limited print" -- prints a string (str), but only a set amount of characters (n).
    * For printing non-null terminated strings.
*/
void lprint(const char* str, int n)
{
    int i = 0;

    while (i < n)
    {
        if (str[i] == '\0') break;

        putchar(str[i]);

        i++;
    }
}

/*
    * SUBROUTINE printf(char*, ...)
    * Standard printf() function with limited formatting options
    * For use in kernel mode debugging
*/
void printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format != '\0')
    {
        if (*format == '%') // Special formatting character
        {
            format++;
            if (*format == 'd') // Int value
            {
                int v = va_arg(args, int);

                print(itoa(v));
            }
            else if (*format == 's')
            {
                char* v = va_arg(args, char*);

                print((const char*)v);
            }
            else if (*format == 'x')
            {
                uintptr_t v = va_arg(args, uintptr_t);

                print((const char*)hexToString(v));
            }
            else if (*format == 'c')
            {
                int v = va_arg(args, int);

                putchar((char)v);
            }

            format++;
            // TODO Implement other formatting options
            // See `man printf`
        }
        else
        {
            printf("%c", *format);
            format++;
        }
    }

    va_end(args);
}
