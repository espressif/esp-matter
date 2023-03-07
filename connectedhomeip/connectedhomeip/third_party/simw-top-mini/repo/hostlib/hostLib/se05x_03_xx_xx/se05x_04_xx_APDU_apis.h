/*
 *
 * Copyright 2019 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#if SSS_HAVE_SE05X_VER_GTE_06_00
/* OK */
#else
#error "Only with SE051 based build"
#endif

/** @addtogroup se05x_apis
 *
 * @{ */

/** Se05x_API_AeadOneShot
 *
 * Authenticated encryption or decryption with associated data in one shot mode.
 *
 * The key object must be either an AES key or DES key.
 *
 * The AEADOneShot command returns the computed GMAC (when P2 equals
 * P2_ENCRYPT_ONESHOT) or indicates whether the GMAC is correct (when P2 equals
 * P2_DECRYPT_ONESHOT). The length of the GMAC is always 16 bytes when P2 equals
 * P2_ENCRYPT_ONESHOT.
 *
 * When P2 equals P2_DECRYPT_ONESHOT:
 *
 *   * the minimum tag length to pass is 4 bytes.
 *
 *   * when the GMAC tag is not correct, only the result will be returned, no output data will be present.
 *
 * Note: on applet v4.4.0, the maximum lengths are not yet enforced and might
 * differ from the values listed in the C-APDU.
 *
 * # Command to Applet
 *
 * @rst
 * +---------+---------------------------+------------------------------------------------+
 * | Field   | Value                     | Description                                    |
 * +=========+===========================+================================================+
 * | CLA     | 0x80                      |                                                |
 * +---------+---------------------------+------------------------------------------------+
 * | INS     | INS_CRYPTO                | :cpp:type:`SE05x_INS_t`                        |
 * +---------+---------------------------+------------------------------------------------+
 * | P1      | P1_AEAD or P1_AEAD_INT_IV | See :cpp:type:`SE05x_P1_t`                     |
 * +---------+---------------------------+------------------------------------------------+
 * | P2      | P2_ENCRYPT_ONESHOT or     | See :cpp:type:`SE05x_P2_t`                     |
 * |         | P2_DECRYPT_ONESHOT        |                                                |
 * +---------+---------------------------+------------------------------------------------+
 * | Lc      | #(Payload)                |                                                |
 * +---------+---------------------------+------------------------------------------------+
 * | Payload | TLV[TAG_1]                | 4-byte identifier of the AESKey Secure object. |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_2]                | 1-byte AEADMode                                |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_3]                | Byte array containing input data. Maximum      |
 * |         |                           | length = 256 bytes.   [Optional]               |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_4]                | Byte array containing Additional Authenticated |
 * |         |                           | Data. Maximum length = 64 bytes.   [Optional]  |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_5]                | Byte array containing an initialization vector |
 * |         |                           | (if P1 equals P1_AEAD) or 2-byte  value        |
 * |         |                           | containing the initialization vector length    |
 * |         |                           | (if P1 equals P1_AEAD_SP800_108).  Maximum IV  |
 * |         |                           | length = 60 bytes.   [Optional]                |
 * |         |                           | [Conditional: required when P1 equals          |
 * |         |                           | P1_AEAD_INT_IV]                                |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_6]                | Byte array containing the GMAC tag to verify.  |
 * |         |                           | [Conditional: when P2 equals                   |
 * |         |                           | P2_DECRYPT_ONESHOT]                            |
 * +---------+---------------------------+------------------------------------------------+
 * | Le      | 0x00                      | Expecting return data.                         |
 * +---------+---------------------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 * @rst
 * +------------+----------------------------------------------+
 * | Value      | Description                                  |
 * +============+==============================================+
 * | TLV[TAG_1] | Byte array containing output data.           |
 * +------------+----------------------------------------------+
 * | TLV[TAG_2] | Byte array containing tag (if P2 =           |
 * |            | P2_ENCRYPT_ONESHOT) or byte array containing |
 * |            | Result  (if P2 = P2_DECRYPT_ONESHOT)         |
 * +------------+----------------------------------------------+
 * | TLV[TAG_3] | Byte array containing the initialization     |
 * |            | vector (if P1 = P1_AEAD_INT_IV and P2 =      |
 * |            | P2_ENCRYPT_ONESHOT).                         |
 * +------------+----------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  objectID        The object id
 * @param[in]  cipherMode      The cipher mode
 * @param[in]  inputData       The input data
 * @param[in]  inputDataLen    The input data length
 * @param[in]  aad             The aad
 * @param[in]  aadLen          The aad length
 * @param[in]  IV              The iv
 * @param[in]  IVLen           The iv length
 * @param      tagData         The tag data
 * @param      tagDataLen      The tag data length
 * @param      outputData      The output data
 * @param      poutputDataLen  The poutput data length
 * @param[in]  operation       The operation
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadOneShot(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_CipherMode_t cipherMode,
    const uint8_t *inputData,
    size_t inputDataLen,
    const uint8_t *aad,
    size_t aadLen,
    uint8_t *IV,
    size_t IVLen,
    uint8_t *tagData,
    size_t *tagDataLen,
    uint8_t *outputData,
    size_t *poutputDataLen,
    const SE05x_Cipher_Oper_OneShot_t operation);

/** Se05x_API_AeadInit
 *
 * Initialize an authentication encryption or decryption with associated data.
 * The Crypto Object keeps the state of the AEAD operation until it's finalized
 * or deleted. Once the AEADFinal function is executed successfully, the Crypto
 * Object state returns to the state immediately after the previous AEADInit
 * function.
 *
 * When P1 equals P1_AEAD_INT_IV and P2 equals P1_ENCRYPT, TLV[TAG_5] must
 * includes the length of the initialization vector. In that case, the
 * initialization vector is generated internally and passed back in the response
 * command. When the device is in FIPS mode (see FIPS Compliance), P1 equal to
 * P1_AEAD will result in SW_CONDITIONS_NOT_SATISFIED.
 *
 * # Command to Applet
 *
 * @rst
 * +---------+---------------------------+------------------------------------------------+
 * | Field   | Value                     | Description                                    |
 * +=========+===========================+================================================+
 * | CLA     | 0x80                      |                                                |
 * +---------+---------------------------+------------------------------------------------+
 * | INS     | INS_CRYPTO                | :cpp:type:`SE05x_INS_t`                        |
 * +---------+---------------------------+------------------------------------------------+
 * | P1      | P1_AEAD or P1_AEAD_INT_IV | See :cpp:type:`SE05x_P1_t`                     |
 * +---------+---------------------------+------------------------------------------------+
 * | P2      | P2_ENCRYPT or P2_DECRYPT  | See :cpp:type:`SE05x_P2_t`                     |
 * +---------+---------------------------+------------------------------------------------+
 * | Lc      | #(Payload)                |                                                |
 * +---------+---------------------------+------------------------------------------------+
 * | Payload | TLV[TAG_1]                | 4-byte identifier of the AESKey Secure object. |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_2]                | 2-byte Crypto Object identifier                |
 * +---------+---------------------------+------------------------------------------------+
 * |         | TLV[TAG_5]                | Byte array containing the initialization       |
 * |         |                           | vector (if P1 equals P1_AEAD or P1 equals      |
 * |         |                           | P1_AEAD and P2 equals P2_DECRYPT) or 2-byte    |
 * |         |                           | value containing the initialization vector     |
 * |         |                           | length (if P1 equals  P1_AEAD_INT_IV and P2    |
 * |         |                           | equals P2_ENCRYPT)   [Optional]                |
 * |         |                           | [Conditional: required when P1 equals          |
 * |         |                           | P1_AEAD_INT_IV and P2 equals P2_ENCRYPT]       |
 * +---------+---------------------------+------------------------------------------------+
 * | Le      | -                         |                                                |
 * +---------+---------------------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 * @rst
 * +------------+-----------------------------------------------+
 * | Value      | Description                                   |
 * +============+===============================================+
 * | TLV[TAG_3] | Byte array containing the used initialization |
 * |            | vector. It remains valid until  deselect,     |
 * |            | AEADInit, AEADFinal or AEADOneShot is called. |
 * |            | [Conditional: Only when P1 equals             |
 * |            | P1_AEAD_INT_IV and P2 equals P2_ENCRYPT]      |
 * +------------+-----------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  objectID        The object id
 * @param[in]  cryptoObjectID  The crypto object id
 * @param[in]  pIV             { parameter_description }
 * @param[in]  IVLen           The iv length
 * @param[in]  operation       The operation
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadInit(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_CipherMode_t cipherMode,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pIV,
    size_t IVLen,
    const SE05x_Cipher_Oper_t operation);

/** Se05x_API_AeadCCMInit
 *
 * Initialize an authentication encryption or decryption with associated data.
 * The Crypto Object keeps the state of the AEAD operation until it's finalized
 * or deleted. Once the AEADFinal function is executed successfully, the Crypto
 * Object state returns to the state immediately after the previous AEADInit
 * function.AEAD in CCM mode.
 *
 * # Command to Applet
 *
 * @rst
 * +---------+--------------------------+------------------------------------------------+
 * | Field   | Value                    | Description                                    |
 * +=========+==========================+================================================+
 * | CLA     | 0x80                     |                                                |
 * +---------+--------------------------+------------------------------------------------+
 * | INS     | INS_CRYPTO               | :cpp:type:`SE05x_INS_t`                        |
 * +---------+--------------------------+------------------------------------------------+
 * | P1      | P1_AEAD                  | See :cpp:type:`SE05x_P1_t`                     |
 * +---------+--------------------------+------------------------------------------------+
 * | P2      | P2_ENCRYPT or P2_DECRYPT | See :cpp:type:`SE05x_P2_t`                     |
 * +---------+--------------------------+------------------------------------------------+
 * | Lc      | #(Payload)               |                                                |
 * +---------+--------------------------+------------------------------------------------+
 * | Payload | TLV[TAG_1]               | 4-byte identifier of the AESKey Secure object. |
 * +---------+--------------------------+------------------------------------------------+
 * |         | TLV[TAG_2]               | 2-byte Crypto Object identifier                |
 * +---------+--------------------------+------------------------------------------------+
 * |         | TLV[TAG_5]               | Byte array containing the initialization       |
 * |         |                          | vector [12 bytes until 60 bytes] or a 2-byte   |
 * |         |                          | value containing the initialization vector     |
 * |         |                          | length, depending on the AEADMode of the       |
 * |         |                          | Crypto Object.                                 |
 * +---------+--------------------------+------------------------------------------------+
 * |         | TLV[TAG_6]               | Byte array containing 2-byte AAD length.       |
 * |         |                          | [Conditional: needed if AEADMode equals        |
 * |         |                          | AES_CCM]                                       |
 * +---------+--------------------------+------------------------------------------------+
 * |         | TLV[TAG_7]               | Byte array containing 2-byte message length.   |
 * |         |                          | [Conditional: needed if AEADMode equals        |
 * |         |                          | AES_CCM]                                       |
 * +---------+--------------------------+------------------------------------------------+
 * |         | TLV[TAG_8]               | Byte array containing 2-byte tag size.         |
 * |         |                          | [Conditional: needed if AEADMode equals        |
 * |         |                          | AES_CCM].                                      |
 * +---------+--------------------------+------------------------------------------------+
 * | Le      | -                        |                                                |
 * +---------+--------------------------+------------------------------------------------+
 * @endrst
 *
 *
 * # R-APDU Body
 *  NA
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  objectID        The object id
 * @param[in]  cryptoObjectID  The crypto object id
 * @param[in]  pIV             { parameter_description }
 * @param[in]  IVLen           The iv length
 * @param[in]  aadLen          The aad length
 * @param[in]  payloadLen      The payloadLen length
 * @param[in]  tagLen          The tag length
 * @param[in]  operation       The operation
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadCCMInit(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_CipherMode_t cipherMode,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pIV,
    size_t IVLen,
    size_t aadLen,
    size_t payloadLen,
    size_t tagLen,
    const SE05x_Cipher_Oper_t operation);

/**
 * @brief      Se05x_API_AeadCCMLastUpdate
 *
 * # Command to Applet
 *
 * @rst
 * +------------+---------------+------------------------------------------------+
 * | Field      | Value         | Description                                    |
 * +============+===============+================================================+
 * | CLA        | 0x80          |                                                |
 * +------------+---------------+------------------------------------------------+
 * | INS        | INS_CRYPTO    | :cpp:type:`SE05x_INS_t`                        |
 * +------------+---------------+------------------------------------------------+
 * | P1         | P1_AEAD       | See :cpp:type:`SE05x_P1_t`                     |
 * +------------+---------------+------------------------------------------------+
 * | P2         | P2_UPDATE     | See :cpp:type:`SE05x_P2_t`                     |
 * +------------+---------------+------------------------------------------------+
 * | Lc         | #(Payload)    |                                                |
 * +------------+---------------+------------------------------------------------+
 * | Payload    | TLV[TAG_2]    | 2-byte Crypto Object identifier                |
 * +------------+---------------+------------------------------------------------+
 * |            | TLV[TAG_3]    | Byte array containing input data               |
 * |            |               | [Conditional: only when                        |
 * |            |               | TLV[TAG_4] is not present]                     |
 * |            |               | [Optional]                                     |
 * +------------+---------------+------------------------------------------------+
 * | Le         | 0x00          | Expecting returned data.                       |
 * +------------+---------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 * NA
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  cryptoObjectID  The crypto object id
 * @param[in]  pInputData      The input data
 * @param[in]  inputDataLen    The input data length
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadCCMLastUpdate(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, const uint8_t *pInputData, size_t inputDataLen);

/** Se05x_API_AeadCCMFinal
 *
 * Finish a sequence of AES_CCM AEAD operations.
 *
 * # Command to Applet
 *
 * @rst
 * +------------+----------------------------------+---------------------------------+
 * | Field      | Value                            | Description                     |
 * +============+==================================+=================================+
 * | CLA        | 0x80                             |                                 |
 * +------------+----------------------------------+---------------------------------+
 * | INS        | INS_CRYPTO                       | :cpp:type:`SE05x_INS_t`         |
 * +------------+----------------------------------+---------------------------------+
 * | P1         | P1_AEAD                          | See :cpp:type:`SE05x_P1_t`      |
 * +------------+----------------------------------+---------------------------------+
 * | P2         | P2_FINAL                         | See :cpp:type:`SE05x_P2_t`      |
 * +------------+----------------------------------+---------------------------------+
 * | Lc         | #(Payload)                       |                                 |
 * +------------+----------------------------------+---------------------------------+
 * | Payload    | TLV[TAG_2]                       | 2-byte Crypto Object identifier |
 * +------------+----------------------------------+---------------------------------+
 * | TLV[TAG_6] | Byte array containing tag to     |                                 |
 * |            | verify   [Conditional] When the  |                                 |
 * |            | mode is decrypt and verify (i.e. |                                 |
 * |            | AEADInit has been called  with   |                                 |
 * |            | P2 = P2_DECRYPT).                |                                 |
 * +------------+----------------------------------+---------------------------------+
 * | Le         | 0x00                             | Expected returned data.         |
 * +------------+----------------------------------+---------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 * @rst
 * +------------+------------------------------------------------+
 * | Value      | Description                                    |
 * +============+================================================+
 * | TLV[TAG_1] | Output data                                    |
 * |            |                                                |
 * +------------+------------------------------------------------+
 * | TLV[TAG_2] | Byte array containing tag (if P2 = P2_ENCRYPT) |
 * |            | or byte array containing Result (if P2  =      |
 * |            | P2_DECRYPT)                                    |
 * +------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  cryptoObjectID  The crypto object id
 * @param[out] pOutputData     The output data
 * @param[out] pOutputLen      The output length
 * @param      tag             The tag
 * @param      tagLen          The tag length
 * @param[in]  operation       The operation
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadCCMFinal(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pOutputData,
    size_t *pOutputLen,
    uint8_t *pTag,
    size_t *pTagLen,
    const SE05x_Cipher_Oper_t operation);

/** Se05x_API_AeadUpdate_aad
 *
 * Update a Crypto Object of type CC_AEAD.
 *
 * The user either needs to send input data or Additional Authenticated Data
 * (AAD), but not both at once.
 *
 * # Command to Applet
 *
 * @rst
 * +------------+----------------------------------+------------------------------------------------+
 * | Field      | Value                            | Description                                    |
 * +============+==================================+================================================+
 * | CLA        | 0x80                             |                                                |
 * +------------+----------------------------------+------------------------------------------------+
 * | INS        | INS_CRYPTO                       | :cpp:type:`SE05x_INS_t`                        |
 * +------------+----------------------------------+------------------------------------------------+
 * | P1         | P1_AEAD                          | See :cpp:type:`SE05x_P1_t`                     |
 * +------------+----------------------------------+------------------------------------------------+
 * | P2         | P2_UPDATE                        | See :cpp:type:`SE05x_P2_t`                     |
 * +------------+----------------------------------+------------------------------------------------+
 * | Lc         | #(Payload)                       |                                                |
 * +------------+----------------------------------+------------------------------------------------+
 * | Payload    | TLV[TAG_2]                       | 2-byte Crypto Object identifier                |
 * +------------+----------------------------------+------------------------------------------------+
 * |            | TLV[TAG_4]                       | Byte array containing Additional Authenticated |
 * |            |                                  | Data.   [Conditional: only when TLV[TAG_3] is  |
 * |            |                                  | not present]   [Optional]                      |
 * +------------+----------------------------------+------------------------------------------------+
 * | Le         | 0x00                             | Expecting returned data.                       |
 * +------------+----------------------------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 * NA
 *
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  cryptoObjectID  The crypto object id
 * @param[in]  pAadData        The aad data
 * @param[in]  aadDataLen      The aad data length
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadUpdate_aad(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, const uint8_t *pAadData, size_t aadDataLen);

/**
 * @brief      Se05x_API_AeadUpdate
 *
 * # Command to Applet
 *
 * @rst
 * +------------+---------------+------------------------------------------------+
 * | Field      | Value         | Description                                    |
 * +============+===============+================================================+
 * | CLA        | 0x80          |                                                |
 * +------------+---------------+------------------------------------------------+
 * | INS        | INS_CRYPTO    | :cpp:type:`SE05x_INS_t`                        |
 * +------------+---------------+------------------------------------------------+
 * | P1         | P1_AEAD       | See :cpp:type:`SE05x_P1_t`                     |
 * +------------+---------------+------------------------------------------------+
 * | P2         | P2_UPDATE     | See :cpp:type:`SE05x_P2_t`                     |
 * +------------+---------------+------------------------------------------------+
 * | Lc         | #(Payload)    |                                                |
 * +------------+---------------+------------------------------------------------+
 * | Payload    | TLV[TAG_2]    | 2-byte Crypto Object identifier                |
 * +------------+---------------+------------------------------------------------+
 * |            | TLV[TAG_3]    | Byte array containing input data               |
 * |            |               | [Conditional: only when                        |
 * |            |               | TLV[TAG_4] is not present]                     |
 * |            |               | [Optional]                                     |
 * +------------+---------------+------------------------------------------------+
 * | Le         | 0x00          | Expecting returned data.                       |
 * +------------+---------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 * @rst
 * +------------+---------------------------------------+
 * | Value      | Description                           |
 * +============+=======================================+
 * | TLV[TAG_1] | Output data   [Conditional: only when |
 * |            | TLV[TAG_3] is passed as input]        |
 * +------------+---------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  cryptoObjectID  The crypto object id
 * @param[in]  pInputData      The input data
 * @param[in]  inputDataLen    The input data length
 * @param      pOutputData     The output data
 * @param      pOutputLen      The output length
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadUpdate(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    const uint8_t *pInputData,
    size_t inputDataLen,
    uint8_t *pOutputData,
    size_t *pOutputLen);

/** Se05x_API_AeadFinal
 *
 * Finish a sequence of AEAD operations. The AEADFinal command provides the
 * computed GMAC or indicates whether the GMAC is correct depending on the P2
 * parameters passed during AEADInit. The length of the GMAC is always 16 bytes
 * when P2 equals P2_ENCRYPT. When P2 equals P2_DECRYPT, the minimum tag length
 * to pass is 4 bytes.
 *
 * # Command to Applet
 *
 * @rst
 * +------------+----------------------------------+---------------------------------+
 * | Field      | Value                            | Description                     |
 * +============+==================================+=================================+
 * | CLA        | 0x80                             |                                 |
 * +------------+----------------------------------+---------------------------------+
 * | INS        | INS_CRYPTO                       | :cpp:type:`SE05x_INS_t`         |
 * +------------+----------------------------------+---------------------------------+
 * | P1         | P1_AEAD                          | See :cpp:type:`SE05x_P1_t`      |
 * +------------+----------------------------------+---------------------------------+
 * | P2         | P2_FINAL                         | See :cpp:type:`SE05x_P2_t`      |
 * +------------+----------------------------------+---------------------------------+
 * | Lc         | #(Payload)                       |                                 |
 * +------------+----------------------------------+---------------------------------+
 * | Payload    | TLV[TAG_2]                       | 2-byte Crypto Object identifier |
 * +------------+----------------------------------+---------------------------------+
 * | TLV[TAG_6] | Byte array containing tag to     |                                 |
 * |            | verify   [Conditional] When the  |                                 |
 * |            | mode is decrypt and verify (i.e. |                                 |
 * |            | AEADInit has been called  with   |                                 |
 * |            | P2 = P2_DECRYPT).                |                                 |
 * +------------+----------------------------------+---------------------------------+
 * | Le         | 0x00                             | Expected returned data.         |
 * +------------+----------------------------------+---------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 * @rst
 * +------------+------------------------------------------------+
 * | Value      | Description                                    |
 * +============+================================================+
 * | TLV[TAG_2] | Byte array containing tag (if P2 = P2_ENCRYPT) |
 * |            | or byte array containing Result (if P2  =      |
 * |            | P2_DECRYPT)                                    |
 * +------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx     The session context
 * @param[in]  cryptoObjectID  The crypto object id
 * @param      tag             The tag
 * @param      tagLen          The tag length
 * @param[in]  operation       The operation
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_AeadFinal(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *tag,
    size_t *tagLen,
    const SE05x_Cipher_Oper_t operation);

/** Se05x_API_DisableObjCreation
 *
 *
 * # Command to Applet
 *
 *
 * # R-APDU Body
 *
 * NA
 *
 * # R-APDU Trailer
 *
 *
 *
 *
 * @param[in] session_ctx Session Context [0:kSE05x_pSession]
 * @param[in] lockIndicator [1:kSE05x_TAG_1]
 * @param[in] restrictMode [2:kSE05x_TAG_2]
 */
