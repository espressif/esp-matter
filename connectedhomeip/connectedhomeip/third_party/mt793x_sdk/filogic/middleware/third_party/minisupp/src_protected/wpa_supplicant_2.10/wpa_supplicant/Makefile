#====== Add by Michael ======
SOURCE_BUILD_DIR = $(shell pwd)/../../../../../../
SOURCE_DIR = $(SOURCE_BUILD_DIR)

CC ?= arm-none-eabi-gcc
#CXX = arm-none-eabi-g++
#AR = arm-none-eabi-ar
LD ?= arm-none-eabi-ld
#OBJCOPY = arm-none-eabi-objcopy
#OBJDUMP = arm-none-eabi-objdump

#CFLAGS = -mthumb -mcpu=cortex-m4 -gdwarf-2 -Os -fno-builtin -ffunction-sections -fdata-sections -Wall -Werror -MMD
#CXXFLAGS = -mthumb -mcpu=cortex-m4 -gdwarf-2 -Os -fno-builtin -ffunction-sections -fdata-sections -Wall -Werror -MMD
#LDFLAGS = --gc-sections -Map ${@:.elf=.map}
LDFLAGS ?= -Wall -mlittle-endian -mthumb -mcpu=cortex-m33 --specs=nosys.specs -lc -lnosys -nostartfiles -Wl,-Map=$@.map
#BUILDDATE = ${shell date}
#======================

ifndef CC
CC=$(CROSS)gcc
endif

ifndef LD
LD=$(CROSS)ld
endif

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld

BINALL=wpa_supplicant
#rtos porting
#BINALL=wpa_supplicant wpa_cli
#ifndef CONFIG_NO_WPA_PASSPHRASE
#BINALL += wpa_passphrase
#endif

ifndef CFLAGS
CFLAGS = -MMD -O2 -Wall -g
endif

ALL = $(BINALL)
#ALL += systemd/wpa_supplicant.service
#ALL += systemd/wpa_supplicant@.service
#ALL += systemd/wpa_supplicant-nl80211@.service
#ALL += systemd/wpa_supplicant-wired@.service
#ALL += dbus/fi.epitest.hostap.WPASupplicant.service
#ALL += dbus/fi.w1.wpa_supplicant1.service
ifdef CONFIG_BUILD_WPA_CLIENT_SO
#ALL += libwpa_client.so
endif

BUILD_DIR ?= ..

CFLAGS += -I$(DIR)/src/utils # -DANDROID_P2P  <== Disable P2P for testing ... Michael
CFLAGS += -I$(DIR)/src/drivers # -DANDROID_P2P  <== Disable P2P for testing ... Michael

#COM_CFLAGS += -DMTK_MINISUPP_ENABLE

export LIBBUILD_DIR ?= /usr/local/lib/
export BINBUILD_DIR ?= /usr/local/sbin/

EXTRA_TARGETS=dynamic_eap_methods

#CONFIG_FILE=.config
#include ../src/build.rules

#ifdef LIBS
# If LIBS is set with some global build system defaults, clone those for
# LIBS_c and LIBS_p to cover wpa_passphrase and wpa_cli as well.
#ifndef LIBS_c
#LIBS_c := $(LIBS)
#endif
#ifndef LIBS_p
#LIBS_p := $(LIBS)
#endif
#endif

export LIBDIR ?= /usr/local/lib
export INCDIR ?= /usr/local/include
export BINDIR ?= /usr/local/sbin
PKG_CONFIG ?= pkg-config

#CFLAGS += $(EXTRA_CFLAGS)
#CFLAGS += -I$(abspath ../src)
#CFLAGS += -I$(abspath ../src/utils)

#ifndef CONFIG_NO_GITVER
## Add VERSION_STR postfix for builds from a git repository
#ifeq ($(wildcard ../.git),../.git)
#GITVER := $(shell git describe --dirty=+)
#ifneq ($(GITVER),)
#CFLAGS += -DGIT_VERSION_STR_POSTFIX=\"-$(GITVER)\"
#endif
#endif
#endif

#ifdef CONFIG_TESTING_OPTIONS
#CFLAGS += -DCONFIG_TESTING_OPTIONS
#CONFIG_WPS_TESTING=y
#CONFIG_TDLS_TESTING=y
#endif

CFLAGS += -I$(DIR)/src
ifeq ($(BUILD_BACH),)
CFLAGS += -I$(SOURCE_BUILD_DIR)/middleware/third_party/lwip/src/lwip-master/src/include
CFLAGS += -I$(SOURCE_BUILD_DIR)/middleware/src/lwip-master/src/include/ipv4
CFLAGS += -I$(DIR)/src/lwip-master/ports/include
endif

CFLAGS += -I$(SOURCE_BUILD_DIR)/middleware/third_party/mbedtls/include
CFLAGS += -I$(SOURCE_BUILD_DIR)/middleware/MTK/connectivity/wlan/os/freertos/include

# Determine use feature.mk , feature_wps.mk or other mk file input in compile command
ifeq ($(FEATURE),)
FEATURE  = feature.mk
endif
include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_BUILD_DIR)/.config
# IC Config
-include $(SOURCE_BUILD_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_BUILD_DIR)/config/board/$(BOARD_CONFIG)/board.mk
CFLAGS += -I$(PROJ_PATH)/../inc


-include $(DIR)/wpa_supplicant/.config
#
# This should not be submitted to server but used as a way to create
# customized code for development and test purposes.
#
-include $(BUILD_DIR)/../../custom.config


#rtos_porting
#install: $(addprefix $(DESTDIR)$(BINDIR)/,$(BINALL))
#	$(MAKE) -C ../src install
#ifdef CONFIG_BUILD_WPA_CLIENT_SO
#	install -m 0644 -D libwpa_client.so $(DESTDIR)/$(LIBDIR)/libwpa_client.so
#	install -m 0644 -D ../src/common/wpa_ctrl.h $(DESTDIR)/$(INCDIR)/wpa_ctrl.h
#endif
#	if ls eap_*.so >/dev/null 2>&1; then \
#		install -d $(DESTDIR)$(LIBDIR)/wpa_supplicant && \
#		cp *.so $(DESTDIR)$(LIBDIR)/wpa_supplicant \
#	; fi

ifdef CONFIG_FIPS
CONFIG_NO_RANDOM_POOL=
CONFIG_OPENSSL_CMAC=y
endif

#rtos_porting
OBJS = $(BUILD_DIR)/wpa_supplicant/config.o
OBJS += $(BUILD_DIR)/wpa_supplicant/notify.o
OBJS += $(BUILD_DIR)/wpa_supplicant/bss.o
OBJS += $(BUILD_DIR)/wpa_supplicant/eap_register.o
OBJS += $(BUILD_DIR)/src/utils/common.o
OBJS += $(BUILD_DIR)/src/utils/config.o
OBJS += $(BUILD_DIR)/src/utils/wpa_debug.o
OBJS += $(BUILD_DIR)/src/utils/wpabuf.o
OBJS += $(BUILD_DIR)/src/utils/bitfield.o
OBJS += $(BUILD_DIR)/src/utils/ip_addr.o
OBJS += $(BUILD_DIR)/src/utils/crc32.o
OBJS += $(BUILD_DIR)/wpa_supplicant/op_classes.o
OBJS += $(BUILD_DIR)/wpa_supplicant/rrm.o
OBJS += $(BUILD_DIR)/wpa_supplicant/twt.o
ifdef CFG_SUP_ROBUSTAV
OBJS += $(BUILD_DIR)/wpa_supplicant/robust_av.o
endif
OBJS_p = $(BUILD_DIR)/wpa_supplicant/wpa_passphrase.o
OBJS_p += $(BUILD_DIR)/src/utils/common.o
OBJS_p += $(BUILD_DIR)/src/utils/wpa_debug.o
OBJS_p += $(BUILD_DIR)/src/utils/wpabuf.o
OBJS_c = $(BUILD_DIR)/wpa_supplicant/wpa_cli.o $(BUILD_DIR)/src/common/wpa_ctrl.o
OBJS_c += $(BUILD_DIR)/src/utils/wpa_debug.o
OBJS_c += $(BUILD_DIR)/src/utils/common.o
OBJS_c += $(BUILD_DIR)/src/common/cli.o
OBJS += $(BUILD_DIR)/wpa_supplicant/wmm_ac.o

ifndef CONFIG_OS
ifdef CONFIG_NATIVE_WINDOWS
CONFIG_OS=win32
else
CONFIG_OS=unix
endif
endif

ifeq ($(CONFIG_OS), freertos)
CFLAGS += -DCONFIG_OS_FRERRTOS
endif

ifeq ($(CONFIG_OS), internal)
CFLAGS += -DOS_NO_C_LIB_DEFINES
endif

OBJS +=  $(BUILD_DIR)/src/utils/os_$(CONFIG_OS).o
OBJS_p +=  $(BUILD_DIR)/src/utils/os_$(CONFIG_OS).o
OBJS_c +=  $(BUILD_DIR)/src/utils/os_$(CONFIG_OS).o

ifdef CONFIG_WPA_TRACE
CFLAGS += -DWPA_TRACE
OBJS += $(BUILD_DIR)/src/utils/trace.o
OBJS_p += $(BUILD_DIR)/src/utils/trace.o
OBJS_c += $(BUILD_DIR)/src/utils/trace.o
OBJS_priv += $(BUILD_DIR)/src/utils/trace.o
LIBCTRL += $(BUILD_DIR)/src/utils/trace.o
LIBCTRLSO += $(BUILD_DIR)/src/utils/trace.c
LDFLAGS += -rdynamic
CFLAGS += -funwind-tables
ifdef CONFIG_WPA_TRACE_BFD
CFLAGS += -DPACKAGE="wpa_supplicant" -DWPA_TRACE_BFD
LIBS += -lbfd -ldl -liberty -lz
LIBS_p += -lbfd -ldl -liberty -lz
LIBS_c += -lbfd -ldl -liberty -lz
endif
endif

ifeq ($(BUILD_BACH),)
all: verify_config $(ALL) dynamic_eap_methods
else
all: wpa_supplicant
endif

verify_config:
	@if [ ! -r  $(BUILD_DIR)/wpa_supplicant/.config ]; then \
		echo 'Building wpa_supplicant requires a configuration file'; \
		echo '(.config). See README for more instructions. You can'; \
		echo 'run "cp defconfig .config" to create an example'; \
		echo 'configuration.'; \
		exit 1; \
	fi

mkconfig:
	@if [ -f  $(BUILD_DIR)/wpa_supplicant/.config ]; then \
		echo '.config exists - did not replace it'; \
		exit 1; \
	fi
	echo CONFIG_DRIVER_HOSTAP=y >>  $(BUILD_DIR)/wpa_supplicant/.config
	echo CONFIG_DRIVER_WEXT=y >>  $(BUILD_DIR)/wpa_supplicant/.config

#$(DESTDIR)$(BINDIR)/%: %
$(DESTBUILD_DIR)$(BINDIR)/%: %
	install -D $(<) $(@)

ifndef CONFIG_ELOOP
CONFIG_ELOOP=eloop
endif
OBJS += $(BUILD_DIR)/src/utils/$(CONFIG_ELOOP).o
OBJS_c += $(BUILD_DIR)/src/utils/$(CONFIG_ELOOP).o

