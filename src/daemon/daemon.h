// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../common.h"

/**
 * Stops a daemon given its name
 *
 * @param daemonName[in] Daemon to kill
 */
void killDaemon(const char * daemonName);

/**
 * Starts a daemon given its name
 *
 * @param daemonName[in] Daemon to start
 */
void startDaemon(const char * daemonName);
