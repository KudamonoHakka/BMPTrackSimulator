#include "main.h"
#include "network.h"


int connectServer(int port)
{
  // This function simply connects to a local socket given a port and returns that socket

  int sock = 0;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};
  char *message = "Hello from C client";

  // Creating socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("\n Socket creation error \n");
      return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);


  // Convert IPv4 and IPv6 addresses from text to binary form
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (serv_addr.sin_addr.s_addr == INADDR_NONE) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      printf("\nConnection Failed \n");
      return -1;
  }

  send(sock, message, strlen(message), 0);
  printf("Message sent\n");
  read(sock, buffer, 1024);
  printf("Message from server: %s\n", buffer);

  close(sock);

  return 0;
}
