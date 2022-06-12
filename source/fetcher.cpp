//
// Created by Sofyan Aarrass on 07/06/2022.
//

#include <cstdlib>
#include <cstring>
#include "../include/fetcher.h"
#include "../include/bus.h"

Fetcher::Fetcher(uint16_t mapAddr1, uint8_t tileLine1, Bus *bus1) {
    bus = bus1;
    ticks = 0;
    tileIndex = 0;
    mapAddr = mapAddr1;
    tileLine = tileLine1;
    state = ReadTileID;

}

void Fetcher::tick() {
    ticks++;
    if(ticks < 2) return;
    ticks = 0;

    switch (state) {
        case ReadTileData0:
        case ReadTileData1:
            readtiledata();
            break;
        case PushToFIFO:
            pushtofifo();
            break;
        case ReadTileID:
            readtileid();
            break;
        default:
            exit(6);
    }
}

void Fetcher::readtiledata() {
    uint16_t offset;
    if(bus->ppu_registers->lcdc.bg_window_tile_data_area){
        offset = 0x8000;
        offset += ((uint8_t)tileID * 16);
    }else{
        offset = 0x9000;
        offset += (((int8_t)tileID) * 16);
    }
    uint16_t addr = offset + (tileLine * 2);
    uint8_t data = bus->read_v(addr + (state == ReadTileData1));

    for (uint8_t bitPos = 0; bitPos <= 7; bitPos++) {
        if(state == ReadTileData0){
            pixelData[bitPos] = (data >> bitPos) & 0x1;
        }else{
            pixelData[bitPos] |= ((data >> bitPos) & 0x1) << 1;
        }
    }

    state = state == ReadTileData0 ? ReadTileData1 : PushToFIFO;
}

void Fetcher::pushtofifo() {
    if(fifo_bg.size() <= 8){
        // We stored pixel bits from least significant (rightmost) to most
        // (leftmost) in the data array, so we must push them in reverse
        // order.
        for(int8_t i = 7; i >= 0; i--){
            fifo_bg.push(pixelData[i]);
        }
        // Advance to the next tile in the map's row.
        tileIndex++;
        state = ReadTileID;
    }
}

void Fetcher::readtileid() {
    tileID = bus->read_v(mapAddr + tileIndex);
    memset(pixelData, 0, 8);
    state = ReadTileData0;
}

void Fetcher::start(uint16_t mapAddr1, uint8_t tileLine1) {
    tileIndex = 0;
    mapAddr = mapAddr1;
    tileLine = tileLine1;
    state = ReadTileID;
    while(!fifo_bg.empty()){fifo_bg.pop();}
}

