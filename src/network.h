#ifndef NETWORK_H_   /* Include guard */
#define NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

struct sockaddr_in* connectServer(int port);

#endif
