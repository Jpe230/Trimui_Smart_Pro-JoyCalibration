// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gfx.h"
#include "utils/primitives.h"
#include <SDL2/SDL_ttf.h>

extern SDL_Renderer *sdlRenderer;
extern TTF_Font *font32;
extern TTF_Font *font26;
extern TTF_Font *font24;
extern TTF_Font *font20;
extern sdl_axis_t sdlAxis;
extern double deltaTime;

void drawUI(uint8_t blockExit)
{
    SDL_SetRenderDrawColor(sdlRenderer, 45, 45, 45, 255);
    SDL_RenderClear(sdlRenderer);

    SDL_SetRenderDrawColor(sdlRenderer, 150, 150, 150, 255);

    SDL_Rect r;
    r.w = 1200;
    r.h = 2;
    r.x = CERTER_HOR(1200);
    r.y = SCREEN_HEIGHT - 100;
    SDL_RenderFillRect(sdlRenderer, &r);

    r.y = 100;
    SDL_RenderFillRect(sdlRenderer, &r);
    
    writeText(sdlRenderer, "Calibrate Control Sticks", font32, 70, 50);

    if(blockExit)
    {
        return;
    }

    SDL_SetRenderDrawColor(sdlRenderer, 255,255, 255, 255);
    drawFilledCircle(sdlRenderer, 970, 665, 20);
    TTF_SetFontStyle(font26, TTF_STYLE_BOLD);
    writeTextBg(sdlRenderer, "X", font26, 963, 648);
    TTF_SetFontStyle(font26, TTF_STYLE_NORMAL);

    writeText(sdlRenderer, "Exit program", font24, 1010, 650);
}

