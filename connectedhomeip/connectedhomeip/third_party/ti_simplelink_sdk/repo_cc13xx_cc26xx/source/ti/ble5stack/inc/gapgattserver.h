/******************************************************************************

 @file  gapgattserver.h

 @brief This file contains GAP GATT attribute definitions and prototypes
        prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

/**
 *  @defgroup GAPGATTSERVER GAP GATT Server
 *  @brief This module implements the GAP Attribute Server
 *  @{
 *  @file  gapgattserver.h
 *     GAP Attribute Server interface
 */


#ifndef GAPGATTSERVER_H
#define GAPGATTSERVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * CONSTANTS
 */

///  Length of Device Name Attribute value excluding null-terminate char
#define GAP_DEVICE_NAME_LEN                     21

/// Central Address Resolution: Privacy is Disabled
#define GAP_PRIVACY_DISABLED                    0x00
/// Central Address Resolution: Privacy is Enabled
#define GAP_PRIVACY_ENABLED                     0x01

/// Only Resolvable Private Addresses Shall Be Used
#define GAP_RPA_ONLY_MODE                       0x00

/**
 * @defgroup GGS_Params GAP GATT Server Parameters
 *
 * These parameters are used with @ref GGS_SetParameter, @ref GGS_GetParameter,
 * and @ref ggsAttrValueChange_t
 * @{
 */
/**
 * Device Name attribute
 *
 * properties: RW
 *
 * size: uint8_t[GAP_DEVICE_NAME_LEN]
 */
#define GGS_DEVICE_NAME_ATT                     0
/**
 * Appearance attribute
 *
 * properties: RW
 *
 * size: uint16_t
 */
#define GGS_APPEARANCE_ATT                      1
/**
 * Peripheral Preferred Connection Parameters attribute
 *
 * properties: RW
 *
 * size: sizeof(@ref gapPeriConnectParams_t)
 */
#define GGS_PERI_CONN_PARAM_ATT                 4
/// @cond NODOC
#if defined ( TESTMODES )
#define GGS_W_PERMIT_DEVICE_NAME_ATT            6   //!< W   uint8
#define GGS_W_PERMIT_APPEARANCE_ATT             7   //!< W   uint8
#endif // TESTMODES
/// @endcond // NODOC
/**
 * Central Address Resolution Attribute
 *
 * This will always be set to @ref GAP_PRIVACY_ENABLED since privacy 1.2.1
 * is always supported.
 *
 * properties: R
 *
 * size: uint8_t
 */
#define GGS_CENT_ADDR_RES_ATT                   9
/**
 * Resolvable Address Only Attribute
 *
 * This will always exist and be set to @ref GAP_RPA_ONLY_MODE since privacy
 * 1.2.1 is always supported.
 *
 * properties: R
 *
 * size: uint8_t
 */
#define GGS_RESOLVABLE_PRIVATE_ADDRESS_ONLY_ATT 10
/** @} End GGS_Params */

/// @cond NODOC
// GAP Services bit fields
#define GAP_SERVICE                             0x00000001

// GGS TestModes
#if defined ( TESTMODES )
/// Don't use any test modes
#define GGS_TESTMODE_OFF                      0
///  Make Device Name attribute writable
#define GGS_TESTMODE_W_PERMIT_DEVICE_NAME     1
/// Make Appearance attribute writable
#define GGS_TESTMODE_W_PERMIT_APPEARANCE      2
/**
 * Toggle the CAR char value between privacy enabled (1) and disabled (0). The
 * initial value is enabled (1).
 */
#define GGS_TESTMODE_TOGGLE_CAR_VALUE         3
#endif  // TESTMODES
/// @endcond // NODOC

/*********************************************************************
 * TYPEDEFS
 */

/// Callback to notify when attribute value is changed over the air.
typedef void (*ggsAttrValueChange_t)
(
  uint16 connHandle,    //!< connection handle
  uint8 attrId          //!< attribute ID
);

// GAP GATT Server callback structure
typedef struct
{
  ggsAttrValueChange_t  pfnAttrValueChange;  // When attribute value is changed OTA
} ggsAppCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/**
 * Set a GAP GATT Server parameter.
 *
 * @param param Profile parameter ID
 * @param len length of data to write
 * @param value pointer to data to write. This is dependent on  the parameter
 *        ID and will be cast to the appropriate data type (example: data type
 *        of uint16_t will be cast to uint16 pointer)
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_SetParameter( uint8 param, uint8 len, void *value );

/**
 * Get a GAP GATT Server parameter.
 *
 * @param param Profile parameter ID
 * @param value pointer to data to use. This is dependent on  the parameter
 *        ID and will be cast to the appropriate data type (example: data type
 *        of uint16_t will be cast to uint16_t pointer)
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_GetParameter( uint8 param, void *value );

/**
 * Add function for the GAP GATT Service.
 *
 * @param services services to add. This is a bit map and can contain more than
 *        one service.
 *
 * @return @ref SUCCESS : Service added successfully.
 * @return @ref INVALIDPARAMETER : Invalid service field.
 * @return @ref FAILURE : Not enough attribute handles available.
 * @return @ref bleMemAllocError : Memory allocation error occurred.
 */
extern bStatus_t GGS_AddService( uint32 services );

/**
 * Delete function for the GAP GATT Service.
 *
 * @param services services to delete. This is a bit map and can contain more
 * than one service.
 *
 * @return @ref SUCCESS : Service deleted successfully.
 * @return @ref FAILURE : Service not found.
 */
extern bStatus_t GGS_DelService( uint32 services );

/**
 * Registers the application callback function.
 *
 * @note Callback registration is needed only when the Device Name is made
 * writable. The application will be notified when the Device Name is changed
 * over the air.
 *
 * @param appCallbacks pointer to application callbacks.
 */
extern void GGS_RegisterAppCBs( ggsAppCBs_t *appCallbacks );

/**
 * Set a GGS Parameter value.
 *
 * Use this function to change the default GGS parameter values.
 *
 * @param value new GGS param value
 */
extern void GGS_SetParamValue( uint16 value );

/**
 * Get a GGS Parameter value.
 *
 * @return GGS Parameter value
 */
extern uint16 GGS_GetParamValue( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GAPGATTSERVER_H */

/** @} End GAPGATTSERVER */