smStatus_t Se05x_API_DisableObjCreation(
    pSe05xSession_t session_ctx, SE05x_LockIndicator_t lockIndicator, SE05x_RestrictMode_t restrictMode);

/** Se05x_API_ReadObjectAttributes
 *
 *
 *
 * @param[in] session_ctx Session Context [0:kSE05x_pSession]
 * @param[in] objectID object id [1:kSE05x_TAG_1]
 * @param[out] data  [0:kSE05x_TAG_2]
 * @param[in,out] pdataLen Length for data
 */
smStatus_t Se05x_API_ReadObjectAttributes(
    pSe05xSession_t session_ctx, uint32_t objectID, uint8_t *data, size_t *pdataLen);

/** Se05x_API_TriggerSelfTest
 *
 * Trigger a system health check for the system. When calling this command, a self-test is
 * triggered in the operating system. When the test fails, the device might not respond with
 * a R-APDU as the chip is reset.
 * If HealthCheckMode is set to HCM_FIPS, the test will only work if the device is running in
 * FIPS approved mode of operation.
 *
 * # Command to Applet
 *
 *
 * @rst
 * +------------+---------------------------------+------------------------------------------------+
 * | Field      | Value                           | Description                                    |
 * +============+=================================+================================================+
 * | CLA        | 0x80                            |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | INS        | INS_MGMT                        | See :cpp:type:`SE05x_INS_t`. In addition to    |
 * |            |                                 | INS_CRYPTO, users  can set the INS_ATTEST      |
 * |            |                                 | flag. In that case, attestation applies.       |
 * +------------+---------------------------------+------------------------------------------------+
 * | P1         | P1_DEFAULT                      | See :cpp:type:`SE05x_P1_t`                     |
 * +------------+---------------------------------+------------------------------------------------+
 * | P2         | P2_SANITY                       | See :cpp:type:`SE05x_P2_t`                     |
 * +------------+---------------------------------+------------------------------------------------+
 * | Lc         | #(Payload)                      | Payload length                                 |
 * +------------+---------------------------------+------------------------------------------------+
 * | Payload    | TLV[TAG_1]                      | 2-byte value from HealthCheckMode              |
 * +------------+---------------------------------+------------------------------------------------+
 * | Le         | 0x00                            | 2-byte response + attested data (if INS_ATTEST |
 * |            |                                 | is set).                                       |
 * +------------+---------------------------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 *
 * @rst
 * +------------+------------------------------------------------+
 * | Value      | Description                                    |
 * +============+================================================+
 * | TLV[TAG_1] | TLV containing 1-byte Result.                  |
 * +------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx      The session context
 * @param[in]  HealthCheckMode  The health check mode
 * @param      result           The result of Self Test
 *
 * @return     The sm status.
 */

