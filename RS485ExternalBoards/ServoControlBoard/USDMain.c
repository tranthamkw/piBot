#include <pic.h>
#include <string.h>
#include "ModBusRTU.h"


/*

COMBO UNIT.  

SERVO CONTROL
Microcontroller listens to RS485 commands and sets a servo to one of 8 positions. 
RC5 ==> servo signal

Set Servo to one of eight positions:
Write 
<RS485Address> 06 0A 0A 00 <pos> <CRCh> <CRCl>
with <pos> in the range 0 to 8

ULTRASONIC MEASUREMENT
Get USM distance
RA5 output ===> USM module trigger
RA4 input - T1Gate <=== USM echo
Write 
<RS485Address> 03 0A 0E <CRCh> <CRCl>
Responds with echo time, or shows error in byte[1]

*/

__CONFIG(INTIO & WDTDIS & PWRTEN & MCLRDIS & UNPROTECT & BORDIS &
IESODIS & FCMDIS);

#define BASEREG 0x0A0A
#define FNREG 0x00F0
#define MAX_COUNT 200
#define MYFUNCTION "USM"
unsigned char address=0xA0;
unsigned char startUSM=0;
unsigned char time_count=0;  
unsigned char servo1_count=10;  //counts number of 0.1mS.  Minimum is 10. Maximum is 20


static void interrupt isr(void)
{
  if(T0IF) {	            // TIMER0 Interrupt Flag
	time_count++; // counts the number of times we go through this interupt. This interrupt is called every 0.1 mS
       
    if (time_count == MAX_COUNT) { //reset everything and turn on 
		RC5=1;
		time_count=0;
        }
    if (time_count == servo1_count) {
		RC5=0;
      }

    TMR0 = 180;             // Initial Value for 0.1ms Interrupt
    T0IF = 0;		    // Clear TIMER0 interrupt flag
  }
}

unsigned short measureDistance(void)
{
unsigned short k;

	// start a pulse for the USM
		RA5=1;
		for (k=0;k<2;k++);
		RA5=0;
	TMR1L=0;// reset timer
	TMR1H=0;
	T1CON=0b11110101; // gated on RA4 and start
/*
	bit 7: 1 timer gate 1 gate is inverted
	bit 6: Timer1 gate Enabled
	bit 5-4: inoput clock prescale 
			11 = 1:8 prescale
	bit 3: LP oscillator enable
	bit 2: ignored because bit 1
	bit 1: Timer1 clock source 0=Fosc/4
	bit 0: Timer 1 on enabled
Fosc = 8Mhz so Timer1 frequency = 8/(4*8) = 250MHz ==> period per count = 0.004mS
TOF_mS = timer1 * 0.004mS
TOF_uS = timer1 * 4 uS
distance = TOF/2 * c
c=340m/s=3400cm/s=0.034cm/uS
distance (cm) = TOF_uS /2 * 0.034cm/uS
distance (cm) = timer1 * 4uS / 2 * 0.034sm/us


*/
	//wait until RA4 goes hi. Since gated, this time does not count in the counter
	do {
	} while (!RA4);
	// now time how long it takes to go low.
	do {
	} while (RA4);
	T1CON=0b01000000;// stop timer. probably not needed since gated
	k = (TMR1L | (TMR1H<<8));
	return k;
}

void main (void) {	
 	int i,k;
	int reg;
	int strlength;
	char error;
	char data[24];
	char fnString[16];
	unsigned short outdata;
	
	PORTA =0;    //clears port A
	TRISA=0;
	TRISC = 0;    
	PORTC = 0;    //clears port C
	CM1CON0 = 7;    //turn off comparators
	CM2CON0 = 7;
//	CM2CON1 = 7;
	CM2CON1 = 0b00000010;
	ANSEL = 0;    //turns off ADC
	OSCCON=0x70;         // Select 8 MHz internal clock

	TRISA4=1;  // A4 digital in is ussd to ga
	TRISA5=0; // A4 is digital out an is used to initiate a Ultrasonic measurement.
	address=EEPROM_READ(0x00);
	RS485_Init();

  
 OPTION = 0b00000000; // 1:2 Prescaller
  TMR0=180;            // Interupt every 0.1 ms
  T0IE = 1;		       // Enable interrupt on TMR0 overflow
  GIE = 1;		       // Global interrupt enable
	strlength=0;
	error=0;
	RA5=0;

  do  {

	if (RS485_Data_Ready()==1){
			strlength = RS485_Read_Data(data,24);
			CREN=0;	
			if (validateRTU(data,strlength) ==0){
					// RTU CRC valid
					if (!RA3) {	// if button pressed, remember this address as our own.
						address = data[0];
						EEPROM_WRITE(0x00,address);
					}

					if (data[0]==address){ //address byte
							// message intended for us
						switch (data[1]){  // command code byte
				
							case 0x06: // write a register
								reg = (data[2]<<8) | (data[3]);
									switch (reg){
									case BASEREG:
										outdata =(data[4]<<8)| data[5];  // assign new data to varible ("register")
										if (outdata<9){
											servo1_count=outdata + 10;
											// formulate a response echo back command
											strlength=6;
											RS485_Write_Data(data,strlength);
											error = 0;
												}else {
												error = 0x06; // out of range
												}
										break;
								
									default:
										// invalid register
										error = 0x02;
										break;
										}
								break; // write to a register
							case 0x03:  // read a register
								reg = (data[2]<<8) | (data[3]);
								switch (reg) {
										case BASEREG:
											outdata = (unsigned short) servo1_count;
											outdata-=10;
										// formulate a response. 
											data[2]=2;  //number of bytes to follow of data
											data[3]=(outdata&0xFF00)>>8; //MSB first
											data[4]=(outdata&0x00FF);
											strlength=5;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
										case BASEREG+4:
											outdata = measureDistance();
											// formulate a response. 
											data[2]=2;  //number of bytes to follow of data
											data[3]=(outdata&0xFF00)>>8; //MSB first
											data[4]=(outdata&0x00FF);
											strlength=5;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
										case FNREG:
											strcpy(fnString,MYFUNCTION);
											strlength = strlen(fnString);
										//data[0]=<RS485Address>; //data[1]=03;
											data[2] = (unsigned char)strlength;// number of bytes to follow
											for (i=0;i<strlength;i++){
												data[3+i]=fnString[i];
											}
											strlength+=3;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
										default:
										error = 0x02; // invalid register
										break;
								}		
							break;// read a register
						
							default:
								// unknown command byte
								error = 0x08;
							break;
						}//end switch
						if (error) {
								data[1]=data[1] | 0x80;
								data[2]= (error & 0xFF00)>>8;
								data[3]=(error & 0x00FF);
								strlength=4;
								RS485_Write_Data(data,strlength);
						}
					}/// message intended for another address. ignore message.
			}else {
					//invalid RTU  just ignore the message
			}

			CREN=1; // this sequence resets the receive register. The act of writing tends to load a single bit 
			CREN=0;
			CREN=1;
		}// end data ready
 
}while(1);
}