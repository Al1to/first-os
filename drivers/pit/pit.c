#include "./pit.h"

void print_pci_pit_data(pci_t pci, uint8_t i, uint8_t j, uint8_t k) {
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
	vga_print("Detected PIT on port i:j (printf hernya)\n");
}

uint64_t ticks;
const uint32_t freq = 100;

void on_irq0(struct int_regs *regs) {
    ticks += 1;
}

void pit_init() {
    pci_register_driver(print_pci_pit_data, 8, 2);
    ticks = 0;
    irq_install_handler(0, &on_irq0);

    uint32_t divisor = 1193180 / freq;

    out_port_b(0x43, 0x36);
    out_port_b(0x40, (uint8_t)(divisor & 0xFF));
    out_port_b(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}
