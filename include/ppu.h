//
// Created by Sofyan Aarrass on 06/06/2022.
//

#include "SDL2/SDL.h"
#include "bus.h"
#include "fetcher.h"
#include "lcd.h"

#define LY 0xFF44
#define LYC 0xFF45

#ifndef GB_PPU_H
#define GB_PPU_H
enum PPUState {
    OAMSearch,
    PixelTransfer,
    HBlank,
    VBlank
};

class Ppu {
private:
    SDL_Window *window;
    Bus *bus;
    PPUState state;
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t ticks;
    Lcd *lcd;
    Fetcher *fetcher;
    SDL_Renderer *renderer;
public:
    explicit Ppu(Bus *bus1, SDL_Renderer *renderer1, Lcd *lcd1);

    void start();

    void tick();

    void oamfetch();
    void pixeltransfer();
    void hblank();
    void vblank();

    void draw_line();

};


#endif //GB_PPU_H
