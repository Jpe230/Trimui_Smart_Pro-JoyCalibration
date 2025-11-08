#include "serial-joystick.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

static int read_exact(int fd, uint8_t *buf, size_t want) {
    size_t got = 0;
    while (got < want) {
        ssize_t r = read(fd, buf + got, want - got);
        if (r > 0) {
            got += (size_t)r;
            continue;
        }
        if (r == 0) {
            // EOF (unusual for ttys, but handle it)
            return 0;
        }
        if (errno == EINTR) {
            continue; // interrupted by signal -> retry
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // timed out / no data right now under nonblocking or VTIME=0
            return 0;
        }
        // real error
        perror("read");
        return -1;
    }
    return (int)got;
}

int openSerialJoystick(const char *serialPort)
{
    // Open as blocking so VMIN/VTIME work
    int fd = open(serialPort, O_RDONLY | O_NOCTTY);
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

    // Make reads return after we have a full frame or a short timeout:
    // VMIN=7 -> block until 7 bytes are available
    // VTIME=1 -> or 0.1s timeout if fewer arrive (units are deciseconds)
    tty.c_cc[VMIN]  = 7;
    tty.c_cc[VTIME] = 1;

    // Apply now; flush pending I/O first so we start clean
    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
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
    uint8_t b[7];

    int got = read_exact(fd, b, sizeof b);
    if (got <= 0) {
        // 0 => timeout / no data yet; -1 => error already perror'ed
        return 0;
    }

    // Optional sync check
    if (b[0] != 0xFF || b[1] != 0x01) {
        // Not a header; you might want to resync by shifting until 0xFF 0x01
        return 0;
    }

    parseRawData(b, 7, j);
    return 1;
}
