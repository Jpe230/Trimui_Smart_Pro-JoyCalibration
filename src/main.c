// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include "serial/serial-joystick.h"

int main(int argc, char *argv[]) 
{
    int serial_port = openJoypad(argv[1]);
    //while(1)
    //{
        // Ask user for which joystick

        // Run calibration tool
            // Rotation
            
            // Zero point

            // Print results

            // Save (?)

        // If saved restart services
    //}

    return 0;
}