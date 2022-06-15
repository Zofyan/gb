//
// Created by sofyan on 1-6-22.
//

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <thread>
#include "../include/cpu.h"
#include "../include/instructions.h"
#include "../include/bus.h"

Cpu::Cpu(Bus *bus, Ppu *ppu) {
    registers1.AF = static_cast<uint16_t *>(calloc(2, 1));
    registers1.BC = static_cast<uint16_t *>(calloc(2, 1));
    registers1.DE = static_cast<uint16_t *>(calloc(2, 1));
    registers1.HL = static_cast<uint16_t *>(calloc(2, 1));
    registers1.SP = static_cast<uint16_t *>(calloc(2, 1));
    registers1.PC = static_cast<uint16_t *>(calloc(2, 1));
    registers1.A = reinterpret_cast<uint8_t *>(registers1.AF) + 1;
    registers1.F = reinterpret_cast<uint8_t *>(registers1.AF);
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

    (*registers1.PC) = 0x0100;
    (*registers1.SP) = HRAM_END;
    (*registers1.BC) = 0x0013;
    (*registers1.AF) = 0x01B0;
    (*registers1.DE) = 0x00D8;
    (*registers1.HL) = 0x014D;
    this->bus = bus;
    this->ppu = ppu;
}

void Cpu::cycles(uint8_t cycles) {
    for (uint8_t i = 0; i < cycles; i++) {
        ppu->tick();
        ppu->tick();
        ppu->tick();
        ppu->tick();
        bus->dma_tick();
        bus->timer->timer_div += 1;
        if (bus->timer->timer_tac.timer_enable) {
            uint16_t freq = 0;
            switch (bus->timer->timer_tac.timer_clock) {
                case 0b00:
                    freq = 1024;
                    break;
                case 0b01:
                    freq = 16;
                    break;
                case 0b10:
                    freq = 64;
                    break;
                case 0b11:
                    freq = 256;
                    break;
                default:
                    break;
            }
            if ((bus->timer->timer_div % (freq / 4)) == 0) {
                uint16_t overflow = bus->timer->timer_tima + 1;
                if (overflow > 0x00FF) {
                    bus->interrupt_request->timer = 1;
                    bus->timer->timer_tima = bus->timer->timer_tma;
                } else bus->timer->timer_tima = overflow;
            }
        }
        cycles_done++;
        if(cycles_done >= 17556){
            count++;
            if(count == 60) count = 0;
            timeval curTime{};
            gettimeofday(&curTime, NULL);
            auto end = curTime.tv_usec / 1000;
            if(4 > (end - start)){
                std::this_thread::sleep_for(std::chrono::milliseconds(4 - (end - start)));
            }
            cycles_done = 0;
            start = curTime.tv_usec / 1000;
        }
    }
}

bool Cpu::execute_next_instruction() {
    execute_interrupt();
    uint8_t instruction;
    bus->read_cpu(*registers1.PC, &instruction);

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
            cycles(1);
            break;
        case HALT:
            (*registers1.PC)++;
            while (!(*(uint8_t *) bus->interrupt_request)) { cycles(1); }
            break;
        case STOP:
            stop = true;
            //while (stop){}
            (*registers1.PC) += 2;
            cycles(1);
            break;
        default:
            printf("whoops %02X\n", instruction);
            return false;
    }
    return true;
}

bool Cpu::execute_16bit_ops(uint8_t instruction) {
    if (instruction != FLAG_16bit) return false;

    (*registers1.PC) += 1;
    cycles(1);
    bus->read_cpu(*registers1.PC, &instruction);

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
        case 0x30:
            reg != nullptr ? swap_r(reg) : swap_m(address);
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
    uint16_t addressA = 0, addressB = 0;
    bool addressAUse = false, addressBUse = false;
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
        case 0x70:
            addressA = *registers1.HL;
            addressAUse = true;
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
        case 0x06:
            addressB = *registers1.HL;
            addressBUse = true;
            break;
        case 0x07:
            regB = registers1.A;
            break;
        default:
            return false;
    }
    //printf("regA: %p, regB: %p, addressA: %04X, addressB: %04X, inst: %02X\n", regA, regB, addressA, addressB, instruction & 0xF8);
    if (addressAUse) ld_r_to_m_8(regB, addressA);
    else if (addressBUse) ld_m_to_r_8(regA, addressB);
    else ld_r_to_r_8(regA, regB);
    return true;
}

