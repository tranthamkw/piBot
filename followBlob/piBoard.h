/*

lowest layer for wiring Pi interface to RaspPi GPIO. See schematics.

Provides basic UART communication with RS485. RS485Devices uses this to send specifically formated data to various devices on the network

MODBUS RTU LAYER::
	uses Modbus 16 communcation protocal .  Modbus specifies how data is organized
	and can be transmitted over different types of electrical standards (e.g. rs485, rs232,...)
	Useful resource:
	http://www.lammertbies.nl/comm/info/modbus.html
	8 bits
	No parity
	1 stop bit(I think)
	9600 baud
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mcp3004.h>// ADC connected to MCP3004 chip. This circuit does not exist on the piBoard4. Do not Call getADC.
#include <wiringPi.h>
#include <wiringSerial.h>

#ifdef __cplusplus
extern "C" {
#endif
extern int initializeBoard();
//wiring pi setup , initialyze global variables
extern int closeBoard();

extern int getADC(unsigned short chan, unsigned int * returndata);  // not functioning on piBoard4
// uses MCP3008 ADC conected to SPI channel 0.
extern void delay_uS(int x);

/*
 RS485 I/O
*/
extern void write_rs485BYTE(unsigned char* cmd, unsigned int numchar, unsigned char* pszEcho, unsigned int* sizeEcho);
// uses RaspiUART and one GPIO pin to control serial IO through RS485
extern unsigned short modRTU_CRC(unsigned char* buff,unsigned short len);
// calculates Modbus16 CRC. Returns CRC word. This gets seperated into two bytes
extern int validateRTU(unsigned char* buff, unsigned short len);
//checks a byte buffer . Assumes that the last two bytes are Modbus16 CRC
int write_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int writedata);
//writes a message to machine at address, and to register reg.
// returns 0 if no errors
// returns -1 for general errors, such as CRC check error or time-out no response(100ms)
// returns machine specific error code if the machine responds
extern int read_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int* cnReturnData);
//read from a register for a machine at address. 
//same errors are returned.
//if an error is returned then cnReturnData is junk

#ifdef __cplusplus
}
#endif
