CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lSDL2 -lSDL2_ttf

TARGET = TSP_Calibration

SRCDIR = src
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
BINDIR = $(BUILDDIR)/TSP_Calibration/$(TARGET)

ASSETS = assets
FONT = $(ASSETS)/font.ttf
LAUNCHSCRIPT = $(ASSETS)/TSP_Calibration.sh

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
	cp $(FONT) $(BINDIR)/
	cp $(LAUNCHSCRIPT) $(BINDIR)/../

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

MKDIR_P ?= mkdir -p