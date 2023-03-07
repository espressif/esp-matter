top_srcdir = ../..

WITH_BRANCH=-b STABLE-2_0_0_RC1

all: server

lwip:
	git clone --depth 1 git://git.savannah.nongnu.org/lwip.git $(WITH_BRANCH)

lwip-contrib:
	git clone --depth 1 git://git.savannah.nongnu.org/lwip/lwip-contrib.git $(WITH_BRANCH)

# lwip and coap opts (include early to shadow the lwip-contrib/ports/unix/proj/minimal/ file and any ../../config.h)
CFLAGS += -DWITH_LWIP -iquote.

# lwip library

LWIPOBJS = def.o init.o tapif.o etharp.o netif.o timers.o stats.o mem.o memp.o udp.o tcp.o pbuf.o ip4_addr.o ip4.o inet_chksum.o tcp_in.o tcp_out.o icmp.o raw.o ip_frag.o sys_arch.o ethernet.o ip.o
vpath %.c lwip/src/core/ lwip-contrib/ports/unix/proj/minimal/ lwip/src/netif/ lwip/src/core/ipv4/ lwip-contrib/ports/unix/ lwip-contrib/ports/unix/netif/
CFLAGS += -DLWIP_UNIX_LINUX

# if ipv6 is used
vpath %.c lwip/src/core/ipv6/
LWIPOBJS += mld6.o ip6.o icmp6.o ethip6.o nd6.o ip6_addr.o ip6_frag.o

# coap library

CFLAGS += -std=gnu99

CFLAGS += -I$(top_srcdir)/include/coap

vpath %.c $(top_srcdir)/src

COAPOBJS = net.o debug.o option.o resource.o hashkey.o pdu.o encode.o subscribe.o coap_io_lwip.o block.o uri.o

CFLAGS += -g3 -Wall -Wextra -pedantic -O0 -fpack-struct
# not sorted out yet
CFLAGS += -Wno-unused-parameter

CFLAGS += -Ilwip/src/include/ -Ilwip/src/include/ipv4/ -Ilwip-contrib/ports/unix/include/ -Ilwip-contrib/ports/unix/proj/minimal/

# making an executable out of all of it

OBJS = server.o server-coap.o ${LWIPOBJS} ${COAPOBJS}

../../include/coap/coap.h:
	@echo "Error: coap.h not present. Run teh autotools chain (\`./autogen.sh && ./configure\`) in the project root directory to build the required coap.h file."
	@exit 1
${OBJS}: lwip lwip-contrib ../../include/coap/coap.h

server: ${OBJS}

clean:
	rm -f ${OBJS}

.PHONY: clean
