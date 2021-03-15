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
#include <stdbool.h> 
#include "packet_interface.h"
#include "create_socket.h"

typedef struct window_receiver {
        int window_size;
        pkt_t* window[32];
        int next_seqnum;
        int window_val;
}window_receiver_t;

window_receiver_t* init_receiver_window();

pkt_t* receive_ack(int, struct sockaddr_in6);
int receive_and_send_message(int, struct sockaddr_in6, window_receiver_t*);
int send_message(int, pkt_t*, struct sockaddr_in6);

#endif