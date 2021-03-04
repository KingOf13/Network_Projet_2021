#include "log.h"
#include "create_socket.h"
#include "packet_interface.h"

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-f filename] [-s stats_filename] receiver_ip receiver_port", prog_name);
    return EXIT_FAILURE;
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
     * socket creation and connection
     * **********/

    struct sockaddr_in6 peer_addr = create_address(receiver_ip, receiver_port);
    int sock = create_socket();
    if(sock == -1){return -1;}

    //connect not necessary in UDP (normally)
    /*int connect = connect_to_server(sock, peer_addr);
    if(connect == -1){return -1;}*/

    /*************
     * end socket creation 
     * **********/

    /*************
     * file handling
     * **********/

    //timeout to prevent recvfrom to block code
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    int window = 32;
    int seqnum = 0;

    //if no file is given, read the stdin input and send it as message to server (receiver)
    char line[512];

    if (filename == NULL){
            while(1){
                int res = fread(line, 512*8, 1, stdin);
                //printf("%s\n", line);
                //send_stdin_message(sock, line, peer_addr);
                int n = receive_message(sock, peer_addr);
                if(n == -1){
                    printf("No anwser from server\n");
                }
                if(res != 1){break;}
            }
            
    }else{
        FILE* fp = fopen(filename, "r");
        if (fp == NULL){return -1;}
        while(1){
            window--;
            int res = fread(line, 512, 1, fp);
            pkt_t* pkt = pkt_new();
            pkt_set_type(pkt, PTYPE_DATA);
            pkt_set_tr(pkt, 0);
            pkt_set_window(pkt, window);
            pkt_set_timestamp(pkt, 0);
            //printf("%d\n", );
            pkt_set_seqnum(pkt, seqnum);
            pkt_set_length(pkt, 512);
            //printf("ici %d\n", pkt_get_length(pkt));
            seqnum++;
            pkt_set_payload(pkt, line, 512);
            //printf("payload %s\n", pkt_get_payload(pkt));
            char* buf = malloc(sizeof(char)*4224);
            size_t* len = malloc(sizeof(int));
            *len = 4224;
            pkt_encode(pkt, buf, len);
            /*pkt_t* pkt2 = pkt_new();
            pkt_decode(buf, 4224, pkt2);
            printf("%s\n", pkt_get_payload(pkt2));*/
            send_stdin_message(sock, buf, peer_addr);
            receive_message(sock, peer_addr);
            memset(line, 0, 512);
            if(res != 1){break;}
        }
    }
    
    

    /*************
     * end file handling
     * **********/


    close(sock);

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