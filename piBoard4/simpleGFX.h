/* 
 * File:   simpleGFX.h
 * Author: tranthamkw
 *
 * Created on June 30, 2018, 7:10 PM
 */




void initDisplay(unsigned char chan);// only sets up communication. nothing else

short loadBMP(char* filename);// reads in 128x128x24bit into pixelArray buffer, reduces color depth to 12bit

void updateDisplay(unsigned char chan);// pushes pixelArray buffer to display.

//drawing tools. Works on pixelArray buffer.  UpdateDisplay needs to be called to actually effect the changes.
void clearCanvas(unsigned short color);// makes all pixels this color.
void drawBox(short x1, short y1, short x2, short y2, unsigned short color);
void drawCircle(short x0, short y0, short r, unsigned short color);
void drawLine(short x0, short y0, short x1, short y1, unsigned short color);
void setPixel (short x0, short y0, unsigned short color);
void writeText(unsigned char x0,unsigned char y0, char* c, char length, unsigned short fcolor);

void charToHex (unsigned int value, char *result, char numPos );


//quickly manipulates only rows row to 9+row and pushes text to display.
void printLine(unsigned char row, char* c, char length, unsigned short fcolor, unsigned short bcolor, unsigned char chan);


// 12 bits per pixel
// each pixel RGB,4 bits each
#define RED 0xF00
#define BLUE 0x00F
#define CYAN 0x0FF
#define GREEN 0x0F0
#define WHITE 0xFFF
#define BLACK 0x000
#define YELLOW 0xFF0
#define GRAY 0x888



