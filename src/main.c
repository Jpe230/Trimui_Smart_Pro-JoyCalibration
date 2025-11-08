// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "debug.h"
#include "serial/serial-joystick.h"
#include "gfx/gfx.h"
#include "calibration/calibration.h"

#define MAX_COMMAND_LENGTH 512

SDL_Renderer *sdlRenderer = NULL;
SDL_Window *sdlWindow = NULL;
sdl_axis_t sdlAxis;
double deltaTime = 0;

void saveChanges(joypad_cali_t *cali, uint8_t joystickToCalibrate);

#define FRAME_TARGET_MS 16u
#define APPLY_CAL_SCRIPT "./apply_calibration.sh"

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
        uint32_t frameStart = SDL_GetTicks();
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)((now - last)*1000 / (double)SDL_GetPerformanceFrequency() );

        run = pollSdlJoystick(&event, &sdlAxis);
        if (run == 0 && blockExit == 0)
        {
            INFO("Quitting main loop.");
            break;
        }

        if (canRead)
        {
            int readResult = readSerialJoypad(joystickFd, &joystickSerialData);
            if (readResult < 0)
            {
                continue;
            }
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

        uint32_t frameDuration = SDL_GetTicks() - frameStart;
        if (frameDuration < FRAME_TARGET_MS)
        {
            SDL_Delay(FRAME_TARGET_MS - frameDuration);
        }
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
    joySaving(0, "Closing SDL joystick...");
    sleep(1);
    closeJoystick();

    joySaving(20, "Running calibration script...");
    sleep(1);

    char command[MAX_COMMAND_LENGTH];
    int commandLength = snprintf(
        command,
        sizeof(command),
        "%s %u %u %u %u %u %u %u",
        APPLY_CAL_SCRIPT,
        joystickToCalibrate,
        (unsigned int)cali->x_min,
        (unsigned int)cali->x_max,
        (unsigned int)cali->y_min,
        (unsigned int)cali->y_max,
        (unsigned int)cali->x_zero,
        (unsigned int)cali->y_zero
    );

    if (commandLength < 0 || commandLength >= (int)sizeof(command))
    {
        fprintf(stderr, "Failed to compose calibration script command.\n");
    }
    else
    {
        int scriptResult = system(command);
        if (scriptResult != 0)
        {
            fprintf(stderr, "Calibration script exited with status %d.\n", scriptResult);
        }
    }

    joySaving(55, "Opening SDL joystick...");
    sleep(3);
    openJoystick();

    int joysticks = SDL_NumJoysticks();
    const char *joystickName = (joysticks > 0) ? SDL_JoystickNameForIndex(0) : "unknown";
    printf("There are %d joysticks connected: %s.\n", joysticks, joystickName);
    
    joySaving(85, "Calibration complete.");
    sleep(1);
}
