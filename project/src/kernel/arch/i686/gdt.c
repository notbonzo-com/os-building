#include "gdt.h"
#include <stdint.h>
#include <memory.h>

GDTDescriptor g_GDTDescriptor;

typedef struct  {
    GDTEntry entries[5];
    GDTEntry tss;
} __attribute__((packed)) GDTType;

GDTType g_GDT;

TSS g_TSS = {0};

static void i686_GDT_Load()
{
    __asm__ volatile (
        "lgdtl (%0)\n"            // Load GDT descriptor
        "mov $0x10, %%ax\n"       // Load data segment selector
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        "ljmp $0x08, $.1\n"       // Far jump to reload the code segment
        ".1:\n"                   // Label to jump to
        : : "r" (&g_GDTDescriptor)
        : "ax"                    // AX is used aka "clobbered"
    );

    __asm__ volatile ("ltr %0" : : "rm" ((uint16_t)0x28) : "memory");
}

void i686_Flush_TSS()
{
    __asm__ volatile ("mov $0x28, %ax\n" "ltr %ax\n");
}

void i686_GDT_Initialize()
{
    g_GDT.entries[0] = (GDTEntry) { 0, 0, 0, 0, 0, 0 };

    /* Kernel Code */
    g_GDT.entries[1] = (GDTEntry) {
        0xFFFF, 0, 0,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
        (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),
        0
    };

    /* Kernel Data */
    g_GDT.entries[2] = (GDTEntry) {
        0xFFFF, 0, 0,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
        (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),
        0
    };

    /* User Code */
    g_GDT.entries[3] = (GDTEntry) {
        0xFFFF, 0, 0,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
        (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),
        0
    };

    /* User Data */
    g_GDT.entries[4] = (GDTEntry) {
        0xFFFF, 0, 0,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
        (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),
        0
    };

    /* TSS */
    uint32_t tss_base = (uint32_t)&g_TSS;
    uint32_t tss_limit = sizeof(TSS);

    g_GDT.tss.LimitLow = tss_limit & 0xFFFF;
    g_GDT.tss.BaseLow = (unsigned int)(&tss_base) & 0xFFFF;
    g_GDT.tss.BaseMiddle = ((unsigned int)(&g_TSS) & 0x00FF0000) >> 16;
    g_GDT.tss.Access = 0xE9;
    g_GDT.tss.FlagsLimitHi = ((tss_limit & 0xF0000) >> 16) | (0x00);
    g_GDT.tss.BaseHigh = ((unsigned int)(&g_TSS) & 0xFF000000) >> 24;


    memset(&g_TSS, 0, sizeof(g_TSS));

    g_TSS.ss0 = i686_GDT_DATA_SEGMENT;
    uint32_t stackAddr;
    __asm__ volatile ("mov %%esp, %0" : "=r" (stackAddr));
    g_TSS.esp0 = stackAddr;

    g_GDTDescriptor.Limit = sizeof(g_GDT) - 1;
    g_GDTDescriptor.Ptr = (GDTEntry*)&g_GDT;
    
    i686_GDT_Load();
}
