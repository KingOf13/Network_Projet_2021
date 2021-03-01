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
int receive_message(int sock, struct sockaddr_in6  peer_addr){
    char buf[512];
    printf("%s\n", buf);
    socklen_t len_peer = sizeof(struct sockaddr_in6);
    int n = recvfrom(sock, (char *)buf, 512, 0, (struct sockaddr *) &peer_addr, &len_peer); 
    buf[n] = '\0'; 
    printf("Server : %s\n", buf);
    return 0;
}

//receive message from sender (client) and send message back (-> still don't know how to separe them)
int receive_and_send_message(int sock, struct sockaddr_in6 cli_addr){
    char buffer[512]; 
    printf("HEEEEY\n");
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

//send message from stdin input "./sender ::1 12345 < file.txt"
int send_stdin_message(int sock, char* buffer, struct sockaddr_in6 peer_addr){
    sendto(sock, (const char*)buffer, strlen(buffer), 0, (const struct sockaddr *) &peer_addr, sizeof(peer_addr));
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