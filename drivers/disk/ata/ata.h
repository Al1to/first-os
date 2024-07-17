#include "../../../stdafx.h"

#ifndef ATA_H
#define ATA_H

typedef struct {
    uint16_t dd_dcr;
    uint32_t dd_stLBA;
    uint32_t dd_prtlen;
    uint8_t dd_sbits;
} driver_data;

void ata_init();

bool ata_read(uint32_t n, void* dest, driver_data* dd, uint32_t lba);

#endif