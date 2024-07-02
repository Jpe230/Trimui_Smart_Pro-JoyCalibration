# Trimui Smart Pro Joystick Calibration

Small POC to calibrate the joysticks for the TSP

## Compile

Use a rooffs/crosscompiler:

I personally use my [Sonic Pad rootfs creator](https://github.com/Jpe230/SonicPad-Debian/) for my dev enviroment (same SoC BTW)

To compile
```bash
gcc main.c -o joycalibration
```

## Usage

To calibrate the sticks run:

For the left stick
```
./joycalibration /dev/ttyS4
```

For the right stick
```
./joycalibration /dev/ttyS3
```

## Example 

```bash
[root@TrimuiSmartPro /userdata/roms/ports]# ./joycalibration /dev/ttyS4

Rotate your joystick for about 5 seconds...
x: 1938 max_x: 3170 min_x: 0742 - y: 1057 max_y: 3397 min_y: 1045, TIME_LEFT: 001
Don't touch your joystick!

Write the following to '/mnt/UDISK/joypad(_right).config':
x_min=742
x_max=3170
y_min=1045
y_max=3397
x_zero=2035
y_zero=2201

[root@TrimuiSmartPro /userdata/roms/ports]#

```



Write the results to the corresponding file
