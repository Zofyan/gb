//
// Created by sofyan on 1-6-22.
//

#ifndef GB_CPU_H
#define GB_CPU_H

#include <cstdint>
#include "memory.h"
#include "bus.h"
#include "ppu.h"


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

typedef struct registers_t{
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
    flags_t *flags;
} registers_t;

class Cpu{
private:
    void ld_r_to_r_8(uint8_t *regA, uint8_t *regB);
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
    void jp(uint16_t *reg);
    void j_cond(flag flag, uint8_t set, bool relative);
    void add_r(uint8_t *reg, bool addCarry);
    void add_m(uint16_t address, bool addCarry);
    void add_d8(bool addCarry);
    void sub_r(uint8_t *reg, bool addCarry);
    void sub_m(uint16_t address, bool addCarry);
    void sub_d8(bool addCarry);
    void and_r(const uint8_t *reg);
    void and_m(uint16_t address);
    void and_d8();
    void xor_r(uint8_t *reg);
    void xor_m(uint16_t address);
    void xor_d8();
    void or_r(const uint8_t *reg);
    void or_m(uint16_t address);
    void or_d8();
    void cmp_r(const uint8_t *reg);
    void cmp_m(uint16_t address);
    void cmp_d8();
    void disable_int();
    void enable_int();
    void ld_r_to_a16m();
    void ld_a16m_to_r();
    void ld_r_to_a8m();
    void ld_a8m_to_r();
    void ld_r_to_cm();
    void ld_cm_to_r();
    void pop_r(uint16_t *reg);
    void push_r(const uint16_t *reg);
    void add_r16(const uint16_t *reg);
    void call();
    void rst(uint8_t n);
    void ret();
    void call_cond(flag flag, uint8_t set);
    void ret_cond(flag flag, uint8_t set);
    void rlc_r(uint8_t *reg);
    void rlc_m(uint16_t address);
    void rrc_r(uint8_t *reg);
    void rrc_m(uint16_t address);
    void rl_r(uint8_t *reg);
    void rl_m(uint16_t address);
    void rr_r(uint8_t *reg);
    void rr_m(uint16_t address);
    void sla_r(uint8_t *reg);
    void sla_m(uint16_t address);
    void sra_r(uint8_t *reg);
    void sra_m(uint16_t address);
    void srl_r(uint8_t *reg);
    void srl_m(uint16_t address);
    void swap_r(uint8_t *reg);
    void swap_m(uint16_t address);
    void bit_r(uint8_t *reg, uint8_t n);
    void bit_m(uint16_t address, uint8_t n);
    void reset_r(uint8_t *reg, uint8_t n);
    void reset_m(uint16_t address, uint8_t n);
    void set_r(uint8_t *reg, uint8_t n);
    void set_m(uint16_t address, uint8_t n);
    void ld_hl_sp();
    void add_sp_s8();
    void ld_hl_sp_s8();
    void c_flag(bool set);
    void flip_a();
    void ld_sp_to_a16();
    void reti();

    void cycles(uint8_t cycles);
    bool execute_add_r(uint8_t instruction);
    bool execute_16bit_ops(uint8_t instruction);
    bool execute_function(uint8_t instruction);
    bool execute_stack(uint8_t instruction);
    bool execute_other_ld(uint8_t instruction);
    bool execute_misc(uint8_t instruction);
    bool execute_arithmetic(uint8_t instruction);
    bool execute_bitwise(uint8_t instruction);
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

    Ppu *ppu;

public:
    uint64_t start;
    uint64_t cycles_done = 0;
    uint32_t count = 0;
    bool ime = true;
    registers_t registers1{};
    Cpu(Bus *bus, Ppu *ppu);
    Bus *bus;

    bool execute_next_instruction();

    bool execute_frame();
};


#endif //GB_CPU_H
