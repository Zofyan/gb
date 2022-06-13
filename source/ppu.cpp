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
    memset(oams, 0xFF, sizeof(oams));
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

        uint8_t count = 0;
        for(int i = 0; i < 40; i++){
            if((uint8_t)(bus->ppu_registers->ly - (bus->sprites[i]->position_y - 16)) < 8){
                oams[i] = *bus->sprites[i];
                oams[i].position_x -= 8;
                if(count++ >= 10) break;
            }
        }
    }
}


void Ppu::pixeltransfer() {
    fetcher->tick();
    uint8_t pixel, sprite_pixel, byte1, byte2, line;
    oam_t oam;
    if (!fetcher->fifo_bg.empty()) {
        pixel = fetcher->fifo_bg.front();
        fetcher->fifo_bg.pop();

        sprite_pixel = 0;
        bool bg_priority = true;
        for(int i = 0; i < 40; i++){
            oam = oams[i];
            if((uint8_t)(x - oam.position_x) < 8 && !oam.priority){
                uint16_t offset;
                offset = 0x8000;
                offset += ((uint8_t)oam.tile_index * 16);
                uint16_t addr = offset + ((bus->ppu_registers->ly + 16 - oam.position_y) * 2);

                uint8_t data = bus->read_v(addr);
                sprite_pixel = ((data >> ((7 - x + oam.position_x))) & 0x1);
                data = bus->read_v(addr + 1);
                sprite_pixel |= ((data >> ((7 - x + oam.position_x))) & 0x1) << 1;
                lcd->write_pixel(x, bus->ppu_registers->ly, sprite_pixel, oam.palette, true);
                if(sprite_pixel) bg_priority = oam.priority;
            }
        }
        if(bg_priority) lcd->write_pixel(x, bus->ppu_registers->ly, pixel);
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
            memset(oams, 0xDF, sizeof(oams));
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
            memset(oams, 0xDF, sizeof(oams));
        }
    }
}

void Ppu::find_oams() {

}
