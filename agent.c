#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "init.h"
#include "agent.h"

double* screenshot(AGENT* agent, unsigned char* pixelData, int width, int height)
{
  // Define the view buffer of our agent
  double* viewBuffer = malloc(sizeof(double) * (agent->xViewRange*2+1) * (agent->yViewRange*2+1));

  // Start in the top-left relative to the agent; go through every square
  for (int y = -agent->yViewRange; y <= agent->yViewRange; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange; x++)
    {
      // Get relative angle of point; then add agent angle to find new angle

      double angle = 180 - twoPointAngle(0, 0, x, y) - 90;

      double distance = sqrt(pow(x, 2) + pow(y, 2));

      angle = (angle < 0)? angle+360 : angle;
      angle -= agent->angle;


      //printf("(%d, %d): %.2f\n", x, y, angle);

      // Get relative X and Y positions based off of calculated angle
      double xChange = cos(degreeRadConvert(angle, 0))*distance;
      double yChange = -sin(degreeRadConvert(angle, 0))*distance;

      // Convert above values to whole number relative positions
      int relX = (int)(xChange + 0.5*((xChange >= 0)?1.0:-1.0));
      int relY = (int)(yChange + 0.5*((yChange >= 0)?1.0:-1.0));

      //printf("(%d, %d)\n", x, y);
      //printf("(%d, %d): %.2f\n", relX, relY, angle);
      //printf("(%d, %d): %.2f\n", (int)(cos(degreeRadConvert(angle, 0))*distance+0.5), (int)(sin(degreeRadConvert(angle, 0))*distance+0.5), angle);

      // With calculated index, populate viewBuffer
      int index = ((agent->yPos + relY) * width + (agent->xPos + relX)) * 3;
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x);

      //printf("%d        %d\n", viewBufferIndex, index);
      viewBuffer[viewBufferIndex] = pixelData[index];

    }
  }
  return viewBuffer;
}

double degreeRadConvert(double inp, char isRad)
{
  return (isRad)? inp / M_PI * 180.0 : inp / 180.0 * M_PI;
}

double twoPointAngle(int xPos1, int yPos1, int xPos2, int yPos2)
{
  // Get raw theta value based on difference in two points
  double theta =  atan2(yPos1 - yPos2, xPos1 - xPos2) - M_PI / 2.0;

  // Return the value in degrees
  theta = degreeRadConvert(theta, 1);

  // Don't spit a negative result
  return (theta >= 0)? theta : theta + 360;
}

void printViewport(AGENT* agent, double* aScreenshot)
{
  for (int y = -agent->yViewRange; y <= agent->yViewRange; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange; x++)
    {
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x);
      printf("%d", aScreenshot[viewBufferIndex] > 0.0);
    }
    printf("\n");
  }
}
