#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "piBoard.h"
#include "RS485Devices.h"
#include "botDefines.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace cv;
using namespace std;

// function declaration
// TODO int menu();
float detectObj();
float calculateDist();
unsigned int measureDistance(unsigned char);
void moveBot(unsigned int);

// global variables
unsigned int dir1,dir0,brake,speed1,speed0,usmAngle;
unsigned int value,temp,k;
unsigned int speed,dircode,frontD,leftD,rightD,mode;
float volts,stdev;
float leftX, rightX, leftY, rightY;
float sumLY = 0.0f;
float sumLX = 0.0f;
float sumRY = 0.0f;
float sumRX = 0.0f;
float dist = 0.0f;
float diffX = 0.0f;
const float K1 = 3998.12f;
const float K2 = -0.87f;

int main(int argc, char* argv[]) {

// TODO use USM front distance rather than calculateDist

  initializeBoard();

  brake = 0;
  speed = 70;
  usmAngle = 5;
  value = 4;
  setRS485ServoPosition(USM,0,usmAngle);
  setRS485ServoPosition(CAMERASERVO,0,value);

  frontD = measureDistance(0);
  mode = 1;

  while (frontD > 30) {
    frontD = measureDistance(0);
    rightD = measureDistance(1);
    leftD = measureDistance(2);
    dist = calculateDist();

    //TODO Camera tilt: every 36-40 pixels
    unsigned int valueY = (unsigned int)(avgLY + avgRY) / 2;
    if (valueY >= 0 && valueY < 40) {
      value = 8;
    } else if (valueY >= 40 && valueY < 80) {
      value = 7;
    } else if (valueY >= 80 && valueY < 120) {
      value = 6;
    } else if (valueY >= 120 && valueY < 160) {
      value = 5;
    } else if (valueY >= 160 && valueY < 200) {
      value = 4;
    } else if (valueY >= 200 && valueY < 240) {
      value = 3;
    } else if (valueY >= 240 && valueY < 280) {
      value = 2;
    } else if (valueY > 280) {
      value = 1;
    }

    setRS485ServoPosition(CAMERASERVO,0,value);

    if ((mode > 0) && (mode < 1) && (dist > 60.0)) {
      // TODO use rightD and leftD to detect objects and avoid / will change to cameras later
      if ((leftD + rightD) > 200) {
        mode = 1;
        if (leftD < 95) mode = 2; //vere right
				if (rightD < 95) mode = 3; //vere left
			} else { // between two walls.  Try to even the distance between
	       if (leftD > rightD) {
          mode = 2;
        } else {
          mode = 3;
        }
      }

      // follow ball forward and veer right/left based on avg x axis
      if ((avgLX < 132) && avgRX < 132) { // veer right
        mode = 2;
      } else if ((avgLX > 220) && (avgRX > 220)) { // veer left
        mode = 3;
      } else if ((avgLX + avgRX > 264) && (avgLX + avgRX < 440)) {
        mode = 1;
      }
    } else if ((mode > 0) && (mode < 4) && (dist < 60) && (frontD > 30))  {
			if ((leftD + rightD) > 200) {
				mode = 1;// move forward
				if (leftD < 95) mode = 2; //vere right
				if (rightD < 95) mode = 3; //vere left
			} else { // between two walls.  Try to even the distance between
	       if (leftD > rightD) {
          mode = 2;
        } else {
          mode = 3;
        }
			}
	  } else if (frontD < 31) { // Stop bot
        mode = 0;
    }
      moveBot(mode);
  }

    readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
	  printf("volts %2.2f +/- %1.2f\n",volts,stdev);

    return 0;
}

// TODO: code for main menu to be added later
// int menu() {
//   // int select = 0;
//   // char pressed = '';
//   // printf("Good day Dr. Trantham, what would you like to do today?\n");
//   // printf("Press A to collect data.\n");
//   // printf("Press Q to quit.");
//   // scanf("%c\n", &pressed);
//   // if(pressed == 'a' || pressed == 'A') {
//   //   select = 1;
//   // }
//   // return dist;
// }

void moveBot(unsigned int mode) {
  switch (mode){
    case 0: // STOP
      speed0=0;
      speed1=0;
      dir0 = 1;
      dir1 = 1;
    break;
    case 1:
      speed0=speed;
      speed1=speed;
      dir0 = 1;
      dir1 = 1;
    break;
    case 2: // vere right
      speed0=speed/2;
      speed1=speed;
      dir0 = 1;
      dir1 = 1;
    break;
    case 3: // vere left
      speed0=speed;
      speed1=speed/2;
      dir0 = 1;
      dir1 = 1;
    break;
    case 4: // spin right
      speed0=speed;
      speed1=speed;
      dir0 = 0;
      dir1 = 1;
    break;
    case 5: // spin left
      speed0=speed;
      speed1=speed;
      dir0 = 1;
      dir1 = 0;
    break;
  }
  setLMD18200Status(MOTOR0, brake, dir0, speed0);
  setLMD18200Status(MOTOR1, brake, dir1, speed1);
  return;
}