ifndef CONFIG_OSX
ifeq ($(CONFIG_ELOOP), eloop)
# Using glibc < 2.17 requires -lrt for clock_gettime()
# OS X has an alternate implementation
LIBS += -lrt
LIBS_c += -lrt
LIBS_p += -lrt
endif
endif

ifdef CONFIG_ELOOP_POLL
CFLAGS += -DCONFIG_ELOOP_POLL
endif

ifdef CONFIG_ELOOP_EPOLL
CFLAGS += -DCONFIG_ELOOP_EPOLL
endif

ifdef CONFIG_ELOOP_KQUEUE
CFLAGS += -DCONFIG_ELOOP_KQUEUE
endif

ifdef CONFIG_EAPOL_TEST
CFLAGS += -Werror -DEAPOL_TEST
endif

ifdef CONFIG_CODE_COVERAGE
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
LIBS += -lgcov
LIBS_c += -lgcov
LIBS_p += -lgcov
endif

ifeq ($(CONFIG_DISABLE_SYSTEM_LOG), y)
CFLAGS += -DCONFIG_DISABLE_SYSTEM_LOG
endif

ifdef CONFIG_HT_OVERRIDES
CFLAGS += -DCONFIG_HT_OVERRIDES
endif

ifdef CONFIG_VHT_OVERRIDES
CFLAGS += -DCONFIG_VHT_OVERRIDES
endif

ifdef CONFIG_HE_OVERRIDES
CFLAGS += -DCONFIG_HE_OVERRIDES
endif

ifndef CONFIG_BACKEND
CONFIG_BACKEND=file
endif

ifeq ($(CONFIG_BACKEND), file)
ifeq ($(BUILD_BACH),)
OBJS += config_file.o
endif
ifndef CONFIG_NO_CONFIG_BLOBS
NEED_BASE64=y
endif
CFLAGS += -DCONFIG_BACKEND_FILE
endif

ifeq ($(CONFIG_BACKEND), winreg)
OBJS += config_winreg.o
endif

ifeq ($(CONFIG_BACKEND), none)
OBJS += $(BUILD_DIR)/wpa_supplicant/config_none.o
endif

ifeq ($(CONFIG_BACKEND), nvram)
OBJS += $(BUILD_DIR)/wpa_supplicant/config_nvram.o
endif

ifdef CONFIG_NO_CONFIG_WRITE
CFLAGS += -DCONFIG_NO_CONFIG_WRITE
endif

ifdef CONFIG_NO_CONFIG_BLOBS
CFLAGS += -DCONFIG_NO_CONFIG_BLOBS
endif

ifdef CONFIG_NO_SCAN_PROCESSING
CFLAGS += -DCONFIG_NO_SCAN_PROCESSING
endif

ifdef CONFIG_SUITEB
CFLAGS += -DCONFIG_SUITEB
endif

ifdef CONFIG_SUITEB192
CFLAGS += -DCONFIG_SUITEB192
NEED_SHA384=y
endif

ifdef CONFIG_OCV
CFLAGS += -DCONFIG_OCV
OBJS += $(BUILD_DIR)/src/common/ocv.o
endif

ifdef CONFIG_IEEE80211R
CFLAGS += -DCONFIG_IEEE80211R
OBJS += $(BUILD_DIR)/src/rsn_supp/wpa_ft.o
endif

ifdef CONFIG_MESH
NEED_80211_COMMON=y
NEED_AES_SIV=y
CONFIG_SAE=y
CONFIG_AP=y
CFLAGS += -DCONFIG_MESH
OBJS += mesh.o
OBJS += mesh_mpm.o
OBJS += mesh_rsn.o
endif

ifdef CONFIG_SAE
CFLAGS += -DCONFIG_SAE
OBJS += $(BUILD_DIR)/src/common/sae.o
ifdef CONFIG_SAE_PK
CFLAGS += -DCONFIG_SAE_PK
OBJS += $(BUILD_DIR)/src/common/sae_pk.o
endif
NEED_ECC=y
NEED_DH_GROUPS=y
NEED_HMAC_SHA256_KDF=y
NEED_DRAGONFLY=y
ifdef CONFIG_TESTING_OPTIONS
NEED_DH_GROUPS_ALL=y
endif
endif

ifdef CONFIG_DPP
CFLAGS += -DCONFIG_DPP
OBJS += $(BUILD_DIR)/src/common/dpp.o
OBJS += $(BUILD_DIR)/src/common/dpp_auth.o
OBJS += $(BUILD_DIR)/src/common/dpp_backup.o
OBJS += $(BUILD_DIR)/src/common/dpp_crypto.o
OBJS += $(BUILD_DIR)/src/common/dpp_pkex.o
OBJS += $(BUILD_DIR)/src/common/dpp_reconfig.o
OBJS += $(BUILD_DIR)/src/common/dpp_tcp.o
OBJS += dpp_supplicant.o
NEED_AES_SIV=y
NEED_HMAC_SHA256_KDF=y
NEED_HMAC_SHA384_KDF=y
NEED_HMAC_SHA512_KDF=y
NEED_SHA384=y
NEED_SHA512=y
NEED_ECC=y
NEED_JSON=y
NEED_GAS_SERVER=y
NEED_BASE64=y
NEED_ASN1=y
ifdef CONFIG_DPP2
CFLAGS += -DCONFIG_DPP2
endif
endif

ifdef CONFIG_OWE
CFLAGS += -DCONFIG_OWE
NEED_ECC=y
NEED_HMAC_SHA256_KDF=y
NEED_HMAC_SHA384_KDF=y
NEED_HMAC_SHA512_KDF=y
NEED_SHA384=y
NEED_SHA512=y
endif

ifdef CONFIG_FILS
CFLAGS += -DCONFIG_FILS
NEED_SHA384=y
NEED_AES_SIV=y
ifdef CONFIG_FILS_SK_PFS
CFLAGS += -DCONFIG_FILS_SK_PFS
NEED_ECC=y
endif
endif

ifdef CONFIG_MBO
CONFIG_WNM=y
endif

ifdef CONFIG_WNM
CFLAGS += -DCONFIG_WNM
OBJS += $(BUILD_DIR)/wpa_supplicant/wnm_sta.o
endif

ifdef CONFIG_TDLS
CFLAGS += -DCONFIG_TDLS
OBJS += $(BUILD_DIR)/src/rsn_supp/tdls.o
endif

ifdef CONFIG_TDLS_TESTING
CFLAGS += -DCONFIG_TDLS_TESTING
endif

ifdef CONFIG_PMKSA_CACHE_EXTERNAL
CFLAGS += -DCONFIG_PMKSA_CACHE_EXTERNAL
endif

ifndef CONFIG_NO_WPA
OBJS += $(BUILD_DIR)/src/rsn_supp/wpa.o
OBJS += $(BUILD_DIR)/src/rsn_supp/preauth.o
OBJS += $(BUILD_DIR)/src/rsn_supp/pmksa_cache.o
OBJS += $(BUILD_DIR)/src/rsn_supp/wpa_ie.o
OBJS += $(BUILD_DIR)/src/common/wpa_common.o
NEED_AES=y
NEED_SHA1=y
NEED_MD5=y
NEED_RC4=y
else
CFLAGS += -DCONFIG_NO_WPA
ifeq ($(CONFIG_TLS), internal)
NEED_SHA1=y
NEED_MD5=y
endif
endif

ifdef CONFIG_IBSS_RSN
NEED_RSN_AUTHENTICATOR=y
CFLAGS += -DCONFIG_IBSS_RSN
CFLAGS += -DCONFIG_NO_VLAN
OBJS += ibss_rsn.o
endif

ifdef CONFIG_MATCH_IFACE
CFLAGS += -DCONFIG_MATCH_IFACE
endif

ifdef CONFIG_P2P
OBJS += p2p_supplicant.o
OBJS += p2p_supplicant_sd.o
OBJS += $(BUILD_DIR)/src/p2p/p2p.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_utils.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_parse.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_build.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_go_neg.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_sd.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_pd.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_invitation.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_dev_disc.o
OBJS += $(BUILD_DIR)/src/p2p/p2p_group.o
OBJS += $(BUILD_DIR)/src/ap/p2p_hostapd.o
CFLAGS += -DCONFIG_P2P
NEED_GAS=y
NEED_OFFCHANNEL=y
CONFIG_WPS=y
CONFIG_AP=y
ifdef CONFIG_P2P_STRICT
CFLAGS += -DCONFIG_P2P_STRICT
endif
ifdef CONFIG_WIFI_DISPLAY
CFLAGS += -DCONFIG_WIFI_DISPLAY
OBJS += wifi_display.o
endif
endif

ifdef CONFIG_PASN
CFLAGS += -DCONFIG_PASN
CFLAGS += -DCONFIG_PTKSA_CACHE
NEED_HMAC_SHA256_KDF=y
NEED_HMAC_SHA384_KDF=y
NEED_SHA256=y
NEED_SHA384=y
OBJS += $(BUILD_DIR)/src/common/ptksa_cache.o
OBJS += pasn_supplicant.o
endif

ifdef CONFIG_HS20
OBJS += hs20_supplicant.o
CFLAGS += -DCONFIG_HS20
CONFIG_INTERWORKING=y
endif

ifdef CONFIG_INTERWORKING
OBJS += interworking.o
CFLAGS += -DCONFIG_INTERWORKING
NEED_GAS=y
endif

ifdef CONFIG_NO_ROAMING
CFLAGS += -DCONFIG_NO_ROAMING
endif

include $(DIR)/src/drivers/drivers.mak
ifdef CONFIG_AP
OBJS_d += $(DRV_BOTH_OBJS)
CFLAGS += $(DRV_BOTH_CFLAGS)
LDFLAGS += $(DRV_BOTH_LDFLAGS)
LIBS += $(DRV_BOTH_LIBS)
else
NEED_AP_MLME=
OBJS_d += $(DRV_WPA_OBJS)
CFLAGS += $(DRV_WPA_CFLAGS)
LDFLAGS += $(DRV_WPA_LDFLAGS)
LIBS += $(DRV_WPA_LIBS)
endif

ifndef CONFIG_L2_PACKET
CONFIG_L2_PACKET=linux
endif

OBJS_l2 += $(BUILD_DIR)/src/l2_packet/l2_packet_$(CONFIG_L2_PACKET).o

ifeq ($(CONFIG_L2_PACKET), pcap)
ifdef CONFIG_WINPCAP
CFLAGS += -DCONFIG_WINPCAP
LIBS += -lwpcap -lpacket
LIBS_w += -lwpcap
else
LIBS += -ldnet -lpcap
endif
endif

ifeq ($(CONFIG_L2_PACKET), winpcap)
LIBS += -lwpcap -lpacket
LIBS_w += -lwpcap
endif

ifeq ($(CONFIG_L2_PACKET), freebsd)
LIBS += -lpcap
endif

ifdef CONFIG_ERP
CFLAGS += -DCONFIG_ERP
NEED_HMAC_SHA256_KDF=y
endif

