
void bugkiller_tcp_wnd_dump(unsigned long unrecv_len, void *pcb);
void bugkiller_tcp_seg_dump(void *pcb);
void bugkiller_tcp_listen_dump(unsigned long accept_num, void *pcb);
extern char *lwip_bugkiller_format[7];
extern char *lwip_bugkiller_tcp_state[11];
extern char *lwip_bugkiller_tcp_format[4];
extern char *lwip_bugkiller_tcp_listen_format[1];
