#ifndef __SELECTIVE_REPEAT_H_
#define __SELECTIVE_REPEAT_H_
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
#include "handle_message.h"

typedef struct window_sender{
    pkt_t* window[32];
    time_t start_time[32];
    int last_ack;
}window_sender_t;




window_sender_t* window_sender_init();
int check_ack(window_sender_t*, int, int);
int resend_nack(int, window_sender_t*, int sock, struct sockaddr_in6);
int check_timer(window_sender_t*, int, struct sockaddr_in6);
int set_min_max_rtt(time_t);
bool process_pkt(pkt_t*, window_receiver_t*);

#endif