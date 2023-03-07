/***************************************************************************//**
 * @file
 * @brief Stub version of the eccapi.asm file.  It returns only
 * failure.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "eccapi.h"

int ZSE_ECCGenerateKey(unsigned char *privateKey,
                       unsigned char *publicKey,
                       GetRandomDataFunc *GetRandomData,
                       YieldFunc *yield,
                       unsigned long yieldLevel)
{
  (void)privateKey;
  (void)publicKey;
  (void)GetRandomData;
  (void)yield;
  (void)yieldLevel;
  return MCE_LIBRARY_NOT_PRESENT;
}

int ZSE_ECCKeyBitGenerate(unsigned char *privateKey,
                          unsigned char *ephemeralPrivateKey,
                          unsigned char *ephemeralPublicKey,
                          unsigned char *remoteCertificate,
                          unsigned char *remoteEphemeralPublicKey,
                          unsigned char *caPublicKey,
                          unsigned char *keyBits,
                          HashFunc *Hash,
                          YieldFunc *yield,
                          unsigned long yieldLevel)
{
  (void)privateKey;
  (void)ephemeralPrivateKey;
  (void)ephemeralPublicKey;
  (void)remoteCertificate;
  (void)remoteEphemeralPublicKey;
  (void)caPublicKey;
  (void)keyBits;
  (void)Hash;
  (void)yield;
  (void)yieldLevel;
  return MCE_LIBRARY_NOT_PRESENT;
}

int ZSE_ECDSASign(unsigned char *privateKey, unsigned char *msgDigest,
                  GetRandomDataFunc *GetRandomData,
                  unsigned char *r, unsigned char *s,
                  YieldFunc *yield, unsigned long yieldLevel)
{
  (void)privateKey;
  (void)msgDigest;
  (void)GetRandomData;
  (void)r;
  (void)s;
  (void)yield;
  (void)yieldLevel;
  return MCE_LIBRARY_NOT_PRESENT;
}

int ZSE_ECDSAVerify(unsigned char *msgDigest, unsigned char *publicKey,
                    unsigned char *r, unsigned char *s,
                    YieldFunc *yield, unsigned long yieldLevel)
{
  (void)msgDigest;
  (void)publicKey;
  (void)r;
  (void)s;
  (void)yield;
  (void)yieldLevel;
  return MCE_LIBRARY_NOT_PRESENT;
}

int ZSE_ECQVReconstructPublicKey(unsigned char* certificate,
                                 unsigned char* caPublicKey,
                                 unsigned char* publicKey,
                                 HashFunc *Hash,
                                 YieldFunc *yield, unsigned long yieldLevel)
{
  (void)certificate;
  (void)caPublicKey;
  (void)publicKey;
  (void)Hash;
  (void)yield;
  (void)yieldLevel;
  return MCE_LIBRARY_NOT_PRESENT;
}

unsigned long ZSE_GetVersion(void)
{
  return STUB_ECC_LIBRARY;
}
