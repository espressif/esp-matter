/******************************************************************************

 @file  gap.c

 @brief This file contains the GAP Configuration API.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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

#include "bcomdef.h"
#include "osal_snv.h"
#include "gap.h"
#include "sm.h"
#ifdef GAP_BOND_MGR
#include "gapbondmgr_internal.h"
#include "gapbondmgr.h"
#endif //GAP_BOND_MGR
#include "gap_internal.h"
#include "gap_advertiser_internal.h"
#include "gap_scanner_internal.h"

#include "rom_jt.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES - These are available as part of the internal API
 *                (gap_internal.h), not part of the public API (gap.h)
 */
uint8 gapAppTaskID    = INVALID_TASK_ID; // default task ID to send events.
uint8 gapEndAppTaskID = INVALID_TASK_ID; // end application task ID to send events.
                                         // if bond manager is not enabled,
                                         // gapEndAppTaskID and gapAppTaskID
                                         // will be the same.

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
void GAP_DeviceInit_per_role(uint8_t profileRole);

/*********************************************************************
 * API FUNCTIONS
 * OneBleLib: Duplicate GAP_DeviceInit without init gap bond manager - save flash size when not using GapBondMgr
 * OneBleLib comopiled as PERIPHERAL and CENTRAL:  
 *         to privent for example init scan module in BROADCASTER
 */

/*********************************************************************
 * Validate correct role, address modes and init state
 * common function for both GAP_DeviceInit and GAP_DeviceInit_noGAPBondMgr
 */
bStatus_t GAP_DeviceInit_validate_params(uint8_t profileRole, uint8_t taskID,
                         GAP_Addr_Modes_t addrMode, uint8_t* pRandomAddr)
{
  bStatus_t stat = INVALIDPARAMETER;   // Return status

  // Ensure that initialization hasn't occurred
  if (MAP_gapGetState() == GAP_INITSTATE_READY)
  {
    return bleIncorrectMode;
  }

  // Check for valid address mode...
  if (
      (addrMode > ADDRMODE_RP_WITH_RANDOM_ID) ||
      // And if a random mode, that an address is passed in
      ((addrMode == ADDRMODE_RANDOM
        || addrMode == ADDRMODE_RP_WITH_RANDOM_ID
       ) && (pRandomAddr == NULL)))
  {
    return ( INVALIDPARAMETER );
  }

  // If using a random address, check that it is valid:
  if((addrMode == ADDRMODE_RANDOM
       || addrMode == ADDRMODE_RP_WITH_RANDOM_ID
      ) &&
     // If all bits excluding the 2 MSBs are all 0's...
     ((MAP_osal_isbufset(pRandomAddr, 0x00, B_ADDR_LEN - 1) &&
      ((pRandomAddr[B_ADDR_LEN - 1] & 0x3F) == 0)) ||
      // Or all bites are 1's
      MAP_osal_isbufset(pRandomAddr, 0xFF, B_ADDR_LEN) ||
      // Or the 2 MSBs are not 11b
      !(GAP_IS_ADDR_RS(pRandomAddr))))
  {
    // This is an invalid ramdom static address
    return( INVALIDPARAMETER );
  }

  // Valid profile roles and supported combinations
  switch ( profileRole )
  {
    case GAP_PROFILE_BROADCASTER:
      #if ( HOST_CONFIG & ( BROADCASTER_CFG | PERIPHERAL_CFG ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case GAP_PROFILE_OBSERVER:
      #if ( HOST_CONFIG & ( OBSERVER_CFG | CENTRAL_CFG ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case GAP_PROFILE_PERIPHERAL:
      #if ( HOST_CONFIG & PERIPHERAL_CFG )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case GAP_PROFILE_CENTRAL:
      #if ( HOST_CONFIG & CENTRAL_CFG )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_BROADCASTER | GAP_PROFILE_OBSERVER):
      #if ( ( HOST_CONFIG & ( BROADCASTER_CFG | PERIPHERAL_CFG ) ) && \
            ( HOST_CONFIG & ( OBSERVER_CFG | CENTRAL_CFG ) ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_PERIPHERAL | GAP_PROFILE_OBSERVER):
      #if ( ( HOST_CONFIG & PERIPHERAL_CFG ) && \
            ( HOST_CONFIG & ( OBSERVER_CFG | CENTRAL_CFG ) ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_CENTRAL | GAP_PROFILE_BROADCASTER):
      #if ( ( HOST_CONFIG & CENTRAL_CFG ) && \
            ( HOST_CONFIG & ( BROADCASTER_CFG | PERIPHERAL_CFG ) ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_CENTRAL | GAP_PROFILE_PERIPHERAL):
      #if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    // Invalid profile roles
    default:
      stat = INVALIDPARAMETER;
      break;
  }
  return stat;
}