ifdef CONFIG_EAP_TLS
# EAP-TLS
ifeq ($(CONFIG_EAP_TLS), dyn)
CFLAGS += -DEAP_TLS_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_tls.so
else
CFLAGS += -DEAP_TLS
OBJS += $(BUILD_DIR)/src/eap_peer/eap_tls.o
endif
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_UNAUTH_TLS
# EAP-UNAUTH-TLS
CFLAGS += -DEAP_UNAUTH_TLS
ifndef CONFIG_EAP_TLS
OBJS += $(BUILD_DIR)/src/eap_peer/eap_tls.o
TLS_FUNCS=y
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_PEAP
# EAP-PEAP
SRC_EAP_PEAP = $(BUILD_DIR)/src/eap_peer/eap_peap.c $(BUILD_DIR)/src/eap_common/eap_peap_common.c
ifeq ($(CONFIG_EAP_PEAP), dyn)
CFLAGS += -DEAP_PEAP_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_peap.so
else
CFLAGS += -DEAP_PEAP
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_PEAP))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_peap.o
OBJS += $(BUILD_DIR)/src/eap_common/eap_peap_common.o
endif
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_TTLS
# EAP-TTLS
ifeq ($(CONFIG_EAP_TTLS), dyn)
CFLAGS += -DEAP_TTLS_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_ttls.so
else
CFLAGS += -DEAP_TTLS
OBJS += $(BUILD_DIR)/src/eap_peer/eap_ttls.o
endif
TLS_FUNCS=y
ifndef CONFIG_FIPS
MS_FUNCS=y
CHAP=y
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_MD5
# EAP-MD5
ifeq ($(CONFIG_EAP_MD5), dyn)
CFLAGS += -DEAP_MD5_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_md5.so
else
CFLAGS += -DEAP_MD5
OBJS += $(BUILD_DIR)/src/eap_peer/eap_md5.o
endif
CHAP=y
CONFIG_IEEE8021X_EAPOL=y
endif

# backwards compatibility for old spelling
ifdef CONFIG_MSCHAPV2
ifndef CONFIG_EAP_MSCHAPV2
CONFIG_EAP_MSCHAPV2=y
endif
endif

ifdef CONFIG_EAP_MSCHAPV2
# EAP-MSCHAPv2
SRC_EAP_MSCHAPV2 = $(BUILD_DIR)/src/eap_peer/eap_mschapv2.c $(BUILD_DIR)/src/eap_peer/mschapv2.c
ifeq ($(CONFIG_EAP_MSCHAPV2), dyn)
CFLAGS += -DEAP_MSCHAPv2_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_mschapv2.so
EAPDYN += $(BUILD_DIR)/src/eap_peer/mschapv2.so
else
CFLAGS += -DEAP_MSCHAPv2
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_MSCHAPV2))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_mschapv2.o
OBJS += $(BUILD_DIR)/src/eap_peer/mschapv2.o
endif
MS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_GTC
# EAP-GTC
ifeq ($(CONFIG_EAP_GTC), dyn)
CFLAGS += -DEAP_GTC_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_gtc.so
else
CFLAGS += -DEAP_GTC
OBJS += $(BUILD_DIR)/src/eap_peer/eap_gtc.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_OTP
# EAP-OTP
ifeq ($(CONFIG_EAP_OTP), dyn)
CFLAGS += -DEAP_OTP_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_otp.so
else
CFLAGS += -DEAP_OTP
OBJS += $(BUILD_DIR)/src/eap_peer/eap_otp.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_SIM
# EAP-SIM
ifeq ($(CONFIG_EAP_SIM), dyn)
CFLAGS += -DEAP_SIM_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_sim.so
else
CFLAGS += -DEAP_SIM
OBJS += $(BUILD_DIR)/src/eap_peer/eap_sim.o
endif
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_SIM_COMMON=y
NEED_AES_CBC=y
endif

ifdef CONFIG_EAP_LEAP
# EAP-LEAP
ifeq ($(CONFIG_EAP_LEAP), dyn)
CFLAGS += -DEAP_LEAP_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_leap.so
else
CFLAGS += -DEAP_LEAP
OBJS += $(BUILD_DIR)/src/eap_peer/eap_leap.o
endif
MS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_PSK
# EAP-PSK
SRC_EAP_PSK = $(BUILD_DIR)/src/eap_peer/eap_psk.c $(BUILD_DIR)/src/eap_common/eap_psk_common.c
ifeq ($(CONFIG_EAP_PSK), dyn)
CFLAGS += -DEAP_PSK_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_psk.so
else
CFLAGS += -DEAP_PSK
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_PSK))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_psk.o $(BUILD_DIR)/src/eap_common/eap_psk_common.o
endif
CONFIG_IEEE8021X_EAPOL=y
NEED_AES=y
NEED_AES_ENCBLOCK=y
NEED_AES_EAX=y
endif

ifdef CONFIG_EAP_AKA
# EAP-AKA
ifeq ($(CONFIG_EAP_AKA), dyn)
CFLAGS += -DEAP_AKA_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_aka.so
else
CFLAGS += -DEAP_AKA
OBJS += $(BUILD_DIR)/src/eap_peer/eap_aka.o
endif
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_SIM_COMMON=y
NEED_AES_CBC=y
endif

ifdef CONFIG_EAP_PROXY
CFLAGS += -DCONFIG_EAP_PROXY
OBJS += $(BUILD_DIR)/src/eap_peer/eap_proxy_$(CONFIG_EAP_PROXY).o
include eap_proxy_$(CONFIG_EAP_PROXY).mak
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_AKA_PRIME
# EAP-AKA'
ifeq ($(CONFIG_EAP_AKA_PRIME), dyn)
CFLAGS += -DEAP_AKA_PRIME_DYNAMIC
else
CFLAGS += -DEAP_AKA_PRIME
endif
endif

ifdef CONFIG_EAP_SIM_COMMON
OBJS += $(BUILD_DIR)/src/eap_common/eap_sim_common.o
NEED_AES=y
NEED_FIPS186_2_PRF=y
endif

ifdef CONFIG_EAP_FAST
# EAP-FAST
SRC_EAP_FAST = $(BUILD_DIR)/src/eap_peer/eap_fast.c $(BUILD_DIR)/src/eap_peer/eap_fast_pac.c
SRC_EAP_FAST += $(BUILD_DIR)/src/eap_common/eap_fast_common.c
ifeq ($(CONFIG_EAP_FAST), dyn)
CFLAGS += -DEAP_FAST_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_fast.so
EAPDYN += $(BUILD_DIR)/src/eap_common/eap_fast_common.o
else
CFLAGS += -DEAP_FAST
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_FAST))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_fast.o $(BUILD_DIR)/src/eap_peer/eap_fast_pac.o
OBJS += $(BUILD_DIR)/src/eap_common/eap_fast_common.o
endif
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
NEED_T_PRF=y
endif

ifdef CONFIG_EAP_TEAP
# EAP-TEAP
SRC_EAP_TEAP = $(BUILD_DIR)/src/eap_peer/eap_teap.c $(BUILD_DIR)/src/eap_peer/eap_teap_pac.c
SRC_EAP_TEAP += $(BUILD_DIR)/src/eap_common/eap_teap_common.c
ifeq ($(CONFIG_EAP_TEAP), dyn)
CFLAGS += -DEAP_TEAP_DYNAMIC
EAPDYN += eap_teap.so
else
CFLAGS += -DEAP_TEAP
OBJS += $(patsubst %.c, %.o, $(SRC_EAP_TEAP))
endif
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
NEED_T_PRF=y
NEED_SHA384=y
NEED_TLS_PRF_SHA256=y
NEED_TLS_PRF_SHA384=y
endif

ifdef CONFIG_EAP_PAX
# EAP-PAX
SRC_EAP_PAX = $(BUILD_DIR)/src/eap_peer/eap_pax.c $(BUILD_DIR)/src/eap_common/eap_pax_common.c
ifeq ($(CONFIG_EAP_PAX), dyn)
CFLAGS += -DEAP_PAX_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_pax.so
else
CFLAGS += -DEAP_PAX
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_PAX))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_pax.o $(BUILD_DIR)/src/eap_common/eap_pax_common.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_SAKE
# EAP-SAKE
SRC_EAP_SAKE = $(BUILD_DIR)/src/eap_peer/eap_sake.c $(BUILD_DIR)/src/eap_common/eap_sake_common.c
ifeq ($(CONFIG_EAP_SAKE), dyn)
CFLAGS += -DEAP_SAKE_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_sake.so
else
CFLAGS += -DEAP_SAKE
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_SAKE))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_sake.o $(BUILD_DIR)/src/eap_common/eap_sake_common.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_GPSK
# EAP-GPSK
SRC_EAP_GPSK = $(BUILD_DIR)/src/eap_peer/eap_gpsk.c $(BUILD_DIR)/src/eap_common/eap_gpsk_common.c
ifeq ($(CONFIG_EAP_GPSK), dyn)
CFLAGS += -DEAP_GPSK_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_gpsk.so
else
CFLAGS += -DEAP_GPSK
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_GPSK))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_gpsk.o $(BUILD_DIR)/src/eap_common/eap_gpsk_common.o
endif
CONFIG_IEEE8021X_EAPOL=y
ifdef CONFIG_EAP_GPSK_SHA256
CFLAGS += -DEAP_GPSK_SHA256
endif
endif

ifdef CONFIG_EAP_PWD
CFLAGS += -DEAP_PWD
ifeq ($(CONFIG_TLS), wolfssl)
CFLAGS += -DCONFIG_ECC
endif
OBJS += $(BUILD_DIR)/src/eap_peer/eap_pwd.o $(BUILD_DIR)/src/eap_common/eap_pwd_common.o
CONFIG_IEEE8021X_EAPOL=y
NEED_ECC=y
NEED_DRAGONFLY=y
endif

ifdef CONFIG_EAP_EKE
# EAP-EKE
SRC_EAP_EKE = $(BUILD_DIR)/src/eap_peer/eap_eke.c $(BUILD_DIR)/src/eap_common/eap_eke_common.c
ifeq ($(CONFIG_EAP_EKE), dyn)
CFLAGS += -DEAP_EKE_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_eke.so
else
CFLAGS += -DEAP_EKE
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_EKE))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_eke.o $(BUILD_DIR)/src/eap_common/eap_eke_common.o
endif
CONFIG_IEEE8021X_EAPOL=y
NEED_DH_GROUPS=y
NEED_DH_GROUPS_ALL=y
NEED_AES_CBC=y
endif

ifdef CONFIG_WPS
# EAP-WSC
CFLAGS += -DCONFIG_WPS -DEAP_WSC
OBJS += $(BUILD_DIR)/wpa_supplicant/wps_supplicant.o
OBJS += $(BUILD_DIR)/src/utils/uuid.o
OBJS += $(BUILD_DIR)/src/eap_peer/eap_wsc.o $(BUILD_DIR)/src/eap_common/eap_wsc_common.o
OBJS += $(BUILD_DIR)/src/wps/wps.o
OBJS += $(BUILD_DIR)/src/wps/wps_common.o
OBJS += $(BUILD_DIR)/src/wps/wps_attr_parse.o
OBJS += $(BUILD_DIR)/src/wps/wps_attr_build.o
OBJS += $(BUILD_DIR)/src/wps/wps_attr_process.o
OBJS += $(BUILD_DIR)/src/wps/wps_dev_attr.o
OBJS += $(BUILD_DIR)/src/wps/wps_enrollee.o
OBJS += $(BUILD_DIR)/src/wps/wps_registrar.o
CONFIG_IEEE8021X_EAPOL=y
NEED_DH_GROUPS=y
NEED_BASE64=y
NEED_AES_CBC=y
NEED_MODEXP=y

