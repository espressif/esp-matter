/******************************************************************************

 @file  provisioning_gatt_profile.h

 @brief This file contains the GATT provisioning profile definitions and prototypes
        prototypes.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2010-2020, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef PROVISIONINGGATTPROFILE_H
#define PROVISIONINGGATTPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */



// Provisioning Profile Services bit fields
#define PROVPROFILE_SERVICE               0x00000001

// Maximum user description length in bytes for characteristics
#define PROVISIONING_MAX_DESCRIPTION_LEN (50)

// Length, in bytes, of the network id
#define PROVISIONING_NTWK_ID_LEN (2)

// Length, in bytes, of the extended network id
#define PROVISIONING_EXT_NTWK_ID_LEN (8)

// Length, in bytes, of the network channel mask mapping
#if defined(DMM_ZEDSWITCH) || defined(DMM_ZRLIGHT) || defined(DMM_ZCSWITCH) || defined(USEOT)
#define PROVISIONING_NTWK_CHNL_LEN (4)
#else
#define PROVISIONING_NTWK_CHNL_LEN (17)
#endif

// Length, in bytes, of the IEEE FFD address
#define PROVISIONING_IEEE_FFD_ADDR_LEN (8)

// Length, in bytes, of the network security key
#define PROVISIONING_NTWK_KEY_LEN (16)

// Length, in bytes, of single byte attribute lengths
#define PROVISIONING_GENERIC_LEN (1)

#ifdef USEOT
// Length, in bytes, of the short address
#define PROVISIONING_SHORT_ADDR_LEN (2)

// Length, in bytes, of the network name <Network Name Length = 16, Null terminator +1>
#define PROVISIONING_NTWK_NAME_LEN (17)

// Length, in bytes, of the network PSKd
#define PROVISIONING_PROV_PSKD_LEN (8)

#endif

// Profile Parameters
#define PROVPROFILE_NTWK_PAN_ID_CHAR            0  // RW uint8[2] - Network PAN ID characteristic index
#define PROVPROFILE_EXT_NTWK_PAN_ID_CHAR        1  // RW uint8[8] - Extended Network PAN ID characteristic index
#define PROVPROFILE_SENSOR_FREQ_CHAR            2  // RW uint8 - Network Frequency of operation characteristic index
#define PROVPROFILE_SENSOR_CHANNEL_CHAR         3  // RW uint8[17] - Network channel list characteristic index
#define PROVPROFILE_IEEE_FFD_ADDR_CHAR          4  // RW uint8[8] - IEEE Full function device address characteristic index
#define PROVPROFILE_NTWK_KEY_CHAR               5  // RW uint8[16] - Network security key characteristic index
#define PROVPROFILE_PROV_SENSOR_CHAR            6  // RW uint8 - Sensor provisioning start characteristic index
#define PROVPROFILE_PROV_STATE_CHAR             7  // RN uint8 - Sensor provisioning state characteristic index
#ifdef USEOT
#define PROVPROFILE_PROV_PSKD_CHAR                   8  // R uint8[6] "123456" HARDCODED
#define PROVPROFILE_NTWK_NAME_CHAR              9  // RW uint8[16] -> 16 byte UTF8 string HARDCODED
#define PROVPROFILE_NTWK_SHORT_ADDR_CHAR        10 // R  uint8[2] - Network Short Address
#define PROVPROFILE_NTWK_RESET_CHAR                  11 // RW uint8 - Reset network
#endif

// Provisioning Profile Service UUID
#define PROVPROFILE_SERV_UUID               0x1190
#define PROVPROFILE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// Provisioning characteristic UUID mapping
#define PROVPROFILE_CHAR_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// Provisioning Profile characteristic UUID's
#define PROVPROFILE_NTWK_PAN_ID_CHAR_UUID            0x1191
#define PROVPROFILE_EXT_NTWK_PAN_ID_CHAR_UUID        0x1192
#define PROVPROFILE_SENSOR_FREQ_CHAR_UUID            0x1193
#define PROVPROFILE_SENSOR_CHANNEL_CHAR_UUID         0x1194
#define PROVPROFILE_IEEE_FFD_ADDR_CHAR_UUID          0x1195
#define PROVPROFILE_NTWK_KEY_CHAR_UUID               0x1196
#define PROVPROFILE_PROV_SENSOR_CHAR_UUID            0x1197
#define PROVPROFILE_PROV_STATE_CHAR_UUID             0x1198
#ifdef USEOT
#define PROVPROFILE_PROV_PSKD_CHAR_UUID              0x1199
#define PROVPROFILE_NTWK_NAME_CHAR_UUID              0x119A
#define PROVPROFILE_NTWK_SHORT_ADDR_CHAR_UUID        0x119B
#define PROVPROFILE_NTWK_RESET_CHAR_UUID             0x119C
#endif

