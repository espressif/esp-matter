CONTIKI=contiki
TARGET?=minimal-net

all: server

$(CONTIKI):
	git clone --depth 1 https://github.com/contiki-os/contiki.git $@
	cd $(CONTIKI)/apps && ln -s ../../../.. libcoap

server:	$(CONTIKI)
	$(MAKE) -f Makefile.contiki V=1 CONTIKI=$(CONTIKI) TARGET=$(TARGET) NODE_ADDR=0x1000 server