smStatus_t Se05x_API_TriggerSelfTest(
    pSe05xSession_t session_ctx, SE05x_HealthCheckMode_t healthCheckMode, uint8_t *result);

#if SSS_HAVE_SE05X_VER_GTE_07_02
/** Se05x_API_TriggerSelfTest_W_Attst
 *
 * Trigger a system health check for the system. When calling this command, a self-test is
 * triggered in the operating system. When the test fails, the device might not respond with
 * a R-APDU as the chip is reset.
 * If HealthCheckMode is set to HCM_FIPS, the test will only work if the device is running in
 * FIPS approved mode of operation.
 *
 * # Command to Applet
 *
 *
 * @rst
 * +------------+---------------------------------+------------------------------------------------+
 * | Field      | Value                           | Description                                    |
 * +============+=================================+================================================+
 * | CLA        | 0x80                            |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | INS        | INS_MGMT                        | See :cpp:type:`SE05x_INS_t`. In addition to    |
 * |            |                                 | INS_CRYPTO, users  can set the INS_ATTEST      |
 * |            |                                 | flag. In that case, attestation applies.       |
 * +------------+---------------------------------+------------------------------------------------+
 * | P1         | P1_DEFAULT                      | See :cpp:type:`SE05x_P1_t`                     |
 * +------------+---------------------------------+------------------------------------------------+
 * | P2         | P2_SANITY                       | See :cpp:type:`SE05x_P2_t`                     |
 * +------------+---------------------------------+------------------------------------------------+
 * | Lc         | #(Payload)                      | Payload length                                 |
 * +------------+---------------------------------+------------------------------------------------+
 * | Payload    | TLV[TAG_1]                      | 2-byte value from HealthCheckMode              |
 * +------------+---------------------------------+------------------------------------------------+
 * | TLV[TAG_5] | 4-byte attestation object       |                                                |
 * |            | identifier.   [Optional]        |                                                |
 * |            | [Conditional: only when         |                                                |
 * |            | INS_ATTEST is set]              |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | TLV[TAG_6] | 1-byte AttestationAlgo          |                                                |
 * |            | [Optional]   [Conditional: only |                                                |
 * |            | when INS_ATTEST is set]         |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | TLV[TAG_7] | 16-byte freshness random        |                                                |
 * |            | [Optional]   [Conditional: only |                                                |
 * |            | when INS_ATTEST is set]         |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | Le         | 0x00                            | 2-byte response + attested data (if INS_ATTEST |
 * |            |                                 | is set).                                       |
 * +------------+---------------------------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 *
 * @rst
 * +------------+------------------------------------------------+
 * | Value      | Description                                    |
 * +============+================================================+
 * | TLV[TAG_1] | TLV containing 1-byte Result.                  |
 * +------------+------------------------------------------------+
 * | TLV[TAG_3] | TLV containing 12-byte timestamp               |
 * |            | [Conditional: only when C-APDU contains        |
 * |            | INS_ATTEST]                                    |
 * +------------+------------------------------------------------+
 * | TLV[TAG_4] | TLV containing 16-byte freshness (random)      |
 * |            | [Conditional: only when C-APDU contains        |
 * |            | INS_ATTEST]                                    |
 * +------------+------------------------------------------------+
 * | TLV[TAG_5] | TLV containing 18-byte chip unique ID          |
 * |            | [Conditional: only when C-APDU contains        |
 * |            | INS_ATTEST]                                    |
 * +------------+------------------------------------------------+
 * | TLV[TAG_6] | TLV containing signature over the concatenated |
 * |            | values of TLV[TAG_1], TLV[TAG_3],  TLV[TAG_4]  |
 * |            | and TLV[TAG_5].   [Conditional: only when      |
 * |            | C-APDU contains INS_ATTEST]                    |
 * +------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx      The session context
 * @param[in]  HealthCheckMode  The health check mode
 * @param[in]  attestID         The attest id
 * @param[in]  attestAlgo       The attest algorithm
 * @param[in]  random           The random
 * @param[in]  randomLen        The random length
 * @param      result           The result of Self Test
 * @param      ptimeStamp       The ptime stamp
 * @param      outrandom        The outrandom
 * @param      poutrandomLen    The poutrandom length
 * @param      chipId           The chip identifier
 * @param      pchipIdLen       The pchip identifier length
 * @param      signature        The signature
 * @param      psignatureLen    The psignature length
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_TriggerSelfTest_W_Attst(pSe05xSession_t session_ctx,
    SE05x_HealthCheckMode_t healthCheckMode,
    uint32_t attestID,
    SE05x_AttestationAlgo_t attestAlgo,
    const uint8_t *random,
    size_t randomLen,
    uint8_t *result,
    SE05x_TimeStamp_t *ptimeStamp,
    uint8_t *chipId,
    size_t *pchipIdLen,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *pObjectSize,
    size_t *pObjectSizeLen,
    uint8_t *pCmd,
    size_t *pCmdLen);
#else
/** Se05x_API_TriggerSelfTest_W_Attst
 *
 * Trigger a system health check for the system. When calling this command, a self-test is
 * triggered in the operating system. When the test fails, the device might not respond with
 * a R-APDU as the chip is reset.
 * If HealthCheckMode is set to HCM_FIPS, the test will only work if the device is running in
 * FIPS approved mode of operation.
 *
 * # Command to Applet
 *
 *
 * @rst
 * +------------+---------------------------------+------------------------------------------------+
 * | Field      | Value                           | Description                                    |
 * +============+=================================+================================================+
 * | CLA        | 0x80                            |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | INS        | INS_MGMT                        | See :cpp:type:`SE05x_INS_t`. In addition to    |
 * |            |                                 | INS_CRYPTO, users  can set the INS_ATTEST      |
 * |            |                                 | flag. In that case, attestation applies.       |
 * +------------+---------------------------------+------------------------------------------------+
 * | P1         | P1_DEFAULT                      | See :cpp:type:`SE05x_P1_t`                     |
 * +------------+---------------------------------+------------------------------------------------+
 * | P2         | P2_SANITY                       | See :cpp:type:`SE05x_P2_t`                     |
 * +------------+---------------------------------+------------------------------------------------+
 * | Lc         | #(Payload)                      | Payload length                                 |
 * +------------+---------------------------------+------------------------------------------------+
 * | Payload    | TLV[TAG_1]                      | 2-byte value from HealthCheckMode              |
 * +------------+---------------------------------+------------------------------------------------+
 * | TLV[TAG_5] | 4-byte attestation object       |                                                |
 * |            | identifier.   [Optional]        |                                                |
 * |            | [Conditional: only when         |                                                |
 * |            | INS_ATTEST is set]              |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | TLV[TAG_6] | 1-byte AttestationAlgo          |                                                |
 * |            | [Optional]   [Conditional: only |                                                |
 * |            | when INS_ATTEST is set]         |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | TLV[TAG_7] | 16-byte freshness random        |                                                |
 * |            | [Optional]   [Conditional: only |                                                |
 * |            | when INS_ATTEST is set]         |                                                |
 * +------------+---------------------------------+------------------------------------------------+
 * | Le         | 0x00                            | 2-byte response + attested data (if INS_ATTEST |
 * |            |                                 | is set).                                       |
 * +------------+---------------------------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Body
 *
 *
 * @rst
 * +------------+------------------------------------------------+
 * | Value      | Description                                    |
 * +============+================================================+
 * | TLV[TAG_1] | TLV containing 1-byte Result.                  |
 * +------------+------------------------------------------------+
 * | TLV[TAG_3] | TLV containing 12-byte timestamp               |
 * |            | [Conditional: only when C-APDU contains        |
 * |            | INS_ATTEST]                                    |
 * +------------+------------------------------------------------+
 * | TLV[TAG_4] | TLV containing 16-byte freshness (random)      |
 * |            | [Conditional: only when C-APDU contains        |
 * |            | INS_ATTEST]                                    |
 * +------------+------------------------------------------------+
 * | TLV[TAG_5] | TLV containing 18-byte chip unique ID          |
 * |            | [Conditional: only when C-APDU contains        |
 * |            | INS_ATTEST]                                    |
 * +------------+------------------------------------------------+
 * | TLV[TAG_6] | TLV containing signature over the concatenated |
 * |            | values of TLV[TAG_1], TLV[TAG_3],  TLV[TAG_4]  |
 * |            | and TLV[TAG_5].   [Conditional: only when      |
 * |            | C-APDU contains INS_ATTEST]                    |
 * +------------+------------------------------------------------+
 * @endrst
 *
 * # R-APDU Trailer
 *
 *
 * @rst
 * +-------------+--------------------------------------+
 * | SW          | Description                          |
 * +=============+======================================+
 * | SW_NO_ERROR | The command is handled successfully. |
 * +-------------+--------------------------------------+
 * @endrst
 *
 *
 * @param[in]  session_ctx      The session context
 * @param[in]  HealthCheckMode  The health check mode
 * @param[in]  attestID         The attest id
 * @param[in]  attestAlgo       The attest algorithm
 * @param[in]  random           The random
 * @param[in]  randomLen        The random length
 * @param      result           The result of Self Test
 * @param      ptimeStamp       The ptime stamp
 * @param      outrandom        The outrandom
 * @param      poutrandomLen    The poutrandom length
 * @param      chipId           The chip identifier
 * @param      pchipIdLen       The pchip identifier length
 * @param      signature        The signature
 * @param      psignatureLen    The psignature length
 *
 * @return     The sm status.
 */
