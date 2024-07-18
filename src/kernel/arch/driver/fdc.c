#include "fdc.h"
#include <arch/i686/io.h>
#include <arch/i686/irq.h>
#include <arch/i686/pic.h>
#include <debug.h>
#include <stdio.h>

void fdc_irq_handler(Registers* regs);

void FDC_send_command(uint8_t cmd) {
    while ((i686_inb(FDC_MSR) & 0xC0) != 0x80);
    i686_outb(FDC_FIFO, cmd);
}

int FDC_wait_for_ready() {
    for (int timeout = 0; timeout < 100000; timeout++) {
        if ((i686_inb(FDC_MSR) & 0x80) == 0x80) {
            return 1;
        }
    }
    log_warn("FDC", "FDC_wait_for_ready: Timeout waiting for FDC");
    return 0;
}

void FDC_reset() {
    i686_outb(FDC_DOR, 0x00);
    for (volatile int i = 0; i < 100; i++);
    i686_outb(FDC_DOR, 0x0C);

    if (!FDC_wait_for_ready()) {
        log_err("FDC", "Reset failed: FDC not ready");
        return;
    }

    for (int i = 0; i < 4; i++) {
        FDC_send_command(FDC_CMD_SENSE_INTERRUPT);
        uint8_t st0 = i686_inb(FDC_FIFO);
        uint8_t cyl = i686_inb(FDC_FIFO);
        log_debug("FDC", "SENSE_INTERRUPT st0: %02x, cyl: %02x", st0, cyl);
    }

    i686_outb(FDC_CCR, 0x00);
}

void init_fdc() {
    log_info("FDC", "Initializing FDC");
    i686_IRQ_RegisterHandler(6, fdc_irq_handler);
    FDC_reset();
    FDC_calibrate_drive(0);
    FDC_calibrate_drive(1);
}

void FDC_calibrate_drive(int drive) {
    i686_outb(FDC_DOR, 0x1C | drive);
    FDC_send_command(FDC_CMD_RECALIBRATE);
    FDC_send_command(drive);
    if (!FDC_wait_for_ready()) {
        printf("FDC", "FDC_calibrate_drive failed: FDC not ready");
        return;
    }
    i686_outb(FDC_DOR, 0x0C);

    FDC_send_command(FDC_CMD_SENSE_INTERRUPT);
    i686_inb(FDC_FIFO);
    i686_inb(FDC_FIFO);
}

void FDC_read_sector(int drive, int sector, uint8_t *buffer) {
    i686_outb(FDC_FIFO, 0x06);
    i686_outb(FDC_FIFO, (sector & 0xFF) | (drive << 8));

    i686_outb(FDC_FIFO, FDC_CMD_READ);

    if (!FDC_wait_for_ready()) {
        log_debug("FDC", "FDC_read_sector failed: FDC not ready");
        return;
    }

    for (int i = 0; i < 512; i++) {
        buffer[i] = i686_inb(FDC_FIFO);
    }
}

void fdc_irq_handler(Registers*) {
    log_debug("FDC", "IRQ 6 Occured!");
    i686_PIC_SendEndOfInterrupt(6);
}