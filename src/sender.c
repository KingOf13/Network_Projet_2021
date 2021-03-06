#include "log.h"
#include "create_socket.h"
#include "packet_interface.h"
#include "selective_repeat.h"

int item_window_nb = 0;
int seqnum = 0;
int window_val = 1;

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-f filename] [-s stats_filename] receiver_ip receiver_port", prog_name);
    return EXIT_FAILURE;
}

int check_ack(window_t* window, int index){
    window->last_ack = index;
    while (1)
    {
        if(index < 0){
            index = 31;
        }
        //printf("index: %d\n", index);
        
        if(window->window[index%32] == NULL){
            
            break;
        }
        //printf("mod: %d\n", index%32);
        free(window->window[index%32]);
        window->start_time[index%32] = 0;
        window->window[index%32] = NULL;
        
        index--;
        item_window_nb--;
        //printf("win: %d\n", item_window_nb);
    }
    
}

int check_nack(int index, window_t* window, int sock, struct sockaddr_in6 peer_addr){
    char* buf = malloc(sizeof(char)*528);
    size_t* len = malloc(sizeof(int));
    *len = 1024;
    pkt_encode(window->window[index], buf, len);
    window->start_time[index] = clock();
    send_stdin_message(sock, buf, peer_addr, *len);
    free(buf);
    free(len);
}

int check_timer(window_t* window, int sock, struct sockaddr_in6 peer_addr){
    
    for (size_t i = 0; i < 32; i++)
    {
        pkt_t* pkt = window->window[i];
        if(pkt == NULL){continue;}
        //printf("%ld, %ld\n", i, clock() - window->start_time[i]);
        if(clock() - window->start_time[i] > 200){
            printf("add timer: %d\n", pkt_get_seqnum(pkt));
            char* buf = malloc(sizeof(char)*528);
            size_t* len = malloc(sizeof(int));
            *len = 1024;
            pkt_encode(pkt, buf, len);
            window->start_time[i] = clock();
            send_stdin_message(sock, buf, peer_addr, *len);
            free(buf);
            free(len);
        }
    }
    
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
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    struct pollfd pollfd[2];
    pollfd[0].fd = sock;
    pollfd[0].events = POLLIN;
    pollfd[0].revents = 0;
    

    //if no file is given, read the stdin input and send it as message to server (receiver)
    char* line = malloc(sizeof(char)*513);
    //make window init
    window_t* window = malloc(sizeof(window_t));
    window->last_ack = 0;
    FILE* fp;
    if (filename == NULL)
    {
        fp = stdin;
        pollfd[1].fd = fileno(stdin);
        pollfd[1].events = POLLIN;
        pollfd[1].revents = 0;
    }else{
        fp = fopen(filename, "r");
        if (fp == NULL){return -1;}
        pollfd[1].fd = fileno(fp);
        pollfd[1].events = POLLIN;
        pollfd[1].revents = 0;
    }
    
    int res = 1;
    while(1){
        int sock_poll_res = poll(pollfd, 2, 1000);
        if(sock_poll_res < 0){
            printf("error with poll\n");
        }
        if(pollfd[1].revents & POLLIN && res == 1 && seqnum <= window->last_ack+window_val){
            res = fread(line, 512, 1, fp);
            pkt_t* pkt = pkt_new();
            pkt_set_type(pkt, PTYPE_DATA);
            pkt_set_tr(pkt, 0);
            pkt_set_window(pkt, window_val);
            pkt_set_timestamp(pkt, 0);
            pkt_set_seqnum(pkt, seqnum);
            pkt_set_length(pkt, 512);
            pkt_set_payload(pkt, line, 512);
            window->window[seqnum%32] = pkt;
            item_window_nb++;
            printf("add: %d\n", seqnum);
            char* buf = malloc(sizeof(char)*528);
            size_t* len = malloc(sizeof(int));
            *len = 1024;
            pkt_encode(pkt, buf, len);
            window->start_time[seqnum%32] = clock();
            seqnum = (seqnum+1)%256;
            send_stdin_message(sock, buf, peer_addr, *len);
            free(buf);
            free(len);
            memset(line, 0, 512);
        }
        if(pollfd[0].revents & POLLIN){
            pkt_t* pkt_ack = receive_ack(sock, peer_addr);
            printf("ack: %d\n", pkt_get_seqnum(pkt_ack)-1);
            if(pkt_get_type(pkt_ack) == PTYPE_NACK){
                check_nack(pkt_get_seqnum(pkt_ack), window, sock, peer_addr);
            }else if(pkt_get_type(pkt_ack) == PTYPE_ACK){
                check_ack(window, pkt_get_seqnum(pkt_ack)-1);
            }
            window_val = pkt_get_window(pkt_ack);
            
        }
        check_timer(window, sock, peer_addr);
        if(res != 1 && item_window_nb <= 0){break;}
    }
    
    

    /*************
     * end file handling
     * **********/
    for (size_t i = 0; i < 32; i++)
    {
       // printf("OUI\n");
       if(window->window[i] != NULL){
           printf("WTF\n");
       }
    }
    free(window);
    free(line);
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