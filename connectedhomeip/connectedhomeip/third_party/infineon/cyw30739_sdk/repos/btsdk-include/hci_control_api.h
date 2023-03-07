/***************************************************************************//**
* \file <hci_control_api.h>
*
* Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*
*	HCI Control Protocol Definitions
*
* \brief
* 	This file provides definitions for the AIROC HCI Control Interface between an MCU
* 	and an embedded application running on an Infineon Bluetooth SoC. Please refer to the AIROC
* 	HCI Control Protocol Software User Manual (002-16618) for additional details.
*
* Packets exchanged over the UART between MCU and hci_control applications contain a 5-byte header.
* -------------------------------------------------------------------------------------------------------
* |  Packet Type      | Command Code          |    Group Code       |        Packet Length              |
* -------------------------------------------------------------------------------------------------------
* |HCI_WICED_PKT(0x19)|HCI_CONTROL_COMMAND_...|HCI_CONTROL_GROUP_...|length(low byte)| length(high byte)|
* -------------------------------------------------------------------------------------------------------
******************************************************************************/
#ifndef __HCI_CONTROL_API_H
#define __HCI_CONTROL_API_H

/* Packet types */
#define HCI_EVENT_PKT                                       4
#define HCI_ACL_DATA_PKT                                    2
#define HCI_WICED_PKT                                       25

/* Group codes */
#define HCI_CONTROL_GROUP_DEVICE                              0x00
#define HCI_CONTROL_GROUP_LE                                  0x01
#define HCI_CONTROL_GROUP_GATT                                0x02
#define HCI_CONTROL_GROUP_HF                                  0x03
#define HCI_CONTROL_GROUP_SPP                                 0x04
#define HCI_CONTROL_GROUP_AUDIO                               0x05
#define HCI_CONTROL_GROUP_HIDD                                0x06
#define HCI_CONTROL_GROUP_AVRC_TARGET                         0x07
#define HCI_CONTROL_GROUP_TEST                                0x08
#define HCI_CONTROL_GROUP_ANCS                                0x0b
#define HCI_CONTROL_GROUP_ALERT                               0x0c
#define HCI_CONTROL_GROUP_IAP2                                0x0d
#define HCI_CONTROL_GROUP_AG                                  0x0e
#define HCI_CONTROL_GROUP_BSG                                 0x10
#define HCI_CONTROL_GROUP_AVRC_CONTROLLER                     0x11
#define HCI_CONTROL_GROUP_AMS                                 0x12
#define HCI_CONTROL_GROUP_HIDH                                0x13
#define HCI_CONTROL_GROUP_AUDIO_SINK                          0x14
#define HCI_CONTROL_GROUP_PBC                                 0x15
#define HCI_CONTROL_GROUP_MESH                                0x16
#define HCI_CONTROL_GROUP_BATT_CLIENT                         0x17
#define HCI_CONTROL_GROUP_FINDME_LOCATOR                      0x18
#define HCI_CONTROL_GROUP_DEMO                                0x19
#define HCI_CONTROL_GROUP_OPS                                 0x20
#define HCI_CONTROL_GROUP_ANC                                 0x21
#define HCI_CONTROL_GROUP_ANS                                 0x22
#define HCI_CONTROL_GROUP_LE_COC                              0x23
#define HCI_CONTROL_GROUP_LED_DEMO                            0x24
#define HCI_CONTROL_GROUP_SCRIPT                              0x25
#define HCI_CONTROL_GROUP_OTP                                 0x26
#define HCI_CONTROL_GROUP_MCE                                 0x27
#define HCI_CONTROL_GROUP_HK                                  0x28
#define HCI_CONTROL_GROUP_HCI_AUDIO                           0x29
#define HCI_CONTROL_GROUP_DFU                                 0x2a
#define HCI_CONTROL_GROUP_AUDIO_DUAL_A2DP                     0x2b
#define HCI_CONTROL_GROUP_HCITEST                             0x2c
#define HCI_CONTROL_GROUP_MESH_MODELS                         0x2d
#define HCI_CONTROL_GROUP_CONN_MESH                           0x2e
#define HCI_CONTROL_GROUP_PANU                                0x2f
#define HCI_CONTROL_GROUP_MISC                                0xFF

#define HCI_CONTROL_GROUP(x) ((((x) >> 8)) & 0xff)

/* General purpose commands */
#define HCI_CONTROL_COMMAND_RESET                           ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x01 )    /* Restart controller */
#define HCI_CONTROL_COMMAND_TRACE_ENABLE                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x02 )    /* Enable or disable WICED traces */
#define HCI_CONTROL_COMMAND_SET_LOCAL_BDA                   ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x03 )    /* Set local device addrsss */
#define HCI_CONTROL_COMMAND_PUSH_NVRAM_DATA                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x05 )    /* Download previously saved NVRAM chunk */
#define HCI_CONTROL_COMMAND_DELETE_NVRAM_DATA               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x06 )    /* Delete NVRAM chunk currently stored in RAM */
#define HCI_CONTROL_COMMAND_INQUIRY                         ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x07 )    /* Start/stop inquiry */
#define HCI_CONTROL_COMMAND_SET_VISIBILITY                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x08 )    /* Set BR/EDR connectability and discoverability of the device */
#define HCI_CONTROL_COMMAND_SET_PAIRING_MODE                ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x09 )    /* Set Pairing Mode for the device 0 = Not pairable 1 = Pairable */
#define HCI_CONTROL_COMMAND_UNBOND                          ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0A )    /* Delete bond with specified BDADDR */
#define HCI_CONTROL_COMMAND_USER_CONFIRMATION               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0B )    /* User Confirmation during pairing, TRUE/FALSE passed as parameter */
#define HCI_CONTROL_COMMAND_ENABLE_COEX                     ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0C )    /* Enable coex functionality */
#define HCI_CONTROL_COMMAND_DISABLE_COEX                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0D )    /* Disable coex functionality */
#define HCI_CONTROL_COMMAND_SET_BATTERY_LEVEL               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0E )    /* Sets battery level in the GATT database */
#define HCI_CONTROL_COMMAND_READ_LOCAL_BDA                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0F )    /* Get local device addrsss */
#define HCI_CONTROL_COMMAND_BOND                            ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x10 )    /* Initiate Bonding with a peer device */
#define HCI_CONTROL_COMMAND_READ_BUFF_STATS                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x11 )    /* Read Buffer statistics */
#define HCI_CONTROL_COMMAND_SET_LOCAL_NAME                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x12 )    /* Set the local name */
#define HCI_CONTROL_COMMAND_UNBOND_DEVICE                   ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x13 )    /* Unbond (Delete NVRAM Id) a Device (by BdAddr) */
#define HCI_CONTROL_COMMAND_GET_BONDED_DEVICES              ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x14 )    /* Get Bonded/Paired Device list */
#define HCI_CONTROL_COMMAND_PIN_REPLY                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x15 )    /* PIN Code Reply */
#define HCI_CONTROL_COMMAND_SSP_REPLY                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x16 )    /* SSP Code Reply */


/* LE Commands */
#define HCI_CONTROL_LE_COMMAND_SCAN                         ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x01 )    /* Start scan */
#define HCI_CONTROL_LE_COMMAND_ADVERTISE                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x02 )    /* Start advertisements */
#define HCI_CONTROL_LE_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x03 )    /* Connect to peer */
#define HCI_CONTROL_LE_COMMAND_CANCEL_CONNECT               ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x04 )    /* Cancel connect */
#define HCI_CONTROL_LE_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x05 )    /* Disconnect */
#define HCI_CONTROL_LE_RE_PAIR                              ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x06 )    /* Delete keys and then re-pair */
#define HCI_CONTROL_LE_COMMAND_GET_IDENTITY_ADDRESS         ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x07 )    /* Get identity address */
#define HCI_CONTROL_LE_COMMAND_SET_CHANNEL_CLASSIFICATION   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x08 )    /* Set channel classification for available 40 channels */
#define HCI_CONTROL_LE_COMMAND_SET_CONN_PARAMS              ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x09 )    /* Set connection parameters */
#define HCI_CONTROL_LE_COMMAND_SET_RAW_ADVERTISE_DATA       ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x0a )    /* Set raw advertisement data */

/* GATT Commands */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_SERVICES          ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x01 )    /* Discover services */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_CHARACTERISTICS   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x02 )    /* Discover characteristics */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_DESCRIPTORS       ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x03 )    /* Discover descriptors */
#define HCI_CONTROL_GATT_COMMAND_READ_REQUEST               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x04 )    /* Send read request */
#define HCI_CONTROL_GATT_COMMAND_READ_RESPONSE              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x05 )    /* Send read response */
#define HCI_CONTROL_GATT_COMMAND_WRITE_COMMAND              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x06 )    /* Send write command */
#define HCI_CONTROL_GATT_COMMAND_WRITE_REQUEST              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x07 )    /* Send write request */
#define HCI_CONTROL_GATT_COMMAND_WRITE_RESPONSE             ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x08 )    /* Send write response */
#define HCI_CONTROL_GATT_COMMAND_NOTIFY                     ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x09 )    /* Send notification */
#define HCI_CONTROL_GATT_COMMAND_INDICATE                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0a )    /* Send indication */
#define HCI_CONTROL_GATT_COMMAND_INDICATE_CONFIRM           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0b )    /* Send indication confirmation */
#define HCI_CONTROL_GATT_COMMAND_DB_INIT                    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0d )    /* Initialize GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_PRIMARY_SERVICE_ADD     ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0e )	  /* Add primary service into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_SECONDARY_SERVICE_ADD   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0f )	  /* Add secondary service into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_INCLUDED_SERVICE_ADD    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x10 )	  /* Add included service into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_CHARACTERISTIC_ADD      ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x11 )	  /* Add characteristic into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_DESCRIPTOR_ADD          ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x12 )	  /* Add descriptor into GATT database */

/* Handsfree Commands */
#define HCI_CONTROL_HF_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x01 )    /* Establish connection to HF Audio Gateway */
#define HCI_CONTROL_HF_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x02 )    /* Release HF connection */
#define HCI_CONTROL_HF_COMMAND_OPEN_AUDIO                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x03 )    /* Create audio connection on existing service level connection */
#define HCI_CONTROL_HF_COMMAND_CLOSE_AUDIO                  ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x04 )    /* Disconnect audio */
#define HCI_CONTROL_HF_COMMAND_AUDIO_ACCEPT_CONN            ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x05 )    /* Accept/Reject Audio connection request */
#define HCI_CONTROL_HF_COMMAND_TURN_OFF_PCM_CLK             ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x06 )    /* To turn off PCM/I2S clock in Master case for SCO */
#define HCI_CONTROL_HF_COMMAND_BUTTON_PRESS                 ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x07 )    /* Simulate a button press on a Hands-free headset */
#define HCI_CONTROL_HF_COMMAND_LONG_BUTTON_PRESS            ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x08 )    /* Simulate a long button press on a Hands-free headset */

/* Sub commands to send various AT Commands */
#define HCI_CONTROL_HF_AT_COMMAND_BASE                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x20 )    /* Send AT command and supporting data */
#define HCI_CONTROL_HF_AT_COMMAND_SPK                       0x00    /* Update speaker volume */
#define HCI_CONTROL_HF_AT_COMMAND_MIC                       0x01    /* Update microphone volume */
#define HCI_CONTROL_HF_AT_COMMAND_A                         0x02    /* Answer incoming call */
#define HCI_CONTROL_HF_AT_COMMAND_BINP                      0x03    /* Retrieve number from voice tag */
#define HCI_CONTROL_HF_AT_COMMAND_BVRA                      0x04    /* Enable/Disable voice recognition */
#define HCI_CONTROL_HF_AT_COMMAND_BLDN                      0x05    /* Last Number redial */
#define HCI_CONTROL_HF_AT_COMMAND_CHLD                      0x06    /* Call hold command */
#define HCI_CONTROL_HF_AT_COMMAND_CHUP                      0x07    /* Call hang up command */
#define HCI_CONTROL_HF_AT_COMMAND_CIND                      0x08    /* Read Indicator Status */
#define HCI_CONTROL_HF_AT_COMMAND_CNUM                      0x09    /* Retrieve Subscriber number */
#define HCI_CONTROL_HF_AT_COMMAND_D                         0x0A    /* Place a call using a number or memory dial */
#define HCI_CONTROL_HF_AT_COMMAND_NREC                      0x0B    /* Disable Noise reduction and echo canceling in AG */
#define HCI_CONTROL_HF_AT_COMMAND_VTS                       0x0C    /* Transmit DTMF tone */
#define HCI_CONTROL_HF_AT_COMMAND_BTRH                      0x0D    /* CCAP incoming call hold */
#define HCI_CONTROL_HF_AT_COMMAND_COPS                      0x0E    /* Query operator selection */
#define HCI_CONTROL_HF_AT_COMMAND_CMEE                      0x0F    /* Enable/disable extended AG result codes */
#define HCI_CONTROL_HF_AT_COMMAND_CLCC                      0x10    /* Query list of current calls in AG */
#define HCI_CONTROL_HF_AT_COMMAND_BIA                       0x11    /* Activate/Deactivate indicators */
#define HCI_CONTROL_HF_AT_COMMAND_BIEV                      0x12    /* Send HF indicator value to peer */
#define HCI_CONTROL_HF_AT_COMMAND_UNAT                      0x13    /* Transmit AT command not in the spec  */
#define HCI_CONTROL_HF_AT_COMMAND_MAX                       0x13    /* For command validation */

/* Serial Port Profile commands */
#define HCI_CONTROL_SPP_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x01 )    /* Establish connection to SPP server */
#define HCI_CONTROL_SPP_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x02 )    /* Release SPP connection */
#define HCI_CONTROL_SPP_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x03 )    /* Send data */
#define HCI_CONTROL_SPP_COMMAND_PEER_MTU                    ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x04 )    /* Get peer mtu size */

/* Audio Profile commands */
#define HCI_CONTROL_AUDIO_COMMAND_CONNECT                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x01 )    /* Audio connect to sink */
#define HCI_CONTROL_AUDIO_COMMAND_DISCONNECT                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x02 )    /* Audio disconnect  */
#define HCI_CONTROL_AUDIO_START                             ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x03 )    /* Start audio with specific sample rate/mode */
#define HCI_CONTROL_AUDIO_STOP                              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x04 )    /* Stop audio */
#define HCI_CONTROL_AUDIO_PACKET_COUNT                      ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x05 )    /* Debug packet counter sent from host */
#define HCI_CONTROL_AUDIO_DATA                              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x06 )    /* Audio data (PCM) */
#define HCI_CONTROL_AUDIO_READ_STATISTICS                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x07 )    /* Audio Statistics */
#define HCI_CONTROL_AUDIO_DATA_MP3                          ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x10 )    /* Audio data (MP3) */
#define HCI_CONTROL_AUDIO_DATA_FORMAT                       ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x11 )    /* Audio data format */

