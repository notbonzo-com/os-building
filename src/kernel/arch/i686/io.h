#pragma once

#include <stdint.h>

#define UNUSED_PORT 0x80

static inline void i686_outb(uint16_t port, uint8_t value)
{
    __asm__ __volatile__ (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint8_t i686_inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__ (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static inline void i686_outw(uint16_t port, uint16_t value)
{
    __asm__ __volatile__ (
        "outw %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint16_t i686_inw(uint16_t port)
{
    uint16_t ret;
    __asm__ __volatile__ (
        "inw %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static inline void i686_outl(uint16_t port, uint32_t value)
{
    __asm__ __volatile__ (
        "outl %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint32_t i686_inl(uint16_t port)
{
    uint32_t ret;
    __asm__ __volatile__ (
        "inl %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static inline void i686_EnableInterrupts()
{
    __asm__ __volatile__ ("sti");
}

static inline void i686_DisableInterrupts()
{
    __asm__ __volatile__ ("cli");
}

static inline uint8_t i686_GetInterrupts()
{
    uint32_t flags;
    __asm__ __volatile__ (
        "pushf\n"
        "pop %0"
        : "=g"(flags)
    );
    return (flags & 0x200) != 0;
}

static inline void i686_Halt()
{
    __asm__ __volatile__ ("hlt");
}

static inline void i686_Panic()
{
    __asm__ __volatile__ (
        "cli\n"
		"1:\n"
        "hlt\n"
		"jmp 1b\n"
    );
}

static inline void i686_iowait()
{
    i686_outb(UNUSED_PORT, 0);
}
