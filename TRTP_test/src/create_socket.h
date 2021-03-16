#ifndef __CREATE_SOCKET_H_
#define __CREATE_SOCKET_H_
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include "packet_interface.h"

int create_socket();
struct sockaddr_in6 create_address(char*, uint16_t);
struct sockaddr_in6 create_client_address();
int bind_server(int, struct sockaddr_in6);
int connect_to_server(int, struct sockaddr_in6);

#endif