smStatus_t Se05x_API_TriggerSelfTest_W_Attst(pSe05xSession_t session_ctx,
    SE05x_HealthCheckMode_t healthCheckMode,
    uint32_t attestID,
    SE05x_AttestationAlgo_t attestAlgo,
    const uint8_t *random,
    size_t randomLen,
    uint8_t *result,
    SE05x_TimeStamp_t *ptimeStamp,
    uint8_t *outrandom,
    size_t *poutrandomLen,
    uint8_t *chipId,
    size_t *pchipIdLen,
    uint8_t *signature,
    size_t *psignatureLen);
#endif // SSS_HAVE_SE05X_VER_GTE_07_02

/** Se05x_API_ECDHGenerateSharedSecret_InObject
 *
 * See @ref Se05x_API_ECDHGenerateSharedSecret
 *
 */
smStatus_t Se05x_API_ECDHGenerateSharedSecret_InObject(pSe05xSession_t session_ctx,
    uint32_t objectID,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    uint32_t sharedSecretID,
    uint8_t invertEndianness);

/** Se05x_API_TLSCalculateRsaPreMasterSecret
 *
 * @param[in] session_ctx Session Context[0:kSE05x_pSession]
 * @param[in] keyPairId keyPairId[1:kSE05x_TAG_1]
 * @param[in] pskId pskId[2:kSE05x_TAG_2]
 * @param[in] hmacKeyId hmacKeyId[3:kSE05x_TAG_3]
 * @param[in] inputData inputData[4:kSE05x_TAG_4]
 * @param[in] inputDataLen Length of inputData
 * @param[in] clientVersion client version[6:kSE05x_TAG_6]
 * @param[in] clientVersionLen Length of client version
 *
 */

