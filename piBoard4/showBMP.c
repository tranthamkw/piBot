

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
unsigned short length,x0,y0,chan;

	initializeBoard();

//load a bmp
if (argc==3){

	strcpy(data, argv[1]);
	initDisplay(0);
	if(loadBMP(data)) updateDisplay(0);
	strcpy(data, argv[2]);
	initDisplay(1);
	if(loadBMP(data)) updateDisplay(1);

}else{
printf("usage sudo ./showBMP <bmpfile0> <bmpfile1>\n");
}


return 0;

}
/**
 End of File
*/
