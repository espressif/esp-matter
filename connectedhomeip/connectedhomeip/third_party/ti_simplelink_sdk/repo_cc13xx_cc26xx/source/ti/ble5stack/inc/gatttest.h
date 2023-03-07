/******************************************************************************

 @file  gatttest.h

 @brief This file contains the GATT Test Services definitions and prototypes
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

#ifndef GATTTEST_H
#define GATTTEST_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"

/*********************************************************************
 * CONSTANTS
 */
// Length of attribute
#define GATT_TEST_ATTR_LEN               20

// Length of long attribute
#define GATT_TEST_LONG_ATTR_LEN          50

// GATT Test Services bit fields
#define GATT_TEST_SERVICE                0x00000001 // GATT Test
#define GATT_BATT_STATE_SERVICE          0x00000002 // Battery State
#define GATT_THERMO_HUMID_SERVICE        0x00000004 // Thermometer Humidity
#define GATT_WEIGHT_SERVICE              0x00000008 // Weight
#define GATT_POSITION_SERVICE            0x00000010 // Position
#define GATT_ALERT_SERVICE               0x00000020 // Alert
#define GATT_MANUFACT_SENSOR_SERVICE     0x00000040 // Sensor Manufacturer
#define GATT_MANUFACT_SCALES_SERVICE     0x00000080 // Scales Manufacturer
#define GATT_ADDRESS_SERVICE             0x00000100 // Address
#define GATT_128BIT_UUID1_SERVICE        0x00000200 // 128-bit UUID 1
#define GATT_128BIT_UUID2_SERVICE        0x00000400 // 128-bit UUID 2
#define GATT_128BIT_UUID3_SERVICE        0x00000800 // 128-bit UUID 3

/*********************************************************************
 * VARIABLES
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief   Add function for the GATT Test Services.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.
 *          INVALIDPARAMETER: Invalid service field.
 *          FAILURE: Not enough attribute handles available.
 *          bleMemAllocError: Memory allocation error occurred.
 */
extern bStatus_t GATTTest_AddService( uint32 services );

/**
 * @brief   Delete function for the GATT Test Services.
 *
 * @param   services - services to delete. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service deleted successfully.
 *          FAILURE: Service not found.
 */
extern bStatus_t GATTTest_DelService( uint32 services );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by OSAL.
 */

/**
 * @internal
 *
 * @brief   Initialize the GATT Test Application.
 *
 * @param   taskId - Task identifier for the desired task
 *
 * @return  void
 *
 */
extern void GATTTest_Init( uint8 taskId );

/**
 * @internal
 *
 * @brief   GATT Test Application Task event processor. This function
 *          is called to process all events for the task. Events include
 *          timers, messages and any other user defined events.
 *
 * @param   task_id - The OSAL assigned task ID.
 * @param   events - events to process. This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
extern uint16 GATTTest_ProcessEvent( uint8 task_id, uint16 events );

/**
 * @brief   Add function for the GATT Qualification Services.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.
 *          INVALIDPARAMETER: Invalid service field.
 *          FAILURE: Not enough attribute handles available.
 *          bleMemAllocError: Memory allocation error occurred.
 */
extern bStatus_t GATTQual_AddService( uint32 services );

/**
 * @brief   Delete function for the GATT Qualification Services.
 *
 * @param   services - services to delete. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service deleted successfully.
 *          FAILURE: Service not found.
 */
extern bStatus_t GATTQual_DelService( uint32 services );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GATTTEST_H */
