#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {


	unsigned int value;
	int status, i;
//	unsigned int distance[8];

	initializeBoard();

	printf("angle\tTOF\n");
	for (i=4;i>0;i--){
		setRS485ServoPosition(USM,0,i);
		status = getRS485USMdistance(USM,&value);
		if (status) value=0;
		printf("%d\t%d\t%d\n",i,value,value*34/500);
		delay(40);
}
	for (i=0;i<9;i++){
		setRS485ServoPosition(USM,0,i);
		status = getRS485USMdistance(USM,&value);
		if (status) value=0;
		printf("%d\t%d\t%d\n",i,value,value*34/500);
		delay(40);
	}
	for (i=7;i>4;i--){
		setRS485ServoPosition(USM,0,i);
		status = getRS485USMdistance(USM,&value);
		if (status) value=0;
		printf("%d\t%d\t%d\n",i,value,value*34/500);
		delay(40);
}

printf("\n");

//return to position 4
//		setRS485ServoPosition(USM,0,4);

return 0;

}
/**
 End of File
*/
