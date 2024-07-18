#pragma once

#include <stdint.h>

typedef struct {
	uint64_t begin, length;
	uint32_t type, ACPI;
} __attribute__((packed)) memregion_t;

typedef struct {
	uint32_t region_count;
	memregion_t* regions;
} __attribute__((packed)) meminfo_t;

typedef struct {
	uint16_t bootDrive;
	uint16_t reserved0;
	meminfo_t memInfo;
} __attribute__((packed)) bootparams_t;