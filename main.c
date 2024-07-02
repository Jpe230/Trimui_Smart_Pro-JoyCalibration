#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define BAUD_RATE B19200
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

int openSerialDevice(const char *device)
{
    int dev = open(device, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (dev == -1)
    {
        perror("Error opening serial port");
        return dev;
    }

    struct termios tty;
    if (tcgetattr(dev, &tty) != 0)
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

    if (tcsetattr(dev, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        return -1;
    }

    return  dev;
}


int main(int argc, char *argv[]) {

    int serial_port = -1;
    uint8_t buffer[8];
    ssize_t read_result;

    if( argc == 2)
    {
         serial_port = openSerialDevice(argv[1]);
    }
    else
    {
        serial_port = openSerialDevice("/dev/ttyS4");
    }

    if (serial_port == -1)
    {
        close(serial_port);
        return 1;
    }
   
    int x_max = 0;
    int x_min = 10000; //Big ass number
    int y_max = 0;
    int y_min = 10000; //Big ass number

    #define COUNTER 500
    int counter = COUNTER;

    printf("Rotate your joystick for about 5 seconds...\n");

    while(counter > 0)
    {
        read_result = read(serial_port, buffer, sizeof(buffer)-1);
        if (read_result != 7 || buffer[0] != 0xFF || buffer[1] != 0x01)
        {
            continue;
        }

        int current_x = (buffer[3] << 8 | buffer[4]);
        int current_y = (buffer[5] << 8 | buffer[6]);

        x_max = max(x_max, current_x);
        y_max = max(y_max, current_y);

        x_min = min(x_min, current_x);
        y_min = min(y_min, current_y);

        printf("\rx: %04d max_x: %04d min_x: %04d - y: %04d max_y: %04d min_y: %04d, TIME_LEFT: %03d", current_x, x_max, x_min, current_y, y_max, y_min, counter);
        fflush(stdout);
        usleep(10000);
        counter--;
    }

    printf("\nDon't touch your joystick!\n");
    sleep(3);

    int x_zero = 0;
    int y_zero = 0;

    int success = 0;
    do {
        read_result = read(serial_port, buffer, sizeof(buffer)-1);
        if (read_result != 7 || buffer[0] != 0xFF || buffer[1] != 0x01)
        {
            continue;
        }

        x_zero = (buffer[3] << 8 | buffer[4]);
        y_zero = (buffer[5] << 8 | buffer[6]);

        success = 1;
    }
    while(success == 0);

    printf("\nWrite the following to '/mnt/UDISK/joypad(_right).config': \n");
    printf("x_min=%d\n", x_min);
    printf("x_max=%d\n", x_max);
    printf("y_min=%d\n", y_min);
    printf("y_max=%d\n", y_max);
    printf("x_zero=%d\n", x_zero);
    printf("y_zero=%d\n", y_zero);

    close(serial_port);

    return 0;
}
