//
// Created by sofyan on 1-6-22.
//

#include <cstdio>
#include <cstdlib>
#include "../include/cpu.h"
#include "../include/instructions.h"
#include "../include/bus.h"

Cpu::Cpu(Bus *bus) {
    registers.AF = static_cast<uint16_t *>(calloc(2, 1));
    registers.BC = static_cast<uint16_t *>(calloc(2, 1));
    registers.DE = static_cast<uint16_t *>(calloc(2, 1));
    registers.HL = static_cast<uint16_t *>(calloc(2, 1));
    registers.SP = static_cast<uint16_t *>(calloc(2, 1));
    registers.PC = static_cast<uint16_t *>(calloc(2, 1));
    registers.A = reinterpret_cast<uint8_t *>(registers.AF) + 1;
    registers.B = reinterpret_cast<uint8_t *>(registers.BC) + 1;
    registers.C = reinterpret_cast<uint8_t *>(registers.BC);
    registers.D = reinterpret_cast<uint8_t *>(registers.DE) + 1;
    registers.E = reinterpret_cast<uint8_t *>(registers.DE);
    registers.H = reinterpret_cast<uint8_t *>(registers.HL) + 1;
    registers.L = reinterpret_cast<uint8_t *>(registers.HL);
    registers.flags = (flags_t*) (registers.AF);
    registers.flags->Z = 0;
    registers.flags->C = 0;
    registers.flags->H = 0;
    registers.flags->N = 0;
    registers.flags->X = 0;

    (*registers.PC) = 0x100;
    this->bus = bus;
}

bool Cpu::execute_next_instruction() {
    uint8_t instruction;
    bus->read(*registers.PC, &instruction);

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
            (*registers.PC)++;
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
            regA = registers.B;
            break;
        case 0x48:
            regA = registers.C;
            break;
        case 0x50:
            regA = registers.D;
            break;
        case 0x58:
            regA = registers.E;
            break;
        case 0x60:
            regA = registers.H;
            break;
        case 0x68:
            regA = registers.L;
            break;
        case 0x78:
            regA = registers.A;
            break;
        default:
            return false;
    }
    switch (instruction & 0x07){
        case 0x00:
            regB = registers.B;
            break;
        case 0x01:
            regB = registers.C;
            break;
        case 0x02:
            regB = registers.D;
            break;
        case 0x03:
            regB = registers.E;
            break;
        case 0x04:
            regB = registers.H;
            break;
        case 0x05:
            regB = registers.L;
            break;
        case 0x07:
            regB = registers.A;
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
    (*registers.PC) += 1;
}

void Cpu::inc_r_16(uint16_t *reg) {
    (*reg)++;

    cycles(2);
    (*registers.PC) += 1;
}

void Cpu::dec_r_16(uint16_t *reg) {
    (*reg)--;

    cycles(2);
    (*registers.PC) += 1;
}


void Cpu::inc_r_8(uint8_t *reg) {
    if((*reg) == 0xFF) registers.flags->C = 1;
    else registers.flags->C = 0;

    (*reg)++;
    registers.flags->N = 0;

    if(*reg == 0x00) registers.flags->Z = 1;
    else registers.flags->Z = 0;

    cycles(1);
    (*registers.PC) += 1;
}

void Cpu::dec_r_8(uint8_t *reg) {
    if((*reg) == 0x00) registers.flags->C = 1;
    else registers.flags->C = 0;

    (*reg)--;
    registers.flags->N = 1;

    if(*reg == 0x00) registers.flags->Z = 1;
    else registers.flags->Z = 0;

    cycles(1);
    (*registers.PC) += 1;
}

void Cpu::cycles(uint8_t cycles) {

}

