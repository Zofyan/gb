//
// Created by Sofyan Aarrass on 07/06/2022.
//

#include "../include/lcd.h"

Lcd::Lcd(uint16_t width1, uint16_t height1, SDL_Renderer *renderer1, Bus *bus1) {
    bus = bus1;
    renderer = renderer1;
    height = height1;
    width = width1;
    colors1[3] = rgba{40, 240, 40};
    colors1[2] = rgba{40, 180, 40};
    colors1[1] = rgba{40, 120, 40};
    colors1[0] = rgba{40, 60, 40};
    colors2[3] = rgba{40, 60, 40};
    colors2[2] = rgba{40, 180, 40};
    colors2[1] = rgba{40, 240, 40};
    colors2[0] = rgba{40, 120, 40};
}

void Lcd::write_pixel(uint16_t x, uint16_t y, uint8_t color) {
    auto colors = colors1;
    if(x >= 160 || y >= 144) return;

    bus->bg_window_pixels[((160 * y + x) * 4) + 3] = colors[color].blue;
    bus->bg_window_pixels[((160 * y + x) * 4) + 2] = colors[color].green;
    bus->bg_window_pixels[((160 * y + x) * 4) + 1] = colors[color].red;
    bus->bg_window_pixels[((160 * y + x) * 4) + 0] = 0xFF;
}

void Lcd::write_sprite_pixel(uint16_t x, uint16_t y, uint8_t color, uint8_t palette) {
    auto colors = palette ? colors2 : colors1;
    if(x >= 160 || y >= 144) return;
    if(color == 0) return;

    bus->sprite_pixels[((160 * y + x) * 4) + 3] = colors[color].blue;
    bus->sprite_pixels[((160 * y + x) * 4) + 2] = colors[color].green;
    bus->sprite_pixels[((160 * y + x) * 4) + 1] = colors[color].red;
    bus->sprite_pixels[((160 * y + x) * 4) + 0] = 0xFF;
}
