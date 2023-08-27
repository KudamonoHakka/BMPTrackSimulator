#include "main.h"
#include "init.h"

PIXEL_LINK* sortLinkedLists(PIXEL_LINK** linkHeads);

int main() {

  // Read the BMP file; extract pixels and size
  int width;
  int height;
  unsigned char* pixelData = readTrack("road.bmp", &width, &height);

  // Initalize the track and get the two unsorted linked lists of colored points
  PIXEL_LINK** linkHeads = initTrack(pixelData, width, height);

  printf("\nGoing through the green linked list: \n\n");
  for (PIXEL_LINK* pl = linkHeads[1]; pl != NULL; pl = pl->nextPixel)
    printf("%x,  (%d, %d)\n", pl, pl->xPos, pl->yPos);

  printf("\nGoing through the red linked list: \n\n");
  for (PIXEL_LINK* pl = linkHeads[0]; pl != NULL; pl = pl->nextPixel)
    printf("%x,  (%d, %d)\n", pl, pl->xPos, pl->yPos);

  // Sort the linked list
  printf("\nGoing through the sorted linked list: \n\n");
  PIXEL_LINK* sortedHead = sortLinkedLists(linkHeads);

  for (PIXEL_LINK* pl = sortedHead; pl != NULL; pl = pl->nextPixel)
    printf("%x,  (%d, %d)\n", pl, pl->xPos, pl->yPos);

  // TODO: Don't forget to clean up linkHeads and all associated links to prevent memory leak
  // Do a final cleanup
  free(pixelData);

  return 0;
}

PIXEL_LINK* sortLinkedLists(PIXEL_LINK** linkHeads)
{
  // This function will combine the two unsorted colored linked lists into a single linked list based
  // on nearest neighbor

  char listIndex = 0;
  PIXEL_LINK* headPixel = linkHeads[listIndex];
  PIXEL_LINK* currentPixel = headPixel;

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

    for (PIXEL_LINK* pl = linkHeads[listIndex]; pl != NULL; pl = pl->nextPixel)
    {
      // Double assign just for clarity
      iterNextPixel = pl;

      // Check if our currently iterated pixel is closer to the current pixel than current next pixel
      int iterDistance = sqrt(pow(iterNextPixel->xPos - currentPixel->xPos, 2) + pow(iterNextPixel->yPos - currentPixel->yPos, 2));

      // If closer, then swap the next pixel for the iterated pixel
      if (iterDistance < nextDistance)
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
  //while(headPixel != currentPixel);
  return headPixel;
}
