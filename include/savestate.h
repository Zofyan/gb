//
// Created by sofyan on 17-6-22.
//

#ifndef GB_SAVESTATE_H
#define GB_SAVESTATE_H


#include "cpu.h"

class SaveState {
private:
    Cpu *cpu;
public:
    explicit SaveState(Cpu *cpu1);
    void save();
    void load(char *filename);
};


#endif //GB_SAVESTATE_H