ifdef CONFIG_WPS_NFC
CFLAGS += -DCONFIG_WPS_NFC
OBJS += $(BUILD_DIR)/src/wps/ndef.o
NEED_WPS_OOB=y
endif

ifdef NEED_WPS_OOB
CFLAGS += -DCONFIG_WPS_OOB
endif

ifdef CONFIG_WPS_ER
CONFIG_WPS_UPNP=y
CFLAGS += -DCONFIG_WPS_ER
OBJS += $(BUILD_DIR)/src/wps/wps_er.o
OBJS += $(BUILD_DIR)/src/wps/wps_er_ssdp.o
endif

ifdef CONFIG_WPS_UPNP
CFLAGS += -DCONFIG_WPS_UPNP
OBJS += $(BUILD_DIR)/src/wps/wps_upnp.o
OBJS += $(BUILD_DIR)/src/wps/wps_upnp_ssdp.o
OBJS += $(BUILD_DIR)/src/wps/wps_upnp_web.o
OBJS += $(BUILD_DIR)/src/wps/wps_upnp_event.o
OBJS += $(BUILD_DIR)/src/wps/wps_upnp_ap.o
OBJS += $(BUILD_DIR)/src/wps/upnp_xml.o
OBJS += $(BUILD_DIR)/src/wps/httpread.o
OBJS += $(BUILD_DIR)/src/wps/http_client.o
OBJS += $(BUILD_DIR)/src/wps/http_server.o
endif

ifdef CONFIG_WPS_STRICT
CFLAGS += -DCONFIG_WPS_STRICT
OBJS += $(BUILD_DIR)/src/wps/wps_validate.o
endif

ifdef CONFIG_WPS_TESTING
CFLAGS += -DCONFIG_WPS_TESTING
endif

ifdef CONFIG_WPS_REG_DISABLE_OPEN
CFLAGS += -DCONFIG_WPS_REG_DISABLE_OPEN
endif

endif

ifdef CONFIG_EAP_IKEV2
# EAP-IKEv2
SRC_EAP_IKEV2 = $(BUILD_DIR)/src/eap_peer/eap_ikev2.c
SRC_EAP_IKEV2 += $(BUILD_DIR)/src/eap_peer/ikev2.c
SRC_EAP_IKEV2 += $(BUILD_DIR)/src/eap_common/eap_ikev2_common.c
SRC_EAP_IKEV2 += $(BUILD_DIR)/src/eap_common/ikev2_common.c
ifeq ($(CONFIG_EAP_IKEV2), dyn)
CFLAGS += -DEAP_IKEV2_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_ikev2.so $(BUILD_DIR)/src/eap_peer/ikev2.o
EAPDYN += $(BUILD_DIR)/src/eap_common/eap_ikev2_common.o $(BUILD_DIR)/src/eap_common/ikev2_common.o
else
CFLAGS += -DEAP_IKEV2
#OBJS += $(patsubst %.c, %.o, $(SRC_EAP_IKEV2))
OBJS += $(BUILD_DIR)/src/eap_peer/eap_ikev2.o $(BUILD_DIR)/src/eap_peer/ikev2.o
OBJS += $(BUILD_DIR)/src/eap_common/eap_ikev2_common.o $(BUILD_DIR)/src/eap_common/ikev2_common.o
endif
CONFIG_IEEE8021X_EAPOL=y
NEED_DH_GROUPS=y
NEED_DH_GROUPS_ALL=y
NEED_MODEXP=y
NEED_CIPHER=y
endif

ifdef CONFIG_EAP_VENDOR_TEST
ifeq ($(CONFIG_EAP_VENDOR_TEST), dyn)
CFLAGS += -DEAP_VENDOR_TEST_DYNAMIC
EAPDYN += $(BUILD_DIR)/src/eap_peer/eap_vendor_test.so
else
CFLAGS += -DEAP_VENDOR_TEST
OBJS += $(BUILD_DIR)/src/eap_peer/eap_vendor_test.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_TNC
# EAP-TNC
CFLAGS += -DEAP_TNC
OBJS += $(BUILD_DIR)/src/eap_peer/eap_tnc.o
OBJS += $(BUILD_DIR)/src/eap_peer/tncc.o
NEED_BASE64=y
ifndef CONFIG_NATIVE_WINDOWS
ifndef CONFIG_DRIVER_BSD
LIBS += -ldl
endif
endif
endif

ifdef CONFIG_MACSEC
CFLAGS += -DCONFIG_MACSEC
CONFIG_IEEE8021X_EAPOL=y
NEED_AES_ENCBLOCK=y
NEED_AES_UNWRAP=y
NEED_AES_WRAP=y
OBJS += wpas_kay.o
OBJS += $(BUILD_DIR)/src/pae/ieee802_1x_cp.o
OBJS += $(BUILD_DIR)/src/pae/ieee802_1x_kay.o
OBJS += $(BUILD_DIR)/src/pae/ieee802_1x_key.o
OBJS += $(BUILD_DIR)/src/pae/ieee802_1x_secy_ops.o
ifdef CONFIG_AP
OBJS += $(BUILD_DIR)/src/ap/wpa_auth_kay.o
endif
endif

ifdef CONFIG_IEEE8021X_EAPOL
# IEEE 802.1X/EAPOL state machines (e.g., for RADIUS authentication)
CFLAGS += -DIEEE8021X_EAPOL
OBJS += $(BUILD_DIR)/src/eapol_supp/eapol_supp_sm.o
OBJS += $(BUILD_DIR)/src/eap_peer/eap.o
OBJS += $(BUILD_DIR)/src/eap_peer/eap_methods.o
NEED_EAP_COMMON=y
ifdef CONFIG_DYNAMIC_EAP_METHODS
CFLAGS += -DCONFIG_DYNAMIC_EAP_METHODS
LIBS += -ldl -rdynamic
endif
endif

ifdef CONFIG_AP
NEED_EAP_COMMON=y
NEED_RSN_AUTHENTICATOR=y
CFLAGS += -DCONFIG_AP
OBJS += $(BUILD_DIR)/wpa_supplicant/ap.o
CFLAGS += -DCONFIG_NO_RADIUS
CFLAGS += -DCONFIG_NO_ACCOUNTING
CFLAGS += -DCONFIG_NO_VLAN
OBJS += $(BUILD_DIR)/src/ap/hostapd.o
OBJS += $(BUILD_DIR)/src/ap/wpa_auth_glue.o
OBJS += $(BUILD_DIR)/src/ap/utils.o
OBJS += $(BUILD_DIR)/src/ap/authsrv.o
OBJS += $(BUILD_DIR)/src/ap/ap_config.o
OBJS += $(BUILD_DIR)/src/ap/sta_info.o
OBJS += $(BUILD_DIR)/src/ap/tkip_countermeasures.o
OBJS += $(BUILD_DIR)/src/ap/ieee802_1x.o
OBJS += $(BUILD_DIR)/src/eapol_auth/eapol_auth_sm.o
OBJS += $(BUILD_DIR)/src/ap/ieee802_11_auth.o
OBJS += $(BUILD_DIR)/src/ap/ieee802_11_shared.o
OBJS += $(BUILD_DIR)/src/ap/drv_callbacks.o
OBJS += $(BUILD_DIR)/src/ap/ap_drv_ops.o
OBJS += $(BUILD_DIR)/src/ap/beacon.o
OBJS += $(BUILD_DIR)/src/ap/bss_load.o
OBJS += $(BUILD_DIR)/src/ap/eap_user_db.o
OBJS += $(BUILD_DIR)/src/ap/neighbor_db.o
OBJS += $(BUILD_DIR)/src/ap/rrm.o
OBJS += $(BUILD_DIR)/src/ap/ieee802_11_ht.o
ifdef CONFIG_IEEE80211AC
OBJS += $(BUILD_DIR)/src/ap/ieee802_11_vht.o
endif
ifdef CONFIG_IEEE80211AX
OBJS += $(BUILD_DIR)/src/ap/ieee802_11_he.o
endif
ifdef CONFIG_WNM_AP
CFLAGS += -DCONFIG_WNM_AP
OBJS += $(BUILD_DIR)/src/ap/wnm_ap.o
endif
ifdef CONFIG_MBO
ifdef NEED_AP_MLME
OBJS += $(BUILD_DIR)/src/ap/mbo_ap.o
endif
endif
ifdef CONFIG_FILS
OBJS += $(BUILD_DIR)/src/ap/fils_hlp.o
endif
ifdef CONFIG_CTRL_IFACE
OBJS += $(BUILD_DIR)/src/ap/ctrl_iface_ap.o
endif

CFLAGS += -DEAP_SERVER -DEAP_SERVER_IDENTITY
OBJS += $(BUILD_DIR)/src/eap_server/eap_server.o
OBJS += $(BUILD_DIR)/src/eap_server/eap_server_identity.o
OBJS += $(BUILD_DIR)/src/eap_server/eap_server_methods.o

ifdef CONFIG_IEEE80211AC
CFLAGS += -DCONFIG_IEEE80211AC
endif
ifdef CONFIG_IEEE80211AX
CFLAGS += -DCONFIG_IEEE80211AX
endif

ifdef CONFIG_PARTIAL_AP
OBJS += $(BUILD_DIR)/src/ap/wmm.o
OBJS += $(BUILD_DIR)/src/ap/ieee802_11.o
OBJS += $(BUILD_DIR)/src/ap/hw_features.o
CFLAGS += -DCONFIG_PARTIAL_AP
endif

ifdef CONFIG_AP_MLME
OBJS += $(BUILD_DIR)/src/ap/ap_mlme.o
CFLAGS += -DCONFIG_AP_MLME
endif

ifdef NEED_AP_MLME
OBJS += $(BUILD_DIR)/src/ap/wmm.o
OBJS += $(BUILD_DIR)/src/ap/ap_list.o
OBJS += $(BUILD_DIR)/src/ap/ieee802_11.o
OBJS += $(BUILD_DIR)/src/ap/hw_features.o
ifdef CFG_AP_DFS
OBJS += $(BUILD_DIR)/src/ap/dfs.o
endif
CFLAGS += -DNEED_AP_MLME
endif
ifdef CONFIG_WPS
CFLAGS += -DEAP_SERVER_WSC
OBJS += $(BUILD_DIR)/src/ap/wps_hostapd.o
OBJS += $(BUILD_DIR)/src/eap_server/eap_server_wsc.o
endif
ifdef CONFIG_DPP
OBJS += $(BUILD_DIR)/src/ap/dpp_hostapd.o
OBJS += $(BUILD_DIR)/src/ap/gas_query_ap.o
NEED_AP_GAS_SERV=y
endif
ifdef CONFIG_INTERWORKING
NEED_AP_GAS_SERV=y
endif
ifdef NEED_AP_GAS_SERV
OBJS += $(BUILD_DIR)/src/ap/gas_serv.o
endif
ifdef CONFIG_HS20
OBJS += $(BUILD_DIR)/src/ap/hs20.o
endif
endif

ifdef CONFIG_MBO
OBJS += $(BUILD_DIR)/wpa_supplicant/mbo.o
CFLAGS += -DCONFIG_MBO
endif

ifdef NEED_RSN_AUTHENTICATOR
CFLAGS += -DCONFIG_NO_RADIUS
NEED_AES_WRAP=y
OBJS += $(BUILD_DIR)/src/ap/wpa_auth.o
OBJS += $(BUILD_DIR)/src/ap/wpa_auth_ie.o
OBJS += $(BUILD_DIR)/src/ap/pmksa_cache_auth.o
endif

