#!/bin/bash

DIR="$(dirname "$(readlink -f "$0")")"
 
cd "${DIR}/bin"

chmod +x ./TSP_Calibration
./TSP_Calibration

# Kill emulatorlauncher, it doesnt exit, maybe because we are forking the proccess (?)
ps -ef | grep /usr/bin/emulatorlauncher | grep TSP_Calibration.sh | grep -v grep | awk '{print $2}' | xargs kill

exit 0