//
// Created by sofyan on 1-6-22.
//

#include <cstdlib>
#include "../include/memory.h"

int init_memory(memory *memory){
    memory->memory = (uint8_t*) calloc(8 * 1024, 1);
}