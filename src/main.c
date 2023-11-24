#include "main.h"
#include "init.h"
#include "agent.h"
#include "network.h"

#define PORT 31337

int main(int argc, char *argv[]) {

  // Check if we have proper command line arguments
  if (argc != 2)
  {
    printf("Usage: %s [track.bmp]\n", argv[0]);
    exit(-1);
  }

  // Read the BMP file; extract pixels and size
  printf("1\n");
  int width;
  int height;
  unsigned char* pixelData = readTrack(argv[1], &width, &height);
  printf("2\n");
  // Initalize the track and get the two unsorted linked lists of colored points
  PIXEL_LINK** linkHeads = initTrack(pixelData, width, height);
  printf("3\n");
  // Sort the linked list
  PIXEL_LINK* sortedHead = sortLinkedLists(linkHeads);
  printf("4\n");
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

  // Run the simulation
  //simulate(agent, sortedHead, pixelData, width, height);

  // Connect to server
  printf("Connecting to server...\n");
  int sock = connectServer(PORT);
  if (sock == -1)
    return sock;
  printf("Connection complete!\n");
    // Send test message
  char buffer[1024] = {0};
  char *message = "Hello from C client";
  send(sock, message, strlen(message), 0);
  printf("Message sent\n");
  read(sock, buffer, 1024);
  printf("Message from server: %s\n", buffer);
  close(sock);



  // Do a final cleanup
  cleanup(sortedHead);
  free(agent);

  printf("Simulation successful; check img folder for captured data\n");

  return 0;
}
