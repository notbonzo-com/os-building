#include <memdetect.h>
#include <x86.h>
#include <stdio.h>

#define MAX_REGIONS 256

memregion_t g_MemRegions[MAX_REGIONS];
uint32_t g_MemRegionCount = 0;

void Memory_Detect(meminfo_t* structure)
{
	E820MemoryBlock block;
	uint32_t id = 0;
	int ret;

	ret = x86_E820GetNextBlock(&block, &id);

	while (ret > 0 && id != 0) {
		g_MemRegions[g_MemRegionCount].begin = block.base;
		g_MemRegions[g_MemRegionCount].length = block.length;
		g_MemRegions[g_MemRegionCount].type = block.type;
		g_MemRegions[g_MemRegionCount].ACPI = block.acpi;
		g_MemRegionCount++;
		
		printf("Region %d: 0x%llx-0x%llx: %u %u\n", g_MemRegionCount, block.base, block.base + block.length, block.type, block.acpi);

		ret = x86_E820GetNextBlock(&block, &id);
	}

	structure->region_count = g_MemRegionCount;
	structure->regions = g_MemRegions;
}