#include <string.h>
#include <time.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "botDefines.h"
#include <stdlib.h>
/*

quicky program to test new code ..


 */
int main(int argc, char **argv) {

	int i;
	float volts,stdev,count;
	FILE *fp;
	time_t rawtime;

	initializeBoard();

	fp = fopen("/home/pi/batteryMonitor.csv","a");
	if (!fp){
		printf("unable to open file \n");
		return 1;
	}
volts = 20.5;
while (volts>19.5){
	time(&rawtime);

	readRS485AnalogRecorder(BATTERY_MONITOR,3,MAXVOLTS,&volts,&stdev);
	fprintf(fp,"Time, %d, BatteryVolts, %2.3f ,+/-, %1.3f\n",rawtime,volts,stdev);
	printf("Time, %d, BatteryVolts, %2.3f ,+/-, %1.3f\n",rawtime,volts,stdev);
	delay(2000);

}


	fclose(fp);


system("sudo shutdown -h now");

return 0;
}
/**
 End of File
*/