/*********************************************************************
 * Called to setup the device.  Call just once.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_DeviceInit(uint8_t profileRole, uint8_t taskID,
                         GAP_Addr_Modes_t addrMode, uint8_t* pRandomAddr)
{
  bStatus_t stat = INVALIDPARAMETER;   // Return status

  // Validate correct role, address modes and init state 
  stat = GAP_DeviceInit_validate_params(profileRole, taskID, addrMode, pRandomAddr);
  
  if (stat == SUCCESS)
  {
#ifdef GAP_BOND_MGR
    bool eraseNV = FALSE; // Flag to erase NV if needed

    // Read last address mode from NV
    // This will not be a success on the first run since the address mode hasn't
    // been written yet
    GAP_Addr_Modes_t lastAddrMode;
    bStatus_t readStatus = osal_snv_read(BLE_NVID_ADDR_MODE, sizeof(uint8_t),
                                         &lastAddrMode);

    // If the address mode read from NV is valid and different than the last
    // used address mode...
    if((readStatus == SUCCESS) && (lastAddrMode != 0xFF) &&
       (lastAddrMode != addrMode))
    {
      eraseNV = TRUE;
    }
    // If the address mode is the same and is a random mode...
    else if((readStatus == SUCCESS) && (lastAddrMode == addrMode) &&
            (addrMode == ADDRMODE_RANDOM
             || addrMode == ADDRMODE_RP_WITH_RANDOM_ID
            ))
    {
      // Read the last used random address
      uint8 lastRandAddr[B_ADDR_LEN] = {0};
      readStatus = osal_snv_read(BLE_NVID_RANDOM_ADDR, B_ADDR_LEN,
                                 lastRandAddr);

      // If this is different than the address passed in now
      if((readStatus == SUCCESS) &&
         (MAP_osal_memcmp(lastRandAddr, pRandomAddr, B_ADDR_LEN) == FALSE))
      {
        eraseNV = TRUE;
      }
    }

    // Erase NV if found to be needed above
    if (eraseNV == TRUE)
    {
      // Erase all bonds and local information
      stat = GAPBondMgr_SetParameter(GAPBOND_ERASE_ALLBONDS, 0, NULL);
      stat |= GAPBondMgr_SetParameter(GAPBOND_ERASE_LOCAL_INFO, 0, NULL);

      if(stat != SUCCESS)
      {
        return (bleInternalError);
      }
    }
#endif // GAP_BOND_MGR

    // Set the internal GAP address mode
    gapDeviceAddrMode = addrMode;

    // If own address type is random static or RPA with random static...
    if(gapDeviceAddrMode == ADDRMODE_RANDOM
       || gapDeviceAddrMode == ADDRMODE_RP_WITH_RANDOM_ID
      )
    {
      // If valid random static address, put it to the controller
      MAP_LL_SetRandomAddress(pRandomAddr);
    }

    // Set the task ID to receive GAP events.
#ifdef GAP_BOND_MGR
    gapEndAppTaskID = taskID;

    // If using the gapbondmgr, use its taskID as default application task ID
    gapAppTaskID = (uint8_t)MAP_GAP_GetParamValue(GAP_PARAM_AUTH_TASK_ID);

    // If there is no gapbondmgr
    if(gapAppTaskID == 0)
    {
      // If bond manager is included in the build but not used,
      // use end application's task ID as the default appliation task ID
      gapAppTaskID = gapEndAppTaskID;
    }
#else // ! GAP_BOND_MGR
    gapEndAppTaskID = gapAppTaskID = taskID;
#endif // GAP_BOND_MGR

    // Setup the device configuration parameters
    stat = MAP_gap_ParamsInit( profileRole );
    if ( stat == SUCCESS )
    {
      uint8_t tempKey[KEYLEN] = {0};

      // IRK is set based on the following rules:
      // 1. If the own address type is public or random static,
      //    set IRK to all 0's.
      // 2. If the own address type is neither public nor random static,
      //    2-1. If the IRK was set by user with GapConfig_SetParameter() before
      //         GAP_DeviceInit(), use it.
      //    2-2. If the IRK hasn't been set by user, read it from the NV.
      //         2-2-1. If the value is valid, use it.
      //         2-2-2. If not, generate a random value.
      //
      // SRK is set based on the following rules:
      // 1. If the SRK was set by user with GapConfig_SetParameter() before
      //    GAP_DeviceInit(), use it.
      // 2. If the SRK hasn't been set by user, read it from the NV.
      //    2-1. If the value is valid, use it.
      //    2-2. If not, generate a random value.

      if (gapDeviceAddrMode == ADDRMODE_PUBLIC ||
          gapDeviceAddrMode == ADDRMODE_RANDOM)
      {
        // If we are going to use only either public or random static address
        // as the own address, the IRK shall be all 0's.
        MAP_GapConfig_SetParameter(GAP_CONFIG_PARAM_IRK, tempKey);
      }
#ifdef GAP_BOND_MGR
      else
      {
        if ( MAP_osal_isbufset( MAP_GAP_GetIRK(), 0xFF, KEYLEN ) == TRUE )
        {
          // If application didn't set the IRK (i.e. it is all 0xFF's)
          // Check for an IRK stored in NV

          // Read IRK from NV
          // This will not be a success on the first run since the IRK hasn't
          // been written yet
          bStatus_t readStatus = osal_snv_read(BLE_NVID_IRK, KEYLEN, tempKey);

          // If the IRK is found in NV
          if ( readStatus == SUCCESS )
          {
            //Use as IRK
            MAP_GapConfig_SetParameter(GAP_CONFIG_PARAM_IRK, tempKey);
          }
        }
      }

      if ( MAP_osal_isbufset( MAP_gapGetSRK(), 0x00, KEYLEN ) == TRUE )
      {
        // If application didn't set the SRK
        // Check for a valid SRK stored in NV

        // Read SRK from NV
        // This will not be a success on the first run since the SRK hasn't
        // been written yet
        bStatus_t readStatus = osal_snv_read(BLE_NVID_CSRK, KEYLEN, tempKey);

        // If the SRK read from NV is valid
        if ( ( readStatus == SUCCESS ) &&
             ( MAP_osal_isbufset( tempKey, 0xFF, KEYLEN ) == FALSE ) )
        {
          //Use as SRK
          MAP_GapConfig_SetParameter(GAP_CONFIG_PARAM_SRK, tempKey);
        }
      }
#endif // GAP_BOND_MGR

      // Init GAP security, privacy, advertising and scan per role
      GAP_DeviceInit_per_role(profileRole);
    }
  }

  return ( stat );
}

/*********************************************************************
 * Init GAP security, privacy, advertising and scan per role
 * common function for both GAP_DeviceInit and GAP_DeviceInit_noGAPBondMgr
 */
