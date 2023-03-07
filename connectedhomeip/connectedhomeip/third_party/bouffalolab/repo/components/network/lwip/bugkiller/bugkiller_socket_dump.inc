#include "lwip_bugkiller_inner.h"

/*
iperf server(recv) dump socket inner,
SocketId: 0     Type: TCP       Status: 192.168.1.116:5001 -> 0.0.0.0:49466 (LISTEN)
SocketId: 1     Type: TCP       Status: 192.168.1.116:5001 -> 192.168.1.143:44312 (ESTABLISHED)
recv wnd     : -1460 ~ 827230504 ~ +33580
recv_ann wnd :       -33580 ~ 827264084
snd wnd      : -0 ~ 6511 ~ +64240
unsend len: 0, unacked len: 0, ooseq len: 0

iperf client(send) dump socket inner,
SocketId: 0     Type: TCP       Status: 192.168.1.116:61738 -> 192.168.1.143:5001 (ESTABLISHED)
recv wnd     : -0 ~ 76303226 ~ +43800
recv_ann wnd :       -43800 ~ 76347026
snd wnd      : -17520 ~ 7173790 ~ +48015
unsend len: 0, unacked len: 17520, ooseq len: 0
*/
extern unsigned long bugkiller_mbox_peek_unrecved(sys_mbox_t recvmbox);
extern unsigned long bugkiller_mbox_peek_accepted(sys_mbox_t acceptbox);
void __attribute__((section(".bugkiller_code"))) bugkiller_socket_inner_dump(int id)
{
    switch(sockets[id].conn->type) {
    case NETCONN_UDP:
	break;
    case NETCONN_TCP:
	if(sockets[id].conn->pcb.tcp->state == LISTEN) {
	    bugkiller_tcp_listen_dump(bugkiller_mbox_peek_accepted(sockets[id].conn->acceptmbox), \
				      sockets[id].conn->pcb.tcp);
	} else {
	    bugkiller_tcp_wnd_dump(bugkiller_mbox_peek_unrecved(sockets[id].conn->recvmbox) + \
				   (sockets[id].lastdata.pbuf ? sockets[id].lastdata.pbuf->tot_len : 0), \
				   sockets[id].conn->pcb.tcp);
	    bugkiller_tcp_seg_dump(sockets[id].conn->pcb.tcp);
	}
	break;
    default:
	printf(lwip_bugkiller_format[4]);
	break;
    }
}

/* sockets_dump():
SocketId: 6          Type: TCP	Status: localhost:33783 -> baidu.com:8000
SocketId: 7          Type: UDP	Status: localhost:33784 -> baidu.com:8001
SocketId: 8          Type: TCP	Status: localhost:8000 (Listen)
....
*/
void __attribute__((section(".bugkiller_code"))) bugkiller_sockets_dump()
{
    for(int i = 0; i < NUM_SOCKETS && sockets[i].conn; i++) {
        if(sockets[i].conn == NULL)
	    continue;
	switch(sockets[i].conn->type) {
	case NETCONN_UDP:
	    printf(lwip_bugkiller_format[0], i);
	    printf(lwip_bugkiller_format[1],
		   ip4addr_ntoa((const ip4_addr_t *) &sockets[i].conn->pcb.udp->local_ip.addr),
		   sockets[i].conn->pcb.udp->local_port);
	    printf(lwip_bugkiller_format[6],
		   ip4addr_ntoa((const ip4_addr_t *) &sockets[i].conn->pcb.udp->remote_ip.addr),
		   sockets[i].conn->pcb.udp->remote_port);
	    break;
	case NETCONN_TCP:
	    printf(lwip_bugkiller_format[3], i);
	    printf(lwip_bugkiller_format[1],
		   ip4addr_ntoa((const ip4_addr_t *) &sockets[i].conn->pcb.udp->local_ip.addr),
		   sockets[i].conn->pcb.tcp->local_port);
	    if(sockets[i].conn->pcb.tcp->state != LISTEN) {
		printf(lwip_bugkiller_format[2],
		       ip4addr_ntoa((const ip4_addr_t *) &sockets[i].conn->pcb.tcp->remote_ip.addr),
		       sockets[i].conn->pcb.tcp->remote_port);
	    }
	    puts(lwip_bugkiller_tcp_state[sockets[i].conn->pcb.tcp->state]);
	    bugkiller_socket_inner_dump(i);
	    break;
	default:
	    printf(lwip_bugkiller_format[5]);
	    break;
	}
    }
}
