/***************************************************************************//**
 * @file
 * @brief Declarations for CBKE command handlers.
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

// -----------------------------------------------------------------------------
// Ezsp Command Handlers

EmberStatus emberAfEzspDsaSignCommandCallback(uint8_t messageLength,
                                              uint8_t* message);

EmberStatus emberAfEzspSavePreinstalledCbkeData283k1CommandCallback(void);

// The CBKE Data associated with ECC 283k1 is sent in three separate EZSP
// commands because the total length of the combined data is greater than the
// max EZSP message length.  The first EZSP command will contain the certificate
// followed next by the public key and finally the private key. Once
// all the data has been received by the NCP a 4th command is sent by the host
// indicating to push the data into permanent storage.  We use Ember message
// buffers as temporary storage for the 3 components of the CBKE 283k1 data.
#define CBKE_283K1_DATA_CERTIFICATE_OFFSET 0
#define CBKE_283K1_DATA_PUBLIC_KEY_OFFSET \
  (CBKE_283K1_DATA_CERTIFICATE_OFFSET + EMBER_CERTIFICATE_283K1_SIZE)
#define CBKE_283K1_DATA_PRIVATE_KEY_OFFSET \
  (CBKE_283K1_DATA_PUBLIC_KEY_OFFSET + EMBER_PUBLIC_KEY_283K1_SIZE)
#define CBKE_283K1_DATA_LENGTH \
  (EMBER_CERTIFICATE_283K1_SIZE + EMBER_PUBLIC_KEY_283K1_SIZE + EMBER_PRIVATE_KEY_283K1_SIZE)

#ifndef UC_BUILD
void emberAfPluginEzspCbkeSetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspCbkeConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);

// -----------------------------------------------------------------------------
// Framework callbacks

void emberAfPluginEzspCbkePermitHostToNcpFrameCallback(bool *permit);
#endif  // UC_BUILD
