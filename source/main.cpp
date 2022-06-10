#include <iostream>
#include <cstdio>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"
#include "pthread.h"
#include "../include/logger.h"
#include "../include/lcd.h"

void *cpu_thread(void *arg){
    Cpu *cpu = (Cpu *)arg;
    uint32_t c = 0;
    uint8_t temp;
    while(cpu->execute_next_instruction()){
        cpu->bus->read(SERIAL_SC, &temp);
        if(temp == 0x81){
            cpu->bus->read(SERIAL_SB, &temp);
            printf("%c", temp);
            fflush(stdout);
            temp = 0;
            cpu->bus->write(SERIAL_SC, &temp);
            cpu->count++;
        }
        c++;
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialize the SDL2 library\n");
        exit(-1);
    }

    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_CreateWindowAndRenderer(160, 144, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    pthread_t pthread;

    auto *rom = new loader();
    Bus bus;
    Lcd lcd(160, 144, renderer, &bus);

    Ppu ppu(&bus, renderer, &lcd);

    Cpu cpu(&bus, &ppu);
    Logger logger(&cpu.registers1, &cpu, false);

    FILE *openrom, *openlog;
    openrom = fopen(rom->filename, "r");
    //openlog = fopen("../temp_log.txt", "w");
    bus.load_rom(openrom);


    pthread_create(&pthread, NULL, cpu_thread, &cpu);


    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
        }
    }

    pthread_join(pthread, NULL);
    SDL_Quit();
    return 0;
}
