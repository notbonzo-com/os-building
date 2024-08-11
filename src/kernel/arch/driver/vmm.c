#include <arch/driver/pmm.h>
#include <arch/driver/vmm.h>
#include <memory.h>

PageDirectory* kernel_page_directory;

// Linker symbols for mapping
extern char __text_start[], __text_end[], __rodata_start[], __rodata_end[], __data_start[], __data_end[], __bss_start[], __bss_end[]; 

static void enable_paging(PageDirectory* pd) {

	__asm__ volatile (
		"mov %0, %%cr3\n"
		"mov %%cr0, %%eax\n"
		"or $0x80000000, %%eax\n"
		"mov %%eax, %%cr0\n"
		: : "r" (pd)
		: "rax"
	);

}

void VMM_Initialize(PageDirectory* pageDirectory)
{
	pageDirectory = alloc_pages(1);
	memset(pageDirectory, 0, PAGE_SIZE);

	// Map the recursive mapping for kernel being to access the paging structures after paging enabled
	pageDirectory->tables[RECURSIVE_MAPPING_INDEX] = (PageTable*)((ROUND_UP_TO_PAGE((uint32_t)pageDirectory)) | PAGE_PRESENT | PAGE_RW);

	uint32_t text_start = ROUND_DOWN_TO_PAGE((uint32_t)__text_start);
    uint32_t text_end = ROUND_UP_TO_PAGE((uint32_t)__text_end);
	for (uint32_t addr = text_start; addr < text_end; addr += PAGE_SIZE)
    {
        VMM_MapPage(pageDirectory, addr, addr, PAGE_PRESENT | PAGE_RW); // Read-Execute permissions
    }

	uint32_t rodata_start = ROUND_DOWN_TO_PAGE((uint32_t)__rodata_start);
    uint32_t rodata_end = ROUND_UP_TO_PAGE((uint32_t)__rodata_end);
    for (uint32_t addr = rodata_start; addr < rodata_end; addr += PAGE_SIZE)
    {
        VMM_MapPage(pageDirectory, addr, addr, PAGE_PRESENT); // Read-Only permissions
    }

    uint32_t data_start = ROUND_DOWN_TO_PAGE((uint32_t)__data_start);
    uint32_t data_end = ROUND_UP_TO_PAGE((uint32_t)__data_end);
    for (uint32_t addr = data_start; addr < data_end; addr += PAGE_SIZE)
    {
        VMM_MapPage(pageDirectory, addr, addr, PAGE_PRESENT | PAGE_RW); // Read-Write permissions
    }

    uint32_t bss_start = ROUND_DOWN_TO_PAGE((uint32_t)__bss_start);
    uint32_t bss_end = ROUND_UP_TO_PAGE((uint32_t)__bss_end);
    for (uint32_t addr = bss_start; addr < bss_end; addr += PAGE_SIZE)
    {
        VMM_MapPage(pageDirectory, addr, addr, PAGE_PRESENT | PAGE_RW); // Read-Write permissions
    }
	uint32_t vga_buffer = 0xB8000;
    VMM_MapPage(pageDirectory, vga_buffer, vga_buffer, PAGE_PRESENT | PAGE_RW);

	for (uint64_t i = 0; i < page_manager.total_pages; ++i)
    {
        uint64_t phys_addr = i * PAGE_SIZE;

        if ((page_manager.page_bitmap[i / 64] & (1ULL << (i % 64))) == 0)
        {
            VMM_MapPage(pageDirectory, phys_addr, phys_addr, PAGE_PRESENT | PAGE_RW);
        }
    }

	enable_paging(pageDirectory);
}

bool VMM_MapPage(PageDirectory* pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t flags)
{
    uint32_t directoryIndex = virtualAddress >> 22;
    uint32_t tableIndex = (virtualAddress >> 12) & 0x03FF;

    // Get the Page Table from the Page Directory
    PageTable* pageTable = pageDirectory->tables[directoryIndex];

    // If the Page Table doesn't exist, allocate a new one
    if (!pageTable)
    {
        pageTable = (PageTable*)alloc_pages(1);
        memset(pageTable, 0, PAGE_SIZE); // Clear the new page table
        pageDirectory->tables[directoryIndex] = pageTable;
    }

    // Set the entry in the Page Table
    pageTable->entries[tableIndex] = (physicalAddress & 0xFFFFF000) | (flags & 0xFFF);

    return true;
}

bool VMM_UnmapPage(PageDirectory* pageDirectory, uint32_t virtualAddress)
{
    uint32_t directoryIndex = virtualAddress >> 22;
    uint32_t tableIndex = (virtualAddress >> 12) & 0x03FF;

    PageTable* pageTable = pageDirectory->tables[directoryIndex];

    if (pageTable)
    {
        // Clear the entry in the Page Table and free the page
        pageTable->entries[tableIndex] = 0;
		free_pages(pageTable, 1);
    }

    return true;
}