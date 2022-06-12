//
// Created by Sofyan Aarrass on 07/06/2022.
//

#ifndef GB_FETCHER_H
#define GB_FETCHER_H

#include <cstdint>
#include <queue>
#include "bus.h"

enum FetcherState {
    ReadTileData0,
    ReadTileData1,
    PushToFIFO,
    ReadTileID,
    Sleep
};

class Fetcher {
private:
    Bus *bus;
    FetcherState state;
    uint16_t ticks;
    uint16_t tileIndex;
    uint16_t mapAddr;
    uint8_t tileID;
    uint8_t pixelData[16];
public:

    std::queue<uint8_t> fifo_bg;
    std::queue<uint8_t> fifo_sprite;
    Fetcher(uint16_t mapAddr1, uint8_t tileLine1, Bus *bus1);
    uint16_t tileLine;

    void start(uint16_t mapAddr, uint8_t tileLine);
    void tick();
    void readtiledata();
    void pushtofifo();
    void readtileid();
};


#endif //GB_FETCHER_H