/* AVRC Target Profile commands */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x01 )    /* Initiate connection to the peer. */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x02 )    /* Disconnect connection to the peer. */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_TRACK_INFO                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x05 )    /* Track info sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_PLAYER_STATUS               ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x06 )    /* Player status info sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_REPEAT_MODE_CHANGE          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x07 )    /* Repeat Mode changes sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_SHUFFLE_MODE_CHANGE         ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x08 )    /* Shuffle Mode changes sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_EQUALIZER_STATUS_CHANGE     ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x09 )    /* EQ Status changes sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_SCAN_STATUS_CHANGE          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0A )    /* Scan Status changes sent to embedded app */

#define HCI_CONTROL_AVRC_TARGET_COMMAND_REGISTER_NOTIFICATION       ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x99 )    /* Register for notifications (PTS only) */

/* AVRC Controller Profile commands */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_CONNECT                 ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x01 )    /* Initiate connection to the peer */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_DISCONNECT              ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x02 )    /* Disconnect from the peer */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_PLAY                    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x03 )    /* Send play command to the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_STOP                    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x04 )    /* Send stop command to the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_PAUSE                   ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x05 )    /* Send pause command to the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_BEGIN_FAST_FORWARD      ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x06 )    /* Start fast forward on the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_END_FAST_FORWARD        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x07 )    /* End fast forward on the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_BEGIN_REWIND            ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x08 )    /* Passthrough Rewind command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_END_REWIND              ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x09 )    /* Passthrough Rewind command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_NEXT_TRACK              ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0a )    /* Passthrough Next command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_PREVIOUS_TRACK          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0b )    /* Passthrough Prev command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_VOLUME_UP               ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0c )    /* Passthrough Vol Up command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_VOLUME_DOWN             ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0d )    /* Passthrough Vol Down command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_GET_TRACK_INFO          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0e )    /* Get Track Metadata */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_EQUALIZER_STATUS    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0f )    /* Turn Equalizer On/Off */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_REPEAT_MODE         ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x10 )    /* Set Repeat mode */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_SHUFFLE_MODE        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x11 )    /* Set Shuffle mode */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_SCAN_STATUS         ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x12 )    /* Set Scan mode to Off, All tracks or Group scan  */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_VOLUME_LEVEL            ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x13 )    /* Set Absolute Volume */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_GET_PLAY_STATUS         ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x14 )    /* Get play-status command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_POWER                   ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x15 )    /* Passthrough Power command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_MUTE                    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x16 )    /* Passthrough Mute command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_BUTTON_PRESS            ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x17 )    /* Simulate a button press on a stereo headphone */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_LONG_BUTTON_PRESS       ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x18 )    /* Simulate a long button press on a stereo headphone */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_UNIT_INFO               ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x19 )    /* Send Unit Info command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SUB_UNIT_INFO           ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x1A )    /* Send Sub Unit Info command */

/* HID Device commands */
#define HCI_CONTROL_HIDD_COMMAND_ACCEPT_PAIRING             ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x01 )     /* Set device discoverable/connectable to accept pairing */
#define HCI_CONTROL_HIDD_COMMAND_SEND_REPORT                ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x02 )     /* Send HID report */
#define HCI_CONTROL_HIDD_COMMAND_PUSH_PAIRING_HOST_INFO     ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x03 )     /* Paired host address and link keys */
#define HCI_CONTROL_HIDD_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x04 )     /* Connect to previously paired host */
#define HCI_CONTROL_HIDD_COMMAND_HID_HOST_ADDR              ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x05 )     /* Paired host address */
#define HCI_CONTROL_HIDD_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x06 )     /* Disconnect HID connection */
#define HCI_CONTROL_HIDD_COMMAND_VIRTUAL_UNPLUG             ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x07 )     /* Send Virtual Unplug */
#define HCI_CONTROL_HIDD_COMMAND_KEY                        ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x08 )     /* Send USB key */

/* Test commands */
#define HCI_CONTROL_TEST_COMMAND_ENCAPSULATED_HCI_COMMAND   ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x10 )     /* Encapsulated HCI command - For manufacturing test purposes */

/* ANCS commands */
#define HCI_CONTROL_ANCS_COMMAND_ACTION                     ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x01 )      /* ANCS notification */
#define HCI_CONTROL_ANCS_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x02 )      /* Establish ANCS connection */
#define HCI_CONTROL_ANCS_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x03 )      /* Disconnect ANCS */

/* ANC commands */
#define HCI_CONTROL_ANC_COMMAND_READ_SERVER_SUPPORTED_NEW_ALERTS        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x01 )   /* Command to read server supported new alerts */
#define HCI_CONTROL_ANC_COMMAND_READ_SERVER_SUPPORTED_UNREAD_ALERTS     ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x02 )   /* Command to read server supported unread alerts */
#define HCI_CONTROL_ANC_COMMAND_CONTROL_ALERTS                          ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x03 )   /* Command to enable/diable/notify pending or all pending alerts */
#define HCI_CONTROL_ANC_COMMAND_ENABLE_NEW_ALERTS                       ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x04 )   /* Command to enable new alerts notifications */
#define HCI_CONTROL_ANC_COMMAND_ENABLE_UNREAD_ALERTS                    ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x05 )   /* Command to enable unread alerts notifications */
#define HCI_CONTROL_ANC_COMMAND_DISABLE_NEW_ALERTS                      ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x06 )   /* Command to disable new alerts notifications */
#define HCI_CONTROL_ANC_COMMAND_DISABLE_UNREAD_ALERTS                   ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x07 )   /* Command to disable unread alerts notifications */

/* ANS commands */
#define HCI_CONTROL_ANS_COMMAND_SET_SUPPORTED_NEW_ALERT_CATEGORIES      ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x01 ) /* Command to configure supported new alerts */
#define HCI_CONTROL_ANS_COMMAND_SET_SUPPORTED_UNREAD_ALERT_CATEGORIES   ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x02 ) /* Command to configure supported unread alerts  */
#define HCI_CONTROL_ANS_COMMAND_GENERATE_ALERT                          ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x03 ) /* Command to generate new alert and unread alert of requested category */
#define HCI_CONTROL_ANS_COMMAND_CLEAR_ALERT                             ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x04 ) /* Command to clear new alert and unread alert count of requested category */

/* AMS commands */
#define HCI_CONTROL_AMS_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x01 )       /* Establish AMS connection */
#define HCI_CONTROL_AMS_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x02 )       /* Disconnect AMS */

/* IAP2 commands */
#define HCI_CONTROL_IAP2_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x01 )    /* Establish connection to SPP server */
#define HCI_CONTROL_IAP2_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x02 )    /* Release SPP connection */
#define HCI_CONTROL_IAP2_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x03 )    /* Send data */
#define HCI_CONTROL_IAP2_COMMAND_GET_AUTH_CHIP_INFO          ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x04 )    /* Send get auth chip info */
#define HCI_CONTROL_IAP2_COMMAND_GET_AUTH_CHIP_CERTIFICATE   ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x05 )    /* Send get auth chip info */
#define HCI_CONTROL_IAP2_COMMAND_GET_AUTH_CHIP_SIGNATURE     ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x06 )    /* Send get auth chip info */
#define HCI_CONTROL_IAP2_COMMAND_SEND_IDENTITY_DATA          ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x07 )    /* Send identity data */
#define HCI_CONTROL_IAP2_COMMAND_PEER_MTU                    ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x08 )    /* Get peer mtu size */

/* Handsfree AG commands */
#define HCI_CONTROL_AG_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x01 )    /* Establish connection to HF Device */
#define HCI_CONTROL_AG_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x02 )    /* Release HF connection */
#define HCI_CONTROL_AG_COMMAND_OPEN_AUDIO                   ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x03 )    /* Create audio connection on existing service level connection */
#define HCI_CONTROL_AG_COMMAND_CLOSE_AUDIO                  ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x04 )    /* Disconnect audio */
#define HCI_CONTROL_AG_COMMAND_SET_CIND                     ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x05 )    /* Set CIND */
#define HCI_CONTROL_AG_COMMAND_STR                          ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x06 )    /* Send AT Command */

/* BT Serial over GATT service commands */
#define HCI_CONTROL_BSG_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x03 )    /* Send data */


/* Miscellaneous commands */
#define HCI_CONTROL_MISC_COMMAND_PING                       ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x01 )    /* Ping controller */
#define HCI_CONTROL_MISC_COMMAND_GET_VERSION                ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x02 )    /* Get SDK Version */

/* HID Host commands */
#define HCI_CONTROL_HIDH_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x01 )      /* HIDH Connect */
#define HCI_CONTROL_HIDH_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x02 )      /* HIDH Disconnect */
#define HCI_CONTROL_HIDH_COMMAND_ADD                        ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x03 )      /* HIDH Add Device */
#define HCI_CONTROL_HIDH_COMMAND_REMOVE                     ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x04 )      /* HIDH Remove Device */
#define HCI_CONTROL_HIDH_COMMAND_GET_DESCRIPTOR             ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x05 )      /* HIDH Get HID Descriptor */
#define HCI_CONTROL_HIDH_COMMAND_SET_REPORT                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x06 )      /* HIDH Set Report */
#define HCI_CONTROL_HIDH_COMMAND_GET_REPORT                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x07 )      /* HIDH Get Report */
#define HCI_CONTROL_HIDH_COMMAND_SET_PROTOCOL               ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x08 )      /* HIDH Set HID Protocol */
#define HCI_CONTROL_HIDH_COMMAND_WAKEUP_PATTERN_SET         ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x09 )      /* HIDH WakeUp Pattern */
#define HCI_CONTROL_HIDH_COMMAND_WAKEUP_CONTROL             ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x0A )      /* HIDH WakeUp Control */

/* Audio Sink Profile commands */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_CONNECT              ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x01 )    /* Audio connect to source */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_DISCONNECT           ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x02 )    /* Audio disconnect  */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_START                ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x03 )    /* Start audio with specific Sample rate/mode */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_STOP                 ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x04 )    /* Stop audio */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_START_RSP            ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x05 )    /* Response to A2DP start request, send start response */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_CHANGE_ROUTE         ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x06 )    /* Change the audio route */

/* PBAP Client commands */
#define HCI_CONTROL_PBC_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x01 )      /* PBC Connect */
#define HCI_CONTROL_PBC_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x02 )      /* PBC Disconnect */
#define HCI_CONTROL_PBC_COMMAND_GET_PHONEBOOK               ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x03 )      /* Download phonebook */
#define HCI_CONTROL_PBC_COMMAND_GET_CALL_HISTORY            ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x04 )      /* Get call history */
#define HCI_CONTROL_PBC_COMMAND_GET_INCOMMING_CALLS         ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x05 )      /* Get list of incoming calls */
#define HCI_CONTROL_PBC_COMMAND_GET_OUTGOING_CALLS          ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x06 )      /* Get list of outgoing calls */
#define HCI_CONTROL_PBC_COMMAND_GET_MISSED_CALLS            ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x07 )      /* Get list of missed calls */
#define HCI_CONTROL_PBC_COMMAND_ABORT                       ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x08 )      /* Abort current operation */

/* LE COC commands */
#define HCI_CONTROL_LE_COC_COMMAND_CONNECT                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x01 )   /* LE COC Connect */
#define HCI_CONTROL_LE_COC_COMMAND_DISCONNECT               ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x02 )   /* LE COC Disconnect */
#define HCI_CONTROL_LE_COC_COMMAND_SEND_DATA                ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x03 )   /* Send data */
#define HCI_CONTROL_LE_COC_COMMAND_SET_MTU                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x04 )   /* Set MTU size */
#define HCI_CONTROL_LE_COC_COMMAND_SET_PSM                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x05 )   /* Set PSM number */
#define HCI_CONTROL_LE_COC_COMMAND_ENABLE_LE2M              ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x06 )   /* Enable LE2M PHY */

/* LED Demo commands */
#define HCI_CONTROL_LED_COMMAND_TURN_ON                     ( ( HCI_CONTROL_GROUP_LED_DEMO << 8 ) | 0x01 )   /* LED Turn On */
#define HCI_CONTROL_LED_COMMAND_TURN_OFF                    ( ( HCI_CONTROL_GROUP_LED_DEMO << 8 ) | 0x02 )   /* LED Turn Off */
#define HCI_CONTROL_LED_COMMAND_SET_BRIGHTNESS              ( ( HCI_CONTROL_GROUP_LED_DEMO << 8 ) | 0x03 )   /* LED set brightness level */

/* Mesh models commands */
#define HCI_CONTROL_MESH_COMMAND_ONOFF_GET                                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x01 )  /* Generic On/Off Get command */
#define HCI_CONTROL_MESH_COMMAND_ONOFF_SET                                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x02 )  /* Generic On/Off Get command */

#define HCI_CONTROL_MESH_COMMAND_LEVEL_GET                                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x03 )  /* Generic Level Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LEVEL_SET                                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x04 )  /* Generic Level Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LEVEL_DELTA_SET                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x05 )  /* Generic Level Client Delta command */
#define HCI_CONTROL_MESH_COMMAND_LEVEL_MOVE_SET                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x06 )  /* Generic Level Client Move command */

#define HCI_CONTROL_MESH_COMMAND_DEF_TRANS_TIME_GET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x07 )  /* Generic Default Transition Time Set command */
#define HCI_CONTROL_MESH_COMMAND_DEF_TRANS_TIME_SET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x08 )  /* Generic Default Transition Time Get command */

#define HCI_CONTROL_MESH_COMMAND_ONPOWERUP_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x09 )  /* Power On/Off Set command */
#define HCI_CONTROL_MESH_COMMAND_ONPOWERUP_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0a )  /* Power On/Off Get command */

#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_GET                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0b )  /* Generic Power Level Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_SET                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0c )  /* Generic Power Level Set command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_LAST_GET                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0d )  /* Generic Power Level Last Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_DEFAULT_GET                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0e )  /* Generic Power Level Default Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_DEFAULT_SET                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0f )  /* Generic Power Level Default Set command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_RANGE_GET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x10 )  /* Generic Power Level Range Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_RANGE_SET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x11 )  /* Generic Power Level Range Set command */