smStatus_t Se05x_API_TLSCalculateRsaPreMasterSecret(pSe05xSession_t session_ctx,
    uint32_t keyPairId,
    uint32_t pskId,
    uint32_t hmacKeyId,
    const uint8_t *inputData,
    size_t inputDataLen,
    const uint8_t *clientVersion,
    size_t clientVersionLen);

/** Se05x_API_WriteRSAKey_Ver
*
* See @ref Se05x_API_WriteRSAKey. Also allows to set key version (4 bytes).
*
*/
smStatus_t Se05x_API_WriteRSAKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    uint32_t version);

/** Se05x_API_UpdateRSAKey_Ver
*
* See @ref Se05x_API_WriteRSAKey. Also allows to set key version (4 bytes).
* Called to update the value of already existing object. If policy is passed,
* it should match with existing policy on object.
*/
smStatus_t Se05x_API_UpdateRSAKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t size,
    const uint8_t *p,
    size_t pLen,
    const uint8_t *q,
    size_t qLen,
    const uint8_t *dp,
    size_t dpLen,
    const uint8_t *dq,
    size_t dqLen,
    const uint8_t *qInv,
    size_t qInvLen,
    const uint8_t *pubExp,
    size_t pubExpLen,
    const uint8_t *priv,
    size_t privLen,
    const uint8_t *pubMod,
    size_t pubModLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    const SE05x_RSAKeyFormat_t rsa_format,
    uint32_t version);

