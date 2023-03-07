/******************************************************************************
*  Filename:       rf_hid_cmd.h
*
*  Description:    CC13x2/CC26x2 API for HID commands
*
*  Copyright (c) 2015 - 2020, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/
/*!*****************************************************************************
 *  @file       rf_hid_cmd.h
 *  @brief      RF HID command interface for CC13x2, CC26x2
 *
 *
 *  To use the CMD_HID_TX and CMD_HID_RX commands, make sure that the following
 *  override list is being used:
 *  @code
 *  #define TURNAROUND_TIME 64 // 64 µs turnaround time
 *  uint32_t pOverridesCommon[] =
 *  {
 *    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x3 (DITHER_EN=0 and IPEAK=3).
 *    (uint32_t)0x00F388D3,
 *    // Set pilot tone length to 4 us (enable phase error discard as early as possible)
 *    HW_REG_OVERRIDE(0x6024,0x0020),
 *    // Bluetooth 5: Default to no CTE.
 *    HW_REG_OVERRIDE(0x5328,0x0000),
 *    // Synth: Increase mid code calibration time to 5 us
 *    (uint32_t)0x00058683,
 *    // Synth: Increase mid code calibration time to 5 us
 *    HW32_ARRAY_OVERRIDE(0x4004,1),
 *    // Synth: Increase mid code calibration time to 5 us
 *    (uint32_t)0x38183C30,
 *    // HID: Reduce turnaround times:
 *    (uint32_t)((TURNAROUND_TIME*4)<<16)|0x0263, // modify tx to rx turnaround time
 *    (uint32_t)((TURNAROUND_TIME*4)<<16)|0x0283, // modify rx to tx turnaround time
 *    // Of of override list
 *    (uint32_t)0xFFFFFFFF
 *  };
 *  @endcode
 *
 *  @anchor rf_hid_overview
 *  # Overview
 *  The HID commands are designed to be compatible with CC254x devices,
 *  using the same protocol as the CC254x.
 *  The packet are defined as:
 *  8 bit preamble, 32 bit sync word, optional 8 bit address, 8 or 9 bit header,
 *  [0-n] byte payload and 16  bit CRC at 2 Mbps.
 *  The HID commands uses the same automode functionality as found in the
 *  CC254x devices, see [CC254x user guide](https://www.ti.com/lit/ug/swru283b/swru283b.pdf).
 *
 *  The CMD_HID_TX utlizes a TX queue to evaluate if the subsequent packet
 *  should be sent, given that the device is not retranmitting the previous packet.
 *  On submission of the CMD_HID_TX, the radio will check if there are TX
 *  entries in the queue. If TX entires are present, the radio will start sending
 *  the packets as defined by the startTrigger. If no packets are present, the radio
 *  will enter a wait-state, waiting on CMD_TRIGGER_HID_TX. Once CMD_TRIGGER_HID_TX
 *  is submitted, the radio will re-evaluate the TX queue, and if packets are present
 *  , the radio will start sending the TX packets as defined by the startTrigger.
 *  If no entries in the TX queue has been submitted, or CMD_STOP/CMD_ABORT was
 *  submitted, the CMD_HID_TX will end and the radio is ready for another command.
 *
 *
 *  <hr>
 *  @anchor rf_hid_Usage
 *  # Usage
 *
 *  This documentation provides basic @ref rf_hid_settings_c
 *  "rf_hid_settings.c", @ref rf_hid_settings_h
 *  "rf_hid_settings.h" and a set of @ref rf_hid_Examples "examples"
 *  in the form of commented code fragments.  Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor rf_hid_settings
 *  ## RF HID settings
 *  @anchor rf_hid_settings_c
 *  @code
 *  // Filename: rf_hid_settings.c
 *
 *  // Import the settings header file
 *  #include "rf_hid_settings.h"
 *
 *  // Import the RF driver definitions
 *  #include <ti/drivers/rf/RF.h>
 *
 *  // Import the needed radio files
 *  #include <ti/devices/DeviceFamily.h>
 *  #include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
 *  #include DeviceFamily_constructPath(driverlib/rf_hid_cmd.h)
 *  #include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol_hid.h)
 *  #include DeviceFamily_constructPath(driverlib/rf_data_entry.h)
 *  #include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
 *
 *
 *  // TI-RTOS RF Mode Object
 *  RF_Mode RF_hid =
 *  {
 *    .rfMode = RF_MODE_PROPRIETARY,
 *    .cpePatchFxn = &rf_patch_cpe_multi_protocol_hid,
 *    .mcePatchFxn = 0,
 *    .rfePatchFxn = 0
 *  };
 *
 *  // TX and RX queue declarations
 *  uint8_t txEntry1[BUFFER_LEN];
 *  uint8_t txEntry2[BUFFER_LEN];
 *  uint8_t rxEntry1[BUFFER_LEN];
 *  uint8_t rxEntry2[BUFFER_LEN];
 *
 *  dataQueue_t txQ = {
 *    .pCurrEntry = NULL,
 *    .pLastEntry = NULL,
 *  };
 *
 *  dataQueue_t rxQ = {
 *    .pCurrEntry = NULL,
 *    .pLastEntry = NULL,
 *  };
 *
 *  // Statistics
 *  rfc_hidRxTxOutput_t rxTxStatistics;
 *
 *  // Override list. 64 us turnaround compatible with fastest setting
 *  // for CC254x devices
 *  #define TURNAROUND_TIME 64 // Turnaround time in usec
 *  uint32_t pOverridesCommon[] =
 *  {
 *    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x3 (DITHER_EN=0 and IPEAK=3).
 *    (uint32_t)0x00F388D3,
 *    // Set pilot tone length to 4 us (enable phase error discard as early as possible)
 *    HW_REG_OVERRIDE(0x6024,0x0020),
 *    // Bluetooth 5: Default to no CTE.
 *    HW_REG_OVERRIDE(0x5328,0x0000),
 *    // Synth: Increase mid code calibration time to 5 us
 *    (uint32_t)0x00058683,
 *    // Synth: Increase mid code calibration time to 5 us
 *    HW32_ARRAY_OVERRIDE(0x4004,1),
 *    // Synth: Increase mid code calibration time to 5 us
 *    (uint32_t)0x38183C30,
 *    // HID: Reduce turnaround times:
 *    (uint32_t)((TURNAROUND_TIME*4)<<16)|0x0263, // modify tx to rx turnaround time
 *    (uint32_t)((TURNAROUND_TIME*4)<<16)|0x0283, // modify rx to tx turnaround time
 *    // Of of override list
 *    (uint32_t)0xFFFFFFFF
 *  };
 *
 *  // CMD_RADIO_SETUP
 *  // Radio setup for HID command
 *  rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup =
 *  {
 *    .commandNo = CMD_RADIO_SETUP,
 *    .status = 0x0000,
 *    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
 *    .startTime = 0x00000000,
 *    .startTrigger.triggerType = 0x0,
 *    .startTrigger.bEnaCmd = 0x0,
 *    .startTrigger.triggerNo = 0x0,
 *    .startTrigger.pastTrig = 0x0,
 *    .condition.rule = 0x1,
 *    .condition.nSkip = 0x0,
 *    .mode = 0x2, // HID mode
 *    .loDivider = 0x00,
 *    .config.frontEndMode = 0x0,
 *    .config.biasMode = 0x0,
 *    .config.analogCfgMode = 0x0,
 *    .config.bNoFsPowerUp = 0x0,
 *    .txPower = 0x7217,
 *    .pRegOverride = pOverridesCommon,
 *  };
 *
 *  // CMD_FS
 *  // Frequency Synthesizer Programming Command
 *  rfc_CMD_FS_t RF_cmdFs =
 *  {
 *    .commandNo = 0x0803,
 *    .status = 0x0000,
 *    .pNextOp = 0,
 *    .startTime = 0x00000000,
 *    .startTrigger.triggerType = 0x0,
 *    .startTrigger.bEnaCmd = 0x0,
 *    .startTrigger.triggerNo = 0x0,
 *    .startTrigger.pastTrig = 0x1,
 *    .condition.rule = 0x1,
 *    .condition.nSkip = 0x0,
 *    .frequency = 2440,
 *    .fractFreq = 0x0000,
 *    .synthConf.bTxMode = 0x0,
 *    .synthConf.refFreq = 0x0,
 *    .__dummy0 = 0x00,
 *    .__dummy1 = 0x00,
 *    .__dummy2 = 0x00,
 *    .__dummy3 = 0x0000
 *  };
 *
 *  // CMD_HID_TX
 *  // HID TX command
 *  rfc_CMD_HID_TX_t RF_cmdTxHID =
 *  {
 *    .commandNo                  = CMD_HID_TX,
 *    .status                     = 0x0000,
 *    .pNextOp                    = 0x00000000,
 *    .startTime                  = 0x00000000,
 *    .startTrigger.triggerType   = 0x0,
 *    .startTrigger.bEnaCmd       = 0x0,
 *    .startTrigger.triggerNo     = 0x0,
 *    .startTrigger.pastTrig      = 0x0,
 *    .condition.rule             = COND_NEVER,
 *    .condition.nSkip            = 0x0,
 *    .pktConf.bFsOff             = 0x0, // Keep synth on
 *    .pktConf.bAutoRetransmit    = 0x1, // Listen for ack and retransmit
 *    .pktConf.bVarLen            = 0x1, // Variable length mode
 *    .pktConf.hdrMode            = 0x0, // 9 bit mode
 *    .pktConf.bIncludeAddr       = 0x1, // Include address after sync word
 *    .pktConf.hdrConf            = 0x0, // Automatically generate header
 *    .pktConf.bFixedTxLen        = 0x0, // Calculate length when sending packet
 *    .rxConf.bAutoFlushIgnored   = 0x0, // Do not flush ignored packets
 *    .rxConf.bAutoFlushCrcErr    = 0x1, // Flush packets with CRC error
 *    .rxConf.bIncludeAddr        = 0x0, // Do not include address in queue
 *    .rxConf.bIncludeHdr         = 0x1, // Include header in queue
 *    .rxConf.bIncludeCrc         = 0x0, // Do not include CRC in queue
 *    .rxConf.bAppendStatus       = 0x0, // Do not append status byte in queue
 *    .rxConf.bAppendTimestamp    = 0x0, // Do not append time stamp of received packet in queue
 *    .syncWord                   = 0x29417671,
 *    .address                    = 0xEF,
 *    .seqNo                      = 0x00,
 *    .maxAckLen                  = 0x1E, // Maximum length of ack
 *    .pktLen                     = 32,   // Packet is 32 bytes
 *    .maxRetrans                 = 3,    // Maximum three retransmissions
 *    .noAckMode.noAckVal         = 0,    // Set automatic NO_ACK value to inverse of bAutoRetransmit
 *    .noAckMode.bAlwaysAutoRetransmit = 1, // Never disable auto retransmit
 *    .retransDelay               = 0x1E, // Number of RAT ticks from start of transmission of a packet to retransmission
 *    .pPkt                       = 0,
 *    .pRxQueue                   = 0,
 *    .pOutput                    = 0,
 *  };
 *
 *  // CMD_HID_RX
 *  // HID RX command
 *  rfc_CMD_HID_RX_t RF_cmdRxHID =
 *  {
 *    .commandNo                  = CMD_HID_RX,
 *    .status                     = 0x0000,
 *    .pNextOp                    = 0x00000000,
 *    .startTime                  = 0x00000000,
 *    .startTrigger.triggerType   = 0x0,
 *    .startTrigger.bEnaCmd       = 0x0,
 *    .startTrigger.triggerNo     = 0x0,
 *    .startTrigger.pastTrig      = 0x0,
 *    .condition.rule             = COND_NEVER,
 *    .condition.nSkip            = 0x0,
 *    .pktConf.bFsOff             = 0x0, // Keep synth on
 *    .pktConf.bRepeatOk          = 1,   // If packet was received OK, then end
 *    .pktConf.bRepeatNok         = 1,   // If packer was NOK, then go back to sync search
 *    .pktConf.hdrMode            = 0x0, // 9 bit mode
 *    .pktConf.bIncludeAddr       = 0x1, // Include address after sync word
 *    .pktConf.hdrConf            = 0x0, // Automatically generate header
 *    .rxConf.bAutoFlushIgnored   = 0x0, // Do not flush ignored packets
 *    .rxConf.bAutoFlushCrcErr    = 0x0, // Do not flush packets with CRC error
 *    .rxConf.bIncludeAddr        = 0x0, // Do not include address in queue
 *    .rxConf.bIncludeHdr         = 0x0, // Do not include header in queue
 *    .rxConf.bIncludeCrc         = 0x0, // Do not include CRC in queue
 *    .rxConf.bAppendRssi         = 0x0, // Do not append RSSI in queue
 *    .rxConf.bAppendStatus       = 0x0, // Do not append status byte in queue
 *    .rxConf.bAppendTimestamp    = 0x0, // Do not append time stamp of received packet in queue
 *    .syncWord0                  = 0x29417671,
 *    .syncWord1                  = 0x0,
 *    .numAddr                    = 1,   // Number of address entries in pAddrEntry
 *    .__dummy0                   = 0,
 *    .__dummy1                   = 0,
 *    .endTrigger.triggerType     = 1, // Trig never
 *    .endTrigger.bEnaCmd         = 0,
 *    .endTrigger.triggerNo       = 0,
 *    .endTrigger.pastTrig        = 0,
 *    .endTime                    = 0,
 *    .pAddrEntry                 = 0, //pointer to array of address entries
 *    .pRxQueue                   = 0,
 *    .pOutput                    = 0,
 *  };
 *
 *  // Struct used for the address entry
 *  rfc_hidAddrEntry_t addrEntry =
 *  {
 *    .addrConfig.ena0          = 1, // Enable entry for sync word 0
 *    .addrConfig.ena1          = 0, // Disable entry for sync word 0
 *    .addrConfig.autoAckMode   = 2, // Always enable auto-acknowledgement for the entry
 *    .addrConfig.bVarLen       = 1, // Use variable length in receiver when receiving packets
 *    .addrConfig.bFixedTxLen   = 0, // Use actual length in header when sending ACK
 *    .maxPktLen                = 100,
 *    .address                  = 0xEF, // Address of packet
 *    .pTxQueue                 = 0, // Pointer to transmit queue for acknowledgements in use for the address
 *    .crcVal                   = 0,
 *  };
 *  @endcode
 *
 *  @anchor rf_hid_settings_h
 *  @code
 *  // Filename: rf_hid_settings.h
 *
 *  #include <ti/devices/DeviceFamily.h>
 *  #include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
 *  #include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
 *  #include DeviceFamily_constructPath(driverlib/rf_hid_cmd.h)
 *  #include DeviceFamily_constructPath(driverlib/rf_data_entry.h)
 *  #include <ti/drivers/rf/RF.h>
 *
 *  extern RF_Mode RF_hid;
 *
 *  #define BUFFER_LEN 20 // Set the appropriate length here
 *
 *  // RF Core API commands
 *  extern rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup;
 *  extern rfc_CMD_FS_t RF_cmdFs;
 *  extern rfc_CMD_HID_TX_t RF_cmdTxHID;
 *  extern rfc_CMD_HID_RX_t RF_cmdRxHID;
 *  extern rfc_hidAddrEntry_t addrEntry;
 *
 *  // RF HID queue and statistics
 *  extern dataQueue_t txQ;
 *  extern dataQueue_t rxQ;
 *  extern uint8_t txEntry1[BUFFER_LEN];
 *  extern uint8_t rxEntry1[BUFFER_LEN];
 *  extern uint8_t txEntry2[BUFFER_LEN];
 *  extern uint8_t rxEntry2[BUFFER_LEN];
 *  extern rfc_hidRxTxOutput_t rxTxStatistics;
 *
 *  @endcode
 *
 *  <hr>
 *  @anchor rf_hid_Examples
 *  # Examples
 *  The following code example opens the RF driver, setup the radio for CMD_HID_TX,
 *  initiates the TX/RX queues so that the packet is being transmitted as fast
 *  as possible once the CMD_HID_TX is submitted.
 *
 *  ### Using the RF CMD_HID_TX without trigger #
 *  The example below does not use the CMD_HID_TRIGGER_TX, since the
 *  queues are committed to the command before the RF_cmdTxHID is submitted.
 *  The radio will evaluate the TX queue as containing data and proceed to start the
 *  transmission.
 *  The radio will continue to send packets and receive ACK's as long as there are
 *  enough space in the queues to allow reception of ACK's, TX queue is filled and
 *  retramission count is below the threshold, and no other error has occured.
 *  The command will terminate if the RX queue ran out of free entries,
 *  or the maximum number of retransmissions (RF_cmdTxHID.maxRetrans) was done,
 *  or the CMD_STOP/CMD_ABORT command was submitted,
 *  or any error occured.
 *  If the TX queue runs out of valid entires, the radio will enter the same wait state
 *  as if command is submitted without committed TX queues.
 *  @code
 *  // Initialize the radio
 *  RF_Params rfParams;
 *  RF_Params_init(&rfParams);
 *
 *  // Open the radio and submit setup command
 *  RF_Handle rfHandle = RF_open(pObj, &RF_hid, (RF_RadioSetup *)&RF_cmdRadioSetup, NULL);
 *  RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
 *  // Initialize the queues
 *  rfc_dataEntryGeneral_t *txEntry = (rfc_dataEntryGeneral_t *)txEntry1;
 *  txEntry->pNextEntry     = txEntry2;
 *  txEntry->status         = DATA_ENTRY_PENDING;
 *  txEntry->config.type    = DATA_ENTRY_TYPE_GEN;
 *  txEntry->config.irqIntv = 0;
 *  txEntry->config.lenSz   = 0;
 *  txEntry->config.type    = 0;
 *  txEntry->length         = 10; // Set the appropriate length here
 *
 *  txEntry = (rfc_dataEntryGeneral_t *)txEntry2;
 *  txEntry->pNextEntry     = txEntry1;
 *  txEntry->status         = DATA_ENTRY_PENDING;
 *  txEntry->config.type    = DATA_ENTRY_TYPE_GEN;
 *  txEntry->config.irqIntv = 0;
 *  txEntry->config.lenSz   = 0;
 *  txEntry->config.type    = 0;
 *  txEntry->length         = 10; // Set the appropriate length here
 *  txQ.pCurrEntry = txEntry1;
 *  txQ.pLastEntry = NULL;
 *
 *  rfc_dataEntryGeneral_t *rxEntry = (rfc_dataEntryGeneral_t*)rxEntry1;
 *  rxEntry->pNextEntry   = rxEntry2;
 *  rxEntry->status       = DATA_ENTRY_PENDING;
 *  rxEntry->config.type  = DATA_ENTRY_TYPE_GEN;
 *  rxEntry->config.lenSz = 1;
 *  rxEntry->length       = BUFFER_LEN-8;
 *
 *  rxEntry               = (rfc_dataEntryGeneral_t*)rxEntry2;
 *  rxEntry->pNextEntry   = rxEntry1;
 *  rxEntry->status       = DATA_ENTRY_PENDING;
 *  rxEntry->config.type  = DATA_ENTRY_TYPE_GEN;
 *  rxEntry->config.lenSz = 1;
 *  rxEntry->length       = BUFFER_LEN-8;
 *  rxQ.pCurrEntry = rxEntry1;
 *  rxQ.pLastEntry = NULL;
 *
 *  // Commit the queues and statistics to the TX command
 *  RF_cmdTxHID.pPkt = (uint8_t*)&txQ;
 *  RF_cmdTxHID.pRxQueue = &rxQ;
 *  RF_cmdTxHID.pOutput = &rxTxStatistics;
 *
 *  // Submit the command and handle the queue fill/empty in the hidTxCb callback
 *  RF_runCmd(rfHandle,
 *            (RF_Op*)&RF_cmdTxHID,
 *            RF_PriorityNormal,
 *            &hidTxCb,
 *            RF_EventTxDone | RF_EventTxEntryDone | RF_EventRxEntryDone | RF_EventLastCmdDone);
 *  @endcode
 *
 *
 *  ### Using the RF CMD_HID_TX with trigger #
 *  The example below is using the CMD_HID_TRIGGER_TX to trigger the transmission of
 *  the TX packet and ACK reception.
 *  The setup for the command is the same as for the example without the trigger, with
 *  the only difference being that the queues are commited to the command after it has
 *  been submitted.
 *  This is a way to enable faster TX start up, compared to regular command submission.
 *
 *  The example below is using the same data as the above.
 *  The transmission is then triggered 500 usec after CMD_HID_TX command submission.
 *  The delta between RF_runCmd(..., RF_cmdTxHID, ...) and TX start without trigger
 *  is greater than delta between RF_runDirectCmd(rfHandle, CMD_TRIGGER_HID_TX) and
 *  TX start, but will consume more power.
 *
 *  @code
 *  // Initialize the radio
 *  RF_Params rfParams;
 *  RF_Params_init(&rfParams);
 *
 *  // Open the radio and submit setup command
 *  RF_Handle rfHandle = RF_open(pObj, &RF_hid, (RF_RadioSetup *)&RF_cmdRadioSetup, NULL);
 *  RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
 *  // Initialize the queues
 *  rfc_dataEntryGeneral_t *txEntry = (rfc_dataEntryGeneral_t *)txEntry1;
 *  txEntry->pNextEntry     = txEntry2;
 *  txEntry->status         = DATA_ENTRY_PENDING;
 *  txEntry->config.type    = DATA_ENTRY_TYPE_GEN;
 *  txEntry->config.irqIntv = 0;
 *  txEntry->config.lenSz   = 0;
 *  txEntry->config.type    = 0;
 *  txEntry->length         = 10; // Set the appropriate length here
 *
 *  txEntry = (rfc_dataEntryGeneral_t *)txEntry2;
 *  txEntry->pNextEntry     = txEntry1;
 *  txEntry->status         = DATA_ENTRY_PENDING;
 *  txEntry->config.type    = DATA_ENTRY_TYPE_GEN;
 *  txEntry->config.irqIntv = 0;
 *  txEntry->config.lenSz   = 0;
 *  txEntry->config.type    = 0;
 *  txEntry->length         = 10; // Set the appropriate length here
 *  txQ.pCurrEntry = txEntry1;
 *  txQ.pLastEntry = NULL;
 *
 *  rfc_dataEntryGeneral_t *rxEntry = (rfc_dataEntryGeneral_t*)rxEntry1;
 *  rxEntry->pNextEntry   = rxEntry2;
 *  rxEntry->status       = DATA_ENTRY_PENDING;
 *  rxEntry->config.type  = DATA_ENTRY_TYPE_GEN;
 *  rxEntry->config.lenSz = 1;
 *  rxEntry->length       = BUFFER_LEN-8;
 *
 *  rxEntry               = (rfc_dataEntryGeneral_t*)rxEntry2;
 *  rxEntry->pNextEntry   = rxEntry1;
 *  rxEntry->status       = DATA_ENTRY_PENDING;
 *  rxEntry->config.type  = DATA_ENTRY_TYPE_GEN;
 *  rxEntry->config.lenSz = 1;
 *  rxEntry->length       = BUFFER_LEN-8;
 *  rxQ.pCurrEntry = rxEntry1;
 *  rxQ.pLastEntry = NULL;
 *
 *  RF_cmdTxHID.pRxQueue = &rxQ;
 *  RF_cmdTxHID.pOutput = &rxTxStatistics;
 *
 *  // Submit the command and handle the queue fill/empty in the hidTxCb callback
 *  RF_CmdHandle ch = RF_postCmd(rfHandle,
 *                    (RF_Op*)&RF_cmdTxHID,
 *                    RF_PriorityNormal,
 *                    &hidTxCb,
 *                    RF_EventTxDone | RF_EventTxEntryDone | RF_EventRxEntryDone | RF_EventLastCmdDone);
 *  // Wait 500 usec
 *  usleep(500);
 *
 *  // Commit the queues and statistics to the TX command
 *  RF_cmdTxHID.pPkt = (uint8_t*)&txQ;
 *
 *  // Submit the trigger
 *  RF_runDirectCmd(rfHandle, CMD_TRIGGER_HID_TX);
 *
 *  // Pend on the command end.
 *  RF_pendCmd(rfHandle, ch, RF_EventCmdDone);
 *
 *  @endcode
 *
 *
 *
 *  ### Using the RF CMD_HID_RX command #
 *  The CMD_HID_RX is not using the trigger as done with the CMD_HID_TX, instead
 *  the reception is handled by the startTrigger.
 *
 *  The TX queues are used for sending the ACK's, and the TX queue to use is decided
 *  by the address entry, meaning that it's possible to have different TX queues for
 *  different received addresses, allowing for different ACK's for different addresses.
 *
 *  If pktConf.bIncludeAddr is zero, meaning that no address is sent, the address entry
 *  used will be the first entry that matches the received sync word with the sync word
 *  ID in the address entry.
 *
 *  The command will terminate if the RX queue is full, CMD_STOP/CMD_ABORT is being sent
 *  or if any error occured.
 *
 *  An example of the CMD_HID_RX is shown below.
 *  @code
 *  // Initialize the radio
 *  RF_Params rfParams;
 *  RF_Params_init(&rfParams);
 *
 *  // Open the radio and submit setup command
 *  RF_Handle rfHandle = RF_open(pObj, &RF_hid, (RF_RadioSetup *)&RF_cmdRadioSetup, NULL);
 *  RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
 *  // Initialize the queues
 *  rfc_dataEntryGeneral_t *txEntry = (rfc_dataEntryGeneral_t *)txEntry1;
 *  txEntry->pNextEntry     = txEntry2;
 *  txEntry->status         = DATA_ENTRY_PENDING;
 *  txEntry->config.type    = DATA_ENTRY_TYPE_GEN;
 *  txEntry->config.irqIntv = 0;
 *  txEntry->config.lenSz   = 0;
 *  txEntry->config.type    = 0;
 *  txEntry->length         = 10; // Set the appropriate length here
 *
 *  txEntry = (rfc_dataEntryGeneral_t *)txEntry2;
 *  txEntry->pNextEntry     = txEntry1;
 *  txEntry->status         = DATA_ENTRY_PENDING;
 *  txEntry->config.type    = DATA_ENTRY_TYPE_GEN;
 *  txEntry->config.irqIntv = 0;
 *  txEntry->config.lenSz   = 0;
 *  txEntry->config.type    = 0;
 *  txEntry->length         = 10; // Set the appropriate length here
 *
 *  rfc_dataEntryGeneral_t *rxEntry = (rfc_dataEntryGeneral_t*)rxEntry1;
 *  rxEntry->pNextEntry   = rxEntry2;
 *  rxEntry->status       = DATA_ENTRY_PENDING;
 *  rxEntry->config.type  = DATA_ENTRY_TYPE_GEN;
 *  rxEntry->config.lenSz = 1;
 *  rxEntry->length       = BUFFER_LEN-8;
 *
 *  rxEntry               = (rfc_dataEntryGeneral_t*)rxEntry2;
 *  rxEntry->pNextEntry   = rxEntry1;
 *  rxEntry->status       = DATA_ENTRY_PENDING;
 *  rxEntry->config.type  = DATA_ENTRY_TYPE_GEN;
 *  rxEntry->config.lenSz = 1;
 *  rxEntry->length       = BUFFER_LEN-8;
 *  rxQ.pCurrEntry = rxEntry1;
 *  rxQ.pLastEntry = NULL;
 *
 *  // Attach the TX queue to the address entry. Used for ACK's
 *  addrEntry.pTxQueue = &txQ;
 *  RF_cmdRxHID.pAddrEntry = &addrEntry;
 *  RF_cmdRxHID.pRxQueue = &rxQ;
 *  RF_cmdRxHID.pOutput = &rxTxStatistics;
 *
 *  // Submit the command and handle the queue fill/empty in the hidRxCb callback
 *  RF_runCmd(rfHandle,
 *            (RF_Op*)&RF_cmdRxHID,
 *            RF_PriorityNormal,
 *            &hidRxCb,
 *            RF_EventTxDone | RF_EventTxEntryDone | RF_EventRxEntryDone | RF_EventLastCmdDone);
 *  @endcode
 *
 *
 *  <hr>
 *
 *  ============================================================================
 */
