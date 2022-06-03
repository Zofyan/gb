//
// Created by sofyan on 1-6-22.
//

#include <cstdio>
#include <cstdlib>
#include "../include/cpu.h"
#include "../include/instructions.h"
#include "../include/bus.h"

int init_registers(struct registers *registers){
    registers->AF = static_cast<uint16_t *>(calloc(2, 1));
    registers->BC = static_cast<uint16_t *>(calloc(2, 1));
    registers->DE = static_cast<uint16_t *>(calloc(2, 1));
    registers->HL = static_cast<uint16_t *>(calloc(2, 1));
    registers->SP = static_cast<uint16_t *>(calloc(2, 1));
    registers->PC = static_cast<uint16_t *>(calloc(2, 1));
    registers->A = reinterpret_cast<uint8_t *>(registers->AF) + 1;
    registers->B = reinterpret_cast<uint8_t *>(registers->BC) + 1;
    registers->C = reinterpret_cast<uint8_t *>(registers->BC);
    registers->D = reinterpret_cast<uint8_t *>(registers->DE) + 1;
    registers->E = reinterpret_cast<uint8_t *>(registers->DE);
    registers->H = reinterpret_cast<uint8_t *>(registers->HL) + 1;
    registers->L = reinterpret_cast<uint8_t *>(registers->HL);
    return 0;
}

int execute_instruction(struct registers *registers, bus *bus, uint8_t *buf){
    uint8_t inst;

    inst = buf[*registers->PC];

    if(load_instruction(inst, registers, bus, buf)) return 0;

    switch (inst) {
        case NOP:
            (*registers->PC)++;
            //printf("NOP\n");
            break;
        case RET:
            ret(registers);
            break;

        // INC instructions
        case INCB:
            inc8(registers->B, registers);
            break;
        case INCD:
            inc8(registers->D, registers);
            break;
        case INCH:
            inc8(registers->H, registers);
            break;
        case INCHL:
            inc8(BUS_OFFSET(bus, *registers->HL), registers, true, true);
            break;
        case INCC:
            inc8(registers->C, registers);
            break;
        case INCE:
            inc8(registers->E, registers);
            break;
        case INCL:
            inc8(registers->L, registers);
            break;
        case INCA:
            inc8(registers->A, registers);
            break;
        case DECB:
            inc8(registers->B, registers, false);
            break;
        case DECD:
            inc8(registers->D, registers, false);
            break;
        case DECH:
            inc8(registers->H, registers, false);
            break;
        case DECHL:
            inc8(BUS_OFFSET(bus, *registers->HL), registers, false, true);
            break;
        case DECC:
            inc8(registers->C, registers, false);
            break;
        case DECE:
            inc8(registers->E, registers, false);
            break;
        case DECL:
            inc8(registers->L, registers, false);
            break;
        case DECA:
            inc8(registers->A, registers, false);
            break;
        case INC_BC:
            inc16(registers->BC, registers, true);
            break;
        case INC_DE:
            inc16(registers->DE, registers, true);
            break;
        case INC_HL:
            inc16(registers->HL, registers, true);
            break;
        case INC_SP:
            inc16(registers->SP, registers, true);
            break;
        case DEC_BC:
            inc16(registers->BC, registers, false);
            break;
        case DEC_DE:
            inc16(registers->DE, registers, false);
            break;
        case DEC_HL:
            inc16(registers->HL, registers, false);
            break;
        case DEC_SP:
            inc16(registers->SP, registers, false);
            break;
        // JUMP instructions
        case JP:
            jp(registers, buf);
            break;
        case JR_NZ:
            jf(flag_z, false, true, registers, buf);
            break;
        case JR:
            jr(registers, buf);
            break;
        case JR_NC:
            jf(flag_c, false, true, registers, buf);
            break;
        case JR_C:
            jf(flag_c, true, true, registers, buf);
            break;
        case JR_Z:
            jf(flag_z, true, true, registers, buf);
            break;
        case JP_NZ:
            jf(flag_z, false, false, registers, buf);
            break;
        case JP_NC:
            jf(flag_c, false, false, registers, buf);
            break;
        case JP_C:
            jf(flag_c, true, false, registers, buf);
            break;
        case JP_Z:
            jf(flag_z, true, false, registers, buf);
            break;
        // LOAD instructions
            // LOAD memory instructions
            case LD_ABC:
                ldm2r8(registers->A, registers->BC, registers, buf);
                break;
            case LD_ADE:
                ldm2r8(registers->A, registers->DE, registers, buf);
                break;
            case LD_AHLP:
                ldm2r8(registers->A, registers->HL, registers, buf);
                (*BUS_OFFSET(bus, *registers->HL))++;
                break;
            case LD_AHLM:
                ldm2r8(registers->A, registers->HL, registers, buf);
                (*BUS_OFFSET(bus, *registers->HL))--;
                break;
            case LD_BCA:
                ldr2m8(registers->A, registers->BC, registers, buf);
                break;
            case LD_DEA:
                ldr2m8(registers->A, registers->DE, registers, buf);
                break;
            case LD_HLPA:
                ldr2m8(registers->A, registers->HL, registers, buf);
                (*BUS_OFFSET(bus, *registers->HL))++;
                break;
            case LD_HLMA:
                ldr2m8(registers->A, registers->HL, registers, buf);
                (*BUS_OFFSET(bus, *registers->HL))--;
                break;
            // LOAD register to register instructions

            // LOAD register instructions
            case LD_A8:
                ld8(registers->A, registers, buf);
                break;
            case LD_B8:
                ld8(registers->B, registers, buf);
                break;
            case LD_H8:
                ld8(registers->H, registers, buf);
                break;
            case LD_C8:
                ld8(registers->C, registers, buf);
                break;
            case LD_D8:
                ld8(registers->D, registers, buf);
                break;
            case LD_E8:
                ld8(registers->E, registers, buf);
                break;
            case LD_L8:
                ld8(registers->L, registers, buf);
                break;
            case LD_HL8:
                ldm8(registers->HL, registers, buf);
                break;
            case LD_HL16:
                ldm16(registers->HL, registers, buf);
                break;
            case LD_BC16:
                ld16(registers->BC, registers, buf);
                break;
            case LD_DE16:
                ld16(registers->DE, registers, buf);
                break;
            case LD_SP16:
                ld16(registers->SP, registers, buf);
                break;
        default:
            printf("whoops %02X\n", inst);
            break;
    }

    return 0;
}

