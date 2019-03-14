#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {

	unsigned int value,position,numpings;
	int status, i;
	unsigned int sumvalue;
	unsigned int distance;

	distance=0;
	sumvalue=0;

	if (argc == 3) {
			position = atoi(argv[1]);
			numpings = atoi(argv[2]);
			printf("Measuring %d times...\n",numpings);

			if (numpings<1) numpings=1;
			if (position>8) position=8;
			initializeBoard();

			setRS485ServoPosition(USM,0,position);
			delay(50);
			for (i=0;i<numpings;i++){
				status = getRS485USMdistance(USM,&value);
				if (status) value=0;
				sumvalue+=value;
				delay(2);
			}

		distance=sumvalue*34/(500*numpings);
	printf("angle\t%d\t\tdistance\t%d cm\n",position,distance);

	} else {
		printf("usage: sudo ./getUSMDistance <angle 0..8> <numpings 1 ... >\n");
	}

	printf("\n");

return 0;

}
/**
 End of File
*/
