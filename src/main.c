// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "debug.h"
#include "serial/serial-joystick.h"
#include "gfx/gfx.h"
#include "cali/cali.h"
#include "daemon/daemon.h"

#define MAX_COMMAND_LENGTH 256

SDL_Renderer *sdlRenderer = NULL;
SDL_Window *sdlWindow = NULL;
TTF_Font *font32 = NULL;
TTF_Font *font26 = NULL;
TTF_Font *font24 = NULL;
TTF_Font *font20 = NULL;
sdl_axis_t sdlAxis;
double deltaTime = 0;

void saveChanges(joypad_cali_t *cali, uint8_t joyToCal);

int main ()
{   
    INFO("Starting calibration tool.");

    initSDL(&sdlWindow, &sdlRenderer);
    cleanScreen(sdlRenderer);

    INFO("SDL init. correct.");

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
        if (run == 0)
        {
            INFO("Quitting main loop.");
            break;
        }

        if(canRead && readJoypad(joyFd, &joyData) == 0)
        {
            continue;
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
                break;
            case 10:
                joyTutorial(&state, 15);
                break;
            case 15:
                state = 20;
                canRead = 1;
                resetTimer = 1;
                break;
            case 20:
                calculateMinMax(&joyData, &joyCali);
                joyCaliPanel(/*&joyData, &joyCali,*/ resetTimer, &state, 25);
                resetTimer = 0;
                break;
            case 25:
                state = 30;
                resetTimer = 1;
                break;
            case 30:
                joyZeroPanel(/*&joyData, &joyCali,*/ resetTimer, &state, 35);
                resetTimer = 0;
                break;
            case 35:
                canRead = 0;
                calculateZero(&joyData, &joyCali);
                closeJoypad(joyFd);
                saveChanges(&joyCali, joyToCal);
                state = 40;
                break;
            case 40:
                joySave(&state, 0);
                break;
        }

        SDL_RenderPresent(sdlRenderer);
    }    
    
    INFO("Cleaning up SDL objects.");
    
    closeJoypad(joyFd);
    cleanScreen(sdlRenderer);

    usleep(1000 * 50); // Wait for screen to clear

    cleanUpSDL(sdlWindow, sdlRenderer);
    INFO("Exiting.");
    exit(0);
}


void saveChanges(joypad_cali_t *cali, uint8_t joyToCal)
{   
    const char* fileCal = joyToCal == 0 ? "/mnt/UDISK/joypad.config" :  "/mnt/UDISK/joypad_right.config";
    const char* fileCalBk = joyToCal == 0 ? "/mnt/UDISK/joypad.config.bk" :  "/mnt/UDISK/joypad_right.config.bk";

    joySaving(0, "Backing up old file...");
    sleep(1);
    if (access(fileCal, F_OK) == 0) {
        rename(fileCal, fileCalBk);
    }
    
    joySaving(15, "Closing SDL joystick...");
    sleep(1);
    closeJoystick();

    joySaving(20, "Writing new calibartion data...");
    sleep(1);
    writeCali(cali, fileCal);

    joySaving(35, "Killing input daemon...");
    sleep(1);
    killDaemon("trimui_inputd");

    joySaving(55, "Restarting input daemon...");
    sleep(1);
    startDaemon("trimui_inputd &");
    
    joySaving(70, "Opening SDL joystick...");
    sleep(1);
    int joysticks = SDL_NumJoysticks();
    printf("There are %d joysticks connected: %s.\n", joysticks, SDL_JoystickNameForIndex(0));
    
    joySaving(85, "Applying changes to boot partition...");
    sleep(1);
    system("batocera-save-overlay");
}