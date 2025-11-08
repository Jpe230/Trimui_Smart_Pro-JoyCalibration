#!/bin/sh

# Applies joystick calibration values supplied via arguments.
# Usage: apply_calibration.sh <joystick_id> <x_min> <x_max> <y_min> <y_max> <x_zero> <y_zero>

set -eu

if [ "$#" -ne 7 ]; then
    echo "Usage: $0 <joystick_id> <x_min> <x_max> <y_min> <y_max> <x_zero> <y_zero>" >&2
    exit 1
fi

JOYSTICK_ID="$1"
shift
X_MIN="$1"
X_MAX="$2"
Y_MIN="$3"
Y_MAX="$4"
X_ZERO="$5"
Y_ZERO="$6"

TARGET_DIR="/userdata/system/config/trimui-input"
INIT_SCRIPT="/etc/init.d/S12trimuiinput"

mkdir -p "$TARGET_DIR"

case "$JOYSTICK_ID" in
    0) TARGET_FILE="$TARGET_DIR/joypad.config" ;;
    1) TARGET_FILE="$TARGET_DIR/joypad_right.config" ;;
    *) TARGET_FILE="$TARGET_DIR/joypad_${JOYSTICK_ID}.config" ;;
esac

cat > "$TARGET_FILE" <<EOF
x_min=$X_MIN
x_max=$X_MAX
y_min=$Y_MIN
y_max=$Y_MAX
x_zero=$X_ZERO
y_zero=$Y_ZERO
EOF

if [ -x "$INIT_SCRIPT" ]; then
    "$INIT_SCRIPT" stop >/dev/null 2>&1 || true
    "$INIT_SCRIPT" start >/dev/null 2>&1 || true
else
    echo "Warning: $INIT_SCRIPT not found or not executable" >&2
fi
