#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

HOST_PROJ_ROOT ?= ../..
include $(HOST_PROJ_ROOT)/Makefile.defs

API_DIR = $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api
DRV_DIR = $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/driver
MBEDTLS_ALT_DIR = $(CODESAFE_SRCDIR)/mbedtls_api
MBEDTLS_ALT_API = $(SHARED_INCDIR)/mbedtls
MBEDTLS_ALT_API_TESTS = $(SHARED_SRCDIR)/mbedtls/tests
MBEDTLS_ROOT = $(HOST_SRCDIR)/../../mbedtls
SOFTCRYS_API = $(HOST_SRCDIR)/softcrys/softcrys_api
CC_API = $(SHARED_INCDIR)/crypto_api
CC_API_3x = $(SHARED_INCDIR)/crypto_api/$(PROJ_PRD)
MANAGEMENT_API = $(HOST_SRCDIR)/cc_mng
MANAGEMENT_INCLUDES = $(SHARED_INCDIR)/cc_mng
UTILS_API = $(HOST_SRCDIR)/utils
SBROM_DIR = $(CODESAFE_SRCDIR)/secure_boot_debug

#TESTS_COMMON_API = $(HOST_SRCDIR)/tests/common


TARGET_LIBS = cc_312

ifeq ($(PKA_DEBUG),1)
CFLAGS_EXTRA += -DPKA_DEBUG
endif



PUBLIC_INCLUDES += $(CC_API_3x)/cc_aes_defs_proj.h
PUBLIC_INCLUDES += $(CODESAFE_SRCDIR)/mbedtls_api/mbedtls_ccm_common.h $(CC_API)/cc_ecpki_types.h $(CC_API_3x)/cc_pka_defs_hw.h
PUBLIC_INCLUDES += $(CC_API)/cc_kdf.h $(CC_API)/cc_hash_defs.h# not to document
PUBLIC_INCLUDES += $(CC_API)/cc_error.h $(CC_API)/cc_aes_defs.h
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_cc_sha512_t.h
PUBLIC_INCLUDES += $(CC_API_3x)/cc_rnd_common.h

PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_cc_ecies.h
PUBLIC_INCLUDES += $(HOST_LIBDIR)/cc_lib.h $(HOST_LIBDIR)/mbedtls_cc_sbrt.h $(HOST_LIBDIR)/mbedtls_cc_util_asset_prov.h
PUBLIC_INCLUDES += $(CC_API)/$(PROJ_PRD)/cc_hash_defs_proj.h
PUBLIC_INCLUDES += $(MBEDTLS_ALT_API)/sha1_alt.h $(MBEDTLS_ALT_API)/sha256_alt.h $(MBEDTLS_ALT_API)/aes_alt.h
PUBLIC_INCLUDES += $(MBEDTLS_ALT_API)/ccm_alt.h $(MBEDTLS_ALT_API)/gcm_alt.h $(MBEDTLS_ALT_API)/rsa_alt.h
PUBLIC_INCLUDES += $(MBEDTLS_ALT_API)/config-cc312.h $(MBEDTLS_ALT_API)/config-cc312-mps2-freertos.h $(MBEDTLS_ALT_API)/config-cc312-mps2-no-os.h
PUBLIC_INCLUDES += $(MBEDTLS_ALT_API)/cmac_alt.h $(MBEDTLS_ALT_API)/dhm_alt.h $(MBEDTLS_ALT_API)/chacha20_alt.h $(MBEDTLS_ALT_API)/poly1305_alt.h $(MBEDTLS_ALT_API)/chachapoly_alt.h
PUBLIC_INCLUDES += $(MBEDTLS_ALT_API)/cc_ecc_internal.h $(CC_API_3x)/mbedtls_cc_ecdsa_edwards.h $(CC_API_3x)/mbedtls_cc_ecdh_edwards.h

