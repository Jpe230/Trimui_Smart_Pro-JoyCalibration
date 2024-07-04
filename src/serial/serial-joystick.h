// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../common.h"

/**
 * Opens the serial device for the *d* joypad
 *
 * @param serialPort[in] the port of the serial device (ttySX)
 * @return file descriptor if successful
 * @return -1 if error
 */
int openJoypad(const char *serialPort);

/**
 * Closes the file descriptor
 *
 * @param fd[in] the handle of the device to close
 * @return 1 if successful
 * @return -1 if error
 */
int closeJoypad(int fd);

/**
 * Reads Joypad raw data
 *
 * @param fd[in] the handle of the device to read
 * @param j[in] joypad struct
 */
int readJoypad(int fd, joypad_struct_t *j);