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