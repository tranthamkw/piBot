
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"


/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {


	unsigned short value;

	initializeBoard();

if (argc==2){

	value = atoi(argv[1]);  // 0 to 8

	setRS485ServoPosition(0xb1,0,value);
} else {



printf("usage\n");

}

return 0;

}
/**
 End of File
*/
