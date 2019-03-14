#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

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

  
  for(int i = 0; i < 10; i++ ) {
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


      // TODO Old code that used HoughCircles to track round objects.
      //
      // inRange(hsvLeft, low, high, leftVid);
      // inRange(hsvRight, low, high, rightVid);
      // bitwise_and(leftCam, leftCam, leftVid, mask2=mask2);
      // bitwise_and(rightCam, rightCam, rightVid, mask=mask);
      // medianBlur(leftVid, leftVid, 5);
      // medianBlur(rightVid, rightVid, 5);
      //
      // Mat gray, gray2;
      // cvtColor(leftVid, gray, COLOR_BGR2GRAY);
      // medianBlur(gray, gray, 5);
      // cvtColor(rightVid, gray2, COLOR_BGR2GRAY);
      // medianBlur(gray2, gray2, 5);
      //
      // vector<Vec3f> circles;
      // HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
      //              gray.rows/16,  // change this value to detect circles with different distances to each other
      //              100, 30, 10, 20 // change the last two parameters
      //         // (min_radius & max_radius) to detect larger circles
      // );
      //
      // for( size_t i = 0; i < circles.size(); i++ )
      // {
      //     Vec3i c = circles[i];
      //     int x = c[0];
      //     int y = c[1];
      //     // printf("Left Cam: X Cord = %d, Y Cord = %d\n", x, y);
      //     Point center = Point(c[0], c[1]);
      //     // circle center
      //     circle( leftCam, center, 1, Scalar(0,100,100), 1, LINE_AA);
      //     // circle outline
      //     int radius = c[2];
      //     circle( leftCam, center, radius, Scalar(255,0,255), 1, LINE_AA);
      //
      //     cout << "Left x-cord = " << x << " y-cord = " << y << endl;
      //
      // }
      //
      // vector<Vec3f> circles2;
      // HoughCircles(gray2, circles2, HOUGH_GRADIENT, 1,
      //              gray2.rows/16,  // change this value to detect circles with different distances to each other
      //              100, 30, 10, 20 // change the last two parameters
      //         // (min_radius & max_radius) to detect larger circles
      // );
      //
      // for( size_t i = 0; i < circles2.size(); i++ )
      // {
      //     Vec3i c2 = circles2[i];
      //     int x2 = c2[0];
      //     int y2 = c2[1];
      //     // printf("Right Cam: X Cord = %d, Y Cord = %d\n", x, y);
      //     Point center = Point(c2[0], c2[1]);
      //     // circle center
      //     circle( rightCam, center, 1, Scalar(0, 100, 100), 1, LINE_AA );
      //     // circle outline
      //     int radius = c2[2];
      //     circle( rightCam, center, radius, Scalar(255,0,255), 1, LINE_AA);
      //
      //     cout << "Right x-cord = " << x2 << " y-cord = " << y2 << endl;
      // }


      // TODO video capture testing code
      // imshow("Left Cam", leftCam);
      // imshow("Left Object", leftVid);
      //
      // imshow("Right Cam", rightCam);
      // imshow("Right Object", rightVid);

      // commented out due to for loop changed to while loop
      //if((char)waitKey(5) == 'q') {
      //  break;
      //}
  }

	int avgLX = sumLX / 10;
	int avgRX = sumRX / 10;
	int avgLY = sumLY / 10;
	int avgRY = sumRY / 10;

	cout << "Left X Average = " << avgLX << endl;
	cout << "Left Y Average = " << avgLY << endl;
	cout << "Right X Average = " << avgRX << endl;
	cout << "Right Y Average = " << avgRY << endl;

  return 0;
}
