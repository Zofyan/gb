//
// Created by sofyan on 2-6-22.
//

#ifndef GB_BUS_H
#define GB_BUS_H

#define SERIAL_SB 0xFF01
#define SERIAL_SC 0xFF02

#define BUS_OFFSET(bus, offset) ((uint8_t*)(&bus + offset))
#define ROM_SIZE 16 * 1024
#define VRAM_SIZE 8 * 1024
#define ERAM_SIZE 8 * 1024
#define WRAM_SIZE 4 * 1024

#define VRAM_OFFSET (ROM_SIZE + ROM_SIZE)
#define ERAM_OFFSET (VRAM_OFFSET + VRAM_SIZE)
#define WRAM_OFFSET (ERAM_OFFSET + ERAM_SIZE)

#include <cstdint>

typedef struct bus{
    uint8_t rom_bank_16kb_0[ROM_SIZE];
    uint8_t rom_bank_16kb_n[ROM_SIZE];
    uint8_t vram_8kb[VRAM_SIZE];
    uint8_t eram_8kb[ERAM_SIZE];
    uint8_t wram_4kb[WRAM_SIZE];
    uint8_t wram_4kb_x[WRAM_SIZE];
    uint8_t nintendo_says_no_7680b_1[8 * 1024 - 512];
    uint8_t oam_159b[160];
    uint8_t nintendo_says_no_95b_2[96];
    uint8_t io_registers_127b[128];
    uint8_t hram_126b[127];
    uint8_t interrupt_enable_regsiter_1b[1];
} bus;

int init_bus(bus *bus);
int bus_read(bus *bus, uint16_t offset, uint8_t *buf, uint16_t size);
int bus_read_8b(bus *bus, uint16_t offset, uint8_t *buf);
int bus_read_16b(bus *bus, uint16_t offset, uint16_t *buf);
int bus_write(bus *bus, uint16_t offset, uint8_t *buf, uint16_t size);
int bus_write_8b(bus *bus, uint16_t offset, uint8_t *buf);
int bus_write_16b(bus *bus, uint16_t offset, uint16_t *buf);


#endif //GB_BUS_H
