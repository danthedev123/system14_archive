// https://wiki.osdev.org/Tar
/*
    * tar.c
    * 
    * ABSTRACT:
    * 
    *   -> Implements the tar (Tape Archive) file storage system.
    * 
    * COPYRIGHT (C) 2023 DanielH
    * This code along with the rest of this project is licensed under the MIT open-source license. A copy of the MIT license should be included in the source tree.
    *
    * 
    * HISTORY
    *   -> 2023 DanielH created
    * 
*/

#include <stdint.h>
#include <stdbool.h>
#include "../util/print.h"
#include "tar.h"
#include "../mm/heapalloc/heap.h"
#include "../util/memutil.h"
#include "../util/string.h"

bool ramdiskInitialized = false;

struct FileNode filehead =
{
    0
};

void InsertEntry(char FilePath[100], uint32_t FileSize, char* FileBegin)
{
    // Insert the entry
    struct FileNode* node = malloc(sizeof(struct FileNode));

    node->next = filehead.next;
    strcpy(node->FilePath, FilePath);
    node->FileSize = FileSize;
    node->FileBegin = FileBegin;

    filehead.next = node;
}

struct FileNode* GetRamdiskListing()
{
    if (!ramdiskInitialized) return NULL;

    return &filehead;
}

/* Only for internal use */
struct FileNode* RdFileGetFrame(char* path)
{
    if (!ramdiskInitialized) return NULL;
    if (!path) return NULL;

    struct FileNode* current = &filehead;

    while (current->next != 0)
    {
        if (strncmp((char*)path, (char*)current->next->FilePath, 100) == 0)
        {
            return current->next;
        }

        current = current->next;
    }

    return NULL;
}

void RdFileGetStream(char* path, uint8_t* buffer, int n)
{
    if (!ramdiskInitialized) return;

    struct FileNode* frame = RdFileGetFrame(path);
    if (!frame) return;
    if (!frame->FileBegin) return;

    memcpy(buffer, frame->FileBegin, n);
}

/* Gets info about a file */
uint32_t RdFileGetSz(char* path)
{
    if (!ramdiskInitialized) return 0;

    struct FileNode* frame = RdFileGetFrame(path);
    if (!frame) return 0;

    return frame->FileSize;
}

// All the integer values in the tar format are stored as ASCII characters, and the value is encoded in
// base 8.
// Thanks to https://wiki.osdev.org/Tar for this decoding algorithm.
uint32_t DecodeValue(const char* value)
{
    uint32_t ret = 0;
    uint32_t count = 1;

    for (int j = 11; j > 0; j--, count *= 8)
    {
        ret += ((value[j - 1] - '0') * count);
    }

    return ret;
}

void InitializeRamdisk(uintptr_t ramdiskOffset)
{
    uint8_t* rdstart = (uint8_t*)ramdiskOffset; // The first file

    while (*rdstart != 0)
    {
        struct tar_header* file = (struct tar_header*)rdstart;

        printf("Registering file:\n");
        printf("    - File path: %s\n", file->path);
        /* We can asume if a file size is 0 bytes then it is a directory */
        if (DecodeValue(file->size) != 0) printf("    - File size: %d bytes\n", DecodeValue(file->size));
        else printf("    - File is directory/0 bytes\n");
        
        InsertEntry(file->path, DecodeValue(file->size), (char*)rdstart + 512);

        rdstart += ALIGN_UP(512 + DecodeValue(file->size), 512);
    }
    
    ramdiskInitialized = true;
};