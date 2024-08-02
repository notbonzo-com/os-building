#pragma once

#include <stdint.h>
#include <stddef.h>
#include <boot/bootparams.h>

#define PAGE_SIZE 4096

typedef struct {
    uint64_t* page_bitmap;
    size_t bitmap_size;
    uint64_t total_pages;
} page_manager_t;

void pmm_init(meminfo_t* memInfo);
void* alloc_page();
void free_page(void* page);
