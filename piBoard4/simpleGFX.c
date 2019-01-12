#include "simpleGFX.h"
#include "font.h"
#include "cfaf128.h"
#include <stdlib.h>

unsigned char pixelBuffer[24576];
/*
	128 lines (y-coordinate) * 128 columns (x-coordinate) * 1.5 bytes per pixel = 24576

4 bits per color so 12 bits of information per pixel
image is manipulated in pixelBuffer, then send pixel information in cfaf128.c one, complete line at at time.
pixelBuffer is char, so holds two of the three R, G, B values needed for a  pixel. Algorithm depends on if the pixel x value is even or odd.

Let Line# =0 (y=0)

pixelBuff[k]	k=	 0    1    2   3   4	 5   6    7    8						189  190  191
	k(x even)=								 3*x/2	 +1
	k (x odd)=			integer division trunc decimal-->		 3*x/2   +1
color image data =	(RG)(B  R)(GB)(RG)(B  R)(GB)(RG)(B  R)(GB)  . . .	(RG)	(B  R)	(GB)   . . .	(RG)(B  R)(GB)
Pixel x =		[  0  ][  1  ][  2  ][  3  ][  4  ][  5  ]		[   even  ][  odd  ]		[ 126 ][ 127 ]

for line number  y,  k = 3*x/2 + 192*y

*/
short loadBMP(char * filename){
/*		open a BMP and write to the pixelBuffer.
	BMP must be 128x128 and 24 bpp
 */
	short loaded = 0;
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;

	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPels;
	int biYPels;
	unsigned int biClrUsed;
	unsigned int biClrImportant;

	FILE *in;

	char data[3];
	unsigned short x,y,k;
	unsigned char red,green,blue;

	in=fopen(filename,"rb");
	if (in==NULL){
		printf("Splash image file does not exist\n");
		fclose(in);
		return loaded;
	}
	fread(&bfType,sizeof(bfType),1,in);
	fread(&bfSize,sizeof(bfSize),1,in);
	fread(&bfReserved1,sizeof(bfReserved1),1,in);
	fread(&bfReserved2,sizeof(bfReserved2),1,in);
	fread(&bfOffBits,sizeof(bfOffBits),1,in);
/*	printf("Type\t%d\n",bfType);
	printf("Size\t%d\n",bfSize);
	printf("OffBits\t%d\n",bfOffBits);
*/
	if (bfType!=19778) {
		printf("filename %s not a DIB\n",filename);
		fclose(in);
		return loaded;
	}

	fread(&biSize, sizeof(biSize),1,in);
	fread(&biWidth,sizeof(biWidth),1,in);
	fread(&biHeight,sizeof(biHeight),1,in);
	fread(&biPlanes,sizeof(biPlanes),1,in);
	fread(&biBitCount,sizeof(biBitCount),1,in);
	fread(&biCompression,sizeof(biCompression),1,in);
	fread(&biSizeImage,sizeof(biSizeImage),1,in);
	fread(&biXPels,sizeof(biXPels),1,in);
	fread(&biYPels,sizeof(biYPels),1,in);
	fread(&biClrUsed,sizeof(biClrUsed),1,in);
	fread(&biClrImportant,sizeof(biClrImportant),1,in);
/*
	printf("Width\t%d\n",biWidth);
	printf("Height\t%d\n",biHeight);
	printf("BitPerPixel\t%d\n",biBitCount);
*/
	if (biBitCount!=24){
		printf("filename %s not 24 bit per pixel\n",filename);
		fclose(in);
		return loaded;
	}
	if (biWidth!=128){
		printf("filename %s not 128 pixels wide\n",filename);
		fclose(in);
		return loaded;
	}
	if (biHeight!=128){
		printf("filename %s not 128 pixels high\n",filename);
		fclose(in);
		return loaded;
	}
	loaded = 1;
	for (y=0;y<128;y++){
	for (x=0;x<128;x++){
		fread(&data,sizeof(data),1,in);
			red = (unsigned char) ((data[0]&0xF0)>>4);
			green = (unsigned char) ((data[1]&0xF0)>>4);
			blue = (unsigned char) ((data[2]&0xF0)>>4);
	//is x even or odd
		if((x & 0x01) == 1) {
		//odd
			k=(x*3/2);
			pixelBuffer[y*192 + k]=(pixelBuffer[y*192 +k] & 0xF0) | (red);
			pixelBuffer[y*192 + k+1]=(green<<4) | blue;
		} else {
			//even
			k=(x*3/2);
		pixelBuffer[y*192 + k] = (red <<4) | green;
		pixelBuffer[y*192 + k + 1] = (pixelBuffer[y*192 + k+1] & 0x0F) | (blue<<4);
		}

	}
	}
	fclose(in);

return loaded;


}

