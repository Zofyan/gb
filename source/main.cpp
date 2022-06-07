#include <iostream>
#include <cstdio>
#include <csignal>
#include "../include/loader.h"
#include "../include/instructions.h"
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"
#include "pthread.h"
#include "../include/logger.h"
#include "../include/lcd.h"

void *cpu_thread(void *vargp){

    Cpu cpu = *((Cpu*)vargp);

    Logger logger(&cpu.registers1, &cpu, true);
    uint8_t temp;
    uint32_t c = 0;
    while (cpu.execute_next_instruction()) {
        logger.print_registers();
        logger.print_flags();
        logger.print_instruction();
        if (cpu.ime) {
            if (cpu.bus->interrupt_request->vblank && cpu.bus->interrupt_enable->vblank) {
                cpu.bus->interrupt_request->vblank = 0;
                cpu.bus->push(*cpu.registers1.PC, cpu.registers1.SP);
                (*cpu.registers1.PC) = 0x40;
                cpu.ime = false;

            }
            if (cpu.bus->interrupt_request->timer && cpu.bus->interrupt_enable->timer) {
                cpu.bus->interrupt_request->timer = 0;
                cpu.bus->push(*cpu.registers1.PC, cpu.registers1.SP);
                (*cpu.registers1.PC) = 0x50;
                cpu.ime = false;
            }
            if (cpu.bus->interrupt_request->lcd_stat && cpu.bus->interrupt_enable->lcd_stat) {
                cpu.bus->interrupt_request->lcd_stat = 0;
                cpu.bus->push(*cpu.registers1.PC, cpu.registers1.SP);
                (*cpu.registers1.PC) = 0x48;
                cpu.ime = false;
            }
            if (cpu.bus->interrupt_request->serial && cpu.bus->interrupt_enable->serial) {
                cpu.bus->interrupt_request->serial = 0;
                cpu.bus->push(*cpu.registers1.PC, cpu.registers1.SP);
                (*cpu.registers1.PC) = 0x58;
                cpu.ime = false;
            }
            if (cpu.bus->interrupt_request->joypad && cpu.bus->interrupt_enable->joypad) {
                cpu.bus->interrupt_request->joypad = 0;
                cpu.bus->push(*cpu.registers1.PC, cpu.registers1.SP);
                (*cpu.registers1.PC) = 0x60;
                cpu.ime = false;
            }
        }

        //logger.print_registers();
        //logger.print_flags();
        //logger.print_instruction();
        cpu.bus->read(SERIAL_SC, &temp);
        if (temp == 0x81) {
            cpu.bus->read(SERIAL_SB, &temp);
            printf("%c", temp);
            fflush(stdout);
            temp = 0;
            cpu.bus->write(SERIAL_SC, &temp);
            cpu.count++;
        }
        c++;
        //printf("%u\n", c);
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

    //SDL_CreateWindowAndRenderer(160, 144, 0, &window, &renderer);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    //SDL_RenderClear(renderer);

    pthread_t pthread;

    auto *rom = new loader();
    Bus bus;

    Lcd lcd(160, 144);

    Ppu ppu(&bus, renderer, nullptr);

    Cpu cpu(&bus, &ppu);


    FILE *openrom, *openlog;
    openrom = fopen(rom->filename, "r");
    bus.load_rom(openrom);


    pthread_create(&pthread, NULL, cpu_thread, &cpu);


    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool quit = false;
    /*while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
        }
        //SDL_RenderClear(renderer);
        //SDL_RenderCopy(renderer, texture, NULL, NULL);
        //SDL_RenderPresent(renderer);
    }
*/
    pthread_join(pthread, NULL);
    return 0;
}
