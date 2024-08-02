#include "hal.h"
#include <arch/i686/gdt.h>
#include <arch/i686/idt.h>
#include <arch/i686/irq.h>
#include <arch/driver/fdc.h>
#include <arch/driver/key.h>
#include <arch/driver/ata.h>
#include <arch/driver/pmm.h>

void HAL_Initialize(bootparams_t bootparams)
{
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    i686_IRQ_Initialize();
    if (bootparams.bootDrive == 0 || bootparams.bootDrive == 1)
        init_fdc();
    else
        ata_init();
    pmm_init(&bootparams.memInfo);
    init_keyboard();
}
