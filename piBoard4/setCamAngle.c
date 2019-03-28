#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {


	unsigned short value;
	float volts,stdev;

	initializeBoard();

if (argc==2){

	value = atoi(argv[1]);  // 0 to 8
	setRS485ServoPosition(CAMERASERVO,0,value);


	readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
	printf("volts %2.2f +/- %1.2f\n",volts,stdev);
//	readRS485AnalogRecorderSimple(BATTERY_MONITOR,3,&volts);
//	volts = volts * MAXVOLTS/1024.0;
//	printf("volts %2.2f +/- %1.2f\n",volts,stdev);




} else {



printf("usage:  sudo ./setCamAngle <0..8> \n");

}

return 0;

}
/**
 End of File
*/
