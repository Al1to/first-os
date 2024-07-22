#include "./pci.h"

typedef struct {
	uint8_t class;
	uint8_t subclass;
	const char *name; // вот тут структура
} pci_classname;      // не знаю что про нее говорить, ну вот да она такая да

pci_classname classnames[] = {
	{0x00, 0x00, "Non-VGA-Compatible Unclassified Device"},
	{0x00, 0x01, "VGA-Compatible Unclassified Device"},
	{0x01, 0x00, "SCSI Bus Controller"},
	{0x01, 0x01, "IDE Controller"},
	{0x01, 0x02, "Floppy Disk Controller"},
	{0x01, 0x03, "IPI Bus Controller"},
	{0x01, 0x04, "RAID Controller"},
	{0x01, 0x05, "ATA Controller"},
	{0x01, 0x06, "Serial ATA Controller"},  // TODO: SATA drv reg to pci
	{0x01, 0x07, "Serial Attached SCSI Controller"},
	{0x01, 0x08, "Non-Volatile Memory Controller"},
	{0x01, 0x80, "Other Mass Storage Controller"},
	{0x02, 0x00, "Ethernet Controller"},
	{0x02, 0x01, "Token Ring Controller"},
	{0x02, 0x02, "FDDI Controller"},
	{0x02, 0x03, "ATM Controller"},
	{0x02, 0x04, "ISDN Controller"},
	{0x02, 0x05, "WorldFip Controller"},
	{0x02, 0x06, "PICMG 2.14 Multi Computing Controller"},
	{0x02, 0x07, "Infiniband Controller"},
	{0x02, 0x08, "Fabric Controller"},
	{0x02, 0x80, "Other Network Controller"},
	{0x03, 0x00, "VGA Compatible Controller"},
	{0x03, 0x01, "XGA Controller"},
	{0x03, 0x02, "3D Controller (Not VGA-Compatible)"},
	{0x03, 0x80, "Other Display Controller"},
	{0x04, 0x00, "Multimedia Video Controller"},
	{0x04, 0x01, "Multimedia Audio Controller"},
	{0x04, 0x02, "Computer Telephony Device"},
	{0x04, 0x03, "Audio Device"},
	{0x04, 0x80, "Other Multimedia Controller"},
	{0x05, 0x00, "RAM Controller"},
	{0x05, 0x01, "Flash Controller"},
	{0x05, 0x80, "Other Memory Controller"},
	{0x06, 0x00, "Host Bridge"},
	{0x06, 0x01, "ISA Bridge"},
	{0x06, 0x02, "EISA Bridge"},
	{0x06, 0x03, "MCA Bridge"},
	{0x06, 0x04, "PCI-to-PCI Brige"},
	{0x06, 0x05, "PCMCIA Bridge"},
	{0x06, 0x06, "NuBus Bridge"},
	{0x06, 0x07, "CardBus Bridge"},
	{0x06, 0x08, "RACEway Bridge"},
	{0x06, 0x09, "PCI-to-PCI Bridge"},
	{0x06, 0x0A, "Infiniband-to-PCI Host Bridge"},
	{0x06, 0x80, "Other Bridge"},
	{0x07, 0x00, "Serial Controller"},
	{0x07, 0x01, "Parallel Controller"},
	{0x07, 0x02, "Multiport Serial Controller"},
	{0x07, 0x03, "Modem"},
	{0x07, 0x04, "IEEE 488.1/2 (GPIB) Controller"},
	{0x07, 0x05, "Smart Card Controller"},
	{0x07, 0x80, "Other Simple Communication Controller"},
	{0x08, 0x00, "PIC"},
	{0x08, 0x01, "DMA Controller"},
	{0x08, 0x02, "Timer"},
	{0x08, 0x03, "RTC Controller"},
	{0x08, 0x04, "PCI Hot-Plug Controller"},
	{0x08, 0x05, "SD Host Controller"},
	{0x08, 0x07, "IOMMU"},
	{0x08, 0x80, "Other Base System Peripheral"},
	{0x09, 0x00, "Keyboard Controller"},  // TODO: Keyboard drv reg to pci
	{0x09, 0x01, "Digitizer Pen"},
	{0x09, 0x02, "Mouse Controller"},
	{0x09, 0x03, "Scanner Controller"},
	{0x09, 0x04, "Gameport Controller"},
	{0x09, 0x80, "Other Input Device Controller"},
	{0x0A, 0x00, "Generic Docking Station"},
	{0x0A, 0x80, "Other Docking Station"},
	{0x0B, 0x00, "386 Processor"},
	{0x0B, 0x01, "486 Processor"},
	{0x0B, 0x02, "Pentium Processor"},
	{0x0B, 0x03, "Pentioum Pro Processor"},
	{0x0B, 0x10, "Alpha Processor"},
	{0x0B, 0x20, "PowerPC Processor"},
	{0x0B, 0x30, "MIPS Processor"},
	{0x0B, 0x40, "Co-Processor"},
	{0x0B, 0x80, "Other Processor"},
	{0x0C, 0x00, "FireWire (IEEE 1394) Controller"},
	{0x0C, 0x01, "ACCESS Bus Controller"},
	{0x0C, 0x02, "SSA"},
	{0x0C, 0x03, "USB Controller"},
	{0x0C, 0x04, "Fibre Channel"},
	{0x0C, 0x05, "SMBus Controller"},
	{0x0C, 0x06, "InfiniBand Controller"},
	{0x0C, 0x07, "IPMI Interface"},
	{0x0C, 0x08, "SERCOS Interface (IEC 61491)"},
	{0x0C, 0x09, "CANbus Controller"},
	{0x0C, 0x80, "Other Serial Bus Controller"},
	{0x0D, 0x00, "iRDA Compatible Controller"},
	{0x0D, 0x00, "Consumer IR Controller"},
	{0x0D, 0x00, "RF Controller"},
	{0x0D, 0x00, "Bluetooth Controller"},
	{0x0D, 0x00, "Broadband Controller"},
	{0x0D, 0x00, "Ethernet Controller (802.1a)"},
	{0x0D, 0x00, "Ethernet Controller (802.1b)"},
	{0x0D, 0x00, "Other Wireless Controller"},
	{0x0E, 0x00, "I20"},
	{0x0F, 0x01, "Satellite TV Controller"},
	{0x0F, 0x02, "Satellite Audio Controller"},
	{0x0F, 0x03, "Satellite Voice Controller"},
	{0x0F, 0x04, "Satellite Data Controller"},
	{0x10, 0x00, "Network and Computing Encryption/Decryption"},
	{0x10, 0x10, "Entertainment Encryption/Decryption"},
	{0x10, 0x80, "Other Encryption Controller"},
	{0x11, 0x00, "DPIO Modules"},
	{0x11, 0x01, "Performance Counters"},
	{0x11, 0x10, "Communication Synchronizer"},
	{0x11, 0x20, "Signal Processing Management"},
	{0x11, 0x80, "Other Signal Processing Controller"},
}; 

