#!/bin/bash

cd /home/pi/piBot/piBoard4/

sudo ./setCamAngle "$1" 2>&1
sudo ./getBatteryStatus

echo "done"
