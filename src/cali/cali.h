// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../common.h"

/**
 * Calculates the min and max position of the stickes
 *
 * @param joypad[in] the current values
 * @param cali[out] the calculated values
 */
void calculateMinMax(joypad_struct_t *joypad, joypad_cali_t *cali);

/**
 * Calculates the zero position of the stickes
 *
 * @param joypad[in] the current values
 * @param cali[out] the calculated values
 */
void calculateZero(joypad_struct_t *joypad, joypad_cali_t *cali);

/**
 * Clears the data of the input parameters
 *
 * @param joypad[in] the current values
 * @param cali[in] the calculated values
 */
void clearData(joypad_struct_t *joypad, joypad_cali_t *cali);

/**
 * Prints the data into a file
 *
 * @param joypad[in] the current values
 * @param cali[in] the calculated values
 */
void writeCali(joypad_cali_t *cali, const char *caliFile);