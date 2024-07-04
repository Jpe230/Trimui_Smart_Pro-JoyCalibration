// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius);

void drawFilledCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius);

void drawCrosshair(SDL_Renderer *renderer, int centerX, int centerY, int radius);

void writeText(SDL_Renderer *renderer, const char *text, TTF_Font *font, int x, int y);