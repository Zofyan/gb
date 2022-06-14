//
// Created by sofyan on 31-5-22.
//

#include "../include/loader.h"

Loader::Loader(Bus *bus) {
    FILE *rom = fopen(filename, "r");

    fread(bus->roms[0], 1, ROM_0_END - ROM_0 + 1, rom);

    printf("ROM type %02X\n", bus->read_v(0x0147));
    switch (bus->read_v(0x0148)) {
        case 00:
            bus->rom_size = size_32KiB;
            break;
        case 01:
            bus->rom_size = size_64KiB;
            break;
        case 02:
            bus->rom_size = size_128KiB;
            break;
        case 03:
            bus->rom_size = size_256KiB;
            break;
        case 04:
            bus->rom_size = size_512KiB;
            break;
        case 05:
            bus->rom_size = size_1MiB;
            break;
        case 06:
            bus->rom_size = size_2MiB;
            break;
    }
    switch (bus->read_v(0x0149)) {
        case 00:
        case 01:
            bus->ram_size = 0;
            break;
        case 02:
            bus->ram_size = size_8KiB;
            break;
        case 03:
            bus->ram_size = size_32KiB;
            break;
        case 04:
            bus->ram_size = size_64KiB;
            break;
    }
    printf("ROM size %02X, %u\n", bus->read_v(0x0148), bus->rom_size);

    switch(bus->read_v(0x0147)){
        case 0x00:
            fread(bus->roms[1], 1, ROM_N_END - ROM_N + 1, rom);
            break;
        case 0x02:
        case 0x03:
            bus->eram_enable = true;
        case 0x01:
            bus->mbc = MBC1;
            for(int i = 1; i < 256; i++){
                uint16_t temp = fread(bus->roms[i], 1, ROM_N_END - ROM_N + 1, rom);
                //printf("read %04X to rom %i\n", temp, i);
                if(temp == 0) break;
            }
            break;
        case 0x13:
            bus->mbc = MBC3;
            for(int i = 1; i < 256; i++){
                uint16_t temp = fread(bus->roms[i], 1, ROM_N_END - ROM_N + 1, rom);
                //printf("read %04X to rom %i\n", temp, i);
                if(temp == 0) break;
            }
            break;
    }
}
