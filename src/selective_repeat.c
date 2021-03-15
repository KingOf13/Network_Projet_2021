#include "selective_repeat.h"

int packet_retransmitted = 0;
time_t min_rtt = 100000000;
time_t max_rtt = 0;


window_sender_t* window_sender_init(){
    window_sender_t* window = malloc(sizeof(window_sender_t));
    window->last_ack = 0;
    for (size_t i = 0; i < 32; i++)
    {
        window->window[i] = NULL;
        window->start_time[i] = 0;
    }
    return window;
}

window_receiver_t* init_receiver_window(){
  window_receiver_t* receive_window = malloc(sizeof(window_receiver_t));
  if (receive_window == NULL) return NULL;
  receive_window->next_seqnum = 0;
  receive_window->window_size = 32;
  for(int i=0; i<32; i++) {
    receive_window->window[i] = NULL;
  }
  return receive_window;
}

int set_min_max_rtt(time_t new_time){
    min_rtt = (new_time < min_rtt) ? new_time : min_rtt;
    max_rtt = (new_time > max_rtt) ? new_time : max_rtt;
}

int check_ack(window_sender_t* window, int index, int item_window_nb){
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
        pkt_del(window->window[index%32]);
        set_min_max_rtt(window->start_time[index%32]);
        window->start_time[index%32] = 0;
        window->window[index%32] = NULL;
        index--;
        item_window_nb--;
        //printf("win: %d\n", item_window_nb);
    }
    return item_window_nb;
    
}

int resend_nack(int index, window_sender_t* window, int sock, struct sockaddr_in6 peer_addr){
    window->start_time[index] = clock();
    send_message(sock, window->window[index], peer_addr);
    packet_retransmitted++;
}

int check_timer(window_sender_t* window, int sock, struct sockaddr_in6 peer_addr){
    for (size_t i = 0; i < 32; i++)
    {
        pkt_t* pkt = window->window[i];
        if(pkt == NULL){continue;}
        //printf("%ld, %ld\n", i, clock() - window->start_time[i]);
        if(clock() - window->start_time[i] > 500){
            printf("add timer: %d\n", pkt_get_seqnum(pkt));
            window->start_time[i] = clock();
            send_message(sock, pkt, peer_addr);
            packet_retransmitted++;
        }
    }
    
}
