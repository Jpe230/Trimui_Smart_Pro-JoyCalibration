// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "serial-joystick.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

int openJoypad(const char *serialPort)
{
    int fd = open(serialPort, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
    {
        perror("Error opening serial port");
        return fd;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("Error from tcgetattr");
        return -1;
    }
    
    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    tty.c_cflag |= CS8 | CREAD | CLOCAL;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        return -1;
    }

    return fd;
}

int closeJoypad(int fd)
{
    close(fd);
}

void parseRawData(uint8_t *b, uint8_t rb, joypad_struct_t *j)
{
    if (rb != 7)
    {
        return;
    }

    j->header = (b[0] << 8 | b[1]);
    j->buttons.b = b[2];
    j->x = (b[3] << 8 | b[4]);
    j->y = (b[5] << 8 | b[6]);
}

void readJoypad(int fd, joypad_struct_t *j)
{
    uint8_t r = 0;
    uint8_t b[8];
    ssize_t rb;

    int rtrs = 100; // Max retries
    do {
        rb = read(fd, b, sizeof(b)-1);
        
        // Discard invalid messages
        if (rb != 7 || b[0] != 0xFF || b[1] != 0x01)
        {
            rtrs--;
            continue;
        }

        parseRawData(b, 7, j);
        
        r = 1;
    }
    while(r == 0 && rtrs > 0);
}