void initDisplay(unsigned char chan){
	    initLCD(chan);
}

void updateDisplay(unsigned char chan){
    displayPixels(pixelBuffer, 0, 128, chan);
}

void drawBox(short x1, short y1, short x2, short y2, unsigned short color){
	unsigned char i,j;
	for (i=x1;i<x2; i++){
		setPixel(i,y1,color);
		setPixel(i,y2,color);
	}
	for (j=y1;j<y2;j++){
		setPixel(x1,j,color);
		setPixel(x2,j,color);
	}
}

void drawLine(short x0, short y0, short x1, short y1, unsigned short color){
	char steep = (abs(y1-y0) > abs(x1-x0));
	short t,dx,dy,err,ystep;
	if (steep){
		t=x0; // swap x0 and y0
		x0=y0;		y0=t;
		t=x1; // swap x1 and y1
		x1=y1;		y1=t;
	}
	if (x0>x1){
		t=x0; //swap x0 and x1
		x0=x1;		x1=t;
		t=y0; //swap y0 and y1
		y1=y0;		y0=t;
	}
	dx=x1-x0; dy=abs(y1-y0); err = dx/2;
	if (y0<y1){
		ystep = 1;
	} else {
		ystep = -1;
	}
	for (;x0<=x1;x0++){
		if (steep){
			setPixel(y0,x0,color);
		} else {
			setPixel(x0,y0,color);
		}
		err -= dy;
		if (err<0){
			y0+=ystep; err +=dx;
		}
	}
}


void drawCircle(short x0, short y0, short r, unsigned short color){
	short f = 1- r;
	short ddFx = 1;
	short ddFy = -2*r;
	short x=0;
	short y=r;

	setPixel(x0,y0+r,color);	setPixel(x0,y0-r,color);
	setPixel(x0+r,y0,color);	setPixel(x0-r,y0,color);

	while (x<y){
		if (f>=0){
		y--; 	ddFy+=2;	f+=ddFy;
		}
	x++; 	ddFx+=2;	f+=ddFx;
	setPixel(x0+x,y0+y,color);	setPixel(x0-x,y0+y,color);
	setPixel(x0+x,y0-y,color);	setPixel(x0-x,y0-y,color);
	setPixel(x0+y,y0+x,color);	setPixel(x0-y,y0+x,color);
	setPixel(x0+y,y0-x,color);	setPixel(x0-y,y0-x,color);
	}
}

void setPixel (short x0, short y0, unsigned short color){
	unsigned short k,x,y;
	char red, green, blue;
	blue = (char) ((color & 0xF00)>>8);
	green = (char)((color & 0x0F0)>>4);
	red = (char)(color & 0x00F);
	//error trap  0 < x,y < 128
	x = ((unsigned short) x0)& 0x7F;
	y = ((unsigned short) y0)& 0x7F;

	//is x even or odd
	if((x & 0x01) == 1) {
		//odd
		k=(x*3/2);
		pixelBuffer[y*192 + k]=(pixelBuffer[y*192 +k] & 0xF0) | (red);
		pixelBuffer[y*192 + k+1]=(green<<4) | blue;
	} else {
		//even
		k=(x*3/2);
	pixelBuffer[y*192 + k] = (red <<4) | green;
	pixelBuffer[y*192 + k + 1] = (pixelBuffer[y*192 + k+1] & 0x0F) | (blue<<4);
	}
}