float calculateDist() {
  // calcDist set to 0.0 means object is really close.
  float calcDist = 0.0;
  float changeX = (abs(detectObj()));
  if (changeX < 10) {
    // calcDist set to INFINITY means object is far away
    calcDist = INFINITY;
  } else if (diffX >= 10 && diffX <= 100) {
    // physics calculation to determine objects distance from bot...
    calcDist = K1 * pow(changeX, K2);
  }
  return calcDist;
}

float detectObj() {

  VideoCapture right(0);  // open video capture for dev0 - rightCam
  VideoCapture left(1);   // open video capture for dev0 - leftCam

  left.set(CAP_PROP_FPS, 20); // capture 20 frames per second
  right.set(CAP_PROP_FPS, 20); // capture 20 frames per second

  // verify both cams video capture is opened.
  if(!left.isOpened() || !right.isOpened()) {
    cout << "Unable to open a webcam. Exited objTracker." << endl;
    return -1;
  }

  // variables for captured frames
  Mat leftCam, rightCam, hsvLeft, hsvRight, mask, mask2, leftVid, rightVid;

  float scalingFactor = 0.75;

  // capture 20 frames and calculate average Rx, Ry, Lx, Ly
  for(int i = 0; i < 10; i++ ) {
    leftVid = Scalar(0, 0, 0);
    rightVid = Scalar(0, 0, 0);

    left >> leftCam;
    right >> rightCam;

    // flip left cam x and y axis so image same orientation as right cam
    flip(leftCam, leftCam, -1);

    // check if fram has been captured
    if(leftCam.empty() || rightCam.empty())
      break;

      // adjust size of captured frame
      resize(leftCam, leftCam, Size(), scalingFactor, scalingFactor, INTER_AREA);
      resize(rightCam, rightCam, Size(), scalingFactor, scalingFactor, INTER_AREA);

      // convert image color from BGR to HSV
      cvtColor(leftCam, hsvLeft, COLOR_BGR2HSV); // converts to HSV color
      cvtColor(rightCam, hsvRight, COLOR_BGR2HSV); // converts to HSV color

      // set low and high thresholds for color of blob to detect.
      Scalar low = Scalar(60, 100, 100);
      Scalar high = Scalar(180, 255, 255);

      // check converted image for blob within threshold
      inRange(hsvLeft, low, high, leftVid);
      inRange(hsvRight, low, high, rightVid);

      // method calls to remove unwanted noise from image
      erode(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      dilate(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      dilate(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      erode(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

      erode(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      dilate(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      dilate(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      erode(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

      // return x, y coords for blob detected from left and right camera
      Moments leftMoments = moments(leftVid);
      Moments rightMoments = moments(rightVid);

      double leftM01 = leftMoments.m01;
      double leftM10 = leftMoments.m10;
      double leftArea = leftMoments.m00;

      double rightM01 = rightMoments.m01;
      double rightM10 = rightMoments.m10;
      double rightArea = rightMoments.m00;

      // if area is less than 10000 assume blob not detected/correct
      if (leftArea > 10000 && rightArea > 10000) {
        leftX = leftM10 / leftArea;
        leftY = leftM01 / leftArea;
        rightX = rightM10 / rightArea;
        rightY = rightM01 / rightArea;

        // sum detected Lx, Ly, Rx, Ry
        sumLX += leftX;
        sumLY += leftY;
        sumRX += rightX;
        sumRY += rightY;

      }
  }

  // find avg Lx, Ly, Rx, Ry
  float avgLX = sumLX / 10;
  float avgRX = sumRX / 10;
  float avgLY = sumLY / 10;
  float avgRY = sumRY / 10;

// calc difference between Lx and Rx, Ly and Ry
  float xDiff = avgRX - avgLX;
  float yDiff = avgRY - avgLY;

// reset sums to 0
  sumLX = 0.0f;
  sumLY = 0.0f;
  sumRX = 0.0f;
  sumRY = 0.0f;
// return difference of Lx and Rx for distance calculation.
  return xDiff;
}

unsigned int measureDistance(unsigned char chan){
	int status;
	unsigned int value;
	status = getRS485USMdistance(USM+chan,&value);
	if (status) value = 0;
	return (value*34/500); // distance in cm
}
