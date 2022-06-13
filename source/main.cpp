#include <iostream>
#include <cstdio>
#include <thread>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"
#include "pthread.h"
#include "../include/logger.h"
#include "../include/lcd.h"

#define SCALE 4

void *cpu_thread(void *arg) {
    Cpu *cpu = (Cpu *) arg;
    uint32_t c = 0;
    uint8_t temp;
    Logger logger(&cpu->registers1, cpu, true);
    //logger.print_instruction();
    while (cpu->execute_next_instruction()) {
        //logger.print_instruction();
        cpu->bus->read(SERIAL_SC, &temp);
        if (temp == 0x81) {
            cpu->bus->read(SERIAL_SB, &temp);
            printf("%c", temp);
            fflush(stdout);
            temp = 0;
            cpu->bus->write(SERIAL_SC, &temp);
        }
        c++;
    }
}

void debug_tiledata(SDL_Renderer *renderer, Bus *bus) {

    SDL_RenderClear(renderer);
    uint16_t line;
    uint8_t color;
    for (int x = 0; x < 384; x++) {
        printf("x: %u, y: %u\n", ((x % 20) * 8), ((x / 20) * 8));
        for (int l = 0; l < 8; l++) {
            bus->read(0x8000 + x * 16 + l * 2, (uint8_t *) &line);
            bus->read(0x8000 + x * 16 + l * 2 + 1, ((uint8_t *) &line) + 1);
            for (int b = 0; b < 8; b++) {
                color = (line >> (b * 2)) & 0x03;
                SDL_SetRenderDrawColor(renderer, 0, color * 60 + 60, 0, 255);
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        //printf("x: %u, y: %u\n", ((x % 20) * 8 + b) * 2, ((x % 20) * 8 + b) * 2);
                        SDL_RenderDrawPoint(renderer, ((x % 20) * 9 + b) * 3 + i,
                                            (((uint8_t) (x / 20)) * 9 + l) * 3 + j);
                    }
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
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
    SDL_Renderer *renderer, *renderer1, *renderer2;
    SDL_Window *window, *window1, *window2;
    SDL_Texture *texture;
    int i;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(160 * SCALE, 144 * SCALE, 0, &window, &renderer);
    //SDL_CreateWindowAndRenderer(9 * 20 * 3, 9 * 20 * 3, 0, &window1, &renderer1);
    //SDL_CreateWindowAndRenderer(256 * SCALE, 512 * SCALE, 0, &window2, &renderer2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, 160, 144);

    SDL_SetRenderDrawColor(renderer1, 0, 0, 0, 0);
    SDL_RenderClear(renderer1);
    SDL_SetRenderDrawColor(renderer1, 255, 0, 0, 255);


    bool quit = false, press = false;
    uint32_t oldFrame = 10000;
    uint8_t color, lastColor = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            press = true;
            if (event.type == SDL_KEYDOWN) {
                press = false;
            }
            if (event.type == SDL_KEYUP) {
                press = true;
            }
            switch (event.key.keysym.sym) {
                case SDLK_a:
                    bus.joypad_real1.left_b = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_s:
                    bus.joypad_real1.down_start = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_w:
                    bus.joypad_real1.up_select = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_d:
                    bus.joypad_real1.right_a = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_e:
                    bus.joypad_real2.up_select = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_x:
                    bus.joypad_real2.left_b = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_r:
                    bus.joypad_real2.down_start = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                case SDLK_c:
                    bus.joypad_real2.right_a = press;
                    bus.interrupt_request->joypad = !press;
                    break;
                default:
                    break;

            }
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        if (cpu.count != oldFrame) {
            pthread_mutex_lock(&bus.lock);
            SDL_RenderClear(renderer);

            oldFrame = cpu.count;

            SDL_UpdateTexture(texture, nullptr, bus.pixels, 160 * 4);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            lcd.render = false;
            //debug_tiledata(renderer1, &bus);
            pthread_mutex_unlock(&bus.lock);
        }
    }

    pthread_join(pthread, NULL);
    SDL_Quit();
    return 0;
}
