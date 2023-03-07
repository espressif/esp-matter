MDNSD = middleware/third_party/mDNSResponder

#Apple OpenSource mDNSReponder-544
C_FILES  += $(MDNSD)/src/mDNSCore/DNSCommon.c \
            $(MDNSD)/src/mDNSCore/DNSDigest.c \
            $(MDNSD)/src/mDNSCore/mDNS.c \
            $(MDNSD)/src/mDNSCore/uDNS.c \
            $(MDNSD)/src/mDNSCore/anonymous.c \
            $(MDNSD)/src/mDNSCore/CryptoAlg.c \
            $(MDNSD)/src/mDNSPosix/PosixDaemon.c \
            $(MDNSD)/src/mDNSPosix/mDNSPosix.c \
            $(MDNSD)/src/mDNSPosix/mDNSUNP.c \
            $(MDNSD)/src/mDNSShared/dnssd_ipc.c \
            $(MDNSD)/src/mDNSShared/GenLinkedList.c \
            $(MDNSD)/src/mDNSShared/mDNSDebug.c \
            $(MDNSD)/src/mDNSShared/PlatformCommon.c \
            $(MDNSD)/src/mDNSShared/uds_daemon.c \
            $(MDNSD)/src/mDNSShared/dnssd_clientlib.c \
            $(MDNSD)/src/mDNSShared/dnssd_clientstub.c \
            $(MDNSD)/src/Clients/ClientCommon.c \
            $(MDNSD)/src/Clients/bct.c \
            $(MDNSD)/src/Clients/mdns_cli.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS  += -I$(SOURCE_DIR)/$(MDNSD)/inc
CFLAGS  += -I$(SOURCE_DIR)/$(MDNSD)/src/mDNSCore
CFLAGS  += -I$(SOURCE_DIR)/$(MDNSD)/src/mDNSShared
CFLAGS  += -I$(SOURCE_DIR)/$(MDNSD)/src/mDNSPosix
CFLAGS  += -I$(SOURCE_DIR)/$(MDNSD)/src/Clients
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F

ifeq ($(MTK_HOMEKIT_ENABLE),y)

ifeq ($(PRODUCT_VERSION),7697)
CFLAGS += -DMTK_MDNS_STORAGE_IN_TCM
endif

endif

CFLAGS += -DMTK_MDNS_ENABLE
CFLAGS += -DUSE_TCP_LOOPBACK
CFLAGS += -DHAVE_IPV6
# Slim Action: reduce mDNSStorage size (TCM_BSS)
CFLAGS += -DLIMITED_RESOURCES_TARGET
#CFLAGS += -DDEBUG_MDNS
