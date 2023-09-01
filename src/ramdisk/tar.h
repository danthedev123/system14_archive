#pragma once

struct tar_header
{
    char path[100]; // Stores the full path
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
    char padding[355]; // The header must be 512 bytes, so the rest is padded.
};

struct FileNode
{
    char FilePath[100];
    uint32_t FileSize;
    char* FileBegin;

    struct FileNode* next;
};

void InitializeRamdisk(uintptr_t ramdiskOffset);
struct FileNode* GetRamdiskListing();
void RdFileGetStream(char* path, uint8_t* buffer, int n);
uint32_t RdFileGetSz(char* path);
