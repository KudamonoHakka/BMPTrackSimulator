#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

// Linked list of pixels
typedef struct PIXEL_LINK {
  int xPos;
  int yPos;
  struct PIXEL_LINK *nextPixel;
} PIXEL_LINK;

typedef struct AGENT{
  int xPos;
  int yPos;
  double angle;
  int xViewRange;
  int yViewRange;
} AGENT;
