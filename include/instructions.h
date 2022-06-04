//
// Created by sofyan on 1-6-22.
//

#ifndef GB_INSTRUCTIONS_H
#define GB_INSTRUCTIONS_H

#include <cstdint>
#include "cpu.h"
#include "memory.h"

#define NOP 0x00
#define HALT 0x76

#define INC_B 0x04
#define INC_D 0x14
#define INC_H 0x24
#define INC_C 0x0C
#define INC_E 0x1C
#define INC_L 0x2C
#define INC_A 0x3C
#define DEC_B 0x05
#define DEC_D 0x15
#define DEC_H 0x25
#define DEC_C 0x0D
#define DEC_E 0x1D
#define DEC_L 0x2D
#define DEC_A 0x3D

#define INC_BC 0x03
#define INC_DE 0x13
#define INC_HL 0x23
#define INC_SP 0x33
#define DEC_BC 0x0B
#define DEC_DE 0x1B
#define DEC_HL 0x2B
#define DEC_SP 0x3B

#define LD_B_d8 0x06
#define LD_D_d8 0x16
#define LD_H_d8 0x26
#define LD_C_d8 0x0E
#define LD_A_d8 0x1E
#define LD_L_d8 0x2E
#define LD_E_d8 0x3E

#define LD_BC_d16 0x01
#define LD_DE_d16 0x11
#define LD_HL_d16 0x21
#define LD_SP_d16 0x31

#define LD_BC_m_A 0x02
#define LD_DE_m_A 0x12
#define LD_HL_mp_A 0x22
#define LD_HL_mm_A 0x32
#define INC_HL_m 0x34
#define DEC_HL_m 0x35
#define LD_HL_m_d8 0x36
#define LD_HL_m_B 0x70
#define LD_HL_m_C 0x71
#define LD_HL_m_D 0x72
#define LD_HL_m_E 0x73
#define LD_HL_m_H 0x74
#define LD_HL_m_L 0x75
#define LD_HL_m_A 0x77

#define LD_A_BC_m 0x0A
#define LD_A_DE_m 0x1A
#define LD_A_HL_mp 0x2A
#define LD_A_HL_mm 0x3A
#define LD_B_HL_m 0x46
#define LD_D_HL_m 0x56
#define LD_H_HL_m 0x66
#define LD_C_HL_m 0x4E
#define LD_E_HL_m 0x5E
#define LD_L_HL_m 0x6E
#define LD_A_HL_m 0x7E

#define JR_NZ 0x20
#define JR_NC 0x30
#define JR 0x18
#define JR_Z 0x28
#define JR_C 0x38
#define JP_NZ 0xC2
#define JP_NC 0xD2
#define JP 0xC3
#define JP_Z 0xCA
#define JP_C 0xDA


#endif //GB_INSTRUCTIONS_H
