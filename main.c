#include "main.h"
#include "init.h"

int main() {



  // Read the BMP file; extract pixels and size
  int width;
  int height;
  unsigned char* pixelData = readTrack("road.bmp", &width, &height);

  // Initalize the track and get the two unsorted linked lists of colored points
  PIXEL_LINK** linkHeads = initTrack(pixelData, width, height);

  //printf("%x    %x", linkHeads[0], linkHeads[1]);

  // Iterate through red linked list
  printf("Going through the red linked list: \n\n");
  for (PIXEL_LINK* pl = linkHeads[0]; pl != NULL; pl = pl->nextPixel)
    printf("%x\n", pl);

  printf("\nGoing through the green linked list: \n\n");
  for (PIXEL_LINK* pl = linkHeads[1]; pl != NULL; pl = pl->nextPixel)
    printf("%x\n", pl);

  // TODO: Don't forget to clean up linkHeads and all associated links to prevent memory leak
  // Do a final cleanup
  free(pixelData);

  return 0;
}
