/*

General header File to encapsulate all slave device functions

Provides routines to inteface to various devices connected to the RS485 bus.

 */


#define BASEREGANLG 0x0D0D
#define BASEREGSERVO 0x0A0A
#define BASEREGLMD18200 0x0A1A
#define BASEREG485BRIDGE232 0x0C0C
#define BASEREGSTEPMTR 0x0B0B
#define BASEREGFN 0x00F0

/* **************************************************************************************************

Universal ID function.  All slave devices will respond to this.

*/
#ifdef __cplusplus
extern "C" {
#endif

extern int getRS485SlaveID(unsigned char* returnData, unsigned  short Address);
extern int changeRS485Address(unsigned short Address, unsigned short newAddress);
/*
to do: add functions to interface to DC motor driver slave, and Ultrasonic Measureing unit.

*/

/* **************************************************************************************************
	LMD18200 motor controller 

*/

extern int setLMD18200Status(unsigned short address, unsigned char brake, unsigned char dir, unsigned char speed);
extern int getLMD18200Current(unsigned short address, unsigned int* current);

/* **************************************************************************************************

Stepper motor slave module:

*/
extern int findHomeRS485StepperMotor(unsigned short chan, unsigned char state, unsigned char direction);
//macro that has device find home on its own
extern int moveRS485StepperMotor(unsigned short chan, unsigned short steps, unsigned char dir);
//move by number of steps
extern int getRS485StepperMotorSteps(unsigned short chan, unsigned int* steps);
// use this to query motion.  The return value is steps yet to move. This counts down to zero when the move is completed
extern int getRS485StepperMotorStepsRev(unsigned short chan, unsigned int* stepsperrev);
extern int setRS485StepperMotorStepsRev(unsigned short chan, unsigned short stepsperrev);
// stepsperrev is only used in homing. If home not found in stepsperrev, motion ends
extern int getRS485StepperMotorSpeed(unsigned short chan, unsigned int* speed);
extern int setRS485StepperMotorSpeed(unsigned short chan, unsigned short speed);
// speed that pulses are sent out

/* **************************************************************************************************

Dual servo slave module:

*/
extern int setRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned short position);
extern int getRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned int* position);

/*
the Ultra-Sonic Measurment unit also have a servo attached. the position of the servo through above functions.

to retreive the distance measurment:
*/

extern int getRS485USMdistance(unsigned short chan, unsigned int* tof);
//returns the time of flight in units of of 4uS for echo to return.  to convert to distance in cm : d = 2*34/1000

/* **************************************************************************************************

Digital out module

*/

extern int setRS485DigitalOut(unsigned short chan, unsigned short value);
extern int getRS485DigitalOut(unsigned short chan, unsigned short * value);



/* **************************************************************************************************

Analog recorder slave module

*/
extern int setRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned short ADCperiods);
extern int getRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned int* ADCperiods);
// ADCperiods is in units of 16mS.  How often the PIC takes, and buffers, a reading
extern unsigned int readRS485AnalogRecorderSimple(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount);
extern unsigned int readRS485AnalogRecorder(unsigned short RS485Chan, unsigned short AnalogChan, float vref, float* volts, float* stdev);
/* vref scales the ADCcount. For an analog signal, undivided by resistors into the PIC, vref=5.0.  This will need to be modified 
 if there is an input voltage divider.  If the returned value needs to not be converted to an actual voltage then set vref=1024.0. 

IMPORTANT: VERSION 3 BOARDS (WITH THE DISPLAYS) ONLY SAMPLE 7 MEASURMENTS.  OLD VERSION 2 BOARDS SAMPLED 10 MEASUREMENTS.
THE CODE IN THIS LIBRARY HAS BEEN UPDATED TO THE NEW BOARDS.

	R1		R2
[Vinput] ───/\/\/\──────┬──────/\/\/\───┐
			│		│
			│	      [gnd]
			│
			└──────────────────────────────[PIC analog input]. [0 to 5 V full scale = 0 to 1024 ADCcount]
*/



/* **************************************************************************************************

RS485 to RS232 Bridge  module

*/
extern int writeRS485to232Bridge(unsigned char* cmd, unsigned char* returnData, unsigned  short Address);
// passes ascii cmd to 232 device. returnData is response from device.
extern int setRS485BridgeReads(unsigned short reads, unsigned short Address);
extern int getRS485BridgeReads(unsigned short* reads, unsigned short Address);
extern int setRS485BridgeTimeout(unsigned short timeout,unsigned short Address);
extern int getRS485BridgeTimeout(unsigned short* timout,unsigned short Address);

// common to both bridges
extern int setRS485BridgeDebugPrint(unsigned short debug, unsigned short Address);
// debug = 0 or 1 to turn off/on local debug printing to attached screen

// RS485 to GPIB bridge functions

extern int getRS485GPIBStatus(unsigned short* GPIBStatus, unsigned short Address);
/*
This is more of a debugging thing. indicates the current status of the control lines on the GPIB bus.
probaably not useful for much of anything else.
Address is the RS485 device address
*/

extern int resetGPIBBridge(unsigned short Address); // Reasserts REN, and sends InterFaceClear (IFC) to all devices
//Address is the RS485 device address
//int sendGPIBCommand(unsigned char* cmd, unsigned short Address);  // lets not make it public
//but it is used by 'closeGPIBBridge(); 
// this is used for MANUALLY controlling the GPIB bus.  probably not needed for most instances.


extern int closeGPIBBridge(unsigned short Address);
// basically issues a GoToLocal (GTL) command .


extern int sendGPIBData(unsigned char *cmd, char gpibaddress, unsigned short Address);

/* sendGPIBCData( command to send, RS485 address of bridge device)
//Address is the RS485 device address
The Bridge tells the device at gpibaddress to listen, then sends cmd as data (not a command). The difference
is whether or not ATN is asserted. Then, an unlisten command is issued afterwards.  All of these steps are 
handled on the Bridge.

The return is an error status. status==0 means no errors.

The character array cmd must be a NULL terminated string, since the functions in this library determine the number of bytes
to send based on the length of the string. 
*/


extern int listenGPIBData(unsigned char *returnData, char terminator, char gpibaddress, unsigned short Address);
/* listenGPIBData( buffer for return data, RS485 address of bridge device)
//Address is the RS485 device address
// gpibaddress is the GPIB address of the instrument connected to RS485bridge

*/

#ifdef __cplusplus
}
#endif



