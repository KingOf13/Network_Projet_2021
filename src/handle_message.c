#include "handle_message.h"
int data_sent = 0;
int data_received = 0;
int data_truncated_received = 0;
int ack_sent = 0;
int ack_received = 0;
int nack_sent = 0;
int nack_received = 0;
int packet_duplicated = 0;
int seqnum_receiver = 1;


//receive message back from server
pkt_t* receive_ack(int sock, struct sockaddr_in6  peer_addr){
    char buf[528];
    pkt_t* pkt = pkt_new();
    socklen_t len_peer = sizeof(struct sockaddr_in6);
    int n = recvfrom(sock, (char *)buf, 528, 0, (struct sockaddr *) &peer_addr, &len_peer); 
    if(n == -1){
        return NULL;
    }
    int status = pkt_decode(buf, 528, pkt);
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
    int decode_status = pkt_decode(buffer, 528, pkt);
    if(decode_status != PKT_OK){
        return 0;
    }
    switch (pkt_get_type(pkt)){
        case PTYPE_DATA:
            data_received++;
            break;
        case PTYPE_ACK:
            ack_received++;
            break;
        case PTYPE_NACK:
            nack_received++;
            break;
        default:
            break;
    }
    //printf("lastseq: %d, %d, %d\n", pkt_get_seqnum(pkt), seqnum_receiver-1, pkt_get_length(pkt));
    if(pkt_get_type(pkt) == PTYPE_DATA && pkt_get_length(pkt) == 0 && pkt_get_seqnum(pkt) == seqnum_receiver-1){
        printf("server shutdown\n");
        return -1;
    }
    pkt_t* pkt_ack = pkt_new();
    //printf("tr: %d\n", pkt_get_length(pkt));
    if(pkt_get_tr(pkt) == 0){
        pkt_set_type(pkt_ack, PTYPE_ACK);
        pkt_set_seqnum(pkt_ack, seqnum_receiver);
    }else{
        pkt_set_type(pkt_ack, PTYPE_NACK);
        pkt_set_seqnum(pkt_ack, pkt_get_seqnum(pkt)%256);
    } 
    seqnum_receiver++;
    pkt_set_window(pkt_ack, 31);
    /*size_t* length = malloc(sizeof(int));
    *length = 1024;
    char* buf_ack = malloc(sizeof(char)*528);
    int encode_status = pkt_encode(pkt_ack, buf_ack, length);
    sendto(sock, (const char *)buf_ack, *length, 0, (const struct sockaddr *) &cli_addr, len);
    free(length);
    free(buf_ack);*/
    send_message(sock, pkt_ack, cli_addr);
    pkt_del(pkt);
    pkt_del(pkt_ack);
    return len;
}

//send message from stdin input "./sender ::1 12345 < file.txt"
int send_message(int sock, pkt_t* pkt, struct sockaddr_in6 peer_addr){
    char* buffer = malloc(sizeof(char)*528);
    size_t* len = malloc(sizeof(int));
    *len = 1024;
    int status = pkt_encode(pkt, buffer, len);
    if(status != PKT_OK){
        free(buffer);
        free(len);
        return -1;
    }
    switch (pkt_get_type(pkt)){
        case PTYPE_DATA:
            data_sent++;
            break;
        case PTYPE_ACK:
            ack_sent++;
            break;
        case PTYPE_NACK:
            nack_sent++;
            break;
        default:
            break;
    }
    sendto(sock, (const char*) buffer, *len, 0, (const struct sockaddr *) &peer_addr, sizeof(peer_addr));
    free(buffer);
    free(len);
    return 0;
}