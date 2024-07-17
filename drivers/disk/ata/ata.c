#include "./ata.h"

void ata_init() {}

bool ata_read(uint32_t n, void* dest, driver_data* dd, uint32_t lba) {
    if (n < 0) ata_reset(dd->dd_dcr);
    if ((n > 0x3fffff) || (dd->dd_prtlen < n) || (dd->dd_prtlen < lba) || (lba > dd->dd_prtlen - n + 1)) return false;
    /* FIXME: */ if (in_port_b(dd->dd_dcr) & 0x88) ata_reset(dd->dd_dcr);
    if (lba > 28 || dd->dd_stLBA > 28 || (dd->dd_stLBA+lba) > 28 || (dd->dd_stLBA+lba+n) > 28) ata_pio48_read();
    else ata_pio28_read();
}

void ata_pio48_read(/*uint32_t n, void* dest, driver_data* dd*/) {
    
}

void ata_pio28_read() {
    
}

void ata_reset(uint16_t dcr) {
    out_port_b(dcr, 4);
    out_port_b(dcr, 0);
    in_port_b(dcr);
    in_port_b(dcr);
    in_port_b(dcr);
    char in = in_port_b(dcr);
    do {
        in = in_port_b(dcr);
        in &= 0xc0;
    } while (in != 0x40);
}