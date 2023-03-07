/******************************************************************************

 @file  provisioning_gatt_profile.c

 @brief This file contains the GATT Provisioning profile used to provision a 15.4
        sensor device.

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

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "provisioning_gatt_profile.h"
#include "ti_ble_config.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Provisioning GATT Profile Service UUID
CONST uint8 provisioningProfileServUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_SERV_UUID_BASE128(PROVPROFILE_SERV_UUID)
};

// Network PAN ID characteristic UUID
CONST uint8 provisioningProfileNtwkPanIdCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_NTWK_PAN_ID_CHAR_UUID)
};
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
// Extended Network PAN ID characteristic UUID
CONST uint8 provisioningProfileExtNtwkPanIdCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_EXT_NTWK_PAN_ID_CHAR_UUID)
};

// Network Frequency of operation characteristic UUID
CONST uint8 provisioningProfileSensorFreqCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_SENSOR_FREQ_CHAR_UUID)
};
#endif
// Network channel list characteristic UUID
CONST uint8 provisioningProfileSensorChannelCharUUID[ATT_UUID_SIZE] =
{ 
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_SENSOR_CHANNEL_CHAR_UUID)
};

#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
// IEEE Full function device address characteristic UUID
CONST uint8 provisioningProfileIeeeFfdAddrCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_IEEE_FFD_ADDR_CHAR_UUID)
};

// Network security key characteristic UUID
CONST uint8 provisioningProfileNtwkKeyCharUUID[ATT_UUID_SIZE] =
{ 
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_NTWK_KEY_CHAR_UUID)
};
#endif
// Sensor provisioningProfile start characteristic UUID
CONST uint8 provisioningProfileSensorCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_PROV_SENSOR_CHAR_UUID)
};

// Sensor provisioningProfile state characteristic UUID
CONST uint8 provisioningProfileStateCharUUID[ATT_UUID_SIZE] =
{ 
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_PROV_STATE_CHAR_UUID)
};
#ifdef USEOT
// Sensor provisioningProfile PSKd characteristic UUID
CONST uint8 provisioningProfilePSKdCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_PROV_PSKD_CHAR_UUID)
};
// Sensor provisioningProfile Network Name characteristic UUID
CONST uint8 provisioningProfileNtwkNameCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_NTWK_NAME_CHAR_UUID)
};
// Sensor provisioningProfile Short Address characteristic UUID
CONST uint8 provisioningProfileShortAddrCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_NTWK_SHORT_ADDR_CHAR_UUID)
};

//Sensor provisioningProfile reset network characteristic UUID
CONST uint8 provisioningProfileNtwkResetCharUUID[ATT_UUID_SIZE] =
{
  PROVPROFILE_CHAR_UUID_BASE128(PROVPROFILE_NTWK_RESET_CHAR_UUID)
};
#endif

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static provisioningProfileCBs_t *provisioningProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

//  Remote Display Profile Service attribute
static CONST gattAttrType_t provisioningProfileService = { ATT_UUID_SIZE, provisioningProfileServUUID };

// Provisioning Profile Network PAN ID Characteristic Properties
static uint8_t provisioningProfileNtwkPanIdCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Network PAN ID Characteristic Value
static uint8_t provisioningProfileNtwkPanIdCharVal[PROVISIONING_NTWK_ID_LEN] = {0x00, 0x00};

// Provisioning Profile Network PAN ID Characteristic User Description
static uint8_t provisioningProfileNtwkPanIdDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Network PAN ID";

#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
// Provisioning Profile Extended Network PAN ID Characteristic Properties
static uint8_t provisioningProfileExtNtwkPanIdCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Extended Network PAN ID Characteristic Value
static uint8_t provisioningProfileExtNtwkPanIdCharVal[PROVISIONING_EXT_NTWK_ID_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Provisioning Profile Extended Network PAN ID Characteristic User Description
static uint8_t provisioningProfileExtNtwkPanIdDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Extended Network PAN ID";

// Provisioning Profile Sensor Frequency Characteristic Properties
#ifdef USEOT
static uint8_t provisioningProfileSensorFreqCharProps = GATT_PROP_READ | GATT_PROP_WRITE;
#else
static uint8_t provisioningProfileSensorFreqCharProps = GATT_PROP_READ;
#endif

// Sensor Frequency Characteristic Value
static uint8_t provisioningProfileSensorFreqCharVal = 0x00;

#if defined(DMM_COLLECTOR)
static uint8_t provisioningProfileSensorFreqDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Collector operating frequency";
#else /* DMM_SENSOR */
#ifdef USEOT
static uint8_t provisioningProfileSensorFreqDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Sensor Channel";
#else
static uint8_t provisioningProfileSensorFreqDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Sensor operating frequency";
#endif
#endif

