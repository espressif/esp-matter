/***************************************************************************//**
 * @brief Mbeedtls context structs for Bluetooth
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef PROTOCOL_BLUETOOTH_BGCOMMON_INCLUDE_SL_BT_MBEDTLS_CONTEXT_H_
#define PROTOCOL_BLUETOOTH_BGCOMMON_INCLUDE_SL_BT_MBEDTLS_CONTEXT_H_
#include <stddef.h>
#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cipher.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"

size_t sl_bt_get_mbedtls_aes_ctx_size();
size_t sl_bt_get_mbedtls_cipher_ctx_size();
size_t sl_bt_get_mbedtls_crt_drbg_ctx_size();
size_t sl_bt_get_mbedtls_entropy_ctx_size();
size_t sl_bt_get_mbedtls_ccm_ctx_size();
size_t sl_bt_get_mbedtls_ecp_group_size();
size_t sl_bt_get_mbedtls_ecp_point_size();
size_t sl_bt_get_mbedtls_mpi_size();

#endif /* PROTOCOL_BLUETOOTH_BGCOMMON_INCLUDE_SL_BT_MBEDTLS_CONTEXT_H_ */
