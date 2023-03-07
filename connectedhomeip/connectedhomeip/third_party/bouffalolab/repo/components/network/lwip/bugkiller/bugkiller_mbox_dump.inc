
unsigned long __attribute__((section(".bugkiller_code"))) bugkiller_mbox_peek_unrecved(sys_mbox_t recvmbox)
{
    unsigned long sum = 0;
    struct pbuf *msg[TCPIP_MBOX_SIZE] = {0};
    int i = 0, j = 0;

    while(sys_arch_mbox_tryfetch(&recvmbox, (void **)&msg[i++]) != SYS_MBOX_EMPTY) {
	sum += msg[i-1]->tot_len;
    }
    while(--i) {
	sys_mbox_trypost(&recvmbox, (void *)msg[j++]);
    }
    return sum;
}
unsigned long __attribute__((section(".bugkiller_code"))) bugkiller_mbox_peek_accepted(sys_mbox_t acceptmbox)
{
    unsigned long sum = 0;
    struct pbuf *msg[TCPIP_MBOX_SIZE] = {0};
    int i = 0, j = 0;

    while(sys_arch_mbox_tryfetch(&acceptmbox, (void **)&msg[i++]) != SYS_MBOX_EMPTY) {
	sum++;
    }
    while(--i) {
	sys_mbox_trypost(&acceptmbox, (void *)msg[j++]);
    }
    return sum;
}