// все что выше, это всякая дичь на которую в целом пофиг


// эта функа просто ищет в этом массиве нужный драйвер, ща чуть выше
const char *pci_find_name(uint8_t class, uint8_t subclass) { 
	switch(class) {
		case 0x12:
			return "Processing Accelerator";
		case 0x13:
			return "Non-Essential Instrumentation";
		case 0x40:
			return "Co-Processor";
		case 0xFF:
			return "Vendor Specific";
		default:
			for (size_t i = 0; i < sizeof(classnames)/sizeof(pci_classname); i++) {
				if (classnames[i].class == class && classnames[i].subclass == subclass) {
					return classnames[i].name;
				}
			}
			return "Unknown PCI Device";
	}
}

// уже более полезная вещь, в не ну думаю очевидно что хранится (pci_driver)
typedef struct {
	bool used;   // самое интересное поле
	uint8_t class;
	uint8_t subclass;
	void (*pci_function)(pci_t, uint8_t, uint8_t, uint8_t); // самое непонятное поле, да я тоже в афиге что так можно было
} pci_driver;

// массив крч 255 драйверов ну понятно
pci_driver pci_drivers[255];

// тоже думаю логика понятна, чекает фулл массив этих pci_driver, пока не найдет used false
bool pci_register_driver(void (*pci_function)(pci_t, uint8_t, uint8_t, uint8_t), uint8_t class, uint8_t subclass) {
    for (uint16_t i = 0; i < 255; ++i) {
        if (!pci_drivers[i].used) {
            pci_drivers[i].used = true;
            pci_drivers[i].class = class;
            pci_drivers[i].subclass = subclass;
            pci_drivers[i].pci_function = pci_function;
        }
    }
}

