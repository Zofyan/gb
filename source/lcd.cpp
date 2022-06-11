//
// Created by Sofyan Aarrass on 07/06/2022.
//

#include "../include/lcd.h"

Lcd::Lcd(uint16_t width1, uint16_t height1, SDL_Renderer *renderer1, Bus *bus1) {
    bus = bus1;
    renderer = renderer1;
    height = height1;
    width = width1;
    colors[0] = 0;
    colors[1] = 1;
    colors[2] = 2;
    colors[3] = 3;
}

void Lcd::write_pixel(uint16_t x, uint16_t y, uint8_t color) {
    //printf("DEBUG: pixel: x: %u, y: %u, color: %u\n", x, y, colors[color] * 60 + 60);
    bus->pixels.push(colors[color] * 60 + 60);
    bus->pixels.push(x);
    bus->pixels.push(y);
}
