#include "create_socket.h"



//create socket
int create_socket(){
    int sock = socket(AF_INET6, SOCK_DGRAM, 0); 
    if (sock == -1) {return -1;}
    return sock;
}

//creation of server adress
struct sockaddr_in6 create_address(char* ip, uint16_t port){
    struct sockaddr_in6 peer_addr;             
    memset(&peer_addr, 0, sizeof(peer_addr));        
    peer_addr.sin6_family = AF_INET6;                
    peer_addr.sin6_port = htons(port);                 
    inet_pton(AF_INET6, ip, &peer_addr.sin6_addr); 
    return peer_addr;
}

//receive message back from server
pkt_t* receive_ack(int sock, struct sockaddr_in6  peer_addr){
    char buf[528];
    pkt_t* pkt = pkt_new();
    socklen_t len_peer = sizeof(struct sockaddr_in6);
    int n = recvfrom(sock, (char *)buf, 528, 0, (struct sockaddr *) &peer_addr, &len_peer); 
    if(n == -1){
        return NULL;
    }
    pkt_decode(buf, 528, pkt);
    return pkt;
}

//receive message from sender (client) and send message back (-> still don't know how to separe them)
int receive_and_send_message(int sock, struct sockaddr_in6 cli_addr){
    char buffer[528];
    pkt_t* pkt = pkt_new();
    int len = sizeof(cli_addr);
    int n = recvfrom(sock, (char*) buffer, 528, 0, ( struct sockaddr *) &cli_addr, &len);
    if(n == -1){
        printf("server shutdown\n");
        return -1;
    }
    pkt_decode(buffer, 528, pkt);
    pkt_t* pkt_ack = pkt_new();
    //printf("tr: %d\n", pkt_get_length(pkt));
    if(pkt_get_tr(pkt) == 0){
        pkt_set_type(pkt_ack, PTYPE_ACK);
        pkt_set_seqnum(pkt_ack, (pkt_get_seqnum(pkt)+1)%256);
    }else{
        pkt_set_type(pkt_ack, PTYPE_NACK);
        pkt_set_seqnum(pkt_ack, pkt_get_seqnum(pkt)%256);
        
    } 
    
    pkt_set_window(pkt_ack, 31);
    size_t* length = malloc(sizeof(int));
    *length = 1024;
    char* buf_ack = malloc(sizeof(char)*528);
    pkt_encode(pkt_ack, buf_ack, length);
    sendto(sock, (const char *)buf_ack, *length, 0, (const struct sockaddr *) &cli_addr, len);
    free(length);
    free(buf_ack);
    return len;
}

//send message from stdin input "./sender ::1 12345 < file.txt"
int send_stdin_message(int sock, char* buffer, struct sockaddr_in6 peer_addr, size_t len){
    sendto(sock, (const char*) buffer, len, 0, (const struct sockaddr *) &peer_addr, sizeof(peer_addr));
    return 0;
}

//creation of sender (client) address
struct sockaddr_in6 create_client_address() {
    struct sockaddr_in6 cli_addr;             
    memset(&cli_addr, 0, sizeof(cli_addr)); 
    return cli_addr;
}

//bind the socket to the server address
int bind_server(int sock, struct sockaddr_in6 peer_addr){
    if ( bind(sock, (const struct sockaddr *) &peer_addr, sizeof(peer_addr)) < 0 ){ 
        return -1;
    }
    return 0; 

}

//client (sender) connection to 
int connect_to_server(int sock, struct sockaddr_in6 peer_addr){
    return connect(sock, (struct sockaddr *) &peer_addr, sizeof(peer_addr));
}