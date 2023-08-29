#include "main.h"
#include "init.h"
#include "agent.h"

void simulate(AGENT* agent, PIXEL_LINK* sortedHead, unsigned char* pixelData, int width, int height);
double errorCalculate(double deltaTheta);

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
  agent->stepSize = 2.0;
  agent->rotStepSize = 2.0;
  //agent->angle = twoPointAngle(sortedHead->xPos, sortedHead->yPos, sortedHead->nextPixel->xPos, sortedHead->nextPixel->yPos);
  agent->angle = 0;
  agent->xViewRange = 25;
  agent->yViewRange = 10;

  simulate(agent, sortedHead, pixelData, width, height);

  // Do a final cleanup
  cleanup(sortedHead);
  free(agent);

  return 0;
}

double errorCalculate(double deltaTheta)
{
  // This function produces a label in relationship to deltaTheta
  deltaTheta -= (deltaTheta > 180.0)? 360 : 0;

  // Map our deltaTheta (0, 90) to a sinusoidal function; if we exceed an absolute deltaTheta greater than 90, assume the value to be max turning
  return (abs(deltaTheta) < 90)? -sin(degreeRadConvert(deltaTheta, 0)): abs(deltaTheta) / deltaTheta;
}

void simulate(AGENT* agent, PIXEL_LINK* sortedHead, unsigned char* pixelData, int width, int height)
{
  double* aScreen;
  for (PIXEL_LINK* pl = sortedHead; pl->nextPixel != 0x0; pl = pl->nextPixel)
  {
    // Rotate the agent until we face point slowly; collect rotational training data
    double deltaTheta = agent->angle - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);
    deltaTheta += (deltaTheta < 0)? 360 : 0;
    while (abs(deltaTheta) > 4.0)
    {
      aScreen = screenshot(agent, pixelData, width, height);
      //printViewport(agent, aScreen);
      free(aScreen);


      deltaTheta = agent->angle - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);
      //deltaTheta += (deltaTheta < 0)? 360 : 0;
      while (deltaTheta < 0)
        deltaTheta += 360;
      while(deltaTheta > 360)
        deltaTheta -= 360;
      //printf("Delta theta: %.2f\n", deltaTheta);

      double turnDirection = (deltaTheta > 180)? 1.0 : -1.0;

      printf("Turn %c: %.2f\n\n", ((turnDirection == 1.0)? 'R' : 'L'), errorCalculate(deltaTheta));

      agent->angle += agent->rotStepSize * turnDirection;
      //printf("A\n");
    }

    //agent->angle = 360 - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos) + 90;
    agent->angle = twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);

    // Continuously approach the next checkpoint
    while (sqrt(pow(agent->xPos - pl->nextPixel->xPos, 2) + pow(agent->yPos - pl->nextPixel->yPos, 2)) > 10.0)
    {
      //agent->angle = 360 - twoPointAngle(agent->xPos, agent->yPos, pl->xPos, pl->yPos)+90;
      agent->angle = twoPointAngle(agent->xPos, agent->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);
      agent->xPos += (int)((cos(degreeRadConvert(360 - agent->angle + 90, 0)) * agent->stepSize));
      agent->yPos -= (int)((sin(degreeRadConvert(360 - agent->angle + 90, 0)) * agent->stepSize));

      aScreen = screenshot(agent, pixelData, width, height);
      //printViewport(agent, aScreen);
      free(aScreen);

      //printf("Go forward %.2f; (%.2f, %.2f); distance: %.2f\n\n", agent->angle, cos(degreeRadConvert(360 - agent->angle + 90, 0)), sin(degreeRadConvert(360 - agent->angle + 90, 0)), sqrt(pow(agent->xPos - pl->nextPixel->xPos, 2) + pow(agent->yPos - pl->nextPixel->yPos, 2)));
      //printf("%.2f\n", sqrt(pow(agent->xPos - pl->xPos, 2) + pow(agent->yPos - pl->yPos, 2)));

    }

    // At this point we're close enough to the checkpoint to lock onto it
    agent->xPos = pl->nextPixel->xPos;
    agent->yPos = pl->nextPixel->yPos;

    printf("\n");
  }
  free(aScreen);
}
