//
// Created by sofyan on 31-5-22.
//

#include "../include/loader.h"

Loader::Loader(Bus *bus) {
    FILE *rom = fopen(filename, "r");

    fread(bus->get_buffer(ROM_0), 1, ROM_0_END - ROM_0 + 1, rom);

    printf("ROM type %02X\n", bus->read_v(0x0147));
    switch (bus->read_v(0x0148)) {
        case 00:
            bus->rom_size = 0x4000 * 2;
            break;
        case 01:
            bus->rom_size = 0x4000 * 4;
            break;
        case 02:
            bus->rom_size = 0x4000 * 8;
            break;
        case 03:
            bus->rom_size = 0x4000 * 16;
            break;
        case 04:
            bus->rom_size = 0x4000 * 32;
            break;
        case 05:
            bus->rom_size = 0x4000 * 64;
            break;
        case 06:
            bus->rom_size = 0x4000 * 128;
            break;
    }
    printf("ROM size %02X, %u\n", bus->read_v(0x0148), bus->rom_size);

    switch(bus->read_v(0x0147)){
        case 0x00:
            printf("read %04X to rom 0\n", fread(bus->roms[0], 1, ROM_N_END - ROM_N + 1, rom));
            break;
        case 0x02:
        case 0x03:
            bus->ram = true;
        case 0x01:
            for(int i = 0; i < 128; i++){
                printf("read %04X to rom %i\n", fread(bus->roms[i], 1, ROM_N_END - ROM_N + 1, rom), i);
            }
            break;
    }
}
