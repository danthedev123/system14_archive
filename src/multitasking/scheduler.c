/*
    * scheduler.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements a simple round-robin preemptive scheduler that can spawn tasks.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include "scheduler.h"
#include "../interrupts/idt.h"
#include "../util/memutil.h"
#include "../mm/heapalloc/heap.h"
#include "../mm/allocator/allocator.h"
#include "../util/print.h"
#include "../util/string.h"
#include "../system/panic.h"
#include "../drivers/rtc/rtc.h"

uint8_t highest_pid = 1;
bool schedulingStarted = false;

struct ProcessFrame prochead =
{
    0  
};

struct ProcessFrame* current = {0};

/* 
    * SUBROUTINE TaskSwitch(struct Registers*)
    * Performs a task/context switch.
    * Called by timer interrupt.
*/
void TaskSwitch(struct Registers* stack)
{
    /*  RTC is our timer interrupt source, and 
        not doing this will cause the IRQ to stop firing.
        It involves reading RTC register C.
    */
    RTC_Check();

    if (!current || current == &prochead)
    {
        if (prochead.next)
        {
            current = prochead.next;
        }
        else return;
    }
    else memcpy(&current->registers, stack, sizeof(struct Registers));

    if (current->invalid)
    {
        TerminateTask(current->pid);

        if (current->next) current = current->next;
        if (prochead.next) current = prochead.next;

        else
        {
            current = &prochead;
            return;
        }
    }

    if (current->quanta == 0)
    {
        current->quanta = 10;

        if (current->next)
        {
            current = current->next;
        } 
        else current = prochead.next;
    }
    current->quanta--;

    memcpy(stack, &current->registers, sizeof(struct Registers));
}

void _TaskSwitch_Stage2()
{
    if (current && current != &prochead && current->cr3 && current->quanta) cr3load((uint64_t)current->cr3);
}

/*
    * SUBROUTINE AddTask(char*, void*)
    * Spawns a process.
*/
void IntAddTask(char* taskName, void* start, void* end, bool kernel)
{
    asm ("cli");

    struct ProcessFrame* frame = malloc(sizeof(struct ProcessFrame));

    frame->pid = highest_pid++;

    if (strlen(taskName) > 32) strcpy(frame->processName, "Process");
    else strcpy(frame->processName, taskName);

    frame->quanta = 10;

    frame->registers.cs = 0x08; // Kernel code segment
    frame->registers.rip = (uint64_t)start;
    frame->registers.ss = 0x10; // Kernel data segment
    frame->cr3 = CreateProcessPML4(start, end, kernel);

    frame->registers.rsp = (uint64_t)PageAlloc();
    frame->registers.flags = 0x202;

    frame->next = prochead.next;

    prochead.next = frame;

    asm ("sti");
}

void AddTask(char* taskName, void* start, void* end)
{
    IntAddTask(taskName, start, end, false);
}

void KeAddTask(char* taskName, void* start, void* end)
{
    IntAddTask(taskName, start, end, true);
}

/* 
    * SUBROUTINE TerminateTask(uint8_t)
    * Terminates a task from it's pid.
*/
void TerminateTask(uint8_t pid)
{
    printf("[DEBUG] Task termination requested for PID %d.\n", pid);

    asm ("cli");

    // Traverse the linked list
    struct ProcessFrame* current_frame = &prochead;
    struct ProcessFrame* prev_frame = NULL;

    while (1)
    {
        prev_frame = current_frame;
        current_frame = current_frame->next;

        if (current_frame->pid == pid)
        {
            // Unlink the node
            // TODO Free the frame
            prev_frame->next = current_frame->next;

            return;
        }

        if (current_frame == NULL)
        {
            /* We did not find that process, exit. */
            return;
        }
    }

    asm ("sti");
}

void ProcessExit()
{
    asm("cli");

    current->invalid = true;

    asm("sti");

    while(1);
}

void CommonExceptionHandler(char* exceptionType)
{
    asm ("cli");

    if (schedulingStarted)
    {
        if (!current) panic(exceptionType);

        printf("(!) The process \"%s\" with PID %d has been terminated due to an exception (%s).\n",
               current->processName,
               current->pid,
               exceptionType);
        
        current->invalid = true;

        asm ("sti");
        while (1) asm ("hlt");
    }
    else
    {
        panic(exceptionType);   
    }

    asm ("sti");
}

void MarkSchedulingActive()
{
    schedulingStarted = true;
}

struct ProcessFrame GetCurrentProcess()
{
    return *current;
}