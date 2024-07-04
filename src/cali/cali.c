// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cali.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

void clearData(joypad_struct_t *joypad, joypad_cali_t *cali)
{
    joypad->header = 0;
    joypad->buttons.b = 0;
    joypad->x = 0;
    joypad->y = 0;

    cali->x_min=10000; // big ass number
    cali->x_max=0;
    cali->y_min=10000; // big ass number
    cali->y_max=0;
    cali->x_zero=0;
    cali->y_zero=0;
}

void calculateMinMax(joypad_struct_t *joypad, joypad_cali_t *cali)
{
    cali->x_max=max(cali->x_max, joypad->x);
    cali->y_max=max(cali->y_max, joypad->y);

    if(joypad->x > 0)
    {
        cali->x_min=min(cali->x_min, joypad->x);
    }
    if(joypad->y > 0)
    {
        cali->y_min=min(cali->y_min, joypad->y);
    }
}

void calculateZero(joypad_struct_t *joypad, joypad_cali_t *cali) 
{
    cali->x_zero=joypad->x;
    cali->y_zero=joypad->y;
}