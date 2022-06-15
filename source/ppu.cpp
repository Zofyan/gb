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
    window_fetcher = new WindowFetcher((bus->ppu_registers->lcdc.window_tile_map_area ? 0x9C00 : 0x9800), 0, bus);
    memset(oams, 0xFF, sizeof(oams));
}

void Ppu::start() {

}

void Ppu::tick() {
    ticks++;
    switch (state) {
        case OAMSearch:
            oamfetch();
            bus->lcd_status->mode = 0x02;
            break;
        case PixelTransfer:
            pixeltransfer();
            bus->lcd_status->mode = 0x03;
            break;
        case HBlank:
            hblank();
            bus->lcd_status->mode = 0x00;
            break;
        case VBlank:
            vblank();
            bus->lcd_status->mode = 0x01;
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
        auto tileMapRowAddr = (bus->ppu_registers->lcdc.bg_tile_map_area ? 0x9C00 : 0x9800);
        fetcher->start(tileMapRowAddr, tileLine);

        tileMapRowAddr = (bus->ppu_registers->lcdc.window_tile_map_area ? 0x9C00 : 0x9800) + uint8_t((bus->ppu_registers->ly - bus->read_v(0xFF4A)) / 8) * 32;
        window_fetcher->start(tileMapRowAddr, tileLine);

        x_shift = bus->ppu_registers->scx % 8;
        y_shift = bus->ppu_registers->scy % 8;

        ticks_pixeltransfer = 168;

        memset(oams_selection, 0, sizeof(oams_selection));
        uint8_t count = 0;
        for(int i = 0; i < 40; i++){
            if((uint8_t)(bus->ppu_registers->ly - y_shift - (bus->sprites[i]->position_y - 16)) < (bus->ppu_registers->lcdc.obj_size ? 16 : 8)){
                oams[i] = *bus->sprites[i];
                oams[i].position_x -= 8;
                oams_selection[i] = true;

                ticks_pixeltransfer += 11 - (uint8_t)fmin(5, (x + bus->ppu_registers->scx) % 8);
                if(count++ >= 10) break;
            }
        }
    }
}

uint8_t Ppu::oamtransfer(){
    oam_t oam;
    uint8_t sprite_pixel;
    bool bg_priority = true;
    for(int i = 0; i < 40; i++){
        oam = oams[i];
        if((uint8_t)(x - oam.position_x) < 8 && !oam.priority && bus->ppu_registers->lcdc.obj_enable && oams_selection[i]){
            uint16_t offset;
            offset = 0x8000;
            if(bus->ppu_registers->lcdc.obj_size) {
                if(oam.flip_y) offset += ((oam.tile_index | 0x01) * 16);
                else offset += ((oam.tile_index & 0xFE) * 16);
            }
            else {
                offset += ((uint8_t)oam.tile_index * 16);
            }

            uint16_t addr = offset;
            if(oam.flip_y) addr += (( 8 - (bus->ppu_registers->ly + 16 - oam.position_y)) * 2);
            else addr += ((bus->ppu_registers->ly + 16 - oam.position_y) * 2);

            uint8_t bit_shift = 7 - (x - oam.position_x);
            if(oam.flip_x) bit_shift = x - oam.position_x;

            uint8_t data = bus->read_v(addr);
            sprite_pixel = ((data >> bit_shift) & 0x1);
            data = bus->read_v(addr + 1);
            sprite_pixel |= ((data >> bit_shift) & 0x1) << 1;
            lcd->write_pixel(x, bus->ppu_registers->ly, sprite_pixel, oam.palette, true);
            if(sprite_pixel) bg_priority = oam.priority;
        }
    }
    return bg_priority;
}

bool draw_window(Bus *bus){
    return bus->ppu_registers->lcdc.bg_window_enable && bus->ppu_registers->lcdc.window_enable;
}

void Ppu::pixeltransfer() {
    if (x == 160 + x_shift) {
        if(ticks >= ticks_pixeltransfer) {
            if (bus->lcd_status->hblank_stat_int) {
                bus->interrupt_request->lcd_stat = 1;
            }
            state = HBlank;
            x = 0;
        }
        return;
    }
    fetcher->tick();
    uint8_t pixel, window_pixel;
    bool bg_priority;
    y_shift = bus->ppu_registers->scy % 8;

    if (!fetcher->fifo_bg.empty()) {
        pixel = fetcher->fifo_bg.front();
        fetcher->fifo_bg.pop();

        bg_priority = oamtransfer();

        if(x >= 0 && bus->ppu_registers->ly >= 0 && bg_priority) lcd->write_pixel(x, bus->ppu_registers->ly, pixel);

        uint8_t wy = bus->read_v(0xFF4A);
        uint8_t wx = bus->read_v(0xFF4B);
        if(bus->ppu_registers->lcdc.window_enable && wy <= bus->ppu_registers->ly && wx <= x + 7){
            window_fetcher->tick();
            if (!window_fetcher->fifo_bg.empty()) {
                window_pixel = window_fetcher->fifo_bg.front();
                window_fetcher->fifo_bg.pop();
                if(draw_window(bus) && bg_priority) lcd->write_pixel(x, bus->ppu_registers->ly, window_pixel);
            }
        }
        x++;
    }
}


void Ppu::hblank() {
    if (ticks != 456) return;

    ticks = 0;

    bus->ppu_registers->ly++;
    if (bus->ppu_registers->ly == bus->ppu_registers->lyc) {
        bus->interrupt_request->lcd_stat = 1;
        bus->lcd_status->lyc_ly_flag = 1;
    } else{
        bus->lcd_status->lyc_ly_flag = 0;
    }

    if (bus->ppu_registers->ly == 144) {
        state = VBlank;
        lcd->render = true;
        bus->interrupt_request->vblank = 1;
        if(bus->lcd_status->vblank_stat_int){
            bus->interrupt_request->lcd_stat = 1;
        }
    } else {
        state = OAMSearch;
        if(bus->lcd_status->oam_stat_interrupt){
            bus->interrupt_request->lcd_stat = 1;
        }
        memset(oams, 0xDF, sizeof(oams));
    }
}

void Ppu::vblank() {
    if (ticks == 456) return;
    ticks = 0;
    bus->ppu_registers->ly++;
    if (bus->ppu_registers->ly == 153) {
        bus->ppu_registers->ly = 0;
        state = OAMSearch;
        if(bus->lcd_status->oam_stat_interrupt){
            bus->interrupt_request->lcd_stat = 1;
        }
        memset(oams, 0xDF, sizeof(oams));
    }
}