#define HCI_CONTROL_MESH_COMMAND_LOCATION_GLOBAL_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x12 )  /* Set Global Location data */
#define HCI_CONTROL_MESH_COMMAND_LOCATION_LOCAL_SET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x13 )  /* Set Local Location data */
#define HCI_CONTROL_MESH_COMMAND_LOCATION_GLOBAL_GET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x14 )  /* Get Global Location data */
#define HCI_CONTROL_MESH_COMMAND_LOCATION_LOCAL_GET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x15 )  /* Get_Local Location data */

#define HCI_CONTROL_MESH_COMMAND_BATTERY_GET                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x16 )  /* Battery Get state */
#define HCI_CONTROL_MESH_COMMAND_BATTERY_SET                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x17 )  /* Battery state changed */

#define HCI_CONTROL_MESH_COMMAND_PROPERTIES_GET                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x18 )  /* Generic Set Value of the Property */
#define HCI_CONTROL_MESH_COMMAND_PROPERTY_GET                               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x19 )  /* Generic Set Value of the Property */
#define HCI_CONTROL_MESH_COMMAND_PROPERTY_SET                               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1a )  /* Generic Value of the Property Changed Status*/

#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_GET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1b )  /* Light Lightness Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1c )  /* Light Lightness Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_GET                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1d )  /* Light Lightness Linear Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_SET                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1e )  /* Light Lightness Linear Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LAST_GET                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1f )  /* Light Lightness Last Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_GET                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x20 )  /* Light Lightness Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_SET                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x21 )  /* Light Lightness Default Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_GET                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x22 )  /* Light Lightness Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_SET                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x23 )  /* Light Lightness Range Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x24 )  /* Light CTL Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x25 )  /* Light CTL Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_GET                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x26 )  /* Light CTL Client Temperature Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_SET                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x27 )  /* Light CTL Client Temperature Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_RANGE_GET            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x28 )  /* Light CTL Client Temperature Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_RANGE_SET            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x29 )  /* Light CTL Client Temperature Range Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_DEFAULT_GET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2a )  /* Light CTL Client Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2b )  /* Light CTL Client Default Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2c )  /* Light HSL Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2d )  /* Light HSL Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_TARGET_GET                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2e )  /* Light HSL Client Target Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_RANGE_GET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2f )  /* Light HSL Client Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_RANGE_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x30 )  /* Light HSL Client Range Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_DEFAULT_GET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x31 )  /* Light HSL Client Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x32 )  /* Light HSL Client Default Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_HUE_GET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x33 )  /* Light HSL Client Hue Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_HUE_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x34 )  /* Light HSL Client Hue Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_SATURATION_GET                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x35 )  /* Light HSL Client Saturation Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_SATURATION_SET                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x36 )  /* Light HSL Client Saturation Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x37 )  /* Light XYL Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x38 )  /* Light XYL Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_RANGE_GET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x39 )  /* Light XYL Client Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_RANGE_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3a )  /* Light XYL Client Range Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_TARGET_GET                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3b )  /* Light XYL Client Target Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_DEFAULT_GET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3c )  /* Light XYL Client Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3d )  /* Light XYL Client Default Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_MODE_GET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3e )  /* Light LC Client Mode Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_MODE_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3f )  /* Light LC Client Mode Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_OCCUPANCY_MODE_GET                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x40 )  /* Light LC Client Occupancy Mode Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_OCCUPANCY_MODE_SET                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x41 )  /* Light LC Client Occupancy Mode Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_ONOFF_GET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x42 )  /* Light LC Client OnOff Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_ONOFF_SET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x43 )  /* Light LC Client OnOff Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_PROPERTY_GET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x44 )  /* Light LC Client Property Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_PROPERTY_SET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x45 )  /* Light LC Client Property Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_OCCUPANCY_SET                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x46 )  /* Light LC Server Occupancy Detected command */

#define HCI_CONTROL_MESH_COMMAND_SENSOR_DESCRIPTOR_GET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x47 )  /* Sensor Descriptor Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_CADENCE_GET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x48 )  /* Sensor Cadence Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_CADENCE_SET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x49 )  /* Sensor Cadence Set command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SETTINGS_GET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4a )  /* Sensor Settings Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SETTING_GET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4b )  /* Sensor Setting Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_GET                                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4c )  /* Sensor Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_COLUMN_GET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4d )  /* Sensor Column Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SERIES_GET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4e )  /* Sensor Series Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SETTING_SET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4f )  /* Sensor Setting Set command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SET                                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x50 )  /* Sensor Status command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_COLUMN_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x51 )  /* Sensor Column Status command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SERIES_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x52 )  /* Sensor Series Status command */

#define HCI_CONTROL_MESH_COMMAND_SCENE_STORE                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x53 )  /* Scene Store command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_RECALL                               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x54 )  /* Scene Delete command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_GET                                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x55 )  /* Scene Get command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_REGISTER_GET                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x56 )  /* Scene Register Get command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_DELETE                               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x57 )  /* Scene Delete command */

#define HCI_CONTROL_MESH_COMMAND_SCHEDULER_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x58 )  /* Scheduler Register Get command */
#define HCI_CONTROL_MESH_COMMAND_SCHEDULER_ACTION_GET                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x59 )  /* Scheduler Action Get command */
#define HCI_CONTROL_MESH_COMMAND_SCHEDULER_ACTION_SET                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x5a )  /* Scheduler Action Set command */

#define HCI_CONTROL_MESH_COMMAND_TIME_GET                                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x5b )  /* Time Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_SET                                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x5c )  /* Time Set command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ZONE_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x5d )  /* Time Zone Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ZONE_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x5e )  /* Time Zone Set command */
#define HCI_CONTROL_MESH_COMMAND_TIME_TAI_UTC_DELTA_GET                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x5f )  /* Time TAI_UTC Delta Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_TAI_UTC_DELTA_SET                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x60 )  /* Time TAI_UTC Delta Set command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ROLE_GET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x61 )  /* Time Role Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ROLE_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x62 )  /* Time Role Set command */

#define HCI_CONTROL_MESH_COMMAND_FW_DISTRIBUTION_UPLOAD_START               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x63 )  /* Prepare Distribution Client for FW Upload */
#define HCI_CONTROL_MESH_COMMAND_FW_DISTRIBUTION_UPLOAD_DATA                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x64 )  /* FW Upload next data chunk */
#define HCI_CONTROL_MESH_COMMAND_FW_DISTRIBUTION_UPLOAD_FINISH              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x65 )  /* FW Upload completed */
#define HCI_CONTROL_MESH_COMMAND_FW_DISTRIBUTION_UPLOAD_GET_STATUS          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x66 )  /* Get current status and phase */

/* Mesh commands */
#define HCI_CONTROL_MESH_COMMAND_CORE_NETWORK_LAYER_TRNSMIT                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x01 )  /* Network Layer Transmit Mesage command */
#define HCI_CONTROL_MESH_COMMAND_CORE_TRANSPORT_LAYER_TRNSMIT               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x02 )  /* Transport Layer Transmit Mesage command */
#define HCI_CONTROL_MESH_COMMAND_CORE_IVUPDATE_SIGNAL_TRNSIT                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x03 )  /* IV UPDATE Transit Signal command */

#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_CAPABILITIES_GET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x04 )  /* Sends command to get remote provisioner scan information */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x05 )  /* Sends command to get remote provisioner scan */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_START                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x06 )  /* Sends command to start scanning for unprovisioned devices */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_STOP                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x07 )  /* Sends command to stop scanning for unprovisioned devices */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_CONNECT                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x08 )  /* Sends command to establish provisioning link to remote device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_DISCONNECT                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x09 )  /* Sends command to disconnect provisioning link with remote device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_START                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0a )  /* Sends command to start provisioning of the remote device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_OOB_CONFIGURE                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0b )  /* Sends out of band configuration for provisioning device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_OOB_VALUE                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0c )  /* Sends command with out of band value for confirmation calculation */
#define HCI_CONTROL_MESH_COMMAND_SEARCH_PROXY                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0d )  /* Sends a command to start/stop scanning for GATT proxy devices */
#define HCI_CONTROL_MESH_COMMAND_PROXY_CONNECT                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0e )  /* Sends a command to connect to a GATT proxy devices */
#define HCI_CONTROL_MESH_COMMAND_PROXY_DISCONNECT                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0f )  /* Sends a command to disconnect to a GATT proxy devices */
#define HCI_CONTROL_MESH_COMMAND_PROXY_FILTER_TYPE_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x10 )  /* Set Proxy Filter Type */
#define HCI_CONTROL_MESH_COMMAND_PROXY_FILTER_ADDRESSES_ADD                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x11 )  /* Add Addresses to Filter */
#define HCI_CONTROL_MESH_COMMAND_PROXY_FILTER_ADDRESSES_DELETE              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x12 )  /* Remove Addresses to Filter  */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_NODE_RESET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x13 )  /* Node Reset */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_BEACON_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x14 )  /* Beacon State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_BEACON_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x15 )  /* Beacon State Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_COMPOSITION_DATA_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x16 )  /* Composition Data Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_DEFAULT_TTL_GET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x17 )  /* Default TTL Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_DEFAULT_TTL_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x18 )  /* Default TTL Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_GATT_PROXY_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x19 )  /* GATT Proxy State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_GATT_PROXY_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1a )  /* GATT Proxy State Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_RELAY_GET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1b )  /* Relay Configuration Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_RELAY_SET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1c )  /* Relay Configuration Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_FRIEND_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1d )  /* Relay Configuration Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_FRIEND_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1e )  /* Relay Configuration Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_SUBSCRIPTION_GET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1f )  /* Hearbeat Subscription Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_SUBSCRIPTION_SET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x20 )  /* Hearbeat Subscription Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_PUBLICATION_GET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x21 )  /* Hearbeat Publication Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_PUBLICATION_SET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x22 )  /* Hearbeat Publication Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NETWORK_TRANSMIT_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x23 )  /* Network Transmit Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NETWORK_TRANSMIT_SET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x24 )  /* Network Transmit Set */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_PUBLICATION_GET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x25 )  /* Model Publication Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_PUBLICATION_SET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x26 )  /* Model Publication Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_ADD              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x27 )  /* Model Subscription Add */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_DELETE           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x28 )  /* Model Subscription Delete */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x29 )  /* Model Subscription Overwrite */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2a )  /* Model Subscription Delete All */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_GET              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2b )  /* Model Subscription Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_ADD                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2c )  /* NetKey Add */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_DELETE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2d )  /* NetKey Delete */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_UPDATE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2e )  /* NetKey Update */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2f )  /* NetKey Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_ADD                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x30 )  /* AppKey Add */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_DELETE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x31 )  /* AppKey Delete */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_UPDATE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x32 )  /* AppKey Update */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x33 )  /* AppKey Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_APP_BIND                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x34 )  /* Model App Bind */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_APP_UNBIND                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x35 )  /* Model App Unind */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_APP_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x36 )  /* Model App Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NODE_IDENTITY_GET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x37 )  /* Node Identity Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NODE_IDENTITY_SET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x38 )  /* Node Identity Get */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_LPN_POLL_TIMEOUT_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x39 )  /* LPN Poll Timeout Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_KEY_REFRESH_PHASE_GET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3a )  /* Key Refresh Phase Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_KEY_REFRESH_PHASE_SET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3b )  /* Key Refresh Phase Set */

#define HCI_CONTROL_MESH_COMMAND_HEALTH_FAULT_GET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3c )  /* Health Fault Get */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_FAULT_CLEAR                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3d )  /* Health Fault Clear */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_FAULT_TEST                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3e )  /* Health Fault Test */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_PERIOD_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3f )  /* Health Period Get */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_PERIOD_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x40 )  /* Health Period Set */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_ATTENTION_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x41 )  /* Health Attention Get */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_ATTENTION_SET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x42 )  /* Health Attention Set */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_ADD                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x43 )  /* Add Vendor Model */

#define HCI_CONTROL_MESH_COMMAND_TRACE_CORE_SET                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x44 )  /* Set Level for Core Traces */
#define HCI_CONTROL_MESH_COMMAND_TRACE_MODELS_SET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x45 )  /* Set Level for Models Traces */
#define HCI_CONTROL_MESH_COMMAND_RSSI_TEST_START                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x46 )  /* Start RSSI test */
#define HCI_CONTROL_MESH_COMMAND_SET_ADV_TX_POWER                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x47 )  /* Set Mesh Core ADV Tx Power */

#define HCI_CONTROL_MESH_COMMAND_SET_LOCAL_DEVICE                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x48 )  /* Set Local Device. Application can set it once to make provisioner client. */
#define HCI_CONTROL_MESH_COMMAND_SET_DEVICE_KEY                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x49 )  /* Setup device key.  Application can set it once and then send multiple configuration commands. */
#define HCI_CONTROL_MESH_COMMAND_CORE_LOW_POWER_SEND_FRIEND_CLEAR           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4a )  /* Terminate friendship with a Friend by sending a Friend Clear */
#define HCI_CONTROL_MESH_COMMAND_CORE_PROVISION                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4b )  /* Sends command to provision remote device */
#define HCI_CONTROL_MESH_COMMAND_CORE_CLEAR_REPLAY_PROT_LIST                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4c )  /* Sends command to clear replay protection list */
#define HCI_CONTROL_MESH_COMMAND_CORE_SET_IV_UPDATE_TEST_MODE               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4d )  /* Sends command to set or reset IV UPDATE test mode */
#define HCI_CONTROL_MESH_COMMAND_CORE_SET_IV_RECOVERY_STATE                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4e )  /* Sends command to set or reset IV RECOVERY mode */
#define HCI_CONTROL_MESH_COMMAND_CORE_HEALTH_SET_FAULTS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4f )  /* Sends command to set faults array */
#define HCI_CONTROL_MESH_COMMAND_CORE_CFG_ADV_IDENTITY                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x50 )  /* Begin advertising with Node Identity, simulating user interaction */
#define HCI_CONTROL_MESH_COMMAND_CORE_ACCESS_PDU                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x51 )  /* Handle Access PDU */
#define HCI_CONTROL_MESH_COMMAND_CORE_SEND_SUBS_UPDT                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x52 )  /* Send Friend Subscription Add or Remove message */

#define HCI_CONTROL_MESH_COMMAND_CORE_SET_SEQ                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x53 )  /* Set Sequence Number Command */
#define HCI_CONTROL_MESH_COMMAND_CORE_DEL_SEQ                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x54 )  /* Delete Sequence Number from RPL list Command */

#define HCI_CONTROL_MESH_COMMAND_APP_START                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x55 )  /* Starts Mesh Application - calls mesh_application_init() and deletes all NVRAM chunks */

