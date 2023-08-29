#include "main.h"
#include "init.h"
#include "agent.h"

int main() {
  // Read the BMP file; extract pixels and size
  int width;
  int height;
  unsigned char* pixelData = readTrack("road.bmp", &width, &height);

  // Initalize the track and get the two unsorted linked lists of colored points
  PIXEL_LINK** linkHeads = initTrack(pixelData, width, height);

  // Sort the linked list
  PIXEL_LINK* sortedHead = sortLinkedLists(linkHeads);

  // Define the agent and set it to the head of the sorted list
  AGENT* agent = malloc(sizeof(AGENT));

  agent->xPos = sortedHead->xPos;
  agent->yPos = sortedHead->yPos;


  agent->angle = twoPointAngle(sortedHead->xPos, sortedHead->yPos, sortedHead->nextPixel->xPos, sortedHead->nextPixel->yPos);

  agent->xViewRange = 20;
  agent->yViewRange = 10;

  double* aScreen = screenshot(agent, pixelData, width, height);


  for (int y = -agent->yViewRange; y <= agent->yViewRange; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange; x++)
    {
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x);
      printf("%d%d", aScreen[viewBufferIndex] > 0.0,aScreen[viewBufferIndex] > 0.0);
    }
    printf("\n");
  }

  // Do a final cleanup
  cleanup(sortedHead);
  free(aScreen);
  free(agent);

  return 0;
}
