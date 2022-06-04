#include <iostream>
#include <cstdio>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/bus.h"
#include "../include/logger.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto *rom = new loader();
    registers registers;
    bus bus;
    init_registers(&registers);
    init_bus(&bus);

    Logger logger(&registers);

    registers.bus1 = &bus;
    *registers.PC = 0x0100;

    uint8_t temp = 0;

    FILE *openrom;
    openrom = fopen(rom->filename, "r");

    fread(bus.rom_bank_16kb_0, 64000, 1, openrom);
    uint16_t c = 0;
    while(bus.rom_bank_16kb_0[*registers.PC] != HALT && c < 20000){
        printf("%02X %04X\n", (uint8_t)bus.rom_bank_16kb_0[*registers.PC], *registers.PC);
        execute_instruction(&registers, &bus, bus.rom_bank_16kb_0);
        logger.print_registers();
        bus_read(&bus, SERIAL_SC, &temp, 1);
        if(temp == 0x81){
            bus_read(&bus, SERIAL_SB, &temp, 1);
            printf("%c", temp);
            temp = 0;
            bus_write(&bus, SERIAL_SC, &temp, 1);
        }
        c++;
    }

    return 0;
}
