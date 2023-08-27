#include "main.h"
#include "init.h"

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
  PIXEL_LINK* sortedHead = sortLinkedLists(linkHeads);

  printf("\nGoing through the sorted linked list: \n\n");

  for (PIXEL_LINK* pl = sortedHead; pl != NULL; pl = pl->nextPixel)
    printf("%x,  (%d, %d)\n", pl, pl->xPos, pl->yPos);


  // TODO: Don't forget to clean up linkHeads and all associated links to prevent memory leak
  // Do a final cleanup
  free(pixelData);

  return 0;
}
