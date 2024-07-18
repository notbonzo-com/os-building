#include "idt.h"
#include <stdint.h>
#include <stdio.h>
#include <arch/i686/io.h>
#include <stddef.h>
#include <arch/i686/gdt.h>
#include <util/binary.h>

typedef struct
{
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t Reserved;
    uint8_t Flags;
    uint16_t BaseHigh;
} __attribute__((packed)) IDTEntry;

typedef struct
{
    uint16_t Limit;
    IDTEntry* Ptr;
} __attribute__((packed)) IDTDescriptor;

IDTEntry g_IDT[256];

IDTDescriptor g_IDTDescriptor = { sizeof(g_IDT) - 1, g_IDT };

ISRHandler g_ISRHandlers[256];

static const char* const g_Exceptions[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

extern void *isr_table[];

static void i686_ISR_Initialize()
{
    for (int i = 0; i < 256; i++) {
        i686_IDT_SetGate(i, isr_table[i], i686_GDT_CODE_SEGMENT, IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
        i686_IDT_EnableGate(i);
    }

    i686_IDT_DisableGate(0x80);
}

void __attribute__((cdecl)) i686_ISR_Handler(Registers* regs)
{
    if (g_ISRHandlers[regs->interrupt] != NULL)
        g_ISRHandlers[regs->interrupt](regs);

    else if (regs->interrupt >= 32)
        debugf("Unhandled interrupt %d!\n", regs->interrupt);

    else 
    {
        debugf("Unhandled exception %d %s\n", regs->interrupt, g_Exceptions[regs->interrupt]);
        
        debugf("  eax=%x  ebx=%x  ecx=%x  edx=%x  esi=%x  edi=%x\n",
               regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);

        debugf("  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x\n",
               regs->esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds, regs->ss);

        debugf("  interrupt=%x  errorcode=%x\n", regs->interrupt, regs->error);

        debugf("KERNEL PANIC!\n");
        i686_Panic();
    }
}

void i686_IDT_SetGate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags)
{
    g_IDT[interrupt].BaseLow = ((uint32_t)base) & 0xFFFF;
    g_IDT[interrupt].SegmentSelector = segmentDescriptor;
    g_IDT[interrupt].Reserved = 0;
    g_IDT[interrupt].Flags = flags;
    g_IDT[interrupt].BaseHigh = ((uint32_t)base >> 16) & 0xFFFF;
}

void i686_IDT_EnableGate(int interrupt)
{
    FLAG_SET(g_IDT[interrupt].Flags, IDT_FLAG_PRESENT);
}

void i686_IDT_DisableGate(int interrupt)
{
    FLAG_UNSET(g_IDT[interrupt].Flags, IDT_FLAG_PRESENT);
}

void i686_ISR_RegisterHandler(int interrupt, ISRHandler handler)
{
    g_ISRHandlers[interrupt] = handler;
    i686_IDT_EnableGate(interrupt);
}

void i686_IDT_Initialize()
{
    __asm__ volatile ("lidt %0" : : "m" (g_IDTDescriptor) : "memory");
    i686_ISR_Initialize();
}