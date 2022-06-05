#include <iostream>
#include <cstdio>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/logger.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto *rom = new loader();
    Bus bus;
    Cpu cpu(&bus);
    Logger logger(&cpu.registers1, &cpu, false);

    FILE *openrom, *openlog;
    openlog = fopen("../temp_log.txt", "w");
    openrom = fopen(rom->filename, "r");
    bus.load_rom(openrom);

    uint8_t temp;
    logger.print_instruction();
    uint16_t c;
    logger.annas_log(openlog);
    while(cpu.execute_next_instruction()){
        logger.annas_log(openlog);
        logger.print_registers();
        logger.print_flags();

        bus.read(SERIAL_SC, &temp);
        if(temp == 0x81){
            bus.read(SERIAL_SB, &temp);
            printf("%c", temp);
            temp = 0;
            bus.write(SERIAL_SC, &temp);
        }

        logger.print_instruction();
        c++;
    }

    return 0;
}
