/***************************************************************************//**
 * @file
 * @brief Routines for timers used by the GPD.
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

#include "gpd-components-common.h"

static uint8_t gpdBuildHeader(uint8_t * buffer,
                              uint8_t payloadLength,
                              EmberGpd_t * gpd)
{
  uint8_t index = 1; //index 0 is the length of the entire mpdu, filled later.
  // Set Network Frame Control Fields
  uint8_t nwkFc = EMBER_GPD_NWK_FC_FRAME_TYPE_DATA                                       // b0-b1 Frame Type
                  | (EMBER_GPD_NWK_FC_ZIGBEE_VER << EMBER_GPD_NWK_FC_ZIGBEE_VER_OFFSET)  // b2-b3-b4-b5 Zigbee Protocol Version
                  | (gpd->autoCommissioning ? EMBER_GPD_NWK_FC_AUTOCOM_MASK : 0);
  // Extended FC present if (any of its fields are not 0):
  //   App ID = IEEE
  //   OR Security Level > None
  //   or RxAfterTx set
  //   or direction is 1, but here for gpd-to-proxy it is 0                              // b6 Direction
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  nwkFc |= EMBER_GPD_NWK_FC_EXT_FC_MASK;                                                 // b7 Ext Nwk Fc present
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  if (gpd->securityLevel > EMBER_GPD_SECURITY_LEVEL_NONE
      || gpd->rxAfterTx == true) {
    nwkFc |= EMBER_GPD_NWK_FC_EXT_FC_MASK;                                               // b7 Ext Nwk Fc present
  }
#else
#error "Invalid application Id configuration"
#endif
  // Build Extended Network Frame Control Fields
  uint8_t extNwkFc = gpd->addr.appId;                                             // B[2-0]
  extNwkFc |= (gpd->securityLevel << EMBER_GPD_EXT_NWK_FC_SECURITY_LEVEL_OFFSET); // B[4-3]
  if (gpd->securityLevel == EMBER_GPD_SECURITY_LEVEL_NONE) {
    // for security level 0, the security indication is EMBER_GPD_SECURITY_KEY_INDICATION_SHARED , i.e 0
  } else {
    extNwkFc |= ((gpd->securityKeyType & EMBER_GPD_SECURITY_KEY_TYPE_INDICATION_MASK) \
                 >> EMBER_GPD_SECURITY_KEY_TYPE_INDICATION_MASK_OFFSET) ? EMBER_GPD_EXT_NWK_FC_SEC_KEY_TYPE_MASK : 0;  //B[5]
  }
  extNwkFc |= gpd->rxAfterTx ? EMBER_GPD_EXT_NWK_FC_RX_AFTER_TX_MASK : 0; //B[6]
  // Direction from GPD to GPP is 0 ,so save a byte of code by nod adding it! //B[7]

  //
  //---------------------------------------------------------------------------
  //2      | 1     |  0/2| 0/2/8| 0/2   | 0/2/8  | 0/5/6/10/14| variable| 2
  //---------------------------------------------------------------------------
  //Frame  | Seq   | Dest| Dest | Source| Source | Auxiliary  | Frame   | FCS
  //Control| Number| PAN | Addr | PAN Id| Address| Security   | Payload |
  //       |       | Id  |      |       |        | Header     |         |
  //---------------------------------------------------------------------------

#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  buffer[index++] = LO_UINT16(EMBER_GPD_MAC_FC_WORD);
  buffer[index++] = HI_UINT16(EMBER_GPD_MAC_FC_WORD);
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  buffer[index++] = LO_UINT16(EMBER_GPD_MAC_FC_IEEE_WORD);
  buffer[index++] = HI_UINT16(EMBER_GPD_MAC_FC_IEEE_WORD);
#endif

  // The MAC seq number
  buffer[index++] = (uint8_t)(gpd->securityFrameCounter & 0x000000FF);

  // PAN ID
  buffer[index++] = LO_UINT16(0xFFFF);
  buffer[index++] = HI_UINT16(0xFFFF);
  // Destination Address
  buffer[index++] = LO_UINT16(0xFFFF);
  buffer[index++] = HI_UINT16(0xFFFF);

#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  (void) memcpy(&buffer[index], gpd->addr.id.ieee, EMBER_GPD_EUI64_SIZE);
  index += 8;
#endif
  buffer[index++] = nwkFc;
  if ((nwkFc & EMBER_GPD_NWK_FC_EXT_FC_MASK) == EMBER_GPD_NWK_FC_EXT_FC_MASK) {
    buffer[index++] = extNwkFc;
  }
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  emberGpdUtilityCopy4Bytes(&buffer[index], gpd->addr.id.srcId);
  index += 4;
#endif
  // Add end point if AppID = IEEE
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  buffer[index++] = gpd->addr.gpdEndpoint;
#endif

  uint8_t micLength = 0;
  if (gpd->securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_AUTH_ONLY
      || gpd->securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_FULL) {
    micLength = TAG_SIZE; // mic length will be considered at the end
    emberGpdUtilityCopy4Bytes(&buffer[index], gpd->securityFrameCounter);
    index += 4; // uint32 frame counter length
  }
  // Fill up the frame length, all the data needed to determine frame length is
  // available here.
  buffer[0] = index + 1 + payloadLength + micLength;
  return index;
}

uint8_t emberGpdBuildMpdu(uint8_t * mpdu,
                          uint8_t * payload,
                          uint8_t payloadLength,
                          EmberGpd_t * gpd)
{
  uint8_t hdrLength = gpdBuildHeader(mpdu, payloadLength, gpd);
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  uint8_t posNwkHdr = EMBER_GPD_MAC_HEADER_SIZE; // Default app Id = SrcID
  // check if SrcID or IEEE
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  uint8_t posNwkHdr = EMBER_GPD_MAC_HEADER_SIZE_IEEE_TX;
#else
#error "Unsupported GPD Application Id"
#endif
  // payload is the Gpd Command + command payload, so payload[0] = command Id
  // Suppress RxAfterTx if Commissioning Success
  if (payload[0] == GP_CMD_COMMISSIONING_SUCCESS) {
    // Success command does not have a rxAfterTx even if it is going from a GPD
    // that is capable of rxAfterTx.
    if ((mpdu[posNwkHdr] & EMBER_GPD_NWK_FC_EXT_FC_MASK) == EMBER_GPD_NWK_FC_EXT_FC_MASK) {
      mpdu[posNwkHdr + 1] &= ~(EMBER_GPD_EXT_NWK_FC_RX_AFTER_TX_MASK);
    }
  }
  // set Tx Buffer with payload data
  for (int n = 0; n < payloadLength; n++) {
    mpdu[hdrLength + n] = payload[n];
  }
  return (hdrLength + payloadLength); // total mpdu length
}

uint8_t emberGpdSecureOutgoingMPdu(uint8_t * mpdu,
                                   uint8_t mpduLength,
                                   uint8_t payloadLength,
                                   EmberGpdSecurityLevel_t securityLevel)
{
  if (securityLevel == EMBER_GPD_SECURITY_LEVEL_NONE) {
  } else if (securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_AUTH_ONLY) {
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
    emberTagTxBuff(mpdu,
                   mpduLength,
                   EMBER_GPD_AUTH_START_SRC_ID,
                   EMBER_GPD_AUTH_FIXED_LEN_SRC_ID + payloadLength);
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
    emberTagTxBuff(mpdu,
                   mpduLength,
                   EMBER_GPD_AUTH_START_IEEE,
                   EMBER_GPD_AUTH_FIXED_LEN_IEEE + payloadLength);
#else
#error "Unsupported GPD Application Id"
#endif
    mpduLength += TAG_SIZE; // 4 byte MIC added to the payload
  } else if (securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_FULL) {
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
    emberEncryptAndTagTxBuff(mpdu,
                             mpduLength,
                             payloadLength,
                             EMBER_GPD_AUTH_START_SRC_ID,
                             EMBER_GPD_AUTH_FIXED_LEN_SRC_ID);
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
    emberEncryptAndTagTxBuff(mpdu,
                             mpduLength,
                             payloadLength,
                             EMBER_GPD_AUTH_START_IEEE,
                             EMBER_GPD_AUTH_FIXED_LEN_IEEE);
#else
#error "Unsupported GPD Application Id"
#endif
    mpduLength += TAG_SIZE; // 4 byte MIC added to the payload
  }

#if defined (EMBER_GPD_PACKET_SECURITY_TEST)
  extern uint8_t testOutgoingMpdu[];
  extern uint8_t testOutgoingMpduLength;
  testOutgoingMpduLength = mpduLength;
  (void) memcpy(testOutgoingMpdu, mpdu, mpduLength);
#endif
  return mpduLength;
}

static uint8_t buildMaintHeader(uint8_t seqNo,
                                uint8_t* buffer,
                                uint8_t payloadLength,
                                uint8_t enterRx)
{
  uint8_t nwkFc;
  uint8_t index = 1;

  // Set Network Frame Control Fields
  nwkFc = EMBER_GPD_NWK_FC_FRAME_TYPE_MAINT                                       // b0-b1 frame type
          | (EMBER_GPD_NWK_FC_ZIGBEE_VER << EMBER_GPD_NWK_FC_ZIGBEE_VER_OFFSET)   // b2-b3-b4-b5 Zigbee Protocol Version
          | (enterRx ? 0 : EMBER_GPD_NWK_FC_AUTOCOM_MASK);

  // Build header
  buffer[index++] = LO_UINT16(EMBER_GPD_MAC_FC_WORD);
  buffer[index++] = HI_UINT16(EMBER_GPD_MAC_FC_WORD);
  buffer[index++] = seqNo;
  buffer[index++] = LO_UINT16(0xFFFF);
  buffer[index++] = HI_UINT16(0xFFFF);
  buffer[index++] = LO_UINT16(0xFFFF);
  buffer[index++] = HI_UINT16(0xFFFF);
  buffer[index++] = nwkFc;
  buffer[0] = index + 1 + payloadLength; //frame length

  return index;
}

uint8_t emberBuildMaintMpdu(uint8_t * mpdu,
                            EmberGpd_t * gpd,
                            uint8_t * pPayload,
                            uint8_t payloadLength)
{
  uint8_t hdrLength;

  // TODO : Remove the following temporary fix when following ticket is resolved
  // EMZIGBEE-3092 : if the MAC Seq=0,the proxy dorops the frame at dup filtering.
  if ( (uint8_t)(gpd->securityFrameCounter) == 0) {
    gpd->securityFrameCounter++;
  }
  // -- End of Temporary fix. --

  hdrLength = buildMaintHeader((uint8_t)(gpd->securityFrameCounter),
                               mpdu,
                               payloadLength,
                               !(gpd->autoCommissioning));
  for (int n = 0; n < payloadLength; n++) {
    mpdu[hdrLength + n] = pPayload[n];
  }
  return (hdrLength + payloadLength); // total mpdu length
}

uint8_t emberBuildOutGoingPdu(uint8_t frameType,
                              uint8_t * mpdu,
                              uint8_t * payload,
                              uint8_t payloadLength,
                              EmberGpd_t * gpd)
{
  uint8_t length = 0;
  uint8_t mpduLength = 0;
  if (frameType == EMBER_GPD_NWK_FC_FRAME_TYPE_DATA) {
    // build data GPDF MPDU packet
    mpduLength = emberGpdBuildMpdu(mpdu, payload, payloadLength, gpd);
    //secure out going MPDU
    length = emberGpdSecureOutgoingMPdu(mpdu, mpduLength, payloadLength, gpd->securityLevel);
  } else if (frameType == EMBER_GPD_NWK_FC_FRAME_TYPE_MAINT) {
    // build beacon MPDU packet
    mpduLength = emberBuildMaintMpdu(mpdu, gpd, payload, payloadLength);
    //secure out going MPDU
    length = emberGpdSecureOutgoingMPdu(mpdu, mpduLength, payloadLength, 0);
  } else {
    // Nothing
  }
  return length;
}
#ifndef USER_HAS_GPD_INCOMING_MESSAGE_HANDLER
void emberGpdIncomingMessageHandler(uint8_t * buffer,
                                    uint16_t length)
{
  // buffer[0] - total length including MAC 2byte FCS
  uint8_t nwkPayloadLength = buffer[0] - EMBER_GPD_MAC_FOOTER_SIZE;
  if (nwkPayloadLength < 8) {
    return; // DROP - short payload
  }
  uint8_t currentFinger = 0; // Moved along each byte parsed.
  uint16_t mFc = (uint16_t)(buffer[2] << 8) + buffer[1];
  uint8_t nwkFcOffset = 0;
  // find the NWK offset in the incoming frame based on the addressing
  uint8_t ieeeAddressOffset = 0;
  if (mFc == EMBER_GPD_MAC_FC_IEEE_WORD_INCOMING) {
    ieeeAddressOffset = EMBER_GPD_MAC_LONG_ADDRESS_OFFSET;
    nwkFcOffset = EMBER_GPD_NWK_FC_OFFSET_FOR_LONG_ADDRESS;
  } else if (mFc == EMBER_GPD_MAC_FC_WORD) {
    // The network FC offset = 8 in buffer
    nwkFcOffset = EMBER_GPD_NWK_FC_OFFSET_FOR_SHORT_ADDRESS;
  }

  if (nwkFcOffset == 0) {
    return; // DROP - mFc did not qualify the check
  }
  currentFinger = nwkFcOffset;
  uint8_t nwkFc = buffer[currentFinger++];
  // Check Zigbee Protocol version in NWK FC b5-b2 must be 0x03
  if ((((nwkFc & EMBER_GPD_NWK_FC_ZIGBEE_VER_MASK) \
        >> EMBER_GPD_NWK_FC_ZIGBEE_VER_OFFSET) != EMBER_GPD_NWK_FC_ZIGBEE_VER) // DROP - Bad protocol version
      || ((nwkFc & EMBER_GPD_NWK_FC_FRAME_TYPE_MASK) > EMBER_GPD_NWK_FC_FRAME_TYPE_MAINT)) {      // DROP - Bad Frame Type > maintenance frame
    return;
  }
  bool autoCommissioning = (nwkFc & EMBER_GPD_NWK_FC_AUTOCOM_MASK) ? true : false; // Bit 6 - Auto-commissioning
  uint8_t nwkFcExtn = 0; // default value in case the field is not present
  bool nwkFcExtnPresent = false;
  if (nwkFc & EMBER_GPD_NWK_FC_EXT_FC_MASK) {
    nwkFcExtn  = buffer[currentFinger++];
    nwkFcExtnPresent = true;
  }
  uint8_t direction = ((nwkFcExtn & EMBER_GPD_EXT_NWK_FC_DIRECTION_MASK) \
                       ? EMBER_GPD_EXT_NWK_FC_DIR_ZGPP_TO_ZGPD : EMBER_GPD_EXT_NWK_FC_DIR_ZGPD_TO_ZGPP);    // Bit 7
  if (nwkFcExtnPresent == true
      && direction != EMBER_GPD_EXT_NWK_FC_DIR_ZGPP_TO_ZGPD) {
    return; // DROP - Bad Direction
  }
  bool rxAfterTx = (nwkFcExtn & EMBER_GPD_EXT_NWK_FC_RX_AFTER_TX_MASK) ? true : false;        // Bit 6
  if ((nwkFcExtnPresent == true
       && rxAfterTx)
      && autoCommissioning) {
    return; // DROP - if both Auto commissioning and rxAfterTx both are set
  }
  uint8_t securityKeyIndication = ((nwkFcExtn & EMBER_GPD_EXT_NWK_FC_SEC_KEY_TYPE_MASK) \
                                   ? EMBER_GPD_SECURITY_KEY_INDICATION_INDIVIDUAL : EMBER_GPD_SECURITY_KEY_INDICATION_SHARED);   // Bit 5
  uint8_t securityLevel = ((nwkFcExtn & EMBER_GPD_EXT_NWK_FC_SECURITY_LEVEL_MASK) >> EMBER_GPD_EXT_NWK_FC_SECURITY_LEVEL_OFFSET); // Bits 4-3
  uint8_t appId =  (nwkFcExtn & EMBER_GPD_EXT_NWK_FC_APP_ID_MASK);        // Bits 2-0

  EmberGpdAddr_t addr = { 0 }; // assign an invalid address
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  if (nwkFcExtnPresent == true
      && appId == EMBER_GPD_APP_ID_SRC_ID) {
    if (!(nwkFc & EMBER_GPD_NWK_FC_FRAME_TYPE_MASK)) { // Only for data frame, the source Id will be present
      addr.appId = appId;
      addr.id.srcId = *((uint32_t *)(&(buffer[currentFinger])));
      currentFinger += 4; // SrcId size is 4 bytes, its uint32
    }
  }
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  if (nwkFcExtnPresent == true
      && appId == EMBER_GPD_APP_ID_IEEE_ID) {
    if (!(nwkFc & EMBER_GPD_NWK_FC_FRAME_TYPE_MASK)) { // Only for data frame, the source Id will be present
      addr.appId = appId;
      // copy IEEE address from the MAC header
      (void) memcpy(addr.id.ieee, &(buffer[ieeeAddressOffset]), EMBER_GPD_EUI64_SIZE);
      addr.gpdEndpoint = buffer[currentFinger++];
    }
  }
#else
#error "Application Id not defined"
#endif
  // check if the address matches with emGpd device set address
  if (!(nwkFc & EMBER_GPD_NWK_FC_FRAME_TYPE_MASK) // Data frames , then check address match
      && !emberGpdUtilityAddrMatch(&addr, emberGpdGetAddr())) {
    return; // DROP - Bad address
  }
  uint32_t securityFrameCounter = 0;
  uint8_t micLength = 0;
  uint32_t mic = 0;
  if (securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_AUTH_ONLY
      || securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_FULL) {
    securityFrameCounter = *((uint32_t *)&(buffer[currentFinger]));
    currentFinger += 4; // 4 bytes Frame counter
    micLength = TAG_SIZE;     // 4 byte mic length
    mic = *((uint32_t *)&(buffer[nwkPayloadLength - micLength]));
  }

  uint8_t * gpdfAppPayload = NULL;
  uint8_t gpdfAppPayloadLength = 0;
  if (nwkPayloadLength > (currentFinger + micLength - 1)) {
    gpdfAppPayload = &(buffer[currentFinger]);
    gpdfAppPayloadLength = nwkPayloadLength - (currentFinger + micLength - 1);
  }
  // Data Frame Security Processing - Decrypt/Authenticate
  // TODO : Drop Maintenance Frame with security enabled
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  if (appId == EMBER_GPD_APP_ID_SRC_ID
      && !(nwkFc & EMBER_GPD_NWK_FC_FRAME_TYPE_MASK)) { // for data frames only
    if (securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_AUTH_ONLY) {
      emberDecryptRxBuffer(buffer,
                           EMBER_GPD_AUTH_START_SRC_ID - 1,
                           0,
                           nwkPayloadLength - EMBER_GPD_AUTH_START_SRC_ID + 1 - EMBER_GPD_MIC_LENGTH);
    } else if (securityLevel == EMBER_GPD_SECURITY_LEVEL_LONG_FULL) {
      emberDecryptRxBuffer(buffer,
                           EMBER_GPD_AUTH_START_SRC_ID,
                           nwkPayloadLength - EMBER_GPD_AUTH_START_SRC_ID + 1 - EMBER_GPD_AUTH_FIXED_LEN_SRC_ID - EMBER_GPD_MIC_LENGTH,
                           EMBER_GPD_AUTH_FIXED_LEN_SRC_ID);
    }
  }
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
#else
#error "Application Id not defined"
#endif
  if (gpdfAppPayload && gpdfAppPayloadLength) {
    emberGpdIncomingCommandHandler(gpdfAppPayload, gpdfAppPayloadLength);
  }

  // Unused Variables
  (void)securityFrameCounter;
  (void)micLength;
  (void)mic;
  (void)securityKeyIndication;
  (void)ieeeAddressOffset;
  (void)length;

  return;
}
#endif //USER_HAS_GPD_INCOMING_MESSAGE_HANDLER
