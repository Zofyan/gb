//
// Created by sofyan on 2-6-22.
//

#ifndef GB_BUS_H
#define GB_BUS_H

#define SERIAL_SB 0xFF01
#define SERIAL_SC 0xFF02

#define ROM_0              0x0000
#define ROM_0_END          0x3FFF
#define ROM_N              0x4000
#define ROM_N_END          0x7FFF
#define VRAM               0x8000
#define VRAM_END           0x9FFF
#define ERAM               0xA000
#define ERAM_END           0xBFFF
#define WRAM_0             0xC000
#define WRAM_0_END         0xCFFF
#define WRAM_N             0xD000
#define WRAM_N_END         0xDFFF
#define OAM                0xFE00
#define OAM_END            0xFE9F
#define IO_REGISTERS       0xFF00
#define IO_REGISTERS_END   0xFF7F
#define HRAM               0xFF80
#define HRAM_END           0xFFFE
#define INT_ENABLE         0xFFFF
#define INT_ENABLE_END     0xFFFF
#define INT_REQUEST        0xFF0F


#include <cstdint>
#include <cstdio>

typedef struct interrupts{
    uint8_t vblank : 1;
    uint8_t lcd_stat : 1;
    uint8_t timer : 1;
    uint8_t serial : 1;
    uint8_t joypad : 1;
    uint8_t x : 3;
} interrupts_t;

typedef struct timer_tac {
    uint8_t timer_clock : 2;
    uint8_t timer_enable : 1;
    uint8_t x : 5;
} timer_tac_t;

typedef struct timer{
    uint8_t timer_div;
    uint8_t timer_tima;
    uint8_t timer_tma;
    timer_tac_t timer_tac;
} timer_t2;

class Bus{
private:
    uint8_t *rom_0;
    uint8_t *rom_n;
    uint8_t *vram;
    uint8_t *eram;
    uint8_t *wram_0;
    uint8_t *wram_n;
    uint8_t *oam;
    uint8_t *io_registers;
    uint8_t *hram;
    uint8_t *int_enable;
    void read_rom(uint16_t address, uint8_t *buffer);
    void read_vram(uint16_t address, uint8_t *buffer);
    void read_eram(uint16_t address, uint8_t *buffer);
    void read_wram(uint16_t address, uint8_t *buffer);
    void read_oam(uint16_t address, uint8_t *buffer);
    void read_io_registers(uint16_t address, uint8_t *buffer);
    void read_hram(uint16_t address, uint8_t *buffer);
    void read_int_enable(uint16_t address, uint8_t *buffer);
    void write_rom(uint16_t address, uint8_t *buffer);
    void write_vram(uint16_t address, uint8_t *buffer);
    void write_eram(uint16_t address, uint8_t *buffer);
    void write_wram(uint16_t address, uint8_t *buffer);
    void write_oam(uint16_t address, uint8_t *buffer);
    void write_io_registers(uint16_t address, uint8_t *buffer);
    void write_hram(uint16_t address, uint8_t *buffer);
    void write_int_enable(uint16_t address, uint8_t *buffer);
public:
    Bus();
    void load_rom(FILE *rom);
    void read(uint16_t address, uint8_t *buffer);
    uint8_t read_v(uint16_t address);
    void write(uint16_t address, uint8_t *buffer);
    void write_v(uint16_t address, uint8_t value);
    void push(uint16_t value, uint16_t *sp);
    uint16_t pop(uint16_t *sp);
    interrupts_t *interrupt_enable;
    interrupts_t *interrupt_request;

    timer_t2 *timer;
};

#endif //GB_BUS_H
