#include <iostream>
#include <cstdio>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"
#include "pthread.h"
#include "../include/logger.h"
#include "../include/lcd.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto *rom = new loader();
    Bus bus;
    Lcd lcd(160, 144);
    Ppu ppu(&bus, nullptr, &lcd);
    Cpu cpu(&bus, &ppu);
    Logger logger(&cpu.registers1, &cpu, true);

    FILE *openrom, *openlog;
    openrom = fopen(rom->filename, "r");
    //openlog = fopen("../temp_log.txt", "w");
    bus.load_rom(openrom);
    uint8_t temp;
    logger.print_instruction();
    //logger.other_log(openlog);
    uint32_t c = 0;
    while(cpu.execute_next_instruction()){
        //logger.other_log(openlog);
        //logger.print_registers();
        //logger.print_flags();
        logger.print_instruction();
        bus.read(SERIAL_SC, &temp);
        if(temp == 0x81){
            bus.read(SERIAL_SB, &temp);
            printf("%c", temp);
            fflush(stdout);
            temp = 0;
            bus.write(SERIAL_SC, &temp);
            cpu.count++;
        }
        c++;
    }

    return 0;
}
