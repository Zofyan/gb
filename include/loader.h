//
// Created by sofyan on 31-5-22.
//
#include <string>
#include "bus.h"

#ifndef GB_LOADER_H
#define GB_LOADER_H
class Loader {
    public:
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/04-op r,imm.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/05-op rp.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/01-special.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/09-op r,r.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";
        //char filename[100] = "../../gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb";


        //char filename[100] = "../../gb-test-roms/cpu_instrs/cpu_instrs.gb";
        //char filename[100] = "../Tetris.gb";
        //char filename[100] = "../Tennis (JUE) [!].gb";
        //char filename[100] = "../Dr. Mario (World).gb";
        //char filename[100] = "../Super Mario Land (World).gb";
        char filename[100] = "../Kirby's Dream Land (USA, Europe).gb";
        //char filename[100] = "../Donkey Kong Land (USA, Europe) (SGB Enhanced).gb";
        //char filename[100] = "../Pokemon Red.gb";

        Loader(Bus *bus);
};


#endif //GB_LOADER_H
