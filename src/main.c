#include "main.h"
#include "init.h"
#include "agent.h"

void simulate(AGENT* agent, PIXEL_LINK* sortedHead, unsigned char* pixelData, int width, int height);
double errorCalculate(double deltaTheta);
void outputImage(double* aScreen, AGENT* agent, double error);

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


void outputImage(double* aScreen, AGENT* agent, double error)
{
  FILE *f;

  // We need to convert our double values of aScreen to unsigned char equals
  int w = agent->xViewRange*2+1;
  int h = agent->yViewRange*2+1;
  unsigned char* img = (unsigned char *)malloc(3*w*h);
  memset(img,0,3*w*h);

  // Assign values from our view matrix to formated BMP pixels
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      for (int b = 0; b < 3; b++)
        img[(y * w + x)*3 + b] = 0xFF - (char)aScreen[y * w + x]; // Invert the data so that 1 represents road and 0 represents nothing

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
  sprintf(fileName, "img\\%.2f (%x).bmp", error, (int)(agent->xPos * agent->yPos * (agent->angle + 1.5)));
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
      //printf("%.2f\n\n", errorCalculate(deltaTheta));
      outputImage(aScreen, agent, errorCalculate(deltaTheta));

      free(aScreen);
      //return;


      deltaTheta = agent->angle - twoPointAngle(pl->xPos, pl->yPos, pl->nextPixel->xPos, pl->nextPixel->yPos);
      //deltaTheta += (deltaTheta < 0)? 360 : 0;
      while (deltaTheta < 0)
        deltaTheta += 360;
      while(deltaTheta > 360)
        deltaTheta -= 360;
      //printf("Delta theta: %.2f\n", deltaTheta);

      double turnDirection = (deltaTheta > 180)? 1.0 : -1.0;

      //printf("Turn %c: %.2f\n\n", ((turnDirection == 1.0)? 'R' : 'L'), errorCalculate(deltaTheta));

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

      // Take snapshot of agent view matrix and save image
      aScreen = screenshot(agent, pixelData, width, height);

      //printViewport(agent, aScreen);
      //printf("0.0\n\n");
      outputImage(aScreen, agent, 0.0);
      free(aScreen);


    }

    // At this point we're close enough to the checkpoint to lock onto it
    agent->xPos = pl->nextPixel->xPos;
    agent->yPos = pl->nextPixel->yPos;
  }
  free(aScreen);
}
