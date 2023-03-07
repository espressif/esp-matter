include $(COMPONENT_PATH)/../ble_common.mk

##################################################################################
#
# Component Makefile
#
##################################################################################
ble_stack_srcs_dirs := src/bl_hci_wrapper \
					src/port \
					src/common \
					src/common/tinycrypt/source  \
					src/hci_onchip   \
					src/host   \
					src/services \
					src/sbc/dec \
					src/sbc/enc \

ifeq ($(CONFIG_BT_OAD_SERVER),1)
ble_stack_srcs_dirs+= src/services/oad
endif

ifeq ($(CONFIG_BT_OAD_CLIENT),1)
ble_stack_srcs_dirs+= src/services/oad
endif

ifeq ($(CONFIG_BT_STACK_CLI),1)
ble_stack_srcs_dirs+= src/cli_cmds
endif

ble_stack_srcs_include_dirs    += src/port/include \
								src \
								src/common \
								src/common/include \
								src/common/include/zephyr  \
								src/common/include/misc  \
								src/common/include/toolchain \
								src/common/tinycrypt/include/tinycrypt  \
								src/hci_onchip   \
								src/bl_hci_wrapper \
								src/host   \
								src/include/bluetooth  \
								src/include/drivers/bluetooth  \
								src/profiles \
								src/cli_cmds \
								src/services

ifneq ($(CONFIG_BT_OAD_SERVER)_$(CONFIG_BT_OAD_CLIENT),0_0)
ble_stack_srcs_include_dirs    += src/services/oad
endif

# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS := $(ble_stack_srcs_include_dirs)

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS   :=

## This component's src 
ble_stack_srcs  := src/port/bl_port.c \
					src/common/atomic_c.c \
					src/common/buf.c \
					src/common/log.c \
					src/common/poll.c \
					src/common/rpa.c \
					src/common/work_q.c \
					src/common/utils.c \
					src/common/dec.c \
					src/common/dummy.c \
					src/common/tinycrypt/source/aes_decrypt.c \
					src/common/tinycrypt/source/aes_encrypt.c \
					src/common/tinycrypt/source/cbc_mode.c \
					src/common/tinycrypt/source/ccm_mode.c \
					src/common/tinycrypt/source/cmac_mode.c \
					src/common/tinycrypt/source/ctr_mode.c \
					src/common/tinycrypt/source/ctr_prng.c \
					src/common/tinycrypt/source/ecc.c \
					src/common/tinycrypt/source/ecc_dh.c \
					src/common/tinycrypt/source/ecc_dsa.c \
					src/common/tinycrypt/source/ecc_platform_specific.c \
					src/common/tinycrypt/source/hmac.c \
					src/common/tinycrypt/source/hmac_prng.c \
					src/common/tinycrypt/source/sha256.c \
					src/common/tinycrypt/source/utils.c \
					src/bl_hci_wrapper/bl_hci_wrapper.c \
					src/hci_onchip/hci_driver.c \
					src/host/crypto.c \
					src/host/hci_core.c \
					src/host/hci_ecc.c \
					src/host/l2cap.c \
					src/host/uuid.c

ifneq ($(CONFIG_BT_CONN), 0)
ble_stack_srcs  += src/host/att.c \
                   src/host/conn.c \
		   src/host/gatt.c
endif
					
ifneq ($(CONFIG_DISABLE_BT_SMP), 1)
ble_stack_srcs  += src/host/smp.c \
                   src/host/keys.c
endif
ifeq ($(CONFIG_BT_DEBUG_MONITOR), 1)
ble_stack_srcs  += src/host/monitor.c 
endif
				
ifeq ($(CONFIG_BT_OAD_CLIENT),1)
ble_stack_srcs   += src/services/oad/oad_client.c
endif

ifneq ($(CONFIG_DBG_RUN_ON_FPGA), 1)
ble_stack_srcs   += src/host/settings.c
endif

ifeq ($(CONFIG_BT_OAD_SERVER),1)
ble_stack_srcs   += src/services/oad/oad_main.c \
					src/services/oad/oad_service.c
endif

ifeq ($(CONFIG_BT_SPP_SERVER),1)
ble_stack_srcs   += src/services/spp.c
endif


ifeq ($(CONFIG_BT_STACK_CLI),1)
ble_stack_srcs   += src/cli_cmds/ble_cli_cmds.c \
					src/cli_cmds/bredr_cli_cmds.c \
					src/cli_cmds/pts_cli_cmds.c
endif

ifeq ($(CONFIG_BT_BAS_SERVER),1)
ble_stack_srcs   += src/services/bas.c
endif

ifeq ($(CONFIG_BT_SCPS_SERVER),1)
ble_stack_srcs   += src/services/scps.c
endif

ifeq ($(CONFIG_BT_DIS_SERVER),1)
ble_stack_srcs   += src/services/dis.c
endif

ifeq ($(CONFIG_BT_IAS_SERVER),1)
ble_stack_srcs   += src/services/ias.c
endif

ifeq ($(CONFIG_HOGP_SERVER),1)
ble_stack_srcs   += src/services/hog.c
endif

ifeq ($(CONFIG_ATVV_SERVER),1)
ble_stack_srcs   += src/services/atvv.c
endif


ifeq ($(CONFIG_BLE_TP_SERVER),1)
ble_stack_srcs   += src/services/ble_tp_svc.c
endif

ifeq ($(CONFIG_BLE_MULTI_ADV),1)
ble_stack_srcs   += src/host/multi_adv.c
endif

ifeq ($(CONFIG_BT_RESET),1)
ble_stack_srcs   += src/host/bl_host_assist.c
endif

ble_audio_srcs   := src/host/iso.c

bredr_stack_srcs := src/host/keys_br.c \
                    src/host/l2cap_br.c \

sbc_codec_srcs := 	src/sbc/dec/alloc.c \
					src/sbc/dec/bitalloc.c \
					src/sbc/dec/bitalloc-sbc.c \
					src/sbc/dec/bitstream-decode.c \
					src/sbc/dec/decoder-oina.c \
					src/sbc/dec/decoder-private.c \
					src/sbc/dec/decoder-sbc.c \
					src/sbc/dec/dequant.c \
					src/sbc/dec/framing.c \
					src/sbc/dec/framing-sbc.c \
					src/sbc/dec/oi_codec_version.c \
					src/sbc/dec/synthesis-8-generated.c \
					src/sbc/dec/synthesis-dct8.c \
					src/sbc/dec/synthesis-sbc.c \
					src/sbc/enc/sbc_analysis.c \
					src/sbc/enc/sbc_dct.c \
					src/sbc/enc/sbc_dct_coeffs.c \
					src/sbc/enc/sbc_enc_bit_alloc_mono.c \
					src/sbc/enc/sbc_enc_bit_alloc_ste.c \
					src/sbc/enc/sbc_enc_coeffs.c \
					src/sbc/enc/sbc_encoder.c \
					src/sbc/enc/sbc_packing.c \

sbc_codec_include_dirs := 	src/sbc/dec \
							src/sbc/enc \

COMPONENT_SRCS := $(ble_stack_srcs)

ifeq ($(CONFIG_BT_AUDIO),1)
COMPONENT_SRCS += $(ble_audio_srcs)
endif

ifeq ($(CONFIG_BT_BREDR),1)
COMPONENT_SRCS += $(bredr_stack_srcs)
COMPONENT_SRCS += $(sbc_codec_srcs)
COMPONENT_ADD_INCLUDEDIRS += $(sbc_codec_include_dirs)
endif

COMPONENT_OBJS   := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := $(ble_stack_srcs_dirs)
