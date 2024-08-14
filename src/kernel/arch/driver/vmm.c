#include <arch/driver/pmm.h>
#include <arch/driver/vmm.h>
#include <memory.h>

#define RECURSIVE_MAPPING_INDEX 1023

PageDirectory* kernel_page_directory;

static inline uint32_t get_page_index(uint32_t addr) {
    return addr / PAGE_SIZE;
}

static inline bool is_page_present(page_table_entry* entry) {
    return entry->present == 1;
}

static inline void set_page_entry(page_table_entry* entry, uint32_t addr, uint32_t flags) {
    entry->present = (flags & PAGE_PRESENT) ? 1 : 0;
    entry->readwrite = (flags & PAGE_RW) ? 1 : 0;
    entry->user = (flags & PAGE_USER) ? 1 : 0;
    entry->address = addr >> 12;
}

void VMM_Initialize(PageDirectory* pageDirectory) {
    // Identity map the first 4 MiB of memory
    pageDirectory = alloc_pages(1);
    debugf("Page Directory Adress 0x%p\n", pageDirectory);
    memset(pageDirectory, 0, PAGE_SIZE);
    VMM_MapPage(pageDirectory, 0x00000000, PAGE_SIZE * 1024, 0x00000000, PAGE_PRESENT | PAGE_RW);

    debugf(
        "Kernel Adresses: text 0x%p - 0x%p, rodata 0x%p - 0x%p, data 0x%p - 0x%p, bss 0x%p - 0x%p\n",
        __text_start, __text_end, __rodata_start, __rodata_end, __data_start, __data_end, __bss_start, __bss_end
    );

    VMM_MapPage(pageDirectory, (uint32_t)__text_start, __text_end - __text_start, (uint32_t)__text_start, PAGE_PRESENT);
    VMM_MapPage(pageDirectory, (uint32_t)__rodata_start, __rodata_end - __rodata_start, (uint32_t)__rodata_start, PAGE_PRESENT);
    VMM_MapPage(pageDirectory, (uint32_t)__data_start, __data_end - __data_start, (uint32_t)__data_start, PAGE_PRESENT | PAGE_RW);
    VMM_MapPage(pageDirectory, (uint32_t)__bss_start, __bss_end - __bss_start, (uint32_t)__bss_start, PAGE_PRESENT | PAGE_RW);

    pageDirectory->entries[RECURSIVE_MAPPING_INDEX].address = (uint32_t)pageDirectory >> 12;
    pageDirectory->entries[RECURSIVE_MAPPING_INDEX].present = 1;
    pageDirectory->entries[RECURSIVE_MAPPING_INDEX].readwrite = 1;
    pageDirectory->entries[RECURSIVE_MAPPING_INDEX].user = 0;


    __asm__ volatile(
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        : :"r"(pageDirectory)
        : "memory"
    );
}

bool VMM_MapPage(PageDirectory* pageDirectory, uint32_t virtualAddress, size_t size, uint32_t physicalAddress, uint32_t flags) {
    uint32_t vaddr = ROUND_DOWN_TO_PAGE(virtualAddress);
    uint32_t paddr = ROUND_DOWN_TO_PAGE(physicalAddress);
    size = ROUND_UP_TO_PAGE(size);

    while (size > 0) {
        uint32_t pageDirIndex = vaddr >> 22;
        uint32_t pageTableIndex = (vaddr >> 12) & 0x03FF;

        page_dir_entry* pageDirEntry = &pageDirectory->entries[pageDirIndex];

        if (!is_page_present(pageDirEntry)) {
            PageTable* newPageTable = alloc_pages(1);
            if (!newPageTable) return false;

            set_page_entry((page_table_entry*)pageDirEntry, (uint32_t)newPageTable, PAGE_PRESENT | PAGE_RW | PAGE_USER);
        }

        PageTable* pageTable = (PageTable*)(pageDirEntry->address << 12);
        set_page_entry(&pageTable->entries[pageTableIndex], paddr, flags);

        vaddr += PAGE_SIZE;
        paddr += PAGE_SIZE;
        size -= PAGE_SIZE;
    }

    __asm__ volatile(
        "invlpg (%0)" : :"r"(virtualAddress) : "memory"
    );
    return true;
}

bool VMM_UnmapPage(PageDirectory* pageDirectory, uint32_t virtualAddress) {
    uint32_t vaddr = ROUND_DOWN_TO_PAGE(virtualAddress);
    uint32_t pageDirIndex = vaddr >> 22;
    uint32_t pageTableIndex = (vaddr >> 12) & 0x03FF;

    page_dir_entry* pageDirEntry = &pageDirectory->entries[pageDirIndex];

    if (!is_page_present(pageDirEntry)) return false;

    PageTable* pageTable = (PageTable*)(pageDirEntry->address << 12);
    page_table_entry* pageEntry = &pageTable->entries[pageTableIndex];

    if (!is_page_present(pageEntry)) return false;

    free_pages((void*)(pageEntry->address << 12), 1);
    pageEntry->present = 0;
    __asm__ volatile(
        "invlpg (%0)" : :"r"(virtualAddress) : "memory"
    );

    return true;
}