ifdef CONFIG_ACS_OFFLOAD
CFLAGS += -DCONFIG_ACS_OFFLOAD
endif

ifdef CONFIG_ACS
CFLAGS += -DCONFIG_ACS
OBJS += $(BUILD_DIR)/src/ap/acs.o
LIBS += -lm
endif

ifdef CONFIG_PCSC
# PC/SC interface for smartcards (USIM, GSM SIM)
CFLAGS += -DPCSC_FUNCS -I/usr/include/PCSC
OBJS += $(BUILD_DIR)/src/utils/pcsc_funcs.o
# -lpthread may not be needed depending on how pcsc-lite was configured
ifdef CONFIG_NATIVE_WINDOWS
#Once MinGW gets support for WinScard, -lwinscard could be used instead of the
#dynamic symbol loading that is now used in pcsc_funcs.c
#LIBS += -lwinscard
else
#LIBS += $(shell $(PKG_CONFIG) --libs libpcsclite)
LIBS += -lpcsclite -lpthread
endif
endif

ifdef CONFIG_SIM_SIMULATOR
CFLAGS += -DCONFIG_SIM_SIMULATOR
NEED_MILENAGE=y
endif

ifdef CONFIG_USIM_SIMULATOR
CFLAGS += -DCONFIG_USIM_SIMULATOR
NEED_MILENAGE=y
endif

ifdef NEED_MILENAGE
OBJS += $(BUILD_DIR)/src/crypto/milenage.o
NEED_AES_ENCBLOCK=y
endif

ifdef CONFIG_PKCS12
CFLAGS += -DPKCS12_FUNCS
endif

ifdef CONFIG_SMARTCARD
CFLAGS += -DCONFIG_SMARTCARD
endif

ifdef NEED_DRAGONFLY
OBJS += $(BUILD_DIR)/src/common/dragonfly.o
endif

ifdef MS_FUNCS
OBJS += $(BUILD_DIR)/src/crypto/ms_funcs.o
NEED_DES=y
NEED_MD4=y
endif

ifdef CHAP
OBJS += $(BUILD_DIR)/src/eap_common/chap.o
endif

ifdef TLS_FUNCS
NEED_DES=y
# Shared TLS functions (needed for EAP_TLS, EAP_PEAP, EAP_TTLS, EAP_FAST, and
# EAP_TEAP)
OBJS += $(BUILD_DIR)/src/eap_peer/eap_tls_common.o
ifndef CONFIG_FIPS
NEED_TLS_PRF=y
NEED_SHA1=y
NEED_MD5=y
endif
endif

ifndef CONFIG_TLS
CONFIG_TLS=openssl
endif

ifdef CONFIG_TLSV11
CFLAGS += -DCONFIG_TLSV11
endif

ifdef CONFIG_TLSV12
CFLAGS += -DCONFIG_TLSV12
endif

ifeq ($(CONFIG_TLS), wolfssl)
ifdef TLS_FUNCS
CFLAGS += -DWOLFSSL_DER_LOAD
OBJS += $(BUILD_DIR)/src/crypto/tls_wolfssl.o
endif
OBJS += $(BUILD_DIR)/src/crypto/crypto_wolfssl.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_wolfssl.o
ifdef NEED_FIPS186_2_PRF
OBJS += $(BUILD_DIR)/src/crypto/fips_prf_wolfssl.o
endif
NEED_TLS_PRF_SHA256=y
LIBS += -lwolfssl -lm
LIBS_p += -lwolfssl -lm
endif

ifeq ($(CONFIG_TLS), openssl)
ifdef TLS_FUNCS
CFLAGS += -DEAP_TLS_OPENSSL
OBJS += $(BUILD_DIR)/src/crypto/tls_openssl.o
OBJS += $(BUILD_DIR)/src/crypto/tls_openssl_ocsp.o
LIBS += -lssl
endif
OBJS += $(BUILD_DIR)/src/crypto/crypto_openssl.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_openssl.o
OBJS_priv += $(BUILD_DIR)/src/crypto/crypto_openssl.o
ifdef NEED_FIPS186_2_PRF
OBJS += $(BUILD_DIR)/src/crypto/fips_prf_openssl.o
endif
NEED_TLS_PRF_SHA256=y
LIBS += -lcrypto
LIBS_p += -lcrypto
ifdef CONFIG_TLS_ADD_DL
LIBS += -ldl
LIBS_p += -ldl
endif
ifndef CONFIG_TLS_DEFAULT_CIPHERS
CONFIG_TLS_DEFAULT_CIPHERS = "DEFAULT:!EXP:!LOW"
endif
CFLAGS += -DTLS_DEFAULT_CIPHERS=\"$(CONFIG_TLS_DEFAULT_CIPHERS)\"
endif

ifeq ($(CONFIG_TLS), gnutls)
ifndef CONFIG_CRYPTO
# default to libgcrypt
CONFIG_CRYPTO=gnutls
endif
ifdef TLS_FUNCS
OBJS += $(BUILD_DIR)/src/crypto/tls_gnutls.o
LIBS += -lgnutls -lgpg-error
endif
OBJS += $(BUILD_DIR)/src/crypto/crypto_$(CONFIG_CRYPTO).o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_$(CONFIG_CRYPTO).o
OBJS_priv += $(BUILD_DIR)/src/crypto/crypto_$(CONFIG_CRYPTO).o
ifdef NEED_FIPS186_2_PRF
OBJS += $(BUILD_DIR)/src/crypto/fips_prf_internal.o
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1-internal.o
endif
ifeq ($(CONFIG_CRYPTO), gnutls)
LIBS += -lgcrypt
LIBS_p += -lgcrypt
CONFIG_INTERNAL_RC4=y
CONFIG_INTERNAL_DH_GROUP5=y
endif
ifeq ($(CONFIG_CRYPTO), nettle)
LIBS += -lnettle -lgmp
LIBS_p += -lnettle -lgmp
CONFIG_INTERNAL_RC4=y
CONFIG_INTERNAL_DH_GROUP5=y
endif
endif

ifeq ($(CONFIG_TLS), internal)
CFLAGS  += -I$(DIR)/src/crypto/openssl/crypto/bn
CFLAGS  += -I$(DIR)/src/crypto/openssl/crypto/ec
CFLAGS  += -I$(DIR)/src/crypto/openssl/crypto/include
CFLAGS  += -I$(DIR)/src/crypto/openssl/include
include $(DIR)/src/crypto/openssl/ecc.mak
OBJS += $(BUILD_DIR)/src/crypto/crypto_openssl.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_openssl.o
OBJS_priv += $(BUILD_DIR)/src/crypto/crypto_openssl.o
ifndef CONFIG_CRYPTO
CONFIG_CRYPTO=internal
endif
ifdef TLS_FUNCS
OBJS += $(BUILD_DIR)/src/crypto/crypto_internal-rsa.o
OBJS += $(BUILD_DIR)/src/crypto/tls_internal.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_common.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_record.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_cred.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client_write.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client_read.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client_ocsp.o
OBJS += $(BUILD_DIR)/src/tls/rsa.o
OBJS += $(BUILD_DIR)/src/tls/x509v3.o
OBJS += $(BUILD_DIR)/src/tls/pkcs1.o
OBJS += $(BUILD_DIR)/src/tls/pkcs5.o
OBJS += $(BUILD_DIR)/src/tls/pkcs8.o
NEED_ASN1=y
NEED_BASE64=y
NEED_TLS_PRF=y
ifdef CONFIG_TLSV12
NEED_TLS_PRF_SHA256=y
endif
NEED_MODEXP=y
NEED_CIPHER=y
CFLAGS += -DCONFIG_TLS_INTERNAL_CLIENT
endif
ifdef NEED_CIPHER
NEED_DES=y
OBJS += $(BUILD_DIR)/src/crypto/crypto_internal-cipher.o
endif
ifdef NEED_MODEXP
OBJS += $(BUILD_DIR)/src/crypto/crypto_internal-modexp.o
OBJS += $(BUILD_DIR)/src/tls/bignum.o
endif
ifeq ($(CONFIG_CRYPTO), libtomcrypt)
OBJS += $(BUILD_DIR)/src/crypto/crypto_libtomcrypt.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_libtomcrypt.o
LIBS += -ltomcrypt -ltfm
LIBS_p += -ltomcrypt -ltfm
CONFIG_INTERNAL_SHA256=y
CONFIG_INTERNAL_RC4=y
CONFIG_INTERNAL_DH_GROUP5=y
endif
ifeq ($(CONFIG_CRYPTO), internal)
OBJS += $(BUILD_DIR)/src/crypto/crypto_internal.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_internal.o
NEED_AES_ENC=y
CFLAGS += -DCONFIG_CRYPTO_INTERNAL
#OBJS += $(BUILD_DIR)/src/crypto/crypto_mbedtdls.o
#OBJS_p += $(BUILD_DIR)/src/crypto/crypto_mbedtdls.o
ifdef CONFIG_INTERNAL_LIBTOMMATH
CFLAGS += -DCONFIG_INTERNAL_LIBTOMMATH
ifdef CONFIG_INTERNAL_LIBTOMMATH_FAST
CFLAGS += -DLTM_FAST
endif
else
LIBS += -ltommath
LIBS_p += -ltommath
endif
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_DES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD4=y
CONFIG_INTERNAL_MD5=y
CONFIG_INTERNAL_SHA256=y
CONFIG_INTERNAL_SHA384=y
CONFIG_INTERNAL_SHA512=y
CONFIG_INTERNAL_RC4=y
CONFIG_INTERNAL_DH_GROUP5=y
endif
ifeq ($(CONFIG_CRYPTO), cryptoapi)
OBJS += $(BUILD_DIR)/src/crypto/crypto_cryptoapi.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_cryptoapi.o
CFLAGS += -DCONFIG_CRYPTO_CRYPTOAPI
CONFIG_INTERNAL_SHA256=y
CONFIG_INTERNAL_RC4=y
endif
endif

ifeq ($(CONFIG_TLS), linux)
OBJS += $(BUILD_DIR)/src/crypto/crypto_linux.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_linux.o
ifdef TLS_FUNCS
OBJS += $(BUILD_DIR)/src/crypto/crypto_internal-rsa.o
OBJS += $(BUILD_DIR)/src/crypto/tls_internal.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_common.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_record.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_cred.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client_write.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client_read.o
OBJS += $(BUILD_DIR)/src/tls/tlsv1_client_ocsp.o
OBJS += $(BUILD_DIR)/src/tls/rsa.o
OBJS += $(BUILD_DIR)/src/tls/x509v3.o
OBJS += $(BUILD_DIR)/src/tls/pkcs1.o
OBJS += $(BUILD_DIR)/src/tls/pkcs5.o
OBJS += $(BUILD_DIR)/src/tls/pkcs8.o
NEED_ASN1=y
NEED_BASE64=y
NEED_TLS_PRF=y
ifdef CONFIG_TLSV12
NEED_TLS_PRF_SHA256=y
endif
NEED_MODEXP=y
NEED_CIPHER=y
CFLAGS += -DCONFIG_TLS_INTERNAL_CLIENT
endif
ifdef NEED_MODEXP
OBJS += $(BUILD_DIR)/src/crypto/crypto_internal-modexp.o
OBJS += $(BUILD_DIR)/src/tls/bignum.o
CFLAGS += -DCONFIG_INTERNAL_LIBTOMMATH
CFLAGS += -DLTM_FAST
endif
CONFIG_INTERNAL_DH_GROUP5=y
ifdef NEED_FIPS186_2_PRF
OBJS += $(BUILD_DIR)/src/crypto/fips_prf_internal.o
OBJS += $(BUILD_DIR)/src/crypto/sha1-internal.o
endif
endif

