//
// Created by sofyan on 4-6-22.
//

#ifndef GB_LOGGER_H
#define GB_LOGGER_H


#include "cpu.h"

class Logger {
public:
    Logger(registers *registers);

    void print_registers();

    void print_flags();

    void print_bus_address(uint16_t address);

private:
    registers *registers1;

};


#endif //GB_LOGGER_H
