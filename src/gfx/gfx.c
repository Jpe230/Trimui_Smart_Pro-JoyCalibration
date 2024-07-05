// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gfx.h"

extern TTF_Font *font32;
extern TTF_Font *font26;
extern TTF_Font *font24;
extern TTF_Font *font20;

SDL_Joystick* joystick;

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

    font32 = TTF_OpenFont("./font.ttf", 32);
    font26 = TTF_OpenFont("./font.ttf", 26);
    font24 = TTF_OpenFont("./font.ttf", 24);
    font20 = TTF_OpenFont("./font.ttf", 20);

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

uint8_t pollControl(SDL_Event *event, sdl_axis_t *sdlAxis)
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

		if(event->type == SDL_QUIT || (event->type == SDL_JOYBUTTONDOWN && event->jbutton.button == 3))
		{
			printf("Exiting\n");
			return 0;
		}

		if(event->type == SDL_JOYBUTTONDOWN)
		{
			switch(event->jbutton.button)
			{
				case 0:
					sdlAxis->button_b = 1;
					break;
				case 1:
					sdlAxis->button_a = 1;
					break;
			}
		}

		if(event->type == SDL_JOYBUTTONUP)
		{
			switch(event->jbutton.button)
			{
				case 0:
					sdlAxis->button_b = 0;
					break;
				case 1:
					sdlAxis->button_a = 0;
					break;
			}
		}

		if(event->type == SDL_JOYAXISMOTION)
		{
			switch(event->jaxis.axis)
			{
				case 0:
					sdlAxis->axis0 = event->jaxis.value;
					break;
				case 1:
					sdlAxis->axis1 = event->jaxis.value;
					break;
				case 4:
					sdlAxis->axis2 = event->jaxis.value;
					break;
				case 3:
					sdlAxis->axis3 = event->jaxis.value;
					break;
			}
		}
	}

	return 1;
}