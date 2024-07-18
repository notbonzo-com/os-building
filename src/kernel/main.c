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

extern uint8_t __bss_start;
extern uint8_t __bss_end;

void printMemoryMap(memregion_t* regions, uint32_t count)
{
    log_debug("Kernel", "Memory map:\n");
    for (uint32_t i = 0; i < count; i++)
    {
        debugf("Region %d: 0x%llx-0x%llx: %u %u\n", i, regions[i].begin, regions[i].begin + regions[i].length, regions[i].type, regions[i].ACPI);
    }
}

void __attribute__((section(".entry"))) _start(bootparams_t params)
{
    memset(&__bss_start, 0, (&__bss_end) - (&__bss_start));

    HAL_Initialize();
    i686_DisableInterrupts();
    
    VGA_clrscr();
    log_debug("Kernel", "Bootdrive: %d\n", params.bootDrive);
    

    printMemoryMap(params.memInfo.regions, params.memInfo.region_count);
    
    i686_EnableInterrupts();

    for (;;)
    {
        char c = getchar();
        if (c == -1)
            continue;
        printf("%c", c);
    }

    for(;;) __asm__ volatile ("hlt\n");
}