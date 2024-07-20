#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <hal/hal.h>
#include <arch/i686/io.h>
#include <arch/i686/irq.h>
#include <arch/i686/vga_text.h>
#include <debug.h>
#include <arch/driver/key.h>

#include <boot/bootparams.h>
#include <arch/driver/ata.h>
#include <arch/driver/pmm.h>

#include <arch/sched.h>

extern uint8_t __bss_start;
extern uint8_t __bss_end;

void task()
{
    for (;;) {
        debugf(".");        
    }
}

void _start(bootparams_t params)
{
    memset(&__bss_start, 0, (&__bss_end) - (&__bss_start));
    HAL_Initialize(params);
    i686_DisableInterrupts();
    VGA_clrscr();
    log_debug("kernel", "Bootdrive: %d\n", params.bootDrive);


    scheduler_init();
    i686_EnableInterrupts();

    create_task(task, 0);

    for (;;)
    {
        char c = getchar();
        if (c == -1)
            continue;
        printf("%c", c);
    }

    i686_Panic();
}