#define Se05x_API_WriteECKey_with_version Se05x_API_WriteECKey_Ver
/** Se05x_API_WriteECKey_Ver
*
* See @ref Se05x_API_WriteECKey. Also allows to set key version (4 bytes).
*
*/
smStatus_t Se05x_API_WriteECKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    uint32_t version);

/** Se05x_API_UpdateECKey_Ver
*
* See @ref Se05x_API_WriteECKey. Also allows to set key version (4 bytes).
* Called to update the value of already existing object. If policy is passed,
* it should match with existing policy on object.
*/
smStatus_t Se05x_API_UpdateECKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part,
    uint32_t version);

/** Se05x_API_WriteSymmKey_Ver
*
* See @ref Se05x_API_WriteSymmKey. Also allows to set key version (4 bytes).
*
*/
smStatus_t Se05x_API_WriteSymmKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version);

/** Se05x_API_UpdateSymmKey_Ver
*
* See @ref Se05x_API_WriteSymmKey. Also allows to set key version (4 bytes).
* Called to update the value of already existing object. If policy is passed,
* it should match with existing policy on object.
*/
smStatus_t Se05x_API_UpdateSymmKey_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version);

/** Se05x_API_WriteBinary_Ver
*
* See @ref Se05x_API_WriteBinary. Also allows to set key version (4 bytes).
*
*/
smStatus_t Se05x_API_WriteBinary_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t offset,
    uint16_t length,
    const uint8_t *inputData,
    size_t inputDataLen,
    uint32_t version);

