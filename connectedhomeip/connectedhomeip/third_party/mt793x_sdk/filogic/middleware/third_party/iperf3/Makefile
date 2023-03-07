#	@see http://mtkswits01/~wimac
#	@see libbase/Makefile
#	@see libbase/Makefile.config
PREFIX    = ../install
SCRIPTS   = ../../scripts
NAME      = iperf3
COMPILER  = GNU
CROSS     = arm-none-eabi-
CROSS_SUP = arm-none-eabi- native
CFLAGS    = $(DEF_CFLAGS)
CXXFLAGS  = $(CFLAGS) $(CXXFLAGS_ADD)
CONFIG    =
ifeq ($(shell echo $(CROSS) | grep -e "arm-none-eabi-\|arm-none-elf-\|i586-mingw32msvc-"),)
  LINUX          = 0
  DEP_LIBS       = FreeRTOS lwip
  BACH_BUILD_DEP = FreeRTOS lwip
else
  LINUX = 0
  NEWLIB   = 0
  ifeq ($(strip $(NEWLIB)),1)
      DEP_LIBS       = FreeRTOS lwip
      BACH_BUILD_DEP = FreeRTOS lwip
  else
      DEP_LIBS       = FreeRTOS lwip
      BACH_BUILD_DEP = FreeRTOS lwip
  endif
endif

$(NAME)_PATH = $(shell pwd)

$(NAME)_SRC  = $($(NAME)_PATH)/src/iperf_api.c   \
               $($(NAME)_PATH)/src/iperf_client_api.c   \
               $($(NAME)_PATH)/src/iperf_error.c   \
               $($(NAME)_PATH)/src/iperf_locale.c   \
               $($(NAME)_PATH)/src/iperf_server_api.c   \
               $($(NAME)_PATH)/src/iperf_tcp.c   \
               $($(NAME)_PATH)/src/iperf_udp.c   \
               $($(NAME)_PATH)/src/iperf_util.c   \
               $($(NAME)_PATH)/src/main.c   \
               $($(NAME)_PATH)/src/net.c   \
               $($(NAME)_PATH)/src/tcp_info.c   \
               $($(NAME)_PATH)/src/tcp_window_size.c   \
               $($(NAME)_PATH)/src/timer.c   \
               $($(NAME)_PATH)/src/t_timer.c   \
               $($(NAME)_PATH)/src/t_units.c   \
               $($(NAME)_PATH)/src/t_uuid.c   \
               $($(NAME)_PATH)/src/units.c

			 
CFLAGS    += $(FPUFLAGS) -DPRODUCT_VERSION=$(PRODUCT_VERSION)

# suppress all building warning in this module
CFLAGS    += -w

INIT 	   = syss_init
#LDFLAGS  = $(DEF_LDFLAGS)
LDFLAGS  = $(DEF_LDFLAGS) --specs=nano.specs
LIB      = lib$(NAME).a
ALL      =
CLEAN    = _clean
INSTALL  =
STRICT   = 1	
ROOT     = $(PREFIX)/$(CROSS:%-=%)

#################################################################################

INCLUDE_ADD     += -I$($(NAME)_PATH)/inc
INCLUDE_ADD     += -I../FreeRTOS/Source/include
INCLUDE_ADD     += -I../FreeRTOS/Source

#################################################################################
mbedtls_VER ?= mbedtls-2.0.0
LwIP_VER ?= lwip-master
INCLUDE_ADD      += -I$($(NAME)_PATH)/inc
INCLUDE_ADD      += -I../lwip/src/$(LwIP_VER)/src/include
INCLUDE_ADD      += -I../lwip/src/$(LwIP_VER)/src/include/posix
INCLUDE_ADD      += -I../cjson/include
INCLUDE_ADD      += -I../lwip/include
INCLUDE_ADD      += -I../hal/include
#INCLUDE_ADD      += -I../lwip/src/ports/include

#################################################################################

include $(SCRIPTS)/make/Makefile.template
include $(SCRIPTS)/make/Makefile.config

#################################################################################

$(NAME)_OBJS  = $(patsubst %.c,%.o, $(patsubst %.s,%.o, $($(NAME)_SRC)))

$(LIB): $($(NAME)_OBJS)

_clean:
	echo $($(NAME)_SRC)
	echo $($(NAME)_OBJS)
	rm -f $(LIB) $($(NAME)_OBJS)
