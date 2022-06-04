//
// Created by sofyan on 1-6-22.
//

#include <cstdio>
#include <cstdlib>
#include "../include/cpu.h"
#include "../include/instructions.h"
#include "../include/bus.h"

Cpu::Cpu(Bus *bus) {
    registers1.AF = static_cast<uint16_t *>(calloc(2, 1));
    registers1.BC = static_cast<uint16_t *>(calloc(2, 1));
    registers1.DE = static_cast<uint16_t *>(calloc(2, 1));
    registers1.HL = static_cast<uint16_t *>(calloc(2, 1));
    registers1.SP = static_cast<uint16_t *>(calloc(2, 1));
    registers1.PC = static_cast<uint16_t *>(calloc(2, 1));
    registers1.A = reinterpret_cast<uint8_t *>(registers1.AF) + 1;
    registers1.B = reinterpret_cast<uint8_t *>(registers1.BC) + 1;
    registers1.C = reinterpret_cast<uint8_t *>(registers1.BC);
    registers1.D = reinterpret_cast<uint8_t *>(registers1.DE) + 1;
    registers1.E = reinterpret_cast<uint8_t *>(registers1.DE);
    registers1.H = reinterpret_cast<uint8_t *>(registers1.HL) + 1;
    registers1.L = reinterpret_cast<uint8_t *>(registers1.HL);
    registers1.flags = (flags_t*) (registers1.AF);
    registers1.flags->Z = 0;
    registers1.flags->C = 0;
    registers1.flags->H = 0;
    registers1.flags->N = 0;
    registers1.flags->X = 0;

    (*registers1.PC) = 0x100;
    this->bus = bus;
}

bool Cpu::execute_next_instruction() {
    uint8_t instruction;
    bus->read(*registers1.PC, &instruction);

    if(execute_ld_r_to_r_8(instruction)) return true;
    if(execute_inc_r_16(instruction)) return true;
    if(execute_dec_r_16(instruction)) return true;
    if(execute_inc_r_8(instruction)) return true;
    if(execute_dec_r_8(instruction)) return true;
    if(execute_ld_r_d8(instruction)) return true;
    if(execute_ld_r_d16(instruction)) return true;
    if(execute_from_m(instruction)) return true;
    if(execute_to_m(instruction)) return true;
    if(execute_jump(instruction)) return true;

    switch (instruction) {
        case NOP:
            (*registers1.PC)++;
            break;
        case HALT:
            return false;
        default:
            printf("whoops %02X\n", instruction);
            exit(0);
    }
    return true;
}

bool Cpu::execute_ld_r_to_r_8(uint8_t instruction) {
    if(instruction == 0x76) return false;
    uint8_t *regA;
    uint8_t *regB;
    switch (instruction & 0xF8){
        case 0x40:
            regA = registers1.B;
            break;
        case 0x48:
            regA = registers1.C;
            break;
        case 0x50:
            regA = registers1.D;
            break;
        case 0x58:
            regA = registers1.E;
            break;
        case 0x60:
            regA = registers1.H;
            break;
        case 0x68:
            regA = registers1.L;
            break;
        case 0x78:
            regA = registers1.A;
            break;
        default:
            return false;
    }
    switch (instruction & 0x07){
        case 0x00:
            regB = registers1.B;
            break;
        case 0x01:
            regB = registers1.C;
            break;
        case 0x02:
            regB = registers1.D;
            break;
        case 0x03:
            regB = registers1.E;
            break;
        case 0x04:
            regB = registers1.H;
            break;
        case 0x05:
            regB = registers1.L;
            break;
        case 0x07:
            regB = registers1.A;
            break;
        default:
            return false;
    }
    execute_ld_r_to_r_8(regA, regB);
    return true;
}

