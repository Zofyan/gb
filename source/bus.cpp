//
// Created by sofyan on 2-6-22.
//

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "../include/bus.h"
#include "../include/logger.h"
#include "../include/instructions.h"

Bus::Bus() {
s    rom_0 = (uint8_t*)calloc(ROM_0_END - ROM_0 + 1, 1);
    rom_n = (uint8_t*)calloc(ROM_N_END - ROM_N + 1, 1);
    vram = (uint8_t*)calloc(VRAM_END - VRAM_END + 1, 1);
    eram = (uint8_t*)calloc(ERAM_END - ERAM + 1, 1);
    wram_0 = (uint8_t*)calloc(WRAM_0_END - WRAM_0 + 1, 1);
    wram_n = (uint8_t*)calloc(WRAM_N_END - WRAM_N + 1, 1);
    oam = (uint8_t*)calloc(OAM_END - OAM + 1, 1);
    io_registers = (uint8_t*)calloc(IO_REGISTERS_END - IO_REGISTERS + 1, 1);
    hram = (uint8_t*)calloc(HRAM_END - HRAM + 1, 1);
    int_enable = (uint8_t*)calloc(INT_ENABLE_END - INT_ENABLE + 1, 1);

    interrupt_request = (interrupts_t *) ((uint8_t *) (io_registers + (INT_REQUEST & 0x00FF)));
    memset(interrupt_request, 0, 1);
    interrupt_enable = (interrupts_t *) ((uint8_t *) (int_enable));
    memset(interrupt_enable, 0, 1);


    joypad = (JoyPad_t *) &io_registers[0];
    timer = (timer_t2*) &io_registers[4];
    io_registers[0] = 0x00;
    ppu_registers = (PPURegisters_t *) &io_registers[0x40];
    lcd_status = (stat_t *) &io_registers[0x41];
    pthread_mutex_init(&lock, NULL);

    sprites = (oam_t*)oam;

    memset(pixels, 0, 144 * 160);
}

void Bus::read(uint16_t address, uint8_t *buffer) {
    if(address <= ROM_N_END) read_rom(address, buffer);
    else if(address <= VRAM_END) read_vram(address, buffer);
    else if(address <= ERAM_END) read_eram(address, buffer);
    else if(address <= WRAM_N_END) read_wram(address, buffer);
    else if(address < OAM) (*buffer) = 0xFF;
    else if(address <= OAM_END) read_oam(address, buffer);
    else if(address < IO_REGISTERS) (*buffer) = 0xFF;
    else if(address <= IO_REGISTERS_END) read_io_registers(address, buffer);
    else if(address <= HRAM_END) read_hram(address, buffer);
    else if(address <= INT_ENABLE_END) read_int_enable(address, buffer);
    else exit(2);
}

void Bus::write(uint16_t address, uint8_t *buffer) {
    if(address <= ROM_N_END) write_rom(address, buffer);
    else if(address <= VRAM_END) write_vram(address, buffer);
    else if(address <= ERAM_END) write_eram(address, buffer);
    else if(address <= WRAM_N_END) write_wram(address, buffer);
    else if(address < OAM);
    else if(address <= OAM_END) write_oam(address, buffer);
    else if(address < IO_REGISTERS);
    else if(address <= IO_REGISTERS_END) write_io_registers(address, buffer);
    else if(address <= HRAM_END) write_hram(address, buffer);
    else if(address <= INT_ENABLE_END) write_int_enable(address, buffer);
    else exit(1);
}

void Bus::write_int_enable(uint16_t address, uint8_t *buffer) {
    memcpy(&int_enable[address - INT_ENABLE], buffer, 1);
}

void Bus::read_rom(uint16_t address, uint8_t *buffer) {
    if(address <= ROM_0_END)
        memcpy(buffer, &rom_0[address - ROM_0], 1);
    else
        memcpy(buffer, &rom_n[address - ROM_N], 1);
}

void Bus::read_vram(uint16_t address, uint8_t *buffer) {
    memcpy(buffer, &vram[address - VRAM], 1);
}

void Bus::read_eram(uint16_t address, uint8_t *buffer) {
    memcpy(buffer, &eram[address - ERAM], 1);
}

