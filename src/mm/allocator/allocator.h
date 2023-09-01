#pragma once
#include "../../limine.h"
#include <stdbool.h>

void InitializeAllocator(struct limine_memmap_response mmap);
void* PageAlloc();
void PageFree(void* addr);