ifeq ($(CONFIG_TLS), none)
ifdef TLS_FUNCS
OBJS += $(BUILD_DIR)/src/crypto/tls_none.o
CFLAGS += -DEAP_TLS_NONE
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD5=y
endif
OBJS += $(BUILD_DIR)/src/crypto/crypto_none.o
OBJS_p += $(BUILD_DIR)/src/crypto/crypto_none.o
CONFIG_INTERNAL_SHA256=y
CONFIG_INTERNAL_RC4=y
endif

ifdef TLS_FUNCS
ifdef CONFIG_SMARTCARD
ifndef CONFIG_NATIVE_WINDOWS
ifneq ($(CONFIG_L2_PACKET), freebsd)
LIBS += -ldl
endif
endif
endif
endif

ifndef TLS_FUNCS
OBJS += $(BUILD_DIR)/src/crypto/tls_none.o
ifeq ($(CONFIG_TLS), internal)
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD5=y
CONFIG_INTERNAL_RC4=y
endif
endif

AESOBJS = # none so far (see below)
ifdef CONFIG_INTERNAL_AES
AESOBJS += $(BUILD_DIR)/src/crypto/aes-internal.o $(BUILD_DIR)/src/crypto/aes-internal-dec.o
endif

ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), wolfssl)
NEED_INTERNAL_AES_WRAP=y
endif
endif
ifdef CONFIG_OPENSSL_INTERNAL_AES_WRAP
# Seems to be needed at least with BoringSSL
NEED_INTERNAL_AES_WRAP=y
CFLAGS += -DCONFIG_OPENSSL_INTERNAL_AES_WRAP
endif
ifdef CONFIG_FIPS
# Have to use internal AES key wrap routines to use OpenSSL EVP since the
# OpenSSL AES_wrap_key()/AES_unwrap_key() API is not available in FIPS mode.
NEED_INTERNAL_AES_WRAP=y
endif

ifdef NEED_INTERNAL_AES_WRAP
ifneq ($(CONFIG_TLS), linux)
AESOBJS += $(BUILD_DIR)/src/crypto/aes-unwrap.o
endif
endif
ifdef NEED_AES_EAX
AESOBJS += $(BUILD_DIR)/src/crypto/aes-eax.o
NEED_AES_CTR=y
endif
ifdef NEED_AES_SIV
AESOBJS += $(BUILD_DIR)/src/crypto/aes-siv.o
NEED_AES_CTR=y
endif
ifdef NEED_AES_CTR
AESOBJS += $(BUILD_DIR)/src/crypto/aes-ctr.o
endif
ifdef NEED_AES_ENCBLOCK
AESOBJS += $(BUILD_DIR)/src/crypto/aes-encblock.o
endif
NEED_AES_ENC=y
ifdef CONFIG_OPENSSL_CMAC
CFLAGS += -DCONFIG_OPENSSL_CMAC
else
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), wolfssl)
AESOBJS += $(BUILD_DIR)/src/crypto/aes-omac1.o
endif
endif
endif
ifdef NEED_AES_WRAP
NEED_AES_ENC=y
ifdef NEED_INTERNAL_AES_WRAP
AESOBJS += $(BUILD_DIR)/src/crypto/aes-wrap.o
endif
endif
ifdef NEED_AES_CBC
NEED_AES_ENC=y
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), wolfssl)
AESOBJS += $(BUILD_DIR)/src/crypto/aes-cbc.o
endif
endif
endif
endif
ifdef NEED_AES_ENC
ifdef CONFIG_INTERNAL_AES
AESOBJS += $(BUILD_DIR)/src/crypto/aes-internal-enc.o
endif
endif
ifdef NEED_AES
OBJS += $(AESOBJS)
endif

ifdef NEED_SHA1
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1.o
endif
endif
endif
endif
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1-prf.o
ifdef CONFIG_INTERNAL_SHA1
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1-internal.o
ifdef NEED_FIPS186_2_PRF
SHA1OBJS += $(BUILD_DIR)/src/crypto/fips_prf_internal.o
endif
endif
ifdef CONFIG_NO_WPA_PASSPHRASE
CFLAGS += -DCONFIG_NO_PBKDF2
else
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), wolfssl)
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1-pbkdf2.o
endif
endif
endif
ifdef NEED_T_PRF
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1-tprf.o
endif
ifdef NEED_TLS_PRF
SHA1OBJS += $(BUILD_DIR)/src/crypto/sha1-tlsprf.o
endif
endif

ifndef CONFIG_FIPS
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
MD5OBJS += $(BUILD_DIR)/src/crypto/md5.o
endif
endif
endif
endif
endif
ifdef NEED_MD5
ifdef CONFIG_INTERNAL_MD5
MD5OBJS += $(BUILD_DIR)/src/crypto/md5-internal.o
endif
OBJS += $(MD5OBJS)
OBJS_p += $(MD5OBJS)
OBJS_priv += $(MD5OBJS)
endif

ifdef NEED_MD4
ifdef CONFIG_INTERNAL_MD4
OBJS += $(BUILD_DIR)/src/crypto/md4-internal.o
endif
endif

DESOBJS = # none needed when not internal
ifdef NEED_DES
ifndef CONFIG_FIPS
CFLAGS += -DCONFIG_DES
endif
ifdef CONFIG_INTERNAL_DES
DESOBJS += $(BUILD_DIR)/src/crypto/des-internal.o
endif
endif

ifdef CONFIG_NO_RC4
CFLAGS += -DCONFIG_NO_RC4
endif

ifdef NEED_RC4
ifdef CONFIG_INTERNAL_RC4
ifndef CONFIG_NO_RC4
OBJS += $(BUILD_DIR)/src/crypto/rc4.o
endif
endif
endif

SHA256OBJS = # none by default
CFLAGS += -DCONFIG_SHA256
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha256.o
endif
endif
endif
endif
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha256-prf.o
ifdef CONFIG_INTERNAL_SHA256
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha256-internal.o
endif
ifdef CONFIG_INTERNAL_SHA384
CFLAGS += -DCONFIG_INTERNAL_SHA384
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha384-internal.o
endif
ifdef CONFIG_INTERNAL_SHA512
CFLAGS += -DCONFIG_INTERNAL_SHA512
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha512-internal.o
endif
ifdef NEED_TLS_PRF_SHA256
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha256-tlsprf.o
endif
ifdef NEED_TLS_PRF_SHA384
SHA256OBJS += $(BUILD_DIR)/src/crypto/sha384-tlsprf.o
endif
ifdef NEED_HMAC_SHA256_KDF
CFLAGS += -DCONFIG_HMAC_SHA256_KDF
OBJS += $(BUILD_DIR)/src/crypto/sha256-kdf.o
endif
ifdef NEED_HMAC_SHA384_KDF
CFLAGS += -DCONFIG_HMAC_SHA384_KDF
OBJS += $(BUILD_DIR)/src/crypto/sha384-kdf.o
endif
ifdef NEED_HMAC_SHA512_KDF
CFLAGS += -DCONFIG_HMAC_SHA512_KDF
OBJS += $(BUILD_DIR)/src/crypto/sha512-kdf.o
endif
OBJS += $(SHA256OBJS)
ifdef NEED_SHA384
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
OBJS += $(BUILD_DIR)/src/crypto/sha384.o
endif
endif
endif
endif
CFLAGS += -DCONFIG_SHA384
OBJS += $(BUILD_DIR)/src/crypto/sha384-prf.o
endif
ifdef NEED_SHA512
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
OBJS += $(BUILD_DIR)/src/crypto/sha512.o
endif
endif
endif
endif
CFLAGS += -DCONFIG_SHA512
OBJS += $(BUILD_DIR)/src/crypto/sha512-prf.o
endif

ifdef NEED_ASN1
OBJS += $(BUILD_DIR)/src/tls/asn1.o
endif

ifdef NEED_DH_GROUPS
OBJS += $(BUILD_DIR)/src/crypto/dh_groups.o
endif
ifdef NEED_DH_GROUPS_ALL
CFLAGS += -DALL_DH_GROUPS
endif
ifdef CONFIG_INTERNAL_DH_GROUP5
ifdef NEED_DH_GROUPS
OBJS += $(BUILD_DIR)/src/crypto/dh_group5.o
endif
endif

ifdef NEED_ECC
CFLAGS += -DCONFIG_ECC
endif

ifdef CONFIG_NO_RANDOM_POOL
CFLAGS += -DCONFIG_NO_RANDOM_POOL
else
ifdef CONFIG_GETRANDOM
CFLAGS += -DCONFIG_GETRANDOM
endif
OBJS += $(BUILD_DIR)/src/crypto/random.o
endif

ifdef CONFIG_CTRL_IFACE
ifeq ($(CONFIG_CTRL_IFACE), y)
ifdef CONFIG_NATIVE_WINDOWS
CONFIG_CTRL_IFACE=named_pipe
else
CONFIG_CTRL_IFACE=unix
endif
endif
CFLAGS += -DCONFIG_CTRL_IFACE
ifeq ($(CONFIG_CTRL_IFACE), unix)
CFLAGS += -DCONFIG_CTRL_IFACE_UNIX
OBJS += $(BUILD_DIR)/src/common/ctrl_iface_common.o
endif
ifeq ($(CONFIG_CTRL_IFACE), udp)
CFLAGS += -DCONFIG_CTRL_IFACE_UDP
endif
ifeq ($(CONFIG_CTRL_IFACE), udp6)
CONFIG_CTRL_IFACE=udp
CFLAGS += -DCONFIG_CTRL_IFACE_UDP
CFLAGS += -DCONFIG_CTRL_IFACE_UDP_IPV6
endif
ifeq ($(CONFIG_CTRL_IFACE), named_pipe)
CFLAGS += -DCONFIG_CTRL_IFACE_NAMED_PIPE
endif
ifeq ($(CONFIG_CTRL_IFACE), udp-remote)
CONFIG_CTRL_IFACE=udp
CFLAGS += -DCONFIG_CTRL_IFACE_UDP
CFLAGS += -DCONFIG_CTRL_IFACE_UDP_REMOTE
endif
ifeq ($(CONFIG_CTRL_IFACE), udp6-remote)
CONFIG_CTRL_IFACE=udp
CFLAGS += -DCONFIG_CTRL_IFACE_UDP
CFLAGS += -DCONFIG_CTRL_IFACE_UDP_REMOTE
CFLAGS += -DCONFIG_CTRL_IFACE_UDP_IPV6
endif
OBJS += $(BUILD_DIR)/wpa_supplicant/ctrl_iface.o $(BUILD_DIR)/wpa_supplicant/ctrl_iface_$(CONFIG_CTRL_IFACE).o
endif