PUBLIC_INCLUDES += $(MANAGEMENT_INCLUDES)/mbedtls_cc_mng.h $(MANAGEMENT_INCLUDES)/mbedtls_cc_mng_error.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/cc_util/mbedtls_cc_util_key_derivation.h $(SHARED_INCDIR)/cc_util/mbedtls_cc_util_key_derivation_defs.h $(SHARED_INCDIR)/cc_util/mbedtls_cc_util_defs.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/cc_util/cc_util_error.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/cc_regs.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/cc_bitops.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/../hw/include/dx_host.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/../hw/include/dx_nvm.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/../hw/include/dx_reg_common.h
PUBLIC_INCLUDES += $(HOST_PROJ_ROOT)/src/cc_mng/mbedtls_cc_mng_int.h
PUBLIC_INCLUDES += $(HOST_PROJ_ROOT)/src/hal/cc_hal_plat.h

PUBLIC_INCLUDES += $(HOST_PROJ_ROOT)/src/cc3x_productionlib/common/prod_hw_defs.h
PUBLIC_INCLUDES += $(HOST_PROJ_ROOT)/src/cc3x_productionlib/common/prod_util.h

PUBLIC_INCLUDES += $(SHARED_INCDIR)/proj/cc3x/cc_general_defs.h $(SHARED_INCDIR)/proj/cc3x/cc_ecpki_domains_defs.h $(SHARED_INCDIR)/proj/cc3x/cc_sram_map.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/proj/cc3x/cc_pka_hw_plat_defs.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/proj/cc3x/cc_otp_defs.h
PUBLIC_INCLUDES += $(SHARED_INCDIR)/proj/cc3x/cc_production_asset.h

#FFCDH
PUBLIC_INCLUDES += $(CC_API)/cc_ffcdh.h $(CC_API)/cc_ffc_domain.h $(CC_API)/cc_ffcdh_error.h $(CC_API)/cc_ffc_domain_error.h

#INCDIRS_EXTRA += $(TESTS_COMMON_API)
#external DMA chacha include
ifeq ($(CC_CONFIG_CC_CHACHA_POLY_SUPPORT),1)
ifeq ($(CC_CONFIG_SUPPORT_EXT_DMA),1)
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_chacha_ext_dma.h
endif
endif

ifeq ($(CC_CONFIG_SUPPORT_EXT_DMA),1)
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_aes_ext_dma.h
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_hash_ext_dma.h
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_ext_dma_error.h
endif

#lib sources
SOURCES_cc_312 = cc_lib.c cc_hal.c
SOURCES_cc_312 += bypass_driver.c aes_driver.c aesccm_driver.c
SOURCES_cc_312 += aesgcm_driver.c driver_common.c
SOURCES_cc_312 += mbedtls_cc_sha512_t.c

#external DMA
ifeq ($(CC_CONFIG_SUPPORT_EXT_DMA),1)
SOURCES_cc_312 += aes_driver_ext_dma.c mbedtls_aes_ext_dma.c hash_driver_ext_dma.c mbedtls_hash_ext_dma.c
endif

ifeq ($(CC_CONFIG_CC_CHACHA_POLY_SUPPORT),1)
ifeq ($(CC_CONFIG_SUPPORT_EXT_DMA),1)
SOURCES_cc_312 += mbedtls_chacha_ext_dma.c chacha_driver_ext_dma.c
endif
endif

#mbedtls_api
SOURCES_cc_312 += sha1_alt.c sha256_alt.c mbedtls_hash_common.c aes_alt.c mbedtls_common.c ccm_alt.c gcm_alt.c trng_api.c mbedtls_ccm_internal.c
SOURCES_cc_312 += rsa_alt.c ecp_common.c cc_ecp_internal.c ecdsa_alt.c ecdh_alt.c ecdsa_edwards.c
SOURCES_cc_312 += cmac_alt.c dhm_alt.c chacha20_alt.c poly1305_alt.c chachapoly_alt.c
##SOURCES_cc_312 += platform_alt.c

#management
SOURCES_cc_312 += mbedtls_cc_mng.c mbedtls_cc_mng_int.c


