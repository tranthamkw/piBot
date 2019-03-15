

#include <string.h>
#include <math.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

simple roaming bot program

move forward unless object in front. 

 */

unsigned int measureDistance(void){
	int status;
	unsigned int value;
	status = getRS485USMdistance(USM,&value);
	if (status) value = 0;
	return (value*34/500); // distance in cm
}


int main(int argc, char **argv) {
	unsigned int dir1,dir0,brake,speed1,speed0,usmAngle;
	unsigned int value,temp,k;
	unsigned int speed,dircode,distance,mode;
	float volts,stdev;

	initializeBoard();

	brake=0;
	speed=80;
	usmAngle=3;
	setRS485ServoPosition(USM,0,usmAngle);
	distance = measureDistance();
	mode = 0;
while (distance > 30){

	distance = measureDistance();
	printf("distance %d cm\t",distance);
	fflush(stdout);

	if (((mode==0)|(mode==4))&(distance > 80)) {
		mode = 4;
		} else {
		if (distance>120){
			mode = 4;
			}else{
			mode = 2;
			}
		if (distance<30) mode =0;
	}

	switch (mode){
		case 1: // spin right
			speed0=speed;
			speed1=speed;
			dir0 = 0;
			dir1 = 1;
		break;
		case 2: // spin left
			speed0=speed;
			speed1=speed;
			dir0 = 1;
			dir1 = 0;
		break;
		case 0: // STOP
			speed0=0;
			speed1=0;
			dir0 = 1;
			dir1 = 1;
		break;
		case 4:
			speed0=speed;
			speed1=speed;
			dir0 = 1;
			dir1 = 1;
		break;

	} // end switch

	setLMD18200Status(MOTOR0, brake, dir0, speed0);
	setLMD18200Status(MOTOR1, brake, dir1, speed1);
	readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
	printf("volts %2.2f +/- %1.2f\n",volts,stdev);

}

return 0;

}
/**
 End of File
*/