#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_EXTENDED_START              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x56 )  /* Sends command to start extended scan */
#define HCI_CONTROL_MESH_COMMAND_GATEWAY_CONN_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x57 )  /* Application sends connect proxy */
#define HCI_CONTROL_MESH_COMMAND_SEND_PROXY_DATA                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x58 )  /* Mesh Application can send proxy data from MCU */
#define HCI_CONTROL_MESH_COMMAND_SET_MODEL_LEVEL_ACCESS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x59 )  /* Mesh Application processes raw data access layer data */
#define HCI_CONTROL_MESH_COMMAND_RAW_MODEL_DATA                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5a )  /* Raw model data to be passed to access layer */
#define HCI_CONTROL_MESH_COMMAND_VENDOR_DATA                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5b )  /* Vendor model data */

#define HCI_CONTROL_MESH_COMMAND_HARD_RESET                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5c )  /* Sets UUID if it is not empty and then does factory reset */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_PRIVATE_BEACON_GET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5d )  /* Private Beacon State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_PRIVATE_BEACON_SET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5e )  /* Private Beacon State Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_PRIVATE_GATT_PROXY_GET              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5f )  /* Private GATT Proxy State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_PRIVATE_GATT_PROXY_SET              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x60 )  /* Private GATT Proxy State Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_PRIVATE_NODE_IDENTITY_GET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x61 )  /* Private Node Identity Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_PRIVATE_NODE_IDENTITY_SET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x62 )  /* Private Node Identity Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_ON_DEMAND_PRIVATE_PROXY_GET         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x63 )  /* On-Demand Private Proxy State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_ON_DEMAND_PRIVATE_PROXY_SET         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x64 )  /* On-Demand Private Proxy State Set */
#define HCI_CONTROL_MESH_COMMAND_SOLICITATION_PDU_RPL_ITEMS_CLEAR           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x65 )  /* Solicitation PDU RPL items clear */
#define HCI_CONTROL_MESH_COMMAND_SEND_SOLICITATION_PDU                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x66 )  /* Send solicitation PDU */
#define HCI_CONTROL_MESH_COMMAND_TEST_EVENTS_REPORT_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x67 )  /* Mesh core to report events for test purpose */

#define HCI_CONTROL_MESH_COMMAND_NETWORK_FILTER_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x68 )  /* Send Network Filter Get message */
#define HCI_CONTROL_MESH_COMMAND_NETWORK_FILTER_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x69 )  /* Send Network Filter Set message */

#define HCI_CONTROL_MESH_COMMAND_DF_SET_PATH_MONITORING_TEST_MODE           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6a )  /* Set Path Monitoring Test Mode */
#define HCI_CONTROL_MESH_COMMAND_DF_TRANSIT_TO_PATH_MONITORING_STATE_SIGNAL ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6b )  /* Transit to Path Monitoring state signal command */
#define HCI_CONTROL_MESH_COMMAND_DF_STATS_GET                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6c )  /* Send Directed Forwarding_Statistics Get message */

#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_CONTROL_GET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6d )  /* an acknowledged message used to get the current Directed Control state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_CONTROL_SET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6e )  /* an acknowledged message used to set the Directed Control state of a subnet */
#define HCI_CONTROL_MESH_COMMAND_DF_PATH_METRIC_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6f )  /* An acknowledged message used to get the current Path Metric state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_PATH_METRIC_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x70 )  /* an acknowledged message used to set the Path Metric state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DISCOVERY_TABLE_CAPABILITIES_GET        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x71 )  /* an acknowledged message used to get the current Discovery Table Capabilities state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DISCOVERY_TABLE_CAPABILITIES_SET        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x72 )  /* an acknowledged message used to set the Max Concurrent Init state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_ADD                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x73 )  /* an acknowledged message used to add a fixed path entry to the Forwarding Table state of a node or to update an existing fixed path entry */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_DELETE                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x74 )  /* an acknowledged message used to delete all the path entries from a specific Path Origin to a specific destination from the Forwarding Table state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_DEPENDENTS_ADD         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x75 )  /* an acknowledged message used to add entries to the Dependent_Origin_List field or to the Dependent_Target_List field of a fixed path entry in the Forwarding Table state of a node. */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_DEPENDENTS_DELETE      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x76 )  /* an acknowledged message used to delete dependent node entries from the Dependent_Origin_List field or the Dependent_Target_List field of a fixed path entry in the Forwarding Table state of a node. */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_DEPENDENTS_GET         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x77 )  /* an acknowledged message used to get the list of unicast address ranges of dependent nodes of the Path Origin or the Path Target of a Forwarding Table entry. */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_ENTRIES_COUNT_GET      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x78 )  /* an acknowledged message used to get the information about the Forwarding Table state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_FORWARDING_TABLE_ENTRIES_GET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x79 )  /* an acknowledged message used to get a filtered set of path entries in the Forwarding Table state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_WANTED_LANES_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7a )  /* an acknowledged message used to get the Wanted Lanes state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_WANTED_LANES_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7b )  /* an acknowledged message used to set the Wanted Lanes state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_TWO_WAY_PATH_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7c )  /* an acknowledged message used to get the current Two Way Path state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_TWO_WAY_PATH_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7d )  /* an acknowledged message used to set the Two Way Path state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_PATH_ECHO_INTERVAL_GET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7e )  /* an acknowledged message used to get the current Path Echo Interval state of a nod */
#define HCI_CONTROL_MESH_COMMAND_DF_PATH_ECHO_INTERVAL_SET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7f )  /* an acknowledged message used to set the Path Echo Interval state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_NETWORK_TRANSMIT_GET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x80 )  /* an acknowledged message used to get the current Directed Network Transmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_NETWORK_TRANSMIT_SET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x81 )  /* an acknowledged message used to set the Directed Network Transmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_RELAY_RETRANSMIT_GET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x82 )  /* an acknowledged message used to get the current Directed Relay Retransmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_RELAY_RETRANSMIT_SET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x83 )  /* an acknowledged message used to set the Directed Relay Retransmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_RSSI_THRESHOLD_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x84 )  /* an acknowledged message used to get the current RSSI Threshold state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_RSSI_THRESHOLD_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x85 )  /* an acknowledged message used to set the RSSI Margin state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_PATHS_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x86 )  /* an acknowledged message used to get the Directed Paths state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_PUBLISH_POLICY_GET             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x87 )  /* an acknowledged message used to get the Directed Publish Policy state of a model of an element of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_PUBLISH_POLICY_SET             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x88 )  /* an acknowledged message used to set the Directed Publish Policy state of a model of an element of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_PATH_DISCOVERY_TIMING_CONTROL_GET       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x89 )  /* an acknowledged message used to get the Path Discovery Timing Control state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_PATH_DISCOVERY_TIMING_CONTROL_SET       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8a )  /* an acknowledged message used to set the Path Discovery Timing Control state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8b )  /* an acknowledged message used to get the current Directed Control Network Transmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8c )  /* an acknowledged message used to set the Directed Control Network Transmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8d )  /* an acknowledged message used to get the current Directed Control Relay Retransmit state of a node */
#define HCI_CONTROL_MESH_COMMAND_DF_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8e )  /* an acknowledged message used to set the Directed Control Relay Retransmit state of a node */

#define HCI_CONTROL_MESH_COMMAND_CORE_CFG_BEACON                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8f )  /* Enable/disable beacon/private beacon */

/* Battery Client Profile commands */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_CONNECT             ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x00 )    /* Battery Client connect */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_DISCONNECT          ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x01 )    /* Battery Client disconnect */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_ADD                 ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x02 )    /* Battery Client Add */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_REMOVE              ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x03 )    /* Battery Client Remove */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_READ                ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x04 )    /* Battery Client Read Level */

/* FindMe Locator Client Profile commands */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_CONNECT          ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x00 )    /* FindMe Target connect */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_DISCONNECT       ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x01 )    /* FindMe Target disconnect */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_ADD              ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x02 )    /* FindMe Target Add */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_REMOVE           ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x03 )    /* FindMe Target Remove */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_WRITE            ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x04 )    /* FindMe Target Write Alert */

/* OPP Server commands */
#define HCI_CONTROL_OPS_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x00 )      /* OPS disconnect */
#define HCI_CONTROL_OPS_COMMAND_ACCESS_RSP                  ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x01 )      /* OPS access response */

/* Script commands */
#define HCI_CONTROL_SCRIPT_COMMAND_EXECUTE                  ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x01 )    /* Execute function */

/* OTP Commands */
#define HCI_CONTROL_OTP_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x00 )      /* Connect */
#define HCI_CONTROL_OTP_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x01 )      /* Disconnect */
#define HCI_CONTROL_OTP_COMMAND_START_UPGRADE               ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x02 )      /* Starting with the firmware upgrade. Prepare for Upgrade */
#define HCI_CONTROL_OTP_COMMAND_SEND_DATA                   ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x03 )      /* OTP send firmware upgrade data */
#define HCI_CONTROL_OTP_COMMAND_UPGRADE                     ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x04 )      /* Upgrade to the new firmware */
#define HCI_CONTROL_OTP_COMMAND_READ_CHAR                   ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x05 )      /* Read Characteristic */
#define HCI_CONTROL_OTP_COMMAND_WRITE_CHAR                  ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x06 )      /* Write Characteristic */
#define HCI_CONTROL_OTP_COMMAND_CONFIGURE_IND               ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x07 )      /* Configure Indication */
#define HCI_CONTROL_OTP_COMMAND_OACP                        ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x08 )      /* Write OACP Command */

/* MCE commands */
#define HCI_CONTROL_MCE_COMMAND_GET_MAS_INSTANCES           ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x00 )      /* Get MAS instances */
#define HCI_CONTROL_MCE_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x01 )      /* Connect */
#define HCI_CONTROL_MCE_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x02 )      /* Disconnect */
#define HCI_CONTROL_MCE_COMMAND_LIST_FOLDERS                ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x03 )      /* List folders */
#define HCI_CONTROL_MCE_COMMAND_SET_FOLDER                  ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x04 )      /* Set folder */
#define HCI_CONTROL_MCE_COMMAND_LIST_MESSAGES               ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x05 )      /* List messages */
#define HCI_CONTROL_MCE_COMMAND_GET_MESSAGE                 ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x06 )      /* Get message */
#define HCI_CONTROL_MCE_COMMAND_PUSH_MESSAGE                ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x07 )      /* Push message */
#define HCI_CONTROL_MCE_COMMAND_UPDATE_INBOX                ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x08 )      /* Update inbox */
#define HCI_CONTROL_MCE_COMMAND_SET_MESSAGE_STATUS          ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x09 )      /* Set message status */
#define HCI_CONTROL_MCE_COMMAND_ABORT                       ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x0A )      /* Abort */
#define HCI_CONTROL_MCE_COMMAND_NOTIF_REG                   ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x0B )      /* Register for message notification */

/* HomeKit commands */
#define HCI_CONTROL_HK_COMMAND_READ                         ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x01 )    /* Read characteristic */
#define HCI_CONTROL_HK_COMMAND_WRITE                        ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x02 )    /* Write characteristic */
#define HCI_CONTROL_HK_COMMAND_LIST                         ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x03 )    /* List all characteristics */
#define HCI_CONTROL_HK_COMMAND_FACTORY_RESET                ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x04 )    /* Factory reset */
#define HCI_CONTROL_HK_COMMAND_GET_TOKEN                    ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x05 )    /* Get software authentication token */

/* HCI Audio commands */
#define HCI_CONTROL_HCI_AUDIO_COMMAND_GET                   ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x00 ) /* Generic Get command */
#define HCI_CONTROL_HCI_AUDIO_COMMAND_SET                   ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x01 ) /* Generic Set command */
#define HCI_CONTROL_HCI_AUDIO_COMMAND_MIC_DATA              ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x02 ) /* Record data from MIC */
#define HCI_CONTROL_HCI_AUDIO_COMMAND_PUSH_NVRAM_DATA       ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x03 ) /* Push NVRAM data by ID */
#define HCI_CONTROL_HCI_AUDIO_COMMAND_BT_START              ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x04 ) /* Start BT stack */
#define HCI_CONTROL_HCI_AUDIO_COMMAND_BUTTON                ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x30 ) /* Button event */

/* HCI DFU */
#define HCI_CONTROL_DFU_COMMAND_READ_CONFIG                 ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x00 ) /* DFU start request */
#define HCI_CONTROL_DFU_COMMAND_WRITE_COMMAND               ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x01 ) /* DFU Data Transfer Packet */
#define HCI_CONTROL_DFU_COMMAND_WRITE_DATA                  ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x02 ) /* DFU Data Transfer Packet */

/* HCI TEST */
#define HCI_CONTROL_HCITEST_COMMAND                         ( ( HCI_CONTROL_GROUP_HCITEST << 8 ) | 0x0 ) /* Test start request */
#define HCI_CONTROL_HCITEST_CONFIGURE                       ( ( HCI_CONTROL_GROUP_HCITEST << 8 ) | 0x1 ) /* Test configure request */

/* Connected Mesh Commands */
#define HCI_CONTROL_CONN_MESH_COMMAND_BECOME_PROVISIONER    ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x01 )  /* Become Provisioner */
#define HCI_CONTROL_CONN_MESH_COMMAND_END_PROVISIONING      ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x02 )  /* Become Provisioner */
#define HCI_CONTROL_CONN_MESH_COMMAND_GET_NODE_LIST         ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x03 )  /* Provisioner only - report list of all nodes */
#define HCI_CONTROL_CONN_MESH_COMMAND_SEND_APP_DATA         ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x04 )  /* Send Application Data */
#define HCI_CONTROL_CONN_MESH_COMMAND_RESET					( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x05 )  /* Reset device */
#define HCI_CONTROL_CONN_MESH_COMMAND_FACTORY_RESET         ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x06 )  /* Factory reset the network */
#define HCI_CONTROL_CONN_MESH_COMMAND_GET_CONN_STATUS       ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x07 )  /* Report connection status */
#define HCI_CONTROL_CONN_MESH_COMMAND_PING_ALL_NODES        ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x08 )  /* Provisioner only - ping all nodes */
#define HCI_CONTROL_CONN_MESH_COMMAND_START_STOP_DATA       ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x09 )  /* Start/Stop Data */
#define HCI_CONTROL_CONN_MESH_COMMAND_GET_STATS				( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x0A )  /* Collect stats */
#define HCI_CONTROL_CONN_MESH_COMMAND_IDENTIFY				( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x0B )  /* Identify node */
#define HCI_CONTROL_CONN_MESH_COMMAND_GET_RSSI 				( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x0C )  /* Collect RSSI values */

#define HCI_CONTROL_PANU_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x01 )
#define HCI_CONTROL_PANU_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x02 )

