//
// Created by sofyan on 1-6-22.
//

#ifndef GB_MEMORY_H
#define GB_MEMORY_H


#include <cstdint>

typedef struct memory{
    uint8_t *memory;
} memory;

int init_memory(memory *memory);

#endif //GB_MEMORY_H