void GAP_DeviceInit_per_role(uint8_t profileRole)
{
      if ((profileRole & GAP_PROFILE_CENTRAL) || (profileRole & GAP_PROFILE_PERIPHERAL))
      //#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
      {
        // The signCounter parameter is not needed anymore since it is handled
        // by the gapbondmgr. The API is not changed in order to prevent
        // patching ROM
        MAP_gap_SecParamsInit(MAP_gapGetSRK(), NULL);
      }
      //#endif

      // Set IRK GAP Parameter
      MAP_gap_PrivacyInit(MAP_GAP_GetIRK());

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
      if ((profileRole & GAP_PROFILE_CENTRAL) || (profileRole & GAP_PROFILE_OBSERVER))
      {
        // Initialize GAP Scanner module
        VOID MAP_gapScan_init();
#if ( HOST_CONFIG & CENTRAL_CFG )
        if (profileRole & GAP_PROFILE_CENTRAL)
        {
          // Register GAP Central Connection processing functions
          MAP_gap_CentConnRegister();

          // Initialize SM Initiator
          VOID MAP_SM_InitiatorInit();
        }
#endif
      }
#endif

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
      if ((profileRole & GAP_PROFILE_PERIPHERAL) || (profileRole & GAP_PROFILE_BROADCASTER))
      {
        // Initialize GAP Advertiser module
        VOID MAP_gapAdv_init();
#if ( HOST_CONFIG & PERIPHERAL_CFG )
        if (profileRole & GAP_PROFILE_PERIPHERAL)
        {
          // Register GAP Peripheral Connection processing functions
          MAP_gap_PeriConnRegister();

          // Initialize SM Responder
          VOID MAP_SM_ResponderInit();
        }
#endif
      }
#endif
}
/*********************************************************************
 * Public function defined in gap.h.
 */
