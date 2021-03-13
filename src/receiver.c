#include <poll.h>
#include "log.h"
#include "create_socket.h"
#include "packet_interface.h"
#include "handle_message.h"

extern int data_received;
extern int data_truncated_received;
extern int ack_sent;
extern int nack_sent;
extern int packet_duplicated;
int seqnum = 0;

w_buffer* init_window(){
  w_buffer* receive_window = malloc(sizeof(w_buffer))
  if (receive_window == NULL) return NULL;
  while (receive_window->window_size < 32) {
    receive_window->buffer[receive_window->window_size] = (char*) malloc(512*sizeof(char));
    if (receive_window->buffer[receive_window->window_size] == NULL) break;
    receive_window->len[receive_window->window_size] = 0;
    receive_window->window_size += 1;
  }
  return receive_window;
}

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
    tv.tv_sec = 2;
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
    boolean loop = true;
    w_buffer* buffer = init_window();

    while (loop)
    {
        int len = receive_and_send_message(sock, cli_addr, &loop, &buffer);
        seqnum++;
    }

    close(sock);

    /*************
     * end message handling
     * **********/


    /*************
     * stats file handling
     * **********/
    data_received--;
    if (stats_filename != NULL)
    {
        FILE* fp = fopen(stats_filename, "w+");
        fprintf(fp,"data_sent:%d\n", 0);
        fprintf(fp,"data_received:%d\n", data_received);
        fprintf(fp,"data_truncated_received:%d\n", data_truncated_received);
        fprintf(fp,"ack_sent:%d\n", ack_sent);
        fprintf(fp,"ack_received:%d\n", 0);
        fprintf(fp,"nack_sent:%d\n", nack_sent);
        fprintf(fp,"nack_received:%d\n", 0);
        fprintf(fp,"packet_ignored:%d\n", 0);
        fprintf(fp, "packet_duplicated:%d\n", packet_duplicated);
        fclose(fp);
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
