//
// Created by Sofyan Aarrass on 06/06/2022.
//

#include "SDL2/SDL.h"
#include "bus.h"
#include "fetcher.h"
#include "lcd.h"
#include "window_fetcher.h"

#define LY 0xFF44
#define LYC 0xFF45

#ifndef GB_PPU_H
#define GB_PPU_H
enum PPUState {
    HBlank,
    VBlank,
    OAMSearch,
    PixelTransfer
};

class Ppu {
private:
    SDL_Window *window;
    Bus *bus;
    PPUState state;
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t ticks;
    oam_t oams[40];
    bool oams_selection[40];
    Fetcher *fetcher;
    uint16_t ticks_pixeltransfer;
    WindowFetcher *window_fetcher;
    SDL_Renderer *renderer;

    void find_oams();
public:
    Lcd *lcd;
    uint8_t x_shift, y_shift;
    explicit Ppu(Bus *bus1, SDL_Renderer *renderer1, Lcd *lcd1);

    void start();

    void tick();

    void oamfetch();
    void pixeltransfer();
    void hblank();
    void vblank();

    void draw_line();

    uint8_t oamtransfer();
};


#endif //GB_PPU_H
