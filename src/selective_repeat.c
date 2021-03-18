#include "selective_repeat.h"

int packet_retransmitted = 0;
time_t min_rtt = 100000000;
time_t max_rtt = 0;
time_t last_rtt[15];
int count_rtt;


window_sender_t* window_sender_init(){
    window_sender_t* window = malloc(sizeof(window_sender_t));
    window->last_ack = 0;
    for (size_t i = 0; i < MAX_WINDOW_SIZE; i++)
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
  receive_window->window_val = MAX_WINDOW_SIZE;
  receive_window->window_size = MAX_WINDOW_SIZE;
  for(int i=0; i<MAX_WINDOW_SIZE; i++) {
    receive_window->window[i] = NULL;
  }
  return receive_window;
}

int set_min_max_rtt(time_t new_time){
    time_t actual_new_time = clock() - new_time;
    min_rtt = (actual_new_time < min_rtt) ? actual_new_time : min_rtt;
    max_rtt = (actual_new_time > max_rtt) ? actual_new_time : max_rtt;
}

time_t get_rtt_mean(){
    if(count_rtt < 15){return -1;}
    time_t sum = 0;
    
    for (size_t i = 0; i < 15; i++)
    {
        //printf("first: %ld\n", last_rtt[i]);
        sum += last_rtt[i];
    }
    return sum/15;
    
}

int check_ack(window_sender_t* window, int index, int item_window_nb){
    window->last_ack = index;
    while (1)
    {
        if(index < 0){
            index = 30;
        }
        //printf("index: %d\n", index);
        
        if(window->window[index%MAX_WINDOW_SIZE] == NULL){
            
            break;
        }
        //printf("mod: %d\n", index%32);
        pkt_del(window->window[index%MAX_WINDOW_SIZE]);
        set_min_max_rtt(window->start_time[index%MAX_WINDOW_SIZE]);
        last_rtt[count_rtt%15] = clock() - window->start_time[index%MAX_WINDOW_SIZE];
        count_rtt++;
        window->start_time[index%MAX_WINDOW_SIZE] = 0;
        window->window[index%MAX_WINDOW_SIZE] = NULL;
        index--;
        item_window_nb--;
        //printf("win: %d\n", item_window_nb);
    }
    return item_window_nb;
    
}

int resend_nack(int index, window_sender_t* window, int sock, struct sockaddr_in6 peer_addr){
    //printf("ind: %d\n", index);
    window->start_time[index] = clock();
    send_message(sock, window->window[index], peer_addr);
    packet_retransmitted++;
}

int check_timer(window_sender_t* window, int sock, struct sockaddr_in6 peer_addr){
    for (size_t i = 0; i < MAX_WINDOW_SIZE; i++)
    {
        pkt_t* pkt = window->window[i];
        if(pkt == NULL){continue;}
        //printf("%ld, %ld\n", i, clock() - window->start_time[i]);
        time_t max_time = get_rtt_mean();
        if(max_time == -1 || max_time < 100){max_time = 300;}
        //printf("%ld\n", max_time);
        if(clock() - window->start_time[i] > max_time+0.1*max_time){
            //printf("add timer: %d\n", pkt_get_seqnum(pkt));
            window->start_time[i] = clock();
            send_message(sock, pkt, peer_addr);
            packet_retransmitted++;
        }
    }
    
}
