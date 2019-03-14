#include <pic.h>

/* header file for analog reads. 

Corresponding digital Ports must be set to inputs.
Simple A/D conversion using Vdd as the reference voltage.
the function getADC(int chan) returns an 8 bit digital number PROPORTIONAL
to the voltage at analog input of the selected channel.
so
0 corresponds to 0 volts.
255 corresponds to Vdd.

*/


unsigned short getADC(short chan);
void initADC(void);