void GAP_UpdateResolvingList(uint8_t *pIRK)
{
#if defined ( GAP_BOND_MGR )
  // Sync bond records with Resolving List. Bond Manager also manages local
  // IRK in Resolving List
  gapBondMgr_syncResolvingList();
#else
  // Pass local IRK to the controller
  HCI_LE_AddDeviceToResolvingListCmd( 0, 0, 0, pIRK );
#endif // GAP_BOND_MGR
}

/*********************************************************************
 * Called to setup the device.  Call just once. 
 * !!!!!!! Only when GAP_BOND_MGR is not defined !!!!!!!!!
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_DeviceInit_noGAPBondMgr(uint8_t profileRole, uint8_t taskID,
                         GAP_Addr_Modes_t addrMode, uint8_t* pRandomAddr)
{
  bStatus_t stat = INVALIDPARAMETER;   // Return status

  // Validate correct role, address modes and init state 
  stat = GAP_DeviceInit_validate_params(profileRole, taskID, addrMode, pRandomAddr);
  
  if (stat == SUCCESS)
  {
    // Set the internal GAP address mode
    gapDeviceAddrMode = addrMode;

    // If own address type is random static or RPA with random static...
    if(gapDeviceAddrMode == ADDRMODE_RANDOM
       || gapDeviceAddrMode == ADDRMODE_RP_WITH_RANDOM_ID
      )
    {
      // If valid random static address, put it to the controller
      MAP_LL_SetRandomAddress(pRandomAddr);
    }

    // Set the task ID to receive GAP events.
    gapEndAppTaskID = gapAppTaskID = taskID;

    // Setup the device configuration parameters
    stat = MAP_gap_ParamsInit( profileRole );
    if ( stat == SUCCESS )
    {
      uint8_t tempKey[KEYLEN] = {0};

      // IRK is set based on the following rules:
      // 1. If the own address type is public or random static,
      //    set IRK to all 0's.
      // 2. If the own address type is neither public nor random static,
      //    2-1. If the IRK was set by user with GapConfig_SetParameter() before
      //         GAP_DeviceInit(), use it.
      //    2-2. If the IRK hasn't been set by user, read it from the NV.
      //         2-2-1. If the value is valid, use it.
      //         2-2-2. If not, generate a random value.
      //
      // SRK is set based on the following rules:
      // 1. If the SRK was set by user with GapConfig_SetParameter() before
      //    GAP_DeviceInit(), use it.
      // 2. If the SRK hasn't been set by user, read it from the NV.
      //    2-1. If the value is valid, use it.
      //    2-2. If not, generate a random value.

      if (gapDeviceAddrMode == ADDRMODE_PUBLIC ||
          gapDeviceAddrMode == ADDRMODE_RANDOM)
      {
        // If we are going to use only either public or random static address
        // as the own address, the IRK shall be all 0's.
        MAP_GapConfig_SetParameter(GAP_CONFIG_PARAM_IRK, tempKey);
      }

      // Init GAP security, privacy, advertising and scan per role
      GAP_DeviceInit_per_role(profileRole);
    }
  }

  return ( stat );
}

/*********************************************************************
 * Public function defined in gap.h.
 * !!!!!!! Only when GAP_BOND_MGR is not defined !!!!!!!!!
 */
void GAP_UpdateResolvingList_noGAPBondMgr(uint8_t *pIRK)
{
  // Pass local IRK to the controller
  HCI_LE_AddDeviceToResolvingListCmd( 0, 0, 0, pIRK );
}

/*******************************************************************************
 * This API is used to disable the RF
 *
 * Defined in gap.h.
 */
void GAP_DeInit( void )
{
  // Close the RF
  MAP_LL_DeInit();
}

/*******************************************************************************
 * This API is used to re-enable the RF
 *
 * Defined in gap.h.
 */
void GAP_ReInit( void )
{
  // Open the RF
  MAP_LL_ReInit();
}

/*********************************************************************
*********************************************************************/
