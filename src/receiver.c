#include <poll.h>
#include "log.h"
#include "create_socket.h"

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-s stats_filename] listen_ip listen_port", prog_name);
    return EXIT_FAILURE;
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

    int sock = create_socket();
    struct sockaddr_in6 peer_addr = create_address(listen_ip, listen_port);
    struct sockaddr_in6 cli_addr = create_client_address();
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    bind_server(sock, peer_addr);

    /*************
     * end socket address and bind creation
     * **********/

    /*************
     * message handling
     * **********/
    //timeout to prevent recvfrom to block code
    

    while (1)
    {
        int res = receive_and_send_message(sock, cli_addr);
        if (res == -1){break;}
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