#endif /*!defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) */

// Provisioning Profile Sensor Channel Characteristic Properties
static uint8_t provisioningProfileSensorChannelCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Sensor Channel Characteristic Value
#if defined(DMM_ZEDSWITCH) || defined(DMM_ZRLIGHT) || defined(DMM_ZCSWITCH) || defined(USEOT)
static uint8_t provisioningProfileSensorChannelCharVal[PROVISIONING_NTWK_CHNL_LEN] = {0x00, 0x00, 0x00, 0x00};
#else
static uint8_t provisioningProfileSensorChannelCharVal[PROVISIONING_NTWK_CHNL_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

// Provisioning Profile Sensor Channel Characteristic User Description
#if defined(DMM_ZEDSWITCH)
static uint8_t provisioningProfileSensorChannelDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "ZED operating channels";
#elif defined(DMM_ZRLIGHT)
static uint8_t provisioningProfileSensorChannelDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "ZR operating channels";
#elif defined(DMM_ZCSWITCH)
static uint8_t provisioningProfileSensorChannelDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "ZC operating channels";
#elif defined(DMM_COLLECTOR)
static uint8_t provisioningProfileSensorChannelDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Collector operating channels";
#elif defined(USEOT)
static uint8_t provisioningProfileSensorChannelDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Sensor Channel Mask";
#else
static uint8_t provisioningProfileSensorChannelDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Sensor operating channels";
#endif


#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
// Provisioning Profile IEEE Full Function Device (FFD) Address Characteristic Properties
static uint8_t provisioningProfileIeeeFfdAddrCharProps = GATT_PROP_READ;

// IEEE FFD Address Characteristic Value
static uint8_t provisioningProfileIeeeFfdAddrCharVal[PROVISIONING_IEEE_FFD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Provisioning Profile IEEE FFD Address Characteristic User Description
#ifdef USEOT
static uint8_t provisioningProfileIeeeFfdAddrDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "EUI64";
#else
static uint8_t provisioningProfileIeeeFfdAddrDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "IEEE FFD address";
#endif
// Provisioning Profile Network Security Key Characteristic Properties
static uint8_t provisioningProfileNtwkKeyCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Network Security Key Characteristic Value
static uint8_t provisioningProfileNtwkKeyCharVal[PROVISIONING_NTWK_KEY_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Provisioning Profile Network Security Key Characteristic User Description
#ifdef USEOT
static uint8_t provisioningProfileNtwkKeyDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Masterkey";
#else
static uint8_t provisioningProfileNtwkKeyDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Network Security Key";
#endif
#endif
#ifdef USEOT

static bool provisionAttributeLock;
// Provisioning Profile Network Name Characteristic Properties
static uint8_t provisioningProfileNtwkNameCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Network Name Characteristic Value
static uint8_t provisioningProfileNtwkNameCharVal[PROVISIONING_NTWK_NAME_LEN] = "TI-OpenThread";

// Provisioning Profile Network Name Characteristic User Description
static uint8_t provisioningProfileNtwkNameDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Network Name";

// Provisioning Profile Short Address Characteristic Properties
static uint8_t provisioningProfileShortAddrCharProps = GATT_PROP_READ;

// Short Address Characteristic Value
static uint8_t provisioningProfileShortAddrCharVal[PROVISIONING_SHORT_ADDR_LEN] = {0x00, 0x00};

// Provisioning Profile Short Address Characteristic User Description
static uint8_t provisioningProfileShortAddrDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Short Address";

// Provisioning Profile PSKd Characteristic Properties
static uint8_t provisioningProfilePSKdCharProps = GATT_PROP_READ;

// PSKd Characteristic Value
static uint8_t provisioningProfilePSKdCharVal[PROVISIONING_PROV_PSKD_LEN] = "PPSSKK";

// Provisioning Profile PSKd Characteristic User Description
static uint8_t provisioningProfilePSKdDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "PSKd";

// Provisioning Profile Network Reset Characteristic Properties
static uint8_t provisioningProfileNtwkResetCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Network Reset Characteristic Value
static uint8_t provisioningProfileNtwkResetCharVal = 0x00;

// Provisioning Profile Network Reset Characteristic User Description
static uint8_t provisioningProfileNtwkResetDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Reset Network";
#endif
// Provisioning Profile Provision Sensor Characteristic Properties
static uint8_t provisioningProfileSensorCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Provision Sensor Characteristic Value
static uint8_t provisioningProfileSensorCharVal = 0x00;

// Provisioning Profile Provision Sensor Characteristic User Description
#if defined(DMM_ZEDSWITCH)
static uint8_t provisioningProfileSensorDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Provision ZED";
#elif defined(DMM_ZRLIGHT)
static uint8_t provisioningProfileSensorDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Provision ZR";
#elif defined(DMM_ZCSWITCH)
static uint8_t provisioningProfileSensorDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Provision ZC";
#elif defined(DMM_COLLECTOR)
static uint8_t provisioningProfileSensorDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Provision Collector";
#else
static uint8_t provisioningProfileSensorDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Connect Sensor";
#endif

// Provisioning Profile Sensor Provisioning State Characteristic Properties
static uint8_t provisioningProfileStateCharProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Sensor Provisioning State Characteristic Value
static uint8_t provisioningProfileStateCharVal = (uint8_t)Provisioning_states_initWaiting;

// Provisioning Profile Sensor Provisioning State Characteristic Configuration
static gattCharCfg_t *provisioningProfileStateDataConfig;


// Provisioning Profile Sensor Provisioning State Characteristic User Description
#if defined(DMM_ZEDSWITCH)
static uint8_t provisioningProfileStateDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "ZED Provisioning State";
#elif defined(DMM_ZRLIGHT)
static uint8_t provisioningProfileStateDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "ZR Provisioning State";
#elif defined(DMM_ZCSWITCH)
static uint8_t provisioningProfileStateDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "ZC Provisioning State";
#elif defined(DMM_COLLECTOR)
static uint8_t provisioningProfileStateDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Collector Provisioning State";
#else
static uint8_t provisioningProfileStateDataUserDesp[PROVISIONING_MAX_DESCRIPTION_LEN] = "Sensor Provisioning State";
#endif


/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t provisioningProfileAttrTbl[] =
{
  // Provisioning Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&provisioningProfileService      /* pValue */
  },
      // Network PAN ID Characteristic Declaration
      {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &provisioningProfileNtwkPanIdCharProps
      },
        // Network PAN ID Characteristic Value
        {
          { ATT_UUID_SIZE, provisioningProfileNtwkPanIdCharUUID },
          GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
          0,
          provisioningProfileNtwkPanIdCharVal
        },

        // Network PAN ID Characteristic User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          provisioningProfileNtwkPanIdDataUserDesp
        },
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
        // Extended Network PAN ID Characteristic Declaration
        {
          { ATT_BT_UUID_SIZE, characterUUID },
          GATT_PERMIT_READ,
          0,
          &provisioningProfileExtNtwkPanIdCharProps
        },
          // Extended Network PAN ID Characteristic Value
          {
            { ATT_UUID_SIZE, provisioningProfileExtNtwkPanIdCharUUID },
            GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
            0,
            provisioningProfileExtNtwkPanIdCharVal
          },

          // Extended Network PAN ID Characteristic User Description
          {
            { ATT_BT_UUID_SIZE, charUserDescUUID },
            GATT_PERMIT_READ,
            0,
            provisioningProfileExtNtwkPanIdDataUserDesp
          },

          // Sensor Frequency Characteristic Declaration
          {
            { ATT_BT_UUID_SIZE, characterUUID },
            GATT_PERMIT_READ,
            0,
            &provisioningProfileSensorFreqCharProps
          },
            // Sensor Frequency Characteristic Value
            {
              { ATT_UUID_SIZE, provisioningProfileSensorFreqCharUUID },
#ifdef USEOT
              GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
#else
              GATT_PERMIT_AUTHEN_READ,
#endif
              0,
              &provisioningProfileSensorFreqCharVal
            },
            // Sensor Frequency Characteristic User Description
            {
              { ATT_BT_UUID_SIZE, charUserDescUUID },
              GATT_PERMIT_READ,
              0,
              provisioningProfileSensorFreqDataUserDesp
            },
#endif
            // Sensor Channel Characteristic Declaration
            {
              { ATT_BT_UUID_SIZE, characterUUID },
              GATT_PERMIT_READ,
              0,
              &provisioningProfileSensorChannelCharProps
            },
              // Sensor Channel Characteristic Value
              {
                { ATT_UUID_SIZE, provisioningProfileSensorChannelCharUUID },
                GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
                0,
                provisioningProfileSensorChannelCharVal
              },

              // Sensor Channel Characteristic User Description
              {
                { ATT_BT_UUID_SIZE, charUserDescUUID },
                GATT_PERMIT_READ,
                0,
                provisioningProfileSensorChannelDataUserDesp
              },
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
              // IEEE FFD Address Characteristic Declaration
              {
                { ATT_BT_UUID_SIZE, characterUUID },
                GATT_PERMIT_READ,
                0,
                &provisioningProfileIeeeFfdAddrCharProps
              },
                // IEEE FFD Address Characteristic Value
                {
                  { ATT_UUID_SIZE, provisioningProfileIeeeFfdAddrCharUUID },
                  GATT_PERMIT_AUTHEN_READ,
                  0,
                  provisioningProfileIeeeFfdAddrCharVal
                },

                // IEEE FFD Address Characteristic User Description
                {
                  { ATT_BT_UUID_SIZE, charUserDescUUID },
                  GATT_PERMIT_READ,
                  0,
                  provisioningProfileIeeeFfdAddrDataUserDesp
                },

                // Network Security Key Characteristic Declaration
                {
                  { ATT_BT_UUID_SIZE, characterUUID },
                  GATT_PERMIT_READ,
                  0,
                  &provisioningProfileNtwkKeyCharProps
                },
                  // Network Security Key Characteristic Value
                  {
                    { ATT_UUID_SIZE, provisioningProfileNtwkKeyCharUUID },
                    GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
                    0,
                    provisioningProfileNtwkKeyCharVal
                  },

                  // Network Security Key Characteristic User Description
                  {
                    { ATT_BT_UUID_SIZE, charUserDescUUID },
                    GATT_PERMIT_READ,
                    0,
                    provisioningProfileNtwkKeyDataUserDesp
                  },
#endif
#ifdef USEOT
                  // PSKd Characteristic Declaration
                  {
                    { ATT_BT_UUID_SIZE, characterUUID },
                    GATT_PERMIT_READ,
                    0,
                    &provisioningProfilePSKdCharProps
                  },
                    // PSKd Characteristic Value
                    {
                      { ATT_UUID_SIZE, provisioningProfilePSKdCharUUID },
                      GATT_PERMIT_AUTHEN_READ,
                      0,
                      provisioningProfilePSKdCharVal
                    },

                    // PSKd Characteristic User Description
                    {
                      { ATT_BT_UUID_SIZE, charUserDescUUID },
                      GATT_PERMIT_READ,
                      0,
                      provisioningProfilePSKdDataUserDesp
                    },
                  // Network Name Characteristic Declaration
                  {
                    { ATT_BT_UUID_SIZE, characterUUID },
                    GATT_PERMIT_READ,
                    0,
                    &provisioningProfileNtwkNameCharProps
                  },
                    // Network Name Characteristic Value
                    {
                      { ATT_UUID_SIZE, provisioningProfileNtwkNameCharUUID },
                      GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
                      0,
                      provisioningProfileNtwkNameCharVal
                    },

                    // Network Name Characteristic User Description
                    {
                      { ATT_BT_UUID_SIZE, charUserDescUUID },
                      GATT_PERMIT_READ,
                      0,
                      provisioningProfileNtwkNameDataUserDesp
                    },

                    // Short Address Characteristic Declaration
                    {
                      { ATT_BT_UUID_SIZE, characterUUID },
                      GATT_PERMIT_READ,
                      0,
                      &provisioningProfileShortAddrCharProps
                    },
                      // Short Address Characteristic Value
                      {
                        { ATT_UUID_SIZE, provisioningProfileShortAddrCharUUID },
                        GATT_PERMIT_AUTHEN_READ,
                        0,
                        provisioningProfileShortAddrCharVal
                      },

                      // Short Address Characteristic User Description
                      {
                        { ATT_BT_UUID_SIZE, charUserDescUUID },
                        GATT_PERMIT_READ,
                        0,
                        provisioningProfileShortAddrDataUserDesp
                      },

                        // Network Reset Characteristic Declaration
                        {
                          { ATT_BT_UUID_SIZE, characterUUID },
                          GATT_PERMIT_READ,
                          0,
                          &provisioningProfileNtwkResetCharProps
                        },
                          // Network Reset Characteristic Value
                          {
                            { ATT_UUID_SIZE, provisioningProfileNtwkResetCharUUID },
                            GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
                            0,
                            &provisioningProfileNtwkResetCharVal
                          },

                          // Network Reset Characteristic User Description
                          {
                            { ATT_BT_UUID_SIZE, charUserDescUUID },
                            GATT_PERMIT_READ,
                            0,
                            provisioningProfileNtwkResetDataUserDesp
                          },
#endif
                  // Provision Sensor Characteristic Declaration
                  {
                    { ATT_BT_UUID_SIZE, characterUUID },
                    GATT_PERMIT_READ,
                    0,
                    &provisioningProfileSensorCharProps
                  },
                    // Provision Sensor Characteristic Value
                    {
                      { ATT_UUID_SIZE, provisioningProfileSensorCharUUID },
                      GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
                      0,
                      &provisioningProfileSensorCharVal
                    },

                    // Provision Sensor Characteristic User Description
                    {
                      { ATT_BT_UUID_SIZE, charUserDescUUID },
                      GATT_PERMIT_READ,
                      0,
                      provisioningProfileSensorDataUserDesp
                    },
                    // Sensor Provisioning State Characteristic Declaration
                    {
                      { ATT_BT_UUID_SIZE, characterUUID },
                      GATT_PERMIT_READ,
                      0,
                      &provisioningProfileStateCharProps
                    },
                      // Sensor Provisioning State Characteristic Value
                      {
                        { ATT_UUID_SIZE, provisioningProfileStateCharUUID },
                        GATT_PERMIT_AUTHEN_READ,
                        0,
                        &provisioningProfileStateCharVal
                      },

                      // Sensor Provisioning State Characteristic configuration
                      {
                        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
                        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
                        0,
                        (uint8 *)&provisioningProfileStateDataConfig
                      },

                      // Sensor Provisioning State Characteristic User Description
                      {
                        { ATT_BT_UUID_SIZE, charUserDescUUID },
                        GATT_PERMIT_READ,
                        0,
                        provisioningProfileStateDataUserDesp
                      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t ProvisioningProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr, 
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t ProvisioningProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Remote Display Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t provisioningProfileCBs =
{
  ProvisioningProfile_ReadAttrCB,  // Read callback function pointer
  ProvisioningProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Provisioning_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t ProvisioningProfile_AddService( uint32 services )
{
  uint8 status;
  // Allocate Client Characteristic Configuration tables
  provisioningProfileStateDataConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            MAX_NUM_BLE_CONNS );
  if ( provisioningProfileStateDataConfig == NULL )
  {     
    return ( bleMemAllocError );
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LL_CONNHANDLE_INVALID, provisioningProfileStateDataConfig );

  if ( services & PROVPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( provisioningProfileAttrTbl,
                                          GATT_NUM_ATTRS( provisioningProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &provisioningProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}

/*********************************************************************
 * @fn      Provisioning_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t ProvisioningProfile_RegisterAppCBs( provisioningProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    provisioningProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      ProvisioningProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ProvisioningProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case PROVPROFILE_NTWK_PAN_ID_CHAR:
      if ( len == PROVISIONING_NTWK_ID_LEN )
      {
          VOID memcpy(provisioningProfileNtwkPanIdCharVal, value, PROVISIONING_NTWK_ID_LEN);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    case PROVPROFILE_EXT_NTWK_PAN_ID_CHAR:
      if ( len == PROVISIONING_EXT_NTWK_ID_LEN )
      {
          VOID memcpy(provisioningProfileExtNtwkPanIdCharVal, value, PROVISIONING_EXT_NTWK_ID_LEN);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    case PROVPROFILE_SENSOR_FREQ_CHAR:
      if ( len == sizeof (uint8_t) )
      {
          provisioningProfileSensorFreqCharVal = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
#endif
    case PROVPROFILE_SENSOR_CHANNEL_CHAR:
      if ( len == PROVISIONING_NTWK_CHNL_LEN )
      {
          VOID memcpy(provisioningProfileSensorChannelCharVal, value, PROVISIONING_NTWK_CHNL_LEN);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    case PROVPROFILE_IEEE_FFD_ADDR_CHAR:
      if ( len == PROVISIONING_IEEE_FFD_ADDR_LEN )
      {
          VOID memcpy(provisioningProfileIeeeFfdAddrCharVal, value, PROVISIONING_IEEE_FFD_ADDR_LEN);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    case PROVPROFILE_NTWK_KEY_CHAR:
      if ( len == PROVISIONING_NTWK_KEY_LEN )
      {
          VOID memcpy(provisioningProfileNtwkKeyCharVal, value, PROVISIONING_NTWK_KEY_LEN);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
#endif
#ifdef USEOT
    case PROVPROFILE_PROV_PSKD_CHAR:
        VOID memcpy(provisioningProfilePSKdCharVal, value, PROVISIONING_PROV_PSKD_LEN);
        break;
    case PROVPROFILE_NTWK_NAME_CHAR:
        VOID memcpy(provisioningProfileNtwkNameCharVal, value, PROVISIONING_NTWK_NAME_LEN);
        break;
    case PROVPROFILE_NTWK_SHORT_ADDR_CHAR:
        VOID memcpy(provisioningProfileShortAddrCharVal, value, PROVISIONING_SHORT_ADDR_LEN);
        break;
    case PROVPROFILE_NTWK_RESET_CHAR:
        provisioningProfileNtwkResetCharVal = *((uint8*)value);
        break;
#endif
    case PROVPROFILE_PROV_SENSOR_CHAR:
      if ( len == sizeof (uint8_t) )
      {
          provisioningProfileSensorCharVal = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    case PROVPROFILE_PROV_STATE_CHAR:
      if ( len == sizeof (uint8_t) )
      {
          provisioningProfileStateCharVal = *((uint8*)value);

          // See if Notification has been enabled
          GATTServApp_ProcessCharCfg( provisioningProfileStateDataConfig, &provisioningProfileStateCharVal, FALSE,
                                      provisioningProfileAttrTbl, GATT_NUM_ATTRS( provisioningProfileAttrTbl ),
                                      INVALID_TASK_ID, ProvisioningProfile_ReadAttrCB );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      ProvisioningProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ProvisioningProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case PROVPROFILE_NTWK_PAN_ID_CHAR:
        VOID memcpy(value, provisioningProfileNtwkPanIdCharVal, PROVISIONING_NTWK_ID_LEN);
        break;
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    case PROVPROFILE_EXT_NTWK_PAN_ID_CHAR:
        VOID memcpy(value, provisioningProfileExtNtwkPanIdCharVal, PROVISIONING_EXT_NTWK_ID_LEN);
        break;
    case PROVPROFILE_SENSOR_FREQ_CHAR:
        *((uint8*)value) = provisioningProfileSensorFreqCharVal;
        break;
#endif
    case PROVPROFILE_SENSOR_CHANNEL_CHAR:
        VOID memcpy(value, provisioningProfileSensorChannelCharVal, PROVISIONING_NTWK_CHNL_LEN);
        break;
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    case PROVPROFILE_IEEE_FFD_ADDR_CHAR:
        VOID memcpy(value, provisioningProfileIeeeFfdAddrCharVal, PROVISIONING_IEEE_FFD_ADDR_LEN);
        break;
    case PROVPROFILE_NTWK_KEY_CHAR:
        VOID memcpy(value, provisioningProfileNtwkKeyCharVal, PROVISIONING_NTWK_KEY_LEN);
        break;
#endif
#ifdef USEOT
    case PROVPROFILE_PROV_PSKD_CHAR:
        VOID memcpy(value, provisioningProfilePSKdCharVal, PROVISIONING_PROV_PSKD_LEN);
        break;
    case PROVPROFILE_NTWK_NAME_CHAR:
        VOID memcpy(value, provisioningProfileNtwkNameCharVal, PROVISIONING_NTWK_NAME_LEN);
        break;
    case PROVPROFILE_NTWK_SHORT_ADDR_CHAR:
        VOID memcpy(value, provisioningProfileShortAddrCharVal, PROVISIONING_SHORT_ADDR_LEN);
        break;
    case PROVPROFILE_NTWK_RESET_CHAR:
        *((uint8*)value) = provisioningProfileNtwkResetCharVal;
        break;
#endif
    case PROVPROFILE_PROV_SENSOR_CHAR:
        *((uint8*)value) = provisioningProfileSensorCharVal;
        break;
    case PROVPROFILE_PROV_STATE_CHAR:
        *((uint8*)value) = provisioningProfileStateCharVal;
        break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          ProvisioningProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t ProvisioningProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  bStatus_t status = SUCCESS;
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
    if(!memcmp(pAttr->type.uuid, provisioningProfileSensorCharUUID, pAttr->type.len) ||
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
       !memcmp(pAttr->type.uuid, provisioningProfileSensorFreqCharUUID, pAttr->type.len) ||
#endif
       !memcmp(pAttr->type.uuid, provisioningProfileStateCharUUID, pAttr->type.len) )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads
      *pLen = PROVISIONING_GENERIC_LEN;
      *pValue = *pAttr->pValue;
    }
    else if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkPanIdCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_NTWK_ID_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_NTWK_ID_LEN );
    }
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    else if(!memcmp(pAttr->type.uuid, provisioningProfileExtNtwkPanIdCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_EXT_NTWK_ID_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_EXT_NTWK_ID_LEN );
    }
#endif

    else if(!memcmp(pAttr->type.uuid, provisioningProfileSensorChannelCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_NTWK_CHNL_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_NTWK_CHNL_LEN );
    }
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    else if(!memcmp(pAttr->type.uuid, provisioningProfileIeeeFfdAddrCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_IEEE_FFD_ADDR_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_IEEE_FFD_ADDR_LEN );
    }

    else if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkKeyCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_NTWK_KEY_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_NTWK_KEY_LEN );
    }
#endif
#ifdef USEOT
    else if(!memcmp(pAttr->type.uuid, provisioningProfilePSKdCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_PROV_PSKD_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_PROV_PSKD_LEN );
    }

    else if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkNameCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_NTWK_NAME_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_NTWK_NAME_LEN );
    }
    else if(!memcmp(pAttr->type.uuid, provisioningProfileShortAddrCharUUID, pAttr->type.len))
    {
      *pLen = PROVISIONING_SHORT_ADDR_LEN;
      VOID memcpy( pValue, pAttr->pValue, PROVISIONING_SHORT_ADDR_LEN );
    }
    else if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkResetCharUUID, pAttr->type.len))
    {
        *pLen = PROVISIONING_GENERIC_LEN;
        *pValue = *pAttr->pValue;
    }
#endif
    else
    {
      // Should never get here! (other characteristics do not have read permissions)
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }
  return ( status );
}

/*********************************************************************
 * @fn      ProvisioningProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t ProvisioningProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{

  bStatus_t status = SUCCESS;

  uint8 notifyApp = 0xFF;


  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
#ifdef USEOT
      if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkResetCharUUID, pAttr->type.len))
      {
        if ( offset == 0 )
        {
          if ( len != 1)
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }

        // Write the value
        if ( status == SUCCESS )
        {
            uint8 *pCurValue = (uint8 *)pAttr->pValue;
            *pCurValue = pValue[0];
            notifyApp = PROVPROFILE_NTWK_RESET_CHAR;
        }
      }
      else if(!memcmp(pAttr->type.uuid, provisioningProfileSensorCharUUID, pAttr->type.len))
      {
        // Verify that this is not a blob operation
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }

        // Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;
          *pCurValue = pValue[0];

          notifyApp = PROVPROFILE_PROV_SENSOR_CHAR;
        }
      }
      else if(provisionAttributeLock){
          return status;
      }//Beyond this point it's been confirmed that the provisionlock is disabled
      else
#endif
          if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkPanIdCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != PROVISIONING_NTWK_ID_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, PROVISIONING_NTWK_ID_LEN );

        notifyApp = PROVPROFILE_NTWK_PAN_ID_CHAR;
      }
    }
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    else if(!memcmp(pAttr->type.uuid, provisioningProfileSensorFreqCharUUID, pAttr->type.len))
    {
      // Verify that this is not a blob operation
      if ( offset == 0 )
      {
        if ( len != 1 )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        *pCurValue = pValue[0];

        notifyApp = PROVPROFILE_SENSOR_FREQ_CHAR;
      }
    }
#endif
#ifndef USEOT
    else if(!memcmp(pAttr->type.uuid, provisioningProfileSensorCharUUID, pAttr->type.len))
    {
      // Verify that this is not a blob operation
      if ( offset == 0 )
      {
        if ( len != 1 )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        *pCurValue = pValue[0];

        notifyApp = PROVPROFILE_PROV_SENSOR_CHAR;
      }
    }
#endif
#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    else if(!memcmp(pAttr->type.uuid, provisioningProfileExtNtwkPanIdCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != PROVISIONING_EXT_NTWK_ID_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, PROVISIONING_EXT_NTWK_ID_LEN );

        notifyApp = PROVPROFILE_EXT_NTWK_PAN_ID_CHAR;
      }
    }
#endif
    else if(!memcmp(pAttr->type.uuid, provisioningProfileSensorChannelCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != PROVISIONING_NTWK_CHNL_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, PROVISIONING_NTWK_CHNL_LEN );

        notifyApp = PROVPROFILE_SENSOR_CHANNEL_CHAR;
      }
    }

#if !defined(DMM_ZEDSWITCH) && !defined(DMM_ZRLIGHT) && !defined(DMM_ZCSWITCH)
    else if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkKeyCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != PROVISIONING_NTWK_KEY_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, PROVISIONING_NTWK_KEY_LEN );

        notifyApp = PROVPROFILE_NTWK_KEY_CHAR;
      }
    }
#endif
#ifdef USEOT
    else if(!memcmp(pAttr->type.uuid, provisioningProfileNtwkNameCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len > PROVISIONING_NTWK_NAME_LEN -1 )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, PROVISIONING_NTWK_NAME_LEN-1 );
        //pAttr->pValue[len] = '\0';
        int i;
        for(i = len; i< PROVISIONING_NTWK_NAME_LEN; i++)
            pAttr->pValue[i] = '\0';
        notifyApp = PROVPROFILE_NTWK_NAME_CHAR;
      }
    }
#endif
    else
    {
      status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else if (( pAttr->type.len == ATT_BT_UUID_SIZE ) &&
          (BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]) == GATT_CLIENT_CHAR_CFG_UUID))
  {
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY );
  }
  else
  {
    // unsupported length
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && provisioningProfile_AppCBs->pfnRemoteDisplayProfileChange )
  {
    provisioningProfile_AppCBs->pfnRemoteDisplayProfileChange( notifyApp );
  }


  return ( status );
}

#ifdef USEOT
void ProvisioningProfile_SetProvisionLock(bool state){
    provisionAttributeLock = state;
}
#endif
/*********************************************************************
*********************************************************************/
