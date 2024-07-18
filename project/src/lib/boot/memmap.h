#include <stdint.h>

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