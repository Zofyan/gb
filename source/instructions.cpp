//
// Created by sofyan on 1-6-22.
//

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "../include/instructions.h"
#include "../include/memory.h"

void inc8(uint8_t* reg, registers *registers, bool inc, bool memory){

    uint8_t newValue = *reg;
    if((*reg == 0xFF && inc) || (*reg == 0x00 && !inc)){
        registers->flags.C = 1;
    }
    else registers->flags.C = 0;

    if(inc){
        newValue++;
        registers->flags.N = 0;
    }
    else{
        newValue--;
        registers->flags.N = 1;
    }
    *reg = newValue;
    (*registers->PC)++;

    if(*reg == 0x00){
        registers->flags.Z = 1;
    }
    else registers->flags.Z = 0;
    //printf("INC %04X\n", *registers->DE);
}

void inc16(uint16_t* reg, registers *registers, bool inc, bool memory){

    if((*reg == 0xFFFF && inc) || (*reg == 0x0000 && !inc)){
        registers->flags.C = 1;
    }
    else registers->flags.C = 0;

    if(inc){
        (*reg)++;
        registers->flags.N = 0;
    }
    else{
        (*reg)--;
        registers->flags.N = 1;
    }

    if(*reg == 0x0000){
        registers->flags.Z = 1;
    }
    else registers->flags.Z = 0;
    (*registers->PC)++;
    //printf("INC %02X\n", *registers->AF);
}

void ret(registers *registers){
    assert(false);
    (*registers->PC) = *registers->SP;
    (*registers->SP) += 2;
    //printf("RET %02X %02X\n", *registers->PC, *registers->SP);
}

void jp(registers *registers, uint8_t* buf){
    uint16_t temp;
    bus_read_16b(registers->bus1, *registers->PC + 1, &temp);
    (*registers->PC) = temp;
    //printf("JP %02X\n", temp);
}

void jr(registers *registers, uint8_t* buf){
    int8_t temp;
    if((*registers->PC) != 0x0209 && (*registers->PC) != 0x020d){
        printf("weird\n");
    }
    bus_read(registers->bus1, *registers->PC + 1, (uint8_t*)&temp, 1);
    //printf("PC %04X\n", (*registers->PC));
    (*registers->PC) += temp;
    //printf("PC %04X\n", (*registers->PC));
    //printf("JR %02X\n", temp);
    //printf("JR %i\n", temp);
}

void jf(flag flag, bool set, bool relative, registers *registers, uint8_t *buf){
    bool jump = false;
    switch(flag){
        case flag_z:
            if(registers->flags.Z == set){
                jump = true;
            }
            break;
        case flag_n:
            if(registers->flags.N == set){
                jump = true;
            }
            break;
        case flag_c:
            if(registers->flags.C == set){
                jump = true;
            }
            break;
        case flag_h:
            if(registers->flags.H == set){
                jump = true;
            }
            break;
        default:
            break;
    }
    if(jump && relative) jr(registers, buf);
    else if(jump) jp(registers, buf);
    else if(relative) (*registers->PC) += 2;
    else (*registers->PC) += 3;
}

void ld16(uint16_t* reg, registers *registers, uint8_t *buf){
    //printf("ld %04X\n", *registers->PC + 1);
    bus_read_16b(registers->bus1, *registers->PC + 1, reg);
    (*registers->PC) += 3;
}

void ld8(uint8_t* reg, registers *registers, uint8_t *buf){
    (*reg) = *((uint8_t*) &buf[*registers->PC + 1]);
    (*registers->PC) += 2;
}

void ldm8(uint16_t* reg, registers *registers, uint8_t *buf){
    bus_write_8b(registers->bus1, *reg, &buf[*registers->PC + 1]);
    (*registers->PC) += 2;
}
void ldm16(uint16_t* reg, registers *registers, uint8_t *buf){
    bus_write(registers->bus1, *reg, &buf[*registers->PC + 1], 2);
    (*registers->PC) += 3;
}

void ldr8(uint8_t* regA, uint8_t* regB, registers *registers){
    (*regA) = *regB;
    (*registers->PC)++;
}

void ldm2r8(uint8_t* regD, uint16_t* regS, registers *registers, uint8_t *buf){
    bus_read_8b(registers->bus1, *regS, regD);
    (*registers->PC)++;
}

void ldr2m8(uint8_t* regS, uint16_t* regD, registers *registers, uint8_t *buf){

    //printf("write %04X\n", *regS);
    //printf("location %04X\n", *regD );
    bus_write_8b(registers->bus1, *regD , regS);
    (*registers->PC)++;
}