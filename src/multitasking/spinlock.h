#pragma once
#include <stdatomic.h>
#include <stdint.h>

#define INIT_SPINLOCK(name) _Atomic uint8_t name = ATOMIC_VAR_INIT(0)
void spinlock_acquire(_Atomic uint8_t* lock);
void spinlock_release(_Atomic uint8_t* lock);