// Provisioning Characteristic Lengths in bytes
#define PROVPROFILE_NTWK_PAN_ID_CHAR_LEN             (2)
#define PROVPROFILE_EXT_NTWK_PAN_ID_CHAR_LEN         (8)
#define PROVPROFILE_SENSOR_FREQ_CHAR_LEN             (1)
#if defined(DMM_ZEDSWITCH) || defined(DMM_ZRLIGHT) || defined(DMM_ZCSWITCH) || defined(USEOT)
#define PROVPROFILE_SENSOR_CHANNEL_CHAR_LEN          (4)
#else
#define PROVPROFILE_SENSOR_CHANNEL_CHAR_LEN          (17)
#endif
#define PROVPROFILE_IEEE_FFD_ADDR_CHAR_LEN           (8)
#define PROVPROFILE_NTWK_KEY_CHAR_LEN                (16)
#define PROVPROFILE_PROV_SENSOR_CHAR_LEN             (1)
#define PROVPROFILE_PROV_STATE_CHAR_LEN              (1)
#define PROVISIONING_GENERIC_CHAR_LEN                (1)

#ifdef USEOT
#define PROVPROFILE_PROV_PSKD_CHAR_LEN              (8)
#define PROVPROFILE_NTWK_NAME_CHAR_LEN              (16)
#define PROVPROFILE_NTWK_SHORT_ADDR_CHAR_LEN        (2)
#define PROVPROFILE_NTWK_RESET_CHAR_LEN             (1)
#endif

#ifdef USEOT
#define PROVPROFILE_ATTACH           (0xAA)
#define PROVPROFILE_JOIN             (0xBB)
#define PROVPROFILE_RESETDEVICE      (0x01)
#define PROVPROFILE_RESETNETWORK      (0x02)
#endif

#define PROVPROFILE_PAN_CONNECT      (0xAA)
#define PROVPROFILE_PAN_DISCONNECT   (0xDD)
#define PROVPROFILE_PAN_NETWORKOPEN  (0x55)
#define PROVPROFILE_PAN_NETWORKCLOSE (0xCC)


/*********************************************************************
 * TYPEDEFS
 */
/*!
 Provisioning profile Device State Values
*/
typedef enum
{
    /*! Powered up, not provisioned and waiting for user to provision device */
    Provisioning_states_initWaiting,
    /*! Device successfully provisioned and operating in network */
    Provisioning_states_joined,
    /*! Device is restored as device in the network */
    Provisioning_states_rejoined,
    /*! Device has lost connection to network */
    Provisioning_states_orphan,
    /*! Device join access denied or network at capacity */
    Provisioning_states_accessDenied
} ProvisioningProfile_states_t;


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*provisioningProfileChange_t)( uint8 paramID );

typedef struct
{
  provisioningProfileChange_t        pfnRemoteDisplayProfileChange;  // Called when characteristic value changes
} provisioningProfileCBs_t;

    

/*********************************************************************
 * API FUNCTIONS 
 */


/*
 * ProvisioningProfile_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t ProvisioningProfile_AddService( uint32 services );

/*
 * ProvisioningProfile_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ProvisioningProfile_RegisterAppCBs( provisioningProfileCBs_t *appCallbacks );

/*
 * ProvisioningProfile_SetParameter - Set a Remote Display GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProvisioningProfile_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * ProvisioningProfile_GetParameter - Get a Remote Display GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProvisioningProfile_GetParameter( uint8 param, void *value );

#ifdef USEOT
/*
 * ProvisioningProfile_SetProvisionLock - Set the provisioning lock state
 *
 *    state - What state to lock it into
 */
extern void ProvisioningProfile_SetProvisionLock(bool state);
#endif
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PROVISIONINGGATTPROFILE_H */