/* General events that the controller can send */
#define HCI_CONTROL_EVENT_COMMAND_STATUS                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_EVENT_WICED_TRACE                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x02 )    /* WICED trace packet */
#define HCI_CONTROL_EVENT_HCI_TRACE                         ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x03 )    /* Bluetooth protocol trace */
#define HCI_CONTROL_EVENT_NVRAM_DATA                        ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x04 )    /* Request to MCU to save NVRAM chunk */
#define HCI_CONTROL_EVENT_DEVICE_STARTED                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x05 )    /* Device completed power up initialization */
#define HCI_CONTROL_EVENT_INQUIRY_RESULT                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x06 )    /* Inquiry result */
#define HCI_CONTROL_EVENT_INQUIRY_COMPLETE                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x07 )    /* Inquiry completed event */
#define HCI_CONTROL_EVENT_PAIRING_COMPLETE                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x08 )    /* Pairing Completed */
#define HCI_CONTROL_EVENT_ENCRYPTION_CHANGED                ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x09 )    /* Encryption changed event */
#define HCI_CONTROL_EVENT_CONNECTED_DEVICE_NAME             ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0A )    /* Device name event */
#define HCI_CONTROL_EVENT_USER_CONFIRMATION                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0B )    /* User Confirmation during pairing */
#define HCI_CONTROL_EVENT_DEVICE_ERROR                      ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0C )    /* Device Error event */
#define HCI_CONTROL_EVENT_READ_LOCAL_BDA                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0D )    /* Local BDA Read event */
#define HCI_CONTROL_EVENT_MAX_NUM_OF_PAIRED_DEVICES_REACHED ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0E )    /* Key Buffer Pool Full */
#define HCI_CONTROL_EVENT_READ_BUFFER_STATS                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0F )    /* Read Buffer statistics event */
#define HCI_CONTROL_EVENT_KEYPRESS_NOTIFICATION             ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x10 )    /* KeyPress notification */
#define HCI_CONTROL_EVENT_CONNECTION_STATUS                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x11 )    /* Connection Status */
#define HCI_CONTROL_EVENT_PIN_REQUEST                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x12 )    /* PIN Code Request */
#define HCI_CONTROL_EVENT_SSP_REQUEST                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x13 )    /* SSP Code Request */

/* Events for the HFP profile */
#define HCI_CONTROL_HF_EVENT_OPEN                           ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x01 )    /* HS connection opened or connection attempt failed  */
#define HCI_CONTROL_HF_EVENT_CLOSE                          ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x02 )    /* HS connection closed */
#define HCI_CONTROL_HF_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x03 )    /* HS Service Level Connection is UP */
#define HCI_CONTROL_HF_EVENT_AUDIO_OPEN                     ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x04 )    /* Audio connection open */
#define HCI_CONTROL_HF_EVENT_AUDIO_CLOSE                    ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_HF_EVENT_AUDIO_CONN_REQ                 ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x06 )    /* Audio connection request event */
#define HCI_CONTROL_HF_EVENT_PROFILE_TYPE                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x07 )    /* To check the Profile Selected HSP/HFP */

/* Sub-commands AT events defined with AT Commands */
#define HCI_CONTROL_HF_AT_EVENT_BASE                        ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x20 )
#define HCI_CONTROL_HF_AT_EVENT_OK                          0x00    /* OK response received to previous AT command */
#define HCI_CONTROL_HF_AT_EVENT_ERROR                       0x01    /* ERROR response received */
#define HCI_CONTROL_HF_AT_EVENT_CMEE                        0x02    /* Extended error codes response */
#define HCI_CONTROL_HF_AT_EVENT_RING                        0x03    /* RING indicator */
#define HCI_CONTROL_HF_AT_EVENT_VGS                         0x04
#define HCI_CONTROL_HF_AT_EVENT_VGM                         0x05
#define HCI_CONTROL_HF_AT_EVENT_CCWA                        0x06
#define HCI_CONTROL_HF_AT_EVENT_CHLD                        0x07
#define HCI_CONTROL_HF_AT_EVENT_CIND                        0x08
#define HCI_CONTROL_HF_AT_EVENT_CLIP                        0x09
#define HCI_CONTROL_HF_AT_EVENT_CIEV                        0x0A
#define HCI_CONTROL_HF_AT_EVENT_BINP                        0x0B
#define HCI_CONTROL_HF_AT_EVENT_BVRA                        0x0C
#define HCI_CONTROL_HF_AT_EVENT_BSIR                        0x0D
#define HCI_CONTROL_HF_AT_EVENT_CNUM                        0x0E
#define HCI_CONTROL_HF_AT_EVENT_BTRH                        0x0F
#define HCI_CONTROL_HF_AT_EVENT_COPS                        0x10
#define HCI_CONTROL_HF_AT_EVENT_CLCC                        0x11
#define HCI_CONTROL_HF_AT_EVENT_BIND                        0x12
#define HCI_CONTROL_HF_AT_EVENT_BCS                         0x13
#define HCI_CONTROL_HF_AT_EVENT_UNAT                        0x14
#define HCI_CONTROL_HF_AT_EVENT_MAX                         0x14    /* Maximum AT event value */

/* LE events for the BLE GATT/GAP profile */
#define HCI_CONTROL_LE_EVENT_COMMAND_STATUS                 ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_LE_EVENT_SCAN_STATUS                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x02 )    /* LE scanning state change notification */
#define HCI_CONTROL_LE_EVENT_ADVERTISEMENT_REPORT           ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x03 )    /* Advertisement report */
#define HCI_CONTROL_LE_EVENT_ADVERTISEMENT_STATE            ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x04 )    /* LE Advertisement state change notification */
#define HCI_CONTROL_LE_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x05 )    /* LE Connection established */
#define HCI_CONTROL_LE_EVENT_DISCONNECTED                   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x06 )    /* Le Connection Terminated */
#define HCI_CONTROL_LE_EVENT_IDENTITY_ADDRESS               ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x07 )    /* Identity address */
#define HCI_CONTROL_LE_EVENT_PEER_MTU                       ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x08 )    /* Client MTU Request */
#define HCI_CONTROL_LE_EVENT_CONN_PARAMS                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x09 )    /* BLE connection parameter update event */

/* GATT events */
#define HCI_CONTROL_GATT_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_GATT_EVENT_DISCOVERY_COMPLETE           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x02 )    /* Discovery requested by host completed */
#define HCI_CONTROL_GATT_EVENT_SERVICE_DISCOVERED           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x03 )    /* Service discovered */
#define HCI_CONTROL_GATT_EVENT_CHARACTERISTIC_DISCOVERED    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x04 )    /* Characteristic discovered */
#define HCI_CONTROL_GATT_EVENT_DESCRIPTOR_DISCOVERED        ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x05 )    /* Characteristic descriptor discovered */
#define HCI_CONTROL_GATT_EVENT_READ_REQUEST                 ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x06 )    /* Peer sent Read Request */
#define HCI_CONTROL_GATT_EVENT_READ_RESPONSE                ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x07 )    /* Read response */
#define HCI_CONTROL_GATT_EVENT_WRITE_REQUEST                ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x08 )    /* Peer sent Write Request */
#define HCI_CONTROL_GATT_EVENT_WRITE_RESPONSE               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x09 )    /* Write operation completed */
#define HCI_CONTROL_GATT_EVENT_INDICATION                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0a )    /* Indication from peer */
#define HCI_CONTROL_GATT_EVENT_NOTIFICATION                 ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0b )    /* Notification from peer */
#define HCI_CONTROL_GATT_EVENT_READ_ERROR                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0c )    /* GATT Read operation error */
#define HCI_CONTROL_GATT_EVENT_WRITE_ERROR                  ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0d )    /* GATT Write operation error */

/* Events for the SPP profile */
#define HCI_CONTROL_SPP_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x01 )    /* SPP connection opened */
#define HCI_CONTROL_SPP_EVENT_SERVICE_NOT_FOUND             ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x02 )    /* SDP record with SPP service not found */
#define HCI_CONTROL_SPP_EVENT_CONNECTION_FAILED             ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x03 )    /* Connection attempt failed  */
#define HCI_CONTROL_SPP_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x04 )    /* SPP connection closed */
#define HCI_CONTROL_SPP_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x05 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_SPP_EVENT_RX_DATA                       ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x06 )    /* SPP data received */
#define HCI_CONTROL_SPP_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x07 )    /* Command status event for the requested operation */
#define HCI_CONTROL_SPP_EVENT_PEER_MTU                      ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x08 )    /* Get peer mtu size */

/* Events for the Audio profile */
#define HCI_CONTROL_AUDIO_EVENT_COMMAND_COMPLETE            ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x00 )    /* Command complete event for the requested operation */
#define HCI_CONTROL_AUDIO_EVENT_COMMAND_STATUS              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_AUDIO_EVENT_CONNECTED                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x02 )    /* Audio connection opened */
#define HCI_CONTROL_AUDIO_EVENT_SERVICE_NOT_FOUND           ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x03 )    /* SDP record with audio service not found */
#define HCI_CONTROL_AUDIO_EVENT_CONNECTION_FAILED           ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x04 )    /* Connection attempt failed  */
#define HCI_CONTROL_AUDIO_EVENT_DISCONNECTED                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_AUDIO_EVENT_REQUEST_DATA                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x06 )    /* Request for audio pcm sample data */
#define HCI_CONTROL_AUDIO_EVENT_STARTED                     ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x07 )    /* Command for audio start succeeded */
#define HCI_CONTROL_AUDIO_EVENT_STOPPED                     ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x08 )    /* Command for audio stop completed */
#define HCI_CONTROL_AUDIO_EVENT_STATISTICS                  ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x09 )    /* Command for audio statistics */
#define HCI_CONTROL_AUDIO_EVENT_SUPPORT_FEATURES            ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x10 )    /* Device reportes the supported features of AV source */

/* Events for the AVRCP profile target events */
#define HCI_CONTROL_AVRC_TARGET_EVENT_CONNECTED             ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x01 )    /* AVRCP Target connected */
#define HCI_CONTROL_AVRC_TARGET_EVENT_DISCONNECTED          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x02 )    /* AVRCP Target disconnected */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PLAY                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x03 )    /* Play command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_STOP                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x04 )    /* Stop command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PAUSE                 ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x05 )    /* Pause command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_NEXT_TRACK            ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x06 )    /* Next Track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PREVIOUS_TRACK        ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x07 )    /* Previous track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_BEGIN_FAST_FORWARD    ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x08 )    /* Next Track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_END_FAST_FORWARD      ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x09 )    /* Previous track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_BEGIN_REWIND          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0A )    /* Next Track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_END_REWIND            ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0B )    /* Previous track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_VOLUME_LEVEL          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0C )    /* Volume Level changed received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_REPEAT_SETTINGS       ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0D )    /* Repeat settings changed by peer, sent to MCU app */
#define HCI_CONTROL_AVRC_TARGET_EVENT_SHUFFLE_SETTINGS      ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0E )    /* Shuffle settings changed by peer, sent to MCU app */
#define HCI_CONTROL_AVRC_TARGET_EVENT_GET_PLAYER_STATUS     ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0F )    /* Player status info requested by peer, get info from MCU app */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PASSTHROUGH_STATUS    ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x10 )    /* Passthrough command status received from peer */
#define HCI_CONTROL_AVRC_TARGET_EVENT_COMMAND_STATUS        ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0xFF )    /* Result status for AVRCP commands */

/* Events for the AVRCP CT profile */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_CONNECTED          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x01 )    /* AVRCP Controller connected */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_DISCONNECTED       ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x02 )    /* AVRCP Controller disconnected */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_CURRENT_TRACK_INFO ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x03 )    /* AVRCP Controller disconnected */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAY_STATUS        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x04 )    /* AVRCP Controller Play Status Change */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAY_POSITION      ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x05 )    /* AVRCP Controller Play Position Change */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_TRACK_CHANGE       ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x06 )    /* AVRCP Controller Track Changed */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_TRACK_END          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x07 )    /* AVRCP Controller Track reached End */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_TRACK_START        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x08 )    /* AVRCP Controller Track reached Start */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_SETTING_AVAILABLE  ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x09 )    /* AVRCP Controller Player setting available */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_SETTING_CHANGE     ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0a )    /* AVRCP Controller Player setting changed */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAYER_CHANGE      ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0b )    /* AVRCP Controller Player changed */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAY_STATUS_INFO   ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0c )    /* AVRCP Controller Play status response */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_COMMAND_STATUS     ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0xFF )    /* Result status for AVRCP commands */

/* Events for the HID Device profile */
#define HCI_CONTROL_HIDD_EVENT_OPENED                       ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x01 )    /* Both HID channels are opened */
#define HCI_CONTROL_HIDD_EVENT_VIRTUAL_CABLE_UNPLUGGED      ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x02 )    /* Host requested Virtual Cable Unplug */
#define HCI_CONTROL_HIDD_EVENT_DATA                         ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x03 )    /* Host sent report */
#define HCI_CONTROL_HIDD_EVENT_CLOSED                       ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x04 )    /* Host attempt to establish connection failed */
#define HCI_CONTROL_HIDD_EVENT_HOST_ADDR                    ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x05 )    /* Paired host address */
#define HCI_CONTROL_HIDD_EVENT_STATE_CHANGE                 ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x06 )    /* Device State Change */
#define HCI_CONTROL_HIDD_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0xFF )    /* Result status for HID commands */

/* Events for the Test events group */
#define HCI_CONTROL_TEST_EVENT_ENCAPSULATED_HCI_EVENT       ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x01 )     /* Encapsulated HCI Event message */

/* Events for ANCS */
#define HCI_CONTROL_ANCS_EVENT_NOTIFICATION                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x01 )      /* ANCS notification */
#define HCI_CONTROL_ANCS_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x02 )      /* Command status event for the requested operation */
#define HCI_CONTROL_ANCS_EVENT_SERVICE_FOUND                ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x03 )      /* ANCS event for service found */
#define HCI_CONTROL_ANCS_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x04 )      /* ANCS event for service connected */
#define HCI_CONTROL_ANCS_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x05 )      /* ANCS event for service disconnected */

