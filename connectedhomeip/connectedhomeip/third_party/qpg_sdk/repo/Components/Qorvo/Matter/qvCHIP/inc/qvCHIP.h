/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Change$
 * $DateTime$
 */

/** @file "qvCHIP.h"
 *
 *  CHIP wrapper API
 *
 *  Declarations of the public functions and enumerations of qvCHIP.
*/

#ifndef _QVCHIP_H_
#define _QVCHIP_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @brief status return values */
typedef enum qvStatus_ {
    QV_STATUS_NO_ERROR = 0,
    QV_STATUS_BUFFER_TOO_SMALL = 1,
    QV_STATUS_INVALID_ARGUMENT = 2,
    QV_STATUS_KEY_LEN_TOO_SMALL = 3,
    QV_STATUS_INVALID_DATA = 4,
    QV_STATUS_NOT_IMPLEMENTED = 5,
    QV_STATUS_NVM_ERROR = 6,
    QV_STATUS_WRONG_STATE = 7
} qvStatus_t;

#define qvResetReason_UnSpecified                        0x00
#define qvResetReason_HW_BrownOutDetected                0x01
#define qvResetReason_HW_Watchdog                        0x02
#define qvResetReason_HW_Por                             0x03
#define qvResetReason_SW_Por                             0x04
typedef uint8_t                             qvResetReason_t;

#include "qvCHIP_NVM.h"
#include "qvCHIP_KVS.h"
#include "qvCHIP_Ble.h"
#include "qvCHIP_OTA.h"
#include "qvCHIP_factorydata.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    Generic API
 *****************************************************************************/
typedef void (*application_init_callback_t)(void);

/** @brief Initialize Qorvo needed components for CHIP.
*   @return result                   0 if init was successful. -1 when failed
*/
int qvCHIP_init(application_init_callback_t application_init_callback);

/** @brief Printf that outputs on Qorvo platforms.
*
*   @param module                    Module id for module producing the logging.
*   @param formattedMsg              Char buffer with formatted string message.
*/
void qvCHIP_Printf(uint8_t module, const char* formattedMsg);

/** @brief Return a sequence of random bytes.
*
*   @param outputLength              Length of the sequence to be obtained.
*   @param pOutput                   Buffer for the returned random sequence.
*   @return result                   QV_STATUS_NO_ERROR if successful or QV_STATUS_INVALID_ARGUMENT
*/
qvStatus_t qvCHIP_RandomGet(uint8_t outputLength, uint8_t *pOutput);

/** @brief Return a sequence of random bytes using HW entropy source.
*
*   @param outputLength              Length of the sequence to be obtained.
*   @param pOutput                   Buffer for the returned random sequence.
*   @return result                   QV_STATUS_NO_ERROR if successful or QV_STATUS_INVALID_ARGUMENT
*/
qvStatus_t qvCHIP_RandomGetDRBG(uint8_t outputLength, uint8_t *pOutput);

/** @brief Trigger system reset.
*/
void qvCHIP_ResetSystem(void);

/** @brief Return HEAP statistics.
*
*   @param pHeapFree                 HEAP currently free.
*   @param pHeapUsed                 HEAP currently in use.
*   @param pHighWatermark            Maximum HEAP used.
*   @return result                   False if any parameter is NULL. True if parameters are ok
*/
bool qvCHIP_GetHeapStats(size_t* pHeapFree, size_t* pHeapUsed, size_t* pHighWatermark);

/** @brief Reset HEAP statistics.
*/

void qvCHIP_ResetHeapStats(void);

/** @brief Get reset reason.
 *  @return result                   qvResetReason_t - reset reason (see top of the file)
*/
qvResetReason_t qvCHIP_GetResetReason(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_QVCHIP_H_
