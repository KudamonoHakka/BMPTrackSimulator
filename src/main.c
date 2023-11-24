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
  agent->angle = 315;
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


  simulate(agent, sortedHead, pixelData, width, height, sock);


  // Example networking code

  /*
  // Recieve information from server
  char* recvData = malloc(BUFFER_SIZE);
  char* message = "Hello World";

  for (int i = 0; i < 5; i++)
  {
    int err = sendReceive(sock, message, recvData, BUFFER_SIZE);
    if (err != 0)
    {
      printf("Error!!!\n");
      closesocket(sock);
      WSACleanup();
      return err;
    }

    printf("Data: %s\n", recvData);
    memset(recvData, 0x0, BUFFER_SIZE);
  }
  */

  // Do a final cleanup
  cleanup(sortedHead);
  //free(recvData);
  free(agent);
  closesocket(sock);
  WSACleanup();

  printf("Simulation successful\n");

  return 0;
}
