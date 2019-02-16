#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"
#define MAXVOLTS 29.2


/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {


	unsigned short value;
	float volts,stdev;

	initializeBoard();

if (argc==2){

	value = atoi(argv[1]);  // 0 to 8
	setRS485ServoPosition(0xb1,0,value);

	readRS485AnalogRecorder(0xc1,3,MAXVOLTS,&volts,&stdev);
	printf("volts %2.2f +/- %1.2f\n",volts,stdev);

} else {



printf("usage\n");

}

return 0;

}
/**
 End of File
*/
