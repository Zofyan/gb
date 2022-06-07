//
// Created by sofyan on 4-6-22.
//

#ifndef GB_LOGGER_H
#define GB_LOGGER_H


#include "cpu.h"

class Logger {
public:
    Logger(registers_t *registers, Cpu *cpu, bool enable = true);

    void print_registers();

    void print_flags();

    void print_instruction();

    void print( const char *format, ...);

    void print_bus_address(uint16_t address);

    void annas_log(FILE *log);

private:
    registers_t *registers1;
    Cpu *cpu;
    bool enable;

};


#endif //GB_LOGGER_H
