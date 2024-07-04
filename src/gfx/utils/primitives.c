// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "primitives.h"
#include <math.h>

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void drawFilledCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{

    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void drawCrosshair(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    drawCircle(renderer, centerX, centerY, radius);
    
    int w = (radius * 2) + 40;

    SDL_Rect r;

    r.w = w;
    r.h = 2;
    r.x = centerX - (w / 2);
    r.y = centerY - 1;
    
    SDL_SetRenderDrawColor(renderer, 0, 201, 165, 125);
    SDL_RenderFillRect(renderer, &r);

    r.h = w;
    r.w = 2;
    r.y = centerY - (w / 2);
    r.x = centerX - 1;

    SDL_RenderFillRect(renderer, &r);
}

void writeText(SDL_Renderer *renderer, const char *text, TTF_Font *font, int x, int y)
{
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* txtr = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    SDL_QueryTexture(txtr, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, txtr, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(txtr);
}