#SB_RT
ifeq ($(CC_CONFIG_SUPPORT_SB_RT),1)
CFLAGS_EXTRA += -DCC_SB_SUPPORT_SB_RT
PUBLIC_INCLUDES += $(CODESAFE_SRCDIR)/secure_boot_debug/platform/common/cc3x/secureboot_defs.h
PUBLIC_INCLUDES += $(CODESAFE_SRCDIR)/secure_boot_debug/secure_boot_gen/secureboot_gen_defs.h
PUBLIC_INCLUDES += $(CODESAFE_SRCDIR)/secure_boot_debug/secure_boot_gen/secureboot_basetypes.h
PUBLIC_INCLUDES += $(CODESAFE_SRCDIR)/secure_boot_debug/platform/pal/cc3x/cc_pal_sb_plat.h
PUBLIC_INCLUDES += $(CODESAFE_SRCDIR)/secure_boot_debug/cc3x_verifier/bootimagesverifier_def.h
SOURCES_cc_312 += mbedtls_cc_sbrt.c sbrt_int_func.c bootimagesverifier_parser.c nvm_otp.c
SOURCES_cc_312 += bootimagesverifier_swcomp.c secureboot_base_swimgverify.c
SOURCES_cc_312 += rsa_verify.c rsa_exp.c rsa_pki_pka.c
SOURCES_cc_312 += bootimagesverifier_base_single.c bsv_aes_driver.c bsv_hash_driver.c
SOURCES_cc_312 += common_cert_verify.c  secureboot_base_func.c
ifeq ($(CC_CONFIG_SB_X509_CERT_SUPPORTED),0)
SOURCES_cc_312 += common_cert_parser.c
else #x509 files
CFLAGS_EXTRA += -DCC_SB_X509_CERT_SUPPORTED -DCC_SB_SUPPORT_IOT
SOURCES_cc_312 += cc3x_sb_x509_parser.c
SOURCES_cc_312 += util_asn1_parser.c util_x509_parser.c sb_x509_cert_parser.c cc3x_sb_x509_ext_parser.c cc_pal_x509_verify.c
INCDIRS_EXTRA += $(SBROM_DIR)/util $(SHARED_INCDIR)/sbrom $(SBROM_DIR)/platform/pal $(SBROM_DIR)/x509_cert_parser $(SBROM_DIR)/x509_verifier
endif #CC_CONFIG_SB_X509_CERT_SUPPORTED
endif #CC_CONFIG_SUPPORT_SB_RT

#util
SOURCES_cc_312 += cc_util_cmac.c cc_hash_info.c mbedtls_cc_util_key_derivation.c mbedtls_cc_util_asset_prov.c

# Asymmetric sources
#common PKA functions
SOURCES_cc_312 += pki.c pka.c
#KDF
SOURCES_cc_312 += cc_kdf.c

#RSA and dependent functions

ifeq ($(RSA_KG_NO_RND),1)
	CFLAGS+= -DRSA_KG_NO_RND
	SOURCES_cc_312 += rsa_kg_debug_data.c
endif

SOURCES_cc_312 += cc_rsa_info.c cc_rsa_build.c
SOURCES_cc_312 += cc_rsa_oaep.c cc_rsa_schemes.c cc_rsa_pkcs_ver15_util.c cc_rsa_pss21_util.c cc_rsa_prim.c cc_rsa_verify.c
SOURCES_cc_312 += cc_rsa_kg.c cc_rsa_sign.c
SOURCES_cc_312 += rsa_public.c rsa_private.c rsa_genkey.c
# ffcdh
SOURCES_cc_312 += cc_ffc_domain.c


ifeq ($(CC_CONFIG_CC_CHACHA_POLY_SUPPORT),1)
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_cc_chacha.h  $(CC_API_3x)/mbedtls_cc_chacha_error.h
PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_cc_poly.h $(CC_API_3x)/mbedtls_cc_poly_error.h  $(CC_API_3x)/mbedtls_cc_chacha_poly.h $(CC_API_3x)/mbedtls_cc_chacha_poly_error.h
SOURCES_cc_312 += chacha_driver.c mbedtls_cc_chacha.c  mbedtls_cc_chacha_poly.c   mbedtls_cc_poly.c poly.c
endif