void Bus::read_wram(uint16_t address, uint8_t *buffer) {
    if(address <= WRAM_0_END)
        memcpy(buffer, &wram_0[address - WRAM_0], 1);
    else
        memcpy(buffer, &wram_n[address - WRAM_N], 1);
}

void Bus::read_oam(uint16_t address, uint8_t *buffer) {
    memcpy(buffer, &oam[address - OAM], 1);
}

void Bus::read_io_registers(uint16_t address, uint8_t *buffer) {
    memcpy(buffer, &io_registers[address - IO_REGISTERS], 1);
}

void Bus::read_hram(uint16_t address, uint8_t *buffer) {
    memcpy(buffer, &hram[address - HRAM], 1);
}

void Bus::read_int_enable(uint16_t address, uint8_t *buffer) {
    memcpy(buffer, &int_enable[address - INT_ENABLE], 1);
}

void Bus::write_rom(uint16_t address, uint8_t *buffer) {
}

void Bus::write_vram(uint16_t address, uint8_t *buffer) {
    if(address > 0x9800 && address < 0x9BFF){
        //printf("writing to le tilemap\n");
    }
    memcpy(&vram[address - VRAM], buffer, 1);
}

void Bus::write_eram(uint16_t address, uint8_t *buffer) {
    memcpy(&eram[address - ERAM], buffer, 1);
}

void Bus::write_wram(uint16_t address, uint8_t *buffer) {
    if(address <= WRAM_0_END)
        memcpy(&wram_0[address - WRAM_0], buffer, 1);
    else
        memcpy(&wram_n[address - WRAM_N], buffer, 1);
}

void Bus::write_oam(uint16_t address, uint8_t *buffer) {
    memcpy(&oam[address - OAM], buffer, 1);
}

void Bus::write_io_registers(uint16_t address, uint8_t *buffer) {
    if(address == TIMER_DIV){
        timer->timer_div = 0;
        timer->timer_tima = 0;
        return;
    }
    if(address == 0xFF00){
        (*buffer) = ((*buffer) & 0xF0) | (*(uint8_t*)&joypad & 0x0F);
    }
    memcpy(&io_registers[address - IO_REGISTERS], buffer, 1);
    if(address == 0xFF46){
        dma_start(*buffer);
    }
}

void Bus::write_hram(uint16_t address, uint8_t *buffer) {
    memcpy(&hram[address - HRAM], buffer, 1);
}

void Bus::load_rom(FILE *rom) {
    fread(rom_0, 1, ROM_0_END - ROM_0 + 1, rom);
    fread(rom_n, 1, ROM_N_END - ROM_N + 1, rom);
}

void Bus::push(uint16_t value, uint16_t *sp) {
    write(*sp - 1, ((uint8_t *) (&value)) + 1);
    write(*sp - 2, (uint8_t *) (&value));
    (*sp) -= 2;
}

uint16_t Bus::pop(uint16_t *sp) {
    uint16_t value;
    read(*sp + 1, ((uint8_t *)(&value)) + 1);
    read(*sp + 0, (uint8_t *)(&value));
    (*sp) += 2;
    return value;
}

uint8_t Bus::read_v(uint16_t address) {
    uint8_t value;
    read(address, &value);
    return value;
}

void Bus::write_v(uint16_t address, uint8_t value) {
    write(address, &value);
}

void Bus::dma_start(uint8_t value) {
    dma_transfer = true;
    dma_delay = 2;
    dma_source_base = value * 0x100;
}

void Bus::dma_tick() {
    if(!dma_transfer) return;
    if(dma_delay > 0) {
        dma_delay--;
        return;
    }
    write_v(0xFE00 + dma_ticks, read_v(dma_source_base + dma_ticks));
    dma_ticks++;
    if(dma_ticks == 0xA0){
        dma_transfer = false;
        dma_ticks = 0;
    }
}

void Bus::read_cpu(uint16_t address, uint8_t *buffer) {
    if(dma_transfer && (address < HRAM || address > HRAM_END)) return;
    read(address, buffer);
}

void Bus::write_cpu(uint16_t address, uint8_t *buffer) {
    if(dma_transfer && (address < HRAM || address > HRAM_END)) return;
    write(address, buffer);
}
