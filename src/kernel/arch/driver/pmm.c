#include "pmm.h"
#include <stdbool.h>
#include <debug.h>
#include <memory.h>

page_manager_t page_manager = {0};

void pmm_init(meminfo_t* memInfo) {
    uint64_t total_memory = 0;
    for (uint32_t i = 0; i < memInfo->region_count; i++) {
        if (memInfo->regions[i].type == USABLE) {
            total_memory += memInfo->regions[i].length;
        }
    }
    page_manager.total_pages = total_memory / PAGE_SIZE;

    page_manager.bitmap_size = (page_manager.total_pages + 63) / 64;
    size_t bitmap_pages = (page_manager.bitmap_size * sizeof(uint64_t) + PAGE_SIZE - 1) / PAGE_SIZE;

    uint64_t bitmap_start_page = 0;
    bool found_space = false;
    for (uint32_t i = 0; i < memInfo->region_count && !found_space; i++) {
        if (memInfo->regions[i].type == USABLE) {
            uint64_t region_start_page = memInfo->regions[i].begin / PAGE_SIZE;
            uint64_t region_end_page = (memInfo->regions[i].begin + memInfo->regions[i].length) / PAGE_SIZE;
            if (region_end_page - region_start_page >= bitmap_pages) {
                bitmap_start_page = region_start_page;
                found_space = true;
            }
        }
    }
    if (!found_space) {
        debugf("Not enough space for bitmap\n");
        return;
    }
    debugf("Bitmap starts at page %llu\n", bitmap_start_page);
    debugf("Bitmap ends at page %llu\n", bitmap_start_page + bitmap_pages - 1);

    page_manager.page_bitmap = (uint64_t*)(bitmap_start_page * PAGE_SIZE);
    memset(page_manager.page_bitmap, 0xFF, bitmap_pages * PAGE_SIZE);
    for (uint64_t i = 0; i < bitmap_pages; i++) {
        page_manager.page_bitmap[(bitmap_start_page + i) / 64] |= (1ULL << ((bitmap_start_page + i) % 64));
    }

    for (uint32_t i = 0; i < memInfo->region_count; i++) {
        if (memInfo->regions[i].type == USABLE) {
            uint64_t region_start_page = memInfo->regions[i].begin / PAGE_SIZE;
            uint64_t region_end_page = (memInfo->regions[i].begin + memInfo->regions[i].length) / PAGE_SIZE;
            for (uint64_t j = region_start_page; j < region_end_page; j++) {
                page_manager.page_bitmap[j / 64] &= ~(1ULL << (j % 64));
            }
        }
    }

    for (uint64_t i = 0; i < bitmap_pages; i++) {
        page_manager.page_bitmap[(bitmap_start_page + i) / 64] |= (1ULL << ((bitmap_start_page + i) % 64));
    }
}

bool is_page_free(uint64_t page_index) {
    return !(page_manager.page_bitmap[page_index / 64] & (1ULL << (page_index % 64)));
}

void mark_page_as_used(uint64_t page_index) {
    page_manager.page_bitmap[page_index / 64] |= (1ULL << (page_index % 64));
}

void mark_page_as_free(uint64_t page_index) {
    page_manager.page_bitmap[page_index / 64] &= ~(1ULL << (page_index % 64));
}

void* alloc_page() {
    for (uint64_t i = 0; i < page_manager.total_pages; i++) {
        if (is_page_free(i)) {
            mark_page_as_used(i);
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void free_page(void* page) {
    uint64_t page_index = (uint64_t)page / PAGE_SIZE;
    mark_page_as_free(page_index);
}
