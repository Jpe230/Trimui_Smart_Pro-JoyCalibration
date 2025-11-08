#include "serial-joystick.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

int openSerialJoystick(const char *serialPort)
{
    int fd = open(serialPort, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("Error opening serial port");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("Error from tcgetattr");
        close(fd);
        return -1;
    }

    // Put into raw mode first, then override specifics
    cfmakeraw(&tty);

    // Baud: BAUD_RATE must be a speed_t like B115200 (not 115200)
    cfsetispeed(&tty, BAUD_RATE);
    cfsetospeed(&tty, BAUD_RATE);

    // Control flags
    tty.c_cflag |= (CLOCAL | CREAD | CS8);
    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS); // 8N1, no HW flow control

    // Input flags: no SW flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Non-blocking reads should return immediately with whatever is available.
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    // Apply now; flush pending I/O first so we start clean
    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        close(fd);
        return -1;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("Error getting serial port flags");
        close(fd);
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Error setting serial port to non-blocking");
        close(fd);
        return -1;
    }

    printf("Serial device opened correctly!\n");
    return fd;
}

int closeSerialJoystick(int fd)
{
    if (fd <= 0) return -1;
    return close(fd);
}

static inline uint16_t u16_from_be(uint8_t hi, uint8_t lo) {
    return (uint16_t)((hi << 8) | lo);
}

void parseRawData(const uint8_t *b, uint8_t rb, joypad_struct_t *j)
{
    if (rb != 7) return;

    j->header     = u16_from_be(b[0], b[1]);
    j->buttons.b  = b[2];
    j->x          = u16_from_be(b[3], b[4]);
    j->y          = u16_from_be(b[5], b[6]);
}

int readSerialJoypad(int fd, joypad_struct_t *j)
{
    static uint8_t frameBuf[7];
    static size_t framePos = 0;
    static int trackedFd = -1;
    uint8_t tmp[32];
    int parsed = 0;

    if (fd < 0 || j == NULL) {
        return -1;
    }

    if (trackedFd != fd) {
        trackedFd = fd;
        framePos = 0;
    }

    ssize_t r = read(fd, tmp, sizeof tmp);
    if (r < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            return 0;
        }
        perror("read");
        return -1;
    }
    if (r == 0) {
        return 0;
    }

    for (ssize_t i = 0; i < r; ++i) {
        uint8_t byte = tmp[i];

        if (framePos == 0) {
            if (byte != 0xFF) {
                continue;
            }
        } else if (framePos == 1) {
            if (byte != 0x01) {
                if (byte == 0xFF) {
                    frameBuf[0] = 0xFF;
                    framePos = 1;
                } else {
                    framePos = 0;
                }
                continue;
            }
        }

        frameBuf[framePos++] = byte;

        if (framePos == sizeof(frameBuf)) {
            parseRawData(frameBuf, sizeof(frameBuf), j);
            framePos = 0;
            parsed = 1;
        }
    }

    return parsed;
}
