#include "main.h"
#include "init.h"
#include "agent.h"

unsigned char* readTrack(char* fileName, int* w, int* h)
{
  // This whole first bit reads 24-bit BMP file and extracts pixel information
  FILE *fp = fopen(fileName, "rb");
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
          // Create new pixel link in chain
          PIXEL_LINK* pl = malloc(sizeof(PIXEL_LINK));
          pl->xPos = x;
          pl->yPos = y;

          // Determine if this is a red or green PIXEL_LINK; add it to unsorted linked list
          if (red == 255 || blue == 255)
          {

            if (blue == 255 && redHead == 0x0)
            {
              redHead = pl;
              redCurrent = pl;
            }
            else if (blue == 255 && redHead != 0x0)
            {
              redCurrent->nextPixel = redHead;
              pl->nextPixel = redHead->nextPixel;
              redHead->nextPixel = 0x0;
              redCurrent = redHead;
              redHead = pl;
            }
            else if (redHead != 0)
              redCurrent->nextPixel = pl;
            else
              redHead = pl;

            if (blue != 255)
              redCurrent = pl;

            // We want the agent to read this as a black pixel
            for (int i = 0; i < 3; i++)
              pixelData[index + i] = 0x0;
          }
          else
          {
            if (greenHead != 0)
              greenCurrent->nextPixel = pl;
            else
              greenHead = pl;

            greenCurrent = pl;
            // We want the agent to read this as a black pixel
            for (int i = 0; i < 3; i++)
              pixelData[index + i] = 0x0;
          }


        }
        else
          for (int i = 0; i < 3; i++)
            pixelData[index + i] = avg;

    }
  }
  PIXEL_LINK** return_array = malloc(sizeof(PIXEL_LINK*)*2);
  return_array[0] = redHead;
  return_array[1] = greenHead;
  return return_array;
}

PIXEL_LINK* sortLinkedLists(PIXEL_LINK** linkHeads)
{
  // This function will combine the two unsorted colored linked lists into a single linked list based
  // on nearest neighbor

  char listIndex = 0;
  PIXEL_LINK* headPixel = linkHeads[listIndex];
  PIXEL_LINK* currentPixel = headPixel;

  // Make sure to remove from old linked list
  linkHeads[listIndex] = headPixel->nextPixel;
  headPixel->nextPixel = 0x0;

  // Go from start to end of opposite linked list; append the closest link to the end of new list
  do
  {
    // Flip index to be the opposite linked list
    listIndex ^= 0x1;

    // nextPixel shall keep track of the previous pixel relative to nextPixel to fill in the linked gap
    PIXEL_LINK* nextPixel = linkHeads[listIndex];
    PIXEL_LINK* previousPixel = 0x0;
    int nextDistance = 0xFFFFFF;

    // All of the iter variables will maintain above information for iterated linked items
    PIXEL_LINK* iterNextPixel = linkHeads[listIndex];
    PIXEL_LINK* iterPreviousPixel = 0x0;

    for (PIXEL_LINK* pl = linkHeads[listIndex]; pl != 0x0; pl = pl->nextPixel)
    {
      // Double assign just for clarity
      iterNextPixel = pl;

      // Check if our currently iterated pixel is closer to the current pixel than current next pixel
      int iterDistance = sqrt(pow(iterNextPixel->xPos - currentPixel->xPos, 2) + pow(iterNextPixel->yPos - currentPixel->yPos, 2));

      // If closer, then swap the next pixel for the iterated pixel
      if (iterDistance < nextDistance && iterNextPixel != currentPixel)
      {
        nextPixel = iterNextPixel;
        previousPixel = iterPreviousPixel;
        nextDistance = iterDistance;
      }

      // Update the previous pixel to be current one
      iterPreviousPixel = pl;
    }

    // At this point we have the nextPixel that should be inserted next
    currentPixel->nextPixel = nextPixel;
    currentPixel = nextPixel;

    // Remove the inserted link from the old linked list
    if (previousPixel == 0x0) // Edge case; change out the head
    {
      linkHeads[listIndex] = nextPixel->nextPixel;
    }
    // Edge case; at the end of the list
    else if (nextPixel->nextPixel == 0x0)
    {
      previousPixel->nextPixel = 0x0;
    }
    else
    { // Fill in the gap of the old linked liast
      previousPixel->nextPixel = nextPixel->nextPixel;
    }

    // This should be the end of the new linked list; clear the next pixel link
    currentPixel->nextPixel = 0x0;

  } while(linkHeads[listIndex] != 0x0 && linkHeads[listIndex^0x1] != 0x0);
  return headPixel;
}

void cleanup(PIXEL_LINK* sortedHead)
{
  // Go through linked lists and free all links
  PIXEL_LINK* pl = sortedHead;
  while (pl != 0x0)
  {
    PIXEL_LINK* oldPl = pl;
    pl = pl->nextPixel;
    free(oldPl);
  }

}