#ECC(Canonic) CC
SOURCES_cc_312 += cc_ecpki_info.c
SOURCES_cc_312 += cc_ecpki_build_publ.c cc_ecpki_build_priv.c cc_ecdsa_verify.c cc_ecdsa_sign.c
SOURCES_cc_312 += cc_ecpki_kg.c cc_ecdh.c mbedtls_cc_ecies.c ec_wrst_genkey.c
#ECC(Canonic) LLF
SOURCES_cc_312 += pki_modular_arithmetic.c
SOURCES_cc_312 += pka_ec_wrst.c pka_ec_wrst_dsa_verify.c
SOURCES_cc_312 += ec_wrst.c ec_wrst_dsa.c   cc_ecpki_domain.c
SOURCES_cc_312 +=  cc_ecpki_domain_secp192r1.c
SOURCES_cc_312 +=  cc_ecpki_domain_secp192k1.c cc_ecpki_domain_secp224r1.c cc_ecpki_domain_secp224k1.c cc_ecpki_domain_secp256r1.c
SOURCES_cc_312 +=  cc_ecpki_domain_secp256k1.c cc_ecpki_domain_secp384r1.c cc_ecpki_domain_secp521r1.c

#rng
SOURCES_cc_312 += cc_rnd_common.c llf_rnd.c cc_rng_plat.c cc_common_math.c cc_common_conv_endian.c
ifeq ($(CC_CONFIG_TRNG_MODE),0)
        # FE TRNG
        $(info FE TRNG: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
        SOURCES_cc_312 += llf_rnd_fetrng.c
	CFLAGS_EXTRA += -DCC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE)
else ifeq ($(CC_CONFIG_TRNG_MODE),1)
        # TRNG90B
        $(info TRNG90B: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
        SOURCES_cc_312 += llf_rnd_trng90b.c
	CFLAGS_EXTRA += -DCC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE)
else
        $(error illegal TRNG: CC_CONFIG_TRNG_MODE=$(CC_CONFIG_TRNG_MODE))
endif

# hash + hmac HW driver
SOURCES_cc_312 += hash_driver.c
CFLAGS += -DMD5_HASH_EXCLUDED
CFLAGS += -DCC_KEYGEN_MAX_SIZE=$(CC_RSA_MAX_KEY_GENERATION_SIZE_BITS)

ifeq ($(CC_CONFIG_SUPPORT_ECC_SCA_SW_PROTECT), 1)
	SOURCES_cc_312 += pka_ec_wrst_smul_scap.c
else
	SOURCES_cc_312 += pka_ec_wrst_smul_no_scap.c
endif


ifeq ($(LIB_PERF),1)
CFLAGS += -DLIB_PERF
endif

ifeq ($(DEBUG),1)
CFLAGS += -DDEBUG
endif

ifeq ($(PKA_DEBUG),1)
SOURCES_cc_312 += pki_dbg.c
CFLAGS += -DPKA_DEBUG
endif

# always suppose to support smaller PKA and have the IOT flag
CFLAGS_EXTRA += -DCC_IOT -DCC_SUPPORT_PKA_64_16


# Include directories
ifeq ($(TEE_OS),freertos)
include  $(HOST_SRCDIR)/../Makefile.freertos
endif

ifeq ($(TEE_OS),mbedos)
include  $(HOST_SRCDIR)/../Makefile.mbedos
endif

