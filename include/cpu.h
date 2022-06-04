//
// Created by sofyan on 1-6-22.
//

#ifndef GB_CPU_H
#define GB_CPU_H

#include <cstdint>
#include "memory.h"
#include "bus.h"


typedef struct flags_t{
    uint8_t X : 4;
    uint8_t C : 1;
    uint8_t H : 1;
    uint8_t N : 1;
    uint8_t Z : 1;
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
    uint8_t *F;
    flags_t *flags;
} registers;

class Cpu{
private:
    void execute_ld_r_to_r_8(uint8_t *regA, const uint8_t *regB);
    void inc_m(uint16_t address);
    void dec_m(uint16_t address);
    void inc_r_16(uint16_t *reg);
    void dec_r_16(uint16_t *reg);
    void inc_r_8(uint8_t *reg);
    void dec_r_8(uint8_t *reg);
    void ld_m_d8(uint16_t address);
    void ld_r_d8(uint8_t *reg);
    void ld_r_d16(uint16_t *reg);
    void ld_r_to_m_8(uint8_t *reg, uint16_t address);
    void ld_m_to_r_8(uint8_t *reg, uint16_t address);
    void jr();
    void jp();
    void j_cond(flag flag, uint8_t set, bool relative);

    static void cycles(uint8_t cycles);
    bool execute_jump(uint8_t instruction);
    bool execute_from_m(uint8_t instruction);
    bool execute_to_m(uint8_t instruction);
    bool execute_ld_r_d16(uint8_t instruction);
    bool execute_ld_r_d8(uint8_t instruction);
    bool execute_inc_r_16(uint8_t instruction);
    bool execute_dec_r_16(uint8_t instruction);
    bool execute_inc_r_8(uint8_t instruction);
    bool execute_dec_r_8(uint8_t instruction);
    bool execute_ld_r_to_r_8(uint8_t instruction);

public:
    registers registers{};
    Cpu(Bus *bus);
    Bus *bus;

    bool execute_next_instruction();
};


#endif //GB_CPU_H
