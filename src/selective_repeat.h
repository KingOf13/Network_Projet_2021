#include <stddef.h> /* size_t */
#include <stdint.h> /* uintx_t */
#include <stdio.h>  /* ssize_t */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "time.h"
#include <sys/types.h>
#include "packet_interface.h"

typedef struct window{
    pkt_t* window[32];
    time_t start_time[32];
    int last_ack;
}window_t;