/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"

/** @brief Decrypt Data
 *
 * This function is called by the Gbz Message Controller plugin to decrypt a
 * ZCL payload. If the decryption is successful, the application is responsible
 * for allocating the memory for the new decrypted data and passing the pointer
 * via the "plainPayload" field as well as the plainPayloadLength field for the
 * length. The framework will be responsible for free-ing the allocated memory.
 * If the decryption fails, the callback return false and should not modify any
 * data.
 *
 * @param data   Ver.: always
 */
WEAK(void emberAfPluginGbzMessageControllerDecryptDataCallback(EmberAfGbzMessageData *data))
{
}

/** @brief Encrypt Data
 *
 * This function is called by the Gbz Message Controller plugin to encrypt a
 * ZCL payload. If the encryption is successful, the application is responsible
 * for allocating the memory for the new encrypted data and passing the pointer
 * via the "encryptedPayload" field as well as the encryptedPayloadLength field
 * for the length. The framework will be responsible for free-ing the allocated
 * memory. If the encryption fails, the callback return false and should not
 * modify any data.
 *
 * @param data   Ver.: always
 */
WEAK(void emberAfPluginGbzMessageControllerEncryptDataCallback(EmberAfGbzMessageData *data))
{
}
