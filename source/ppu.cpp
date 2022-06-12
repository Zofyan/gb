//
// Created by Sofyan Aarrass on 06/06/2022.
//

#include "../include/ppu.h"
#include "../include/lcd.h"

Ppu::Ppu(Bus *bus1, SDL_Renderer *renderer1, Lcd *lcd1) {
    bus = bus1;
    lcd = lcd1;
    renderer = renderer1;
    ticks = 0;
    state = OAMSearch;
    fetcher = new Fetcher((bus->ppu_registers->lcdc.bg_tile_map_area ? 0x9C00 : 0x9800), 0, bus);
}

void Ppu::start() {

}

void Ppu::tick() {
    ticks++;
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
    if (ticks==40) {
        state = PixelTransfer;
        ticks = 0;

        auto tileLine = bus->ppu_registers->ly % 8;
        auto tileMapRowAddr = (bus->ppu_registers->lcdc.bg_tile_map_area ? 0x9C00 : 0x9800) + (uint16_t(bus->ppu_registers->ly/8) * 32);
        fetcher->start(tileMapRowAddr, tileLine);
    }
}


void Ppu::pixeltransfer() {
    fetcher->tick();
    uint8_t pixel;
    if (!fetcher->fifo_bg.empty()) {
        pixel = fetcher->fifo_bg.front();
        fetcher->fifo_bg.pop();

        lcd->write_pixel(x, bus->ppu_registers->ly, pixel);
        x++;
    }
    if (x == 160) {
        state = HBlank;
        x = 0;
    }

}


void Ppu::hblank() {
    if (ticks == 456) {
        ticks = 0;

        bus->ppu_registers->ly++;
        if (bus->ppu_registers->ly == bus->ppu_registers->lyc) {
            bus->interrupt_request->lcd_stat = 1;
        }
        if (bus->ppu_registers->ly == 144) {
            state = VBlank;
            lcd->render = true;
            bus->interrupt_request->vblank = 1;
        } else {
            state = OAMSearch;
        }

    }

}


void Ppu::vblank() {
    if (ticks == 456) {
        ticks = 0;
        bus->ppu_registers->ly++;
        if (bus->ppu_registers->ly == 153) {
            bus->ppu_registers->ly = 0;
            state = OAMSearch;
        }
    }
}
