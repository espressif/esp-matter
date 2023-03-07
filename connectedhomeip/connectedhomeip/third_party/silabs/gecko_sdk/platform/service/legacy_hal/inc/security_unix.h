/***************************************************************************//**
 * @file
 * @brief This file implements the low-level routines for encrypting using AES.
 * These are normally provided by the chip on our real hardware.
 * For simulation we can either use real encryption, pseudo encryption,
 * or no encryption. This gives us more flexibility in debugging.
 * We use the Rijndael encryption algorithm to do the work for
 * real encryption.
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
#define pseudoEncryptOffset 0x55
#define SECURITY_M    0
#define SECURITY_NONCE_TOTAL_SIZE 0

#define pseudoEncryptOffset 0x55

extern uint8_t emSecurityKey[];

extern uint8_t standardMic[];
typedef enum {
  USE_STANDARD_MIC      = 0,
  USE_TRANSIENT_KEY_MIC = 1,
} PseudoEncryptMicType;

// These routines use PSEUDO Encryption
Parcel *encryptParcel(Parcel *clearText);
Parcel *encryptParcelWithSpecialMic(Parcel *clearText, PseudoEncryptMicType type);
Parcel *encryptParcelNoMic(Parcel *clearText);
Parcel *constructMicParcel(PseudoEncryptMicType type);

#define standardMicParcel() \
  constructMicParcel(USE_STANDARD_MIC)
#define transientLinkKeyMicParcel() \
  constructMicParcel(USE_TRANSIENT_KEY_MIC)

#if defined EMBER_TEST

enum {
  USE_REAL_ENCRYPTION   = 0,
  USE_PSEUDO_ENCRYPTION = 1,
  USE_NO_ENCRYPTION     = 2,
};

typedef uint8_t SimulatorEncryption;

extern SimulatorEncryption simulatorEncryptionType;

void emSetSimulatorEncryption(SimulatorEncryption type);

#endif

#ifndef EMBER_STACK_CONNECT
// These are only used in zigbee.  They cause a compile error
// in flex now that we removed the legacy buffer system header.
bool emPseudoEncryptPacket(PacketHeader *header,
                           uint8_t authenticationStartOffset,
                           uint8_t encryptionStartOffset);

bool emPseudoDecryptPacket(PacketHeader header,
                           uint8_t authenticationStartOffset,
                           uint8_t encryptionStartOffset);

void emPseudoEncryptFlatPacket(uint8_t *packet,
                               uint8_t length,
                               uint8_t authenticationStartOffset,
                               uint8_t encryptionStartOffset);

extern bool emPseudoEncryptUseTransientLinkKey;
#endif
