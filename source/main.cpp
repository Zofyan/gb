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

#define SCALE 4

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
        }
        c++;
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    pthread_t pthread;

    auto *rom = new loader();
    Bus bus;
    Lcd lcd(160, 144, nullptr, &bus);

    Ppu ppu(&bus, nullptr, &lcd);

    Cpu cpu(&bus, &ppu);
    Logger logger(&cpu.registers1, &cpu, false);

    FILE *openrom, *openlog;
    openrom = fopen(rom->filename, "r");
    //openlog = fopen("../temp_log.txt", "w");
    bus.load_rom(openrom);


    pthread_create(&pthread, NULL, cpu_thread, &cpu);


    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    int i;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(144 * SCALE, 160 * SCALE, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);


    bool quit = false;
    uint32_t oldFrame = 10000;
    uint8_t color, lastColor = 0;
    while (!quit){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                quit = true;
            }
        }
        if(lcd.render) {
            pthread_mutex_lock(&bus.lock);
            SDL_RenderClear(renderer);

            printf("frame %u\n", cpu.count);
            for (int x = 0; x < 160; x++) {
                for (int y = 0; y < 160; y++) {
                    color = bus.pixels[y * 160 + x];
                    if (lastColor != color) {
                        SDL_SetRenderDrawColor(renderer, 0, color, 0, 255);
                        lastColor = color;
                    }
                    for (i = 0; i < SCALE; i++) {
                        for (int j = 0; j < SCALE; j++) {
                            SDL_RenderDrawPoint(renderer, (x - bus.ppu_registers->scx) * SCALE + i,
                                                (y - bus.ppu_registers->scy) * SCALE + j);
                        }
                    }
                }
            }
            SDL_RenderPresent(renderer);
            lcd.render = false;
            pthread_mutex_unlock(&bus.lock);
        }
    }

    pthread_join(pthread, NULL);
    SDL_Quit();
    return 0;
}
