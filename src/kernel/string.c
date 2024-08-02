#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

const char* strchr(const char* str, char chr)
{
    if (str == NULL)
        return NULL;

    while (*str)
    {
        if (*str == chr)
            return str;

        ++str;
    }

    return NULL;
}

char* strcpy(char* dst, const char* src)
{
    char* origDst = dst;

    if (dst == NULL)
        return NULL;

    if (src == NULL)
    {
        *dst = '\0';
        return dst;
    }

    while (*src)
    {
        *dst = *src;
        ++src;
        ++dst;
    }
    
    *dst = '\0';
    return origDst;
}

unsigned strlen(const char* str)
{
    unsigned len = 0;
    while (*str)
    {
        ++len;
        ++str;
    }

    return len;
}

bool islower(char chr)
{
    return chr >= 'a' && chr <= 'z';
}

char toupper(char chr)
{
    return islower(chr) ? (chr - 'a' + 'A') : chr;
}

int strcmp(const char* str1, const char* str2)
{
    while (*str1 && *str2 && *str1 == *str2)
    {
        ++str1;
        ++str2;
    }

    return *str1 - *str2;
}

int strncmp(const char* str1, const char* str2, unsigned n) {
    while (n--) {
        if (*str1 != *str2) {
            return (unsigned char)*str1 - (unsigned char)*str2;
        }
        if (*str1 == '\0') {
            return 0;
        }
        str1++;
        str2++;
    }
    return 0;
}

char* strncpy(char* dst, const char* src, unsigned n)
{
    char* origDst = dst;
    if (dst == NULL)
        return NULL;
    if (src == NULL)
    {
        *dst = '\0';
        return dst;
    }
    while (n--)
    {
        *dst = *src;
        ++src;
        ++dst;
    }
    *dst = '\0';
    return origDst;
}