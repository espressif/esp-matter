
###################################################
# Sources
COAP_SRC = middleware/third_party/libcoap/src

C_FILES  += $(COAP_SRC)/pdu.c           \
            $(COAP_SRC)/net.c           \
            $(COAP_SRC)/debug.c         \
            $(COAP_SRC)/encode.c        \
            $(COAP_SRC)/uri.c           \
            $(COAP_SRC)/coap_list.c     \
            $(COAP_SRC)/resource.c      \
            $(COAP_SRC)/hashkey.c       \
            $(COAP_SRC)/str.c           \
            $(COAP_SRC)/option.c        \
            $(COAP_SRC)/async.c         \
            $(COAP_SRC)/subscribe.c     \
            $(COAP_SRC)/block.c


###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/libcoap/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/netif


