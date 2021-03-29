#include "handle_message.h"
#include "log.h"
int data_sent = 0;
int data_received = 0;
int data_truncated_received = 0;
int ack_sent = 0;
int ack_received = 0;
int nack_sent = 0;
int nack_received = 0;
int packet_duplicated = 0;
int seqnum_receiver = 0;
int packet_ignored_by_receiver = 0;
int next_seqnum = 0;

//check if received seqnum is in the valid range
bool is_valid_seqnum(int next_seqnum, int window_size, int pkt_seqnum){
  //printf("%d, %d, %d\n", next_seqnum, window_size, pkt_seqnum);
  if (next_seqnum + window_size > 256){
    if (pkt_seqnum >= next_seqnum || next_seqnum + window_size - 256 > pkt_seqnum) return true;
    else return false;
  }
  else{
    if (pkt_seqnum >= next_seqnum && next_seqnum + window_size > pkt_seqnum) return true;
    else return false;
  }
}

//check if pkt has the expected seqnum, otherwise store it in buffer
bool process_pkt(pkt_t *pkt, window_receiver_t* window_receiver){
  if (is_valid_seqnum(window_receiver->next_seqnum, MAX_WINDOW_SIZE, pkt_get_seqnum(pkt))) {
      data_received++;
    if (pkt_get_seqnum(pkt) == window_receiver->next_seqnum) {
      int place = window_receiver->next_seqnum % MAX_WINDOW_SIZE;
      if(window_receiver->window[place] != NULL){pkt_del(window_receiver->window[place]);}
      window_receiver->window[place] = pkt;
      window_receiver->window_val--;
      while (window_receiver->window[place] != NULL){
        //printf("i: %d\n", place);
        pkt_t* win_pack = window_receiver->window[place];
        //fprintf(stdout, "%s", pkt_get_payload(win_pack));
        //fwrite(pkt_get_seqnum(win_pack), MAX_PAYLOAD_SIZE, 1, stdout);
        fwrite(pkt_get_payload(win_pack), pkt_get_length(win_pack), 1, stdout);
        fflush(stdout);
        pkt_del(win_pack);
        window_receiver->window[place] = NULL;
        window_receiver->window_val++;
        place = (place+1) % MAX_WINDOW_SIZE;
        seqnum_receiver++;
      }
      window_receiver->next_seqnum = seqnum_receiver%256;
      return true;
    }
    else {
      int place = pkt_get_seqnum(pkt) % MAX_WINDOW_SIZE;
      if (window_receiver->window[place] != NULL){
        packet_duplicated++;
        return false;
      } 
      else {
        window_receiver->window[place] = pkt;
        window_receiver->window_val--;
        return true;
      }
    }
  }
  return false;
}

//receive message back from server
pkt_t* receive_ack(int sock, struct sockaddr_in6  peer_addr){
    char buf[528];
    pkt_t* pkt = pkt_new();
    socklen_t len_peer = sizeof(struct sockaddr_in6);
    int n = recvfrom(sock, (char *)buf, 528, 0, (struct sockaddr *) &peer_addr, &len_peer);
    if(n == -1){
        pkt_del(pkt);
        return NULL;
    }
    int status = pkt_decode(buf, 528, pkt);
    if(status != PKT_OK){
      return NULL;
    }
    return pkt;
}



//receive message from sender (client) and send message back
int receive_and_send_message(int sock, struct sockaddr_in6 cli_addr, window_receiver_t* window_receiver){
    char buffer[528];
    pkt_t* pkt = pkt_new();
    socklen_t len = sizeof(cli_addr);
    int n = recvfrom(sock, (char*) buffer, 528, 0, ( struct sockaddr *) &cli_addr, &len);
    if(n == -1){
        return -1;
    }
    int decode_status = pkt_decode(buffer, 528, pkt);
    if(decode_status != PKT_OK){
        packet_ignored_by_receiver++;
        pkt_del(pkt);
        return 0;
    }
    //printf("%d, %d\n", pkt_get_seqnum(pkt), window_receiver->next_seqnum);
    //printf("lastseq: %d, %d, %d\n", pkt_get_seqnum(pkt), window_receiver->next_seqnum-1, pkt_get_length(pkt));
    pkt_t* pkt_ack = pkt_new();
    pkt_set_timestamp(pkt_ack, pkt_get_timestamp(pkt));
    //printf("tr: %d\n", pkt_get_tr(pkt));
    if(pkt_get_tr(pkt) == 0){
      if(pkt_get_type(pkt) == PTYPE_DATA && pkt_get_length(pkt) == 0 && pkt_get_seqnum(pkt) == window_receiver->next_seqnum){
          pkt_set_length(pkt_ack, 0);
          pkt_set_seqnum(pkt_ack, pkt_get_seqnum(pkt));
          send_message(sock, pkt_ack, cli_addr);
          pkt_del(pkt_ack);
          pkt_del(pkt);
          return -1;
      }
      
      if (pkt_get_type(pkt) == PTYPE_DATA){
        if(process_pkt(pkt, window_receiver) == false){
            pkt_del(pkt);
        }
        pkt_set_type(pkt_ack, PTYPE_ACK);
        ERROR("%d\n", window_receiver->next_seqnum);
        pkt_set_seqnum(pkt_ack, window_receiver->next_seqnum);
        }else{
            packet_ignored_by_receiver++;
        }
    }else{
      if(pkt_get_seqnum(pkt) < seqnum_receiver-1){
          pkt_del(pkt_ack);
          pkt_del(pkt);
          packet_ignored_by_receiver++;
          return 0;
      }
      pkt_set_type(pkt_ack, PTYPE_NACK);
      pkt_set_seqnum(pkt_ack, pkt_get_seqnum(pkt)%256);
      pkt_del(pkt);
      data_truncated_received++;
    }
    pkt_set_window(pkt_ack, (window_receiver->window_val)-1);
    send_message(sock, pkt_ack, cli_addr);
    pkt_del(pkt_ack);
    return len;
}

//send message
int send_message(int sock, pkt_t* pkt, struct sockaddr_in6 peer_addr){
    char buffer[528];
    size_t* len = malloc(sizeof(size_t));
    *len = 1024;
    //printf("send: %d\n", pkt_get_seqnum(pkt));
    int status = pkt_encode(pkt, buffer, len);
    
    if(status != PKT_OK){
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
    free(len);
    return 0;
}
