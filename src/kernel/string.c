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

char* strtok_r(char* str, const char* delim, char** saveptr) {
    char *start;

    if (str == NULL)
        str = *saveptr;

    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    start = str;
    str = str + strcspn(str, delim);

    if (*str != '\0') {
        *str = '\0';
        *saveptr = str + 1;
    } else {
        *saveptr = str;
    }

    return start;
}

size_t strcspn(const char* str, const char* reject) {
    const char *s, *r;

    for (s = str; *s != '\0'; ++s) {
        for (r = reject; *r != '\0'; ++r) {
            if (*s == *r)
                return s - str;
        }
    }
    return s - str;
}

size_t strspn(const char* str, const char* accept) {
    const char* s;
    const char* a;

    for (s = str; *s != '\0'; ++s) {
        for (a = accept; *a != '\0'; ++a) {
            if (*s == *a)
                break;
        }
        if (*a == '\0')
            return s - str;
    }
    return s - str;
}
