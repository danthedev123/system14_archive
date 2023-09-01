#pragma once
#include <stdint.h>

const char* itoa(int res);
int strlen(char* str);
int strnlen(char* str, int n);
int strcmp(char* str1, char* str2);
int strncmp(char* str1, char* str2, int n);
void strcpy(char dest[], char src[]);
char* hexToString(uint64_t hexValue);