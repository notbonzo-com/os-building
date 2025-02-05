#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <boot/bootparams.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);

bool __attribute__((cdecl)) x86_Disk_GetDriveParams(uint8_t drive,
                                                    uint8_t* driveTypeOut,
                                                    uint16_t* cylindersOut,
                                                    uint16_t* sectorsOut,
                                                    uint16_t* headsOut);

bool __attribute__((cdecl)) x86_Disk_Reset(uint8_t drive);

bool __attribute__((cdecl)) x86_Disk_Read(uint8_t drive,
                                          uint16_t cylinder,
                                          uint16_t sector,
                                          uint16_t head,
                                          uint8_t count,
                                          void* lowerDataOut);

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} E820MemoryBlock;                        

enum E820MemoryBlockType {
    E820_USABLE = 1,
    E820_RESERVED = 2,
    E820_ACPI = 3,
    E820_NVS = 4,
    E820_BAD = 5
};

int __attribute__((cdecl)) x86_E820GetNextBlock(E820MemoryBlock* block, uint32_t* id);