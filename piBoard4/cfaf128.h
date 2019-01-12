/* 
 * File:   cfaf128.h


DUAL LCD driver for cfaf128x128 displays

Each  Display is Crystalfontz CFAF128128B1-0145T
  https://www.crystalfontz.com/product/cfaf128128b10145t

RasPi Board V4 wiring connection:

LCD 0:
________________
		| - 1 [LED Anode] --\/\/[110R]/\/---- +5V
		| - 2 [LED cathode] --- gnd
sitronix	| - 3 [spi4/!spi3] ---- +3.3v
ST7735		| - 4 [VDD] ----------- +3.3v
		| - 5 [VSS] ----------- gnd
		| - 6 [CS] ------------>GPIO CE0
		| - 7 [RESET] --------->GPIO WPI23
		| - 8 [SPI DATA]------->GPIO MISO
		| - 9 [SPI_CLK]-------->GPIO SCK
________________| - 10 [DC Data/!Command]------->GPIO WPI25

LCD 1
________________
		| - 1 [LED Anode] --\/\/[110R]/\/---- +5V
		| - 2 [LED cathode] --- gnd
sitronix	| - 3 [spi4/!spi3] ---- +3.3v
ST7735		| - 4 [VDD] ----------- +3.3v
		| - 5 [VSS] ----------- gnd
		| - 6 [CS] ------------>GPIO CE1
		| - 7 [RESET] --------->GPIO WPI22
		| - 8 [SPI DATA]------->GPIO MISO
		| - 9 [SPI_CLK]-------->GPIO SCK
________________| - 10 [DC]------->GPIO WPI24

*/

#include "myPiSPI.h"
#include <wiringPi.h>
#include <stdio.h>

// chan is either 0 or 1 for each display respectively
void initLCD(unsigned char chan);
void displayPixels(char* pixels, unsigned char y1,unsigned char y2, unsigned char chan);
void fillLCD(unsigned short bcolor,unsigned char chan);


// these no longer public.  all image maniuplation is done at the simpleGFX level, and displayPixels is used
// to push the image data out to the display.
//void Set_LCD_for_write_at_X_Y(char x, char y,unsigned char chan);
//void Fill_LCD(char R, char G, char B,unsigned char chan);




