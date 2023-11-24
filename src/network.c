#include "main.h"
#include "network.h"


int connectServer(int port)
{
  // This function simply connects to a local socket given a port and returns that socket

  WSADATA wsaData;
  SOCKET sock = INVALID_SOCKET;
  struct sockaddr_in server;
  char *message = "Hello World";
  int result;

  // Initialize Winsock
  result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result != 0)
  {
      printf("WSAStartup failed: %d\n", result);
      return -1;
  }

  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
      printf("Could not create socket : %d\n", WSAGetLastError());
      WSACleanup();
      return -1;
  }

  // Setup the server address
  server.sin_addr.s_addr = inet_addr("127.0.0.1"); // server IP
  server.sin_family = AF_INET;
  server.sin_port = htons(port); // server port

  // Connect to server
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
      printf("connect failed. Error: %d\n", WSAGetLastError());
      closesocket(sock);
      WSACleanup();
      return -1;
  }

  return sock;
}

int recieveData(int sock, char* serverReply, int bufferSize)
{
  // Receive a reply from the server
  int recv_size;
  if ((recv_size = recv(sock, serverReply, bufferSize, 0)) == SOCKET_ERROR)
  {
    return WSAGetLastError();
  }
  serverReply[recv_size] = '\0'; // null-terminate the string
  return 0;
}

int sendReceive(int sock, char* sendBuffer, char* serverReply, int bufferSize)
{
  if (send(sock, sendBuffer, strlen(sendBuffer), 0) < 0)
  {
      int err = WSAGetLastError();
      return err;
  }

  int err = recieveData(sock, serverReply, BUFFER_SIZE);

  if (err != 0)
    return err;

  return 0;
}