double map(double num, double in_min, double in_max, double out_min, double out_max)
{
    return (num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t joystickSelectPanel(uint8_t *state, uint8_t nextState)
{
    int text_x = 420;
    writeText(sdlRenderer, "Check the following:", font24, 100, text_x);
    writeText(sdlRenderer, "1. The indicator is not at the center when the stick is not being touched.", font20, 100, text_x + 50);
    writeText(sdlRenderer, "2. The range of the stick is incomplete or is outside of the circle.", font20, 100, text_x + 90);
    writeText(sdlRenderer, "Press the A button to calibrate the right joystick, B for the left joystick", font24, 100, text_x + 150);

    SDL_SetRenderDrawColor(sdlRenderer, 150, 150, 150, 255);

    int radius = 120;
    int spacing = 150;
    int circleX = ((SCREEN_WIDTH - (4 * radius) - spacing) / 2) + radius;
    int circleY = 270;

    drawCrosshair(sdlRenderer, circleX, circleY, radius);
    drawCrosshair(sdlRenderer, SCREEN_WIDTH - circleX, circleY, radius);

    // Draw joystick indicator
    SDL_SetRenderDrawColor(sdlRenderer, 0, 252, 207, 255);

    // Left joystick
    int leftX = sdlAxis.axis0;
    int leftY = sdlAxis.axis1;
    leftX = (int)map((double)leftX, (double)-32768, (double)32767, (double)radius*-1, (double)radius);
    leftY = (int)map((double)leftY, (double)-32768, (double)32767, (double)radius*-1, (double)radius);

    int jLX = circleX + leftX;
    int jLY = circleY + leftY; 

    drawFilledCircle(sdlRenderer, jLX, jLY, 8);

    // right joystick
    int rghtX = sdlAxis.axis3;
    int rghtY = sdlAxis.axis2;
    rghtX = (int)map((double)rghtX, (double)-32768, (double)32767, (double)radius*-1, (double)radius);
    rghtY = (int)map((double)rghtY, (double)-32768, (double)32767, (double)radius*-1, (double)radius);
    int jRX = SCREEN_WIDTH - circleX + rghtX;
    int jRY = circleY + rghtY;
    
    drawFilledCircle(sdlRenderer, jRX, jRY, 8);

    if (sdlAxis.button_a)
    {
        *state = nextState;
        sdlAxis.button_a = 0;
        return 1;
    }
    if (sdlAxis.button_b)
    {
        *state = nextState;
        sdlAxis.button_b = 0;
        return 0;
    }

    return 0;
}

void joystickTutorialPanel(uint8_t *state, uint8_t nextState)
{
    int text_x = 140;
    writeText(sdlRenderer, "Read the following instructions:", font24, 100, text_x);
    writeText(sdlRenderer, "1. In the next section, you will need to rotate your stick for about 5 seconds.", font20, 100, text_x + 50);
    writeText(sdlRenderer, "2. Afterwards, let the joystick rest to capture its zero position. Is important to not touch the stick", font20, 100, text_x + 90);
    writeText(sdlRenderer, "Press the A button to continue with the calibration", font24, 100, text_x + 150);

    if (sdlAxis.button_a)
    {
        *state = nextState;
        sdlAxis.button_a = 0;
    }
}

void joystickCalibrationPanel(/*joypad_struct_t *joypad, joypad_cali_t *cali,*/ uint8_t resetTimer, uint8_t *state, uint8_t nextState)
{
    static double dTime = 0;
    static uint8_t isFirstFrame = 1;
    const uint8_t timeout = 6;

    int text_x = 140;
    writeText(sdlRenderer, "Please rotate your joystick now!", font24, 100, text_x);
    writeText(sdlRenderer, "Keep applying circular motions until the timer expires", font20, 100, text_x + 50);

    SDL_Rect r;
    r.w = 800;
    r.h = 100;
    r.x = CERTER_HOR(r.w);
    r.y = CERTER_VER(r.h);
    SDL_SetRenderDrawColor(sdlRenderer, 150, 150, 150, 255);
    SDL_RenderFillRect(sdlRenderer, &r);

    if(resetTimer)
    {
        isFirstFrame = 1;
    }

    dTime += deltaTime;
    if(isFirstFrame)
    {
        isFirstFrame = 0;
        dTime = 0;
    }

    r.w = (int)map(dTime, 0, (double)timeout, 0, (double)800);
    
    SDL_SetRenderDrawColor(sdlRenderer, 0, 201, 165, 125);
    SDL_RenderFillRect(sdlRenderer, &r);

    if(dTime >= timeout)
    {
        dTime = 0;
        *state = nextState;
    }
}

void joystickDeathZonePanel(/*joypad_struct_t *joypad, joypad_cali_t *cali,*/ uint8_t resetTimer, uint8_t *state, uint8_t nextState)
{
    static double dTime = 0;
    static uint8_t isFirstFrame = 1;
    const uint8_t timeout = 3;

    if(resetTimer)
    {
        isFirstFrame = 1;
    }

    int text_x = 140;
    writeText(sdlRenderer, "Don't touch your joystick!", font24, 100, text_x);
    writeText(sdlRenderer, "Calibrating zero position", font20, 100, text_x + 50);

    SDL_Rect r;
    r.w = 800;
    r.h = 100;
    r.x = CERTER_HOR(r.w);
    r.y = CERTER_VER(r.h);
    SDL_SetRenderDrawColor(sdlRenderer, 150, 150, 150, 255);
    SDL_RenderFillRect(sdlRenderer, &r);

    dTime += deltaTime;

    if(isFirstFrame)
    {
        isFirstFrame = 0;
        dTime = 0;
    }

    r.w = (int)map(dTime, 0, (double)timeout, 0, (double)800);
    
    SDL_SetRenderDrawColor(sdlRenderer, 0, 201, 165, 125);
    SDL_RenderFillRect(sdlRenderer, &r);

    if (dTime >= timeout)
    {
        dTime = 0;
        *state = nextState;
    }
}

void joySaving(int progress, const char *action)
{   
    drawUI(1);

    int text_x = 140;
    writeText(sdlRenderer, "Applying changes:", font24, 100, text_x);
    writeText(sdlRenderer, action, font20, 100, text_x + 50);

     SDL_Rect r;
    r.w = 800;
    r.h = 100;
    r.x = CERTER_HOR(r.w);
    r.y = CERTER_VER(r.h);
    SDL_SetRenderDrawColor(sdlRenderer, 150, 150, 150, 255);
    SDL_RenderFillRect(sdlRenderer, &r);

    r.w = (int)map(progress, 0, 100.0, 0, 800.0);
    
    SDL_SetRenderDrawColor(sdlRenderer, 0, 201, 165, 125);
    SDL_RenderFillRect(sdlRenderer, &r);

    SDL_RenderPresent(sdlRenderer);
}

void joystickSavePanel(uint8_t *state, uint8_t nextState)
{
    int text_x = 140;
    writeText(sdlRenderer, "Calibration has finished:", font24, 100, text_x);
    writeText(sdlRenderer, "Press A to return to the main menu", font20, 100, text_x + 50);
    writeText(sdlRenderer, "Press X to close the tool", font20, 100, text_x + 90);

    if (sdlAxis.button_a)
    {
        *state = nextState;
        sdlAxis.button_a = 0;
    }
}