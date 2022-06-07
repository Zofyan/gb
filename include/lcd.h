//
// Created by Sofyan Aarrass on 07/06/2022.
//

#ifndef GB_SCREEN_H
#define GB_SCREEN_H

#include "SDL2/SDL.h"
#include <cstdint>



class Lcd {
public:
    Lcd();
    uint16_t width;
    uint16_t height;
    uint8_t colors[4];
};


#endif //GB_SCREEN_H
