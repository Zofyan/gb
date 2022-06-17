//
// Created by sofyan on 17-6-22.
//

#include "../include/savestate.h"


SaveState::SaveState(Cpu *cpu1) {
    cpu = cpu1;
}

void SaveState::save() {
    pthread_mutex_lock(&cpu->lock);
    char *savename = (char*)calloc(128, 1);
    printf("Enter savename: ");
    //scanf("%s", savename);
    strcat(savename, ".gbs");

    FILE *save = fopen("test5.gbs", "w");
    fwrite(cpu->registers1.PC, 2, 1, save);
    fwrite(cpu->registers1.SP, 2, 1, save);
    fwrite(cpu->registers1.BC, 2, 1, save);
    fwrite(cpu->registers1.DE, 2, 1, save);
    fwrite(cpu->registers1.HL, 2, 1, save);
    fwrite(cpu->registers1.AF, 2, 1, save);

    fwrite(cpu->bus->memory, 1, cpu->bus->memory_size, save);
    fwrite(&cpu->bus->rom_size, 1, sizeof(cpu->bus->rom_size), save);
    fwrite(&cpu->bus->eram_enable, 1, sizeof(cpu->bus->eram_enable), save);

    fclose(save);
    pthread_mutex_unlock(&cpu->lock);

}

void SaveState::load(char *filename) {
    pthread_mutex_lock(&cpu->lock);

    FILE *save = fopen("test5.gbs", "r");
    if(save == nullptr) {
        pthread_mutex_unlock(&cpu->lock);
        return;
    }
    fread(cpu->registers1.PC, 2, 1, save);
    fread(cpu->registers1.SP, 2, 1, save);
    fread(cpu->registers1.BC, 2, 1, save);
    fread(cpu->registers1.DE, 2, 1, save);
    fread(cpu->registers1.HL, 2, 1, save);
    fread(cpu->registers1.AF, 2, 1, save);

    fread(cpu->bus->memory, 1, cpu->bus->memory_size, save);
    fread(&cpu->bus->rom_size, 1, sizeof(cpu->bus->rom_size), save);
    fread(&cpu->bus->eram_enable, 1, sizeof(cpu->bus->eram_enable), save);

    fclose(save);
    pthread_mutex_unlock(&cpu->lock);
}