/** Se05x_API_UpdateBinary_Ver
*
* See @ref Se05x_API_WriteBinary. Also allows to set key version (4 bytes).
* Called to update the value of already existing object. If policy is passed,
* it should match with existing policy on object.
*
*/
smStatus_t Se05x_API_UpdateBinary_Ver(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t objectID,
    uint16_t offset,
    uint16_t length,
    const uint8_t *inputData,
    size_t inputDataLen,
    uint32_t version);

/** Se05x_API_ReadState
*
*
* # Command to Applet
*
*
* # R-APDU Body
*
* NA
*
* # R-APDU Trailer
*
*
*
*
* @param[in] session_ctx Session Context [0:kSE05x_pSession]
* @param[out] pstateValues [1:kSE05x_TAG_1]
*/
smStatus_t Se05x_API_ReadState(pSe05xSession_t session_ctx, uint8_t *pstateValues, size_t *pstateValuesLen);

/** Se05x_API_GetExtVersion
*
* Gets the applet extended version information.
*
* This will return 37-byte VersionInfo (including major, minor and patch version
* of the applet, supported applet features and secure box version).
*
* # Command to Applet
*
* @rst
* +-------+------------------------------+----------------------------------------------+
* | Field | Value                        | Description                                  |
* +=======+==============================+==============================================+
* | CLA   | 0x80                         |                                              |
* +-------+------------------------------+----------------------------------------------+
* | INS   | INS_MGMT                     | See :cpp:type:`SE05x_INS_t`                  |
* +-------+------------------------------+----------------------------------------------+
* | P1    | P1_DEFAULT                   | See :cpp:type:`SE05x_P1_t`                   |
* +-------+------------------------------+----------------------------------------------+
* | P2    | P2_VERSION or P2_VERSION_EXT | See :cpp:type:`SE05x_P2_t`                   |
* +-------+------------------------------+----------------------------------------------+
* | Lc    | #(Payload)                   |                                              |
* +-------+------------------------------+----------------------------------------------+
* | Le    | 0x00                         | Expecting TLV with 7-byte data  (when P2 =   |
* |       |                              | P2_VERSION) or a TLV with 37 byte data (when |
* |       |                              | P2=  P2_VERSION_EXT).                        |
* +-------+------------------------------+----------------------------------------------+
* @endrst
*
*
* # R-APDU Body
*
* @rst
* +------------+------------------------------------------------+
* | Value      | Description                                    |
* +============+================================================+
* | TLV[TAG_1] | 7-byte :cpp:type:`VersionInfoRef` (if P2 =     |
* |            | P2_VERSION) or 7-byte  VersionInfo followed by |
* |            | 30 bytes extendedFeatureBits (if P2 =          |
* |            | P2_VERSION_EXT)                                |
* +------------+------------------------------------------------+
* @endrst
*
* # R-APDU Trailer
*
* @rst
* +-------------+--------------------------------+
* | SW          | Description                    |
* +=============+================================+
* | SW_NO_ERROR | Data is returned successfully. |
* +-------------+--------------------------------+
* @endrst
*
* @param[in]  session_ctx       The session context
* @param      pappletVersion    The papplet version
* @param      appletVersionLen  The applet version length
*
* @return     The sm status.
*/
smStatus_t Se05x_API_GetExtVersion(pSe05xSession_t session_ctx, uint8_t *pappletVersion, size_t *appletVersionLen);

