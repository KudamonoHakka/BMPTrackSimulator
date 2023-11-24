#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "init.h"
#include "agent.h"
#include "network.h"

double* screenshot(AGENT* agent, unsigned char* pixelData, int width, int height)
{
  // Define the view buffer of our agent
  double* viewBuffer = malloc(sizeof(double) * (agent->xViewRange*2+1 + agent->xOffset) * (agent->yViewRange*2+1 +agent->yOffset));

  // Start in the top-left relative to the agent; go through every square
  for (int y = -agent->yViewRange; y <= agent->yViewRange + agent->xOffset; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange + agent->yOffset; x++)
    {
      // Get relative angle of point; then add agent angle to find new angle

      double angle = 180 - twoPointAngle(0, 0, x, y) - 90;

      double distance = sqrt(pow(x, 2) + pow(y, 2));

      angle = (angle < 0)? angle+360 : angle;
      angle -= agent->angle;

      // Get relative X and Y positions based off of calculated angle
      double xChange = cos(degreeRadConvert(angle, 0))*distance;
      double yChange = -sin(degreeRadConvert(angle, 0))*distance;

      // Convert above values to whole number relative positions
      int relX = (int)(xChange + 0.5*((xChange >= 0)?1.0:-1.0));
      int relY = (int)(yChange + 0.5*((yChange >= 0)?1.0:-1.0));

      // With calculated index, populate viewBuffer
      int index = ((agent->yPos + relY) * (width+agent->xOffset) + (agent->xPos + relX)) * 3;
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1 + agent->xOffset) + (agent->xViewRange + x);

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
  // Debugging function; prints out the contents of agent's view matrix
  for (int y = -agent->yViewRange; y <= agent->yViewRange; y++)
  {
    for (int x = -agent->xViewRange; x <= agent->xViewRange; x++)
    {
      int viewBufferIndex = (agent->yViewRange + y) * (agent->xViewRange*2+1) + (agent->xViewRange + x);
      printf("%d", !(aScreenshot[viewBufferIndex] > 0.0));
    }
    printf("\n");
  }
}

void outputImage(double* aScreen, AGENT* agent, double error, int imageWidth, int imageHeight)
{
  FILE *f;

  // We need to convert our double values of aScreen to unsigned char equals
  int w = agent->xViewRange*2+1 + agent->xOffset;
  int h = agent->yViewRange*2+1 + agent->yOffset;
  unsigned char* img = (unsigned char *)malloc(3*w*h);

  // Check that we don't have a blank image; discard those
  int sum = 0x0;
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      sum += abs((char)aScreen[y * w + x]);
  if (sum < 2 * w * h)
    return;

  memset(img,0,3*w*h);

  // Assign values from our view matrix to formated BMP pixels
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      for (int b = 0; b < 3; b++)
        img[(y * w + x)*3 + b] = (aScreen[y * w + x] == 255.0)? 0x0 : 0xFF - abs((char)aScreen[y * w + x]); // Invert the data so that 1 represents road and 0 represents nothing

  int filesize = 54 + 3*w*h;

  // Below is a bunch of code I stole from Stackoverflow that formats a BMP file for us
  unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
  unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
  unsigned char bmppad[3] = {0,0,0};

  bmpfileheader[ 2] = (unsigned char)(filesize    );
  bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
  bmpfileheader[ 4] = (unsigned char)(filesize>>16);
  bmpfileheader[ 5] = (unsigned char)(filesize>>24);

  bmpinfoheader[ 4] = (unsigned char)(       w    );
  bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
  bmpinfoheader[ 6] = (unsigned char)(       w>>16);
  bmpinfoheader[ 7] = (unsigned char)(       w>>24);
  bmpinfoheader[ 8] = (unsigned char)(       h    );
  bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
  bmpinfoheader[10] = (unsigned char)(       h>>16);
  bmpinfoheader[11] = (unsigned char)(       h>>24);

  // Create appropriate file name and save
  char* fileName = malloc(100);
  sprintf(fileName, "img\\%.2f (%x) [%x].bmp", error, (int)(agent->xPos * agent->yPos * (agent->angle + 1.5)), imageWidth * imageHeight);
  f = fopen(fileName,"wb");
  free(fileName);

  // Write content into file
  fwrite(bmpfileheader,1,14,f);
  fwrite(bmpinfoheader,1,40,f);
  for(int i=0; i<h; i++)
  {
      fwrite(img+(w*(h-i-1)*3),3,w,f);
      fwrite(bmppad,1,(4-(w*3)%4)%4,f);
  }

  // Close file and cleanup
  fclose(f);
  free(img);

}

double errorCalculate(double deltaTheta)
{
  // This function produces a label in relationship to deltaTheta
  deltaTheta -= (deltaTheta > 180.0)? 360 : 0;

  // Map our deltaTheta (0, 90) to a sinusoidal function; if we exceed an absolute deltaTheta greater than 90, assume the value to be max turning
  return (abs(deltaTheta) < 90)? -sin(degreeRadConvert(deltaTheta, 0)): -abs(deltaTheta) / deltaTheta;;
}

void simulate(AGENT* agent, PIXEL_LINK* sortedHead, unsigned char* pixelData, int width, int height, int sock)
{
  // This will start our simulation and take model prediction information and apply to our agent

  // Agent view matrix
  double* aScreen;
  aScreen = screenshot(agent, pixelData, width, height);
  printViewport(agent, aScreen);


  /*
  // This function will run the simulation and take snapshots of the agent's view matrix
  double* aScreen;
  for (PIXEL_LINK* pl = sortedHead; pl->nextPixel != 0x0; pl = pl->nextPixel)
  {
    // Rotate the agent until we face point slowly; collect rotational training data
    double deltaTheta = agent->angle - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);
    deltaTheta += (deltaTheta < 0)? 360 : 0;
    while (abs(deltaTheta) > 4.0)
    {
      // Take snapshot of agent view matrix and save image
      aScreen = screenshot(agent, pixelData, width, height);
      outputImage(aScreen, agent, errorCalculate(deltaTheta), width, height);
      //printViewport(agent, aScreen);
      //printf("\n");
      free(aScreen);

      // Calculate delta theta between achieved checkpoint and further checkpoint
      deltaTheta = agent->angle - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);

      while (deltaTheta < 0)
        deltaTheta += 360;
      while(deltaTheta > 360)
        deltaTheta -= 360;

      // Calculate turn angle and apply that to our agent
      double turnDirection = (deltaTheta > 180)? 1.0 : -1.0;
      agent->angle += agent->rotStepSize * turnDirection;
    }

    //agent->angle = 360 - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos) + 90;
    agent->angle = twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);

    // Continuously approach the next checkpoint
    while (sqrt(pow(agent->xPos - pl->nextPixel->xPos, 2) + pow(agent->yPos - pl->nextPixel->yPos, 2)) > 10.0)
    {
      agent->angle = twoPointAngle(agent->xPos, agent->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);
      agent->xPos += (int)((cos(degreeRadConvert(360 - agent->angle + 90, 0)) * agent->stepSize));
      agent->yPos -= (int)((sin(degreeRadConvert(360 - agent->angle + 90, 0)) * agent->stepSize));

      // Take snapshot of agent view matrix and save image
      aScreen = screenshot(agent, pixelData, width, height);
      outputImage(aScreen, agent, 0.0, width, height);
      //printViewport(agent, aScreen);
      //printf("\n");
      free(aScreen);
    }

    // At this point we're close enough to the checkpoint to lock onto it
    agent->xPos = pl->nextPixel->xPos;
    agent->yPos = pl->nextPixel->yPos;
    
  }*/
  free(aScreen);
}
