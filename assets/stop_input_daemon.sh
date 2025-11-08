#!/bin/sh

# Stops the Trimui input daemon so the raw serial device can be accessed safely.

set -eu

INIT_SCRIPT="/etc/init.d/S12trimuiinput"

if [ -x "$INIT_SCRIPT" ]; then
    "$INIT_SCRIPT" stop >/dev/null 2>&1 || true
else
    echo "Warning: $INIT_SCRIPT not found or not executable" >&2
fi
