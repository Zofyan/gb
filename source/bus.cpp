//
// Created by sofyan on 2-6-22.
//

#include <cstring>
#include <cstdio>
#include "../include/bus.h"


int init_bus(bus *bus){
    memset(bus, 0, sizeof(struct bus));
}

int bus_read(bus *bus, uint16_t offset, uint8_t *buf, uint16_t size){
    if((offset & 0xFF00) == 0xFF00) {
        //printf("read %04X\n", offset);
    }
    memcpy(buf, (uint8_t*)bus + offset, size);
}

int bus_read_8b(bus *bus, uint16_t offset, uint8_t *buf){
    if((offset & 0xFF00) == 0xFF00) {
        //printf("read %04X\n", offset);
    }
    memcpy(buf, (uint8_t*)bus + offset, 1);
}

int bus_read_16b(bus *bus, uint16_t offset, uint16_t *buf){
    if((offset & 0xFF00) == 0xFF00) {
        //printf("read %04X\n", offset);
    }
    memcpy((uint8_t*)buf + 1, (uint8_t*)bus + offset + 1, 1);
    memcpy((uint8_t*)buf, (uint8_t*)bus + offset, 1);
}

int bus_write(bus *bus, uint16_t offset, uint8_t *buf, uint16_t size){
    //if((offset & 0xFF00) == 0xFF00) {
    //printf("write %04X\n", offset);
    //}
    memcpy( (uint8_t*)bus + offset, buf, size);
}

int bus_write_8b(bus *bus, uint16_t offset, uint8_t *buf){
    //if((offset & 0xFF00) == 0xFF00) {
    //printf("write %04X\n", offset);
    //}
    memcpy( (uint8_t*)bus + offset, buf, 1);
}


int bus_write_16b(bus *bus, uint16_t offset, uint16_t *buf){
    //if((offset & 0xFF00) == 0xFF00) {
    //printf("write %04X\n", offset);
    //}
    memcpy( (uint8_t*)bus + offset + 1, (uint8_t*)buf +1, 1);
    memcpy( (uint8_t*)bus + offset, buf, 1);
}
