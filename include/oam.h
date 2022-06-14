//
// Created by Sofyan Aarrass on 06/06/2022.
//

#ifndef GB_OAM_H
#define GB_OAM_H


#include <cstdint>

typedef struct oam{
    uint8_t position_y;
    uint8_t position_x;
    uint8_t tile_index;
    uint8_t cgb_unused : 4;
    uint8_t palette : 1;
    uint8_t flip_x : 1;
    uint8_t flip_y : 1;
    uint8_t priority : 1;
} oam_t;


#endif //GB_OAM_H
