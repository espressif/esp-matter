# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS +=

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := tcpclient/tcpclient.c \
                tcpserver/tcpserver.c \
                iperf/iperf.c \
                netstat/netstat.c \
                ping/ping.c \


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := tcpclient iperf netstat ping tcpserver


##
#CPPFLAGS += 
