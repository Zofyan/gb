//
// Created by Sofyan Aarrass on 07/06/2022.
//

#ifndef GB_WINDOW_FETCHER_H
#define GB_WINDOW_FETCHER_H

#include <cstdint>
#include <queue>
#include "bus.h"
#include "fetcher.h"

class WindowFetcher {
private:
    Bus *bus;
    std::queue<oam_t> oams;
    FetcherState state;
    uint16_t ticks;
    uint8_t tileIndex;
    uint8_t lineIndex;
    uint16_t mapAddr;
    uint8_t tileID;
    uint8_t pixelData[16];
public:

    std::queue<uint8_t> fifo_bg;
    std::queue<uint8_t> fifo_sprite;
    WindowFetcher(uint16_t mapAddr1, uint8_t tileLine1, Bus *bus1);
    uint16_t tileLine;

    void start(uint16_t mapAddr, uint8_t tileLine);
    void tick();
    void readtiledata();
    void pushtofifo();
    void readtileid();
};


#endif //GB_FETCHER_H
