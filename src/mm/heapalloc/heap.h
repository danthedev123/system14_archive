#pragma once
#include <stdint.h>
#include <stddef.h>

void* malloc(uint64_t size);
void* calloc(uint64_t size);
void free(void* addr);
void* aligned_alloc(size_t alignment, size_t size);
