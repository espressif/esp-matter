/***************************************************************************//**
 * @file
 * @brief Network Crypto Sha1 Utility
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
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDES FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include "net_sha1_priv.h"
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (NET)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_NET

//                                                                 Define the SHA1 circular left shift macro.
#define NET_SHA1_CIRCULAR_SHIFT(bits, word)             (((word) << (bits)) | ((word) >> (32 - (bits))))

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPTES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetSHA1_PadMessage(NET_SHA1_CTX *p_ctx);

static void NetSHA1_ProcessMessageBlock(NET_SHA1_CTX *ptx);

/****************************************************************************************************//**
 *                                           NetSHA1_Reset()
 *
 * @brief    Initialize the NET_SHA1_CTX in preparation for computing a new SHA1 message digest.
 *
 * @param    p_ctx   Pointer to the SHA1 context.
 *
 *           p_err   Pointer to variable that will receive the return error code from this function :
 *                       - NET_SHA1_ERR_NONE            Operation is successful.
 *                       - NET_SHA1_ERR_PTR_NULL        The Context pointer is invalid.
 *
 * @return   DEF_OK, if the the context is properly reset.
 *           DEF_FAIL, if the operation failed.
 *******************************************************************************************************/
CPU_BOOLEAN NetSHA1_Reset(NET_SHA1_CTX *p_ctx,
                          RTOS_ERR     *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_ctx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_ctx->Length_Low = 0;
  p_ctx->Length_High = 0;
  p_ctx->Message_Block_Index = 0;

  p_ctx->Intermediate_Hash[0] = 0x67452301;
  p_ctx->Intermediate_Hash[1] = 0xEFCDAB89;
  p_ctx->Intermediate_Hash[2] = 0x98BADCFE;
  p_ctx->Intermediate_Hash[3] = 0x10325476;
  p_ctx->Intermediate_Hash[4] = 0xC3D2E1F0;

  p_ctx->Computed = DEF_NO;
  RTOS_ERR_SET(p_ctx->Corrupted, RTOS_ERR_NONE);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetSHA1_Result()
 *
 * Description : This function returns the 160-bit message digest into the Message_Digest array  provided
 *               by the caller.
 *
 * Argument(s) : p_ctx         Pointer to the SHA1 context.
 *
 *               p_msg_digest  Pointer to the buffer that receive the message digest.
 *
 *               p_err         Pointer to variable that will receive the return error code from this function :
 *
 *                                   NET_SHA1_ERR_NONE            Operation is successful.
 *                                   NET_SHA1_ERR_PTR_NULL        The Context pointer is invalid.
 *                                   NET_SHA1_ERR_CORRUPTION      Corruption is detected.
 *
 * Return(s)   : DEF_OK, if the the operation is successful.
 *               DEF_FAIL, if the operation failed.
 *
 * Caller(s)   : Application.
 *
 *               This function is a application programming interface (API) function & MAY be called
 *               by application function(s).
 *
 * Note(s)     : (1) The first octet of hash is stored in the 0th element, the last octet of hash in the
 *                   19th element.
 *******************************************************************************************************/
CPU_BOOLEAN NetSHA1_Result(NET_SHA1_CTX *p_ctx,
                           CPU_CHAR     *p_msg_digest,
                           RTOS_ERR     *p_err)
{
  int i;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_ctx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_msg_digest != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (RTOS_ERR_CODE_GET(p_ctx->Corrupted) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(p_ctx->Corrupted));
    return (DEF_FAIL);
  }

  if (p_ctx->Computed == DEF_NO) {
    NetSHA1_PadMessage(p_ctx);
    for (i = 0; i < 64; ++i) {
      p_ctx->Message_Block[i] = 0;
    }
    p_ctx->Length_Low = 0;
    p_ctx->Length_High = 0;
    p_ctx->Computed = 1;
  }

  for (i = 0; i < NET_SHA1_HASH_SIZE; ++i) {
    p_msg_digest[i] = p_ctx->Intermediate_Hash[i >> 2] >> 8 * (3 - (i & 0x03));
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetSHA1_Input()
 *
 * Description : This function accepts an array of octets as the next portion of the message.
 *
 * Argument(s) : p_ctx   Pointer to the SHA1 context.
 *
 *               p_msg   Pointer to an array of characters representing the next portion of the message.
 *
 *               len     Length of the message.
 *
 *               p_err   Pointer to variable that will receive the return error code from this function :
 *
 *                                   NET_SHA1_ERR_NONE            Operation is successful.
 *                                   NET_SHA1_ERR_PTR_NULL        The Context pointer is invalid.
 *                                   NET_SHA1_ERR_CORRUPTION      Corruption is detected.
 *
 * Return(s)   : DEF_OK, if the the operation is successful.
 *               DEF_FAIL, if the operation failed.
 *
 * Caller(s)   : Application.
 *
 *               This function is a application programming interface (API) function & MAY be called
   by application function(s).
 *
 * Note(s)     : none.
 *******************************************************************************************************/
CPU_BOOLEAN NetSHA1_Input(NET_SHA1_CTX   *p_ctx,
                          const CPU_CHAR *p_msg,
                          CPU_INT32U     len,
                          RTOS_ERR       *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_ctx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (len == 0u) {
    return (DEF_OK);
  }

  if (p_ctx->Computed == DEF_YES) {
    RTOS_ERR_SET(p_ctx->Corrupted, RTOS_ERR_INVALID_STATE);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return (DEF_FAIL);
  }

  if (RTOS_ERR_CODE_GET(p_ctx->Corrupted) != RTOS_ERR_NONE) {
    *p_err = p_ctx->Corrupted;
    return (DEF_FAIL);
  }

  while ((len-- != 0u) && (RTOS_ERR_CODE_GET(p_ctx->Corrupted) == RTOS_ERR_NONE)) {
    p_ctx->Message_Block[p_ctx->Message_Block_Index++] = *p_msg & 0xFF;
    p_ctx->Length_Low += 8;

    if (p_ctx->Length_Low == 0) {
      p_ctx->Length_High++;

      if (p_ctx->Length_High == 0) {
        RTOS_ERR_SET(p_ctx->Corrupted, RTOS_ERR_FAIL);
      }
    }

    if (p_ctx->Message_Block_Index == 64) {
      NetSHA1_ProcessMessageBlock(p_ctx);
    }

    p_msg++;
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetSHA1_ProcessMessageBlock()
 *
 * @brief    This function will process the next 512 bits of the message stored in the Message_Block array
 *
 * @param    p_ctx   Pointer to the SHA1 context.
 *
 *           NetSHA1_Input(),
 *           NetSHA1_PadMessage().
 *
 * @note         (1) Many of the variable names in this code, especially the single character names, were used
 *               because those were the names used in the publication.
 *******************************************************************************************************/
static void NetSHA1_ProcessMessageBlock(NET_SHA1_CTX *p_ctx)
{
  CPU_INT32U K[4];
  CPU_INT32U t;                                                 // Loop counter
  CPU_INT32U temp;                                              // Temporary word value
  CPU_INT32U W[80];                                             // Word sequence
  CPU_INT32U A, B, C, D, E;                                     // Word buffers

  //                                                               Constants defined in SHA-1
  K[0] = 0x5A827999;
  K[1] = 0x6ED9EBA1;
  K[2] = 0x8F1BBCDC;
  K[3] = 0xCA62C1D6;
  //                                                               Initialize the first 16 words in the array W
  for (t = 0; t < 16; t++) {
    W[t] = p_ctx->Message_Block[t * 4] << 24;
    W[t] |= p_ctx->Message_Block[t * 4 + 1] << 16;
    W[t] |= p_ctx->Message_Block[t * 4 + 2] << 8;
    W[t] |= p_ctx->Message_Block[t * 4 + 3];
  }

  for (t = 16; t < 80; t++) {
    W[t] = NET_SHA1_CIRCULAR_SHIFT(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
  }

  A = p_ctx->Intermediate_Hash[0];
  B = p_ctx->Intermediate_Hash[1];
  C = p_ctx->Intermediate_Hash[2];
  D = p_ctx->Intermediate_Hash[3];
  E = p_ctx->Intermediate_Hash[4];

  for (t = 0; t < 20; t++) {
    temp = NET_SHA1_CIRCULAR_SHIFT(5, A)
           + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
    E = D;
    D = C;
    C = NET_SHA1_CIRCULAR_SHIFT(30, B);
    B = A;
    A = temp;
  }

  for (t = 20; t < 40; t++) {
    temp = NET_SHA1_CIRCULAR_SHIFT(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
    E = D;
    D = C;
    C = NET_SHA1_CIRCULAR_SHIFT(30, B);
    B = A;
    A = temp;
  }

  for (t = 40; t < 60; t++) {
    temp = NET_SHA1_CIRCULAR_SHIFT(5, A)
           + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
    E = D;
    D = C;
    C = NET_SHA1_CIRCULAR_SHIFT(30, B);
    B = A;
    A = temp;
  }

  for (t = 60; t < 80; t++) {
    temp = NET_SHA1_CIRCULAR_SHIFT(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
    E = D;
    D = C;
    C = NET_SHA1_CIRCULAR_SHIFT(30, B);
    B = A;
    A = temp;
  }

  p_ctx->Intermediate_Hash[0] += A;
  p_ctx->Intermediate_Hash[1] += B;
  p_ctx->Intermediate_Hash[2] += C;
  p_ctx->Intermediate_Hash[3] += D;
  p_ctx->Intermediate_Hash[4] += E;

  p_ctx->Message_Block_Index = 0;
}

/****************************************************************************************************//**
 *                                           NetSHA1_PadMessage()
 *
 * @brief    Pad the message according at the SHA-1 standard.
 *
 * @param    p_ctx   Pointer to the SHA1 context.
 *
 * @return   NONE.
 *
 *           NetSHA1_Result().
 *
 * @note         (1) According to the standard, the message must be padded to an even 512 bits. The first
 *               padding bit must be a '1'.  The last 64 bits represent the length of the original
 *               message.  All bits in between should be 0.  This function will pad the message
 *               according to those rules by filling the Message_Block array accordingly.  It will
 *               also call the ProcessMessageBlock function provided appropriately.  When it returns,
 *               it can be assumed that the message digest has been computed.
 *******************************************************************************************************/
static void NetSHA1_PadMessage(NET_SHA1_CTX *p_ctx)
{
  //                                                               Check to see if the current message block is too...
  //                                                               ...small to hold the initial padding bits and length.

  if (p_ctx->Message_Block_Index > 55) {
    //                                                             If so, we will pad the block, process it, and...
    //                                                             ...then continue padding into a second block.
    p_ctx->Message_Block[p_ctx->Message_Block_Index++] = 0x80;

    while (p_ctx->Message_Block_Index < 64) {
      p_ctx->Message_Block[p_ctx->Message_Block_Index++] = 0;
    }

    NetSHA1_ProcessMessageBlock(p_ctx);

    while (p_ctx->Message_Block_Index < 56) {
      p_ctx->Message_Block[p_ctx->Message_Block_Index++] = 0;
    }
  } else {
    p_ctx->Message_Block[p_ctx->Message_Block_Index++] = 0x80;
    while (p_ctx->Message_Block_Index < 56) {
      p_ctx->Message_Block[p_ctx->Message_Block_Index++] = 0;
    }
  }

  //                                                               Store the message length as the last 8 octets.
  p_ctx->Message_Block[56] = p_ctx->Length_High >> 24;
  p_ctx->Message_Block[57] = p_ctx->Length_High >> 16;
  p_ctx->Message_Block[58] = p_ctx->Length_High >> 8;
  p_ctx->Message_Block[59] = p_ctx->Length_High;
  p_ctx->Message_Block[60] = p_ctx->Length_Low >> 24;
  p_ctx->Message_Block[61] = p_ctx->Length_Low >> 16;
  p_ctx->Message_Block[62] = p_ctx->Length_Low >> 8;
  p_ctx->Message_Block[63] = p_ctx->Length_Low;

  NetSHA1_ProcessMessageBlock(p_ctx);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
