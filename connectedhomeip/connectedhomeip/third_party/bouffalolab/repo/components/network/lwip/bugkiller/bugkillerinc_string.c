char *lwip_bugkiller_format[]={
			       "SocketId: %d\tType: UDP\t",
			       "Status: %s:%d",
			       " -> %s:%d",
			       "SocketId: %d\tType: TCP\t",
			       " (Listen)\r\n",
			       "type not support\r\n",
			       " -> %s:%d\r\n"
};

char *lwip_bugkiller_tcp_state[]={
				  " (CLOSED)     \r\n",
				  " (LISTEN)     \r\n",
				  " (SYN_SENT)   \r\n",
				  " (SYN_RCVD)   \r\n",
				  " (ESTABLISHED)\r\n",
				  " (FIN_WAIT_1) \r\n",
				  " (FIN_WAIT_2) \r\n",
				  " (CLOSE_WAIT) \r\n",
				  " (CLOSING)    \r\n",
				  " (LAST_ACK)   \r\n",
				  " (TIME_WAIT)  \r\n"
};

char *lwip_bugkiller_tcp_format[]={
				   "recv wnd     : -%lu ~ %lu ~ +%lu\r\r\n",
				   "recv_ann wnd : -%lu ~ %lu\r\r\n",
				   "snd wnd      : -%lu ~ %lu ~ +%lu\r\r\n",
				   "unsend len: %lu, unacked len: %lu, ooseq len: %lu\r\r\n"
};

char *lwip_bugkiller_tcp_listen_format[]={
					  "wait_accept_num: %lu\r\r\n"
};
