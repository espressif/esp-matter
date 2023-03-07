/*
 * Copyright 2020 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if (SSS_HAVE_APPLET_SE051_H && SSS_HAVE_SE05X_VER_07_02)
/* OK */
#else
#error "Only with SE051_H based build"
#endif

/** Se05x_API_PAKEConfigDevice
 *
 * See @ref Se05x_API_PAKEConfigDevice.
 * Called to Config the Device type.
 */
smStatus_t Se05x_API_PAKEConfigDevice(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, SE05x_SPAKE2PlusDeviceType_t deviceType);

/** Se05x_API_PAKEInitDevice
*
* See @ref Se05x_API_PAKEInitDevice.
* Used to Init the PAKE device type
*/
smStatus_t Se05x_API_PAKEInitDevice(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pContext,
    size_t contextLen,
    uint8_t *pIdProver,
    size_t idProverLen,
    uint8_t *pIdVerifier,
    size_t idVerifierLen);

/** Se05x_API_PAKEInitCredentials
*
* See @ref Se05x_API_PAKEInitCredentials.
* Used to Init the PAKE device credentials
*/
smStatus_t Se05x_API_PAKEInitCredentials(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint32_t objectID_w0,
    uint32_t objectID_w1,
    uint32_t objectID_L);

/** Se05x_API_PAKEComputeKeyShare
*
* See @ref Se05x_API_PAKEComputeKeyShare.
* Used to Compute the Key share of PAKE device type
*/
smStatus_t Se05x_API_PAKEComputeKeyShare(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pInKey,
    size_t inKeyLen,
    uint8_t *pShareKey,
    size_t *pShareKeyLen);

/** Se05x_API_PAKEComputeSessionKeys
*
* See @ref Se05x_API_PAKEComputeSessionKeys.
* Used to Compute PAKE device Session Keys
*/
smStatus_t Se05x_API_PAKEComputeSessionKeys(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pInKey,
    size_t inKeyLen,
    uint8_t *pShSecret,
    size_t *pShSecretLen,
    uint8_t *pKeyConfMessage,
    size_t *pkeyConfMessageLen);

/** Se05x_API_PAKEVerifySessionKeys
*
* See @ref Se05x_API_PAKEVerifySessionKeys.
* Used to Verify PAKE device Verify Session Keys
*/
smStatus_t Se05x_API_PAKEVerifySessionKeys(pSe05xSession_t session_ctx,
    SE05x_CryptoObjectID_t cryptoObjectID,
    uint8_t *pKeyConfMessage,
    size_t keyConfMessageLen,
    uint8_t *presult);

/** Se05x_API_PAKEReadDeviceType
*
* See @ref Se05x_API_PAKEReadDeviceType.
* Used to read the PAKE device type
*/
smStatus_t Se05x_API_PAKEReadDeviceType(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, SE05x_SPAKE2PlusDeviceType_t *deviceType);

/** Se05x_API_PAKEReadState
*
* See @ref Se05x_API_PAKEReadState.
* Used to Get Pake State
*/
smStatus_t Se05x_API_PAKEReadState(
    pSe05xSession_t session_ctx, SE05x_CryptoObjectID_t cryptoObjectID, SE05x_PAKEState_t *pakeState);

/** Se05x_API_ECDSA_Internal_Sign
*
* See @ref Se05x_API_ECDSA_Internal_Sign.
* Used to Perform ECDSA internal sign
* ECDSA sign is performed on concatenated data of BinaryFile Secure Object,
* The objectIDs of which are the contents of a BinaryFile Secure Object called tbsItemList.
* KeyPair used for Signing should be provided with POLICY_OBJ_INTERNAL_SIGN.
* This policy requires a 4-byte extension containing the identifier of the tbsItemList Secure Object
*/
smStatus_t Se05x_API_ECDSA_Internal_Sign(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_ECSignatureAlgo_t ecSignAlgo,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *hashData,
    size_t *phashDataLen);

/** Se05x_API_RSA_Internal_Sign
*
* See @ref Se05x_API_RSA_Internal_Sign.
* Used to Perform RSA internal sign
* RSA sign is performed on concatenated data of BinaryFile Secure Object,
* The objectIDs of which are the contents of a BinaryFile Secure Object called tbsItemList.
* KeyPair used for Signing should be provided with POLICY_OBJ_INTERNAL_SIGN.
* This policy requires a 4-byte extension containing the identifier of the tbsItemList Secure Object
*/
smStatus_t Se05x_API_RSA_Internal_Sign(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_RSASignatureAlgo_t rsaSigningAlgo,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *hashData,
    size_t *phashDataLen);

/** Se05x_API_EdDSA_Internal_Sign
*
* See @ref Se05x_API_EdDSA_Internal_Sign.
* Used to Perform EDDSA internal sign
* EDDSA sign is performed on concatenated data of BinaryFile Secure Object,
* The objectIDs of which are the contents of a BinaryFile Secure Object called tbsItemList.
* KeyPair used for Signing should be provided with POLICY_OBJ_INTERNAL_SIGN.
* This policy requires a 4-byte extension containing the identifier of the tbsItemList Secure Object
*/
smStatus_t Se05x_API_EdDSA_Internal_Sign(pSe05xSession_t session_ctx,
    uint32_t objectID,
    SE05x_EDSignatureAlgo_t edSignAlgo,
    uint8_t *signature,
    size_t *psignatureLen,
    uint8_t *hashData,
    size_t *phashDataLen);
