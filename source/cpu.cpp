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
    registers1.flags = (flags_t *) (registers1.AF);
    registers1.flags->Z = 0;
    registers1.flags->C = 0;
    registers1.flags->H = 0;
    registers1.flags->N = 0;
    registers1.flags->X = 0;

    (*registers1.PC) = 0x100;
    (*registers1.SP) = HRAM_END;
    (*registers1.BC) = 0x0013;
    (*registers1.AF) = 0x01B0;
    (*registers1.DE) = 0x00D8;
    this->bus = bus;
}

bool Cpu::execute_next_instruction() {
    uint8_t instruction;
    bus->read(*registers1.PC, &instruction);

    if (execute_ld_r_to_r_8(instruction)) return true;
    if (execute_inc_r_16(instruction)) return true;
    if (execute_dec_r_16(instruction)) return true;
    if (execute_inc_r_8(instruction)) return true;
    if (execute_dec_r_8(instruction)) return true;
    if (execute_ld_r_d8(instruction)) return true;
    if (execute_ld_r_d16(instruction)) return true;
    if (execute_from_m(instruction)) return true;
    if (execute_to_m(instruction)) return true;
    if (execute_jump(instruction)) return true;
    if (execute_arithmetic(instruction)) return true;
    if (execute_bitwise(instruction)) return true;
    if (execute_misc(instruction)) return true;
    if (execute_other_ld(instruction)) return true;
    if (execute_function(instruction)) return true;
    if (execute_stack(instruction)) return true;
    if (execute_16bit_ops(instruction)) return true;
    if (execute_add_r(instruction)) return true;

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

bool Cpu::execute_16bit_ops(uint8_t instruction) {
    if (instruction != FLAG_16bit) return false;

    (*registers1.PC) += 1;
    cycles(1);
    bus->read(*registers1.PC, &instruction);

    uint8_t *reg = nullptr;
    uint16_t address;

    switch (instruction & 0x07) {
        case 0x00:
            reg = registers1.B;
            break;
        case 0x01:
            reg = registers1.C;
            break;
        case 0x02:
            reg = registers1.D;
            break;
        case 0x03:
            reg = registers1.E;
            break;
        case 0x04:
            reg = registers1.H;
            break;
        case 0x05:
            reg = registers1.L;
            break;
        case 0x06:
            address = *registers1.HL;
            break;
        case 0x07:
            reg = registers1.A;
            break;
        default:
            printf("Invalid operation\n");
            exit(1);
    }

    switch (instruction & 0xF8) {
        case 0x00:
            reg != nullptr ? rlc_r(reg) : rlc_m(address);
            break;
        case 0x08:
            reg != nullptr ? rrc_r(reg) : rrc_m(address);
            break;
        case 0x10:
            reg != nullptr ? rl_r(reg) : rl_m(address);
            break;
        case 0x18:
            reg != nullptr ? rr_r(reg) : rr_m(address);
            break;
        case 0x20:
            reg != nullptr ? sla_r(reg) : sla_m(address);
            break;
        case 0x28:
            reg != nullptr ? sra_r(reg) : sra_m(address);
            break;
        case 0x38:
            reg != nullptr ? srl_r(reg) : srl_m(address);
            break;
        case 0x40:
            reg != nullptr ? bit_r(reg, 0) : bit_m(address, 0);
            break;
        case 0x48:
            reg != nullptr ? bit_r(reg, 1) : bit_m(address, 1);
            break;
        case 0x50:
            reg != nullptr ? bit_r(reg, 2) : bit_m(address, 2);
            break;
        case 0x58:
            reg != nullptr ? bit_r(reg, 3) : bit_m(address, 3);
            break;
        case 0x60:
            reg != nullptr ? bit_r(reg, 4) : bit_m(address, 4);
            break;
        case 0x68:
            reg != nullptr ? bit_r(reg, 5) : bit_m(address, 5);
            break;
        case 0x70:
            reg != nullptr ? bit_r(reg, 6) : bit_m(address, 6);
            break;
        case 0x78:
            reg != nullptr ? bit_r(reg, 7) : bit_m(address, 7);
            break;
        case 0x80:
            reg != nullptr ? reset_r(reg, 0) : reset_m(address, 0);
            break;
        case 0x88:
            reg != nullptr ? reset_r(reg, 1) : reset_m(address, 1);
            break;
        case 0x90:
            reg != nullptr ? reset_r(reg, 2) : reset_m(address, 2);
            break;
        case 0x98:
            reg != nullptr ? reset_r(reg, 3) : reset_m(address, 3);
            break;
        case 0xA0:
            reg != nullptr ? reset_r(reg, 4) : reset_m(address, 4);
            break;
        case 0xA8:
            reg != nullptr ? reset_r(reg, 5) : reset_m(address, 5);
            break;
        case 0xB0:
            reg != nullptr ? reset_r(reg, 6) : reset_m(address, 6);
            break;
        case 0xB8:
            reg != nullptr ? reset_r(reg, 7) : reset_m(address, 7);
            break;
        case 0xC0:
            reg != nullptr ? set_r(reg, 0) : set_m(address, 0);
            break;
        case 0xC8:
            reg != nullptr ? set_r(reg, 1) : set_m(address, 1);
            break;
        case 0xD0:
            reg != nullptr ? set_r(reg, 2) : set_m(address, 2);
            break;
        case 0xD8:
            reg != nullptr ? set_r(reg, 3) : set_m(address, 3);
            break;
        case 0xE0:
            reg != nullptr ? set_r(reg, 4) : set_m(address, 4);
            break;
        case 0xE8:
            reg != nullptr ? set_r(reg, 5) : set_m(address, 5);
            break;
        case 0xF0:
            reg != nullptr ? set_r(reg, 6) : set_m(address, 6);
            break;
        case 0xF8:
            reg != nullptr ? set_r(reg, 7) : set_m(address, 7);
            break;
        default:
            printf("super whoops %02X\n", instruction);
            exit(0);

    }

    return true;
}

bool Cpu::execute_ld_r_to_r_8(uint8_t instruction) {
    if (instruction == 0x76) return false;
    uint8_t *regA;
    uint8_t *regB;
    switch (instruction & 0xF8) {
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
    switch (instruction & 0x07) {
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
    (*reg)++;
    registers1.flags->N = 0;

    registers1.flags->Z = *reg == 0x00 ? 1 : 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::dec_r_8(uint8_t *reg) {
    (*reg)--;
    registers1.flags->N = 1;

    registers1.flags->Z = *reg == 0x00 ? 1 : 0;

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
    bus->read((*registers1.PC) + 1, (uint8_t *) reg);
    bus->read((*registers1.PC) + 2, ((uint8_t *) reg) + 1);

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
    switch (instruction) {
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
    uint8_t temp;
    bus->read(0xc000, &temp);
    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::inc_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    temp++;
    registers1.flags->N = 0;
    bus->write(address, &temp);

    registers1.flags->Z = temp == 0x00 ? 1 : 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::dec_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    temp--;
    registers1.flags->N = 1;
    bus->write(address, &temp);

    registers1.flags->Z = temp == 0x00 ? 1 : 0;

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
            if (registers1.flags->Z == set) return relative ? jr() : jp();
            break;
        case flag_c:
            if (registers1.flags->C == set) return relative ? jr() : jp();
            break;
        default:
            printf("Unknown flag\n");
            exit(0);
    }
    cycles(2);
    if (relative) (*registers1.PC) += 2;
    else (*registers1.PC) += 3;
}

void Cpu::jr() {
    int8_t steps;
    bus->read((*registers1.PC) + 1, (uint8_t *) &steps);
    if ((uint8_t) steps == 0xFE) {
        printf("dio\n");
    }
    (*registers1.PC) += steps + 2;

    cycles(3);
}

void Cpu::jp() {
    uint16_t address;
    bus->read((*registers1.PC) + 1, (uint8_t *) &address);
    bus->read((*registers1.PC) + 2, ((uint8_t *) &address) + 1);
    (*registers1.PC) = address;

    cycles(3);
}

bool Cpu::execute_arithmetic(uint8_t instruction) {
    bool addCarry = (instruction & 0x08) == 0x08;
    uint8_t *reg = nullptr;
    uint16_t address;

    switch (instruction) { //check for d8 operations
        case ADD_d8:
        case ADD_c_d8:
            add_d8(addCarry);
            return true;
        case SUB_d8:
        case SUB_c_d8:
            sub_d8(addCarry);
            return true;
        default:
            break;
    }

    switch (instruction & 07) {
        case 0x00:
            reg = registers1.B;
            break;
        case 0x01:
            reg = registers1.C;
            break;
        case 0x02:
            reg = registers1.D;
            break;
        case 0x03:
            reg = registers1.E;
            break;
        case 0x04:
            reg = registers1.H;
            break;
        case 0x05:
            reg = registers1.L;
            break;
        case 0x06:
            address = *registers1.HL;
            break;
        case 0x07:
            reg = registers1.A;
            break;
        default:
            printf("invalid op\n");
            exit(1);
    }

    switch (instruction & 0xF0) {
        case 0x80:
            reg != nullptr ? add_r(reg, addCarry) : add_m(address, addCarry);
            break;
        case 0x90:
            reg != nullptr ? sub_r(reg, addCarry) : sub_m(address, addCarry);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::add_r(uint8_t *reg, bool addCarry) {
    uint16_t carry;
    carry = *registers1.A;
    carry += *reg + (addCarry ? registers1.flags->C : 0);

    registers1.flags->C = carry > 0x00FF;

    (*registers1.A) += *reg + (addCarry ? registers1.flags->C : 0);

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::sub_r(uint8_t *reg, bool addCarry) {
    uint16_t carry;
    carry = *registers1.A;
    carry -= *reg - (addCarry ? registers1.flags->C : 0);

    registers1.flags->C = carry > 0x00FF;

    (*registers1.A) -= *reg - (addCarry ? registers1.flags->C : 0);

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::cmp_r(const uint8_t *reg) const {
    uint16_t carry;
    carry = *registers1.A;
    carry -= *reg;

    registers1.flags->C = carry > 0x00FF;

    registers1.flags->Z = *registers1.A == *reg ? 1 : 0;
    registers1.flags->N = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::add_m(uint16_t address, bool addCarry) {
    uint8_t temp;
    bus->read(address, &temp);

    uint16_t carry;
    carry = *registers1.A;
    carry += temp + (addCarry ? registers1.flags->C : 0);

    registers1.flags->C = carry > 0x00FF;

    (*registers1.A) += temp + (addCarry ? registers1.flags->C : 0);

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::sub_m(uint16_t address, bool addCarry) {
    uint8_t temp;
    bus->read(address, &temp);

    uint16_t carry;
    carry = *registers1.A;
    carry -= temp - (addCarry ? registers1.flags->C : 0);

    registers1.flags->C = carry > 0x00FF;

    (*registers1.A) -= temp - (addCarry ? registers1.flags->C : 0);

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::cmp_m(uint16_t address) const {
    uint8_t temp;
    bus->read(address, &temp);

    uint16_t carry;
    carry = *registers1.A;
    carry -= temp;

    registers1.flags->C = carry > 0x00FF;

    registers1.flags->Z = *registers1.A == temp ? 1 : 0;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::add_d8(bool addCarry) {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    uint16_t carry;
    carry = *registers1.A;
    carry += temp + (addCarry ? registers1.flags->C : 0);

    registers1.flags->C = carry > 0x00FF;

    (*registers1.A) += temp + (addCarry ? registers1.flags->C : 0);

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::sub_d8(bool addCarry) {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    uint16_t carry;
    carry = *registers1.A;
    carry -= temp - (addCarry ? registers1.flags->C : 0);

    registers1.flags->C = carry > 0x00FF;

    (*registers1.A) -= temp - (addCarry ? registers1.flags->C : 0);

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::cmp_d8() const {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    uint16_t carry;
    carry = *registers1.A;
    carry -= temp;

    registers1.flags->C = carry > 0x00FF;

    registers1.flags->Z = *registers1.A == temp ? 1 : 0;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 2;
}

bool Cpu::execute_bitwise(uint8_t instruction) {
    uint8_t *reg = nullptr;
    uint16_t address;

    switch (instruction) { //check for d8 operations
        case AND_d8:
            and_d8();
            return true;
        case XOR_d8:
            xor_d8();
            return true;
        case OR_d8:
            or_d8();
            return true;
        case CP_d8:
            cmp_d8();
            return true;
        default:
            break;
    }

    switch (instruction & 07) {
        case 0x00:
            reg = registers1.B;
            break;
        case 0x01:
            reg = registers1.C;
            break;
        case 0x02:
            reg = registers1.D;
            break;
        case 0x03:
            reg = registers1.E;
            break;
        case 0x04:
            reg = registers1.H;
            break;
        case 0x05:
            reg = registers1.L;
            break;
        case 0x06:
            address = *registers1.HL;
            break;
        case 0x07:
            reg = registers1.A;
            break;
        default:
            printf("invalid op\n");
            exit(1);
    }

    switch (instruction & 0xF8) {
        case 0xA0:
            reg != nullptr ? and_r(reg) : and_m(address);
            break;
        case 0xA8:
            reg != nullptr ? xor_r(reg) : xor_m(address);
            break;
        case 0xB0:
            reg != nullptr ? or_r(reg) : or_m(address);
            break;
        case 0xB8:
            reg != nullptr ? cmp_r(reg) : cmp_m(address);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::and_r(const uint8_t *reg) {
    (*registers1.A) &= *reg;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 1;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::and_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    (*registers1.A) &= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 1;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::and_d8() {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    (*registers1.A) &= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 1;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::xor_r(uint8_t *reg) const {
    (*registers1.A) ^= *reg;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::xor_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    (*registers1.A) ^= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::xor_d8() {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    (*registers1.A) ^= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::or_r(const uint8_t *reg) const {
    (*registers1.A) |= *reg;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::or_m(uint16_t address) {
    uint8_t temp;
    bus->read(address, &temp);

    (*registers1.A) |= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::or_d8() {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    (*registers1.A) |= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

bool Cpu::execute_misc(uint8_t instruction) {
    switch (instruction) {
        case DI:
            disable_int();
            break;
        case EI:
            enable_int();
            break;
        case RRA:
            rr_r(registers1.A);
            registers1.flags->Z = 0;
            break;
        case RLA:
            rl_r(registers1.A);
            registers1.flags->Z = 0;
            break;
        case RRCA:
            rrc_r(registers1.A);
            registers1.flags->Z = 0;
            break;
        case RLCA:
            rlc_r(registers1.A);
            registers1.flags->Z = 0;
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::enable_int() {
    uint8_t temp = 1;
    bus->write(INT_ENABLE, &temp);

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::disable_int() {
    uint8_t temp = 0;
    bus->write(INT_ENABLE, &temp);

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::ld_r_to_a16m() {
    uint16_t temp;
    bus->read(*registers1.PC + 1, (uint8_t *) (&temp));
    bus->read(*registers1.PC + 2, ((uint8_t *) &temp) + 1);

    bus->write(temp, registers1.A);

    cycles(4);
    (*registers1.PC) += 3;
}

void Cpu::ld_a16m_to_r() {
    uint16_t temp;
    bus->read(*registers1.PC + 1, (uint8_t *) (&temp));
    bus->read(*registers1.PC + 2, ((uint8_t *) &temp) + 1);

    bus->read(temp, registers1.A);

    cycles(4);
    (*registers1.PC) += 3;
}

void Cpu::ld_r_to_a8m() {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    bus->write(temp + IO_REGISTERS, registers1.A);

    cycles(3);
    (*registers1.PC) += 2;
}

void Cpu::ld_a8m_to_r() {
    uint8_t temp;
    bus->read(*registers1.PC + 1, &temp);

    bus->read(temp + IO_REGISTERS, registers1.A);

    cycles(3);
    (*registers1.PC) += 2;
}

bool Cpu::execute_other_ld(uint8_t instruction) {
    switch (instruction) {
        case LD_A_a16m:
            ld_r_to_a16m();
            break;
        case LD_a16m_A:
            ld_a16m_to_r();
            break;
        case LD_A_a8m:
            ld_r_to_a8m();
            break;
        case LD_a8m_A:
            ld_a8m_to_r();
            break;
        case LD_A_Cm:
            ld_r_to_cm();
            break;
        case LD_Cm_A:
            ld_cm_to_r();
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_to_cm() {
    bus->write((*registers1.C) + IO_REGISTERS, registers1.A);

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::ld_cm_to_r() {
    bus->read((*registers1.C) + IO_REGISTERS, registers1.A);

    cycles(2);
    (*registers1.PC) += 1;
}

bool Cpu::execute_stack(uint8_t instruction) {
    switch (instruction) {
        case PUSH_AF:
            push_r(registers1.AF);
            break;
        case PUSH_BC:
            push_r(registers1.BC);
            break;
        case PUSH_DE:
            push_r(registers1.DE);
            break;
        case PUSH_HL:
            push_r(registers1.HL);
            break;
        case POP_AF:
            pop_r(registers1.AF);
            break;
        case POP_BC:
            pop_r(registers1.BC);
            break;
        case POP_DE:
            pop_r(registers1.DE);
            break;
        case POP_HL:
            pop_r(registers1.HL);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::pop_r(uint16_t *reg) const {
    (*reg) = bus->pop(registers1.SP);

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::push_r(const uint16_t *reg) const {
    bus->push(*reg, registers1.SP);

    cycles(4);
    (*registers1.PC) += 1;
}

bool Cpu::execute_function(uint8_t instruction) {
    switch (instruction) {
        case RST0:
            rst(0);
            break;
        case RST1:
            rst(1);
            break;
        case RST2:
            rst(2);
            break;
        case RST3:
            rst(3);
            break;
        case RST4:
            rst(4);
            break;
        case RST5:
            rst(5);
            break;
        case RST6:
            rst(6);
            break;
        case RST7:
            rst(7);
            break;
        case CALL:
            call();
            break;
        case CALL_Z:
            call_cond(flag_z, 1);
            break;
        case CALL_C:
            call_cond(flag_c, 1);
            break;
        case CALL_NZ:
            call_cond(flag_z, 0);
            break;
        case CALL_NC:
            call_cond(flag_c, 0);
            break;
        case RET:
            ret();
            break;
        case RET_Z:
            ret_cond(flag_z, 1);
            break;
        case RET_C:
            ret_cond(flag_c, 1);
            break;
        case RET_NZ:
            ret_cond(flag_z, 0);
            break;
        case RET_NC:
            ret_cond(flag_c, 0);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::call() const {
    uint16_t temp = *registers1.PC;
    (*registers1.PC) += 3;

    bus->push(*registers1.PC, registers1.SP);

    bus->read(temp + 1, ((uint8_t *) registers1.PC));
    bus->read(temp + 2, ((uint8_t *) registers1.PC) + 1);

    cycles(6);
}

void Cpu::rst(uint8_t n) const {
    if (n > 7) {
        printf("Invalid reset\n");
        exit(3);
    }
    (*registers1.PC) += 1;

    bus->push(*registers1.PC, registers1.SP);

    bus->read(n * 0x08, ((uint8_t *) registers1.PC));
    bus->read(n * 0x08 + 1, ((uint8_t *) registers1.PC) + 1);

    cycles(4);
}

void Cpu::ret() const {
    (*registers1.PC) = bus->pop(registers1.SP);

    cycles(4);
}

void Cpu::call_cond(flag flag, uint8_t set) const { // weird, flag checking should be working with ==...
    switch (flag) {
        case flag_z:
            if (registers1.flags->Z == set) return call();
            break;
        case flag_c:
            if (registers1.flags->C == set) return call();
            break;
        default:
            printf("Unknown flag\n");
            exit(0);
    }
    cycles(3);
    (*registers1.PC) += 3;
}

void Cpu::ret_cond(flag flag, uint8_t set) const { // weird, flag checking should be working with ==...
    cycles(1);
    switch (flag) {
        case flag_z:
            if (registers1.flags->Z == set) return ret();
        case flag_c:
            if (registers1.flags->C == set) return ret();
            break;
        default:
            printf("Unknown flag\n");
            exit(0);
    }
    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rlc_r(uint8_t *reg) const {
    uint8_t carry = *reg >> 7;

    (*reg) <<= 1;

    (*reg) |= carry;

    registers1.flags->Z = *reg == 0x00 ? 1 : 0;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rlc_m(uint16_t address) const {
    uint8_t temp;
    bus->read(address, &temp);

    uint8_t carry = temp >> 7;

    temp <<= 1;

    temp |= carry;

    bus->write(address, &temp);

    registers1.flags->Z = temp == 0x00 ? 1 : 0;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::rrc_r(uint8_t *reg) const {
    uint8_t carry = *reg << 7;

    (*reg) >>= 1;

    (*reg) |= carry;

    registers1.flags->Z = *reg == 0x00 ? 1 : 0;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rrc_m(uint16_t address) const {
    uint8_t temp;
    bus->read(address, &temp);

    uint8_t carry = temp << 7;

    temp >>= 1;

    temp |= carry;

    bus->write(address, &temp);

    registers1.flags->Z = temp == 0x00 ? 1 : 0;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::rl_r(uint8_t *reg) const {
    uint8_t carry = *reg >> 7;
    (*reg) <<= 1;

    (*reg) |= registers1.flags->C;

    registers1.flags->Z = *reg == 0x00 ? 1 : 0;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rl_m(uint16_t address) const {
    uint8_t temp;
    bus->read(address, &temp);

    uint8_t carry = temp >> 7;

    temp <<= 1;

    temp |= registers1.flags->C;

    bus->write(address, &temp);

    registers1.flags->Z = temp == 0x00 ? 1 : 0;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::rr_r(uint8_t *reg) const {
    uint8_t carry = *reg << 7;

    (*reg) >>= 1;

    (*reg) |= registers1.flags->C << 7;

    registers1.flags->Z = *reg == 0x00 ? 1 : 0;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rr_m(uint16_t address) const {
    uint8_t temp;
    bus->read(address, &temp);

    uint8_t carry = temp << 7;

    temp >>= 1;

    temp |= registers1.flags->C << 7;

    bus->write(address, &temp);

    registers1.flags->Z = temp == 0x00 ? 1 : 0;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::sla_r(uint8_t *reg) const {
    rlc_r(reg);
    (*reg) &= 0xFE;
}

void Cpu::sla_m(uint16_t address) const {
    rlc_m(address);

    uint8_t temp;
    bus->read(address, &temp);
    temp &= 0xFE;
    bus->write(address, &temp);
}

void Cpu::srl_r(uint8_t *reg) const {
    rrc_r(reg);
    (*reg) &= 0x7F;
}

void Cpu::srl_m(uint16_t address) const {
    rrc_m(address);

    uint8_t temp;
    bus->read(address, &temp);
    temp &= 0x7F;
    bus->write(address, &temp);
}

void Cpu::sra_r(uint8_t *reg) const {
    uint8_t previous_state = (*reg) >> 7;
    rrc_r(reg);
    if (previous_state == 1) (*reg) |= 0x80;
    else (*reg) &= 0x7F;
}

void Cpu::sra_m(uint16_t address) const {
    rrc_m(address);

    uint8_t temp;
    bus->read(address, &temp);
    uint8_t previous_state = temp >> 7;
    if (previous_state == 1) temp |= 0x80;
    else temp &= 0x7F;
    bus->write(address, &temp);
}

bool Cpu::execute_add_r(uint8_t instruction) {
    switch (instruction) {
        case ADD_BC:
            add_r16(registers1.BC);
            break;
        case ADD_DE:
            add_r16(registers1.DE);
            break;
        case ADD_HL:
            add_r16(registers1.HL);
            break;
        case ADD_SP:
            add_r16(registers1.SP);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::add_r16(const uint16_t *reg) {
    uint32_t carry;
    carry = *registers1.HL;
    carry += *reg;

    registers1.flags->C = carry > 0x0000FFFF;

    (*registers1.HL) += *reg;

    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::swap_r(uint8_t *reg) const {
    uint8_t temp = (*reg) >> 4;
    (*reg) <<= 4;
    (*reg) |= temp;

    registers1.flags->Z = *reg == 0x00;
    registers1.flags->H = 0;
    registers1.flags->N = 0;
    registers1.flags->C = 0;

    cycles(1);
    (*registers1.PC) += 1;
}


void Cpu::swap_m(uint16_t address) const {
    uint8_t temp2;
    bus->read(address, &temp2);

    uint8_t temp = temp2 >> 4;
    temp2 <<= 4;
    temp2 |= temp;

    bus->write(address, &temp2);

    registers1.flags->Z = temp2 == 0x00;
    registers1.flags->H = 0;
    registers1.flags->N = 0;
    registers1.flags->C = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::bit_r(uint8_t *reg, uint8_t n) const {
    uint8_t temp = (*reg << (7 - n)) << 7;

    registers1.flags->Z = temp == 0x00;
    registers1.flags->N = 0;
    registers1.flags->H = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::bit_m(uint16_t address, uint8_t n) const {
    uint8_t temp2;
    bus->read(address, &temp2);

    uint8_t temp = (temp2 << (7 - n)) << 7;

    registers1.flags->Z = temp == 0x00;
    registers1.flags->N = 0;
    registers1.flags->H = 1;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::reset_r(uint8_t *reg, uint8_t n) const {
    (*reg) &= 0xFE << n;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::reset_m(uint16_t address, uint8_t n) const {
    uint8_t temp;
    bus->read(address, &temp);

    temp &= 0xFE << n;

    bus->write(address, &temp);

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::set_r(uint8_t *reg, uint8_t n) const {
    (*reg) |= 0x01 << n;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::set_m(uint16_t address, uint8_t n) const {
    uint8_t temp;
    bus->read(address, &temp);

    temp |= 0x01 << n;

    bus->write(address, &temp);

    cycles(3);
    (*registers1.PC) += 1;
}

