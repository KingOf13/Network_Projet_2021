#include "log.h"
#include "packet.h"
#include "create_socket.h"

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


    /*struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);*/

    //if no file is given, read the stdin input and send it as message to server (receiver)
    if (filename == NULL){
            char line[512];
            
            while(fgets(line, 512, stdin) != NULL){
                send_stdin_message(sock, line, peer_addr);
                //printf("%s\n", line);
                receive_message(sock, peer_addr);
            }
            send_stdin_message(sock, "EOF", peer_addr);
    }else{

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

/*    fd_set inputs;
    FD_ZERO(&inputs);
    FD_SET(fileno(stdin), &inputs);
    FD_SET(sfd, &inputs);
    while (1)
    {
        retval = select(sfd+1, &inputs, NULL, NULL, NULL);
        if (retval == -1){
            perror("select()");
        }
            
        else if (retval){
            if(FD_ISSET(fileno(stdin),&inputs)){
                char* t = fgets(buf,512,stdin);
                if(t == NULL){
                    return;
                }
                packet_t pack;
                pack.type = PTYPE_DATA;
                pack.window = 1;
                pack.length = strlen(buf);
                pack.seqnum = 12;
                pack.timestamp = 1;
                pack.trunc = 0;
                pack.payload = (u_int8_t*) buf;
                int ret;
                int len;
                char* packbuf = encode(&pack,&ret,&len);
                printf("Byte-size of packet is %i\n",len);
                send(sfd,packbuf,len,0);
            }
            if(FD_ISSET(sfd,&inputs)){
                int a = recv(sfd,buf,1024,0);
                *(buf + a) = '\0';
                int b = fwrite(buf,1,strlen(buf),stdout);
                if(b==-1){
                    return;
                }
            }
            FD_ZERO(&inputs);
            FD_SET(fileno(stdin), &inputs);
            FD_SET(sfd, &inputs);
            // fread(buf, sizeof(char),1, stdin);    
        }     
    }
   */