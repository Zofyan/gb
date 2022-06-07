//
// Created by Sofyan Aarrass on 06/06/2022.
//

#include "SDL2/SDL.h"
#include "bus.h"
#include "fetcher.h"

#define LY 0xFF44

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
    uint16_t lx;
    uint16_t ly;
    uint16_t ticks;
    Fetcher *fetcher;
    SDL_Renderer *renderer;
public:
    explicit Ppu(Bus *bus, SDL_Renderer *renderer);

    void start();

    void tick();

    void oamfetch();
    void pixeltransfer();
    void hblank();
    void vblank();

    void draw_line();

};


#endif //GB_PPU_H
