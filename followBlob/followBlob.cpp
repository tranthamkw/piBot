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
double calculateDist(double);
unsigned int measureDistance(unsigned char);
void moveBot(unsigned int);

// global variables
unsigned int dir1,dir0,brake,speed1,speed0,usmAngle;
unsigned int value,temp,k;
unsigned int speed,dircode,frontD,leftD,rightD,mode;
float volts,stdev;
double leftX, rightX, leftY, rightY;
double avgLX, avgRX, avgLY, avgRY;
double sumLY = 0.0f;
double sumLX = 0.0f;
double sumRY = 0.0f;
double sumRX = 0.0f;
double dist = 0.0f;
double diffX = 0.0f;
const double K1 = 3998.12f;
const double K2 = -0.87f;
const unsigned int STOP = 0;
const unsigned int FORWARD = 1;
const unsigned int VEER_RIGHT = 2;
const unsigned int VEER_LEFT = 3;
const unsigned int SPIN_RIGHT = 4;
const unsigned int SPIN_LEFT = 5;

int main(int argc, char* argv[]) {
  VideoCapture right(0);
  VideoCapture left(1);

  left.set(CAP_PROP_FPS, 20); // capture 20 frames per second
  right.set(CAP_PROP_FPS, 20); // capture 20 frames per second

//cout << "setting up camera" << endl;
  // verify both cams video capture is opened.
  if(!left.isOpened() || !right.isOpened()) {
    cout << "Unable to open a webcam. Exited followBlob." << endl;
    return -1;
  }
// cout << "camera is on" << endl;
  initializeBoard();

  brake = STOP;
  speed = 70;
  usmAngle = 5;
  value = 4;
  setRS485ServoPosition(USM,0,usmAngle);
  setRS485ServoPosition(CAMERASERVO,0,value);

  frontD = measureDistance(0);
 // cout << "FrontD dist: " << frontD << endl;
  mode = FORWARD;

  dist = INFINITY;
  //cout << "Blobs Distance = " << dist << " cm" << endl;

  while (dist > 60.0) {
//cout << "dist = " << dist << endl;
// START OF BLOB DETECTION //

    // variables for captured frames
    Mat leftCam, rightCam, hsvLeft, hsvRight, mask, mask2, leftVid, rightVid;
//cout << "setting up Mat-s" << endl;
    double scalingFactor = 0.75;

    // capture 10 frames and calculate average Rx, Ry, Lx, Ly
    for(int i = 0; i < 10; i++ ) {
      leftVid = Scalar(0, 0, 0);
      rightVid = Scalar(0, 0, 0);

      left >> leftCam;
      right >> rightCam;
//cout << "Mat-ing video feed" << endl;
      // flip left cam x and y axis so image same orientation as right cam
      flip(leftCam, leftCam, -1);
//cout << "flipped" << endl;
      // check if fram has been captured
      if(rightCam.empty()) {
        cout << "right cam is empty, exiting" << endl;
        break;
      } else if(leftCam.empty()) {
        cout << "left cam is empty, exiting" << endl;
      } else {

        // adjust size of captured frame
        resize(leftCam, leftCam, Size(), scalingFactor, scalingFactor, INTER_AREA);
        resize(rightCam, rightCam, Size(), scalingFactor, scalingFactor, INTER_AREA);
//cout << "resized" << endl;
        // convert image color from BGR to HSV
        cvtColor(leftCam, hsvLeft, COLOR_BGR2HSV); // converts to HSV color
        cvtColor(rightCam, hsvRight, COLOR_BGR2HSV); // converts to HSV color
//cout << "hsv-ed" << endl;
        // set low and high thresholds for color of blob to detect.
        Scalar low = Scalar(60, 100, 100);
        Scalar high = Scalar(180, 255, 255);

        // check converted image for blob within threshold
        inRange(hsvLeft, low, high, leftVid);
        inRange(hsvRight, low, high, rightVid);
//cout << "inRange" << endl;
        // method calls to remove unwanted noise from left image
        erode(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        // method calls to remove unwanted noise from right image
        erode(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
//cout << "erode and dilated" << endl;
	// return x, y coords for center of blob detected from left and right camera
        Moments leftMoments = moments(leftVid);
        Moments rightMoments = moments(rightVid);
//cout << "looking for moments" << endl;
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
//cout << "sums " << sumLX << ", " << sumRX << endl;
        } else {
          cout << "no blob found, exiting" << endl;
          break;
        }
    }
  }

    // find avg Lx, Ly, Rx, Ry
    avgLX = sumLX / 10;
    avgRX = sumRX / 10;
    avgLY = sumLY / 10;
    avgRY = sumRY / 10;
cout << "avgLX = " << avgLX << ", avgRX = " << avgRX << endl;
  // calc difference between Lx and Rx, Ly and Ry
    double xDiff = avgRX - avgLX;
    double yDiff = avgRY - avgLY;

  // reset sums to 0
    sumLX = 0.0f;
    sumLY = 0.0f;
    sumRX = 0.0f;
    sumRY = 0.0f;
//cout << "found blob" << endl;
// END OF BLOB DETECTION //
    double changeX = (double)(abs(xDiff));
    frontD = measureDistance(0);
//    cout << "frontD USR dist = " << frontD << endl;
    rightD = measureDistance(1);
    leftD = measureDistance(2);
    dist = calculateDist(changeX);
    cout << "Blobs Distance = " << dist << " cm" << endl;
    // Camera adjusts tilt every 36-40 y pixels averaged between right and left cameras
    double valueY = ((avgLY + avgRY) / 2);
    unsigned int camTilt;

    if (valueY >= 0.0 && valueY < 40.0) {
      camTilt = 8;
    } else if (valueY >= 40.0 && valueY < 80.0) {
      camTilt = 7;
    } else if (valueY >= 80.0 && valueY < 120.0) {
      camTilt = 6;
    } else if (valueY >= 120.0 && valueY < 160.0) {
      camTilt = 5;
    } else if (valueY >= 160.0 && valueY < 200.0) {
      camTilt = 4;
    } else if (valueY >= 200.0 && valueY < 240.0) {
      camTilt = 3;
    } else if (valueY >= 240.0 && valueY < 280.0) {
      camTilt = 2;
    } else if (valueY > 280.0) {
      camTilt = 1;
    }
    if(camTilt != value) {
    	 setRS485ServoPosition(CAMERASERVO,0,camTilt);
	     value = camTilt;
    }
    if ((mode > STOP) && (mode < SPIN_RIGHT) && (dist > 60.0)) {
      // uses rightD and leftD USR to detect objects nearby and avoids / switch to cameras?
     if ((leftD + rightD) > 200 && dist <= 60.0) {
       mode = FORWARD;
       if (leftD < 95 && dist <= 60.0) mode = VEER_RIGHT; //vere right
	     if (rightD < 95 && dist <= 60.0) mode = VEER_LEFT; //vere left
			} else { // between two walls.  Try to even the distance between
	       if (leftD > rightD && dist <= 60.0) {
         mode = VEER_RIGHT;
       } else {
         mode = VEER_LEFT;
       }
     }

	    double avgRightLeftX = (avgLX + avgRX) / 2.0;
      // follow ball forward and veer right/left based on avg x axis
//cout << "avgLX = " << avgLX << ", avgRX = " << avgRX << endl;
//cout << "mode = " << mode << endl;
    if ((avgRightLeftX < 132.0) && (avgRightLeftX > 220.0)) {
      mode = FORWARD;
      cout << "moving forward is avgRightLeftX between 131 & 220? " << avgRightLeftX << endl;
      cout << "mode = 0? " << mode << endl;
    } else if (avgRightLeftX <= 132.0 ) { // veer left
          mode = VEER_LEFT;
	        cout << "veering left is avgRightLeftX < 132? " << avgRightLeftX << endl;
		      cout << "mode = 3? " << mode << endl;
    } else if (avgRightLeftX >= 220.0 && mode != VEER_RIGHT) { // veer right
          mode = VEER_RIGHT;
	        cout << "veering right is avgLX > 220? " << avgRightLeftX << endl;
		      cout << "mode = 2? " << mode << endl;
    } else if ((mode > STOP) && (mode < SPIN_RIGHT) && (dist < 60) && (frontD >= 30))  {
			   if ((leftD + rightD) > 200) {
				       mode = FORWARD;// move forward
				       if (leftD < 95) mode = VEER_RIGHT; //vere right
				       if (rightD < 95) mode = VEER_LEFT; //vere left
			   } else { // between two walls.  Try to even the distance between
	          if (leftD > rightD) {
              mode = VEER_RIGHT;
            } else {
              mode = VEER_LEFT;
            }
			  }
	  } else if (frontD < 30) { // Stop bot
        mode = STOP;
    }
  }
    //cout << "bob found blob and is ready to move" << endl;
      moveBot(mode);
    //  cout << "bob is on his way!" << endl;
  }

  setLMD18200Status(MOTOR0, brake, dir0, STOP);
  setLMD18200Status(MOTOR1, brake, dir1, STOP);

  // print battery usage information
  readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
  printf("volts %2.2f +/- %1.2f\n",volts, stdev);
  fflush(stdout);
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
    case STOP: // STOP
      speed0=0;
      speed1=0;
      dir0 = 1;
      dir1 = 1;
    break;
    case FORWARD:
      speed0=speed;
      speed1=speed;
      dir0 = 1;
      dir1 = 1;
    break;
    case VEER_RIGHT: // vere right
      speed0=speed/2;
      speed1=speed;
      dir0 = 1;
      dir1 = 1;
    break;
    case VEER_LEFT: // vere left
      speed0=speed;
      speed1=speed/2;
      dir0 = 1;
      dir1 = 1;
    break;
    case SPIN_RIGHT: // spin right
      speed0=speed;
      speed1=speed;
      dir0 = 0;
      dir1 = 1;
    break;
    case SPIN_LEFT: // spin left
      speed0=speed;
      speed1=speed;
      dir0 = 1;
      dir1 = 0;
    break;
  }
//  printf("MOTOR0 dir0: %d\tspeed0: %d, \tMOTOR1 dir1: %d\tspeed1: %d \n", dir0, speed0, dir1, speed1);
//  fflush(stdout);

  setLMD18200Status(MOTOR0, brake, dir0, speed0);
  setLMD18200Status(MOTOR1, brake, dir1, speed1);
  return;
}

double calculateDist(double changeX) {
  // calcDist set to 0.0 means object is really close.
  double calcDist = 0.0;
  if (changeX >= 10 && changeX <= 100) {
  // physics calculation to determine objects distance from bot...
    calcDist = K1 * pow(changeX, K2);
  }
  return calcDist;
}

unsigned int measureDistance(unsigned char chan){
	int status;
	unsigned int valueUSM;
	status = getRS485USMdistance(USM+chan,&value);
	if (status) value = 0;
	return (valueUSM*34/500); // distance in cm
}
