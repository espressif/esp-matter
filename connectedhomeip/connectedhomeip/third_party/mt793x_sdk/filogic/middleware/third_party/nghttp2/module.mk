
NGHTTP2_SRC = middleware/third_party/nghttp2/lib

			 
C_FILES  += $(NGHTTP2_SRC)/nghttp2_pq.c \
               $(NGHTTP2_SRC)/nghttp2_map.c \
               $(NGHTTP2_SRC)/nghttp2_queue.c \
               $(NGHTTP2_SRC)/nghttp2_frame.c \
               $(NGHTTP2_SRC)/nghttp2_buf.c \
               $(NGHTTP2_SRC)/nghttp2_stream.c \
               $(NGHTTP2_SRC)/nghttp2_outbound_item.c \
               $(NGHTTP2_SRC)/nghttp2_session.c \
               $(NGHTTP2_SRC)/nghttp2_submit.c \
               $(NGHTTP2_SRC)/nghttp2_helper.c \
               $(NGHTTP2_SRC)/nghttp2_npn.c \
               $(NGHTTP2_SRC)/nghttp2_hd.c \
               $(NGHTTP2_SRC)/nghttp2_hd_huffman.c \
               $(NGHTTP2_SRC)/nghttp2_hd_huffman_data.c \
               $(NGHTTP2_SRC)/nghttp2_version.c \
               $(NGHTTP2_SRC)/nghttp2_priority_spec.c \
               $(NGHTTP2_SRC)/nghttp2_option.c \
               $(NGHTTP2_SRC)/nghttp2_callbacks.c \
               $(NGHTTP2_SRC)/nghttp2_mem.c \
               $(NGHTTP2_SRC)/nghttp2_http.c \
	       $(NGHTTP2_SRC)/nghttp2_net.c		 

#################################################################################
#include 
CFLAGS    += $(FPUFLAGS) -DPRODUCT_VERSION=$(PRODUCT_VERSION) -D_U_=""
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes/nghttp2
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include 