bool Cpu::execute_inc_r_16(uint8_t instruction) {
    switch (instruction) {
        case INC_BC:
            inc_r_16(registers.BC);
            break;
        case INC_HL:
            inc_r_16(registers.HL);
            break;
        case INC_DE:
            inc_r_16(registers.DE);
            break;
        case INC_SP:
            inc_r_16(registers.SP);
            break;
        default:
            return false;
    }
    return true;
}
bool Cpu::execute_dec_r_16(uint8_t instruction) {
    switch (instruction) {
        case DEC_BC:
            dec_r_16(registers.BC);
            break;
        case DEC_HL:
            dec_r_16(registers.HL);
            break;
        case DEC_DE:
            dec_r_16(registers.DE);
            break;
        case DEC_SP:
            dec_r_16(registers.SP);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_inc_r_8(uint8_t instruction) {
    switch (instruction) {
        case INC_A:
            inc_r_8(registers.A);
            break;
        case INC_B:
            inc_r_8(registers.B);
            break;
        case INC_C:
            inc_r_8(registers.C);
            break;
        case INC_D:
            inc_r_8(registers.D);
            break;
        case INC_E:
            inc_r_8(registers.E);
            break;
        case INC_H:
            inc_r_8(registers.H);
            break;
        case INC_L:
            inc_r_8(registers.L);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_dec_r_8(uint8_t instruction) {
    switch (instruction) {
        case DEC_A:
            dec_r_8(registers.A);
            break;
        case DEC_B:
            dec_r_8(registers.B);
            break;
        case DEC_C:
            dec_r_8(registers.C);
            break;
        case DEC_D:
            dec_r_8(registers.D);
            break;
        case DEC_E:
            dec_r_8(registers.E);
            break;
        case DEC_H:
            dec_r_8(registers.H);
            break;
        case DEC_L:
            dec_r_8(registers.L);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_ld_r_d8(uint8_t instruction) {
    switch (instruction) {
        case LD_A_d8:
            ld_r_d8(registers.A);
            break;
        case LD_B_d8:
            ld_r_d8(registers.B);
            break;
        case LD_C_d8:
            ld_r_d8(registers.C);
            break;
        case LD_D_d8:
            ld_r_d8(registers.D);
            break;
        case LD_E_d8:
            ld_r_d8(registers.E);
            break;
        case LD_H_d8:
            ld_r_d8(registers.H);
            break;
        case LD_L_d8:
            ld_r_d8(registers.L);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_d8(uint8_t *reg) {
    bus->read((*registers.PC) + 1, reg);

    cycles(2);
    (*registers.PC) += 2;
}

bool Cpu::execute_ld_r_d16(uint8_t instruction) {
    switch (instruction) {
        case LD_BC_d16:
            ld_r_d16(registers.BC);
            break;
        case LD_DE_d16:
            ld_r_d16(registers.DE);
            break;
        case LD_HL_d16:
            ld_r_d16(registers.HL);
            break;
        case LD_SP_d16:
            ld_r_d16(registers.SP);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_d16(uint16_t *reg) {
    bus->read((*registers.PC) + 1, (uint8_t*)reg);
    bus->read((*registers.PC) + 2, ((uint8_t*)reg) + 1);

    cycles(3);
    (*registers.PC) += 3;
}

bool Cpu::execute_from_m(uint8_t instruction) {
    switch (instruction) {
        case LD_A_BC_m:
            ld_m_to_r_8(registers.A, *registers.BC);
            break;
        case LD_A_DE_m:
            ld_m_to_r_8(registers.A, *registers.DE);
            break;
        case LD_A_HL_mp:
            ld_m_to_r_8(registers.A, *registers.HL);
            (*registers.HL)++;
            break;
        case LD_A_HL_mm:
            ld_m_to_r_8(registers.A, *registers.HL);
            (*registers.HL)--;
            break;
        case LD_B_HL_m:
            ld_m_to_r_8(registers.B, *registers.HL);
            break;
        case LD_C_HL_m:
            ld_m_to_r_8(registers.C, *registers.HL);
            break;
        case LD_D_HL_m:
            ld_m_to_r_8(registers.D, *registers.HL);
            break;
        case LD_E_HL_m:
            ld_m_to_r_8(registers.E, *registers.HL);
            break;
        case LD_H_HL_m:
            ld_m_to_r_8(registers.H, *registers.HL);
            break;
        case LD_L_HL_m:
            ld_m_to_r_8(registers.L, *registers.HL);
            break;
        case LD_A_HL_m:
            ld_m_to_r_8(registers.A, *registers.HL);
            break;
        default:
            return false;
    }
    return true;
}

bool Cpu::execute_to_m(uint8_t instruction) {
    switch(instruction){
        case LD_BC_m_A:
            ld_r_to_m_8(registers.A, *registers.BC);
            break;
        case LD_DE_m_A:
            ld_r_to_m_8(registers.A, *registers.DE);
            break;
        case LD_HL_mp_A:
            ld_r_to_m_8(registers.A, *registers.HL);
            (*registers.HL)++;
            break;
        case LD_HL_mm_A:
            ld_r_to_m_8(registers.A, *registers.HL);
            (*registers.HL)--;
            break;
        case INC_HL_m:
            inc_m(*registers.HL);
            break;
        case DEC_HL_m:
            dec_m(*registers.HL);
            break;
        case LD_HL_m_d8:
            ld_m_d8(*registers.HL);
            break;
        case LD_HL_m_B:
            ld_r_to_m_8(registers.B, *registers.HL);
            break;
        case LD_HL_m_C:
            ld_r_to_m_8(registers.C, *registers.HL);
            break;
        case LD_HL_m_D:
            ld_r_to_m_8(registers.D, *registers.HL);
            break;
        case LD_HL_m_E:
            ld_r_to_m_8(registers.E, *registers.HL);
            break;
        case LD_HL_m_H:
            ld_r_to_m_8(registers.H, *registers.HL);
            break;
        case LD_HL_m_L:
            ld_r_to_m_8(registers.L, *registers.HL);
            break;
        case LD_HL_m_A:
            ld_r_to_m_8(registers.A, *registers.HL);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_to_m_8(uint8_t *reg, uint16_t address) {
    bus->write(address, reg);

    cycles(2);
    (*registers.PC) += 1;
}

void Cpu::inc_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    if(temp == 0xFF) registers.flags->C = 1;
    else registers.flags->C = 0;

    temp++;
    registers.flags->N = 0;
    bus->write(address, &temp);

    if(temp == 0x00) registers.flags->Z = 1;
    else registers.flags->Z = 0;

    cycles(3);
    (*registers.PC) += 1;
}

void Cpu::dec_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    if(temp == 0xFF) registers.flags->C = 1;
    else registers.flags->C = 0;

    temp--;
    registers.flags->N = 1;
    bus->write(address, &temp);

    if(temp == 0x00) registers.flags->Z = 1;
    else registers.flags->Z = 0;

    cycles(3);
    (*registers.PC) += 1;
}

void Cpu::ld_m_d8(uint16_t address) {
    uint8_t temp;
    bus->read((*registers.PC) + 1, &temp);
    bus->write(address, &temp);

    cycles(3);
    (*registers.PC) += 2;
}

void Cpu::ld_m_to_r_8(uint8_t *reg, uint16_t address) {
    bus->read(address, reg);

    cycles(2);
    (*registers.PC) += 1;
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
            if(registers.flags->Z == set) return relative ? jr() : jp();
            break;
        case flag_c:
            if(registers.flags->C == set) return relative ? jr() : jp();
            break;
        case flag_h:
            if(registers.flags->H == set) return relative ? jr() : jp();
            break;
        case flag_n:
            if(registers.flags->N == set) return relative ? jr() : jp();
            break;
        default:
            printf("Unkown flag\n");
            exit(0);
    }
    cycles(2);
    if(relative) (*registers.PC) += 2;
    else (*registers.PC) += 3;
}

void Cpu::jr() {
    int8_t steps;
    bus->read((*registers.PC) + 1, (uint8_t*) &steps);
    if((uint8_t)steps == 0xFE){
        printf("dio\n");
    }
    (*registers.PC) += steps + 2;

    cycles(3);
}

void Cpu::jp() {
    uint16_t address;
    bus->read((*registers.PC) + 1, (uint8_t*) &address);
    bus->read((*registers.PC) + 2, ((uint8_t*) &address) + 1);
    (*registers.PC) = address;

    cycles(3);
}
