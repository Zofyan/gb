//
// Created by sofyan on 2-6-22.
//

#ifndef GB_BUS_H
#define GB_BUS_H

#define SERIAL_SB 0xFF01
#define SERIAL_SC 0xFF02

#define BUS_OFFSET(bus, offset) ((uint8_t*)(&bus + offset))

#include <cstdint>

typedef struct bus{
    uint8_t rom_bank_16kb_0[16 * 1024];
    uint8_t rom_bank_16kb_n[16 * 1024];
    uint8_t vram_8kb[8 * 1024];
    uint8_t eram_8kb[8 * 1024];
    uint8_t wram_4kb[4 * 1024];
    uint8_t wram_4kb_x[4 * 1024];
    uint8_t nintendo_says_no_7680b_1[8 * 1024 - 512];
    uint8_t oam_159b[160];
    uint8_t nintendo_says_no_95b_2[96];
    uint8_t io_registers_127b[128];
    uint8_t hram_126b[127];
    uint8_t interrupt_enable_regsiter_1b[1];
} bus;

int init_bus(bus *bus);
int bus_read(bus *bus, uint16_t offset, uint8_t *buf, uint8_t size);
int bus_write(bus *bus, uint16_t offset, uint8_t *buf, uint8_t size);


#endif //GB_BUS_H
