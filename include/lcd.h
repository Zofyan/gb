//
// Created by Sofyan Aarrass on 07/06/2022.
//

#ifndef GB_SCREEN_H
#define GB_SCREEN_H

#include "SDL2/SDL.h"
#include "bus.h"
#include <cstdint>


typedef struct rgba{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgba_t;



class Lcd {
public:
    Lcd(uint16_t width1, uint16_t height1, SDL_Renderer *renderer, Bus *bus);
    Bus *bus;
    uint16_t width;
    uint16_t height;
    SDL_Renderer *renderer;
    bool render = false;
    rgba_t colors1[4];
    rgba_t colors2[4];

    void write_pixel(uint16_t x, uint16_t y, uint8_t color);
    void write_sprite_pixel(uint16_t x, uint16_t y, uint8_t color, uint8_t pallete = 0);
};


#endif //GB_SCREEN_H
