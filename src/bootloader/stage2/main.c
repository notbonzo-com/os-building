#include <stdint.h>
#include <stdio.h>
#include <x86.h>
#include <disk.h>
#include <fat.h>
#include <memdefs.h>
#include <memory.h>
#include <mbr.h>
#include <elf.h>
#include <memdetect.h>

#include <boot/bootparams.h>

uint8_t* KernelLoadBuffer = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel = (uint8_t*)MEMORY_KERNEL_ADDR;

typedef void (*KernelStart)(bootparams_t params);

void __attribute__((cdecl)) start(uint16_t bootDrive, void* partition)
{
    clrscr();

    DISK disk;
    if (!DISK_Initialize(&disk, bootDrive))
    {
        printf("Disk init error\r\n");
        goto end;
    }
    printf("Disk initialized\r\n");

    Partition part;
    MBR_DetectPartition(&part, &disk, partition);

    if (!FAT_Initialize(&part))
    {
        printf("FAT init error\r\n");
        goto end;
    }
    printf("FAT initialized\r\n");

    KernelStart kernelEntry;
    if (!ELF_Read(&part, "/boot/kernel.elf", (void**)&kernelEntry))
    {
        printf("ELF read failed, booting halted!");
        goto end;
    }
    printf("ELF loaded\r\n");

    bootparams_t params;
    params.bootDrive = bootDrive;
    Memory_Detect(&params.memInfo);
    printf("Memory detected\r\n");
    
    kernelEntry(params);

end:
    for (;;);
}
