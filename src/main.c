#include "main.h"
#include "init.h"
#include "agent.h"

int main(int argc, char *argv[]) {

  // Check if we have proper command line arguments

  if (argc != 2 && argc != 3)
  {
    printf("Usage: %s [track.bmp] [angle=0 degrees]\n", argv[0]);
    exit(-1);
  }

  // Read the BMP file; extract pixels and size
  int width;
  int height;
  unsigned char* pixelData = readTrack(argv[1], &width, &height);

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
  agent->angle = 0;
  agent->xViewRange = 15;
  agent->xOffset = 1;
  agent->yViewRange = 15;
  agent->yOffset = 1;

  char* dummyString;
  if (argc == 3)
    agent->angle = strtod(argv[2], &dummyString);

  // Run the simulation
  simulate(agent, sortedHead, pixelData, width, height);

  // Do a final cleanup
  cleanup(sortedHead);
  free(agent);

  printf("Simulation successful; check img folder for captured data\n");

  return 0;
}
