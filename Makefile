CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lSDL2 -lSDL2_ttf

SRCDIR = src
BUILDDIR = build
OBJDIR = build/obj
BINDIR = build/bin

TARGET = TSP_JoyCalibration
#SRCS = $(SRCDIR)/main.c $(SRCDIR)/gfx/gfx_dev.c $(SRCDIR)/gfx/panel.c $(SRCDIR)/gfx/utils/primitives.c $(SRCDIR)/serial/serial_joystick_dev.c 
#SRCS = $(SRCDIR)/main.c $(SRCDIR)/cali/cali.c $(SRCDIR)/gfx/gfx.c $(SRCDIR)/gfx/panel.c $(SRCDIR)/gfx/utils/primitives.c $(SRCDIR)/serial/serial-joystick.c 
SRCS = $(SRCDIR)/main.c $(SRCDIR)/cali/cali.c $(SRCDIR)/gfx/gfx.c $(SRCDIR)/gfx/panel.c $(SRCDIR)/gfx/utils/primitives.c $(SRCDIR)/serial/serial-joystick.c $(SRCDIR)/daemon/daemon.c
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

MKDIR_P ?= mkdir -p