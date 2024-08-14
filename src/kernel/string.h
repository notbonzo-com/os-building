#pragma once

#include <stdbool.h>
#include <stddef.h>

const char* strchr(const char* str, char chr);
char* strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
bool islower(char chr);
char toupper(char chr);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, unsigned n);
char* strncpy(char* dst, const char* src, unsigned n);
char* strtok_r(char* str, const char* delim, char** saveptr);
size_t strcspn(const char* str, const char* reject);
size_t strspn(const char* str, const char* accept);