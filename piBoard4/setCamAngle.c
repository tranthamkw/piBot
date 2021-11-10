#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {


	unsigned short value;
	float volts,stdev,resolution;

	initializeBoard();

if (argc==2){

	value = atoi(argv[1]);  // 0 to 8
	setRS485ServoPosition(CAMERASERVO,0,value);

	delay(100);

} else {



printf("usage:  sudo ./setCamAngle <0..8> \n");

}

return 0;

}
/**
 End of File
*/