// по идее просто вызывает функ pci_function, ну если этот драйвер есть, я думаю ей надо нврн функцию init драйвера передавать или чето такое
void launch_driver(pci_t pci, uint8_t bus, uint8_t device, uint8_t function) {
	for (uint16_t i = 0; i < 255; i++) {
		if (pci_drivers[i].used && pci_drivers[i].class == pci.class && pci_drivers[i].subclass == pci.subclass) {
            // vga_printf("%x.", bus); vga_printf("%x.", device); vga_printf("%x: ", function); боль
            vga_print("Launching the driver for ");
            vga_print(pci_find_name(pci.class, pci.subclass));
            vga_print("\n");
			pci_drivers[i].pci_function(pci, bus, device, function);
			return;
		}
	}
    // vga_printf("%x.", bus); vga_printf("%x.", device); vga_printf("%x: ", function); боль
    vga_print("No driver registered for ");
    vga_print(pci_find_name(pci.class, pci.subclass));
    vga_print("\n");
}

// ее я в кернеле вызываю, по идее в ней ничего сложного, она скорее муторная
// на том скрине как раз ее плоды
void pci_scan() {
	vga_print("Scanning PCI bus\n");
	pci_t c_pci;
	for (uint16_t i = 0; i < 256; i++) {
		c_pci = pci_get_data(i, 0, 0);
		if (c_pci.vendorID != 0xFFFF) {
			for (uint8_t j = 0; j < 32; j++) {
				c_pci = pci_get_data(i, j, 0);
				if (c_pci.vendorID != 0xFFFF) {
					launch_driver(c_pci, i, j, 0);
					for (uint8_t k = 1; k < 8; k++) {
						pci_t pci = pci_get_data(i, j, k);
						if (pci.vendorID != 0xFFFF) {
							launch_driver(pci, i, j, k);
						}
					}
				}
			}
		}
	}
	vga_print("Finished scanning PCI bus\n");
} 
	
// Функция для чтения 32-битного значения из конфигурационного пространства PCI устройства
uint32_t pci_read_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset) { 
    // Формируем 32-битный адрес для доступа к конфигурационному пространству
    // 
    //   - 1 << 31: бит 31 устанавливается в 1, чтобы указать на конфигурационное пространство PCI
    //   - bus << 16: номер шины PCI (8 бит) сдвигается на 16 бит влево
    //   - num << 11: номер устройства на шине (8 бит) сдвигается на 11 бит влево
    //   - function << 8: номер функции устройства (8 бит) сдвигается на 8 бит влево
    //   - offset: смещение внутри конфигурационного пространства (8 бит)
    uint32_t addr = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset);
  
    // Записываем адрес в порт ввода-вывода 0xCF8
    // Порт 0xCF8 используется для записи адреса конфигурационного пространства PCI
    out_port_l(0xCF8, addr);

    // Читаем 32-битное значение из порта ввода-вывода 0xCFC
    // Порт 0xCFC используется для чтения данных из конфигурационного пространства PCI
    return in_port_l(0xCFC);
}

