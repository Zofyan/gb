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


#define  size_4MiB   0x400000
#define  size_2MiB   0x200000
#define  size_1MiB   0x100000
#define  size_512KiB 0x80000
#define  size_256KiB 0x40000
#define  size_128KiB 0x20000
#define  size_64KiB  0x1000
#define  size_32KiB  0x8000
#define  size_8KiB  0x2000


#include <cstdint>
#include <cstdio>
#include <queue>
#include <pthread.h>
#include "oam.h"


enum mbc_t{
    MBC0,
    MBC1,
    MBC2,
    MBC3,
    MBC5,
    MBC6,
    MBC7
};

typedef struct interrupts{
    uint8_t vblank : 1;
    uint8_t lcd_stat : 1;
    uint8_t timer : 1;
    uint8_t serial : 1;
    uint8_t joypad : 1;
    uint8_t x : 3;
} interrupts_t;

typedef struct lcdc{
    uint8_t bg_window_enable : 1;
    uint8_t obj_enable : 1;
    uint8_t obj_size : 1;
    uint8_t bg_tile_map_area : 1;
    uint8_t bg_window_tile_data_area : 1;
    uint8_t window_enable : 1;
    uint8_t window_tile_map_area : 1;
    uint8_t lcd_ppu_enable : 1;
} lcdc_t;

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

typedef struct PPURegisters{
    lcdc_t lcdc;
    uint8_t lcds;
    uint8_t scy;
    uint8_t scx;
    uint8_t ly;
    uint8_t lyc;
} PPURegisters_t;

typedef struct JoyPad{
    uint8_t right_a : 1;
    uint8_t left_b : 1;
    uint8_t up_select : 1;
    uint8_t down_start : 1;
    uint8_t direction : 1;
    uint8_t action : 1;
    uint8_t x : 2;
} JoyPad_t;

typedef struct stat{
    uint8_t mode : 2;
    uint8_t lyc_ly_flag : 1;
    uint8_t hblank_stat_int : 1;
    uint8_t vblank_stat_int : 1;
    uint8_t oam_stat_interrupt : 1;
    uint8_t lyc_ly_stat_interrupt : 1;
    uint8_t x : 1;
} stat_t;

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
    uint8_t *roms[256];
    uint8_t *erams[256];
    uint32_t rom_number = 1;
    uint32_t rom_size = 0;
    uint32_t ram_size = 0;
    bool ram = false;
    mbc_t mbc = MBC0;
    Bus();
    void read(uint16_t address, uint8_t *buffer);
    void read_cpu(uint16_t address, uint8_t *buffer);
    uint8_t read_v(uint16_t address);
    void write(uint16_t address, uint8_t *buffer);
    void write_cpu(uint16_t address, uint8_t *buffer);
    void write_v(uint16_t address, uint8_t value);
    void push(uint16_t value, uint16_t *sp);
    uint16_t pop(uint16_t *sp);

    bool dma_transfer = false;
    uint16_t dma_ticks = 0;
    int16_t dma_delay = 0;
    uint16_t dma_source_base = 0;
    void dma_tick();
    void dma_start(uint8_t value);

    interrupts_t *interrupt_enable;
    interrupts_t *interrupt_request;

    timer_t2 *timer;
    PPURegisters_t *ppu_registers;
    stat_t *lcd_status;
    JoyPad_t *joypad;
    JoyPad_t joypad_real1, joypad_real2;
    uint8_t pixels[144 * 160 * 4];
    pthread_mutex_t lock;

    oam_t *sprites[40];

    uint8_t *get_buffer(uint16_t address);
};

#endif //GB_BUS_H
