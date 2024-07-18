#pragma once
#include <stdint.h>

typedef enum
{
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,

} IDT_FLAGS;

void i686_IDT_Initialize();
void i686_IDT_DisableGate(int interrupt);
void i686_IDT_EnableGate(int interrupt);
void i686_IDT_SetGate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags);

typedef struct 
{
    uint32_t ds;                                            // data segment pushed by us
    uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;    // pusha
    uint32_t interrupt, error;                              // we push interrupt, error is pushed automatically (or our dummy)
    uint32_t eip, cs, eflags, esp, ss;                      // pushed automatically by CPU
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);
void i686_ISR_RegisterHandler(int interrupt, ISRHandler handler);

void __attribute__((cdecl)) i686_ISR_Handler(Registers* regs);