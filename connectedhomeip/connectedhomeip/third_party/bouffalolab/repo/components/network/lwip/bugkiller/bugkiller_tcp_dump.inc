#include "lwip_bugkiller_inner.h"
/* bugkiller_tcp_wnd_dump()
recv wnd     : -2345 ~ 327584 ~ +5466
recv_ann wnd :          -5466 ~ 330084
snd wnd      : -1245 ~ 133458 ~ +244
*/
void __attribute__((section(".bugkiller_code"))) bugkiller_tcp_wnd_dump(unsigned long unrecv_len, void *s)
{
    struct tcp_pcb *pcb = (struct tcp_pcb *)s;
    /* receiver */
    printf(lwip_bugkiller_tcp_format[0], unrecv_len, pcb->rcv_nxt, pcb->rcv_wnd);

    /* receiver ann */
    printf(lwip_bugkiller_tcp_format[1], pcb->rcv_ann_wnd, pcb->rcv_ann_right_edge);

    /* sender */
    printf(lwip_bugkiller_tcp_format[2], pcb->snd_nxt - pcb->lastack, pcb->snd_nxt, pcb->snd_wnd + pcb->lastack - pcb->snd_nxt);
}

/* bugkiller_tcp_seg_dump()
unsend len: 1000, unakced len: 885
*/
void __attribute__((section(".bugkiller_code"))) bugkiller_tcp_seg_dump(void *s)
{
    struct tcp_pcb *pcb = (struct tcp_pcb *)s;
    unsigned long unsend_len = 0, unacked_len = 0, ooseq_len = 0;
    for(struct tcp_seg *seg = pcb->unsent; seg; seg = seg->next) {
	unsend_len += seg->len;
    }
    for(struct tcp_seg *seg = pcb->unacked; seg; seg = seg->next) {
	unacked_len += seg->len;
    }
#if TCP_QUEUE_OOSEQ
    for(struct tcp_seg *seg = pcb->ooseq; seg; seg = seg->next) {
	ooseq_len += seg->len;
    }
#endif /* TCP_QUEUE_OOSEQ */
    printf(lwip_bugkiller_tcp_format[3], unsend_len, unacked_len, ooseq_len);
}

void __attribute__((section(".bugkiller_code"))) bugkiller_tcp_listen_dump(unsigned long accept_num,void *s)
{
    struct tcp_pcb_listen *listen_pcb = (struct tcp_pcb_listen *)s;
    printf(lwip_bugkiller_tcp_listen_format[0], accept_num);
}
