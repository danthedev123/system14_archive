#include <stdatomic.h>
#include "spinlock.h"
#include "../util/print.h"

void spinlock_acquire(_Atomic uint8_t* lock)
{
    while (atomic_exchange(lock, 1))
    {
        asm ("hlt");
    }
}

void spinlock_release(_Atomic uint8_t* lock)
{
    atomic_store(lock, 0);
}
