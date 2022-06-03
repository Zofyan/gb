//
// Created by sofyan on 1-6-22.
//

#ifndef GB_CPU_H
#define GB_CPU_H

#include <cstdint>
#include "memory.h"
#include "bus.h"


typedef struct flags_t{
    uint8_t Z:1;
    uint8_t N:1;
    uint8_t H:1;
    uint8_t C:1;
    uint8_t X:4;
} flags_t;

enum flag{
    flag_z,
    flag_n,
    flag_h,
    flag_c
};

typedef struct registers{
    uint16_t *AF;
    uint16_t *BC;
    uint16_t *DE;
    uint16_t *HL;
    uint16_t *SP;
    uint16_t *PC;
    uint8_t *A;
    uint8_t *B;
    uint8_t *C;
    uint8_t *D;
    uint8_t *E;
    uint8_t *H;
    uint8_t *L;
    flags_t flags;
    bus *bus1;
} registers;

int execute_instruction(struct registers *registers, bus *bus, uint8_t *buf);

int load_instruction(uint8_t inst, struct registers *registers, bus *bus, uint8_t *buf);

int init_registers(struct registers *registers);


#endif //GB_CPU_H
