#pragma once

#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4
#define RECURSIVE_MAPPING_INDEX 1023

#define ROUND_DOWN_TO_PAGE(addr) ((addr) & PAGE_MASK)
#define ROUND_UP_TO_PAGE(addr)   (((addr) + PAGE_SIZE - 1) & PAGE_MASK)

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t entries[1024];
} PageTable;

typedef struct {
    PageTable* tables[1024];
} PageDirectory;

extern PageDirectory* kernel_page_directory;

bool VMM_MapPage(PageDirectory* pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t flags);
bool VMM_UnmapPage(PageDirectory* pageDirectory, uint32_t virtualAddress);

void VMM_Initialize(PageDirectory* pageDirectory);
