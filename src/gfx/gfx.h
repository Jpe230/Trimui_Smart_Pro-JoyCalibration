// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../common.h"

/////////////////////////////////////////////////////////
///
/// Defines

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

/////////////////////////////////////////////////////////
///
/// Joypad Panels

/**
 * Draw the skeleton of the UI
 * @param sdlRenderer[in] SDL Renderer pointer
 */
void drawUI();


/**
 * Panel to select a joystick
 *
 * @param state[in] the pointer to the current state
 * @param nextState[in] the int value for the next panel
 * @return index for joypad
 */
uint8_t joySelectPanel(uint8_t *state, uint8_t nextState);

/**
 * Small tutorial panel
 *
 * @param state[in] the pointer to the current state
 * @param nextState[in] the int value for the next panel
 */
void joyTutorial(uint8_t *state, uint8_t nextState);

/**
 * Panel where the user rotates the stick to calcultate the min max values
 *
 * @param joypad[in] the current values
 * @param cali[in] the calculated values
 * @param resetTimer[in] bool to reset the timer to 0
 * @param state[in] the pointer to the current state
 * @param nextState[in] the int value for the next panel
 */
void joyCaliPanel(joypad_struct_t *joypad, joypad_cali_t *cali, uint8_t resetTimer, uint8_t *state, uint8_t nextState);

/**
 * Panel where the zero pos is calcultated
 *
 * @param joypad[in] the current values
 * @param cali[in] the calculated values
 * @param state[in] the pointer to the current state
 * @param nextState[in] the int value for the next panel
 */
void joyZeroPanel(joypad_struct_t *joypad, joypad_cali_t *cali, uint8_t resetTimer,uint8_t *state, uint8_t nextState);

/**
 * Panel where we ask the user if we have to save the values
 *
 * @param state[in] the pointer to the current state
 * @param nextState[in] the int value for the next panel
 * @param resetState[in] the int value for the first panel
 */
void joySave(uint8_t *state, uint8_t nextState);

/////////////////////////////////////////////////////////
///
/// SDL Helpers

/**
 * Initialize SDL
 *
 * @param sdlWindow[in] SDL WIndow pointer
 * @param sdlRenderer[in] SDL Renderer pointer
 */
void initSDL(SDL_Window **sdlWindow, SDL_Renderer **sdlRenderer);

/**
 * Blanks the screen
 *
 * @param sdlRenderer[in] SDL Renderer pointer
 */
void cleanScreen(SDL_Renderer *sdlRenderer);

/**
 * De-init SDL Window
 *
 * @param sdlWindow[in] SDL WIndow pointer
 * @param sdlRenderer[in] SDL Renderer pointer
 */
void cleanUpSDL(SDL_Window *sdlWindow, SDL_Renderer *sdlRenderer);

/**
 * Poll controllers
 *
 * @param sdlWindow[in] SDL WIndow pointer
 * @param sdlRenderer[in] SDL Renderer pointer
 * @return int to exit
 */
uint8_t pollControl(SDL_Event *event, sdl_axis_t *sdlAxis);


/////////////////////////////////////////////////////////
///
/// Helpers

#define CERTER_HOR(w) ((SCREEN_WIDTH - w) / (2))
#define CERTER_VER(H) ((SCREEN_HEIGHT - H) / (2))