int load_instruction(uint8_t inst, struct registers *registers, bus *bus, uint8_t *buf){
    if(inst == 0x76) return 0;
    uint8_t *regA;
    uint8_t *regB;
    switch (inst & 0xF8){
        case 0x40:
            regA = registers->B;
            break;
        case 0x48:
            regA = registers->C;
            break;
        case 0x50:
            regA = registers->D;
            break;
        case 0x58:
            regA = registers->E;
            break;
        case 0x60:
            regA = registers->H;
            break;
        case 0x68:
            regA = registers->L;
            break;
        case 0x70:
            switch (inst & 0x07){
                case 0x00:
                    regB = registers->B;
                    break;
                case 0x01:
                    regB = registers->C;
                    break;
                case 0x02:
                    regB = registers->D;
                    break;
                case 0x03:
                    regB = registers->E;
                    break;
                case 0x04:
                    regB = registers->H;
                    break;
                case 0x05:
                    regB = registers->L;
                    break;
                case 0x07:
                    regB = registers->A;
                    break;
                default:
                    return 0;
            }
            ldr2m8(regB, registers->HL, registers, buf);
            return 1;
        case 0x78:
            regA = registers->A;
            break;
        default:
            return 0;
    }
    switch (inst & 0x07){
        case 0x00:
            regB = registers->B;
            break;
        case 0x01:
            regB = registers->C;
            break;
        case 0x02:
            regB = registers->D;
            break;
        case 0x03:
            regB = registers->E;
            break;
        case 0x04:
            regB = registers->H;
            break;
        case 0x05:
            regB = registers->L;
            break;
        case 0x06:
            ldm2r8(regA, registers->HL, registers, buf);
            return 1;
        case 0x07:
            regB = registers->A;
            break;
        default:
            return 0;
    }
    ldr8(regA, regB, registers);
    return 1;
}