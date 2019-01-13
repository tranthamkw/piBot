

#include <string.h>
#include <math.h>
#include "piBoard.h"
#include "RS485Devices.h"


/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {
	unsigned char dir,brake,speed;

	initializeBoard();

if (argc==4){

	brake = atoi(argv[1]);  // 0 or 1
	dir = atoi(argv[2]); // 0 or 1
	speed = atoi(argv[3]); // 0 to 255
	setLMD18200Status(0xC1, brake, dir, speed);


} else {


	for (speed =0; speed<255;speed++){
	setLMD18200Status(0xC1, 0, 0, speed);
	delay(100);
	}
	setLMD18200Status(0xC1, 0, 0, 0);

}

return 0;

}
/**
 End of File
*/
