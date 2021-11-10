#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "botDefines.h"

/*

USM is the forward detector
USM + 1 is port
USM + 2 is starboard

 */
int main(int argc, char **argv) {

	unsigned int value,numpings;
	int status, i,k;

	unsigned int distance[10];
	numpings = 3;

if (argc == 2){
	numpings = atoi(argv[1]);
	printf("numpings %d\n\n",numpings);

	for (i=0;i<10;i++){
	distance[i]=0;
	}

/*	distance 0 = port
	distance 1..9 = front
	distance 10 = starboard */

			initializeBoard();
			setRS485ServoPosition(USM,0,8);
			delay(100);

//port		
			for (k=0;k<numpings;k++){
			status = getRS485USMdistance(USM+1,&value);
			distance[0]+=((value*34)/500);
			}
			distance[0]=distance[0]/numpings;
			printf("%d\t",distance[0]);
			fflush(stdout);
//front
			for (i=0;i<9;i++){
			setRS485ServoPosition(USM,0,8-i);
			delay(100);
			for (k=0;k<numpings;k++){
			status = getRS485USMdistance(USM,&value);
			distance[i]+=((value*34)/500);
			}
			distance[i]=distance[i]/numpings;
			printf("%d  ",distance[i]);
			fflush(stdout);

			}
//starbord
			for (k=0;k<numpings;k++){
			status = getRS485USMdistance(USM+2,&value);
			distance[9]+=((value*34)/500);
			}
			distance[9]=distance[9]/numpings;
			printf("\t%d\n\n",distance[9]);
			fflush(stdout);
} else {

printf("Usage ~$> sudo ./USMradar <num pings to average>\n");
}

return 0;

}
/**
 End of File
*/
