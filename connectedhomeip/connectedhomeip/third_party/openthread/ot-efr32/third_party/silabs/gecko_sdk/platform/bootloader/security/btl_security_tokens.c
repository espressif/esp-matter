/***************************************************************************//**
 * @file
 * @brief Manufacturing token handling
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "btl_security_tokens.h"
#include "em_device.h"

#if defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES == 1)
extern const ApplicationProperties_t sl_app_properties;
#endif

const uint8_t* btl_getSignedBootloaderKeyXPtr(void)
{
#if defined(SEMAILBOX_PRESENT)
#if defined(BOOTLOADER_FALLBACK_LEGACY_KEY) && (BOOTLOADER_FALLBACK_LEGACY_KEY == 1)
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif
  uint8_t se_platform_pubKey[64];
  SE_Response_t ret = SE_readPubkey(SE_KEY_TYPE_BOOT, &se_platform_pubKey, 64, false);
  if (ret == SE_RESPONSE_INTERNAL_ERROR || ret == SE_RESPONSE_NOT_INITIALIZED) {
    return (const uint8_t*)(LOCKBITS_BASE + PUBKEY_OFFSET_X);
  } else {
    return NULL;
  }
#else
  return NULL;
#endif
// defined(SEMAILBOX_PRESENT)
#else
  return (const uint8_t*)(LOCKBITS_BASE + PUBKEY_OFFSET_X);
#endif
}

const uint8_t* btl_getSignedBootloaderKeyYPtr(void)
{
#if defined(SEMAILBOX_PRESENT)
#if defined(BOOTLOADER_FALLBACK_LEGACY_KEY) && (BOOTLOADER_FALLBACK_LEGACY_KEY == 1)
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif
  uint8_t se_platform_pubKey[64];
  SE_Response_t ret = SE_readPubkey(SE_KEY_TYPE_BOOT, &se_platform_pubKey, 64, false);
  if (ret == SE_RESPONSE_INTERNAL_ERROR || ret == SE_RESPONSE_NOT_INITIALIZED) {
    return (const uint8_t*)(LOCKBITS_BASE + PUBKEY_OFFSET_Y);
  } else {
    return NULL;
  }
#else
  return NULL;
#endif
// defined(SEMAILBOX_PRESENT)
#else
  return (const uint8_t*)(LOCKBITS_BASE + PUBKEY_OFFSET_Y);
#endif
}

const uint8_t* btl_getImageFileEncryptionKeyPtr(void)
{
#if defined(SEMAILBOX_PRESENT)                             \
  && defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE == 1)
  return NULL;
#elif defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_APP_PROPERTIES == 1)
  return sl_app_properties.decryptKey;
#else
  #if defined(BOOTLOADER_SECURE)
    #error "The GBL decryption key has to be either embedded into the Application Properties struct or stored in Secure Element"
  #endif

  return (const uint8_t*)(LOCKBITS_BASE + 0x286);
#endif
}
