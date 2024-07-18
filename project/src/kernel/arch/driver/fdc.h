#pragma once
#include <stdint.h>

#define FDC_DOR          0x3F2
#define FDC_MSR          0x3F4
#define FDC_FIFO         0x3F5
#define FDC_CCR          0x3F7

#define FDC_CMD_SENSE_INTERRUPT 0x08
#define FDC_CMD_RECALIBRATE     0x07
#define FDC_CMD_READ            0x46

void init_fdc();
void FDC_calibrate_drive(int drive);
void FDC_send_command(uint8_t cmd);
int FDC_wait_for_ready();
void FDC_read_sector(int drive, int sector, uint8_t *buffer);
void FDC_reset();