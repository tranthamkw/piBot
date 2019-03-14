#include <pic.h>

#include "ModBusRTU.h"
#include "readAnalog.h"
#include "SSD1306.h"

#include <string.h>

#define BASEREG 0x0D0D
#define FNREG 0x00F0
#define MYFUNCTION "AnalogRecord"
#define NUM_ADCVALUES 7


/*


*/

__CONFIG(INTIO & WDTDIS & PWRTEN & MCLRDIS & UNPROTECT & BORDIS &
IESODIS & FCMDIS);

char temp[16],outText[24];
unsigned short strlength;
unsigned char address=0x20;
unsigned char interruptTime=125; // this sets 0.128mS * 125 = 16mS per loop
unsigned char waitLoop=0;
unsigned char maxWaitLoop = 10;  // number of loops to skip before doing ADC.
//  i.e. take ADC reading every 10 * 16mS = 160mS.  This variable is configurable through the RS485 bus.  Allowed range 1 to 100
unsigned char loopCounter=0;
// NUM_ADCVALUES * 4 = 
unsigned short ADCvalues[28];		// NUM_ADCVALUES values over four channels   10 bits from ADC. unsigned char is only 8 bits. must use 'int'
unsigned short ADCtotal,ADCstdev;

void calculateADCstats(unsigned char channel){
	// calculate average and stdev. put into the global variables. 
	unsigned char k;
	unsigned int temp;
	ADCtotal = 0;
	for (k=0;k<NUM_ADCVALUES;k++){
		ADCtotal = ADCtotal + (unsigned int) ADCvalues[k+NUM_ADCVALUES*channel];
	}
	ADCstdev=0;
	for (k=0;k<NUM_ADCVALUES;k++){
		temp=(ADCtotal - 10*(unsigned int)ADCvalues[k+NUM_ADCVALUES*channel]);
	ADCstdev += (temp*temp);
	}
}



void updateDisplayStatus(void){

	clearDisplaySSD1306();
	strcpy(outText,"FUNCT'N: ");
	strcat(outText,MYFUNCTION);
	strlength=strlen(outText);
	writeCharSSD1306(0,0,outText,strlength,0);

	strcpy(outText,"RS485Address: 0x");
	itoa(address,&temp,16,2);
	strcat(outText,temp);
	strlength=strlen(outText);
	writeCharSSD1306(1,0,outText,strlength,0);

	strcpy(outText,"Sampling T: ");
	strlength=strlen(outText);
	writeCharSSD1306(2,0,outText,strlength,0);

	itoa((maxWaitLoop * 16),&outText,10,2);
	strcpy(temp," mS");
	strcat(outText,temp);
	strlength=strlen(outText);
	writeCharSSD1306(2,12,outText,strlength,0);

	strcpy(outText,"0 to +5V MAX input");
	strlength=strlen(outText);
	writeCharSSD1306(3,0,outText,strlength,0);
}


static void interrupt isr(void){
 if(T0IF) {	 
	waitLoop++;
	if (waitLoop==maxWaitLoop){
		//RA5=1;// debug. scope this to measure time spent getting ADC's
		waitLoop=0;
		loopCounter++;
		if (loopCounter==NUM_ADCVALUES) loopCounter=0;
		ADCvalues[loopCounter] = getADC(4);
		ADCvalues[loopCounter+NUM_ADCVALUES] = getADC(5);
		ADCvalues[loopCounter+NUM_ADCVALUES*2] = getADC(6);
		ADCvalues[loopCounter+NUM_ADCVALUES*3] = getADC(7);
		//RA4=!RA4; // debug. scope this to ensure this interrupt is executing and to watch dwell time
		// or leave to give visual indication the rate that ADC values are grabbed. 

		//RA5=0;// debug. scope this to measure time spent getting ADC's
	}
	}
    TMR0 = (255-interruptTime);             // reinitialize Value for Interrupt 
    T0IF = 0;		    // Clear TIMER0 interrupt flag.  This starts the counter again.
}

/*

With ADC clock as Fosc/2 : the four ADC converstions above takes 260uS
With ADC clock as Fosc/8:  the four ADC converstions above takes 286uS

*/

