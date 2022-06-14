//
// Created by Sofyan Aarrass on 07/06/2022.
//

#include "../include/lcd.h"

Lcd::Lcd(uint16_t width1, uint16_t height1, SDL_Renderer *renderer1, Bus *bus1) {
    bus = bus1;
    renderer = renderer1;
    height = height1;
    width = width1;
    colors1[0] = rgba{250, 170, 170};
    colors1[1] = rgba{180, 110, 110};
    colors1[2] = rgba{120, 50, 50};
    colors1[3] = rgba{50, 0, 0};
    colors2[0] = rgba{255, 50, 50};
    colors2[1] = rgba{150, 50, 100};
    colors2[2] = rgba{100, 50, 150};
    colors2[3] = rgba{40, 50, 200};
}

void Lcd::write_pixel(uint16_t x, uint16_t y, uint8_t color, uint8_t palette, bool sprite) {
    //printf("DEBUG: pixel: x: %u, y: %u, color: %u\n", x, y, colors[color] * 60 + 60);
    //pthread_mutex_lock(&bus->lock);
    auto colors = palette ? colors2 : colors1;
    if(x >= 160 || y >= 144) return;
    if(sprite && color == 0) return;
        bus->pixels[((160 * y + x) * 4) + 3] = colors[color].blue;
        bus->pixels[((160 * y + x) * 4) + 2] = colors[color].green;
        bus->pixels[((160 * y + x) * 4) + 1] = colors[color].red;
        bus->pixels[((160 * y + x) * 4) + 0] = 0xFF;


    //pthread_mutex_unlock(&bus->lock);
    //printf("bus: %lu\n", bus->pixels.size());
}
