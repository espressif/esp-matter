/***************************************************************************//**
 * @file
 * @brief Network Base64 Library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 * @note   (1) No compiler-supplied standard library functions are used by the network protocol suite.
 *             'net_util.*' implements ALL network-specific library functions.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_BASE64_PRIV_H_
#define  _NET_BASE64_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/source/tcpip/net_priv.h>
#include  <common/include/lib_mem.h>

/********************************************************************************************************
 *                                       BASE 64 ENCODER DEFINES
 *
 * Note(s) : (1) The size of the output buffer the base 64 encoder produces is typically bigger than the
 *               input buffer by a factor of (4 x 3).  However, when padding is necessary, up to 3
 *               additional characters could by appended.  Finally, one more character is used to NULL
 *               terminate the buffer.
 *******************************************************************************************************/

#define  NET_BASE64_ENCODER_OCTETS_IN_GRP                3
#define  NET_BASE64_ENCODER_OCTETS_OUT_GRP               4

#define  NET_BASE64_DECODER_OCTETS_IN_GRP                4
#define  NET_BASE64_DECODER_OCTETS_OUT_GRP               3

#define  NET_BASE64_ENCODER_PAD_CHAR                    '='
//                                                                 See Note #1.
#define  NET_BASE64_ENCODER_OUT_MAX_LEN(length)         (((length / 3) * 4) + ((length % 3) == 0 ? 0 : 4) + 1)

void NetBase64_Encode(CPU_CHAR   *pin_buf,
                      CPU_INT16U in_len,
                      CPU_CHAR   *pout_buf,
                      CPU_INT16U out_len,
                      RTOS_ERR   *p_err);

void NetBase64_Decode(CPU_CHAR   *pin_buf,
                      CPU_INT16U in_len,
                      CPU_CHAR   *pout_buf,
                      CPU_INT16U out_len,
                      RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_BASE64_PRIV_H_
