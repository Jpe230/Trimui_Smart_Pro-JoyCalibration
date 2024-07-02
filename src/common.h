// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <unistd.h>

/**
 * Struct for the state of each of the buttons
 *
 * Note: BTN_F1 is unused in the left side
 */
typedef struct {
    uint8_t BTN_BUMPER : 1;
    uint8_t BTN_TRIGGER : 1;
    uint8_t BTN_NORTH : 1;
    uint8_t BTN_WEST : 1;
    uint8_t BTN_EAST : 1;
    uint8_t BTN_SOUTH : 1;
    uint8_t BTN_F1 : 1;
    uint8_t BTN_F2 : 1;
} joybutton_bitfield_t;


/**
 * Union for the bitfield
 */
typedef union
{
	uint8_t b;
	joybutton_bitfield_t bf;
} joybutton_t;
 

/**
 * Main Struct for the serial message:
 *
 * Header: Unknown meaning
 * Buttons: The bitfield for the button state
 * X & Y: The current ADC values
 */
typedef struct {
    uint16_t header;
    joybutton_t buttons;
    uint16_t x;
    uint16_t y;
} joypad_struct_t;
