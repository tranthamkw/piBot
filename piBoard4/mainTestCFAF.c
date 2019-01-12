

#include "simpleGFX.h"
#include <string.h>
#include <math.h>
#include "piBoard.h"
#include "RS485Devices.h"


/*
                         Main application to test dual CFAF 128x128 display on RasPi

	usage: sudo ./mainTestCFAF <name of bmp to load>

	bmp must be 128x128x24colors

	program draws simple graphics, interfaces to a RS485 instrument and writes text response, and shows bmp.

 */
int main(int argc, char **argv) {
short i,j;
unsigned short color,R,G,B;
float red, green, blue;
char data[32];
unsigned short length,x0,y0;
float volts=0.0;
float pi=3.1415926;

	initializeBoard();
	initDisplay(0);
	initDisplay(1);

//load a bmp
if (argc==2){
	strcpy(data, argv[1]);
	if(loadBMP(data)) updateDisplay(1);
}

// draw some shapes
	clearCanvas(BLACK);
	drawBox(30,30,100,100,GREEN);
	drawCircle(25,25,20, WHITE);
	for (i=10;i<100;i+=5){
		drawLine(10,10,i,100,RED);
	}
	updateDisplay(0);

//print some quick text from a device
	for (i=0;i<50;i++){
		readRS485AnalogRecorderSimple(0xA2,0,&volts);  // instrument address 0xA2 must be connected to the RS485 port
		sprintf(data,"%d: %.2f",i,volts);
		length = strlen(data);
		printLine(45,data,length,WHITE,BLACK,0);
	}

	// make a rainbow
	color=0;
	clearCanvas(BLACK);
	for (j=0;j<128;j++){

		red = 7.9*sin((float)(j)*2.0*pi/128.0)+8.0;
		green = 7.9*sin((float)(j+42)*2.0*pi/128.0)+8.0;
		blue = 7.9*sin((float)(j+84)*2.0*pi/128.0)+8.0;
		R = (unsigned short) red;
		G = (unsigned short) green;
		B = (unsigned short) blue;
		R&=0xF;
		G&=0xF;
		B&=0xF;
		color=(R<<8)|(G<<4)|(B);

		for(i=0;i<128;i++){
		setPixel(i,j,color);
		}
	}
	updateDisplay(0);

return 0;

}
/**
 End of File
*/
