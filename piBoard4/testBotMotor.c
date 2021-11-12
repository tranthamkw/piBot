

#include <string.h>
#include <math.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {
	unsigned int dir1,dir0,brake,speed1,speed0;
	unsigned int value,temp,k;
	unsigned int speed,dircode;
	float volts,stdev;

	initializeBoard();

brake=0;

if (argc==5){
	dir0 = atoi(argv[1]);  // 0 or 1
	speed0 = atoi(argv[2]); // 0 or 1
	dir1 = atoi(argv[3]); // 0 to 
	speed1 = atoi(argv[4]);
	setLMD18200Status(MOTOR0, brake, dir0, speed0);
	setLMD18200Status(MOTOR1, brake, dir1, speed1);
} else {

if (argc==3){
	speed = atoi(argv[1]);
	if (speed>255) speed=255;
	dircode=atoi(argv[2]);
   printf("speed: %d\ndirection: %d\n",speed,dircode);
	fflush(stdout);
	switch (dircode){
		case 8: // move straight ahead
			speed0=speed;
			speed1=speed;
			dir0 = 1;
			dir1 = 1;
		break;
		case 2: // move straight back
			speed0=speed;
			speed1=speed;
			dir0 = 0;
			dir1 = 0;
		break;
		case 9: // vere right
			speed0=speed/2;
			speed1=speed;
			dir0 = 1;
			dir1 = 1;
		break;
		case 7: // vere left
			speed0=speed;
			speed1=speed/2;
			dir0 = 1;
			dir1 = 1;
		break;
		case 4: // spin left
			speed0=speed;
			speed1=speed;
			dir0 = 1;
			dir1 = 0;
		break;
		case 6: // spin right
			speed0=speed;
			speed1=speed;
			dir0 = 0;
			dir1 = 1;
		break;
		case 5: // STOP
			speed0=0;
			speed1=0;
			dir0 = 1;
			dir1 = 1;
		break;
		case 1: // vere back left
			speed0=speed;
			speed1=speed/2;
			dir0 = 0;
			dir1 = 0;
		break;
		case 3: // vere back right
			speed0=speed/2;
			speed1=speed;
			dir0 = 0;
			dir1 = 0;
		break;

	default:
		speed0=0;
		speed1=0;
		break;
	} // end switch
	setLMD18200Status(MOTOR0, brake, dir0, speed0);
	setLMD18200Status(MOTOR1, brake, dir1, speed1);

} else {
printf("Usage: ./testBotMotor <m1dir> <m1speed> <m2dir> <m2speed>\n");
printf("-OR-\nUsage: ./testBotMotor <speed> <dircode>\n");
}
}

return 0;

}
/**
 End of File
*/
