#pragma once
#include <stdint.h>

#define MBR_SIGNATURE 0xAA55

typedef struct {
    uint8_t boot_indicator;  // Boot indicator
    uint8_t start_head;      // Starting head
    uint8_t start_sector;    // Starting sector
    uint8_t start_cylinder;  // Starting cylinder
    uint8_t partition_type;  // Partition type
    uint8_t end_head;        // Ending head
    uint8_t end_sector;      // Ending sector
    uint8_t end_cylinder;    // Ending cylinder
    uint32_t start_sector_lba; // Starting sector (LBA)
    uint32_t num_sectors;    // Number of sectors in partition
} __attribute__((packed)) PartitionEntry_t;

typedef struct {
    uint8_t boot_code[446];        // Boot code
    PartitionEntry_t partitions[4];// Partition table entries
    uint16_t signature;            // Signature (0xAA55)
} __attribute__((packed)) MBRHeader_t;

typedef struct {
	uint32_t partitionOffset;
    uint32_t partitionSize;
} MBR_t;

extern MBR_t g_MBRS[4];
extern uint8_t g_BootMBR;

void MBR_Initialize();
bool MBR_ReadSectors(MBR_t* disk, uint32_t lba, uint8_t sectors, uint8_t* lowerDataOut);