#include "cfaf128.h"

// Defines for the ST7735 registers.

// ref: https://www.crystalfontz.com/products/document/3277/ST7735_V2.1_20100505.pdf
#define ST7735_SLPOUT   0x11
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_RAMRD    0x2E
#define ST7735_MADCTL   0x36
#define ST7735_COLMOD   0x3A
#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5
#define ST7735_GAMCTRP1 0xE0
#define ST7735_GAMCTRN1 0xE1
#define ST7735_VSCRDEF 0x33
#define ST7735_VSCRSADD 0x37

#define SPI_SPEED 10000000
// Defines for piBoard V4 (RASPI_Robot)
// for display #1
//#define LCD_DC 24
//#define LCD_RST 22
// for display #0
#define LCD_DC 25
#define LCD_RST 23
// for display #1, just subtract 1

void SPI_sendCommand(unsigned char command, unsigned char chan)
  {
	unsigned char rx;
	digitalWrite(LCD_DC-chan,LOW);
	if (myPiSPIDataRW(chan,&command, &rx, 1) <0) printf("Error sending command %02x\n",command);
  }
void sendSPIData(unsigned char* data, unsigned int bytes, unsigned char chan)
  {
/*
rx is receive buffer and needs to be as big as the out going data buffer
The LCD display does not send data back, but SPI is inherently fullduplex. As n bits of data are sent out, n bits of unknown data is
shifted in to a receive buffer. this is why they need to be the same size.  Nothing is done with this data. 
The original wiringPiSPI function recieved into the same, outgoing data buffer. This would erase the data. 
In this implimentation, pixels are manipulated in another function/library buffer and should not be erased.
*/
	unsigned char rx[bytes];
	digitalWrite(LCD_DC-chan,HIGH);
	if (myPiSPIDataRW(chan,data,rx,bytes) <0) printf("Error sending data %02x\n",data);	

  }

void resetLCD(unsigned char chan){
//	printf("Reset display channel %d\n",chan);
	digitalWrite(LCD_RST-chan,HIGH);
	delay(150);//120mS max
	digitalWrite(LCD_RST-chan,LOW);
	delay(50);//10uS min
	digitalWrite(LCD_RST-chan,HIGH);
	delay(150);//120mS max
}


void initLCD(unsigned char chan){

	char outData[16];
	char gammaData[16]={0x0A,0x1C,0x0C,0x14,0x33,0x2B,0x24,0x28,0x27,0x25,0x2C,0x39,0x00,0x05,0x03,0x0d};
	char gamma2Data[16]={0x0A,0x1C,0x0C,0x14,0x33,0x2B,0x24,0x28,0x27,0x25,0x2D,0x3a,0x00,0x05,0x03,0x0d};

	// display 0 or 1
	chan &=1;

	pinMode(LCD_RST-chan,OUTPUT);
	pinMode(LCD_DC-chan,OUTPUT);

	myPiSPISetup(chan,SPI_SPEED);

	resetLCD(chan);

	SPI_sendCommand(ST7735_SLPOUT,chan);//SLPOUT (11h): Sleep Out
	delay(120);
	SPI_sendCommand(ST7735_FRMCTR1,chan);//In normal mode(Full colors) //FRMCTR1 (B1h): Frame Rate Control (In normal mode/ Full colors)
		outData[0]=0x02;
		outData[1]=0x35;
		//outData[3]=0x36;
		outData[2]=0x36;
		sendSPIData(outData,3,chan);
	SPI_sendCommand(ST7735_FRMCTR2,chan);//In Idle mode (8-colors)//FRMCTR2 (B2h): Frame Rate Control (In Idle mode/ 8-colors)
		outData[0]=0x02;
		outData[1]=0x35;
		outData[2]=0x36;
		sendSPIData(outData,3,chan);
	SPI_sendCommand(ST7735_FRMCTR3,chan);//In partial mode + Full colors //FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
		outData[0]=0x02;
		outData[1]=0x35;
		outData[2]=0x36;
		outData[3]=0x02;
		outData[4]=0x35;
		outData[5]=0x36;
		sendSPIData(outData,6,chan);
 	SPI_sendCommand(ST7735_INVCTR,chan); //INVCTR (B4h): Display Inversion Control
		outData[0]=0x07;
		sendSPIData(outData,1,chan);
  	SPI_sendCommand(ST7735_PWCTR1,chan);//PWCTR1 (C0h): Power Control 1
		outData[0]=0x02;
		outData[1]=0x02;
		sendSPIData(outData,2,chan);
	SPI_sendCommand(ST7735_PWCTR2,chan);//PWCTR2 (C1h): Power Control 2
  		outData[0]=0xC5;
		sendSPIData(outData,1,chan);
	SPI_sendCommand(ST7735_PWCTR3,chan);//PWCTR3 (C2h): Power Control 3 (in Normal mode/ Full colors)
		outData[0]=0x0D;
		outData[1]=0x00;
		sendSPIData(outData,2,chan);
	SPI_sendCommand(ST7735_PWCTR4,chan);//PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
		outData[0]=0x8D;
		outData[1]=0x1A;
		sendSPIData(outData,2,chan);
	SPI_sendCommand(ST7735_PWCTR5,chan);//PPWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
		outData[0]=0x8D;
		outData[1]=0xEE;
		sendSPIData(outData,2,chan);
	SPI_sendCommand(ST7735_VMCTR1,chan);//VMCTR1 (C5h): VCOM Control 1
		outData[0]=0x51;
		outData[1]=0x4D;
		sendSPIData(outData,2,chan);
 	SPI_sendCommand(ST7735_GAMCTRP1,chan);//GMCTRP1 (E0h): Gamma polarity Correction Characteristics Setting
		sendSPIData(gammaData,16,chan);
	SPI_sendCommand(ST7735_GAMCTRN1,chan);//GMCTRN1 (E1h): Gamma polarity Correction Characteristics Setting
		sendSPIData(gamma2Data,16,chan);
	SPI_sendCommand(ST7735_COLMOD,chan); //COLMOD (3Ah): Interface Pixel Format
		outData[0]=0x03;
		sendSPIData(outData,1,chan);
//  SPI_sendData(0x06);// Default: 0x06 => 18-bit/pixel
                     // IFPF[2:0] MCU Interface Color Format
                     // IFPF[2:0] | Format
                     //      000b | reserved
                     //      001b | reserved
                     //      010b | reserved
                     //      011b | 12-bit/pixel <<<0x03
                     //      100b | reserved
                     //      101b | 16-bit/pixel
                     //      110b | 18-bit/pixel   <<<<<0x06
                     //      111b | reserved

	SPI_sendCommand(ST7735_DISPON,chan);//Display On//DISPON (29h): Display On
	delay(1);

	SPI_sendCommand(ST7735_MADCTL,chan);//MADCTL (36h): Memory Data Access Control
		outData[0]=0x80;
		sendSPIData(outData,1,chan);
	//  SPI_sendData(0x40);// YXVL RH--
                     // |||| ||||-- Unused: 0
                     // |||| ||---- MH: Horizontal Refresh Order
                     // |||| |        0 = left to right
                     // |||| |        1 = right to left
                     // |||| |----- RGB: RGB vs BGR Order
                     // ||||          0 = RGB color filter panel
                     // ||||          1 = BGR color filter panel
                     // ||||------- ML: Vertical Refresh Order
                     // |||           0 = top to bottom
                     // |||           1 = bottom to top
                     // |||-------- MV: Row / Column Exchange
                     // ||--------- MX: Column Address Order  <<<<<
                     // |---------- MY: Row Address Order

  }
