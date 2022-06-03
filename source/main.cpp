#include <iostream>
#include <cstdio>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/bus.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto *rom = new loader();
    registers registers;
    bus bus;
    init_registers(&registers);
    init_bus(&bus);
    registers.bus1 = &bus;
    *registers.PC = 0x0100;

    uint8_t temp = 0;

    FILE *openrom;
    openrom = fopen(rom->filename, "r");

    auto *buffer = (uint8_t*) calloc(64000, 1);
    fread(buffer, 64000, 1, openrom);
    while(buffer[*registers.PC] != HALT){
        //printf("%02X %u\n", (uint8_t)buffer[*registers.PC], *registers.PC);
        execute_instruction(&registers, &bus, buffer);
        bus_read(&bus, SERIAL_SC, &temp, 1);
        if(temp == 0x81){
            bus_read(&bus, SERIAL_SB, &temp, 1);
            printf("%c", temp);
        }
    }

    return 0;
}
