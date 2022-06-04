//
// Created by sofyan on 4-6-22.
//

#include <cstdio>
#include "../include/logger.h"

Logger::Logger(registers *registers){
        this->registers1 = registers;
    }

    void Logger::print_registers(){
        printf("DEBUG: registers1: AF %04X - BC %04X - DE %04X - HL %04X - SP %04X - PC %04X\n",
               *registers1->AF,
               *registers1->BC,
               *registers1->DE,
               *registers1->HL,
               *registers1->SP,
               *registers1->PC
        );
    }

    void Logger::print_flags(){
        printf("DEBUG: FLAGS:    Z %01X - N %01X - H %01X - C %01X\n",
               registers1->flags.Z,
               registers1->flags.N,
               registers1->flags.H,
               registers1->flags.C
        );
    }

    void Logger::print_bus_address(uint16_t address){
        uint8_t temp;
        bus_read_8b(registers1->bus1, address, &temp);
        printf("DEBUG: BUS: address %04X - value %02X\n",
               address,
               temp
        );
    }