ifdef CONFIG_CTRL_IFACE_DBUS_NEW
CFLAGS += -DCONFIG_CTRL_IFACE_DBUS_NEW
OBJS += dbus/dbus_dict_helpers.o
OBJS += dbus/dbus_new_helpers.o
OBJS += dbus/dbus_new.o dbus/dbus_new_handlers.o
OBJS += dbus/dbus_common.o
ifdef CONFIG_WPS
OBJS += dbus/dbus_new_handlers_wps.o
endif
ifdef CONFIG_P2P
OBJS += dbus/dbus_new_handlers_p2p.o
endif
ifndef DBUS_LIBS
DBUS_LIBS := $(shell $(PKG_CONFIG) --libs dbus-1)
endif
ifndef DBUS_INCLUDE
DBUS_INCLUDE := $(shell $(PKG_CONFIG) --cflags dbus-1)
endif
ifdef CONFIG_CTRL_IFACE_DBUS_INTRO
OBJS += dbus/dbus_new_introspect.o
CFLAGS += -DCONFIG_CTRL_IFACE_DBUS_INTRO
endif
CFLAGS += $(DBUS_INCLUDE)
LIBS += $(DBUS_LIBS)
endif

ifdef CONFIG_READLINE
OBJS_c += $(BUILD_DIR)/src/utils/edit_readline.o
LIBS_c += -lreadline -lncurses
else
ifdef CONFIG_WPA_CLI_EDIT
OBJS_c += $(BUILD_DIR)/src/utils/edit.o
else
OBJS_c += $(BUILD_DIR)/src/utils/edit_simple.o
endif
endif

ifdef CONFIG_NATIVE_WINDOWS
CFLAGS += -DCONFIG_NATIVE_WINDOWS
LIBS += -lws2_32 -lgdi32 -lcrypt32
LIBS_c += -lws2_32
LIBS_p += -lws2_32 -lgdi32
ifeq ($(CONFIG_CRYPTO), cryptoapi)
LIBS_p += -lcrypt32
endif
DBUS_CFLAGS += $(DBUS_INCLUDE)
endif

ifdef CONFIG_NO_STDOUT_DEBUG
CFLAGS += -DCONFIG_NO_STDOUT_DEBUG
ifndef CONFIG_CTRL_IFACE
CFLAGS += -DCONFIG_NO_WPA_MSG
endif
endif

ifdef CONFIG_IPV6
# for eapol_test only
CFLAGS += -DCONFIG_IPV6
endif

ifdef CONFIG_NO_LINUX_PACKET_SOCKET_WAR
CFLAGS += -DCONFIG_NO_LINUX_PACKET_SOCKET_WAR
endif

ifdef NEED_BASE64
OBJS += $(BUILD_DIR)/src/utils/base64.o
endif

ifdef NEED_SME
OBJS +=  $(BUILD_DIR)/wpa_supplicant/sme.o
CFLAGS += -DCONFIG_SME
endif

OBJS += $(BUILD_DIR)/src/common/ieee802_11_common.o
OBJS += $(BUILD_DIR)/src/common/hw_features_common.o

ifdef NEED_EAP_COMMON
OBJS += $(BUILD_DIR)/src/eap_common/eap_common.o
endif

ifndef CONFIG_MAIN
CONFIG_MAIN=main
endif

ifdef CONFIG_DEBUG_SYSLOG
CFLAGS += -DCONFIG_DEBUG_SYSLOG
ifdef CONFIG_DEBUG_SYSLOG_FACILITY
CFLAGS += -DLOG_HOSTAPD="$(CONFIG_DEBUG_SYSLOG_FACILITY)"
endif
endif

ifdef CONFIG_DEBUG_LINUX_TRACING
CFLAGS += -DCONFIG_DEBUG_LINUX_TRACING
endif

ifdef CONFIG_DEBUG_FILE
CFLAGS += -DCONFIG_DEBUG_FILE
endif

ifdef CONFIG_DELAYED_MIC_ERROR_REPORT
CFLAGS += -DCONFIG_DELAYED_MIC_ERROR_REPORT
endif

ifdef CONFIG_FIPS
CFLAGS += -DCONFIG_FIPS
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), wolfssl)
$(error CONFIG_FIPS=y requires CONFIG_TLS=openssl)
endif
endif
endif

OBJS += $(SHA1OBJS) $(DESOBJS)

OBJS_p += $(SHA1OBJS)
OBJS_p += $(SHA256OBJS)
OBJS_priv += $(SHA1OBJS)

ifdef CONFIG_BGSCAN_SIMPLE
CFLAGS += -DCONFIG_BGSCAN_SIMPLE
OBJS += bgscan_simple.o
NEED_BGSCAN=y
endif

ifdef CONFIG_BGSCAN_LEARN
CFLAGS += -DCONFIG_BGSCAN_LEARN
OBJS += bgscan_learn.o
NEED_BGSCAN=y
endif

ifdef NEED_BGSCAN
CFLAGS += -DCONFIG_BGSCAN
OBJS += bgscan.o
endif

ifdef CONFIG_AUTOSCAN_EXPONENTIAL
CFLAGS += -DCONFIG_AUTOSCAN_EXPONENTIAL
OBJS += autoscan_exponential.o
NEED_AUTOSCAN=y
endif

ifdef CONFIG_AUTOSCAN_PERIODIC
CFLAGS += -DCONFIG_AUTOSCAN_PERIODIC
OBJS += autoscan_periodic.o
NEED_AUTOSCAN=y
endif

ifdef NEED_AUTOSCAN
CFLAGS += -DCONFIG_AUTOSCAN
OBJS += autoscan.o
endif

ifdef CONFIG_EXT_PASSWORD_TEST
OBJS += $(BUILD_DIR)/src/utils/ext_password_test.o
CFLAGS += -DCONFIG_EXT_PASSWORD_TEST
NEED_EXT_PASSWORD=y
endif

ifdef CONFIG_EXT_PASSWORD_FILE
OBJS += $(BUILD_DIR)/src/utils/ext_password_file.o
CFLAGS += -DCONFIG_EXT_PASSWORD_FILE
NEED_EXT_PASSWORD=y
endif

ifdef NEED_EXT_PASSWORD
OBJS += $(BUILD_DIR)/src/utils/ext_password.o
CFLAGS += -DCONFIG_EXT_PASSWORD
endif

ifdef NEED_GAS_SERVER
OBJS += $(BUILD_DIR)/src/common/gas_server.o
CFLAGS += -DCONFIG_GAS_SERVER
NEED_GAS=y
endif

ifdef NEED_GAS
OBJS += $(BUILD_DIR)/src/common/gas.o
OBJS += gas_query.o
CFLAGS += -DCONFIG_GAS
NEED_OFFCHANNEL=y
endif

ifdef NEED_OFFCHANNEL
OBJS += offchannel.o
CFLAGS += -DCONFIG_OFFCHANNEL
endif

ifdef NEED_JSON
OBJS += $(BUILD_DIR)/src/utils/json.o
CFLAGS += -DCONFIG_JSON
endif
ifdef CONFIG_BAND_PREFERENCE
CFLAGS += -DCONFIG_BAND_PREFERENCE
endif

ifdef CONFIG_FAST_CONNECTION
CFLAGS += -DCONFIG_FAST_CONNECTION
endif

ifdef CONFIG_MTK_COMMON
CFLAGS += -DCONFIG_MTK_COMMON
endif

ifdef CONFIG_ROAMING_CUSTOMIZED_RSSI_DELTA
CFLAGS += -DCONFIG_ROAMING_CUSTOMIZED_RSSI_DELTA
endif

ifdef CONFIG_SUPP_SME
CFLAGS += -DCONFIG_SUPP_SME
endif

ifdef CONFIG_NO_UNUSED_EVENTS
CFLAGS += -DCONFIG_NO_UNUSED_EVENTS
endif

ifdef CONFIG_MODULE_TESTS
CFLAGS += -DCONFIG_MODULE_TESTS
OBJS += $(BUILD_DIR)/src/wps/wpas_module_tests.o
OBJS += $(BUILD_DIR)/src/utils/utils_module_tests.o
OBJS += $(BUILD_DIR)/src/common/common_module_tests.o
OBJS += $(BUILD_DIR)/src/crypto/crypto_module_tests.o
ifdef CONFIG_WPS
OBJS += $(BUILD_DIR)/src/wps/wps_module_tests.o
endif
endif

OBJS += $(BUILD_DIR)/src/drivers/driver_common.o
OBJS_priv += $(BUILD_DIR)/src/drivers/driver_common.o
#rtos_porting
OBJS += $(BUILD_DIR)/wpa_supplicant/wpa_supplicant.o
OBJS += $(BUILD_DIR)/wpa_supplicant/events.o
OBJS += $(BUILD_DIR)/wpa_supplicant/bssid_ignore.o
OBJS += $(BUILD_DIR)/wpa_supplicant/wpas_glue.o
OBJS += $(BUILD_DIR)/wpa_supplicant/scan.o
OBJS_t := $(OBJS) $(OBJS_l2) eapol_test.o
OBJS_t += $(BUILD_DIR)/src/radius/radius_client.o
OBJS_t += $(BUILD_DIR)/src/radius/radius.o
OBJS_t2 := $(OBJS) $(OBJS_l2) preauth_test.o

OBJS_nfc := $(OBJS) $(OBJS_l2) nfc_pw_token.o
OBJS_nfc += $(OBJS_d) $(BUILD_DIR)/src/drivers/drivers.o
#rtos_porting
ifeq ($(BUILD_BACH),)
OBJS += $(CONFIG_MAIN).o
else
OBJS += $(BUILD_DIR)/wpa_supplicant/wpa_supplicant_entry.o
OBJS += $(BUILD_DIR)/wpa_supplicant/wpa_supplicant_task.o
endif

ifdef CONFIG_PRIVSEP
OBJS_priv += $(OBJS_d) $(BUILD_DIR)/src/drivers/drivers.o
OBJS_priv += $(OBJS_l2)
OBJS_priv += $(BUILD_DIR)/src/utils/os_$(CONFIG_OS).o
OBJS_priv += $(BUILD_DIR)/src/utils/$(CONFIG_ELOOP).o
OBJS_priv += $(BUILD_DIR)/src/utils/common.o
OBJS_priv += $(BUILD_DIR)/src/utils/wpa_debug.o
OBJS_priv += $(BUILD_DIR)/src/utils/wpabuf.o
OBJS_priv += $(BUILD_DIR)/wpa_supplicant/wpa_priv.o
ifdef CONFIG_DRIVER_NL80211
OBJS_priv += $(BUILD_DIR)/src/common/ieee802_11_common.o
endif
OBJS += $(BUILD_DIR)/src/l2_packet/l2_packet_privsep.o
OBJS += $(BUILD_DIR)/src/drivers/driver_privsep.o
EXTRA_progs += wpa_priv
else
OBJS += $(OBJS_d) $(BUILD_DIR)/src/drivers/drivers.o
OBJS += $(OBJS_l2)
endif

ifdef CONFIG_NDIS_EVENTS_INTEGRATED
CFLAGS += -DCONFIG_NDIS_EVENTS_INTEGRATED
OBJS += $(BUILD_DIR)/src/drivers/ndis_events.o
EXTRALIBS += -loleaut32 -lole32 -luuid
ifdef PLATFORMSDKLIB
EXTRALIBS += $(PLATFORMSDKLIB)/WbemUuid.Lib
else
EXTRALIBS += WbemUuid.Lib
endif
endif

