#pragma once

#include <stdint.h>
#include <boot/bootparams.h>

extern memregion_t g_MemRegions[];
extern uint32_t g_MemRegionCount;

void Memory_Detect(meminfo_t* structure);