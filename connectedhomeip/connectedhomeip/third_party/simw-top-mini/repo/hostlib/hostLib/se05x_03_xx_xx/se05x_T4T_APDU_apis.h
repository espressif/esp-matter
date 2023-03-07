/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if (SSS_HAVE_APPLET_SE051_H && SSS_HAVE_SE05X_VER_07_02)
/* OK */
#else
#error "Only with SE051_H based build"
#endif

/** Se05x_T4T_API_SelectT4TApplet
 *
 * See @ref Se05x_T4T_API_SelectT4TApplet.
 * Select T4T Applet.
 */
smStatus_t Se05x_T4T_API_SelectT4TApplet(pSe05xSession_t session_ctx);

/** Se05x_T4T_API_SelectFile
 *
 * See @ref Se05x_T4T_API_SelectFile.
 * Select File.
 */
smStatus_t Se05x_T4T_API_SelectFile(pSe05xSession_t session_ctx, uint8_t *fileId, size_t fileIDLen);

/** Se05x_T4T_API_ReadBinary
 *
 * See @ref Se05x_T4T_API_ReadBinary.
 * Read binary data from last selected file.
 */
smStatus_t Se05x_T4T_API_ReadBinary(pSe05xSession_t session_ctx, uint8_t *output, size_t *outlen);

/** Se05x_T4T_API_UpdateBinary
 *
 * See @ref Se05x_T4T_API_UpdateBinary.
 * Write binary data to last selected file.
 */
smStatus_t Se05x_T4T_API_UpdateBinary(pSe05xSession_t session_ctx, uint8_t *data, size_t dataLen);

/** Se05x_T4T_API_GetVersion
 *
 * See @ref Se05x_T4T_API_GetVersion.
 * Get T4T Applet version.
 */
smStatus_t Se05x_T4T_API_GetVersion(pSe05xSession_t session_ctx, uint8_t *version, size_t *versionLen);

/** Se05x_T4T_API_ManageReadCounter
 *
 * See @ref Se05x_T4T_API_ManageReadCounter.
 * Get Reset / Enable / Disable read counter.
 */
smStatus_t Se05x_T4T_API_ManageReadCounter(pSe05xSession_t session_ctx, SE05x_T4T_Read_Ctr_Operation_t readCtrOperation);

/** Se05x_T4T_API_ReadCounterValue
 *
 * See @ref Se05x_T4T_API_ReadCounterValue.
 * Read counter value.
 */
smStatus_t Se05x_T4T_API_ReadCounterValue(pSe05xSession_t session_ctx, uint8_t *ctrValue, size_t *ctrValueLen);

/** Se05x_T4T_API_ConfigureAccessCtrl
 *
 * See @ref Se05x_T4T_API_ConfigureAccessCtrl.
 * Configure contact / contactless access control options.
 */
smStatus_t Se05x_T4T_API_ConfigureAccessCtrl(pSe05xSession_t session_ctx,
    SE05x_T4T_Interface_Const_t seInterface,
    SE05x_T4T_Operation_Const_t operaion,
    SE05x_T4T_Access_Ctrl_t accessCtrl);

/** Se05x_T4T_API_ReadAccessCtrl
 *
 * See @ref Se05x_T4T_API_ReadAccessCtrl.
 * Read contact / contactless access control options.
 */
smStatus_t Se05x_T4T_API_ReadAccessCtrl(pSe05xSession_t session_ctx,
    SE05x_T4T_Interface_Const_t seInterface,
    SE05x_T4T_Access_Ctrl_t *readOperation,
    SE05x_T4T_Access_Ctrl_t *writeOperation);