INCDIRS_EXTRA += $(HOST_SRCDIR)/pal/$(TEE_OS)
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api $(CODESAFE_DIR)/src/crypto_api/cc3x_sym/driver $(SHARED_DIR)/hw/include $(SHARED_INCDIR)/cc_util $(SHARED_INCDIR)/crypto_api/$(PROJ_PRD)
INCDIRS_EXTRA += $(SHARED_INCDIR) $(SHARED_INCDIR)/pal $(SHARED_INCDIR)/pal/$(TEE_OS) $(HOST_PROJ_ROOT)/src/hal $(HOST_PROJ_ROOT)/src/pal  $(HOST_PROJ_ROOT)/src/pal/$(TEE_OS)
INCDIRS_EXTRA += $(SHARED_INCDIR)/crypto_api
INCDIRS_EXTRA += $(HOST_INCDIR) $(CODESAFE_SRCDIR)/crypto_api/common $(SHARED_INCDIR)/trng/
#INCDIRS_EXTRA += $(HOST_SRCDIR)/softcrys/hash_common
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/rnd_dma $(CODESAFE_SRCDIR)/crypto_api/rnd_dma/local
#pka, rsa and ecc directories
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/pki/common $(CODESAFE_SRCDIR)/crypto_api/pki/ec_wrst $(CODESAFE_SRCDIR)/crypto_api/pki/rsa $(CODESAFE_SRCDIR)/crypto_api/pki/poly

# ffc_domain and ffcdh directories
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/ffc_domain $(CODESAFE_SRCDIR)/crypto_api/ffcdh

INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/rsa $(CODESAFE_SRCDIR)/crypto_api/ec_wrst $(CODESAFE_SRCDIR)/crypto_api/ec_wrst/ecc_domains

#mbedtls headers
INCDIRS_EXTRA += $(MBEDTLS_ROOT)/include/mbedtls $(MBEDTLS_ROOT)/include
INCDIRS_EXTRA += $(UTILS_API) $(MANAGEMENT_API)
INCDIRS_EXTRA += $(UTILS_API)
INCDIRS_EXTRA += $(MBEDTLS_ALT_DIR)

#SBROM
INCDIRS_EXTRA += $(SBROM_DIR)/secure_boot_gen $(SBROM_DIR)/common
INCDIRS_EXTRA += $(SBROM_DIR)/platform/common/cc3x $(SBROM_DIR)/platform/pal/cc3x $(SBROM_DIR)/platform/stage/rt/cc3x
INCDIRS_EXTRA += $(SBROM_DIR)/secure_debug/cc3x $(SBROM_DIR)/crypto_driver $(SBROM_DIR)/crypto_driver/reg
INCDIRS_EXTRA += $(SBROM_DIR)/cc3x_verifier $(HOST_SRCDIR)/cc3x_sbromlib
INCDIRS_EXTRA += $(SBROM_DIR)/platform/nvm $(SBROM_DIR)/platform/nvm/cc3x_nvm_rt

CFLAGS_EXTRA += -DDLLI_MAX_BUFF_SIZE=$(DLLI_MAX_BUFF_SIZE)
CFLAGS_EXTRA += -DFW_VER_MAJOR=$(FW_VER_MAJOR) -DFW_VER_MINOR=$(FW_VER_MINOR) -DFW_VER_PATCH=$(FW_VER_PATCH)
CFLAGS_EXTRA += -DCC_HW_VERSION=$(CC_HW_VERSION)

# define flag for non supported RND_DMA
ifeq ($(CC_CONFIG_RND_TEST_MODE),CC_RND_TEST_MODE)
CFLAGS_EXTRA += -DCC_RND_TEST_MODE
endif

ifeq ($(CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES),)
    #default
    CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES = 8192
endif

ifeq ($(CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES),)
    #default
    CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES = 512
endif

CFLAGS_EXTRA += -DCC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES=$(CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES)
CFLAGS_EXTRA += -DCC_MNG_MIN_BACKUP_SIZE_IN_BYTES=$(CC_CONFIG_MNG_MIN_BACKUP_SIZE_IN_BYTES)
CFLAGS_EXTRA += -DCC_SB_CERT_VERSION_MAJOR=$(CC_CONFIG_SB_CERT_VERSION_MAJOR)
CFLAGS_EXTRA += -DCC_SB_CERT_VERSION_MINOR=$(CC_CONFIG_SB_CERT_VERSION_MINOR)


# List of drivers to enable/disable
DRIVERS = AES
CFLAGS_EXTRA += $(foreach driver,$(DRIVERS),$(if $(FW_ENABLE_$(driver)_DRIVER),-DENABLE_$(driver)_DRIVER=$(FW_ENABLE_$(driver)_DRIVER)))


