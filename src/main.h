#ifndef MAIN_H_   /* Include guard */
#define MAIN_H_


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#pragma pack(1)

typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;



// Define custom structure

typedef struct PIXEL_LINK {
  int xPos;
  int yPos;
  struct PIXEL_LINK *nextPixel;
} PIXEL_LINK;

#endif
