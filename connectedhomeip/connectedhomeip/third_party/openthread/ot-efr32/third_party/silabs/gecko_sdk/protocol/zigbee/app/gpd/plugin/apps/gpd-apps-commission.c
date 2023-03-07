/***************************************************************************//**
 * @file
 * @brief GPD commissioning functions.
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

// Build commissioning frame based on the GPD configuration.
// most of the fields are compile time configuration.
static uint8_t gpdBuildCommissioningCommand(EmberGpd_t * gpd,
                                            bool keyPresentInComRequest,
                                            uint8_t keyType,
                                            uint8_t* key,
                                            uint32_t frameCounter,
                                            uint8_t* cmd)
{
  uint8_t index = 0;
  cmd[index++] = GP_CMD_COMMISSIONING;
  // ZGP Device ID
  cmd[index++] = EMBER_AF_PLUGIN_APPS_APP_DEVICE_ID;

  // Options field ------------------------------------------------------------
  cmd[index] = (EMBER_AF_PLUGIN_APPS_MAC_SEQ ? EMBER_GPD_COM_OPT_MAC_SEQ_NBR_CAP_MASK : 0)     // b0 - MAC Seq Nbr capability sub-field
               | (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE ? EMBER_GPD_COM_OPT_RX_CAP_MASK : 0)      // b1 - RxOn capability sub-field
               | (EMBER_AF_PLUGIN_APPS_APP_INFO ? EMBER_GPD_COM_OPT_APP_INFO_MASK : 0)         // b2 - Application Information sub-field
               | (EMBER_AF_PLUGIN_APPS_PAN_ID_REQUEST ? EMBER_GPD_COM_OPT_PANID_REQ_MASK : 0); // b4 - Pan ID Request sub-field
  // Key Request sub-field
  // do not request key if shared key already configured or bidirectional is false
  if ((keyType == EMBER_GPD_SECURITY_KEY_TYPE_NWK)
      || (keyType == EMBER_GPD_SECURITY_KEY_TYPE_GROUP)
      || (keyType == EMBER_GPD_SECURITY_KEY_TYPE_GROUP_DERIVED)
      || (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE == false)
      || EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL == EMBER_GPD_SECURITY_LEVEL_NONE) {
    // No setting of Key Request flag
  } else {
    // else set the key request based on the application configuration
    cmd[index] |= (EMBER_AF_PLUGIN_APPS_KEY_REQUEST ? EMBER_GPD_COM_OPT_KEY_REQ_MASK : 0);     // b5 - GP Security Key Request sub-field
  }
  cmd[index] |= (EMBER_AF_PLUGIN_APPS_FIXED_LOCATION ? EMBER_GPD_COM_OPT_FIXED_LOC_MASK : 0);  // b6 - Fixed location sub-field

  // Extended option sub-field
#if defined(EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL) && (EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL == EMBER_GPD_SECURITY_LEVEL_NONE)
  // If the security level is 0, then there is no extended option as none of its fields are present
  // There is argument of adding this field and make it 0 for such condition.
#elif defined(EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL) && (EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL > EMBER_GPD_SECURITY_LEVEL_NONE)
  // set if: security level capability > none, set the ext option bit
  cmd[index] |= EMBER_GPD_COM_OPT_EXT_OPT_MASK;                                                // b7 - Fixed location sub-field

  index++; // Move to Extended options field

  // Extended Option field-----------------------------------------------------
  // Security capability sub-field
  cmd[index] = (EMBER_GPD_EXT_COM_OPT_SECURITY_LEVEL_CAP_MASK & EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL) // b0-b1 Security Level  sub-field
               | (keyType << EMBER_GPD_EXT_COM_OPT_KEY_TYPE_OFFSET)                                  // b2-b3-b4 Key type  sub-field
               | (keyPresentInComRequest ? EMBER_GPD_EXT_COM_OPT_KEY_PRESENT_MASK : 0)               // b5 Key Present  sub-field
               | (EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT ? EMBER_GPD_EXT_COM_OPT_KEY_ENCRYPT_MASK : 0)     // b6 Key encrypted sub-field
               | (EMBER_GPD_EXT_COM_OPT_OUT_COUNTER_PRESENT_MASK);                                   // b7 OG Frame Counter
  // Extended Option field-----------------------------------------------------

  index++; // Move to next fields

  // add the key
  if (keyPresentInComRequest) {
#if defined(EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT) && (EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT == true)
    uint8_t * protectedKey = &(cmd[index]);
    index += EMBER_AF_PLUGIN_APPS_KEY_LENGTH;
    uint8_t * mic = &(cmd[index]);
    index += EMBER_GPD_MIC_LENGTH;
    emberGpdSecurityEncryptKey(&(gpd->addr), key, protectedKey, mic);
#else
    (void) memcpy(&(cmd[index]), gpd->securityKey, EMBER_AF_PLUGIN_APPS_KEY_LENGTH);
#endif
  }
  // Add security counter
  emberGpdUtilityCopy4Bytes(&cmd[index], frameCounter);
  index += sizeof(frameCounter);
#endif

#if defined(EMBER_AF_PLUGIN_APPS_APP_INFO) && (EMBER_AF_PLUGIN_APPS_APP_INFO == true)
  uint8_t *appInfo = &(cmd[index]); // remember the application info location
  *appInfo = 0; //Initialise to 0

  index++; // Move to next fields

  // Add application information

#if EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID_PRESENT
  *appInfo |= EMBER_GPD_APP_INFO_COM_MS_ID_MASK;                          // b0 Manuf Id present
  // include MS ID
  cmd[index++] = LO_UINT16(EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID);
  cmd[index++] = HI_UINT16(EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID);
#endif

#if EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID_PRESENT
  *appInfo |= EMBER_GPD_APP_INFO_COM_MODEL_ID_MASK;                       // b1 Model Id present
  // include Model ID
  cmd[index++] = LO_UINT16(EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID);
  cmd[index++] = HI_UINT16(EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID);
#endif

#if (EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST_LENGTH > 0)
  *appInfo |= EMBER_GPD_APP_INFO_COM_CMDLIST_MASK;                        // b2 GpdCmd List present
  cmd[index++] = EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST_LENGTH;
  // GPD CMD List
  uint8_t gpdCmdList[] = EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST;
  for (int i = 0; i < EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST_LENGTH; i++) {
    cmd[index++] = gpdCmdList[i];
  }
#endif

#if (EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST_LENGTH > 0) || (EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST_LENGTH > 0)
  *appInfo |= EMBER_GPD_APP_INFO_COM_CLUSTERLIST_MASK;                    // b3 Cluster List present
  // Cluster list length representation
  cmd[index++] = EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST_LENGTH \
                 + (EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST_LENGTH << 4);

#if (EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST_LENGTH > 0)
  // In Cluster list
  uint16_t inClusterList[] = EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST;
  for (int i = 0; i < EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST_LENGTH; i++) {
    cmd[index++] = LO_UINT16(inClusterList[i]);
    cmd[index++] = HI_UINT16(inClusterList[i]);
  }
#endif

#if (EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST_LENGTH > 0)
  // Out cluster list
  uint16_t outClusterList[] = EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST;
  for (int i = 0; i < EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST_LENGTH; i++) {
    cmd[index++] = LO_UINT16(outClusterList[i]);
    cmd[index++] = HI_UINT16(outClusterList[i]);
  }
#endif
#endif //cluster list

#ifdef EMBER_AF_PLUGIN_APPS_GPD_SWITCH_INFO_PRESENT
  *appInfo |= EMBER_GPD_APP_INFO_COM_SWITCH_INFORMATION_PRESENT_MASK;     // b4 Switch Info present
  // include switch information
  cmd[index++] = 2; // as per current spec the switch info has a length of 2
  cmd[index++] = EMBER_AF_PLUGIN_APPS_GPD_SWITCH_INFO;
  cmd[index++] = emberGpdAfPluginGetSwitchContactStutusCallback();
#endif

#ifdef EMBER_AF_PLUGIN_APPS_GPD_APP_DESCRIPTION_FOLLOWS
  *appInfo |= EMBER_GPD_APP_INFO_COM_APP_DESCRIPTION_FOLLOWS_MASK;        // b5 App description follows
#endif

#endif // EMBER_AF_PLUGIN_APPS_APP_INFO

  // return length of commissioning command payload
  return index;
}

// Commissioning Request send function.
// A commissioning request is send unsecured at the GPD network layer, hence it resets
// the security level to none, it is hence advised that the calling function must push and pop
// the security level in case of battery powered implementation.
static void sendCommissioningRequest(EmberGpd_t * gpd)
{
  uint8_t command[EMBER_GPD_CMD_MAX_PAYLOAD_SIZE];
  uint8_t key[] = EMBER_AF_PLUGIN_APPS_KEY;
  // Commissioning Request is with security level 0 until it receives the
  // commissioning reply to set the security level to be used further.
  gpd->securityLevel = EMBER_GPD_SECURITY_LEVEL_NONE;
  uint8_t commandLength = gpdBuildCommissioningCommand(gpd,
                                                       true,
                                                       EMBER_AF_PLUGIN_APPS_SECURITY_KEY_TYPE,
                                                       key,
                                                       gpd->securityFrameCounter,
                                                       command);
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  commandLength,
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}

// Sends a Success command for bidirectional commissioning following commissioning reply,
// security used based on the negotiated security material at the commissioning reply
static void sendSuccess(EmberGpd_t * gpd)
{
  uint8_t command[] = { GP_CMD_COMMISSIONING_SUCCESS };
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}

// Sends out a Channel Request, with channel toggling behaviour payload as configured
static void sendChannelRequest(EmberGpd_t * gpd,
                               uint8_t nextChannel,
                               uint8_t secondNextChannel)
{
  uint8_t command[] = { GP_CMD_CHANNEL_RQST, 0x00 };
  // Channel toggling payload at index 1 :
  //                  b7-b4 - Second next channel
  //                  b3-b0 - next channel
  command[1] = (nextChannel - 11) + ((secondNextChannel - 11) << 4);
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_MAINT,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}

// --------------------------------------------------------------------------------------------
// --------------------------Application Description ------------------------------------------
// --------------------------------------------------------------------------------------------
// This block of code has the implementation of the application description commands to be
// sent with ensuring in case of a bidirectional commissioning, it encloses the correct setting
// of rxAfterTx flag to collect the commissioning reply.

#if (defined EMBER_AF_PLUGIN_APPS_GPD_APP_DESCRIPTION_FOLLOWS)
// Sub states of the application description
#define EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_COMMISSIONING_REQ 0
#define EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_APP_DESCRIPTION_REQ 1
#define EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_APP_DESCRIPTION_FINAL_REQ 2

// Static variable to hold the substate
static uint8_t applicationDescriptionState = EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_COMMISSIONING_REQ;

// Application Description Command, the payload of this command is build by the
// a callback so that the user need to provide the payload appropriately.
static uint8_t gpdBuildCommissioningApplicationDescriptionCommand(uint8_t* cmd,
                                                                  bool * last)
{
  uint8_t index = 0;
  cmd[index++] = GP_CMD_COMMISSIONING_APP_DESCR;
  // Copy the Application description in rest of the payload
  index += emberGpdAfPluginGetApplicationDescriptionCallback(&cmd[index],
                                                             EMBER_GPD_CMD_MAX_PAYLOAD_SIZE,
                                                             last);
  return index;
}

// Calls the user to provide the application description payload and then sends the
// command out. It also takes the decision to set or clear the rxAfterTx if the current
// application is the last application description frame to be sent out.
static bool sendApplicationDescriptionCommand(EmberGpd_t * gpd)
{
  uint8_t command[EMBER_GPD_CMD_MAX_PAYLOAD_SIZE];
  bool last = true;
  uint8_t commandLength = gpdBuildCommissioningApplicationDescriptionCommand(command,
                                                                             &last);

#if defined EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE && (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE == true)
  // Based ont the last application descriptor, the rxAfteTx flag is set to receive
  // the commissioning reply incase of a bidirectional commissioning
  gpd->rxAfterTx = (last ? true : false);
#endif

  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  commandLength,
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
  return last;
}

// Application Description State machine with 3 main states :
// first : send a commissioning command (E0) with application description follows flag set
//         (and no rxAfterTx in case of bidirectional)
// second : send the series of application description frames until last but one frame is
//          sent out. Example, for 3 report descriptors, lets say 3 application description frames.
// last : send the last frame (for the bidirectional, ensure to set the rxAfterTx) couple of times.
static void gpdCommissioningStateMachineWithAppDescription(EmberGpd_t * gpd)
{
  // Step 1 : First send Commissioning without rxAfterTx set and indication the
  // application description to follow is already set in the commissioning command
  // formation.
  if (applicationDescriptionState == EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_COMMISSIONING_REQ) {
    gpd->rxAfterTx = false;
    for (int i = 0; i < EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_COMMISSIONING_REQUEST; i++) {
      sendCommissioningRequest(gpd);
    }
    applicationDescriptionState = EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_APP_DESCRIPTION_REQ;
  } else if (applicationDescriptionState == EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_APP_DESCRIPTION_REQ) {
    // Step 2 to n : Get from user the Application Description  packet with asking
    // if more to follow or not, repeat based on upto n times where n iteration is
    // no more application description to follow.
    bool last = sendApplicationDescriptionCommand(gpd);
    if (last) {
      // go to step n+1 else re-send it again, asking user if more to follow;
      applicationDescriptionState = EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_APP_DESCRIPTION_FINAL_REQ;
    }
  } else if (applicationDescriptionState == EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_APP_DESCRIPTION_FINAL_REQ) {
    // Step n+1 : send the last application description command
    // with rxAfterTx true pull the commissioning reply.
    for (int i = 0; i < EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_COMMISSIONING_REQUEST; i++) {
      sendApplicationDescriptionCommand(gpd);
      if (gpd->gpdState == EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED) {
        applicationDescriptionState = EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_COMMISSIONING_REQ;
        return;
      }
    }
    // at this point no commissioning reply then , it is not commissioned!
    gpd->gpdState = EMBER_GPD_APP_STATE_NOT_COMMISSIONED;
    applicationDescriptionState = EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_COMMISSIONING_REQ;
  } else {
    // just reset the  sub state state so it enters
    applicationDescriptionState = EMBER_GPD_COMM_STATE_APP_DESC_STEP_SEND_COMMISSIONING_REQ;
  }
}
#endif
// --------------------------------------------------------------------------------------------
// --------------------------End of Application Description -----------------------------------
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
// ----------------------------------Channel Request ------------------------------------------
// --------------------------------------------------------------------------------------------
// This block of code is for Channel request by the GPD to find the operational channel of the
// Proxy/Combo in case of bidirectional commissioning.
#if (defined EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE) && (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE == true)

#define EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_ALL_CHANNELS 0
#define EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_RX_CHANNEL   1

static uint8_t chReqSubstate = EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_ALL_CHANNELS;

// Sends channel request maint frame with auto commissioning set on all the enabled channels
// with auto commissioning set and rxAfterTx cleared.
static void gpdCommissioningChannelRequestStateOnAllChannel(EmberGpd_t * gpd)
{
  // Sub state 1
  // Send channel request on configured channel with auto commissioning set and rxAfterTx cleared
  gpd->rxAfterTx = false;
  gpd->autoCommissioning = true;
  uint8_t channel[] = EMBER_AF_PLUGIN_APPS_APP_CHANNEL_SET;
  for (int j = 0; j < EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_CHANNEL_REQUEST_PER_CHANNEL_WITH_AC_SET; j++) {
    for (int i = 0; i < EMBER_AF_PLUGIN_APPS_APP_CHANNEL_SET_LENGTH; i++) {
      gpd->channel = channel[i];
      sendChannelRequest(gpd,
                         EMBER_AP_PLUGIN_APPS_APP_NEXT_RX_CHANNEL,
                         EMBER_AP_PLUGIN_APPS_APP_SECOND_NEXT_RX_CHANNEL);
    }
  }
}

// Sends the channel request on the rx channel with auto-commissioning cleared and rxAfterTx set
static void gpdCommissioningChannelRequestOnRxChannel(EmberGpd_t * gpd)
{
  // Sub state 2
  // Send channel request on rx channel and next-rx channel with auto commissioning clear.
  gpd->rxAfterTx = true;
  gpd->autoCommissioning = false;
  for (int i = 0; i < EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_CHANNEL_REQUEST_PER_CHANNEL; i++) {
    gpd->channel = EMBER_AP_PLUGIN_APPS_APP_NEXT_RX_CHANNEL;
    sendChannelRequest(gpd,
                       EMBER_AP_PLUGIN_APPS_APP_NEXT_RX_CHANNEL,
                       EMBER_AP_PLUGIN_APPS_APP_SECOND_NEXT_RX_CHANNEL);
    if (gpd->gpdState == EMBER_GPD_APP_STATE_CHANNEL_RECEIVED) {
      return;
    }
  }
  gpd->gpdState = EMBER_GPD_APP_STATE_NOT_COMMISSIONED;
}

// Channel Request state machine : mainly two states
// first : Send the channel request with channel toggling behaviour (includes rx channel)
//         on all enabled channels.
// Second : Send the channel request on the rx channel with receive window so that it can
//          receive back the channel configuration.
static void gpdCommissioningChannelRequestStateMachine(EmberGpd_t * gpd)
{
  gpd->securityLevel = EMBER_GPD_SECURITY_LEVEL_NONE;
  gpd->securityKeyType = EMBER_GPD_SECURITY_KEY_TYPE_NOKEY;
  gpd->skipCca = true;
  if (chReqSubstate == EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_ALL_CHANNELS) {
    gpdCommissioningChannelRequestStateOnAllChannel(gpd);
    chReqSubstate = EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_RX_CHANNEL;
  } else if (chReqSubstate == EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_RX_CHANNEL) {
    gpdCommissioningChannelRequestOnRxChannel(gpd);
    chReqSubstate = EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_ALL_CHANNELS;
  } else {
    chReqSubstate = EMBER_GPD_COMM_STATE_CHANNEL_REQ_ON_ALL_CHANNELS;
  }
}
#endif
// --------------------------------------------------------------------------------------------
// --------------------------------- End of Channel Request -----------------------------------
// --------------------------------------------------------------------------------------------

#if defined EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE && (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE == true)
static void gpdBidirCommissioningStateMachine(EmberGpd_t * gpd)
{
#if (defined EMBER_AF_PLUGIN_APPS_GPD_APP_DESCRIPTION_FOLLOWS)
  gpdCommissioningStateMachineWithAppDescription(gpd);
#else
  for (int i = 0; i < EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_COMMISSIONING_REQUEST; i++) {
    sendCommissioningRequest(gpd);
    if (gpd->gpdState == EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED) {
      return;
    }
  }
  gpd->gpdState = EMBER_GPD_APP_STATE_CHANNEL_RECEIVED;
#endif
}
#endif

#if (defined EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE) && (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE == false)
static void sendUndirCommissioningRequest(EmberGpd_t * gpd)
{
  // Save the configured security level and send the commissioning request
  // with security level 0.
  uint8_t securityLevel = gpd->securityLevel;
#if (defined EMBER_AF_PLUGIN_APPS_GPD_APP_DESCRIPTION_FOLLOWS)
  gpdCommissioningStateMachineWithAppDescription(gpd);
#else
  // Send commissioning request
  sendCommissioningRequest(gpd);
#endif
  // Get back to the security level and keys as configured.
  gpd->securityLevel = securityLevel;
  emberGpdMbedtlsCcmSetkeyWraper(gpd->securityKey);
}
#endif

int8_t emberGpdProcessCommissioningReply(EmberGpd_t * gpd,
                                         uint8_t payload_size,
                                         uint8_t* pRxbuffer)
{
  // Unused
  (void)payload_size;

  // local variable
  uint8_t index = 0;

  uint8_t * newKeyEncrypted;
  uint8_t * newKeyMic;
  uint8_t * rxSecCounter;

  uint8_t options = pRxbuffer[index++];
  bool panIdPresent = (options & EMBER_GPD_COM_REP_OPT_PANID_PRESENT_MASK) ? true : false;
  bool keyPresent   = (options & EMBER_GPD_COM_REP_OPT_KEY_PRESENT_MASK) ? true : false;
  bool keyEncrypted = (options & EMBER_GPD_COM_REP_OPT_KEY_ENCRYPT_MASK) ? true : false;
  uint8_t securityLevel = (options & EMBER_GPD_COM_REP_OPT_SECURITY_LEVEL_MASK) >> EMBER_GPD_COM_REP_OPT_SECURITY_LEVEL_OFFSET;
  uint8_t securityKeyType = (options & EMBER_GPD_COM_REP_OPT_KEY_TYPE_MASK) >> EMBER_GPD_COM_REP_OPT_KEY_TYPE_OFFSET;

  if (panIdPresent) {
    uint16_t panId;
    panId = pRxbuffer[index] + (pRxbuffer[index] << 8);
    index += sizeof(panId);
  }
  // TODO: we don't reconfigure PAN ID in our system
  if (keyPresent) {
    // Check if key delivered encrypted
    if (keyEncrypted) {
#if defined(EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT) && (EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT == true)
      // get encrypted key
      newKeyEncrypted = &pRxbuffer[index];
      index += EMBER_AF_PLUGIN_APPS_KEY_LENGTH;
      // get MIC
      newKeyMic = &pRxbuffer[index];
      index += EMBER_GPD_MIC_LENGTH;
      // get security frame counter
      rxSecCounter = &pRxbuffer[index];
      index += sizeof(uint32_t);

      // Unsecured the key
      if (SUCCESS != emberGpdSecurityDecryptKey(&(gpd->addr),
                                                newKeyEncrypted,
                                                newKeyMic,
                                                rxSecCounter,
                                                gpd->securityKey)) {
        return FAILED;
      }
#endif
    } else {
      // copy the new key
      for (int i = 0; i < EMBER_AF_PLUGIN_APPS_KEY_LENGTH; i++) {
        gpd->securityKey[i] = pRxbuffer[index++];
      }
    }
  } else {   // key not present
    // TODO: check rule for not key delivering in com and op
  }
  gpd->securityKeyType = securityKeyType;
  gpd->securityLevel = securityLevel;
  emberGpdMbedtlsCcmSetkeyWraper(gpd->securityKey);
  return SUCCESS;
}

void emberGpdAfPluginCommissioningChannelConfigCallback(uint8_t channel)
{
  emberGpdSetChannel(channel);
  emberGpdSetState(EMBER_GPD_APP_STATE_CHANNEL_RECEIVED);
}

void emberGpdAfPluginCommissioningReplyCallback(uint8_t length,
                                                uint8_t * commReplyPayload)
{
  if (SUCCESS == emberGpdProcessCommissioningReply(emberGpdGetGpd(),
                                                   length,
                                                   commReplyPayload)) {
    emberGpdSetState(EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED);
  }
}

void emberGpdAfPluginDeCommission(EmberGpd_t * gpd)
{
  uint8_t command[] = { GP_CMD_DECOMMISSIONING };
  gpd->rxAfterTx = false;
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
  emberGpdLoadGpdDefaultConfiguration(gpd);
}

void emberGpdAfPluginCommission(EmberGpd_t * gpd)
{
#if (defined EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE) && (EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE == true)
  if (gpd->gpdState == EMBER_GPD_APP_STATE_NOT_COMMISSIONED) {
    gpdCommissioningChannelRequestStateMachine(gpd);
  } else if (gpd->gpdState == EMBER_GPD_APP_STATE_CHANNEL_RECEIVED) {
    gpdBidirCommissioningStateMachine(gpd);
  } else if (gpd->gpdState == EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED) {
    sendSuccess(gpd);
    emberGpdSetState(EMBER_GPD_APP_STATE_COMMISSIONING_SUCCESS_REQUEST);
  } else if (gpd->gpdState == EMBER_GPD_APP_STATE_COMMISSIONING_SUCCESS_REQUEST
             || gpd->gpdState == EMBER_GPD_APP_STATE_OPERATIONAL) {
    sendSuccess(gpd);
    emberGpdSetState(EMBER_GPD_APP_STATE_OPERATIONAL);
  } else {
    gpd->gpdState = EMBER_GPD_APP_STATE_NOT_COMMISSIONED;
  }
#else
  sendUndirCommissioningRequest(gpd);
#endif
}
