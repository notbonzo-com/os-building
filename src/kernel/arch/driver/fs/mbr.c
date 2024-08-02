#include <arch/driver/fs/mbr.h>
#include <arch/driver/ata.h>
#include <arch/driver/pmm.h>

MBR_t g_MBRS[4];
uint8_t g_BootMBR;

void MBR_Initialize()
{
	g_BootMBR = 0;
	MBRHeader_t mbr;
    read_sectors_ATA_PIO((uint8_t*)&mbr, 0, 1);
	for (int i = 0; i < 4; i++) {
		g_MBRS[i].partitionOffset = mbr.partitions[i].start_sector_lba;
		g_MBRS[i].partitionSize = mbr.partitions[i].num_sectors;
	}
}

bool MBR_ReadSectors(MBR_t* disk, uint32_t lba, uint8_t sectors, uint8_t* lowerDataOut)
{
	if (lba+disk->partitionOffset >=disk->partitionSize) {
		return false;
	}
	read_sectors_ATA_PIO(lowerDataOut, lba+disk->partitionOffset, sectors);
	return true;
}