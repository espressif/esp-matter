###################################################
# Sources
PKCS11_SRC      = middleware/third_party/pkcs11
CORE_PKCS11_SRC = $(PKCS11_SRC)/corePKCS11
PKCS11_PSA_SRC  = $(PKCS11_SRC)/freertos-pkcs11-psa
UNITY_SRC       = $(PKCS11_SRC)/UnityTesting
PKCS11_HELP_SRC = $(PKCS11_SRC)/pkcs11-helper

C_FILES += $(CORE_PKCS11_SRC)/source/core_pkcs11.c
C_FILES += $(CORE_PKCS11_SRC)/source/core_pki_utils.c
C_FILES += $(CORE_PKCS11_SRC)/source/dependency/3rdparty/mbedtls_utils/mbedtls_error.c
C_FILES += $(CORE_PKCS11_SRC)/source/dependency/3rdparty/mbedtls_utils/mbedtls_utils.c
C_FILES += $(PKCS11_PSA_SRC)/iot_pkcs11_psa.c
C_FILES += $(PKCS11_PSA_SRC)/iot_pkcs11_psa_input_format.c
C_FILES += $(PKCS11_PSA_SRC)/iot_pkcs11_psa_object_management.c
ifeq ($(MTK_CORE_PKCS11_SYSTEM_TESTS_ENABLE),y)
C_FILES += $(CORE_PKCS11_SRC)/test/system-test/system-tests/pkcs11_system_test.c
C_FILES += $(UNITY_SRC)/src/unity.c
endif

#################################################################################
# include path

# Let tfm include path before mbedtls to prevent using mbedtls psa headers
# FIX: the MbedTLS in secure/nonsecure should be the same
TFM_INC_CFLAGS := -I$(SOURCE_DIR)/middleware/third_party/tfm/trusted-firmware-m/interface/include $(CFLAGS)
CFLAGS  := $(TFM_INC_CFLAGS)
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/util/include
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/SourceV10/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/$(CORE_PKCS11_SRC)/config
CFLAGS  += -I$(SOURCE_DIR)/$(CORE_PKCS11_SRC)/test/shared-config
CFLAGS  += -I$(SOURCE_DIR)/$(CORE_PKCS11_SRC)/source/include
CFLAGS  += -I$(SOURCE_DIR)/$(CORE_PKCS11_SRC)/source/dependency/3rdparty/mbedtls_utils
CFLAGS  += -I$(SOURCE_DIR)/$(PKCS11_PSA_SRC)
CFLAGS  += -I$(SOURCE_DIR)/$(PKCS11_HELP_SRC)/include/pkcs11-helper-1.0

ifeq ($(MTK_CORE_PKCS11_SYSTEM_TESTS_ENABLE),y)
CFLAGS  += -I$(SOURCE_DIR)/$(CORE_PKCS11_SRC)/test/system-test/test-config/
CFLAGS  += -I$(SOURCE_DIR)/$(UNITY_SRC)/src
CFLAGS  += -DMTK_CORE_PKCS11_SYSTEM_TESTS_ENABLE
endif