#ifndef __HID_CMD_H
#define __HID_CMD_H

#ifndef __RFC_STRUCT
#define __RFC_STRUCT
#endif

#ifndef __RFC_STRUCT_ATTR
#if defined(__GNUC__)
#define __RFC_STRUCT_ATTR __attribute__ ((aligned (4)))
#elif defined(__TI_ARM__)
#define __RFC_STRUCT_ATTR __attribute__ ((__packed__,aligned (4)))
#else
#define __RFC_STRUCT_ATTR
#endif
#endif

//! \addtogroup rfc
//! @{

//! \addtogroup hid_cmd
//! @{

#include <stdint.h>
#include "rf_mailbox.h"
#include "rf_common_cmd.h"

typedef struct __RFC_STRUCT rfc_CMD_HID_TX_s rfc_CMD_HID_TX_t;
typedef struct __RFC_STRUCT rfc_CMD_HID_RX_s rfc_CMD_HID_RX_t;
typedef struct __RFC_STRUCT rfc_hidAddrEntry_s rfc_hidAddrEntry_t;
typedef struct __RFC_STRUCT rfc_hidRxTxOutput_s rfc_hidRxTxOutput_t;
typedef struct __RFC_STRUCT rfc_hidRxStatus_s rfc_hidRxStatus_t;

