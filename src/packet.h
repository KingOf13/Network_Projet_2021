#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h> 

typedef struct packet{
    unsigned int type: 2;
    unsigned int TR: 1;
    unsigned int window: 5;
    uint16_t length;
    uint8_t seqnum;
    uint32_t timestamp;
    char* payload;
}packet_t;

char* encode(packet_t*, unsigned int);
packet_t* decode(char*, unsigned int);