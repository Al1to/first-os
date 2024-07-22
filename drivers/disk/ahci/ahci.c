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

uint32_t find_cmd_slot(ahci_port aport) {
    HBAPort *port = aport.port;
    uint32_t slots = (port->sact | port->ci);
    uint32_t cmd_slots = (aport.abar->cap & 0x0F00) >> 8;

    for (uint32_t i = 0; i < cmd_slots; ++i) {
        if ((slots & 1) == 0) {
            return i;
        }
        slots >>= 1;
    }
    return 0xFFFFFFFF;
}

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

uint8_t ahci_read_sectors_internal(ahci_port aport, uint32_t start_low, uint32_t start_high, uint32_t count, uint8_t *buf) {
    HBAPort *port = aport.port;
    port->is = 0xFFFFFFFF;

    uint32_t slot = find_cmd_slot(aport);
    if (slot == 0xFFFFFFFF) {
        return 1;
    }

    HBACommandHeader *cmd_header = (HBACommandHeader *)aport.clb;
    cmd_header += slot;
    cmd_header-> cfl = sizeof(FIS_HostToDevice) / sizeof(uint32_t);
    cmd_header->w = 0; // Для чтения

    HBACommandTable *cmd_table = (HBACommandTable*)aport.ctba[slot];

    cmd_table->prdt_entry[0].dba = (uint32_t)(intptr_t)get_phys_addr(buf);
    cmd_table->prdt_entry[0].dbau = 0;
    cmd_table->prdt_entry[0].dbc - (count * 512) - 1;
    cmd_table->prdt_entry[0].i = 1;

    FIS_HostToDevice *cmd_fis = (FIS_HostToDevice *)(&cmd_table->cfis);

    cmd_fis->FIS_Type = 0x27; // Host to device
    cmd_fis->c = 1;
    cmd_fis->command = SATA_READ_DMA_EX;

    cmd_fis->lba0 = (uint8_t)(start_low);
    cmd_fis->lba1 = (uint8_t)(start_low >> 8);
    cmd_fis->lba2 = (uint8_t)(start_low >> 16);
    cmd_fis->dev = 1 << 6;

    cmd_fis->lba3 = (uint8_t)(start_low >> 24);
    cmd_fis->lba4 = (uint8_t)(start_high);
    cmd_fis->lba5 = (uint8_t)(start_high >> 8);

    cmd_fis->count_low = (count & 0xFF);
    cmd_fis->count_high = (count >> 8);

    for (uint32_t spin = 0; spin < 1000000; ++spin) {
        if(!(port->tfd & (SATA_BUSY | SATA_DRQ))) {
            break;
        }
    }
    if((port->tfd & (SATA_BUSY | SATA_DRQ))) {
        return 2;
    }

    port->ci = (1 << slot);

    while (true) {
        if (!(port->ci & (1 << slot))) {
            break;
        }
        if (port->is & (1 << 30)) {
            return 3;
        }
    }
    if (port->is & (1 << 30)) { // <--------------------------------- я хз зачем тут повторение этого блока условия, но ладно
        return 3;
    }

    return 0;
}

uint8_t ahci_write_sectors_internal(ahci_port aport, uint32_t start_low, uint32_t start_high, uint32_t count, uint8_t *buf) {
    HBAPort *port = aport.port;
    port->is = 0xFFFFFFFF;
    
    uint32_t slot = find_cmdslot(aport);
    if (slot == 0xFFFFFFFF) {
        return 1;
    }

    HBACommandHeader *cmd_header = (HBACommandHeader*)aport.clb;
    cmd_header += slot;
    cmd_header-> cfl = sizeof(FIS_HostToDevice) / sizeof(uint32_t);
    cmd_header->w = 1; // Для записи =)

    HBACommandTable *cmd_table = (HBACommandTable*)aport.ctba[slot];

    cmd_table->prdt_entry[0].dba = (uint32_t)(intptr_t)get_phys_addr(buf);
    cmd_table->prdt_entry[0].dbau = 0;
    cmd_table->prdt_entry[0].dbc - (count * 512) - 1;
    cmd_table->prdt_entry[0].i = 1;

    FIS_HostToDevice *cmd_fis = (FIS_HostToDevice *)(&cmd_table->cfis);

    cmd_fis->FIS_Type = 0x27; // Host to device
    cmd_fis->c = 1;
    cmd_fis->command = SATA_WRITE_DMA_EX;
    
    cmd_fis->lba0 = (uint8_t)(start_low);
    cmd_fis->lba1 = (uint8_t)(start_low >> 8);
    cmd_fis->lba2 = (uint8_t)(start_low >> 16);
    cmd_fis->dev = 1 << 6;

    cmd_fis->lba3 = (uint8_t)(start_low >> 24);
    cmd_fis->lba4 = (uint8_t)(start_high);
    cmd_fis->lba5 = (uint8_t)(start_high >> 8);

    cmd_fis->count_low = (count & 0xFF);
    cmd_fis->count_high = (count >> 8);

    for (uint32_t spin = 0; spin < 1000000; ++spin) {
        if(!(port->tfd & (SATA_BUSY | SATA_DRQ))) {
            break;
        }
    }
    if((port->tfd & (SATA_BUSY | SATA_DRQ))) {
        return 2;
    }

    port->ci = (1 << slot);

    while (true) {
        if (!(port->ci & (1 << slot))) {
            break;
        }
        if (port->is & (1 << 30)) {
            return 3;
        }
    }
    if (port->is & (1 << 30)) { // <--------------------------------- я хз зачем тут повторение этого блока условия, но ладно
        return 3;
    }

    return 0;
}

void ahci_init() {
    ports = pmm_alloc_page_frame(16);

    pci_register_driver(print_pci_AHCI_data, 1, 6);
    driver_id = register_disk_handler(ahci_read_sectors, ahci_write_sectors, 255);

    vga_print("Initialized AHCI driver"); 
}

uint8_t  ahci_read_sectors(uint16_t drive_num, uint64_t start_sector, uint32_t count, void *buf) { // вот тут
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