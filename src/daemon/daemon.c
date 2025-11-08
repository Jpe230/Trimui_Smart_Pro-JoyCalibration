// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "daemon.h"
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

pid_t getPID(const char *daemonName)
{
    FILE *pipe;
    char command[50];
    char pid_str[10];
    
    snprintf(command, sizeof(command), "pgrep -f %s", daemonName);
    pipe = popen(command, "r");
    if (pipe == NULL) {
        return -1;
    }
    if (fgets(pid_str, sizeof(pid_str), pipe) == NULL) {
        pclose(pipe);
        return -1;
    }
    pid_t pid = (pid_t) atoi(pid_str);
    pclose(pipe);
    return pid;
}

void killDaemon(const char* daemonName) 
{
    pid_t pid = getPID(daemonName);
    if (pid > 0) 
    {
        if (kill(pid, SIGTERM) == 0) 
        {
            printf("Daemon '%s' (PID: %d) killed successfully.\n", daemonName, pid);
        } else 
        {
            perror("Error killing daemon");
        }
    } else 
    {
        printf("Daemon '%s' not found.\n", daemonName);
    }
}

void startDaemon(const char* daemonName)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        system(daemonName);
        exit(0);
    }
}
