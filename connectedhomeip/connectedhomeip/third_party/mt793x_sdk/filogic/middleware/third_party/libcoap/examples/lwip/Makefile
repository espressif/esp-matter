all: server

lwip:
	git clone git://git.savannah.nongnu.org/lwip.git -b DEVEL-1_4_1
	curl -L 'https://savannah.nongnu.org/bugs/download.php?file_id=28185' -o lwip/memp.patch
	cd lwip && patch -p1 < memp.patch

lwip-contrib:
	git clone git://git.savannah.nongnu.org/lwip/lwip-contrib.git -b master

# coap library

CFLAGS += -std=gnu99

CFLAGS += -I../..

vpath %.c ../..

COAPOBJS = net.o debug.o option.o resource.o hashkey.o pdu.o encode.o uri.o subscribe.o

# lwip library

LWIPOBJS = def.o init.o mintapif.o etharp.o netif.o timer.o timers.o stats.o mem.o memp.o udp.o tcp.o pbuf.o ip_addr.o ip.o inet_chksum.o tcp_in.o tcp_out.o icmp.o raw.o ip_frag.o sys_arch.o
vpath %.c lwip/src/core/ lwip-contrib/ports/unix/proj/minimal/ lwip/src/netif/ lwip/src/core/ipv4/ lwip-contrib/ports/unix/

CFLAGS += -g -Wall -pedantic -Dopenbsd -DIPv4 -Os -fpack-struct -DLWIP_DEBUG

# lwip opts (include early to shadow the lwip-contrib/ports/unix/proj/minimal/ file)
CFLAGS += -I.

CFLAGS += -Ilwip/src/include/ -Ilwip/src/include/ipv4/ -Ilwip-contrib/ports/unix/include/ -Ilwip-contrib/ports/unix/proj/minimal/

${LWIPOBJS}: lwip lwip-contrib

# making an executable out of all of it

OBJS = server.o server-coap.o ${LWIPOBJS} ${COAPOBJS}

server: ${OBJS}

clean:
	rm -f ${OBJS}

.PHONY: clean
