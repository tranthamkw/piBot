#!/bin/bash

cd /home/pi/piBot/piBoard4/

sudo ./testBotMotor "$1" "$2" 2>&1

echo "done"
