#include "readAnalog.h"

unsigned short getADC(short chan){
   //int j;
    //chan is a three bit number, MUST be in range of 0 to 7.
    // probably should put some error checking here.
    ADCON0 = 0b10000001; // 8 bit control register sets up the ADC
        // bit 7: 1=right justified data placement. see page 95
        // bit 6:  Use Vdd as the reference voltage.  ALL ADC conversions are relative.
        // bit 4, 3, 2 : Channel. Here 0b000 means channel 0
        // bit 1:  Start/finish.  Set this bit to start an AD Conversion, wait until it is cleared to know if done.
        // bit 0: Turn on the ADC
    ADCON0 = ADCON0 | (chan << 2); // bitwise OR operation.  Shift 'chan' by 2 bits and OR it with the register.
        // THIS PLACES CHAN IN THE CORRECT POSITION OF THE ADCON0 REGISTER.
    ADCON0 = ADCON0 | 0b00000010; // bitwise OR again.  basically sets bit 1 (not zero) to a 1.
    // now check bit 1 to see if we're done with the conversion
    // we'll check (ADCON0 & 0b00000010); //if bit 1=1, then (ADCON0 & 0b00000010) = 0b10 which logically evaluates
    // to a TRUE.  If bit1 = 0 then (ADCON0 & 0b00000010) = 0 whis is a FALSE (and we're done with the conversion.
    while (ADCON0 & 0b00000010)
    {//twiddle thumbs
    }// endless loop waiting until we're done.


return ((ADRESH<<8)| ADRESL); //because we right justified the data ADRESH is the highest 2 bits and ADRESL is the lowest 8 bits of the conversion.

}


void initADC(void){
	ADCON0 = 0b10000001; // 8 bit control register sets up the ADC
        // bit 7: 1: right justified data placement
        // bit 6:  Use Vdd as the reference voltage.  ALL ADC conversions are relative.
        // bit 4, 3, 2 : Channel. Here 0b000 means channel 0
        // bit 1:  Start/finish.  Set this bit to start, wait until it is cleared to know if done.
        // bit 0: Turn on the ADC
    ADCON1 = 0b00010000; // another 8 bit controll sets the clock speed for the ADC
          // Select the clock as Fosc/8
//	ADCON1 = 0b00000000; // another 8 bit controll sets the clock speed for the ADC
            // Select the clock as Fosc/2

}
