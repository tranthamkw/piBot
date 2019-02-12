#!/bin/sh

#fswebcam -d /dev/video1 -S 5 -p YUYV -r 640x420 "$1left.jpg"
echo "LEFT CAMERA:\n";
fswebcam -d /dev/video1 -i 0 -S 10 -p YUYV --no-banner --flip v,h "$1left.jpg"
echo "\n\nRIGHT CAMERA:\n";
fswebcam -d /dev/video0 -i 0 -S 10 -p YUYV --no-banner "$1right.jpg"
