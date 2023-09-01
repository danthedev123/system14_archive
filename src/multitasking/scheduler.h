#pragma once
#include "../interrupts/idt.h"
#include "../mm/paging/paging.h"
#include <stdbool.h>

struct ProcessFrame
{
    uint8_t pid;
    char processName[32];
    struct Registers registers;
    uint8_t quanta;
    struct PT* cr3;
    bool invalid;

    struct ProcessFrame* next;
};

void TaskSwitch(struct Registers* stack);
void AddTask(char* taskName, void* start, void* end);
void KeAddTask(char* taskName, void* start, void* end);
void TerminateTask(uint8_t pid);
void ProcessExit();
void CommonExceptionHandler(char* exceptionType);
void MarkSchedulingActive();
struct ProcessFrame GetCurrentProcess();