void Set_LCD_for_write_at_X_Y(char x, char y,unsigned char chan)  {
	char outData[4];

  //CASET (2Ah): Column Address Set
  // * The value of XS [15:0] and XE [15:0] are referred when RAMWR
  //   command comes.
  // * Each value represents one column line in the Frame Memory.
  // * XS [15:0] always must be equal to or less than XE [15:0]
  SPI_sendCommand(ST7735_CASET,chan); //Column address set
  //Write the parameters for the "column address set" command
		outData[0]=0x00;
		outData[1]=0x02+x;
		outData[2]=0x00;
		outData[3]=0x81;
		sendSPIData(outData,4,chan);
//  SPI_sendData(0x00);     //Start MSB = XS[15:8]
//  SPI_sendData(0x02 + x); //Start LSB = XS[ 7:0]
//  SPI_sendData(0x00);     //End MSB   = XE[15:8]
//  SPI_sendData(0x81);     //End LSB   = XE[ 7:0]
  //Write the "row address set" command to the LCD
  //RASET (2Bh): Row Address Set
  // * The value of YS [15:0] and YE [15:0] are referred when RAMWR
  //   command comes.
  // * Each value represents one row line in the Frame Memory.
  // * YS [15:0] always must be equal to or less than YE [15:0]
  SPI_sendCommand(ST7735_RASET,chan); //Row address set
  //Write the parameters for the "row address set" command
		outData[0]=0x00;
		outData[1]=0x02+y;
		outData[2]=0x00;
		outData[3]=0x81;
		sendSPIData(outData,4,chan);
//  SPI_sendData(0x00);     //Start MSB = YS[15:8]
 // SPI_sendData(0x01 + y); //Start LSB = YS[ 7:0]
 // SPI_sendData(0x00);     //End MSB   = YE[15:8]
 // SPI_sendData(0x80);     //End LSB   = YE[ 7:0]
  //Write the "write data" command to the LCD
  //RAMWR (2Ch): Memory Write
  SPI_sendCommand(ST7735_RAMWR,chan); //write data
  }



void fillLCD(unsigned short bcolor, unsigned char chan)  {
    /* 12 bit ; 4bits per pixel
     byte array:  [R,G][B,R][G,B]
     pixel        [n_even][n_odd]
     */
  int i,k;
  unsigned char color[3], R,G,B;
  Set_LCD_for_write_at_X_Y(0, 0,chan);

	B=(unsigned char)((bcolor & 0xF00)>>8);
	G=(unsigned char)((bcolor & 0x0F0)>>4);
	R=(unsigned char)(bcolor & 0x00F);

  color[0]= (R<<4)| G;
  color[1]= (B<<4)| R;
  color[2]=	(G<<4)| B;
  //Fill display with a given RGB value
  for (i = 0; i < 128; i++) {
      for (k=0;k<64;k++){  // color contains two pixels of data so 128/2 = 64
          sendSPIData(color,3,chan);
      }
  }


}



void displayPixels(char* pixels, unsigned char y1, unsigned char y2,unsigned char chan){

	/*	writes one entire row (y=constant) to display buffer at a time from "pixels" array.

	We can make this faster by only re-writing y1 to y2, knowing that is all that has changed. 

	Pass y1=0 y2=128 for full refresh.

	12 bits per pixel
	*/

	unsigned short i;
	Set_LCD_for_write_at_X_Y(0, y1,chan);
	char* temp;

	for (i=y1; i<y2; i++){
		temp = &pixels[i*192];  //128 * 1.5 bytes/pixel
		sendSPIData(temp,192,chan);
	}

}