void Cpu::execute_ld_r_to_r_8(uint8_t *regA, const uint8_t *regB) {
    (*regA) = *regB;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::inc_r_16(uint16_t *reg) {
    (*reg)++;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::dec_r_16(uint16_t *reg) {
    (*reg)--;

    cycles(2);
    (*registers1.PC) += 1;
}


void Cpu::inc_r_8(uint8_t *reg) {
    if((*reg) == 0xFF) registers1.flags->C = 1;
    else registers1.flags->C = 0;

    (*reg)++;
    registers1.flags->N = 0;

    if(*reg == 0x00) registers1.flags->Z = 1;
    else registers1.flags->Z = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::dec_r_8(uint8_t *reg) {
    if((*reg) == 0x00) registers1.flags->C = 1;
    else registers1.flags->C = 0;

    (*reg)--;
    registers1.flags->N = 1;

    if(*reg == 0x00) registers1.flags->Z = 1;
    else registers1.flags->Z = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::cycles(uint8_t cycles) {

}

bool Cpu::execute_inc_r_16(uint8_t instruction) {
    switch (instruction) {
        case INC_BC:
            inc_r_16(registers1.BC);
            break;
        case INC_HL:
            inc_r_16(registers1.HL);
            break;
        case INC_DE:
            inc_r_16(registers1.DE);
            break;
        case INC_SP:
            inc_r_16(registers1.SP);
            break;
        default:
            return false;
    }
    return true;
}
bool Cpu::execute_dec_r_16(uint8_t instruction) {
    switch (instruction) {
        case DEC_BC:
            dec_r_16(registers1.BC);
            break;
        case DEC_HL:
            dec_r_16(registers1.HL);
            break;
        case DEC_DE:
            dec_r_16(registers1.DE);
            break;
        case DEC_SP:
            dec_r_16(registers1.SP);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_inc_r_8(uint8_t instruction) {
    switch (instruction) {
        case INC_A:
            inc_r_8(registers1.A);
            break;
        case INC_B:
            inc_r_8(registers1.B);
            break;
        case INC_C:
            inc_r_8(registers1.C);
            break;
        case INC_D:
            inc_r_8(registers1.D);
            break;
        case INC_E:
            inc_r_8(registers1.E);
            break;
        case INC_H:
            inc_r_8(registers1.H);
            break;
        case INC_L:
            inc_r_8(registers1.L);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_dec_r_8(uint8_t instruction) {
    switch (instruction) {
        case DEC_A:
            dec_r_8(registers1.A);
            break;
        case DEC_B:
            dec_r_8(registers1.B);
            break;
        case DEC_C:
            dec_r_8(registers1.C);
            break;
        case DEC_D:
            dec_r_8(registers1.D);
            break;
        case DEC_E:
            dec_r_8(registers1.E);
            break;
        case DEC_H:
            dec_r_8(registers1.H);
            break;
        case DEC_L:
            dec_r_8(registers1.L);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_ld_r_d8(uint8_t instruction) {
    switch (instruction) {
        case LD_A_d8:
            ld_r_d8(registers1.A);
            break;
        case LD_B_d8:
            ld_r_d8(registers1.B);
            break;
        case LD_C_d8:
            ld_r_d8(registers1.C);
            break;
        case LD_D_d8:
            ld_r_d8(registers1.D);
            break;
        case LD_E_d8:
            ld_r_d8(registers1.E);
            break;
        case LD_H_d8:
            ld_r_d8(registers1.H);
            break;
        case LD_L_d8:
            ld_r_d8(registers1.L);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_d8(uint8_t *reg) {
    bus->read((*registers1.PC) + 1, reg);

    cycles(2);
    (*registers1.PC) += 2;
}

bool Cpu::execute_ld_r_d16(uint8_t instruction) {
    switch (instruction) {
        case LD_BC_d16:
            ld_r_d16(registers1.BC);
            break;
        case LD_DE_d16:
            ld_r_d16(registers1.DE);
            break;
        case LD_HL_d16:
            ld_r_d16(registers1.HL);
            break;
        case LD_SP_d16:
            ld_r_d16(registers1.SP);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_d16(uint16_t *reg) {
    bus->read((*registers1.PC) + 1, (uint8_t*)reg);
    bus->read((*registers1.PC) + 2, ((uint8_t*)reg) + 1);

    cycles(3);
    (*registers1.PC) += 3;
}

bool Cpu::execute_from_m(uint8_t instruction) {
    switch (instruction) {
        case LD_A_BC_m:
            ld_m_to_r_8(registers1.A, *registers1.BC);
            break;
        case LD_A_DE_m:
            ld_m_to_r_8(registers1.A, *registers1.DE);
            break;
        case LD_A_HL_mp:
            ld_m_to_r_8(registers1.A, *registers1.HL);
            (*registers1.HL)++;
            break;
        case LD_A_HL_mm:
            ld_m_to_r_8(registers1.A, *registers1.HL);
            (*registers1.HL)--;
            break;
        case LD_B_HL_m:
            ld_m_to_r_8(registers1.B, *registers1.HL);
            break;
        case LD_C_HL_m:
            ld_m_to_r_8(registers1.C, *registers1.HL);
            break;
        case LD_D_HL_m:
            ld_m_to_r_8(registers1.D, *registers1.HL);
            break;
        case LD_E_HL_m:
            ld_m_to_r_8(registers1.E, *registers1.HL);
            break;
        case LD_H_HL_m:
            ld_m_to_r_8(registers1.H, *registers1.HL);
            break;
        case LD_L_HL_m:
            ld_m_to_r_8(registers1.L, *registers1.HL);
            break;
        case LD_A_HL_m:
            ld_m_to_r_8(registers1.A, *registers1.HL);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_to_m(uint8_t instruction) {
    switch(instruction){
        case LD_BC_m_A:
            ld_r_to_m_8(registers1.A, *registers1.BC);
            break;
        case LD_DE_m_A:
            ld_r_to_m_8(registers1.A, *registers1.DE);
            break;
        case LD_HL_mp_A:
            ld_r_to_m_8(registers1.A, *registers1.HL);
            (*registers1.HL)++;
            break;
        case LD_HL_mm_A:
            ld_r_to_m_8(registers1.A, *registers1.HL);
            (*registers1.HL)--;
            break;
        case INC_HL_m:
            inc_m(*registers1.HL);
            break;
        case DEC_HL_m:
            dec_m(*registers1.HL);
            break;
        case LD_HL_m_d8:
            ld_m_d8(*registers1.HL);
            break;
        case LD_HL_m_B:
            ld_r_to_m_8(registers1.B, *registers1.HL);
            break;
        case LD_HL_m_C:
            ld_r_to_m_8(registers1.C, *registers1.HL);
            break;
        case LD_HL_m_D:
            ld_r_to_m_8(registers1.D, *registers1.HL);
            break;
        case LD_HL_m_E:
            ld_r_to_m_8(registers1.E, *registers1.HL);
            break;
        case LD_HL_m_H:
            ld_r_to_m_8(registers1.H, *registers1.HL);
            break;
        case LD_HL_m_L:
            ld_r_to_m_8(registers1.L, *registers1.HL);
            break;
        case LD_HL_m_A:
            ld_r_to_m_8(registers1.A, *registers1.HL);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_to_m_8(uint8_t *reg, uint16_t address) {
    bus->write(address, reg);

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::inc_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    if(temp == 0xFF) registers1.flags->C = 1;
    else registers1.flags->C = 0;

    temp++;
    registers1.flags->N = 0;
    bus->write(address, &temp);

    if(temp == 0x00) registers1.flags->Z = 1;
    else registers1.flags->Z = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::dec_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    if(temp == 0xFF) registers1.flags->C = 1;
    else registers1.flags->C = 0;

    temp--;
    registers1.flags->N = 1;
    bus->write(address, &temp);

    if(temp == 0x00) registers1.flags->Z = 1;
    else registers1.flags->Z = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::ld_m_d8(uint16_t address) {
    uint8_t temp;
    bus->read((*registers1.PC) + 1, &temp);
    bus->write(address, &temp);

    cycles(3);
    (*registers1.PC) += 2;
}

void Cpu::ld_m_to_r_8(uint8_t *reg, uint16_t address) {
    bus->read(address, reg);

    cycles(2);
    (*registers1.PC) += 1;
}

bool Cpu::execute_jump(uint8_t instruction) {
    switch (instruction) {
        case JR:
            jr();
            break;
        case JR_Z:
            j_cond(flag_z, 1, true);
            break;
        case JR_C:
            j_cond(flag_c, 1, true);
            break;
        case JR_NZ:
            j_cond(flag_z, 0, true);
            break;
        case JR_NC:
            j_cond(flag_c, 0, true);
            break;
        case JP:
            jp();
            break;
        case JP_Z:
            j_cond(flag_z, 1, false);
            break;
        case JP_C:
            j_cond(flag_c, 1, false);
            break;
        case JP_NZ:
            j_cond(flag_z, 0, false);
            break;
        case JP_NC:
            j_cond(flag_c, 0, false);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::j_cond(flag flag, uint8_t set, bool relative) {
    switch (flag) {
        case flag_z:
            if(registers1.flags->Z == set) return relative ? jr() : jp();
            break;
        case flag_c:
            if(registers1.flags->C == set) return relative ? jr() : jp();
            break;
        case flag_h:
            if(registers1.flags->H == set) return relative ? jr() : jp();
            break;
        case flag_n:
            if(registers1.flags->N == set) return relative ? jr() : jp();
            break;
        default:
            printf("Unkown flag\n");
            exit(0);
    }
    cycles(2);
    if(relative) (*registers1.PC) += 2;
    else (*registers1.PC) += 3;
}

void Cpu::jr() {
    int8_t steps;
    bus->read((*registers1.PC) + 1, (uint8_t*) &steps);
    if((uint8_t)steps == 0xFE){
        printf("dio\n");
    }
    (*registers1.PC) += steps + 2;

    cycles(3);
}

void Cpu::jp() {
    uint16_t address;
    bus->read((*registers1.PC) + 1, (uint8_t*) &address);
    bus->read((*registers1.PC) + 2, ((uint8_t*) &address) + 1);
    (*registers1.PC) = address;

    cycles(3);
}