void main (void) {	

	char i;
	unsigned short reg;

	char error;
	unsigned char data[24];
	unsigned int outdata;
	char fnString[13];
	PORTA =0;    //clears port A
	TRISA=0; // Port A outputs
	TRISC = 0x0F;// RC0,1,2,3 digital inputs. So that we can use Analog in AN4,5,6,7
	PORTC = 0;    //clears port C
	CM1CON0 = 7;    //turn off comparators
	CM2CON0 = 7;
	CM2CON1 = 7;
	ANSEL = 0xF0;    //AN 7, 6, 5, & 4 are active. AN 0,1,2,3 analog in's are disabled.
// High register ANSELH is set in RS485_Init().  ANS11  !MUST! be off for UART RX operation

	
	OSCCON=0x70;         // Select 8 MHz internal clock.  UART timing/BAUD rate based on this.

	address=EEPROM_READ(0x00);

	RS485_Init();
	initADC();

 	initSSD1306();
	updateDisplayStatus();
	


/*
TIMER 0 INTERRUPT TIMING
Fosc	8000000		Period mS
Fosc/4	2000000	0.0000005	0.0005
Prescale
Last 3 bits of OPTION
		scale	Period mS per TMR0 tick
	000	2		0.001
	001	4		0.002
	010	8		0.004
	011	16		0.008
	100	32		0.016
	101	64		0.032
	110	128		0.064
	111	256		0.128

An interrupt is generated when TMR0 counts from 0xFF to 0x00.
So, the time between interrupts is (255 - TMR0) * period per tick
Smallest usable value of TMR0 is 1. Largest is 254.
*/

  OPTION = 0b00000111; // see table above
  TMR0=(255-interruptTime);   
  T0IE = 1;		       // Enable interrupt on TMR0 overflow
  GIE = 1;		       // Global interrupt enable
	strlength=0;
	error=0;

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
					delay(300);
					if (data[0]==address){ //address byte
			
						switch (data[1]){  // command code byte
				
							case 0x06: // write to a register
									reg = (data[2]<<8) | (data[3]);
								switch (reg){
										case BASEREG+16:
											outdata=data[5];  // assign new data to varible ("register")
										 if (outdata>10){
											maxWaitLoop=outdata;
											waitLoop=0;
											// formulate a response echo back command
											strlength=6;
											RS485_Write_Data(data,strlength);
											error = 0;
											}else error=0x06;
											break;
										case FNREG:
										if (data[4]==address){
											address=data[5];
											EEPROM_WRITE(0x00,address);
											strlength=6;
											RS485_Write_Data(data,strlength);
											error=0;
										} else {
										error = 0x02;
										}
									break;
										default:
										// invalid register
										error = 0x02;
										break;
										}
								break; // write to a register
							case 0x03:  // read a register
								/*
									This is where we select which ADC channel to read from.
									BASEREG    ==> ADC4
									BASEREG +1 ==> ADC5
									BASEREG +2 ==> ADC5
									BASEREG +3 ==> ADC7
								*/
								reg = (data[2]<<8) | (data[3]);
								switch (reg){
									case BASEREG:
									case BASEREG+1:
									case BASEREG+2:
									case BASEREG+3:
											outdata = getADC(reg-BASEREG+4);
											data[2]=2;  //number of bytes to follow of data
											data[3]=(outdata&0xFF00)>>8; //MSB first
											data[4]=(outdata&0x00FF);
											strlength=5;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
									case BASEREG+16:
											outdata = maxWaitLoop;
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
										// invalid register
										error = 0x02;
									break;
								}								
										
							break;// read a register
							case 0x08:  // read a register
								reg = (data[2]<<8) | (data[3]);
								if ((reg>=BASEREG)&(reg<=BASEREG+3)) {
											calculateADCstats(reg-BASEREG); // no +4 since this offset is accounted for in the interrup loop
											outdata = ADCtotal;
											data[2]=4;  //number of bytes to follow of data
											data[3]=(outdata&0xFF00)>>8; //MSB first
											data[4]=(outdata&0x00FF);
											outdata = ADCstdev;
											data[5]=(outdata&0xFF00)>>8; //MSB first
											data[6]=(outdata&0x00FF);
											strlength=7;
											RS485_Write_Data(data,strlength);
											error =0;
								} else {		
								error = 0x04; // value (channel) out of range
								}		
							break;// read a register
							default:
								// unknown command
								error = 0x08;
							break;
						}//end switch
						if (error) {
								data[1]= data[1] | 0x80;
								data[2]= (error & 0xFF00)>>8;
								data[3]=(error & 0x00FF);
								strlength=4;
								RS485_Write_Data(data,strlength);
						}
				updateDisplayStatus();

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