//
// Created by sofyan on 4-6-22.
//

#include <cstdio>
#include <cstdarg>
#include "../include/logger.h"

Logger::Logger(registers *registers, Cpu *cpu, bool enable){
        this->registers1 = registers;
        this->cpu = cpu;
        this->enable = enable;
    }

    void Logger::print_registers(){
        if(!enable) return;
        printf("DEBUG: registers1: AF %04X - BC %04X - DE %04X - HL %04X - SP %04X - PC %04X\n",
               *registers1->AF,
               *registers1->BC,
               *registers1->DE,
               *registers1->HL,
               *registers1->SP,
               *registers1->PC
        );
    }

    void Logger::print_instruction(){
        if(!enable) return;
        uint8_t temp;
        uint16_t temp2;
        cpu->bus->read(*cpu->registers1.PC, &temp);
        cpu->bus->read(*cpu->registers1.PC + 1, (uint8_t*)&temp2);
        cpu->bus->read(*cpu->registers1.PC + 2, ((uint8_t*)&temp2) + 1);
        printf("\033[0;31m");
        printf("DEBUG: INSTRUCTION:    instruction %02X - address %04X - next 2 bytes %04X\n",
               temp,
               *cpu->registers1.PC,
               temp2
        );
        printf("\033[0m");
    }

    void Logger::print_flags(){
        if(!enable) return;
        printf("DEBUG: FLAGS:    Z %01X - N %01X - H %01X - C %01X\n",
               registers1->flags->Z,
               registers1->flags->N,
               registers1->flags->H,
               registers1->flags->C
        );
    }

    void Logger::print_bus_address(uint16_t address){
        uint8_t temp;
        cpu->bus->read(address, &temp);
        printf("DEBUG: BUS: address %04X - value %02X\n",
               address,
               temp
        );
    }

void Logger::print(const char *format, ...) {
    if(!enable) return;
    va_list args;
    va_start(args, format);
    vprintf(format, args);
}

void Logger::annas_log(FILE *log) {
    return;
    uint8_t temp1, temp2, temp3;
    cpu->bus->read(*cpu->registers1.PC, &temp1);
    cpu->bus->read(*cpu->registers1.PC + 1, &temp2);
    cpu->bus->read(*cpu->registers1.PC + 2, &temp3);
    fprintf(
            log,
            "%04X: (%02X %02X %02X) AF: %04X, BC: %04X, DE: %04X, HL: %04X SP:%04X\n",
            *cpu->registers1.PC,
            temp1, temp2, temp3,
            *cpu->registers1.AF,
            *cpu->registers1.BC,
            *cpu->registers1.DE,
            *cpu->registers1.HL,
            *cpu->registers1.SP
    );
}

void Logger::other_log(FILE *log) {
    if(count > 0x4FFFFF) return;
    return;
    count++;
    fprintf(
            log,
            "A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X (%02X %02X %02X %02X)\n",
            *cpu->registers1.A,
            *cpu->registers1.F,
            *cpu->registers1.B,
            *cpu->registers1.C,
            *cpu->registers1.D,
            *cpu->registers1.E,
            *cpu->registers1.H,
            *cpu->registers1.L,
            *cpu->registers1.SP,
            *cpu->registers1.PC,
            cpu->bus->read_v(*cpu->registers1.PC + 0),
            cpu->bus->read_v(*cpu->registers1.PC + 1),
            cpu->bus->read_v(*cpu->registers1.PC + 2),
            cpu->bus->read_v(*cpu->registers1.PC + 3)
    );
}