/* Events for ANC */
#define HCI_CONTROL_ANC_EVENT_ANC_ENABLED                           ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x01 )   /* ANC event when connected to ANS */
#define HCI_CONTROL_ANC_EVENT_SERVER_SUPPORTED_NEW_ALERTS           ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x02 )   /* ANC event on complete of read server supported new alerts */
#define HCI_CONTROL_ANC_EVENT_SERVER_SUPPORTED_UNREAD_ALERTS        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x03 )   /* ANC event on complete of read server supported unread alerts */
#define HCI_CONTROL_ANC_EVENT_CONTROL_ALERTS                        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x04 )   /* ANC event on complete of control alerts configuration */
#define HCI_CONTROL_ANC_EVENT_ENABLE_NEW_ALERTS                     ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x05 )   /* ANC event on complete of enable new alerts */
#define HCI_CONTROL_ANC_EVENT_DISABLE_NEW_ALERTS                    ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x06 )   /* ANC event on complete of disable new alerts */
#define HCI_CONTROL_ANC_EVENT_ENABLE_UNREAD_ALERTS                  ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x07 )   /* ANC event on complete of enable unread alerts */
#define HCI_CONTROL_ANC_EVENT_DISABLE_UNREAD_ALERTS                 ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x08 )   /* ANC event on complete of disable unread alerts */
#define HCI_CONTROL_ANC_EVENT_ANC_DISABLED                          ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x09 )   /* ANC event when disconnected from ANS */
#define HCI_CONTROL_ANC_EVENT_COMMAND_STATUS                        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x0A )   /* Command status event for the requested operation */

/* Events for ANS */
#define HCI_CONTROL_ANS_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x01 )   /* Command status event for the requested operation */
#define HCI_CONTROL_ANS_EVENT_ANS_ENABLED                   ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x02 )   /* ANS event for Alert server is up and running */
#define HCI_CONTROL_ANS_EVENT_CONNECTION_UP                 ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x03 )   /* ANS event whenconnected to ANS client */
#define HCI_CONTROL_ANS_EVENT_CONNECTION_DOWN               ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x04 )   /* ANS event when disconnected from ANS client */

/* Events for AMS */
#define HCI_CONTROL_AMS_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x01 )       /* Command status event for the requested operation */
#define HCI_CONTROL_AMS_EVENT_SERVICE_FOUND                 ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x02 )       /* AMS event for service found */
#define HCI_CONTROL_AMS_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x03 )       /* AMS event for service connected */
#define HCI_CONTROL_AMS_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x04 )       /* AMS event for service disconnected */

/* Events for the FindMe application */
#define HCI_CONTROL_ALERT_EVENT_NOTIFICATION                ( ( HCI_CONTROL_GROUP_ALERT << 8 ) | 0x01 )     /* Alert Level Notification */

/* Events for IAP2 */
#define HCI_CONTROL_IAP2_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x01 )    /* IAP2 connection opened */
#define HCI_CONTROL_IAP2_EVENT_SERVICE_NOT_FOUND            ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x02 )    /* SDP record with IAP2 service not found */
#define HCI_CONTROL_IAP2_EVENT_CONNECTION_FAILED            ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x03 )    /* Connection attempt failed  */
#define HCI_CONTROL_IAP2_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x04 )    /* IAP2 connection closed */
#define HCI_CONTROL_IAP2_EVENT_TX_COMPLETE                  ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x05 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_IAP2_EVENT_RX_DATA                      ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x06 )    /* IAP2 data received */
#define HCI_CONTROL_IAP2_EVENT_AUTH_CHIP_INFO               ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x07 )    /* IAP2 auth chip info */
#define HCI_CONTROL_IAP2_EVENT_AUTH_CHIP_CERTIFICATE        ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x08 )    /* IAP2 auth chip certificate */
#define HCI_CONTROL_IAP2_EVENT_AUTH_CHIP_SIGNATURE          ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x09 )    /* IAP2 auth chip signature */
#define HCI_CONTROL_IAP2_EVENT_RFCOMM_CONNECTED             ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x0A )    /* IAP2 RFCOMM connected*/
#define HCI_CONTROL_IAP2_EVENT_RFCOMM_DISCONNECTED          ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x0B )    /* IAP2 RFCOMM disconnected */
#define HCI_CONTROL_IAP2_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x0C )    /* Command status event for the requested operation */
#define HCI_CONTROL_IAP2_EVENT_PEER_MTU                     ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x0D )    /* Get peer mtu size */

/* Events for Handsfree AG */
#define HCI_CONTROL_AG_EVENT_OPEN                           ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x01 )
#define HCI_CONTROL_AG_EVENT_CLOSE                          ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x02 )
#define HCI_CONTROL_AG_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x03 )
#define HCI_CONTROL_AG_EVENT_AUDIO_OPEN                     ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x04 )
#define HCI_CONTROL_AG_EVENT_AUDIO_CLOSE                    ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x05 )
#define HCI_CONTROL_AG_EVENT_AT_CMD                         ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x06 )
#define HCI_CONTROL_AG_EVENT_CLCC_REQ                       ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x07 )


/* Events for the Broadcom Serial over GATT profile */
#define HCI_CONTROL_BSG_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x01 )    /* BSG Connected */
#define HCI_CONTROL_BSG_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x02 )    /* BSG Disconnected */
#define HCI_CONTROL_BSG_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x03 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_BSG_EVENT_RX_DATA                       ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x04 )    /* BSG data received */

/* HID Host events */
#define HCI_CONTROL_HIDH_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x01 )      /* HIDH Connected */
#define HCI_CONTROL_HIDH_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x02 )      /* HIDH Disconnected */
#define HCI_CONTROL_HIDH_EVENT_DESCRIPTOR                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x03 )      /* HIDH Descriptor */
#define HCI_CONTROL_HIDH_EVENT_SET_REPORT                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x04 )      /* HIDH Set report */
#define HCI_CONTROL_HIDH_EVENT_GET_REPORT                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x05 )      /* HIDH Get report */
#define HCI_CONTROL_HIDH_EVENT_REPORT                       ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x06 )      /* HIDH Get report */
#define HCI_CONTROL_HIDH_EVENT_VIRTUAL_UNPLUG               ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x07 )      /* HIDH Virtual Unplug */
#define HCI_CONTROL_HIDH_EVENT_SET_PROTOCOL                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x08 )      /* HIDH Set Protocol */

#define HCI_CONTROL_HIDH_EVENT_AUDIO_START                  ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x09 )      /* HIDH Audio Start */
#define HCI_CONTROL_HIDH_EVENT_AUDIO_STOP                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x0A )      /* HIDH Audio Stop */
#define HCI_CONTROL_HIDH_EVENT_AUDIO_DATA                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x0B )      /* HIDH Audio Data */

#define HCI_CONTROL_HIDH_EVENT_STATUS                       ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0xFF )      /* HIDH Command status */

/* Events for the Audio sink profile */
#define HCI_CONTROL_AUDIO_SINK_EVENT_COMMAND_STATUS         ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_AUDIO_SINK_EVENT_CONNECTED              ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x02 )    /* Audio connection opened */
#define HCI_CONTROL_AUDIO_SINK_EVENT_SERVICE_NOT_FOUND      ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x03 )    /* SDP record with audio service not found */
#define HCI_CONTROL_AUDIO_SINK_EVENT_CONNECTION_FAILED      ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x04 )    /* Connection attempt failed  */
#define HCI_CONTROL_AUDIO_SINK_EVENT_DISCONNECTED           ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_AUDIO_SINK_EVENT_STARTED                ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x06 )    /* Command for audio start succeeded */
#define HCI_CONTROL_AUDIO_SINK_EVENT_STOPPED                ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x07 )    /* Command for audio stop completed */
#define HCI_CONTROL_AUDIO_SINK_EVENT_CODEC_CONFIGURED       ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x08 )    /* Peer codec configured event*/
#define HCI_CONTROL_AUDIO_SINK_EVENT_START_IND              ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x09 )    /* A2DP Start indication event, received A2DP Start request */
#define HCI_CONTROL_AUDIO_SINK_EVENT_AUDIO_DATA             ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x0a )    /* Received audio data. Encoded for AAC, decoded for SBC. */

/* PBAP Client events */
#define HCI_CONTROL_PBC_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x01 )      /* PBC Connected */
#define HCI_CONTROL_PBC_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x02 )      /* PBC Disconnected */
#define HCI_CONTROL_PBC_EVENT_PHONEBOOK                     ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x03 )      /* Download phonebook event */
#define HCI_CONTROL_PBC_EVENT_CALL_HISTORY                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x04 )      /* Call history event*/
#define HCI_CONTROL_PBC_EVENT_INCOMMING_CALLS               ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x05 )      /* List of incoming calls event*/
#define HCI_CONTROL_PBC_EVENT_OUTGOING_CALLS                ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x06 )      /* List of outgoing calls event*/
#define HCI_CONTROL_PBC_EVENT_MISSED_CALLS                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x07 )      /* List of missed calls event*/
#define HCI_CONTROL_PBC_EVENT_ABORTED                       ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x08 )      /* Aborted operation event */

/* LE COC Events */
#define HCI_CONTROL_LE_COC_EVENT_CONNECTED                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x01)    /* LE COC Connected */
#define HCI_CONTROL_LE_COC_EVENT_DISCONNECTED               ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x02)    /* LE COC Disconnected */
#define HCI_CONTROL_LE_COC_EVENT_RX_DATA                    ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x03)    /* Received data from peer */
#define HCI_CONTROL_LE_COC_EVENT_TX_COMPLETE                ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x04)    /* Data Transmission successfull event */
#define HCI_CONTROL_LE_COC_EVENT_ADV_STS                    ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x05)    /* Advertising status event */

/* Mesh models events */
#define HCI_CONTROL_MESH_EVENT_ONOFF_SET                                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x01 )      /* ON/OFF Server Set */
#define HCI_CONTROL_MESH_EVENT_ONOFF_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x02 )      /* ON/OFF Client status */

#define HCI_CONTROL_MESH_EVENT_LEVEL_SET                                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x03 )      /* Level Server Set */
#define HCI_CONTROL_MESH_EVENT_LEVEL_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x04 )      /* Level Client status */

#define HCI_CONTROL_MESH_EVENT_LOCATION_GLOBAL_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x05 )      /* Set Global Location data */
#define HCI_CONTROL_MESH_EVENT_LOCATION_LOCAL_SET                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x06 )      /* Set Local Location data */
#define HCI_CONTROL_MESH_EVENT_LOCATION_GLOBAL_STATUS                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x07 )      /* Global Location data changed */
#define HCI_CONTROL_MESH_EVENT_LOCATION_LOCAL_STATUS                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x08 )      /* Local Location data changed */

#define HCI_CONTROL_MESH_EVENT_BATTERY_STATUS                               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x09 )      /* Battery status data */

#define HCI_CONTROL_MESH_EVENT_DEF_TRANS_TIME_STATUS                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0a )      /* Default Transition Time Client Status */

#define HCI_CONTROL_MESH_EVENT_POWER_ONOFF_STATUS                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0b )      /* Power ON/OFF Client status */

#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_SET                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0c )      /* Power Level Server Set */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0d )      /* Power Level Server Set Default Power Level */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_RANGE_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0e )      /* Power Level Server Set Min/Max Power Level range */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_STATUS                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x0f )      /* Power Level Client Status */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_LAST_STATUS                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x10 )      /* Last Power Level Client status */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_DEFAULT_STATUS                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x11 )      /* Default Power Level Client status */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_RANGE_STATUS                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x12 )      /* Default Power Level Client status */

#define HCI_CONTROL_MESH_EVENT_PROPERTY_SET                                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x13 )      /* Set Value of the Property */
#define HCI_CONTROL_MESH_EVENT_PROPERTIES_STATUS                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x14 )      /* List of Properties reported by the Server */
#define HCI_CONTROL_MESH_EVENT_PROPERTY_STATUS                              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x15 )      /* Value of the Property Changed status*/

#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x16 )      /* Light Lightness Server Set */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_STATUS                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x17 )      /* Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_LINEAR_STATUS                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x18 )      /* Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_LAST_STATUS                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x19 )      /* Last Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_DEFAULT_STATUS               ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1a )      /* Default Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_RANGE_STATUS                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1b )      /* Range Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_RANGE_SET                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1c )      /* Light Lightness Server Range Set event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_STATUS                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1d )  /* Client Light CTL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_STATUS                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1e )  /* Client Light CTL Temperature status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_RANGE_STATUS           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x1f )  /* Client Light CTL Temperature Range Statust event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_DEFAULT_STATUS                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x20 )  /* Client Light CTL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_SET                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x21 )  /* Server Light CTL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_SET                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x22 )  /* Server Light CTL Temperature Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_RANGE_SET              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x23 )  /* Server Light CTL Temperature Range Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_DEFAULT_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x24 )  /* Server Light CTL Default Set event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_SET                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x25 )  /* Server Light HSL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_STATUS                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x26 )  /* Client Light HSL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_TARGET_STATUS                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x27 )  /* Client Light HSL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_RANGE_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x28 )  /* Server Light HSL Temperature Range Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_RANGE_STATUS                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x29 )  /* Client Light HSL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_DEFAULT_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2a )  /* Server Light HSL Default Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_DEFAULT_STATUS                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2b )  /* Client Light HSL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_HUE_SET                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2c )  /* Server Light HSL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_HUE_STATUS                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2d )  /* Client Light HSL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_SATURATION_SET                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2e )  /* Server Light HSL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_SATURATION_STATUS                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x2f )  /* Client Light HSL status event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_SET                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x30 )  /* Server Light XYL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_STATUS                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x31 )  /* Client Light XYL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_TARGET_STATUS                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x32 )  /* Client Light XYL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_RANGE_SET                          ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x33 )  /* Server Light XYL Temperature Range Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_RANGE_STATUS                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x34 )  /* Client Light XYL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_DEFAULT_SET                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x35 )  /* Server Light XYL Default Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_DEFAULT_STATUS                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x36 )  /* Client Light XYL Default status event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_MODE_SERVER_SET                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x37 )  /* Light LC Server mode Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_MODE_CLIENT_STATUS                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x38 )  /* Light LC Client mode status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_OCCUPANCY_MODE_SERVER_SET           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x39 )  /* Light LC Server Occupancy mode Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_OCCUPANCY_MODE_CLIENT_STATUS        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3a )  /* Light LC Client Occupancy mode status Event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_ONOFF_SERVER_SET                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3b )  /* Light LC Server OnOff Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_ONOFF_CLIENT_STATUS                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3c )  /* Light LC Client OnOff status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_PROPERTY_SERVER_SET                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3d )  /* Light LC Server Property Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_PROPERTY_CLIENT_STATUS              ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3e )  /* Light LC Client Property status event */

#define HCI_CONTROL_MESH_EVENT_SENSOR_DESCRIPTOR_STATUS                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x3f )  /* Value of the Sensor Descriptor status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_STATUS                                ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x40 )  /* Value of the Sensor status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_COLUMN_STATUS                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x41 )  /* Value of the Sensor Column status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_SERIES_STATUS                         ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x42 )  /* Value of the Sensor Series status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_CADENCE_STATUS                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x43 )  /* Value of the Sensor Cadence status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTING_STATUS                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x44 )  /* Value of the Sensor Setting status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTINGS_STATUS                       ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x45 )  /* Value of the Sensor Settings status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_CADENCE_GET                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x46 )  /* Sensor Cadence Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_CADENCE_SET                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x47 )  /* Sensor Cadence Set */
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTING_GET                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x48 )  /* Sensor Setting Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_GET                                   ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x49 )  /* Sensor Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_COLUMN_GET                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4a )  /* Sensor Column Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_SERIES_GET                            ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4b )  /* Sensor Series Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTING_SET                           ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4c )  /* Sensor Setting Set */

