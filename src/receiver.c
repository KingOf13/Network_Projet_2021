#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include "log.h"

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-s stats_filename] listen_ip listen_port", prog_name);
    return EXIT_FAILURE;
}

//creation of server adress
struct sockaddr_in6 create_address(char* listen_ip, uint16_t listen_port){
    struct sockaddr_in6 peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr)); 
    peer_addr.sin6_family = AF_INET6;                
    peer_addr.sin6_port = htons(listen_port);                 
    inet_pton(AF_INET6, listen_ip, &peer_addr.sin6_addr); 
    return peer_addr;
}

//creation of sender (client) address
struct sockaddr_in6 create_client_address() {
    struct sockaddr_in6 cli_addr;             
    memset(&cli_addr, 0, sizeof(cli_addr)); 
    return cli_addr;
}

//create socket
int create_socket(){
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); 
    if (sock == -1) {
        return -1;
    }
    return sock;
}

//receive message from sender (client) and send message back (-> still don't know how to separe them)
int receive_and_send_message(int sock, struct sockaddr_in6 cli_addr){
    char buffer[512]; 
    socklen_t len = sizeof(cli_addr);
    int n = recvfrom(sock, (char *)buffer, 512, 0, ( struct sockaddr *) &cli_addr, &len);
    buffer[n] = '\0'; 
    printf("Client : %s\n", buffer);
    char hello[1024]; 
    sprintf(hello, "Server anwser: %s", buffer);
    sendto(sock, (const char *)hello, strlen(hello), 0, (const struct sockaddr *) &cli_addr, len); 
    if (strcmp(buffer, "EOF") == 0)
    {
        printf("Server shutdown\n");
        return -1;
    }
    return 0;
}

//bind the socket to the server address
int bind_server(int sock, struct sockaddr_in6 peer_addr){
    if ( bind(sock, (const struct sockaddr *) &peer_addr, sizeof(peer_addr)) < 0 ){ 
        return -1;
    }
    return 0; 

}


int main(int argc, char **argv) {
    /*************
     * given argument handling part
     * **********/
    int opt;

    char *stats_filename = NULL;
    char *listen_ip = NULL;
    char *listen_port_err;
    uint16_t listen_port;

    while ((opt = getopt(argc, argv, "s:h")) != -1) {
        switch (opt) {
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

    listen_ip = argv[optind];
    listen_port = (uint16_t) strtol(argv[optind + 1], &listen_port_err, 10);
    if (*listen_port_err != '\0') {
        ERROR("Receiver port parameter is not a number");
        return print_usage(argv[0]);
    }
    /*************
     * end given argument handling part
     * **********/
    

    /*************
     * socket address and bind creation
     * **********/

    struct pollfd pollfd[2];
    int sock = create_socket();
    struct sockaddr_in6 peer_addr = create_address(listen_ip, listen_port);
    struct sockaddr_in6 cli_addr = create_client_address();
    bind_server(sock, peer_addr);
    
    pollfd[0].fd = sock;
    pollfd[0].events = POLLIN;
    pollfd[0].revents = 0;
    pollfd[1].fd = sock;
    pollfd[1].events = POLLOUT;
    pollfd[1].revents = 0;

    /*************
     * end socket address and bind creation
     * **********/

    /*************
     * message handling
     * **********/

    while (1)
    {
        int ret = poll(pollfd, 1, 1000);
        if (ret == -1)
        {
            printf("Error with poll");
            break;
        }
        if(pollfd[0].revents & POLLIN){
            if(pollfd[0].fd == sock){
                int res = receive_and_send_message(pollfd[0].fd, cli_addr);
                if (res == -1){break;}

                pollfd[1].fd = sock;
                pollfd[1].fd = POLLIN;
            }
            
        }
            
        
        
        
        //send_message(sock, cli_addr, len);
    }
    
    /*************
     * end message handling
     * **********/

    
    /*************
     * stats file handling
     * **********/

    if (stats_filename != NULL)
    {
        
    }
    

    /*************
     * end stats file handling
     * **********/
    
    

    
    
    /*************
     * given test + debug part
     * **********/

    ASSERT(1 == 1); // Try to change it to see what happens when it fails
    DEBUG_DUMP("Some bytes", 11); // You can use it with any pointer type

    // This is not an error per-se.
    ERROR("Receiver has following arguments: stats_filename is %s, listen_ip is %s, listen_port is %u",
        stats_filename, listen_ip, listen_port);

    DEBUG("You can only see me if %s", "you built me using `make debug`");
    ERROR("This is not an error, %s", "now let's code!");

    /*************
     * end given test + debug part
     * **********/
    // Now let's code!
    return EXIT_SUCCESS;
}