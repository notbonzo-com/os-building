#include "pmm.h"

static uint32_t* bitmap;
static size_t total_pages;
static size_t usable_pages;
static size_t bitmap_size;

static size_t get_page_count(uintptr_t length) {
    return (length + PAGE_SIZE - 1) / PAGE_SIZE;
}

void pmm_init(meminfo_t memInfo) {
    total_pages = 0;
    usable_pages = 0;
    size_t usable_page_count = 0;
    for (uint32_t i = 0; i < memInfo.region_count; i++) {
        memregion_t region = memInfo.regions[i];
        if (region.type == USABLE) {
            usable_pages += get_page_count(region.length);
        }
        total_pages += get_page_count(region.length);
    }

    UsablePage UsablePages[usable_pages];

    size_t page_index = 0;
    for (uint32_t i = 0; i < memInfo.region_count; i++) {
        memregion_t region = memInfo.regions[i];
        size_t pages = get_page_count(region.length);
        if (region.type == USABLE) {
            for (size_t j = 0; j < pages; j++) {
                UsablePages[usable_page_count++].base = region.begin + j * PAGE_SIZE;
            }
        }
        page_index += pages;
    }

    bitmap_size = (usable_pages + 31) / 32;

    // Find space for the bitmap within usable pages
    for (size_t i = 0; i < usable_page_count; i++) {
        size_t free_pages = 0;
        for (size_t j = i; j < usable_page_count; j++) {
            if (UsablePages[j].base == UsablePages[i].base + (j - i) * PAGE_SIZE) {
                free_pages++;
                if (free_pages * PAGE_SIZE >= bitmap_size * sizeof(uint32_t)) {
                    bitmap = (uint32_t*)UsablePages[i].base;
                    for (size_t k = 0; k < bitmap_size; k++) {
                        bitmap[k] = 0xFFFFFFFF;
                    }
                    i = usable_page_count;
                    break;
                }
            } else {
                break;
            }
        }
    }

    page_index = 0;
    for (uint32_t i = 0; i < memInfo.region_count; i++) {
        memregion_t region = memInfo.regions[i];
        size_t pages = get_page_count(region.length);
        if (region.type != USABLE) {
            for (size_t j = 0; j < pages; j++) {
                SET_BIT(bitmap, page_index + j);
            }
        }
        page_index += pages;
    }
}

void* pmm_claim(size_t num_pages) {
    size_t consecutive_pages = 0;
    size_t start_page = 0;

    for (size_t i = 0; i < usable_pages; i++) {
        if (!IS_BIT_SET(bitmap, i)) {
            if (consecutive_pages == 0) {
                start_page = i;
            }
            consecutive_pages++;
            if (consecutive_pages == num_pages) {
                for (size_t j = 0; j < num_pages; j++) {
                    SET_BIT(bitmap, start_page + j);
                }
                return (void*)(start_page * PAGE_SIZE);
            }
        } else {
            consecutive_pages = 0;
        }
    }

    return NULL;
}

void pmm_free(void* addr, size_t num_pages) {
    size_t start_page = ((uintptr_t)addr) / PAGE_SIZE;
    for (size_t i = 0; i < num_pages; i++) {
        UNSET_BIT(bitmap, start_page + i);
    }
}