#define HCI_CONTROL_MESH_EVENT_SCENE_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4d )  /* Scene status event */
#define HCI_CONTROL_MESH_EVENT_SCENE_REGISTER_STATUS                        ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4e )  /* Scene register status event */

#define HCI_CONTROL_MESH_EVENT_SCHEDULER_STATUS                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x4f )  /* Scheduler register status event */
#define HCI_CONTROL_MESH_EVENT_SCHEDULER_ACTION_STATUS                      ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x50 )  /* Scheduler action status event */

#define HCI_CONTROL_MESH_EVENT_TIME_STATUS                                  ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x51 )  /* Time Status event */
#define HCI_CONTROL_MESH_EVENT_TIME_ZONE_STATUS                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x52 )  /* Time Zone status event */
#define HCI_CONTROL_MESH_EVENT_TIME_TAI_UTC_DELTA_STATUS                    ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x53 )  /* Time TAI_UTC Delta status event */
#define HCI_CONTROL_MESH_EVENT_TIME_ROLE_STATUS                             ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x54 )  /* Time Role status event */
#define HCI_CONTROL_MESH_EVENT_TIME_SET                                     ( ( HCI_CONTROL_GROUP_MESH_MODELS << 8 ) | 0x55 )  /* Time Set event */

/* Mesh events */
#define HCI_CONTROL_MESH_EVENT_COMMAND_STATUS                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x00 )      /* Command status event */
#define HCI_CONTROL_MESH_EVENT_TX_COMPLETE                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x01 )      /* Tx Complete event */

#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_CAPABILITIES_STATUS           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x02 )  /* Remote Provisioning Scanning Server Information */
#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x03 )  /* Remote Provisioning Scan Parameters */
#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_REPORT                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x04 )  /* Remote Provisioning Report the scanned Device UUID */
#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_EXTENDED_REPORT               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x05 )  /* Remote Provisioning report the Advertising data containing the defined AD Type */
#define HCI_CONTROL_MESH_EVENT_PROVISION_LINK_REPORT                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x06 )  /* Provision link established or dropped */
#define HCI_CONTROL_MESH_EVENT_PROVISION_END                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x07 )  /* Provision end event */
#define HCI_CONTROL_MESH_EVENT_PROVISION_DEVICE_CAPABILITIES                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x08 )  /* Provisioning device capabilities */
#define HCI_CONTROL_MESH_EVENT_PROVISION_OOB_DATA                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x09 )  /* Provisioning OOB data request */
#define HCI_CONTROL_MESH_EVENT_PROXY_DEVICE_NETWORK_DATA                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0a )  /* Proxy device network data event */
#define HCI_CONTROL_MESH_EVENT_PROXY_DATA                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0b )  /* Proxy device network data event */

#define HCI_CONTROL_MESH_EVENT_RAW_MODEL_DATA                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0c )  /* Raw model data from the access layer */

#define HCI_CONTROL_MESH_EVENT_RSSI_TEST_RESULT                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0d )  /* RSSI test result */
#define HCI_CONTROL_MESH_EVENT_PRIVATE_BEACON_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0e )  /* Private beacon status event */
#define HCI_CONTROL_MESH_EVENT_PRIVATE_GATT_PROXY_STATUS                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0f )  /* Private GATT proxy status event */
#define HCI_CONTROL_MESH_EVENT_ON_DEMAND_PRIVATE_PROXY_STATUS               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x10 )  /* On-demand private GATT proxy status event */
#define HCI_CONTROL_MESH_EVENT_PRIVATE_NODE_IDENTITY_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x11 )  /* Private node identity status event */
#define HCI_CONTROL_MESH_EVENT_SOLICITATION_PDU_RPL_ITEMS_STATUS            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x12 )  /* Solicitation PDU RPL items status event */
#define HCI_CONTROL_MESH_EVENT_PRIVATE_BEACON                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x13 )  /* Private beacon received (for test only) */
#define HCI_CONTROL_MESH_EVENT_PROXY_SERVICE_ADV                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x14 )  /* Proxy service advertisement received (for test only) */

#define HCI_CONTROL_MESH_EVENT_PROXY_CONNECTION_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x15 )  /* Proxy connection status */

#define HCI_CONTROL_MESH_EVENT_NODE_RESET_STATUS                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x16 )  /* Config Node Reset status */
#define HCI_CONTROL_MESH_EVENT_COMPOSITION_DATA_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x17 )  /* Config Composition Data status */
#define HCI_CONTROL_MESH_EVENT_FRIEND_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x18 )  /* Config Friend status */
#define HCI_CONTROL_MESH_EVENT_GATT_PROXY_STATUS                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x19 )  /* Config GATT Proxy status */
#define HCI_CONTROL_MESH_EVENT_RELAY_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1a )  /* Config Relay status */
#define HCI_CONTROL_MESH_EVENT_DEFAULT_TTL_STATUS                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1b )  /* Config Default TTL status */
#define HCI_CONTROL_MESH_EVENT_BEACON_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1c )  /* Config Beacon status */
#define HCI_CONTROL_MESH_EVENT_NODE_IDENTITY_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1d )  /* Config Node Identity status */
#define HCI_CONTROL_MESH_EVENT_MODEL_PUBLICATION_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1e )  /* Config Model Publication status */
#define HCI_CONTROL_MESH_EVENT_MODEL_SUBSCRIPTION_STATUS                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1f )  /* Config Model Subscription status */
#define HCI_CONTROL_MESH_EVENT_MODEL_SUBSCRIPTION_LIST                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x20 )  /* Config Model Subscription List */
#define HCI_CONTROL_MESH_EVENT_NETKEY_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x21 )  /* Config NetKey status */
#define HCI_CONTROL_MESH_EVENT_NETKEY_LIST                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x22 )  /* Config Netkey List */
#define HCI_CONTROL_MESH_EVENT_APPKEY_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x23 )  /* Config AppKey status */
#define HCI_CONTROL_MESH_EVENT_APPKEY_LIST                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x24 )  /* Config Appkey List */
#define HCI_CONTROL_MESH_EVENT_MODEL_APP_BIND_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x25 )  /* Config Model App status */
#define HCI_CONTROL_MESH_EVENT_MODEL_APP_LIST                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x26 )  /* Config Model App List */
#define HCI_CONTROL_MESH_EVENT_HEARTBEAT_SUBSCRIPTION_STATUS                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x27 )  /* Config Heartbeat Subscription status */
#define HCI_CONTROL_MESH_EVENT_HEARTBEAT_PUBLICATION_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x28 )  /* Config Heartbeat Publication status */
#define HCI_CONTROL_MESH_EVENT_NETWORK_TRANSMIT_PARAMS_STATUS               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x29 )  /* Config Network Transmit status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_CURRENT_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2a )  /* Health Current status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_FAULT_STATUS                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2b )  /* Health Fault status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_PERIOD_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2c )  /* Health Period status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_ATTENTION_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2d )  /* Health Attention status */
#define HCI_CONTROL_MESH_EVENT_LPN_POLL_TIMEOUT_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2e )  /* Low Power node Poll Timeout status */
#define HCI_CONTROL_MESH_EVENT_KEY_REFRESH_PHASE_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2f )  /* Key Refresh Phase status */
#define HCI_CONTROL_MESH_EVENT_PROXY_FILTER_STATUS                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x30 )  /* Proxy Filter status */
#define HCI_CONTROL_MESH_EVENT_CORE_SEQ_CHANGED                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x31 )  /* New Sequence Number */
#define HCI_CONTROL_MESH_EVENT_NVRAM_DATA                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x32 )  /*  */
#define HCI_CONTROL_MESH_EVENT_VENDOR_DATA                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x33 )  /* Vendor model data */

#define HCI_CONTROL_MESH_EVENT_FW_DISTRIBUTION_UPLOAD_STATUS                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x34 )  /* Prepare Distribution Client for FW Upload */

#define HCI_CONTROL_MESH_EVENT_DF_STATS_STATUS                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x35 )      /* Directed Forwarding Statistics Status event */
#define HCI_CONTROL_MESH_EVENT_NETWORK_FILTER_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x36 )      /* Network Filter Status event */

#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_CONTROL_STATUS                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x37 )      /* an unacknowledged message used to report the current Directed Control state of a subnet */
#define HCI_CONTROL_MESH_EVENT_DF_PATH_METRIC_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x38 )      /* an unacknowledged message used to report the current Path Metric state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DISCOVERY_TABLE_CAPABILITIES_STATUS       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x39 )      /* an unacknowledged message used to report the current Discovery Table Capabilities state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_FORWARDING_TABLE_STATUS                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3a )      /* an unacknowledged message used to report the status of the most recent operation performed on the Forwarding Table state of a n */
#define HCI_CONTROL_MESH_EVENT_DF_FORWARDING_TABLE_DEPENDENTS_STATUS        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3b )      /* an unacknowledged message used to report the status of the most recent operation performed on the Dependent_Origin_List field or the Dependent_Target_List field of a fixed path entry in the Forwarding Table state of a node. */
#define HCI_CONTROL_MESH_EVENT_DF_FORWARDING_TABLE_DEPENDENTS_GET_STATUS    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3c )      /* an unacknowledged message used to report status information for a filtered list of unicast address ranges of dependent nodes of the Path Origin or the Path Target of a Forwarding Table entry */
#define HCI_CONTROL_MESH_EVENT_DF_FORWARDING_TABLE_ENTRIES_COUNT_STATUS     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3d )      /* an unacknowledged message used to report the information about the Forwarding Table state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_FORWARDING_TABLE_ENTRIES_STATUS           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3e )      /* an unacknowledged message used to report status information for a filtered set of path entries in the Forwarding Table state of a node (see Section 4.2.29), optionally including a list of path entries. */
#define HCI_CONTROL_MESH_EVENT_DF_WANTED_LANES_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3f )      /* an unacknowledged message used to report the current Wanted Lanes state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_TWO_WAY_PATH_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x40 )      /* an unacknowledged message used to report the current Two Way Path state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_PATH_ECHO_INTERVAL_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x41 )      /* an unacknowledged message used to report the current Path Echo Interval state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_NETWORK_TRANSMIT_STATUS          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x42 )      /* an unacknowledged message used to report the current Directed Network Transmit state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_RELAY_RETRANSMIT_STATUS          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x43 )      /* an unacknowledged message used to report the current Directed Relay Retransmit state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_RSSI_THRESHOLD_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x44 )      /* an unacknowledged message used to report the current RSSI Threshold state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_PATHS_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x45 )      /* an unacknowledged message used to report the Directed Paths state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_PUBLISH_POLICY_STATUS            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x46 )      /* an unacknowledged message used to report the current Directed Publish Policy state of a model of an element of a node */
#define HCI_CONTROL_MESH_EVENT_DF_PATH_DISCOVERY_TIMING_CONTROL_STATUS      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x47 )      /* an unacknowledged message used to report the current Path Discovery Timing Control state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x48 )      /* an unacknowledged message used to report the current Directed Control Network Transmit state of a node */
#define HCI_CONTROL_MESH_EVENT_DF_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x49 )      /* an unacknowledged message used to report the current Directed Control Relay Retransmit state of a node */

/* Events for the Battery Client profile */
#define HCI_CONTROL_BATT_CLIENT_EVENT_CONNECTED             ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x00 )    /* Battery Client connected */
#define HCI_CONTROL_BATT_CLIENT_EVENT_DISCONNECTED          ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x01 )    /* Battery Client disconnected */
#define HCI_CONTROL_BATT_CLIENT_EVENT_LEVEL                 ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x02 )    /* Battery Client Level */
#define HCI_CONTROL_BATT_CLIENT_EVENT_STATUS                ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0xFF )    /* Battery Client status */

/* Events for the FindMe Locator Client profile */
#define HCI_CONTROL_FINDME_LOCATOR_EVENT_CONNECTED          ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x00 )    /* FindMe Target connected */
#define HCI_CONTROL_FINDME_LOCATOR_EVENT_DISCONNECTED       ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x01 )    /* FindMe Target disconnected */
#define HCI_CONTROL_FINDME_LOCATOR_EVENT_STATUS             ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0xFF )    /* FindMe Locator Command status */

/* OPP Server events */
#define HCI_CONTROL_OPS_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x00 )      /* Connection to peer is open. */
#define HCI_CONTROL_OPS_EVENT_PROGRESS                      ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x01 )      /* Object being sent or received. */
#define HCI_CONTROL_OPS_EVENT_OBJECT                        ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x02 )      /* Object has been received */
#define HCI_CONTROL_OPS_EVENT_CLOSE                         ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x03 )      /* Connection to peer closed */
#define HCI_CONTROL_OPS_EVENT_ACCESS                        ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x04 )      /* Request for access to push or pull object */
#define HCI_CONTROL_OPS_EVENT_PUSH_DATA                     ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x05 )      /* Push data received */

/* OTP events */
#define HCI_CONTROL_OTP_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x00 )      /* Connected to Object Server */
#define HCI_CONTROL_OTP_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x01 )      /* Disconnected from Object Server */
#define HCI_CONTROL_OTP_EVENT_DISCOVERY_COMPLETE            ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x02 )      /* Object Server Service and Characteristics discovery complete */
#define HCI_CONTROL_OTP_EVENT_UPGRADE_INIT_DONE             ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x03 )      /* OTP Procedures completed. Start with the firmware image transfer */
#define HCI_CONTROL_OTP_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x04 )      /* TX complete */

/* Miscellaneous events */
#define HCI_CONTROL_MISC_EVENT_PING_REPLY                   ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x01 )    /* Ping reply */
#define HCI_CONTROL_MISC_EVENT_VERSION                      ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x02 )    /* SDK Version */

/* Script events */
#define HCI_CONTROL_SCRIPT_EVENT_RET_CODE                   ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x01 )   /* Script command return code */
#define HCI_CONTROL_SCRIPT_EVENT_UNKNOWN_CMD                ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x02 )   /* Unknown Script command */
#define HCI_CONTROL_SCRIPT_EVENT_CALLBACK                   ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x03 )   /* Async script callback */

/* Demo events */
#define HCI_CONTROL_DEMO_EVENT_SSID_PASSWD                  ( ( HCI_CONTROL_GROUP_DEMO << 8 ) | 0x01 )    /* SSID and Password */

