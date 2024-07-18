#include "hal.h"
#include <arch/i686/gdt.h>
#include <arch/i686/idt.h>
#include <arch/i686/irq.h>
#include <arch/driver/fdc.h>
#include <arch/driver/key.h>

void HAL_Initialize()
{
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    i686_IRQ_Initialize();
    init_fdc();
    init_keyboard();
}
