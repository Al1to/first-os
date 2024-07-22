#include "./disk.h"

typedef struct {
    bool used;
    uint32_t max_port_count;
    uint8_t (*read_function)(uint16_t drive_num, uint64_t start_sector, uint32_t count, void *buf);
    uint8_t (*write_function)(uint16_t drive_num, uint64_t start_sector, uint32_t count, void *buf);
} disk_handler;

disk_handler handlers[255] = {0};

uint8_t register_disk_handler(uint8_t (*read_function)(uint16_t, uint64_t, uint32_t, void *), uint8_t (*write_function)(uint16_t, uint64_t, uint32_t, void*), uint32_t max_port_count) {
    for (uint16_t i = 0; i < 255; ++i) {
        if (!handlers[i].used) {
            handlers[i].used = true;
            handlers[i].max_port_count = max_port_count;
            handlers[i].read_function = read_function;
            handlers[i].write_function = write_function;
            return i;
        }
    }
    return 255;
}

void unregister_disk_handler(uint8_t handler_id) {
    if (handler_id == 255) {
        return;
    }

    handlers[handler_id].used = false;
    handlers[handler_id].max_port_count = 0;
    handlers[handler_id].read_function = NULL;
    handlers[handler_id].write_function = NULL;
}

uint32_t max_port_for_driver(uint8_t handler_id) {
    return handlers[handler_id].max_port_count;
}

uint8_t next_disk_handler(uint8_t handler_id) {
    for (uint16_t i = handler_id + 1; i < 255; ++i) {
        if (handlers[i].used) {
            return i;
        }
    }
    return 255;
}

uint8_t disk_read_sectors(uint32_t drive_num, uint64_t start_sector, uint32_t count, void *buf) {
    uint8_t handler = (uint8_t)(drive_num >> 16);
    
    if (handler == 255) {
        return INVALID_FILESYSTEM;
    } 
    if (!handlers[handler].used) {
        return INVALID_FILESYSTEM;
    }
    return handlers[handler].read_function(drive_num, start_sector, count, buf);
}

uint8_t disk_write_sectors(uint32_t drive_num, uint64_t start_sector, uint32_t count, void *buf) {
    uint8_t handler = (uint8_t)(drive_num >> 16);
    
    if (handler == 255) {
        return INVALID_FILESYSTEM;
    } 
    if (!handlers[handler].used) {
        return INVALID_FILESYSTEM;
    }
    return handlers[handler].write_function(drive_num, start_sector, count, buf);
}