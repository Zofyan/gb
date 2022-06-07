//
// Created by Sofyan Aarrass on 06/06/2022.
//

#include "../include/ppu.h"

Ppu::Ppu(Bus *bus1, SDL_Renderer *renderer1) {
    bus = bus1;
    renderer = renderer1;
    ticks = 0;
    state = OAMSearch;
    fetcher = new Fetcher(VRAM, 0, bus);
}

void Ppu::start() {

}

void Ppu::draw_line() {

}

void Ppu::tick() {
    switch (state) {
        case OAMSearch:
            oamfetch();
            break;
        case PixelTransfer:
            pixeltransfer();
            break;
        case HBlank:
            hblank();
            break;
        case VBlank:
            vblank();
            break;
        default:
            exit(6);
    }
}

void Ppu::oamfetch() {
    if (ticks == 40) {
        state = PixelTransfer;
        ticks = 0;
    } else {
        ticks++;
    }
}


void Ppu::pixeltransfer() {
    if (lx == 160) {
        state = HBlank;
        ticks = 0;
        lx = 0;
    } else {
        fetcher->tick();

        uint8_t pixel;
        if (!fetcher->fifo.empty()) {
            pixel = fetcher->fifo.front();
            fetcher->fifo.pop();
            if (pixel != 0x00) {
                uint8_t x = 0;
            }
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawPoint(renderer, lx, ly);
            lx++;
        }

        ticks++;
    }
}


void Ppu::hblank() {
    if (ticks == 456) {
        ticks = 0;

        bus->write_v(LY, bus->read_v(LY) + 1);
        if (bus->read_v(LY) == 144) {
            state = VBlank;
        } else {
            state = OAMSearch;
        }
    } else {
        ticks++;
    }

}


void Ppu::vblank() {
    if (ticks == 456) {
        ticks = 0;
        bus->write_v(LY, bus->read_v(LY) + 1);
        if (bus->read_v(LY) == 153) {
            bus->write_v(LY, 0);
            state = OAMSearch;
        }
    } else {
        ticks++;
    }
}