ifdef CONFIG_FST
CFLAGS += -DCONFIG_FST
ifdef CONFIG_FST_TEST
CFLAGS += -DCONFIG_FST_TEST
endif
FST_OBJS += $(BUILD_DIR)/src/fst/fst.o
FST_OBJS += $(BUILD_DIR)/src/fst/fst_session.o
FST_OBJS += $(BUILD_DIR)/src/fst/fst_iface.o
FST_OBJS += $(BUILD_DIR)/src/fst/fst_group.o
FST_OBJS += $(BUILD_DIR)/src/fst/fst_ctrl_aux.o
ifdef CONFIG_CTRL_IFACE
FST_OBJS += $(BUILD_DIR)/src/fst/fst_ctrl_iface.o
endif
OBJS += $(FST_OBJS)
OBJS_t += $(FST_OBJS)
OBJS_t2 += $(FST_OBJS)
OBJS_nfc += $(FST_OBJS)
endif

ifdef CONFIG_WEP
CFLAGS += -DCONFIG_WEP
endif

ifdef CONFIG_NO_TKIP
CFLAGS += -DCONFIG_NO_TKIP
endif

ifdef CONFIG_SUPPORT_6G
CFLAGS += -DCONFIG_SUPPORT_6G
endif

ifdef CONFIG_IEEE80211AD
CFLAGS += -DCONFIG_IEEE80211AD
endif

ifdef CONFIG_LINK_MEASUREMENT
CFLAGS += -DCONFIG_LINK_MEASUREMENT
endif

ifdef CONFIG_AP_MULTI_AP
CFLAGS += -DCONFIG_AP_MULTI_AP
endif

ifdef CONFIG_STA_MULTI_AP
CFLAGS += -DCONFIG_STA_MULTI_AP
endif

ifdef CONFIG_BSS_LOAD
CFLAGS += -DCONFIG_BSS_LOAD
endif

ifdef CONFIG_WMM_STA
CFLAGS += -DCONFIG_WMM_STA
endif

ifdef CONFIG_WMM_AP
CFLAGS += -DCONFIG_WMM_AP
endif

ifdef CONFIG_SCHED_SCAN
CFLAGS += -DCONFIG_SCHED_SCAN
endif

ifdef CONFIG_SCHED_SCAN_PLAN
CFLAGS += -DCONFIG_SCHED_SCAN_PLAN
endif

ifdef CONFIG_BW20_ONLY
CFLAGS += -DCONFIG_BW20_ONLY
endif

ifdef CONFIG_OBSS_SCAN
CFLAGS += -DCONFIG_OBSS_SCAN
endif

ifdef CONFIG_UNUSED_INFO
CFLAGS += -DCONFIG_UNUSED_INFO
endif

ifndef LDO
LDO=$(CC)
endif

Q=@
E=echo
ifeq ($(V), 1)
Q=
E=true
endif
ifeq ($(QUIET), 1)
Q=@
E=true
endif

dynamic_eap_methods: $(EAPDYN)

$(BUILD_DIR)/src/drivers/build.wpa_supplicant:
	@if [ -f $(BUILD_DIR)/src/drivers/build.hostapd ]; then \
		$(MAKE) -C $(BUILD_DIR)/src/drivers clean; \
	fi
	@mkdir -p $(BUILD_DIR)/src/drivers
	@touch $(BUILD_DIR)/src/drivers/build.wpa_supplicant

BCHECK=$(BUILD_DIR)/src/drivers/build.wpa_supplicant

$(OBJS_c) $(OBJS_t) $(OBJS_t2) $(OBJS) $(BCHECK) $(EXTRA_progs): $(DIR)/wpa_supplicant/.config
#rtos_porting
lib$(NAME).a: $(OBJS)
	$(AR) rcs $(BUILD_DIR)/$@ $?

_OBJS_VAR := OBJS_priv
include ../src/objs.mk
wpa_priv: $(BCHECK) $(OBJS_priv)
	$(Q)$(LDO) $(LDFLAGS) -o wpa_priv $(OBJS_priv) $(LIBS)
	@$(E) "  LD " $@

_OBJS_VAR := OBJS_t
include ../src/objs.mk
#rtos_porting
wpa_supplicant: $(BCHECK) $(OBJS) $(EXTRA_progs) lib$(NAME).a
	@echo "Build [wpa_supplicant] Done"
#	$(Q)$(LDO) $(LDFLAGS) -o wpa_supplicant $(OBJS) $(LIBS) $(EXTRALIBS)
#	@$(E) "  LD " $@

eapol_test: $(OBJS_t)
	$(Q)$(LDO) $(LDFLAGS) -o eapol_test $(OBJS_t) $(LIBS)
	@$(E) "  LD " $@

_OBJS_VAR := OBJS_t2
include ../src/objs.mk
preauth_test: $(OBJS_t2)
	$(Q)$(LDO) $(LDFLAGS) -o preauth_test $(OBJS_t2) $(LIBS)
	@$(E) "  LD " $@

_OBJS_VAR := OBJS_p
include ../src/objs.mk
wpa_passphrase: $(OBJS_p)
	$(Q)$(LDO) $(LDFLAGS) -o wpa_passphrase $(OBJS_p) $(LIBS_p) $(LIBS)
	@$(E) "  LD " $@

_OBJS_VAR := OBJS_c
include ../src/objs.mk
wpa_cli: $(OBJS_c)
	$(Q)$(LDO) $(LDFLAGS) -o wpa_cli $(OBJS_c) $(LIBS_c)
	@$(E) "  LD " $@

LIBCTRL += $(BUILD_DIR)/src/common/wpa_ctrl.o
LIBCTRL += $(BUILD_DIR)/src/utils/os_$(CONFIG_OS).o
LIBCTRL += $(BUILD_DIR)/src/utils/common.o
LIBCTRL += $(BUILD_DIR)/src/utils/wpa_debug.o
LIBCTRLSO += $(BUILD_DIR)/src/common/wpa_ctrl.c
LIBCTRLSO += $(BUILD_DIR)/src/utils/os_$(CONFIG_OS).c
LIBCTRLSO += $(BUILD_DIR)/src/utils/common.c
LIBCTRLSO += $(BUILD_DIR)/src/utils/wpa_debug.c

_OBJS_VAR := LIBCTRL
include ../src/objs.mk
libwpa_client.a: $(LIBCTRL)
	$(Q)rm -f $@
	$(Q)$(AR) crs $@ $?
	@$(E) "  AR " $@

libwpa_client.so: $(LIBCTRLSO)
	@$(E) "  CC  $@ ($^)"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -fPIC $^

OBJS_wpatest := libwpa_test.o
_OBJS_VAR := OBJS_wpatest
include ../src/objs.mk
libwpa_test1: $(OBJS_wpatest) libwpa_client.a
	$(Q)$(LDO) $(LDFLAGS) -o libwpa_test1 $(OBJS_wpatest) libwpa_client.a $(LIBS_c)
	@$(E) "  LD " $@

libwpa_test2: $(OBJS_wpatest) libwpa_client.so
	$(Q)$(LDO) $(LDFLAGS) -o libwpa_test2 $(OBJS_wpatest) -L. -lwpa_client $(LIBS_c)
	@$(E) "  LD " $@

_OBJS_VAR := OBJS_nfc
include ../src/objs.mk
nfc_pw_token: $(OBJS_nfc)
	$(Q)$(LDO) $(LDFLAGS) -o nfc_pw_token $(OBJS_nfc) $(LIBS)
	@$(E) "  LD " $@

win_if_list: win_if_list.c
	$(Q)$(LDO) $(LDFLAGS) -o $@ win_if_list.c $(CFLAGS) $(LIBS_w)
	@$(E) "  LD " $@

eap_psk.so: $(SRC_EAP_PSK)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-Deap_peer_psk_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_pax.so: $(SRC_EAP_PAX)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_peap.so: $(SRC_EAP_PEAP)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_sake.so: $(SRC_EAP_SAKE)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_ikev2.so: $(SRC_EAP_IKEV2)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_eke.so: $(SRC_EAP_EKE)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_mschapv2.so: $(SRC_EAP_MSCHAPV2)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_fast.so: $(SRC_EAP_FAST)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_teap.so: $(SRC_EAP_TEAP)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

eap_gpsk.so: $(SRC_EAP_GPSK)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $^ \
		-D$(@F:eap_%.so=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

%.so: ../src/eap_peer/%.c
	$(Q)$(CC) $(LDFLAGS) -o $@ $(CFLAGS) -shared -rdynamic -fPIC $< \
		-D$(*F:eap_%=eap_peer_%)_register=eap_peer_method_dynamic_init
	@$(E) "  CC/LD " $@

ifdef CONFIG_CODE_COVERAGE
$(BUILD_DIR)/%.o: $(DIR)/%.c
#	$(Q)mkdir -p `dirname $@`
	@$(E) "  CC " $<
	$(Q)cd $(dir $@); $(CC) -c -o $(notdir $@) $(CFLAGS) $(notdir $<)
else
$(BUILD_DIR)/%.o: $(DIR)/%.c
	$(Q)mkdir -p `dirname $@`
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<
	@$(E) "  CC " $<
endif

%.service: %.service.in
	$(Q)sed -e 's|\@BINDIR\@|$(BINDIR)|g' $< >$@
	@$(E) "  sed" $<

%@.service: %.service.arg.in
	$(Q)sed -e 's|\@BINDIR\@|$(BINDIR)|g' $< >$@
	@$(E) "  sed" $<

wpa_supplicant.exe: wpa_supplicant
	mv -f $< $@
wpa_cli.exe: wpa_cli
	mv -f $< $@
wpa_passphrase.exe: wpa_passphrase
	mv -f $< $@
win_if_list.exe: win_if_list
	mv -f $< $@
eapol_test.exe: eapol_test
	mv -f $< $@

WINALL=wpa_supplicant.exe wpa_cli.exe wpa_passphrase.exe win_if_list.exe

windows-bin: $(WINALL)
	$(STRIP) $(WINALL)

wpa_gui:
	@echo "wpa_gui has been removed - see wpa_gui-qt4 for replacement"

wpa_gui-qt4/Makefile:
	qmake -o wpa_gui-qt4/Makefile wpa_gui-qt4/wpa_gui.pro

wpa_gui-qt4/lang/wpa_gui_de.qm: wpa_gui-qt4/lang/wpa_gui_de.ts
	lrelease wpa_gui-qt4/wpa_gui.pro

wpa_gui-qt4: wpa_gui-qt4/Makefile wpa_gui-qt4/lang/wpa_gui_de.qm
	$(MAKE) -C wpa_gui-qt4

FIPSDIR=/usr/local/ssl/fips-2.0
FIPSLD=$(FIPSDIR)/bin/fipsld
fips:
	$(MAKE) CC=$(FIPSLD) FIPSLD_CC="$(CC)"

.PHONY: lcov-html
lcov-html: $(call BUILDOBJ,wpa_supplicant.gcda)
	lcov -c -d $(BUILDDIR) > lcov.info
	genhtml lcov.info --output-directory lcov-html

clean:
	$(MAKE) -C $(BUILD_DIR)/src clean
	$(MAKE) -C dbus clean
	rm -f core *~ *.o *.d *.gcno *.gcda *.gcov
	rm -f eap_*.so $(WINALL) eapol_test preauth_test
	rm -f wpa_priv
	rm -f nfc_pw_token
	rm -f lcov.info
	rm -rf lcov-html
	rm -f libwpa_client.a
	rm -f libwpa_client.so
	rm -f libwpa_test1 libwpa_test2