//! \addtogroup CMD_HID_TX
//! @{
#define CMD_HID_TX                                              0x5801
//! HID Transmit Command with Auto Retransmission
struct __RFC_STRUCT rfc_CMD_HID_TX_s {
   uint16_t commandNo;                  //!<        The command ID number 0x5801
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   struct {
      uint8_t bFsOff:1;                 //!< \brief 0: Keep frequency synth on after command<br>
                                        //!<        1: Turn frequency synth off after command
      uint8_t bAutoRetransmit:1;        //!< \brief 0: Do not listen for ACK<br>
                                        //!<        1: Listen for ACK and retransmit if missing
      uint8_t bVarLen:1;                //!< \brief 0: Fixed length mode<br>
                                        //!<        1: Variable length mode
      uint8_t hdrMode:1;                //!< \brief 0: 9-bit header<br>
                                        //!<        1: 10-bit header
      uint8_t bIncludeAddr:1;           //!< \brief 0: Do not include address byte after sync word<br>
                                        //!<        1: Include address byte after sync word
      uint8_t hdrConf:2;                //!< \brief 0: Automatically generate header (no header byte in buffer\)<br>
                                        //!<        1: Insert NO_ACK field from TX buffer<br>
                                        //!<        2: Insert SEQ field from TX buffer<br>
                                        //!<        3: Insert SEQ and NO_ACK field from TX buffer
      uint8_t bFixedTxLen:1;            //!< \brief 0: Use actual length in header when sending packet<br>
                                        //!<        1: Use fixed word in length field of header when sending packet
                                        //!<        (only for peer without variable length packets)
   } pktConf;
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets (RX) or empty ACKs (TX) from RX queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bIncludeAddr:1;           //!<        If 1, the received address byte is included in the Rx queue
      uint8_t bIncludeHdr:1;            //!<        If 1, the received header is included in the Rx queue
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConf;                            //!<        Receive entry configuration
   uint32_t syncWord;                   //!<        Sync word to send
   uint8_t address;                     //!<        Address byte
   uint8_t seqNo;                       //!<        Sequence number to use for next packet
   uint8_t maxAckLen;                   //!<        Maximum length of ACKs
   uint8_t pktLen;                      //!<        Length of transmitted packet
   uint8_t maxRetrans;                  //!<        Maximum number of retransmissions
   struct {
      uint8_t noAckVal:2;               //!< \brief 0: Set automatic NO_ACK value to inverse of bAutoRetransmit<br>
                                        //!<        1: Set automatic NO_ACK value to bAutoRetransmit<br>
                                        //!<        2: Set automatic NO_ACK value to 0<br>
                                        //!<        3: Set automatic NO_ACK value to 1
      uint8_t bAlwaysAutoRetransmit:1;  //!< \brief 0: Disable auto retransmit if transmitted NO_ACK was 1<br>
                                        //!<        1: Never disable auto retransmit
   } noAckMode;
   uint16_t retransDelay;               //!<        Number of RAT ticks from start of transmission of a packet to retransmission
   uint8_t* pPkt;                       //!<        Pointer to transmit queue for packets
   dataQueue_t* pRxQueue;               //!<        Pointer to receive queue for ACKs
   rfc_hidRxTxOutput_t *pOutput;        //!<        Pointer to output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_HID_RX
//! @{
#define CMD_HID_RX                                              0x5802
//! HID Recieve Command with Auto Ack
struct __RFC_STRUCT rfc_CMD_HID_RX_s {
   uint16_t commandNo;                  //!<        The command ID number 0x5802
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   struct {
      uint8_t bFsOff:1;                 //!< \brief 0: Keep frequency synth on after command<br>
                                        //!<        1: Turn frequency synth off after command
      uint8_t bRepeatOk:1;              //!< \brief 0: End operation after receiving a packet correctly<br>
                                        //!<        1: Go back to sync search after receiving a packet correctly
      uint8_t bRepeatNok:1;             //!< \brief 0: End operation after receiving a packet with CRC error<br>
                                        //!<        1: Go back to sync search after receiving a packet with CRC error
      uint8_t hdrMode:1;                //!< \brief 0: 9-bit header<br>
                                        //!<        1: 10-bit header
      uint8_t bIncludeAddr:1;           //!< \brief 0: Include address byte after sync word<br>
                                        //!<        1: Do not include address byte after sync word
      uint8_t hdrConf:2;                //!< \brief 0: Automatically generate header (no header byte in buffer)
                                        //!<        1: Insert NO_ACK field from TX buffer
                                        //!<        2: Insert SEQ field from TX buffer
                                        //!<        3: Insert SEQ and NO_ACK field from TX buffer
   } pktConf;
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets (RX) or empty ACKs (TX) from RX queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bIncludeAddr:1;           //!<        If 1, the received address byte is included in the Rx queue
      uint8_t bIncludeHdr:1;            //!<        If 1, the received header is included in the Rx queue
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConf;                            //!<        Receive entry configuration
   uint32_t syncWord0;                  //!<        Sync word to listen for
   uint32_t syncWord1;                  //!<        Alternative sync word if non-zero
   uint8_t numAddr;                     //!<        Number of address entries
   uint8_t __dummy0;
   uint8_t __dummy1;
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger classifier for ending the operation
   ratmr_t endTime;                     //!<        Time used together with <code>endTrigger</code> for ending the operation
   rfc_hidAddrEntry_t *pAddrEntry;      //!<        Pointer to array of address entries
   dataQueue_t* pRxQueue;               //!<        Pointer to receive queue
   rfc_hidRxTxOutput_t *pOutput;        //!<        Pointer to output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup hidAddrEntry
//! @{
//! Address entry structure for CMD_HID_RX

struct __RFC_STRUCT rfc_hidAddrEntry_s {
   struct {
      uint8_t ena0:1;                   //!< \brief 0: Disable entry for syncWord0<br>
                                        //!<        1: Enable entry for syncWord0
      uint8_t ena1:1;                   //!< \brief 0: Disable entry for syncWord1<br>
                                        //!<        1: Enable entry for syncWord1
      uint8_t autoAckMode:2;            //!< \brief 0: Always disable auto-acknowledgement for the entry<br>
                                        //!<        1: Always enable auto-acknowledgement for the entry<br>
                                        //!<        2: Enable auto-acknowledgement for the entry only if received NO_ACK bit is 0<br>
                                        //!<        3: Enable auto-acknowledgement for the entry only if received NO_ACK bit is 1
      uint8_t bVarLen:1;                //!< \brief 0: Use fixed length given by maxPktLen in receiver when receiving packets<br>
                                        //!<        1: Use variable length in receiver when receiving packets
      uint8_t bFixedTxLen:1;            //!< \brief 0: Use actual length in header when sending ACK
                                        //!<        1: Use fixed word in length field of header when sending ACK and no payload
                                        //!<        (only for peer without variable length ACKs)
   } addrConfig;
   uint8_t maxPktLen;                   //!<        Packet length for fixed length, maximum packet length for variable length
   uint8_t address;                     //!<        Address byte of packet
   struct {
      uint8_t bValid:1;                 //!< \brief 0: The status is not valid. Any packet is viewed as new.<br>
                                        //!<        1: The status is valid. Only packets with a sequence number and CRC different
                                        //!<        from the previous one are accepted.
      uint8_t seq:2;                    //!<        Sequence number of last successfully received packet
      uint8_t ackSeq:2;                 //!<        Sequence number of the next or current ACK to be transmitted
      uint8_t bAckPayloadSent:1;        //!< \brief 0: The last received packet was not acknowledged with payload.<br>
                                        //!<        1: The last received packet was acknowledged with payload.
   } seqStat;
   dataQueue_t* pTxQueue;               //!<        Pointer to transmit queue for acknowledgements in use for the address
   uint16_t crcVal;                     //!<        CRC value (last two bytes if more than 2 CRC bytes) of last successfully received packet
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup hidRxTxOutput
//! @{
//! Output structure for CMD_HID_RX and CMD_HID_TX

struct __RFC_STRUCT rfc_hidRxTxOutput_s {
   uint8_t nTx;                         //!<        Number of packets or acknowledgements transmitted
   uint8_t nRxOk;                       //!<        Number of packets that have been received with CRC OK
   uint8_t nRxNok;                      //!<        Number of packets that have been received with CRC error
   uint8_t nRxIgnored;                  //!<        Number of packets ignored as retransmissions or empty ACKs
   uint8_t nRxBufFull;                  //!<        Number of packets that have been received and discarded due to lack of buffer space
   uint8_t nRxAborted;                  //!<        Number of packets not received due to device address mismatch, invalid length, or abort command
   uint8_t __dummy0;
   int8_t lastRssi;                     //!<        RSSI of last received packet
   ratmr_t timeStamp;                   //!<        Time stamp of last received packet
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup hidRxStatus
//! @{
//! Receive status byte that may be appended to message in receive buffer

struct __RFC_STRUCT rfc_hidRxStatus_s {
   struct {
      uint8_t addrInd:5;                //!<        Index of address found
      uint8_t syncWordId:1;             //!<        0 for primary sync word, 1 for alternate sync word
      uint8_t bIgnore:1;                //!<        1 if the packet is marked as ignored, 0 otherwise
      uint8_t bCrcErr:1;                //!<        1 if the packet was received with CRC error, 0 otherwise
   } status;
} __RFC_STRUCT_ATTR;

//! @}

//! @}
//! @}
#endif
