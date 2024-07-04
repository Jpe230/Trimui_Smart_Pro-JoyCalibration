// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial/serial-joystick.h"
#include "gfx/gfx.h"
#include "cali/cali.h"

#define MAX_COMMAND_LENGTH 256

SDL_Renderer *sdlRenderer = NULL;
SDL_Window *sdlWindow = NULL;
TTF_Font *font32 = NULL;
TTF_Font *font26 = NULL;
TTF_Font *font24 = NULL;
TTF_Font *font20 = NULL;
sdl_axis_t sdlAxis;
double deltaTime = 0;

void writeAndRestart(joypad_cali_t *cali, uint8_t joyToCal);

int main ()
{
    initSDL(&sdlWindow, &sdlRenderer);
    cleanScreen(sdlRenderer);

    uint32_t startTime = SDL_GetTicks();
    uint8_t resetTimer = 0;
    uint8_t canRead = 0;
    uint8_t state = 0;
    uint8_t run = 1;
    int joyToCal = 0;
    int joyFd = -1;

    joypad_struct_t joyData;
    joypad_cali_t joyCali;
    
    SDL_Event event;

    uint32_t now = SDL_GetPerformanceCounter();
    uint32_t last = 0;

    while(run)
    {   
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)((now - last)*1000 / (double)SDL_GetPerformanceFrequency() );

        run = pollControl(&event, &sdlAxis);

        if(canRead && readJoypad(joyFd, &joyData) == 0)
        {
            continue;
            usleep(1000);
        }

        drawUI();

        switch(state)
        {
            case 0:
                joyToCal = joySelectPanel(&state, 5);
                break;
            case 5:
                joyFd = openJoypad(joyToCal == 0 ? "/dev/ttyS4" : "/dev/ttyS3");
                clearData(&joyData, &joyCali);
                state = 10;
            case 10:
                joyTutorial(&state, 15);
                break;
            case 15:
                state = 20;
                canRead = 1;
                resetTimer = 1;
            case 20:
                calculateMinMax(&joyData, &joyCali);
                joyCaliPanel(&joyData, &joyCali, resetTimer, &state, 25);
                resetTimer = 0;
                break;
            case 25:
                state = 30;
                resetTimer = 1;
            case 30:
                joyZeroPanel(&joyData, &joyCali, resetTimer, &state, 35);
                resetTimer = 0;
                break;
            case 35:
                calculateZero(&joyData, &joyCali);
                state = 40;
                canRead = 0;
                closeJoypad(joyFd);
                writeAndRestart(&joyCali, joyToCal);
            case 40:
                joySave(&state, 0);
                break;
        }

        SDL_RenderPresent(sdlRenderer);
    }    
    
    if(joyFd > 0)
    {
        closeJoypad(joyFd);
    }

    cleanScreen(sdlRenderer);
    cleanUpSDL(sdlWindow, sdlRenderer);
	return 0;

}

pid_t findDaemonPid(const char *daemonName) {
    FILE *pipe;
    char command[50];
    char pid_str[10];
    pid_t pid;
    snprintf(command, sizeof(command), "pgrep %s", daemonName);
    pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen");
        return -1;
    }
    if (fgets(pid_str, sizeof(pid_str), pipe) == NULL) {
        perror("fgets");
        pclose(pipe);
        return -1;
    }
    pid = (pid_t) atoi(pid_str);
    pclose(pipe);
    return pid;
}

void killDaemon(const char* daemonName) 
{
    pid_t pid = findDaemonPid(daemonName);
    if (pid > 0) {
        if (kill(pid, SIGTERM) == 0) {
            printf("Daemon '%s' (PID: %d) killed successfully.\n", daemonName, pid);
        } else {
            perror("Error killing daemon");
        }
    } else {
        printf("Daemon '%s' not found.\n", daemonName);
    }
}

void startDaemon(const char* daemonName)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        system(daemonName);
    }
}

void writeAndRestart(joypad_cali_t *cali, uint8_t joyToCal)
{   
    const char* fileCal = joyToCal == 0 ? "/mnt/UDISK/joypad.config" :  "/mnt/UDISK/joypad_right.config";
    const char* fileCalBk = joyToCal == 0 ? "/mnt/UDISK/joypad.config.bk" :  "/mnt/UDISK/joypad_right.config.bk";
    if (access(fileCal, F_OK) == 0) {
        rename(fileCal, fileCalBk);
    }

    FILE *fp = fopen(fileCal, "w");
    if (fp == NULL) {
        perror("Error opening file");
    }

    fprintf(fp, "x_min=%d\n", cali->x_min);
    fprintf(fp, "x_max=%d\n", cali->x_max);
    fprintf(fp, "y_min=%d\n", cali->y_min);
    fprintf(fp, "y_max=%d\n", cali->y_max);
    fprintf(fp, "x_zero=%d\n", cali->x_zero);
    fprintf(fp, "y_zero=%d\n", cali->y_zero);

    fclose(fp);

    killDaemon("trimui_inputd");
    startDaemon("trimui_inputd &");

    system("batocera-save-overlay");

}