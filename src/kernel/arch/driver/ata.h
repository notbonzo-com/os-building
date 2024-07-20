#pragma once

#include <stdint.h>
#include <stdbool.h>

void ata_init();
void read_sectors_ATA_PIO(uint8_t *target_address, uint32_t LBA, uint8_t sector_count);
void write_sectors_ATA_PIO(uint32_t LBA, uint8_t sector_count, uint8_t *rawBytes);
