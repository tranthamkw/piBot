

#include <string.h>
#include <math.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

simple roaming bot program

move forward unless object in front. 

 */

unsigned int measureDistance(unsigned char chan){
	int status;
	unsigned int value;
	status = getRS485USMdistance(USM+chan,&value);
	if (status) value = 0;
	return (value*34/500); // distance in cm
}


int main(int argc, char **argv) {
	unsigned int dir1,dir0,brake,speed1,speed0,usmAngle;
	unsigned int value,temp,k;
	unsigned int speed,dircode,frontD,leftD,rightD,mode;
	float volts,stdev;

	initializeBoard();

	brake=0;
	speed=82;
	usmAngle=5;
	setRS485ServoPosition(USM,0,usmAngle);
	frontD = measureDistance(0);
	mode = 4;
while (frontD > 30){

	frontD = measureDistance(0);
	rightD = measureDistance(1);
	leftD = measureDistance(2);
	printf("LEFT:%d cm \t\tFRONT:%d cm\t\tRIGHT:%d cm\n",leftD,frontD,rightD);
	fflush(stdout);

	if ((mode>3)&(frontD > 80)) { // moving forward and frontD>80
			if ((leftD+rightD)>80) {
				mode = 4;// move forward
				if (leftD<40) mode =5; //vere right
				if (rightD<40) mode = 6; //vere left
			}else{ // between two walls.  Try to even the distance between
				if (leftD>rightD){mode = 5;}else{mode = 6;}
			}
		} else {
		if (frontD>90){
			mode = 4;
			}else{
			if (mode>3)	if (leftD>rightD){mode = 2;} else {mode = 1;}
			}
	}

	if (frontD<30) mode =0;//force bot to stop and then exit program

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
		case 5: // vere right
			speed0=speed/2;
			speed1=speed;
			dir0 = 1;
			dir1 = 1;
		break;
		case 6: // vere left
			speed0=speed;
			speed1=speed/2;
			dir0 = 1;
			dir1 = 1;
		break;

	} // end switch

	setLMD18200Status(MOTOR0, brake, dir0, speed0);
	setLMD18200Status(MOTOR1, brake, dir1, speed1);

}
	readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
	printf("volts %2.2f +/- %1.2f\n",volts,stdev);

return 0;

}
/**
 End of File
*/