ifeq ($(CC_CONFIG_SUPPORT_SRP),1)
	PUBLIC_INCLUDES += $(CC_API_3x)/mbedtls_cc_srp.h $(CC_API_3x)/mbedtls_cc_srp_error.h
	VPATH += $(CODESAFE_SRCDIR)/crypto_api/pki/srp
	INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/pki/srp
	SOURCES_cc_312 += mbedtls_cc_srp.c srp_driver.c srp.c
endif

# EC Montgomery and EC Edwards includes and files

#EC_MONT

INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/ec_mont $(CODESAFE_SRCDIR)/crypto_api/pki/ec_mont
SOURCES_cc_312 += cc_ec_mont.c ec_mont.c pka_ec_mont.c
SOURCES_cc_312 += ec_mont_domain_curve25519.c
VPATH += $(CODESAFE_SRCDIR)/crypto_api/ec_mont $(CODESAFE_SRCDIR)/crypto_api/pki/ec_mont
#EC_EDW
INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crypto_api/ec_edw $(CODESAFE_SRCDIR)/crypto_api/pki/ec_edw
SOURCES_cc_312 += cc_ec_edw.c ec_edw.c pka_ec_edw.c
SOURCES_cc_312 += ec_edw_domain_25519.c
VPATH += $(CODESAFE_SRCDIR)/crypto_api/ec_edw $(CODESAFE_SRCDIR)/crypto_api/pki/ec_edw




# We should flatten the components source trees to avoid long search paths...
VPATH += $(HOST_SRCDIR)/hal/$(PROJ_PRD) $(UTILS_API) $(MANAGEMENT_API)
VPATH += $(CODESAFE_SRCDIR)/crypto_api/cc3x_sym/api $(CODESAFE_DIR)/src/crypto_api/cc3x_sym/driver $(CODESAFE_SRCDIR)/crypto_api/kdf $(CODESAFE_SRCDIR)/crypto_api/rnd_dma
VPATH += $(SHARED_SRCDIR)/proj/$(PROJ_PRD) $(CODESAFE_SRCDIR)/crypto_api/common
#pka, rsa, dh, ffcdh etc. directories
VPATH += $(CODESAFE_SRCDIR)/crypto_api/pki/common $(CODESAFE_SRCDIR)/crypto_api/pki/ec_wrst $(CODESAFE_SRCDIR)/crypto_api/pki/rsa
VPATH += $(CODESAFE_SRCDIR)/crypto_api/rsa/ $(CODESAFE_SRCDIR)/crypto_api/ec_wrst $(CODESAFE_SRCDIR)/crypto_api/ec_wrst/ecc_domains
VPATH += $(CODESAFE_SRCDIR)/crypto_api/dh $(CODESAFE_SRCDIR)/crypto_api/pki/poly
VPATH += $(CODESAFE_SRCDIR)/crypto_api/ffcdh $(CODESAFE_SRCDIR)/crypto_api/ffc_domain
VPATH += $(MBEDTLS_ALT_DIR)
#sbrom
VPATH += $(SBROM_DIR)/cc3x_verifier $(SBROM_DIR)/crypto_driver $(SBROM_DIR)/secure_boot_gen $(SBROM_DIR)/common
VPATH += $(SBROM_DIR)/platform/common/cc3x $(SBROM_DIR)/platform/nvm/cc3x_nvm_rt $(HOST_SRCDIR)/cc3x_sbromlib
VPATH += $(SBROM_DIR)/x509_cert_parser $(SBROM_DIR)/util $(SBROM_DIR)/platform/pal

# SCA protection flag
ifeq ($(CC_CONFIG_SUPPORT_ECC_SCA_SW_PROTECT), 1)
        CFLAGS_EXTRA += -DCC_SUPPORT_ECC_SCA_SW_PROTECT
endif

include $(HOST_PROJ_ROOT)/Makefile.rules

ifeq ($(DEBUG),1)
CFLAGS += -DTEST_DEBUG
endif

