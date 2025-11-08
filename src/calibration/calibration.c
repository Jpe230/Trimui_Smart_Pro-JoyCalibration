// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "calibration.h"
#include <stdio.h>
#include <stdlib.h>

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

void calculateJoystickMinMax(joypad_struct_t *joypad, joypad_cali_t *cali)
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

void calculateJoystickZeroPosition(joypad_struct_t *joypad, joypad_cali_t *cali) 
{
    cali->x_zero=joypad->x;
    cali->y_zero=joypad->y;
}

void writeCali(joypad_cali_t *cali, const char* fileCal)
{
    FILE *fp = fopen(fileCal, "w");
    if (fp == NULL) {
        perror("Error opening file");
    }

    fprintf(fp, "x_min=%d\n", cali->x_min);
    fprintf(fp, "x_max=%d\n", cali->x_max);
    fprintf(fp, "y_min=%d\n", cali->y_min);
    fprintf(fp, "y_max=%d\n", cali->y_max);
    fprintf(fp, "x_zero=%d\n", cali->x_zero);
    fprintf(fp, "y_zero=%d\n", cali->y_zero);

    fclose(fp);
}