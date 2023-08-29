#include "main.h"
#include "init.h"

double twoPointAngle(int xPos1, int yPos1, int xPos2, int yPos2);
double* screenshot(AGENT* agent, unsigned char* pixelData, int width, int height);

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

  //agent->xViewRange = 100;
  //agent->yViewRange = 50;


  double* test = screenshot(agent, pixelData, width, height);


  for (int y = -agent->yViewRange; y <= agent->yViewRange; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange; x++)
    {
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x);
      printf("%d", test[viewBufferIndex] > 0.0);
    }
    printf("\n");
  }

  /*
  for (PIXEL_LINK* pl = sortedHead; pl->nextPixel != 0x0; pl = pl->nextPixel)
    printf("Angle between (%d, %d) and (%d, %d) is %.2f degrees\n",
        pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos, twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos));
  */

  // Do a final cleanup
  cleanup(sortedHead);
  free(test);
  free(agent);

  return 0;
}

double* screenshot(AGENT* agent, unsigned char* pixelData, int width, int height)
{
  // Define the view buffer of our agent
  double* viewBuffer = malloc(sizeof(double) * (agent->xViewRange*2+1) * (agent->yViewRange*2+1));

  // Start in the top-left relative to the agent; go through every square
  for (int y = -agent->yViewRange; y <= agent->yViewRange; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange; x++)
    {
      int index = ((agent->yPos + y) * width + (agent->xPos + x)) * 3;
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x);
      //printf("(%d, %d) ", (agent->xPos + x), (agent->yPos + y));
      //printf("[%d] ", ((agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x)));
      viewBuffer[viewBufferIndex] = pixelData[index];
    }
  }
  return viewBuffer;
}

double twoPointAngle(int xPos1, int yPos1, int xPos2, int yPos2)
{
  // Get raw theta value based on difference in two points
  double theta =  atan2(yPos1 - yPos2, xPos1 - xPos2) - M_PI / 2.0;

  // Return the value in degrees
  theta = theta / M_PI * 180.0;

  // Don't spit a negative result
  return (theta >= 0)? theta : theta + 360;
}
