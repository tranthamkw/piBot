#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "botDefines.h"

/*



 */
int main(int argc, char **argv) {

	unsigned int value,position,numpings;
	int status, i,k;
	unsigned int sumvalue[3];
	unsigned int distance;

	distance=0;

	if (argc == 3) {
			position = atoi(argv[1]);
			numpings = atoi(argv[2]);
			printf("Measuring %d times...\n",numpings);

			if (numpings<1) numpings=1;
			if (position>8) position=8;
			initializeBoard();

			setRS485ServoPosition(USM,0,position);

			delay(50);
		for (k=0;k<3;k++){
			sumvalue[k]=0;
			for (i=0;i<numpings;i++){
				status = getRS485USMdistance(USM+k,&value);
				if (status) value=0;
				sumvalue[k]+=value;
				delay(2);
			}
		}

		distance=sumvalue[0]*34/(500*numpings);
	printf(" FORWARD USM: \t%d cm\n",distance);
		distance=sumvalue[2]*34/(500*numpings);
	printf("    PORT USM: \t%d cm\n",distance);
		distance=sumvalue[1]*34/(500*numpings);
	printf("STARBORD USM: \t%d cm\n",distance);


	} else {
		printf("usage: sudo ./getUSMDistance <front USM angle 0..8> <number of pings 1 ... >\n");
	}

	printf("\n");

return 0;

}
/**
 End of File
*/