// Функция для чтения 16-битного значения из конфигурационного пространства PCI устройства
uint16_t pci_read_config_word(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset) {
    // Формируем 32-битный адрес для доступа к конфигурационному пространству
    // 
    //   - 1 << 31: бит 31 устанавливается в 1, чтобы указать на конфигурационное пространство PCI
    //   - bus << 16: номер шины PCI (8 бит) сдвигается на 16 бит влево
    //   - num << 11: номер устройства на шине (8 бит) сдвигается на 11 бит влево
    //   - function << 8: номер функции устройства (8 бит) сдвигается на 8 бит влево
    //   - offset & 0xfc: смещение внутри конфигурационного пространства выравнивается по 2 байтам
    uint32_t addr = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset & 0xfc);
  
    // Записываем адрес в порт ввода-вывода 0xCF8
    // Порт 0xCF8 используется для записи адреса конфигурационного пространства PCI
    out_port_l(0xCF8, addr);

    // Читаем 32-битное значение из порта ввода-вывода 0xCFC
    // Порт 0xCFC используется для чтения данных из конфигурационного пространства PCI
    // 
    //   - in_port_l(0xCFC) >> ((offset & 2) * 8): сдвиг значения на соответствующее количество битов вправо
    //       - (offset & 2) * 8: вычисляется смещение для чтения 16-битного значения
    //   - & 0xffff: маскируем результат, чтобы получить 16-битное значение
    return (uint16_t)((in_port_l(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

// Функция для записи 32-битного значения в конфигурационное пространство PCI устройства
void pci_write_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint32_t value) {
    // Формируем 32-битный адрес для доступа к конфигурационному пространству
    // 
    //   - 1 << 31: бит 31 устанавливается в 1, чтобы указать на конфигурационное пространство PCI
    //   - bus << 16: номер шины PCI (8 бит) сдвигается на 16 бит влево
    //   - num << 11: номер устройства на шине (8 бит) сдвигается на 11 бит влево
    //   - function << 8: номер функции устройства (8 бит) сдвигается на 8 бит влево
    //   - offset: смещение внутри конфигурационного пространства (8 бит)
    uint32_t addr = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset);
  
    // Записываем адрес в порт ввода-вывода 0xCF8
    // Порт 0xCF8 используется для записи адреса конфигурационного пространства PCI
    out_port_l(0xCF8, addr);

    // Записываем 32-битное значение в порт ввода-вывода 0xCFC
    // Порт 0xCFC используется для записи данных в конфигурационное пространство PCI
    out_port_l(0xCFC, value);
}

// Функция для записи 8-битного значения в конфигурационное пространство PCI устройства
void pci_write_config_byte(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset,  uint8_t value) {
    // Формируем 32-битный адрес для доступа к конфигурационному пространству
    // 
    //   - 1 << 31: бит 31 устанавливается в 1, чтобы указать на конфигурационное пространство PCI
    //   - bus << 16: номер шины PCI (8 бит) сдвигается на 16 бит влево
    //   - num << 11: номер устройства на шине (8 бит) сдвигается на 11 бит влево
    //   - function << 8: номер функции устройства (8 бит) сдвигается на 8 бит влево
    //   - offset & 0xfc: смещение внутри конфигурационного пространства выравнивается по 2 байтам
    uint32_t addr = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset & 0xfc);
  
    // Записываем адрес в порт ввода-вывода 0xCF8
    // Порт 0xCF8 используется для записи адреса конфигурационного пространства PCI
    out_port_l(0xCF8, addr);

    // Читаем 32-битное значение из порта ввода-вывода 0xCFC
    // Порт 0xCFC используется для чтения данных из конфигурационного пространства PCI
    // 
    //   - in_port_l(0xCFC) & ~(0xFF << (offset & 3)): маскируем 32-битное значение, чтобы очистить байт, в который будем записывать значение
    //   - value << ((offset & 3) * 8): сдвигаем значение на соответствующее количество бит влево, чтобы записать его в правильный байт
    //   - |: объединяем два значения, чтобы получить новое 32-битное значение
    out_port_l(0xCFC, ((in_port_l(0xCFC) & ~(0xFF << (offset & 3))) | (value << ((offset & 3) * 8))));
}

// ну тут все изи.
pci_t pci_get_data(uint8_t bus, uint8_t num, uint8_t function) {
    pci_t pci_data;
    uint16_t *pci = (uint16_t*)&pci_data;
    for (uint8_t i = 0; i < 32; ++i) pci[i] = pci_read_config_word(bus, num, function, i*2);
    return pci_data;
}
