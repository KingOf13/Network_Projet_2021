#ifndef __HANDLE_MESSAGE_H_
#define __HANDLE_MESSAGE_H_
#include <stddef.h> /* size_t */
#include <stdint.h> /* uintx_t */
#include <stdio.h>  /* ssize_t */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "time.h"
#include <sys/types.h>
#include "packet_interface.h"
#include "create_socket.h"


pkt_t* receive_ack(int, struct sockaddr_in6);
int receive_and_send_message(int, struct sockaddr_in6);
int send_message(int, pkt_t*, struct sockaddr_in6);

#endif