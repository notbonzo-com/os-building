#pragma once

#include <stdint.h>
#include <stddef.h>
#include <boot/bootparams.h>

#define PAGE_SIZE 4096

#define SET_BIT(bitmap, bit) (bitmap[(bit) / 32] |= (1U << ((bit) % 32)))
#define UNSET_BIT(bitmap, bit) (bitmap[(bit) / 32] &= ~(1U << ((bit) % 32)))
#define IS_BIT_SET(bitmap, bit) (bitmap[(bit) / 32] & (1U << ((bit) % 32)))

typedef struct {
    uintptr_t base;
} UsablePage;

void pmm_init(meminfo_t memInfo);
void* pmm_claim(size_t num_pages);
void pmm_free(void* addr, size_t num_pages);