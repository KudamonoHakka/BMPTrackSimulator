#include "main.h"
#include "init.h"

unsigned char* readTrack(char* fileName, int* w, int* h)
{
  // This whole first bit reads 24-bit BMP file and extracts pixel information
  FILE *fp = fopen("road.bmp", "rb");
  if (fp == NULL)
  {
      printf("Error: Could not open file.\n");
      exit(-1);
  }

  BITMAPFILEHEADER fileHeader;
  BITMAPINFOHEADER infoHeader;

  fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
  fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

  if (fileHeader.bfType != 0x4D42)
  {
      printf("Error: Not a BMP file.\n");
      fclose(fp);
      exit(-1);
  }

  fseek(fp, fileHeader.bfOffBits, SEEK_SET);

  int width = infoHeader.biWidth;
  int height = infoHeader.biHeight;

  // Make sure we pass over width and height information
  *w = width;
  *h = height;

  unsigned char *pixelData = malloc(3 * width * height);
  if (pixelData == NULL)
  {
      printf("Error: Could not allocate memory.\n");
      fclose(fp);
      exit(-1);
  }

  // Create a buffer with all of the pixel information
  fread(pixelData, 3 * width, height, fp);
  fclose(fp);
  return pixelData;
}

PIXEL_LINK** initTrack(unsigned char* pixelData, int width, int height)
{
  // Define the heads of the pixel linked list
  PIXEL_LINK* greenHead = 0;
  PIXEL_LINK* greenCurrent = 0;

  PIXEL_LINK* redHead = 0;
  PIXEL_LINK* redCurrent = 0;

  // Filter the image to be an average grey scale; spare the map points and add them to unsorted list
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {

        // Calculate index of the pixel
        int index = (y * width + x) * 3;

        // Extract original RBG values
        unsigned char blue = pixelData[index];
        unsigned char green = pixelData[index + 1];
        unsigned char red = pixelData[index + 2];

        // Computate original RGB value
        unsigned char avg = (blue + green + red) / 3;

        // Print out values if they're a landmark; else set to average RGB values
        if (red + green + blue == 255)
        {
          printf("Pixel at (%d, %d): R=%u, G=%u, B=%u\n", x, y, red, green, blue);

          // Create new pixel link in chain
          PIXEL_LINK* pl = malloc(sizeof(PIXEL_LINK));
          pl->xPos = x;
          pl->yPos = y;

          // Determine if this is a red or green PIXEL_LINK; add it to unsorted linked list
          if (red == 255)
          {
            if (redHead != 0)
              redCurrent->nextPixel = pl;
            else
              redHead = pl;

            redCurrent = pl;
          }
          else
          {
            if (greenHead != 0)
              greenCurrent->nextPixel = pl;
            else
              greenHead = pl;

            greenCurrent = pl;
          }


        }
        else
          for (int i = 0; i < 3; i++)
            pixelData[index + i] = avg;

        // Extract new RBG values
        blue = pixelData[index];
        green = pixelData[index + 1];
        red = pixelData[index + 2];
    }
  }
  PIXEL_LINK** return_array = malloc(sizeof(PIXEL_LINK*)*2);
  return_array[0] = redHead;
  return_array[1] = greenHead;
  return return_array;
}
