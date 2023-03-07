/******************************************************************************

 @file  gatt_uuid.c

 @brief This file contains Generic Attribute Profile (GATT)
        UUID types.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "comdef.h"
#include "osal.h"

#include "gatt.h"
#include "gatt_uuid.h"

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

/**
 * GATT Services
 */
// Generic Access Profile Service UUID
CONST uint8 gapServiceUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GAP_SERVICE_UUID ), HI_UINT16( GAP_SERVICE_UUID )
};

// Generic Attribute Profile Service UUID
CONST uint8 gattServiceUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_SERVICE_UUID ), HI_UINT16( GATT_SERVICE_UUID )
};

/**
 * GATT Declarations
 */
// Primary Service UUID
CONST uint8 primaryServiceUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_PRIMARY_SERVICE_UUID ), HI_UINT16( GATT_PRIMARY_SERVICE_UUID )
};

// Secondary Service UUID
CONST uint8 secondaryServiceUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_SECONDARY_SERVICE_UUID ), HI_UINT16( GATT_SECONDARY_SERVICE_UUID )
};

// Include UUID
CONST uint8 includeUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_INCLUDE_UUID ), HI_UINT16( GATT_INCLUDE_UUID )
};

// Characteristic UUID
CONST uint8 characterUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_CHARACTER_UUID ), HI_UINT16( GATT_CHARACTER_UUID )
};

/**
 * GATT Descriptors
 */
// Characteristic Extended Properties UUID
CONST uint8 charExtPropsUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_CHAR_EXT_PROPS_UUID ), HI_UINT16( GATT_CHAR_EXT_PROPS_UUID )
};

// Characteristic User Description UUID
CONST uint8 charUserDescUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_CHAR_USER_DESC_UUID ), HI_UINT16( GATT_CHAR_USER_DESC_UUID )
};

// Client Characteristic Configuration UUID
CONST uint8 clientCharCfgUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_CLIENT_CHAR_CFG_UUID ), HI_UINT16( GATT_CLIENT_CHAR_CFG_UUID )
};

// Server Characteristic Configuration UUID
CONST uint8 servCharCfgUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_SERV_CHAR_CFG_UUID ), HI_UINT16( GATT_SERV_CHAR_CFG_UUID )
};

// Characteristic Presentation Format UUID
CONST uint8 charFormatUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_CHAR_FORMAT_UUID ), HI_UINT16( GATT_CHAR_FORMAT_UUID )
};

// Characteristic Aggregate Format UUID
CONST uint8 charAggFormatUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_CHAR_AGG_FORMAT_UUID ), HI_UINT16( GATT_CHAR_AGG_FORMAT_UUID )
};

/**
 * GATT Characteristics
 */
// Device Name UUID
CONST uint8 deviceNameUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( DEVICE_NAME_UUID ), HI_UINT16( DEVICE_NAME_UUID )
};

// Appearance UUID
CONST uint8 appearanceUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( APPEARANCE_UUID ), HI_UINT16( APPEARANCE_UUID )
};

// Reconnection Address UUID
CONST uint8 reconnectAddrUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( RECONNECT_ADDR_UUID ), HI_UINT16( RECONNECT_ADDR_UUID )
};

// Peripheral Preferred Connection Parameters UUID
CONST uint8 periConnParamUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( PERI_CONN_PARAM_UUID ), HI_UINT16( PERI_CONN_PARAM_UUID )
};

// Central Address Resolution UUID
CONST uint8 centAddrResUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( CENTRAL_ADDRESS_RESOLUTION_UUID ), HI_UINT16( CENTRAL_ADDRESS_RESOLUTION_UUID )
};

// Resolvable Private Address Only UUID
CONST uint8 resPrivAddrOnlyUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( RESOLVABLE_PRIVATE_ADDRESS_ONLY_UUID ), HI_UINT16( RESOLVABLE_PRIVATE_ADDRESS_ONLY_UUID )
};

// Service Changed UUID
CONST uint8 serviceChangedUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( SERVICE_CHANGED_UUID ), HI_UINT16( SERVICE_CHANGED_UUID )
};

// Valid Range UUID
CONST uint8 validRangeUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_VALID_RANGE_UUID ), HI_UINT16( GATT_VALID_RANGE_UUID )
};

// External Report Reference Descriptor
CONST uint8 extReportRefUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_EXT_REPORT_REF_UUID ), HI_UINT16( GATT_EXT_REPORT_REF_UUID )
};

// Report Reference characteristic descriptor
CONST uint8 reportRefUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16( GATT_REPORT_REF_UUID ), HI_UINT16( GATT_REPORT_REF_UUID )
};

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      GATT_FindUUIDRec
 *
 * @brief   Find the UUID record for a given UUID.
 *
 * @param   pUUID - UUID to look for.
 * @param   len - length of UUID.
 *
 * @return  Pointer to UUID record. NULL, otherwise.
 */
const uint8 *GATT_FindUUIDRec( const uint8 *pUUID, uint8 len )
{
  const uint8 *pRec = NULL;

  if ( len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pUUID[0], pUUID[1] );
    switch ( uuid )
    {
      /*** GATT Services ***/

      case GAP_SERVICE_UUID:
        pRec = gapServiceUUID;
        break;

      case GATT_SERVICE_UUID:
        pRec = gattServiceUUID;
        break;

      /*** GATT Declarations ***/

      case GATT_PRIMARY_SERVICE_UUID:
        pRec = primaryServiceUUID;
        break;

      case GATT_SECONDARY_SERVICE_UUID:
        pRec = secondaryServiceUUID;
        break;

      case GATT_INCLUDE_UUID:
        pRec = includeUUID;
        break;

      case GATT_CHARACTER_UUID:
        pRec = characterUUID;
        break;

      /*** GATT Descriptors ***/

      case GATT_CHAR_EXT_PROPS_UUID:
        pRec = charExtPropsUUID;
        break;

      case GATT_CHAR_USER_DESC_UUID:
        pRec = charUserDescUUID;
        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        pRec = clientCharCfgUUID;
        break;

      case GATT_SERV_CHAR_CFG_UUID:
        pRec = servCharCfgUUID;
        break;

      case GATT_CHAR_FORMAT_UUID:
        pRec = charFormatUUID;
        break;

      case GATT_CHAR_AGG_FORMAT_UUID:
        pRec = charAggFormatUUID;
        break;

      case GATT_VALID_RANGE_UUID:
        pRec = validRangeUUID;
        break;

      case GATT_EXT_REPORT_REF_UUID:
        pRec = extReportRefUUID;
        break;

      case GATT_REPORT_REF_UUID:
        pRec = reportRefUUID;
        break;

      /*** GATT Characteristics ***/

      case DEVICE_NAME_UUID:
        pRec = deviceNameUUID;
        break;

      case APPEARANCE_UUID:
        pRec = appearanceUUID;
        break;

      case RECONNECT_ADDR_UUID:
        pRec = reconnectAddrUUID;
        break;

      case PERI_CONN_PARAM_UUID:
        pRec = periConnParamUUID;
        break;

      case SERVICE_CHANGED_UUID:
        pRec = serviceChangedUUID;
        break;

      /*** GATT Units ***/

      default:
        break;
    }
  }
  else if ( len == ATT_UUID_SIZE )
  {
    // 128-bit UUID
  }

  return ( pRec );
}

/****************************************************************************
****************************************************************************/
