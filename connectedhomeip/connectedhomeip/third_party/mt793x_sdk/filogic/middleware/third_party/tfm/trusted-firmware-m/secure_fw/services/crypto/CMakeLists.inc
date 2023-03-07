#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "crypto" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#       MBEDCRYPTO_INSTALL_DIR - directory where mbed-crypto headers and libraries can be found. Needed only when using CRYPTO_ENGINE_MBEDCRYPTO ON.
#       TFM_ROOT_DIR        - root directory of the TF-M repository.
#Outputs:
#       Will modify include directories to make the source compile.
#       ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#       Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(CRYPTO_DIR ${CMAKE_CURRENT_LIST_DIR})

#Check input variables
if (NOT DEFINED ENABLE_CRYPTO)
  message(FATAL_ERROR "Incomplete build configuration: ENABLE_CRYPTO is undefined. ")
endif()

if (NOT DEFINED CRYPTO_ENGINE_MBEDTLS)
  message(FATAL_ERROR "Incomplete build configuration: CRYPTO_ENGINE_MBEDTLS is undefined. ")
endif()

if (ENABLE_CRYPTO)
  if (CRYPTO_ENGINE_MBEDTLS)
    if (NOT DEFINED MBEDCRYPTO_INSTALL_DIR)
      message(FATAL_ERROR "Please set MBEDCRYPTO_INSTALL_DIR before including this file.")
    endif()
  endif()

  if (NOT DEFINED TFM_ROOT_DIR)
    message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
  endif()

  set (CRYPTO_C_SRC "${CRYPTO_DIR}/crypto_init.c"
                    "${CRYPTO_DIR}/crypto_alloc.c"
                    "${CRYPTO_DIR}/crypto_cipher.c"
                    "${CRYPTO_DIR}/crypto_hash.c"
                    "${CRYPTO_DIR}/crypto_mac.c"
                    "${CRYPTO_DIR}/crypto_key.c"
                    "${CRYPTO_DIR}/crypto_aead.c"
                    "${CRYPTO_DIR}/crypto_asymmetric.c"
                    "${CRYPTO_DIR}/crypto_key_derivation.c"
                    "${CRYPTO_DIR}/tfm_crypto_secure_api.c"
      )

  #Append all our source files to global lists.
  list(APPEND ALL_SRC_C ${CRYPTO_C_SRC})
  unset(CRYPTO_C_SRC)

  #Setting include directories
  embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
  embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
  embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/include ABSOLUTE)
  if (CRYPTO_ENGINE_MBEDTLS)
    embedded_include_directories(PATH ${MBEDCRYPTO_INSTALL_DIR}/include ABSOLUTE)
  endif()

  #Inform the user about Crypto service features selected based on the Crypto service cmake flags
  message("The Crypto service compile configuration is as follows:")
  if (NOT DEFINED CRYPTO_ENGINE_BUF_SIZE)
    message("- CRYPTO_ENGINE_BUF_SIZE using default value")
  else()
    message("- CRYPTO_ENGINE_BUF_SIZE: " ${CRYPTO_ENGINE_BUF_SIZE})
  endif()
  if (NOT DEFINED CRYPTO_CONC_OPER_NUM)
    message("- CRYPTO_CONC_OPER_NUM using default value")
  else()
    message("- CRYPTO_CONC_OPER_NUM: " ${CRYPTO_CONC_OPER_NUM})
  endif()

  if (NOT DEFINED CRYPTO_KEY_MODULE_DISABLED)
    message("- KEY module enabled")
    set(CRYPTO_KEY_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_KEY_MODULE_DISABLED: " ${CRYPTO_KEY_MODULE_DISABLED})
  endif()
  if (NOT CRYPTO_KEY_MODULE_DISABLED AND
      NOT TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
    message(FATAL_ERROR "Internal trusted storage should be enabled for persistent key storage")
  endif()

  if (NOT DEFINED CRYPTO_AEAD_MODULE_DISABLED)
    message("- AEAD module enabled")
    set(CRYPTO_AEAD_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_AEAD_MODULE_DISABLED: " ${CRYPTO_AEAD_MODULE_DISABLED})
  endif()
  if (NOT DEFINED CRYPTO_MAC_MODULE_DISABLED)
    message("- MAC module enabled")
    set(CRYPTO_MAC_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_MAC_MODULE_DISABLED: " ${CRYPTO_MAC_MODULE_DISABLED})
  endif()
  if (NOT DEFINED CRYPTO_HASH_MODULE_DISABLED)
    message("- HASH module enabled")
    set(CRYPTO_HASH_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_HASH_MODULE_DISABLED: " ${CRYPTO_HASH_MODULE_DISABLED})
  endif()
  if (NOT DEFINED CRYPTO_CIPHER_MODULE_DISABLED)
    message("- CIPHER module enabled")
    set(CRYPTO_CIPHER_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_CIPHER_MODULE_DISABLED: " ${CRYPTO_CIPHER_MODULE_DISABLED})
  endif()
  if (NOT DEFINED CRYPTO_KEY_DERIVATION_MODULE_DISABLED)
    message("- KEY_DERIVATION module enabled")
    set(CRYPTO_KEY_DERIVATION_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_KEY_DERIVATION_MODULE_DISABLED: " ${CRYPTO_KEY_DERIVATION_MODULE_DISABLED})
  endif()
  if (NOT DEFINED CRYPTO_ASYMMETRIC_MODULE_DISABLED)
    message("- ASYMMETRIC module enabled")
    set(CRYPTO_ASYMMETRIC_MODULE_DISABLED 0)
  else()
    message("- CRYPTO_ASYMMETRIC_MODULE_DISABLED: " ${CRYPTO_ASYMMETRIC_MODULE_DISABLED})
  endif()
  if (TFM_PSA_API)
    if (NOT DEFINED CRYPTO_IOVEC_BUFFER_SIZE)
      message("- CRYPTO_IOVEC_BUFFER_SIZE using default value")
    else()
      message("- CRYPTO_IOVEC_BUFFER_SIZE: " ${CRYPTO_IOVEC_BUFFER_SIZE})
    endif()
  endif()

else()
  message(FATAL_ERROR "Build system currently doesn't support selectively disabling of a service.")
endif()
