// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "debug.h"
#include "serial/serial-joystick.h"
#include "gfx/gfx.h"
#include "calibration/calibration.h"
#include "daemon/daemon.h"

#define MAX_COMMAND_LENGTH 256

SDL_Renderer *sdlRenderer = NULL;
SDL_Window *sdlWindow = NULL;
sdl_axis_t sdlAxis;
double deltaTime = 0;

void saveChanges(joypad_cali_t *cali, uint8_t joystickToCalibrate);

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
    int joystickToCalibrate = 0;
    int joystickFd = -1;

    joypad_struct_t joystickSerialData;
    joypad_cali_t joystickCalibrationData;
    
    SDL_Event event;

    uint32_t now = SDL_GetPerformanceCounter();
    uint32_t last = 0;

    uint8_t blockExit = 0;

    while(run)
    {   
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)((now - last)*1000 / (double)SDL_GetPerformanceFrequency() );

        run = pollSdlJoystick(&event, &sdlAxis);
        if (run == 0 && blockExit == 0)
        {
            INFO("Quitting main loop.");
            break;
        }

        if(canRead && readSerialJoypad(joystickFd, &joystickSerialData) == 0)
        {
            continue;
        }

        drawUI(blockExit);

        switch(state)
        {
            case 0:
                joystickToCalibrate = joystickSelectPanel(&state, 5);
                break;
            case 5:
                //                                                           LEFT JOY       RIGHT JOY
                joystickFd = openSerialJoystick(joystickToCalibrate == 0 ? "/dev/ttyS4" : "/dev/ttyS3");
                clearData(&joystickSerialData, &joystickCalibrationData);
                state = 10;
                break;
            case 10:
                joystickTutorialPanel(&state, 15);
                break;
            case 15:
                state = 20;
                canRead = 1;
                resetTimer = 1;
                blockExit = 1;
                break;
            case 20:
                calculateJoystickMinMax(&joystickSerialData, &joystickCalibrationData);
                joystickCalibrationPanel(/*&joystickSerialData, &joystickCalibrationData,*/ resetTimer, &state, 25);
                resetTimer = 0;
                break;
            case 25:
                state = 30;
                resetTimer = 1;
                break;
            case 30:
                joystickDeathZonePanel(/*&joystickSerialData, &joystickCalibrationData,*/ resetTimer, &state, 35);
                resetTimer = 0;
                break;
            case 35:
                canRead = 0;
                calculateJoystickZeroPosition(&joystickSerialData, &joystickCalibrationData);
                closeSerialJoystick(joystickFd);
                saveChanges(&joystickCalibrationData, joystickToCalibrate);
                state = 40;
                blockExit = 0;
                break;
            case 40:
                joystickSavePanel(&state, 0);
                break;
        }

        SDL_RenderPresent(sdlRenderer);
    }    
    
    INFO("Cleaning up SDL objects.");
    
    closeSerialJoystick(joystickFd);
    cleanScreen(sdlRenderer);

    usleep(1000 * 50); // Wait for screen to clear

    cleanUpSDL(sdlWindow, sdlRenderer);
    INFO("Exiting.");
    exit(0);
}


void saveChanges(joypad_cali_t *cali, uint8_t joystickToCalibrate)
{   
    const char* fileCal = joystickToCalibrate == 0 ? "/mnt/UDISK/joypad.config" :  "/mnt/UDISK/joypad_right.config";
    const char* fileCalBk = joystickToCalibrate == 0 ? "/mnt/UDISK/joypad.config.bk" :  "/mnt/UDISK/joypad_right.config.bk";

    joySaving(0, "Backing up old file...");
    sleep(1);
    if (access(fileCal, F_OK) == 0) {
        rename(fileCal, fileCalBk);
    }
    
    joySaving(15, "Closing SDL joystick...");
    sleep(1);
    closeJoystick();

    joySaving(20, "Writing new calibration data...");
    sleep(1);
    writeCali(cali, fileCal);

    joySaving(35, "Killing input daemon...");
    sleep(1);
    killDaemon("trimui_inputd_smart_pro");

    joySaving(55, "Restarting input daemon...");
    sleep(1);
    startDaemon("trimui_inputd_smart_pro &");
    
    joySaving(70, "Opening SDL joystick...");
    sleep(1);
    int joysticks = SDL_NumJoysticks();
    printf("There are %d joysticks connected: %s.\n", joysticks, SDL_JoystickNameForIndex(0));
    
    joySaving(85, "Applying changes to boot partition...");
    sleep(1);
}