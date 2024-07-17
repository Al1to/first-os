#include "./ahci.h"

uint8_t driver_id;

typedef struct {
    HBAData *abar;
    HBAPort *port;
    void *clb;
    void *fb;
    void *ctba[32];
    void *unused[28];
} ahci_port;

ahci_port *ports;
uint32_t num_ports;

void print_pci_AHCI_data(pci_t pci, uint8_t i, uint8_t j, uint8_t k) {
	// (void)i;
	// (void)j;
	// (void)k;
	// if (pci.vendorID != 0xFFFF && pci.class == 1 && pci.subclass == 6) {
	// 	/*if (k==0) {
	// 	  printf("Detected SATA Host on port %X:%X\n", i,j);
	// 	  dprintf("Detected SATA Host on port %X:%X\n", i,j);
	// 	} else {
	// 	  printf("Detected SATA Host on port %X:%X.%u\n",i,j,k);
	// 	  dprintf("Detected SATA Host on port %X:%X.%u\n",i,j,k);
	// 	}*/
	// 	// identity_map((void *)pci.BAR5);
	// 	// initialize_abar((HBAData *)pci.BAR5);
	// }
	vga_print("Detected AHCI on port i:j\n");
}

void ahci_read_sectors_internal() {} // TODO
void ahci_write_sectors_internal() {} // TODO

void ahci_init() {
    ports = pmm_alloc_page_frame(16);

    pci_register_driver(print_pci_AHCI_data, 1, 6);
    driver_id = register_disk_handler(ahci_read_sectors, ahci_write_sectors, 255);

    vga_print("Initialized AHCI driver"); 
}

uint8_t  ahci_read_sectors(uint16_t drive_num, uint64_t start_sector, uint32_t count, void *buf) {
    if (ports[drive_num].abar != 0) {
        return ahci_read_sectors_internal(ports[drive_num], start_sector & 0xFFFFFFFF, (start_sector >> 32) & 0xFFFFFFFF, count, buf);
    }
    else {
        return 4;
    }
}

uint8_t ahci_write_sectors(uint16_t drive_num, uint64_t start_sector, uint32_t count, void *buf) {
    if (ports[drive_num].abar != 0) {
        return ahci_write_sectors_internal(ports[drive_num], start_sector & 0xFFFFFFFF, (start_sector >> 32) & 0xFFFFFFFF, count, buf);
    }
    else {
        return 4;
    }
}

bool ahci_drive_exists(uint16_t drive_num) {

}

void ahci_print_sector(uint8_t *read) {

}