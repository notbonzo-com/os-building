#pragma once

#include <stdint.h>

typedef struct
{
    uint16_t LimitLow;                  // limit (bits 0-15)
    uint16_t BaseLow;                   // base (bits 0-15)
    uint8_t BaseMiddle;                 // base (bits 16-23)
    uint8_t Access;                     // access
    uint8_t FlagsLimitHi;               // limit (bits 16-19) | flags
    uint8_t BaseHigh;                   // base (bits 24-31)
} __attribute__((packed)) GDTEntry;

typedef struct
{
    uint16_t Limit;                     // sizeof(gdt) - 1
    GDTEntry* Ptr;                      // address of GDT
} __attribute__((packed)) GDTDescriptor;

typedef struct {
    uint32_t prev_tss;   // Previous TSS (if hardware task switching is used)
    uint32_t esp0;       // Stack pointer to load when changing to kernel mode
    uint32_t ss0;        // Stack segment to load when changing to kernel mode
    uint32_t esp1;       // Stack pointer to load when changing to ring 1
    uint32_t ss1;        // Stack segment to load when changing to ring 1
    uint32_t esp2;       // Stack pointer to load when changing to ring 2
    uint32_t ss2;        // Stack segment to load when changing to ring 2
    uint32_t cr3;        // Page directory base register (CR3)
    uint32_t eip;        // Instruction pointer
    uint32_t eflags;     // Flags register
    uint32_t eax;        // General purpose registers
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;        // Stack pointer
    uint32_t ebp;        // Base pointer
    uint32_t esi;        // Source index
    uint32_t edi;        // Destination index
    uint32_t es;         // Segment selectors
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;        // Local Descriptor Table selector
    uint16_t trap;       // Trap on task switch
    uint16_t iomap_base; // I/O map base address
} __attribute__((packed)) TSS;

typedef enum
{
    GDT_ACCESS_CODE_READABLE                = 0x02,
    GDT_ACCESS_DATA_WRITEABLE               = 0x02,

    GDT_ACCESS_CODE_CONFORMING              = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL        = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN          = 0x04,

    GDT_ACCESS_DATA_SEGMENT                 = 0x10,
    GDT_ACCESS_CODE_SEGMENT                 = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS               = 0x00,

    GDT_ACCESS_RING0                        = 0x00,
    GDT_ACCESS_RING1                        = 0x20,
    GDT_ACCESS_RING2                        = 0x40,
    GDT_ACCESS_RING3                        = 0x60,

    GDT_ACCESS_PRESENT                      = 0x80,

} GDT_ACCESS;

typedef enum 
{
    GDT_FLAG_64BIT                          = 0x20,
    GDT_FLAG_32BIT                          = 0x40,
    GDT_FLAG_16BIT                          = 0x00,

    GDT_FLAG_GRANULARITY_1B                 = 0x00,
    GDT_FLAG_GRANULARITY_4K                 = 0x80,
} GDT_FLAGS;

#define i686_GDT_CODE_SEGMENT 0x08
#define i686_GDT_DATA_SEGMENT 0x10
#define i686_GDT_USER_CODE_SEGMENT 0x18
#define i686_GDT_USER_DATA_SEGMENT 0x20

void i686_GDT_Initialize();