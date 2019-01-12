/*

 */

#include <stdio.h>

int myPiSPIGetFd     (int channel) ;
int myPiSPIDataRW    (int channel, unsigned char *data, unsigned char *rx, int len) ;
int myPiSPISetupMode (int channel, int speed, int mode) ;
int myPiSPISetup     (int channel, int speed) ;