/**Se05x_API_SendCardManagerCmd
*
* Sends a command to the Card Manager.
*
* This APDU will send command to Card Manager
*
* # Command to Card Manager
*
* @rst
* +---------+---------------+--------------------------------------+
* | Field   | Value         | Description                          |
* +=========+===============+======================================+
* | CLA     | 0x80          |                                      |
* +---------+---------------+--------------------------------------+
* | INS     | INS_MGMT      | See :cpp:type:`SE05x_INS_t`          |
* +---------+---------------+--------------------------------------+
* | P1      | P1_DEFAULT    | See :cpp:type:`SE05x_P1_t`           |
* +---------+---------------+--------------------------------------+
* | P2      | P2_CM_COMMAND | See :cpp:type:`SE05x_P2_t`           |
* +---------+---------------+--------------------------------------+
* | Lc      | #(Payload)    | Payload length                       |
* +---------+---------------+--------------------------------------+
* | Payload | TLV[TAG_1]    | APDU to be sent to the Card Manager. |
* +---------+---------------+--------------------------------------+
* | Le      | 0x00          | Expected response length             |
* +---------+---------------+--------------------------------------+
* @endrst
*
* # R-APDU Body
*
* @rst
* +------------+----------------------------------------+
* | Value      | Description                            |
* +============+========================================+
* | TLV[TAG_1] | Byte array containing the Card Manager |
* |            | response.                              |
* +------------+----------------------------------------+
* @endrst
*
* # R-APDU Trailer
*
* @rst
* +-------------+--------------------------------------+
* | SW          | Description                          |
* +=============+======================================+
* | SW_NO_ERROR | The command is handled successfully. |
* +-------------+--------------------------------------+
* @endrst
*
* @param[in]  session_ctx  The session context
* @param[in]  pCmdData     The command input data
* @param[in]  cmdDataLen   The command input data length
* @param[out]  pOutputData     The response  data
* @param[out]  pOutputDataLen   The response data length
*
* @return     The sm status.
*/
smStatus_t Se05x_API_SendCardManagerCmd(
    pSe05xSession_t session_ctx, uint8_t *pCmdData, size_t cmdDataLen, uint8_t *pOutputData, size_t *pOutputDataLen);

/** Se05x_API_UpdatePCR
*
* See @ref Se05x_API_WritePCR.
* Called to update the value of already existing object. If policy is passed,
* it should match with existing policy on object.
*/
smStatus_t Se05x_API_UpdatePCR(
    pSe05xSession_t session_ctx, pSe05xPolicy_t policy, uint32_t pcrID, const uint8_t *inputData, size_t inputDataLen);

/** Se05x_API_UpdateCounter
*
* See @ref Se05x_API_SetCounterValue.
* Called to update the value of already existing object. If policy is passed,
* it should match with existing policy on object.
*/
smStatus_t Se05x_API_UpdateCounter(
    pSe05xSession_t session_ctx, pSe05xPolicy_t policy, uint32_t objectID, uint16_t size, uint64_t value);

/** Se05x_API_WriteSymmKey_Ver_extended
*
* See @ref Extension of Se05x_API_WriteSymmKey_Ver api. Allows to set minimum tag length for AEAD (2 bytes).
*
*/
smStatus_t Se05x_API_WriteSymmKey_Ver_extended(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type,
    uint32_t version,
    uint16_t min_aead_tag_len);

#if SSS_HAVE_SE05X_VER_GTE_07_02 || defined(__DOXYGEN__)

/** Se05x_API_PBKDF2_extended
*
* See @ref Se05x_API_PBKDF2_extended.
* New PBKDF2 api with optional salt object id and optional derived Session key id.
* This api also supports additional mac algorithms.
* @param[in]  session_ctx                  The session context
* @param[in]  objectID                     HMAC key object id
* @param[in]  salt                         Salt data
* @param[in]  saltLen                      Salt length
* @param[in]  saltID                       Object id with salt data
* @param[in]  macAlgo                      MAC Algorithm
* @param[in]  requestedLen                 Requested derived session key length
* @param[in, out]  derivedSessionKeyID     HMAC object id to store output derived session key
* @param[in, out]  derivedSessionKey       Buffer to store derived session key on host
* @param[in, out]  pderivedSessionKeyLen   DerivedSessionKey buffer length
*
*/
smStatus_t Se05x_API_PBKDF2_extended(pSe05xSession_t session_ctx,
    uint32_t objectID,
    const uint8_t *salt,
    size_t saltLen,
    uint32_t saltID,
    uint16_t count,
    SE05x_MACAlgo_t macAlgo,
    uint16_t requestedLen,
    uint32_t derivedSessionKeyID,
    uint8_t *derivedSessionKey,
    size_t *pderivedSessionKeyLen);

/** Se05x_API_ECDHGenerateSharedSecret_InObject_extended
*
* See @ref Se05x_API_ECDHGenerateSharedSecret_InObject_extended.
* New ECDH api with support for ECDH algo input (EC_SVDP_DH and EC_SVDP_DH_PLAIN).
* @param[in]  session_ctx                  The session context
* @param[in]  objectID                     Private key or key pair identifier
* @param[in]  pubKey                       External EC public key
* @param[in]  pubKeyLen                    External EC public key length
* @param[in]  ecdhAlgo                     ECDH Algorithm
* @param[in]  sharedSecretID               Identifier to store derived key
* @param[in]  invertEndianness             Option to invert endianness of derived key
*
*/
smStatus_t Se05x_API_ECDHGenerateSharedSecret_InObject_extended(pSe05xSession_t session_ctx,
    uint32_t objectID,
    const uint8_t *pubKey,
    size_t pubKeyLen,
    SE05x_ECDHAlgo_t ecdhAlgo,
    uint32_t sharedSecretID,
    uint8_t invertEndianness);

smStatus_t Se05x_API_ECPointMultiply_InputObj(pSe05xSession_t session_ctx,
    uint32_t objectID,
    uint32_t pubKeyID,
    uint32_t sharedSecretID,
    uint8_t *sharedSecretOuput,
    size_t *psharedSecretOuputLen,
    SE05x_ECPMAlgo_t ECPMAlgo);
#endif

/** @} */
