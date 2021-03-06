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
pkt_t* receive_ack(int, struct sockaddr_in6);
int receive_and_send_message(int, struct sockaddr_in6);
int send_stdin_message(int, char*, struct sockaddr_in6, size_t);
struct sockaddr_in6 create_client_address();
int bind_server(int, struct sockaddr_in6);
int connect_to_server(int, struct sockaddr_in6);