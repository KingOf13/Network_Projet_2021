#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.h"

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-f filename] [-s stats_filename] receiver_ip receiver_port", prog_name);
    return EXIT_FAILURE;
}

//creation of server adress
struct sockaddr_in6 create_address(char* receiver_ip, uint16_t receiver_port){
    struct sockaddr_in6 peer_addr;             
    memset(&peer_addr, 0, sizeof(peer_addr));        
    peer_addr.sin6_family = AF_INET6;                
    peer_addr.sin6_port = htons(receiver_port);                 
    inet_pton(AF_INET6, receiver_ip, &peer_addr.sin6_addr); 
    return peer_addr;
}

//send message from stdin input "./sender ::1 12345 < file.txt"
int send_stdin_message(int sock, char* buffer, struct sockaddr_in6 peer_addr){
    sendto(sock, (const char*)buffer, strlen(buffer), 0, (const struct sockaddr *) &peer_addr, sizeof(peer_addr));
    return 0;
}

//receive message back from server
int receive_message(int sock, struct sockaddr_in6  peer_addr){
    char buf[512];
    socklen_t len_peer = sizeof(struct sockaddr_in6);
    int n = recvfrom(sock, (char *)buf, 512, 0, (struct sockaddr *) &peer_addr, &len_peer); 
    buf[n] = '\0'; 
    printf("Server : %s\n", buf);
    return 0;
}

//create socket
int create_socket(){
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); 
    if (sock == -1) {
        return -1;
    }
    return sock;
}


int main(int argc, char **argv) {
    /*************
     * given argument handling part
     * **********/

    int opt;

    char *filename = NULL;
    char *stats_filename = NULL;
    char *receiver_ip = NULL;
    char *receiver_port_err;
    uint16_t receiver_port;

    while ((opt = getopt(argc, argv, "f:s:h")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        case 'h':
            return print_usage(argv[0]);
        case 's':
            stats_filename = optarg;
            break;
        default:
            return print_usage(argv[0]);
        }
    }

    if (optind + 2 != argc) {
        ERROR("Unexpected number of positional arguments");
        return print_usage(argv[0]);
    }

    receiver_ip = argv[optind];
    receiver_port = (uint16_t) strtol(argv[optind + 1], &receiver_port_err, 10);
    if (*receiver_port_err != '\0') {
        ERROR("Receiver port parameter is not a number");
        return print_usage(argv[0]);
    }

    /*************
     * end given argument handling part
     * **********/
    

    /*************
     * socket creation 
     * **********/

    struct sockaddr_in6 peer_addr = create_address(receiver_ip, receiver_port);
    int sock = create_socket();

    /*************
     * end socket creation 
     * **********/

    /*************
     * file handling
     * **********/

    //if no file is given, read the stdin input and send it as message to server (receiver)
    if (filename == NULL)
    {
        char line[512];
        while(fgets(line, 512, stdin)!= NULL){
            //printf("%s\n", line);
            send_stdin_message(sock, line, peer_addr);
            receive_message(sock, peer_addr);
        }
    }
    

    /*************
     * end file handling
     * **********/


    close(sock);

    /*************
     * given test + debug part
     * **********/

    ASSERT(1 == 1); // Try to change it to see what happens when it fails
    DEBUG_DUMP("Some bytes", 11); // You can use it with any pointer type

    // This is not an error per-se.
    ERROR("Sender has following arguments: filename is %s, stats_filename is %s, receiver_ip is %s, receiver_port is %u",
        filename, stats_filename, receiver_ip, receiver_port);

    DEBUG("You can only see me if %s", "you built me using `make debug`");
    ERROR("This is not an error, %s", "now let's code!");

    // Now let's code!
    /*************
     * end given test + debug part
     * **********/
    return EXIT_SUCCESS;
}