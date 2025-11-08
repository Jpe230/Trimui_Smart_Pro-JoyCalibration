// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfx.h"

TTF_Font *font32 = NULL;
TTF_Font *font26 = NULL;
TTF_Font *font24 = NULL;
TTF_Font *font20 = NULL;

SDL_Joystick* joystick;

#define JOYSTICK_CONFIG_PATH "./assets/joystick.ini"

typedef struct {
    int button_exit;
    int button_a;
    int button_b;
    int axis0;
    int axis1;
    int axis2;
    int axis3;
} joystick_config_t;

static joystick_config_t joystickConfig = {
    .button_exit = 2,
    .button_a = 0,
    .button_b = 1,
    .axis0 = 0,
    .axis1 = 1,
    .axis2 = 3,
    .axis3 = 2,
};

static char *trim_whitespace(char *str)
{
    if (str == NULL) return str;

    while (isspace((unsigned char)*str)) {
        ++str;
    }

    if (*str == '\0') {
        return str;
    }

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }

    return str;
}

static void loadJoystickConfig(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Using default joystick mapping (missing %s).\n", path);
        return;
    }

    char line[256];
    while (fgets(line, sizeof line, fp) != NULL) {
        char *comment = strpbrk(line, "#;");
        if (comment) {
            *comment = '\0';
        }

        char *trimmed = trim_whitespace(line);
        if (*trimmed == '\0' || *trimmed == '[') {
            continue;
        }

        char *eq = strchr(trimmed, '=');
        if (!eq) {
            continue;
        }

        *eq = '\0';
        char *key = trim_whitespace(trimmed);
        char *value = trim_whitespace(eq + 1);
        if (*value == '\0') {
            continue;
        }

        int idx = atoi(value);

        if (strcmp(key, "button_exit") == 0) {
            joystickConfig.button_exit = idx;
        } else if (strcmp(key, "button_a") == 0) {
            joystickConfig.button_a = idx;
        } else if (strcmp(key, "button_b") == 0) {
            joystickConfig.button_b = idx;
        } else if (strcmp(key, "axis0") == 0) {
            joystickConfig.axis0 = idx;
        } else if (strcmp(key, "axis1") == 0) {
            joystickConfig.axis1 = idx;
        } else if (strcmp(key, "axis2") == 0) {
            joystickConfig.axis2 = idx;
        } else if (strcmp(key, "axis3") == 0) {
            joystickConfig.axis3 = idx;
        }
    }

    fclose(fp);
}

void initSDL(SDL_Window **sdlWindow, SDL_Renderer **sdlRenderer)
{
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles");

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN, sdlWindow, sdlRenderer);

	if (sdlWindow == NULL) {
        printf("Error while creating window\n");
		exit(1);
	}

	if (sdlRenderer == NULL) {
        printf("Error while creating renderer\n");
		exit(1);
	}

	TTF_Init();

    font32 = TTF_OpenFont(FONT, 32);
    font26 = TTF_OpenFont(FONT, 26);
    font24 = TTF_OpenFont(FONT, 24);
    font20 = TTF_OpenFont(FONT, 20);

    loadJoystickConfig(JOYSTICK_CONFIG_PATH);

	openJoystick();

	if (joystick == NULL)
	{
		printf("Error while opening joystick\n");
		exit(1);
	}
}

void cleanScreen(SDL_Renderer *sdlRenderer)
{
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer);
    SDL_RenderPresent(sdlRenderer);
}

void openJoystick()
{
	if( SDL_NumJoysticks() >= 1 )
	{
		joystick = SDL_JoystickOpen(0);
	}
}

void closeJoystick()
{
	if(joystick)
	{
		SDL_JoystickClose(joystick);
	}
	joystick = NULL;
}

void cleanUpSDL(SDL_Window *sdlWindow, SDL_Renderer *sdlRenderer)
{
	closeJoystick();
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
	SDL_Quit();
}

uint8_t pollSdlJoystick(SDL_Event *event, sdl_axis_t *sdlAxis)
{
	while( SDL_PollEvent( event ) != 0 )
	{

		if (event->type == SDL_JOYDEVICEADDED)
		{
			printf("A Joystick was connected and assigned index %d.\n", event->jdevice.which);

			if (joystick == NULL)
			{	
				printf("Adding new joystick: %d.\n", event->jdevice.which);
				openJoystick();
			}
		}

		if(event->type == SDL_QUIT || (event->type == SDL_JOYBUTTONDOWN && event->jbutton.button == joystickConfig.button_exit))
		{
			printf("Exiting\n");
			return 0;
		}

		if(event->type == SDL_JOYBUTTONDOWN)
		{
            if (event->jbutton.button == joystickConfig.button_a)
            {
                sdlAxis->button_a = 1;
            }
            else if (event->jbutton.button == joystickConfig.button_b)
            {
                sdlAxis->button_b = 1;
            }
		}

		if(event->type == SDL_JOYBUTTONUP)
		{
            if (event->jbutton.button == joystickConfig.button_a)
            {
                sdlAxis->button_a = 0;
            }
            else if (event->jbutton.button == joystickConfig.button_b)
            {
                sdlAxis->button_b = 0;
            }
		}

		if(event->type == SDL_JOYAXISMOTION)
		{
            const int axis = event->jaxis.axis;
            if (axis == joystickConfig.axis0)
            {
                sdlAxis->axis0 = event->jaxis.value;
            }
            else if (axis == joystickConfig.axis1)
            {
                sdlAxis->axis1 = event->jaxis.value;
            }
            else if (axis == joystickConfig.axis2)
            {
                sdlAxis->axis2 = event->jaxis.value;
            }
            else if (axis == joystickConfig.axis3)
            {
                sdlAxis->axis3 = event->jaxis.value;
            }
		}
	}

	return 1;
}
