#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"

#include "botDefines.h"

/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {


	unsigned short value,i,l,h;
	float volts,stdev,count,numDivs;
	float hiVolts, loVolts,hiCount,loCount;
	initializeBoard();

	numDivs=30.0;
	hiVolts = 21.0;
	loVolts = 18.0;

	hiCount = (hiVolts*1024.0/MAXVOLTS);
	loCount = (loVolts*1024.0/MAXVOLTS);

	readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
	printf("BatteryVolts = %2.3f +/- %1.3f\n",volts,stdev);
	printf("\n%2.1f V [",loVolts);
	count = (volts*1024.0/MAXVOLTS);
	l = (unsigned int)numDivs*(count - loCount)/(hiCount-loCount);
	h = (unsigned int)numDivs;
	for (i=0;i<h;i++){
		if (i<l) printf("#");
		if (i==l) printf("#");
		if (i>l) printf("-");
		}
	printf("] %2.1f V\n",hiVolts);


	delay(10);


return 0;

}
/**
 End of File
*/