void clearCanvas(unsigned short color){
	unsigned short k,x,y;
	char red, green, blue;
	blue = (char) ((color & 0xF00)>>8);
	green = (char)((color & 0x0F0)>>4);
	red = (char)(color & 0x00F);
	for (y=0;y<128;y++){
	for (x=0;x<128;x++){
	//is x even or odd
	if((x & 0x01) == 1) {
		//odd
		k=(x*3/2);
		pixelBuffer[y*192 + k]=(pixelBuffer[y*192 +k] & 0xF0) | (red);
		pixelBuffer[y*192 + k+1]=(green<<4) | blue;
	} else {
		//even
		k=(x*3/2);
	pixelBuffer[y*192 + k] = (red <<4) | green;
	pixelBuffer[y*192 + k + 1] = (pixelBuffer[y*192 + k+1] & 0x0F) | (blue<<4);
	}
	}// x
	}//y
}


void writeText(unsigned char x0,unsigned char y0, char* c, char length, unsigned short fcolor){
/*
	writes text over existing image contained in pixel buffer.
*/
	char i,j,k,s;
        for (i=0; i<length;i++){
            for (j=0;j<5;j++){ // x direction
                for (k=0;k<8;k++){  // y direction
                    s=(0x01 << k);
                    if ((myFont[c[i]*5+j] & s)==s){
                        setPixel(i*6+j+x0, 7-k+y0, fcolor);}
				}
			}
		}
}

void printLine(unsigned char row, char* c, char length, unsigned short fcolor, unsigned short bcolor,unsigned char chan){
	/*
		fast clear bottom line and write a string. 
	*/
	unsigned short k,x,y;
	char red, green, blue;
	blue = (char) ((bcolor & 0xF00)>>8);
	green = (char)((bcolor & 0x0F0)>>4);
	red = (char)(bcolor & 0x00F);

	if (row > 119) row = 119;
	//clear line
	for (y=row;y<(9+row);y++){
	for (x=0;x<128;x++){
	//is x even or odd
		if((x & 0x01) == 1) {
		//odd
			k=(x*3/2);
			pixelBuffer[y*192 + k]=(pixelBuffer[y*192 +k] & 0xF0) | (red);
			pixelBuffer[y*192 + k+1]=(green<<4) | blue;
		} else {
		//even
			k=(x*3/2);
			pixelBuffer[y*192 + k] = (red <<4) | green;
			pixelBuffer[y*192 + k + 1] = (pixelBuffer[y*192 + k+1] & 0x0F) | (blue<<4);
		}
	}
	}
	// write the text to image data
	writeText(0,row,c,length,fcolor);
	//push out this change immediately
    displayPixels(pixelBuffer, row, row+9, chan);
}


/*
void itoa (int value, char *result, int base){

	char* ptr=result, *ptr1 = result, tmp_char;
	int tmp_value;
	if (base<2 || base > 16){
	*result = '\0';
		} else {
			do {
				tmp_value=value;
				value /=base;
				*ptr++="FEDCBA9876543210123456789ABCDEF" [15+(tmp_value-value*base)];
			} while (value);

			if (tmp_value<0) *ptr++='-';
			*ptr--='\0';
			while (ptr1<ptr){
				tmp_char=*ptr;
				*ptr-- = *ptr1;
				*ptr1++ = tmp_char;
			}


	}

*/
void charToHex (unsigned int value, char *result, char minPos ){
	char* ptr=result, *ptr1 = result, tmp_char;
	unsigned int tmp_value;
	char i=0;
	do {
	tmp_value=value;
	value /=16;
	*ptr++="0123456789ABCDEF" [(tmp_value-value*16)];
         i++;
	} while (value);

            while (i<minPos){
                *ptr++="0"[0];
                i++;
            }
			*ptr--='\0';
			while (ptr1<ptr){
				tmp_char=*ptr;
				*ptr-- = *ptr1;
				*ptr1++ = tmp_char;
			}
}
