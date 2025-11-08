// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <inttypes.h>

/**
 * Baud rate for the serial devices
 */
#define BAUD_RATE B19200

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

/**
 * Struct for the calibration data
 */
typedef struct {
    uint16_t x_min;
    uint16_t x_max;
    uint16_t y_min;
    uint16_t y_max;
    uint16_t x_zero;
    uint16_t y_zero;
} joypad_cali_t;

/**
 * Helper struct for SDL Inputs
 */
typedef struct {
    int axis0;
    int axis1;
    int axis2;
    int axis3;
    uint8_t button_a;
    uint8_t button_b;
} sdl_axis_t;