/* Scan state events that are reported with the HCI_CONTROL_LE_EVENT_SCAN_STATUS */
#define HCI_CONTROL_SCAN_EVENT_NO_SCAN                      0
#define HCI_CONTROL_SCAN_EVENT_HIGH_SCAN                    1
#define HCI_CONTROL_SCAN_EVENT_LOW_SCAN                     2
#define HCI_CONTROL_SCAN_EVENT_HIGH_CONN                    3
#define HCI_CONTROL_SCAN_EVENT_LOW_CONN                     4

/* MCE events */
#define HCI_CONTROL_MCE_EVENT_MAS_INSTANCES                 ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x00 )      /* MAS instances report */
#define HCI_CONTROL_MCE_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x01 )      /* Connected */
#define HCI_CONTROL_MCE_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x02 )      /* Disconnected */
#define HCI_CONTROL_MCE_EVENT_FOLDER_LIST                   ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x03 )      /* List folders result */
#define HCI_CONTROL_MCE_EVENT_FOLDER_SET                    ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x04 )      /* Set folder result */
#define HCI_CONTROL_MCE_EVENT_MESSAGE_LIST                  ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x05 )      /* List messages result */
#define HCI_CONTROL_MCE_EVENT_MESSAGE                       ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x06 )      /* Get message result */
#define HCI_CONTROL_MCE_EVENT_MESSAGE_PUSHED                ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x07 )      /* Push message result */
#define HCI_CONTROL_MCE_EVENT_INBOX_UPDATED                 ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x08 )      /* Update inbox result */
#define HCI_CONTROL_MCE_EVENT_MESSAGE_STATUS_SET            ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x09 )      /* Set message status result */
#define HCI_CONTROL_MCE_EVENT_ABORTED                       ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x0A )      /* Aborted */
#define HCI_CONTROL_MCE_EVENT_NOTIF_REG                     ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x0B )      /* Notification registration result */
#define HCI_CONTROL_MCE_EVENT_NOTIF                         ( ( HCI_CONTROL_GROUP_MCE << 8 ) | 0x0C )      /* Notification */

/* HomeKit events */
#define HCI_CONTROL_HK_EVENT_READ_RESPONSE                  ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x01 )    /* Response to read characteristic command */
#define HCI_CONTROL_HK_EVENT_UPDATE                         ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x02 )    /* Characteristic value update */
#define HCI_CONTROL_HK_EVENT_LIST_ITEM                      ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x03 )    /* Characteristic list item */
#define HCI_CONTROL_HK_EVENT_TOKEN_DATA                     ( ( HCI_CONTROL_GROUP_HK << 8 ) | 0x04 )    /* Software token data */

/* HCI Audio events */
#define HCI_CONTROL_HCI_AUDIO_EVENT_COMMAND_STATUS          ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_HCI_AUDIO_EVENT_SCO_DATA                ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x02 )    /* SCO data from HFP */
#define HCI_CONTROL_HCI_AUDIO_EVENT_STREAM_START            ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x03 )    /* Stream start */
#define HCI_CONTROL_HCI_AUDIO_EVENT_STREAM_STOP             ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x04 )    /* Stream stop */
#define HCI_CONTROL_HCI_AUDIO_EVENT_STREAM_CONFIG           ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x05 )    /* Stream configure */
#define HCI_CONTROL_HCI_AUDIO_EVENT_STREAM_VOLUME           ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x06 )    /* Stream volume */
#define HCI_CONTROL_HCI_AUDIO_EVENT_STREAM_MIC_GAIN         ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x07 )    /* Stream MIC gain */
#define HCI_CONTROL_HCI_AUDIO_EVENT_WRITE_NVRAM_DATA        ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x08 )    /* Write NVRAM data */
#define HCI_CONTROL_HCI_AUDIO_EVENT_DELETE_NVRAM_DATA       ( ( HCI_CONTROL_GROUP_HCI_AUDIO << 8 ) | 0x09 )    /* Delete NVRAM data */

/* HCI DFU events */
#define HCI_CONTROL_DFU_EVENT_CONFIG                        ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x01 ) /* configuration data */
#define HCI_CONTROL_DFU_EVENT_STARTED                       ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x02 ) /* upgrade started */
#define HCI_CONTROL_DFU_EVENT_DATA                          ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x03 ) /* data received */
#define HCI_CONTROL_DFU_EVENT_VERIFICATION                  ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x04 ) /* verification Complete */
#define HCI_CONTROL_DFU_EVENT_VERIFIED                      ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x05 ) /* verified */
#define HCI_CONTROL_DFU_EVENT_ABORTED                       ( ( HCI_CONTROL_GROUP_DFU << 8 ) | 0x06 ) /* aborted */

/* HCI TEST events */
#define HCI_CONTROL_HCITEST_EVENT_STATUS                    ( ( HCI_CONTROL_GROUP_HCITEST << 8 ) | 0x01 ) /* test status */
#define HCI_CONTROL_HCITEST_EVENT_PACKET                    ( ( HCI_CONTROL_GROUP_HCITEST << 8 ) | 0x02 ) /* test packet (for loopback) */

/* Connected Mesh Events */
#define HCI_CONTROL_CONN_MESH_EVENT_NODE_PROVISIONED        ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x01 )  /* Node addr */
#define HCI_CONTROL_CONN_MESH_EVENT_GET_DATA                ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x02 )  /* Application Data Received */
#define HCI_CONTROL_CONN_MESH_EVENT_APP_DATA                ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x03 )  /* Application Data Received */
#define HCI_CONTROL_CONN_MESH_EVENT_NODE_LIST               ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x04 )  /* Node List report */
#define HCI_CONTROL_CONN_MESH_EVENT_CONN_STATUS             ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x05 )  /* Connection Status update */
#define HCI_CONTROL_CONN_MESH_EVENT_PING_RESULT             ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x06 )  /* Ping Result */
#define HCI_CONTROL_CONN_MESH_EVENT_DATA_STATS              ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x07 )  /* Stats */
#define HCI_CONTROL_CONN_MESH_EVENT_CONN_UP                 ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x08 )  /* Node addr */
#define HCI_CONTROL_CONN_MESH_EVENT_CONN_DOWN               ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x09 )  /* Node addr */
#define HCI_CONTROL_CONN_MESH_EVENT_OP_STATE_CHANGED        ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x0a )  /* Node addr */
#define HCI_CONTROL_CONN_MESH_EVENT_RESET_COMPLETE          ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x0b )  /* Node addr */
#define HCI_CONTROL_CONN_MESH_EVENT_RSSI_VALUES             ( ( HCI_CONTROL_GROUP_CONN_MESH << 8 ) | 0x0c )  /* [Src Node] [Peer addr][RSSI] ... */

#define HCI_CONTROL_PANU_EVENT_OPEN                         ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x01 )
#define HCI_CONTROL_PANU_EVENT_CLOSE                        ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x02 )
#define HCI_CONTROL_PANU_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x03 )
#define HCI_CONTROL_PANU_EVENT_SERVICE_NOT_FOUND            ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x04 )
#define HCI_CONTROL_PANU_EVENT_CONNECTION_FAILED            ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x05 )
#define HCI_CONTROL_PANU_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_PANU << 8 ) | 0x06 )


/* Status codes returned in HCI_CONTROL_EVENT_COMMAND_STATUS the event */
#define HCI_CONTROL_STATUS_SUCCESS                          0
#define HCI_CONTROL_STATUS_IN_PROGRESS                      1
#define HCI_CONTROL_STATUS_ALREADY_CONNECTED                2
#define HCI_CONTROL_STATUS_NOT_CONNECTED                    3
#define HCI_CONTROL_STATUS_BAD_HANDLE                       4
#define HCI_CONTROL_STATUS_WRONG_STATE                      5
#define HCI_CONTROL_STATUS_INVALID_ARGS                     6
#define HCI_CONTROL_STATUS_FAILED                           7
#define HCI_CONTROL_STATUS_UNKNOWN_GROUP                    8
#define HCI_CONTROL_STATUS_UNKNOWN_COMMAND                  9
#define HCI_CONTROL_STATUS_CLIENT_NOT_REGISTERED            10
#define HCI_CONTROL_STATUS_OUT_OF_MEMORY                    11
#define HCI_CONTROL_STATUS_DISALLOWED                       12

/* HS open status codes */
#define HCI_CONTROL_HF_STATUS_SUCCESS                       0   /* Connection successfully opened */
#define HCI_CONTROL_HF_STATUS_FAIL_SDP                      1   /* Open failed due to SDP */
#define HCI_CONTROL_HF_STATUS_FAIL_RFCOMM                   2   /* Open failed due to RFCOMM */
#define HCI_CONTROL_HF_STATUS_FAIL_CONN_TOUT                3   /* Link loss occured due to connection timeout */

/* BSG status codes */
#define HCI_CONTROL_BSG_STATUS_SUCCESS                      0
#define HCI_CONTROL_BSG_STATUS_NOT_CONNECTED                1
#define HCI_CONTROL_BSG_STATUS_OVERRUN                      2


#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN 6
#endif

/* LE advertisement states */
#define LE_ADV_STATE_NO_DISCOVERABLE                        0
#define LE_ADV_STATE_HIGH_DISCOVERABLE                      1
#define LE_ADV_STATE_LOW_DISCOVERABLE                       2

/* HID Report Channel */
#define HCI_CONTROL_HID_REPORT_CHANNEL_CONTROL              0
#define HCI_CONTROL_HID_REPORT_CHANNEL_INTERRUPT            1

/* HID Report Type (matches BT HID Spec definitions) */
#define HCI_CONTROL_HID_REPORT_TYPE_OTHER                   0
#define HCI_CONTROL_HID_REPORT_TYPE_INPUT                   1
#define HCI_CONTROL_HID_REPORT_TYPE_OUTPUT                  2
#define HCI_CONTROL_HID_REPORT_TYPE_FEATURE                 3

#define HCI_CONTROL_HID_REPORT_ID                           1

/* Max TX packet to be sent over SPP */
#define HCI_CONTROL_SPP_MAX_TX_BUFFER                       700

/* Max TX packet to be sent over IAP2 */
#define HCI_CONTROL_IAP2_MAX_TX_BUFFER                       700

/* Max GATT command packet size to be sent over UART */
#define HCI_CONTROL_GATT_COMMAND_MAX_TX_BUFFER              100

/* Gatt Operation Status */
#define HCI_CONTROL_GATT_STATUS_INSUF_AUTHENTICATION        5

/* Define Player Setting capabilities */
#define HCI_CONTROL_PLAYER_EQUALIZER_ENABLED

#define HCI_CONTROL_PLAYER_REPEAT_ENABLED
#define HCI_CONTROL_PLAYER_REPEAT_SINGLE_ENABLED
#define HCI_CONTROL_PLAYER_REPEAT_ALL_ENABLED
#define HCI_CONTROL_PLAYER_REPEAT_GROUP_ENABLED

#define HCI_CONTROL_PLAYER_SHUFFLE_ENABLED
#define HCI_CONTROL_PLAYER_SHUFFLE_ALL_ENABLED
#define HCI_CONTROL_PLAYER_SHUFFLE_GROUP_ENABLED

#define HCI_CONTROL_MESH_STATUS_SUCCESS                     0   /* Command executed successfully */
#define HCI_CONTROL_MESH_STATUS_ERROR                       1   /* Command start failed */
#define HCI_CONTROL_MESH_STATUS_CONNECT_FAILED              2

/* MCE command/event parameter types */
#define HCI_CONTROL_MCE_PARAM_BDA                           1   /* BD address, 6 bytes */
#define HCI_CONTROL_MCE_PARAM_MAS_INS_ID                    2   /* MAS instance ID, 1 byte */
#define HCI_CONTROL_MCE_PARAM_SESS_HANDLE                   3   /* MAP session handle, 2 bytes */
#define HCI_CONTROL_MCE_PARAM_FOLDER                        4   /* Folder name, variable length */
#define HCI_CONTROL_MCE_PARAM_NAV_FLAG                      5   /* Folder navigation flag (up/down/root), 1 byte */
#define HCI_CONTROL_MCE_PARAM_MSG_HANDLE                    6   /* Message handle, 8 bytes */
#define HCI_CONTROL_MCE_PARAM_CHARSET                       7   /* Charset, 1 byte */
#define HCI_CONTROL_MCE_PARAM_MSG_STATUS_INDIC              8   /* Message status indicator: read/delete, 1 byte */
#define HCI_CONTROL_MCE_PARAM_MSG_STATUS_VALUE              9   /* Message status value: yes/no, 1 byte */
#define HCI_CONTROL_MCE_PARAM_DATA                          10  /* Data (folder/message list, message), variable length */
#define HCI_CONTROL_MCE_PARAM_DATA_END                      11  /* Last part of data (folder/message list, message), variable length */
#define HCI_CONTROL_MCE_PARAM_STATUS                        12  /* Event status, 1 byte */
#define HCI_CONTROL_MCE_PARAM_NUM_MAS_INST                  13  /* Number of MAS instances, 1 byte */
#define HCI_CONTROL_MCE_PARAM_NAME                          14  /* Name, variable length */
#define HCI_CONTROL_MCE_PARAM_SUPPORTED_TYPE                15  /* Supported message type, 1 byte */
#define HCI_CONTROL_MCE_PARAM_NOTIF_STATUS                  16  /* Notification status (1:ON, 0:OFF), 1 byte */
#define HCI_CONTROL_MCE_PARAM_MAX_LIST_COUNT                17  /* Maximum number of items listed, 2 bytes */
#define HCI_CONTROL_MCE_PARAM_LIST_START_OFFSET             18  /* Offset of the first item in list, 2 bytes */
#define HCI_CONTROL_MCE_PARAM_SRMP_ENABLE                   19  /* SRMP Enable, 1 byte */

/* HomeKit software token data flags */
#define HCI_TOKEN_DATA_FLAG_START                           0x01
#define HCI_TOKEN_DATA_FLAG_END                             0x02
#define HCI_TOKEN_DATA_FLAG_UUID                            0x04


#define HCI_CONTROL_PANU_STATUS_SUCCESS                       0   /* Connection successfully opened */
#define HCI_CONTROL_PANU_STATUS_FAIL_SDP                      1   /* Open failed due to SDP */
#define HCI_CONTROL_PANU_STATUS_FAIL_RFCOMM                   2   /* Open failed due to RFCOMM */
#define HCI_CONTROL_PANU_STATUS_FAIL_CONN_TOUT                3   /* Link loss occured due to connection timeout */

#endif /* HCI_CONTROL_API.H_ */
