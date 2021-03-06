/*

this libary is similar in nature (almost identical)
to the wiringPi SPI library.

The minor change is the  data read/write function, a second variable was added.

 */

#include <stdio.h>

int myPiSPIGetFd     (int channel) ;
int myPiSPIDataRW    (int channel, unsigned char *data, unsigned char *rx, int len) ;
int myPiSPISetupMode (int channel, int speed, int mode) ;
int myPiSPISetup     (int channel, int speed) ;

