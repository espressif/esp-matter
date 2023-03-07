# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include/bl_supplicant

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := port/include \
                              src \

## This component's src
COMPONENT_SRCS += port/os_bl.c \
                  src/ap/ap_config.c \
                  src/ap/wpa_auth_ie.c \
                  src/ap/wpa_auth_rsn_ccmp_only.c \
                  src/bl_supplicant/bl_hostap.c \
                  src/bl_supplicant/bl_wpa3.c \
                  src/bl_supplicant/bl_wpa_main.c \
                  src/bl_supplicant/bl_wpas_glue.c \
                  src/bl_supplicant/bl_wps.c \
                  src/common/sae.c \
                  src/common/wpa_common.c \
                  src/crypto/aes-cbc.c \
                  src/crypto/aes-internal-bl.c \
                  src/crypto/aes-omac1.c \
                  src/crypto/aes-unwrap.c \
                  src/crypto/aes-wrap.c \
                  src/crypto/crypto_internal-modexp.c \
                  src/crypto/dh_group5.c \
                  src/crypto/dh_groups.c \
                  src/crypto/md5-internal.c \
                  src/crypto/md5.c \
                  src/crypto/rc4.c \
                  src/crypto/sha1-internal.c \
                  src/crypto/sha1-pbkdf2.c \
                  src/crypto/sha1.c \
                  src/crypto/sha256-internal.c \
                  src/crypto/sha256-prf.c \
                  src/crypto/sha256.c \
                  src/eap_peer/eap_common.c \
                  src/rsn_supp/pmksa_cache.c \
                  src/rsn_supp/wpa.c \
                  src/rsn_supp/wpa_ie.c \
                  src/utils/common.c \
                  src/utils/wpa_debug.c \
                  src/utils/wpabuf.c \
                  src/wps/wps.c \
                  src/wps/wps_attr_build.c \
                  src/wps/wps_attr_parse.c \
                  src/wps/wps_attr_process.c \
                  src/wps/wps_common.c \
                  src/wps/wps_dev_attr.c \
                  src/wps/wps_enrollee.c \
                  src/wps/wps_registrar.c \
                  src/wps/wps_validate.c \
                  test/test_crypto-bl.c \


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := port \
                     src/ap \
                     src/bl_supplicant \
                     src/common \
                     src/crypto \
                     src/eap_peer \
                     src/rsn_supp \
                     src/utils \
                     src/wps \
                     test \

##
#CPPFLAGS +=
