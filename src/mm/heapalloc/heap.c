/*
    * heap.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements malloc() and free()
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    * 
    * Macros KSTATUS, KSTATUS_FAIL, KSTATUS_SUCCESS:
    *   -> Signal to kernel caller wether operation has been successful or not.
    *   -> Defined in system14.h
    * 
    * HISTORY
    *   -> 2023 DanielH created
    *
*/

#include "heap.h"
#include <stddef.h>
#include "../allocator/allocator.h"
#include "../../system14.h"
#include "../../util/memutil.h"

struct HeapNode
{
    uint64_t size;
    struct HeapNode* next;
};

struct HeapNode head = {0};

/*
    SUBROUTINE:

    * CreateNode()
    * Inserts a node.
*/
void CreateNode(void* addr, uint64_t size)
{
    struct HeapNode* newNode = (struct HeapNode*)addr;

    newNode->next = head.next;
    newNode->size = size;

    head.next = newNode;
}

/*
    SUBROUTINE:

    * CombineBlocks()
    * Combines many pages to create a large block.
*/
KSTATUS CombineBlocks(uint64_t size)
{
    void* initial = PageAlloc();
    int numPages = 0;

    for (size_t i = 0; i < size; i += 4096)
    {
        if (!PageAlloc())
        {
            // End of memory, we free all used pages and then fail.
            for (int i = numPages; i > 0; i--)
            {
                PageFree(initial + (4096 * i));
            }

            return KSTATUS_FAIL;
        }
        numPages++;
    }

    CreateNode(initial, (numPages * 4096));
    return KSTATUS_SUCCESS;
}

/*
    SUBROUTINE:

    * FindAndRemoveNode()
    * Finds a suitable node for given size.
*/
struct HeapNode* FindAndRemoveNode(uint64_t size)
{
    struct HeapNode* current = &head;

    while (1)
    {
        if (current->next == NULL)
        {
            if ((size + sizeof(struct HeapNode)) >= 4096)
            {
                if (CombineBlocks(size) == KSTATUS_FAIL) return NULL;
                current = &head;
                continue;
            }
            else
            {
                void* newblock = PageAlloc();

                if (newblock) CreateNode(newblock, 4096);
                else return NULL;
            }
        }

        // Node is large enough to fit the structure and the process requested size
        if ((sizeof(struct HeapNode) + size) <= current->next->size)
        {
            void* ret = current->next;
            // Unlink the node
            current->next = current->next->next;

            return ret;
        }

        current = current->next;
    }

    return NULL;
}

/*
    SUBROUTINE

    * _alloc()
    * Internal allocation function for malloc(), aligned_alloc(), calloc()
*/
void* _alloc(size_t size, size_t alignment)
{
    struct HeapNode* node = FindAndRemoveNode(size + (alignment - 1));
    if (!node) return NULL;

    uintptr_t base = (uintptr_t)node + sizeof(struct HeapNode); // Preserve the struct for adding it again in the future.
    uintptr_t base_align = ALIGN_UP(base, alignment);

    uintptr_t end = base_align + size;
    uintptr_t innerSize = node->size - sizeof(struct HeapNode);
    uintptr_t extraSize = innerSize - size;

    if (extraSize > 0) 
    {
        CreateNode((void*)end, extraSize);
        node->size -= extraSize;
    }

    return (void*)base;
}

/*
    LIBRARY EXPORT:

    * malloc()
    * Allocates n bytes of memory.
*/
void* malloc(size_t size)
{
    return _alloc(size, 1);
}

/*
    LIBRARY EXPORT:

    * aligned_alloc()
    * Allocates aligned n bytes of memory.
*/
void* aligned_alloc(size_t alignment, size_t size)
{
    void* node = _alloc(size, alignment);
    if (!node) return NULL;
    return (void*)ALIGN_UP((uintptr_t)node, alignment);
}

/*
    LIBRARY EXPORT:

    * calloc()
    * Clean allocate - zeroes out allocation before returning.
*/
void* calloc(size_t size)
{
    void* r = malloc(size);

    if (r) memset(r, 0, size);

    return r;
}

/*
    LIBRARY EXPORT:

    * free()
    * Frees heap-allocated memory at address.
*/
void free(void* addr)
{
    struct HeapNode* prevNode = (void*)(uintptr_t)addr - sizeof(struct HeapNode);

    CreateNode(prevNode, prevNode->size);
}
