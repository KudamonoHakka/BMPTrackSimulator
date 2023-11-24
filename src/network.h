#ifndef NETWORK_H_   /* Include guard */
#define NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define BUFFER_SIZE 1024

int connectServer(int port);
int recieveData(int sock, char* serverReply, int bufferSize);

#endif
