#ifndef AGENT_H_   /* Include guard */
#define AGENT_H_

typedef struct AGENT{
  int xPos;
  int yPos;
  double angle;
  int xViewRange;
  int yViewRange;
  int xOffset;
  int yOffset;
  double stepSize;
  double rotStepSize;
} AGENT;

double twoPointAngle(int xPos1, int yPos1, int xPos2, int yPos2);
double* screenshot(AGENT* agent, unsigned char* pixelData, int width, int height);
double degreeRadConvert(double inp, char isRad);
void printViewport(AGENT* agent, double* aScreenshot);
void simulate(AGENT* agent, PIXEL_LINK* sortedHead, unsigned char* pixelData, int width, int height);
double errorCalculate(double deltaTheta);
void outputImage(double* aScreen, AGENT* agent, double error, int imageWidth, int imageHeight);

#endif
