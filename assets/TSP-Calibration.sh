#!/bin/bash

DIR="$(dirname "$(readlink -f "$0")")"
 
cd "${DIR}/TSP-Calibration"

./TSP-Calibration

# Kill emulatorlauncher, it doesnt exits for whatever reason *?)
ps -ef | grep /usr/bin/emulatorlauncher | grep TSP-Calibration.sh | grep -v grep | awk '{print $2}' | xargs kill

exit 0