//
// Created by Sofyan Aarrass on 07/06/2022.
//

#ifndef GB_SCREEN_H
#define GB_SCREEN_H

#include "SDL2/SDL.h"
#include <cstdint>



class Lcd {
public:
    Lcd(uint16_t width1, uint16_t height1);
    uint16_t width;
    uint16_t height;
    uint8_t colors[4];

    void write_pixel(uint16_t x, uint16_t y, uint8_t color);
};


#endif //GB_SCREEN_H
