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
	unsigned int distance[9];

for (i=0;i<9;i++){
	distance[i]=0;
}
printf("Measuring ...\n");

	initializeBoard();

	for (i=4;i>=0;i--){
		setRS485ServoPosition(USM,0,i);
		status = getRS485USMdistance(USM,&value);
		if (status) value=0;
		distance[i]+=value*34/500;
		delay(20);
}
	for (i=0;i<9;i++){
		setRS485ServoPosition(USM,0,i);
		status = getRS485USMdistance(USM,&value);
		if (status) value=0;
		distance[i]+=value*34/500;
		delay(20);
	}
	for (i=8;i>4;i--){
		setRS485ServoPosition(USM,0,i);
		status = getRS485USMdistance(USM,&value);
		if (status) value=0;
		distance[i]+=value*34/500;
		delay(20);
}

printf("angle\td(cm)\n");

for (i=0;i<9;i++){
	printf("%d\t%d\n",i,(distance[i]/2));
}
printf("\n");

//return to position 4
//		setRS485ServoPosition(USM,0,4);

return 0;

}
/**
 End of File
*/
