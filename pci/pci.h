#include "../stdafx.h"

#ifndef PCI_H
#define PCI_H

typedef struct {
	uint16_t vendorID;
	uint16_t deviceID;
	uint16_t command; 
	uint16_t status;
	uint8_t  revisionID;
	uint8_t  progIF;
	uint8_t  subclass;
	uint8_t  class;
	uint8_t  cacheLineSize; 
	uint8_t  latencyTimer;
	uint8_t  headerType;
	uint8_t  BIST;
	uint32_t BAR0;
	uint32_t BAR1;
	uint32_t BAR2;
	uint32_t BAR3;
	uint32_t BAR4;
	uint32_t BAR5;
	uint32_t cardbusCISPointer;
	uint16_t subsystemVendorID;
	uint16_t subsystemID;
	uint32_t expansionROMBaseAddress;
	uint8_t  capabilitiesPointer;
	uint8_t  reserved0;
	uint16_t reserved1;
	uint32_t reserved2;
	uint8_t  irq;
	uint8_t  interruptPIN;
	uint8_t  minGrant;
	uint8_t  maxLatency;
} pci_t __attribute__((packed)); 


// pci_register_driver это самая важная тут функа
// мы ей передаем свой драйвер который написали и он там записывается
// ща чуть позже покажу как это делается

bool pci_register_driver(void (*pci_function)(pci_t, uint8_t, uint8_t, uint8_t), uint8_t class, uint8_t subclass);

// вот эти функции 4 штуки эти 2 и след 2, я честно не очень понял чзх, ты там щас в .c их увидишь и так же подумаешь
uint32_t pci_read_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset);
uint16_t  pci_read_config_word(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset);

void pci_write_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint32_t value);
void  pci_write_config_byte(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset,  uint8_t value);

// тут все просто, ей отдаешь номер и всякую базовую инфу про драйвер а она возвращает тебе фулл структуру pci_t, где гораздо больше
pci_t pci_get_data(uint8_t bus, uint8_t num, uint8_t function);

// вот эта функция тоже оч инетересная
// она чекает какие аппаратуры подключены к pci и говорит есть ли для них дрова
// ПС я не добавлял к ней дрова, хотя они и есть уже (таймер, вга и остальные)
// ща кста скину в лс в тг или дс скрин
void pci_scan();

#endif