void Cpu::ld_r_to_r_8(uint8_t *regA, const uint8_t *regB) {
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
    registers1.flags->H = CARRY_4_ADD(*reg, 1);

    (*reg)++;

    registers1.flags->N = 0;

    registers1.flags->Z = *reg == 0x00;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::dec_r_8(uint8_t *reg) {
    uint8_t temp2;
    temp2 = *reg & 0x0F;

    registers1.flags->H = CARRY_4_SUB(*reg, 1);

    (*reg)--;

    registers1.flags->N = 1;

    registers1.flags->Z = *reg == 0x00;

    cycles(1);
    (*registers1.PC) += 1;
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
    bus->read_cpu((*registers1.PC) + 1, reg);

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
    bus->read_cpu((*registers1.PC) + 1, (uint8_t *) reg);
    bus->read_cpu((*registers1.PC) + 2, ((uint8_t *) reg) + 1);

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
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_to_m_8(uint8_t *reg, uint16_t address) {
    bus->write_cpu(address, reg);

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::inc_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);
    registers1.flags->H = CARRY_4_ADD(temp, 1);

    temp++;
    registers1.flags->N = 0;
    bus->write_cpu(address, &temp);

    registers1.flags->Z = temp == 0x00;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::dec_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);
    registers1.flags->H = CARRY_4_SUB(temp, 1);


    temp--;
    registers1.flags->N = 1;
    bus->write_cpu(address, &temp);

    registers1.flags->Z = temp == 0x00;;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::ld_m_d8(uint16_t address) {
    uint8_t temp;
    bus->read_cpu((*registers1.PC) + 1, &temp);
    bus->write_cpu(address, &temp);

    cycles(3);
    (*registers1.PC) += 2;
}

void Cpu::ld_m_to_r_8(uint8_t *reg, uint16_t address) {
    bus->read_cpu(address, reg);

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
            jp(nullptr);
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
        case JP_HL:
            jp(registers1.HL);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::j_cond(flag flag, uint8_t set, bool relative) {
    switch (flag) {
        case flag_z:
            if (registers1.flags->Z == set) return relative ? jr() : jp(nullptr);
            break;
        case flag_c:
            if (registers1.flags->C == set) return relative ? jr() : jp(nullptr);
            break;
        default:
            printf("Unknown flag\n");
            exit(0);
    }

    if (relative) {
        (*registers1.PC) += 2;
        cycles(2);
    }
    else {
        (*registers1.PC) += 3;
        cycles(3);
    }
}

void Cpu::jr() {
    int8_t steps;
    bus->read_cpu((*registers1.PC) + 1, (uint8_t *) &steps);

    (*registers1.PC) += steps + 2;

    cycles(3);
}

void Cpu::jp(uint16_t *reg) {
    if (reg != nullptr) {
        (*registers1.PC) = *reg;
        cycles(1);
        return;
    }
    uint16_t address;
    bus->read_cpu((*registers1.PC) + 1, (uint8_t *) &address);
    bus->read_cpu((*registers1.PC) + 2, ((uint8_t *) &address) + 1);
    (*registers1.PC) = address;

    cycles(3);
}

bool Cpu::execute_arithmetic(uint8_t instruction) {
    bool addCarry = (instruction & 0x08) == 0x08;
    uint8_t *reg = nullptr;
    uint16_t address = 0;
    bool addressUse = false;

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
            addressUse = true;
            break;
        case 0x07:
            reg = registers1.A;
            break;
        default:
            printf("invalid op\n");
            exit(1);
    }
    if (reg == nullptr && !addressUse) exit(12);

    switch (instruction & 0xF0) {
        case 0x80:
            !addressUse ? add_r(reg, addCarry) : add_m(address, addCarry);
            break;
        case 0x90:
            !addressUse ? sub_r(reg, addCarry) : sub_m(address, addCarry);
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::add_r(uint8_t *reg, bool addCarry) {

    uint8_t oldCarry = registers1.flags->C;

    registers1.flags->C = CARRY_8_ADD(*registers1.A, *reg) ||
                          CARRY_8_ADD(*registers1.A + *reg, addCarry ? registers1.flags->C : 0);
    registers1.flags->H = CARRY_4_ADD(*registers1.A, *reg) ||
                          CARRY_4_ADD(*registers1.A + *reg, addCarry ? oldCarry : 0);

    (*registers1.A) += *reg + (addCarry ? oldCarry : 0);

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::sub_r(uint8_t *reg, bool addCarry) {

    uint8_t oldCarry = registers1.flags->C;

    registers1.flags->C = CARRY_8_SUB(*registers1.A, *reg) ||
                          CARRY_8_SUB(*registers1.A - *reg, addCarry ? registers1.flags->C : 0);
    registers1.flags->H = CARRY_4_SUB(*registers1.A, *reg) ||
                          CARRY_4_SUB(*registers1.A - *reg, addCarry ? oldCarry : 0);

    (*registers1.A) -= (*reg + (addCarry ? oldCarry : 0));

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->N = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::cmp_r(const uint8_t *reg) {
    registers1.flags->C = CARRY_8_SUB(*registers1.A, *reg);
    registers1.flags->H = CARRY_4_SUB(*registers1.A, *reg);

    registers1.flags->Z = *registers1.A == *reg;
    registers1.flags->N = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::add_m(uint16_t address, bool addCarry) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    uint8_t oldCarry = registers1.flags->C;

    registers1.flags->C = CARRY_8_ADD(*registers1.A, temp) ||
                          CARRY_8_ADD(*registers1.A + temp, addCarry ? registers1.flags->C : 0);
    registers1.flags->H = CARRY_4_ADD(*registers1.A, temp) ||
                          CARRY_4_ADD(*registers1.A + temp, addCarry ? oldCarry : 0);

    (*registers1.A) += temp + (addCarry ? oldCarry : 0);

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::sub_m(uint16_t address, bool addCarry) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    uint8_t oldCarry = registers1.flags->C;

    registers1.flags->C = CARRY_8_SUB(*registers1.A, temp) ||
                          CARRY_8_SUB(*registers1.A - temp, addCarry ? registers1.flags->C : 0);
    registers1.flags->H = CARRY_4_SUB(*registers1.A, temp) ||
                          CARRY_4_SUB(*registers1.A - temp, addCarry ? oldCarry : 0);

    (*registers1.A) -= (temp + (addCarry ? oldCarry : 0));

    registers1.flags->Z = *registers1.A == 0x00 ? 1 : 0;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::cmp_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    registers1.flags->C = CARRY_8_SUB(*registers1.A, temp);
    registers1.flags->H = CARRY_4_SUB(*registers1.A, temp);

    registers1.flags->Z = *registers1.A == temp;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::add_d8(bool addCarry) {
    uint8_t temp;
    bus->read_cpu(*registers1.PC + 1, &temp);

    uint8_t oldCarry = registers1.flags->C;

    registers1.flags->C = CARRY_8_ADD(*registers1.A, temp) ||
                          CARRY_8_ADD(*registers1.A + temp, addCarry ? registers1.flags->C : 0);
    registers1.flags->H = CARRY_4_ADD(*registers1.A, temp) ||
                          CARRY_4_ADD(*registers1.A + temp, addCarry ? oldCarry : 0);

    (*registers1.A) += temp + (addCarry ? oldCarry : 0);

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::sub_d8(bool addCarry) {
    uint8_t temp;
    bus->read_cpu(*registers1.PC + 1, &temp);

    uint8_t oldCarry = registers1.flags->C;

    registers1.flags->C = CARRY_8_SUB(*registers1.A, temp) ||
                          CARRY_8_SUB(*registers1.A - temp, addCarry ? registers1.flags->C : 0);
    registers1.flags->H = CARRY_4_SUB(*registers1.A, temp) ||
                          CARRY_4_SUB(*registers1.A - temp, addCarry ? oldCarry : 0);

    (*registers1.A) -= (temp + (addCarry ? oldCarry : 0));

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->N = 1;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::cmp_d8() {
    uint8_t temp;
    bus->read_cpu(*registers1.PC + 1, &temp);

    registers1.flags->C = CARRY_8_SUB(*registers1.A, temp);
    registers1.flags->H = CARRY_4_SUB(*registers1.A, temp);

    registers1.flags->Z = *registers1.A == temp;
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
    bus->read_cpu(address, &temp);

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
    bus->read_cpu(*registers1.PC + 1, &temp);

    (*registers1.A) &= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 1;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::xor_r(uint8_t *reg) {
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
    bus->read_cpu(address, &temp);

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
    bus->read_cpu(*registers1.PC + 1, &temp);

    (*registers1.A) ^= temp;

    registers1.flags->Z = *registers1.A == 0x00;
    registers1.flags->H = 0;
    registers1.flags->C = 0;
    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 2;
}

void Cpu::or_r(const uint8_t *reg) {
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
    bus->read_cpu(address, &temp);

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
    bus->read_cpu(*registers1.PC + 1, &temp);

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
        case DAA:
            daa();
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
        case SCF:
            c_flag(true);
            break;
        case CCF:
            c_flag(false);
            break;
        case CPL:
            cpl();
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::enable_int() {
    ime = true;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::disable_int() {
    ime = false;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::ld_r_to_a16m() {
    uint16_t temp;
    bus->read_cpu(*registers1.PC + 1, (uint8_t *) (&temp));
    bus->read_cpu(*registers1.PC + 2, ((uint8_t *) &temp) + 1);

    bus->write_cpu(temp, registers1.A);

    cycles(4);
    (*registers1.PC) += 3;
}

void Cpu::ld_a16m_to_r() {
    uint16_t temp;
    bus->read_cpu(*registers1.PC + 1, (uint8_t *) (&temp));
    bus->read_cpu(*registers1.PC + 2, ((uint8_t *) &temp) + 1);

    bus->read_cpu(temp, registers1.A);

    cycles(4);
    (*registers1.PC) += 3;
}

void Cpu::ld_r_to_a8m() {
    uint8_t temp;
    bus->read_cpu(*registers1.PC + 1, &temp);

    bus->write_cpu(temp + IO_REGISTERS, registers1.A);

    cycles(3);
    (*registers1.PC) += 2;
}

void Cpu::ld_a8m_to_r() {
    uint8_t temp;
    bus->read_cpu(*registers1.PC + 1, &temp);

    bus->read_cpu(temp + IO_REGISTERS, registers1.A);

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
        case LD_a16_SP:
            ld_sp_to_a16();
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::ld_r_to_cm() {
    bus->write_cpu((*registers1.C) + IO_REGISTERS, registers1.A);

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::ld_cm_to_r() {
    bus->read_cpu((*registers1.C) + IO_REGISTERS, registers1.A);

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
            (*registers1.F) &= 0xF0;
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
        case LD_SP_HL:
            ld_hl_sp();
            break;
        case ADD_SP_s8:
            add_sp_s8();
            break;
        case LD_HL_SP_s8:
            ld_hl_sp_s8();
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::pop_r(uint16_t *reg) {
    (*reg) = bus->pop(registers1.SP);

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::push_r(const uint16_t *reg) {
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
        case RETI:
            reti();
            break;
        default:
            return false;
    }
    return true;
}

void Cpu::call() {
    uint16_t temp = *registers1.PC;
    (*registers1.PC) += 3;

    bus->push(*registers1.PC, registers1.SP);

    bus->read_cpu(temp + 1, ((uint8_t *) registers1.PC));
    bus->read_cpu(temp + 2, ((uint8_t *) registers1.PC) + 1);

    cycles(6);
}

void Cpu::rst(uint8_t n) {
    if (n > 7) {
        printf("Invalid reset\n");
        exit(3);
    }
    (*registers1.PC) += 1;

    bus->push(*registers1.PC, registers1.SP);

    (*registers1.PC) = 0x08 * n;

    cycles(4);
}

void Cpu::ret() {
    (*registers1.PC) = bus->pop(registers1.SP);

    cycles(4);
}

void Cpu::reti() {
    ime = true;

    (*registers1.PC) = bus->pop(registers1.SP);

    cycles(4);
}

void Cpu::call_cond(flag flag, uint8_t set) { // weird, flag checking should be working with ==...
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

void Cpu::ret_cond(flag flag, uint8_t set) {
    cycles(1);
    switch (flag) {
        case flag_z:
            if (registers1.flags->Z == set) return ret();
            break;
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

void Cpu::rlc_r(uint8_t *reg) {
    uint8_t carry = *reg >> 7;

    (*reg) <<= 1;

    (*reg) |= carry;

    registers1.flags->Z = *reg == 0x00;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rlc_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    uint8_t carry = temp >> 7;

    temp <<= 1;

    temp |= carry;

    bus->write_cpu(address, &temp);

    registers1.flags->Z = temp == 0x00;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::rrc_r(uint8_t *reg) {
    uint8_t carry = *reg << 7;

    (*reg) >>= 1;

    (*reg) |= carry;

    registers1.flags->Z = *reg == 0x00;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rrc_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    uint8_t carry = temp << 7;

    temp >>= 1;

    temp |= carry;

    bus->write_cpu(address, &temp);

    registers1.flags->Z = temp == 0x00;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::rl_r(uint8_t *reg) {
    uint8_t carry = *reg >> 7;
    (*reg) <<= 1;

    (*reg) |= registers1.flags->C;

    registers1.flags->Z = *reg == 0x00;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rl_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    uint8_t carry = temp >> 7;

    temp <<= 1;

    temp |= registers1.flags->C;

    bus->write_cpu(address, &temp);

    registers1.flags->Z = temp == 0x00;
    registers1.flags->C = carry;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::rr_r(uint8_t *reg) {
    uint8_t carry = *reg << 7;

    (*reg) = (*reg) >> 1;

    (*reg) |= registers1.flags->C << 7;

    registers1.flags->Z = *reg == 0x00;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::rr_m(uint16_t address) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    uint8_t carry = temp << 7;

    temp >>= 1;

    temp |= registers1.flags->C << 7;

    bus->write_cpu(address, &temp);

    registers1.flags->Z = temp == 0x00;
    registers1.flags->C = carry >> 7;
    registers1.flags->H = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::sla_r(uint8_t *reg) {
    rlc_r(reg);
    (*reg) &= 0xFE;
    registers1.flags->Z = *reg == 0x00;
}

void Cpu::sla_m(uint16_t address) {
    rlc_m(address);

    uint8_t temp;
    bus->read_cpu(address, &temp);
    temp &= 0xFE;
    registers1.flags->Z = temp == 0x00;
    bus->write_cpu(address, &temp);
}

void Cpu::srl_r(uint8_t *reg) {
    rrc_r(reg);
    (*reg) &= 0x7F;
    registers1.flags->Z = *reg == 0x00;
}

void Cpu::srl_m(uint16_t address) {
    rrc_m(address);

    uint8_t temp;
    bus->read_cpu(address, &temp);
    temp &= 0x7F;
    registers1.flags->Z = temp == 0x00;
    bus->write_cpu(address, &temp);
}

void Cpu::sra_r(uint8_t *reg) {
    uint8_t previous_state = (*reg) >> 7;
    rrc_r(reg);
    if (previous_state == 1) (*reg) |= 0x80;
    else (*reg) &= 0x7F;
    registers1.flags->Z = *reg == 0x00;
}

void Cpu::sra_m(uint16_t address) {
    uint8_t temp = bus->read_v(address);
    sra_r(&temp);
    bus->write_v(address, temp);
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

    registers1.flags->C = CARRY_16_ADD(*registers1.HL, *reg);
    registers1.flags->H = CARRY_WEIRD_ADD(*registers1.HL, *reg);

    (*registers1.HL) += *reg;

    registers1.flags->N = 0;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::swap_r(uint8_t *reg) {
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


void Cpu::swap_m(uint16_t address) {
    uint8_t temp2;
    bus->read_cpu(address, &temp2);

    uint8_t temp = temp2 >> 4;
    temp2 <<= 4;
    temp2 |= temp;

    bus->write_cpu(address, &temp2);

    registers1.flags->Z = temp2 == 0x00;
    registers1.flags->H = 0;
    registers1.flags->N = 0;
    registers1.flags->C = 0;

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::bit_r(uint8_t *reg, uint8_t n) {
    uint8_t temp = (*reg) & (0x1 << n);

    registers1.flags->Z = temp == 0x00;
    registers1.flags->N = 0;
    registers1.flags->H = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::bit_m(uint16_t address, uint8_t n) {
    uint8_t temp2;
    bus->read_cpu(address, &temp2);

    uint8_t temp = (temp2) & (0x1 << n);

    registers1.flags->Z = temp == 0x00;
    registers1.flags->N = 0;
    registers1.flags->H = 1;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::reset_r(uint8_t *reg, uint8_t n) {
    (*reg) &= ~(0x1 << n);

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::reset_m(uint16_t address, uint8_t n) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    temp &= ~(0x1 << n);

    bus->write_cpu(address, &temp);

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::set_r(uint8_t *reg, uint8_t n) {
    (*reg) |= 0x01 << n;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::set_m(uint16_t address, uint8_t n) {
    uint8_t temp;
    bus->read_cpu(address, &temp);

    temp |= 0x01 << n;

    bus->write_cpu(address, &temp);

    cycles(3);
    (*registers1.PC) += 1;
}

void Cpu::ld_hl_sp() {
    (*registers1.SP) = *registers1.HL;

    cycles(2);
    (*registers1.PC) += 1;
}

void Cpu::add_sp_s8() {
    int8_t temp;
    bus->read_cpu(*registers1.PC + 1, (uint8_t *) &temp);

    registers1.flags->C = CARRY_8_ADD(*registers1.SP, temp);
    registers1.flags->H = CARRY_4_ADD(*registers1.SP, temp);

    (*registers1.SP) += temp;

    registers1.flags->Z = 0;
    registers1.flags->N = 0;

    cycles(4);
    (*registers1.PC) += 2;
}

void Cpu::ld_hl_sp_s8() {
    int8_t temp;
    bus->read_cpu(*registers1.PC + 1, (uint8_t *) &temp);

    registers1.flags->C = CARRY_8_ADD(*registers1.SP, temp);
    registers1.flags->H = CARRY_4_ADD(*registers1.SP, temp);

    (*registers1.HL) = (*registers1.SP) + temp;

    registers1.flags->Z = 0;
    registers1.flags->N = 0;

    cycles(3);
    (*registers1.PC) += 2;
}

void Cpu::cpl() {

    (*registers1.A) = ~(*registers1.A);

    registers1.flags->N = 1;
    registers1.flags->H = 1;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::c_flag(bool set) {
    if (set) registers1.flags->C = 1;
    else registers1.flags->C = 1 - registers1.flags->C;

    registers1.flags->N = 0;
    registers1.flags->H = 0;

    cycles(1);
    (*registers1.PC) += 1;
}

void Cpu::ld_sp_to_a16() {
    uint16_t address;
    bus->read_cpu(*registers1.PC + 1, (uint8_t *) &address);
    bus->read_cpu(*registers1.PC + 2, ((uint8_t *) &address) + 1);

    bus->write_cpu(address + 1, ((uint8_t *) registers1.SP) + 1);
    bus->write_cpu(address, ((uint8_t *) registers1.SP));

    cycles(5);
    (*registers1.PC) += 3;
}

void Cpu::daa() {
    if (!registers1.flags->N) {
        if (registers1.flags->C || *registers1.A > 0x99) {
            *registers1.A += 0x60;
            registers1.flags->C = 1;
        }
        if (registers1.flags->H || (*registers1.A & 0x0f) > 0x09) {
            *registers1.A += 0x6;
        }
    } else {
        if (registers1.flags->C) {
            *registers1.A -= 0x60;
        }
        if (registers1.flags->H) {
            *registers1.A -= 0x6;
        }
    }

    registers1.flags->Z = *registers1.A == 0; // the usual z flag
    registers1.flags->H = 0;

    cycles(1);
    (*registers1.PC) +=1;
}

void Cpu::execute_interrupt() {
    if (ime) {
        //printf("request: %02X, enable: %02X\n", *(uint8_t*)&bus->interrupt_request, *(uint8_t*)&bus->interrupt_enable);
        if (bus->interrupt_request->vblank && bus->interrupt_enable->vblank) {
            bus->interrupt_request->vblank = 0;
            bus->push(*registers1.PC, registers1.SP);
            (*registers1.PC) = 0x40;
            ime = false;
            cycles(5);
        }
        if (bus->interrupt_request->lcd_stat && bus->interrupt_enable->lcd_stat) {
            bus->interrupt_request->lcd_stat = 0;
            bus->push(*registers1.PC, registers1.SP);
            (*registers1.PC) = 0x48;
            ime = false;
            cycles(5);
        }
        if (bus->interrupt_request->timer == 1 && bus->interrupt_enable->timer) {
            bus->interrupt_request->timer = 0;
            bus->push(*registers1.PC, registers1.SP);
            (*registers1.PC) = 0x50;
            ime = false;
            cycles(5);
        }
        if (bus->interrupt_request->serial && bus->interrupt_enable->serial) {
            bus->interrupt_request->serial = 0;
            bus->push(*registers1.PC, registers1.SP);
            (*registers1.PC) = 0x58;
            ime = false;
            cycles(5);
        }
        if (bus->interrupt_request->joypad && bus->interrupt_enable->joypad) {
            bus->interrupt_request->joypad = 0;
            bus->push(*registers1.PC, registers1.SP);
            (*registers1.PC) = 0x60;
            ime = false;
            cycles(5);
        }
    }
}