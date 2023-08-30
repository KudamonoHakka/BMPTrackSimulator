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
  agent->stepSize = 2.0;
  agent->rotStepSize = 2.0;
  agent->angle = 0;
  agent->xViewRange = 25;
  agent->yViewRange = 10;

  // Run the simulation
  simulate(agent, sortedHead, pixelData, width, height);

  // Do a final cleanup
  cleanup(sortedHead);
  free(agent);

  return 0;
}
