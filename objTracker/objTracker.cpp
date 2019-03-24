#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

// function declaration
int menu();

// global variable for data collection
int dist = 0;

// TODO global variables
int leftX, rightX, leftY, rightY;
int sumLY = 0;
int sumLX = 0;
int sumRY = 0;
int sumRX = 0;

int main(int argc, char* argv[]) {

  VideoCapture left(0);   // dev0 camera - might need switched
  VideoCapture right(1);  // dev1 camera - might need switched

  left.set(CAP_PROP_FPS, 1);
  right.set(CAP_PROP_FPS, 1);

  if(!left.isOpened() || !right.isOpened()) {
    cout << "Unable to open a webcam. Exited objTracker." << endl;
    return -1;
  }

  Mat leftCam, rightCam, hsvLeft, hsvRight, mask, mask2, leftVid, rightVid;

  float scalingFactor = 0.75;

  // begin of objTracker.cpp data collection code
  string file = "data.csv";
  ofstream myfile;
  myfile.open(file, ios::out | ios::app);
  dist = menu();
  while (dist > 0) { //code for data collection above
    for(int i = 0; i < 20; i++ ) {
      leftVid = Scalar(0, 0, 0);
      rightVid = Scalar(0, 0, 0);

      left >> leftCam;
      right >> rightCam;

      flip(rightCam, rightCam, -1);


      if(leftCam.empty() || rightCam.empty())
        break;

        resize(leftCam, leftCam, Size(), scalingFactor, scalingFactor, INTER_AREA);
        resize(rightCam, rightCam, Size(), scalingFactor, scalingFactor, INTER_AREA);

        cvtColor(leftCam, hsvLeft, COLOR_BGR2HSV); // converts to HSV color
        cvtColor(rightCam, hsvRight, COLOR_BGR2HSV); // converts to HSV color

        Scalar low = Scalar(60, 100, 100);
        Scalar high = Scalar(180, 255, 255);

        inRange(hsvLeft, low, high, leftVid);
        inRange(hsvRight, low, high, rightVid);

        erode(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(leftVid, leftVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        erode(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(rightVid, rightVid, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        // TODO blob detection that returns x, y coords for left and right camera
        Moments leftMoments = moments(leftVid);
        Moments rightMoments = moments(rightVid);

        double leftM01 = leftMoments.m01;
        double leftM10 = leftMoments.m10;
        double leftArea = leftMoments.m00;

        double rightM01 = rightMoments.m01;
        double rightM10 = rightMoments.m10;
        double rightArea = rightMoments.m00;

        if (leftArea > 10000 && rightArea > 10000) {
          leftX = leftM10 / leftArea;
          leftY = leftM01 / leftArea;
          rightX = rightM10 / rightArea;
          rightY = rightM01 / rightArea;

  	  sumLX += leftX;
 	  sumLY += leftY;
  	  sumRX += rightX;
   	  sumRY += rightY;

        //  cout << "Left X = " << leftX << ", Left Y = " << leftY << endl;
        //  cout << "Right X = " << rightX << ", Right Y = " << rightY << endl;
        }
    }

  	int avgLX = sumLX / 20;
  	int avgRX = sumRX / 20;
  	int avgLY = sumLY / 20;
  	int avgRY = sumRY / 20;

// used for data collection
    int xDiff = avgRX - avgLX;
    int yDiff = avgRY - avgLY;

    myfile << dist << "," << avgLX << "," << avgLY << "," << avgRX << "," << avgRY << "," << xDiff << "," << yDiff << "\n";
    sumLX = 0;
    sumLY = 0;
    sumRX = 0;
    sumRY - 0;
    
    dist = menu();
  }
  myfile.close();

  // end of data collection code

	// cout << "Left X Average = " << avgLX << endl;
	// cout << "Left Y Average = " << avgLY << endl;
	// cout << "Right X Average = " << avgRX << endl;
	// cout << "Right Y Average = " << avgRY << endl;

  return 0;
}

int menu() {
  printf("Enter the ball's distance from robot in cm (enter 0 to quit):\n");
  scanf("%d", &dist);

  // TODO: code for main menu to be added later
  // int select = 0;
  // char pressed = '';
  // printf("Good day Dr. Trantham, what would you like to do today?\n");
  // printf("Press A to collect data.\n");
  // printf("Press Q to quit.");
  // scanf("%c\n", &pressed);
  // if(pressed == 'a' || pressed == 'A') {
  //   select = 1;
  // }
  return dist;
}
