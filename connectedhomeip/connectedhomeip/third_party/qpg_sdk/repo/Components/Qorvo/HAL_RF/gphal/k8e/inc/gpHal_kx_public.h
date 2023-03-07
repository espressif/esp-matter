/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPHAL_KX_PUBLIC_H_
#define _GPHAL_KX_PUBLIC_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_RomCode_kx_public.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include "gpBsp.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// HP masks used for interrupt sources handled directly in the ISR
// LP masks used for interrupt handling done in the main loop/task
// Checks in place in various functions if correct handling is implemented

// RCI interrupt
#ifdef GP_COMP_GPHAL_BLE
// BLE specific sources
#define GPHAL_ISR_RCI_HP_ISR_MASK_BLE (GP_WB_INT_CTRL_MASK_RCI_BLE_CONN_REQ_IND_INTERRUPT_MASK)
#define GPHAL_ISR_RCI_HP_ISR_UNMASKED_BLE (GP_WB_RCI_UNMASKED_BLE_CONN_REQ_IND_INTERRUPT_MASK)
#define GPHAL_ISR_RCI_LP_ISR_MASK_BLE (GP_WB_INT_CTRL_MASK_RCI_BLE_DATA_IND_INTERRUPT_MASK | \
                                       GP_WB_INT_CTRL_MASK_RCI_BLE_ADV_IND_INTERRUPT_MASK  | \
                                       GP_WB_INT_CTRL_MASK_RCI_BLE_DATA_CNF_INTERRUPT_MASK)
#define GPHAL_ISR_RCI_LP_ISR_UNMASKED_BLE (GP_WB_RCI_UNMASKED_BLE_DATA_CNF_INTERRUPT_MASK | \
                                           GP_WB_RCI_UNMASKED_BLE_DATA_IND_INTERRUPT_MASK | \
                                           GP_WB_RCI_UNMASKED_BLE_ADV_IND_INTERRUPT_MASK)
#else
//No BLE used
#define GPHAL_ISR_RCI_HP_ISR_MASK_BLE (0x0)
#define GPHAL_ISR_RCI_HP_ISR_UNMASKED_BLE (0x0)
#define GPHAL_ISR_RCI_LP_ISR_MASK_BLE (0x0)
#define GPHAL_ISR_RCI_LP_ISR_UNMASKED_BLE (0x0)
#endif //GP_COMP_GPHAL_BLE

#ifdef GP_COMP_GPHAL_MAC
// MAC 802.15.4 sources
#define GPHAL_ISR_RCI_HP_ISR_MASK_MAC (0x0)
#define GPHAL_ISR_RCI_HP_ISR_UNMASKED_MAC (0x0)
#define GPHAL_ISR_RCI_LP_ISR_MASK_MAC (GP_WB_INT_CTRL_MASK_RCI_DATA_IND_INTERRUPT_MASK     | \
                                       GP_WB_INT_CTRL_MASK_RCI_DATA_CNF_0_INTERRUPT_MASK   | \
                                       GP_WB_INT_CTRL_MASK_RCI_DATA_CNF_1_INTERRUPT_MASK)
#define GPHAL_ISR_RCI_LP_ISR_UNMASKED_MAC (GP_WB_RCI_UNMASKED_DATA_CNF_0_INTERRUPT_MASK | \
                                           GP_WB_RCI_UNMASKED_DATA_CNF_1_INTERRUPT_MASK | \
                                           GP_WB_RCI_UNMASKED_DATA_IND_INTERRUPT_MASK)
#else
//No MAC 802.15.4 used
#define GPHAL_ISR_RCI_HP_ISR_MASK_MAC (0x0)
#define GPHAL_ISR_RCI_HP_ISR_UNMASKED_MAC (0x0)
#define GPHAL_ISR_RCI_LP_ISR_MASK_MAC (0x0)
#define GPHAL_ISR_RCI_LP_ISR_UNMASKED_MAC (0x0)
#endif //GP_COMP_GPHAL_MAC

// Full masks for RCI handling
#define GPHAL_ISR_RCI_HP_ISR_MASK     (GPHAL_ISR_RCI_HP_ISR_MASK_MAC | GPHAL_ISR_RCI_HP_ISR_MASK_BLE)
#define GPHAL_ISR_RCI_HP_ISR_UNMASKED (GPHAL_ISR_RCI_HP_ISR_UNMASKED_MAC | GPHAL_ISR_RCI_HP_ISR_UNMASKED_BLE)
#define GPHAL_ISR_RCI_LP_ISR_MASK     (GPHAL_ISR_RCI_LP_ISR_MASK_MAC | GPHAL_ISR_RCI_LP_ISR_MASK_BLE)
#define GPHAL_ISR_RCI_LP_ISR_UNMASKED (GPHAL_ISR_RCI_LP_ISR_UNMASKED_MAC | GPHAL_ISR_RCI_LP_ISR_UNMASKED_BLE)

// IPC interrupt
#ifdef GP_COMP_GPHAL_BLE
#define GPHAL_ISR_IPC_GPM_HP_ISR_MASK     (GP_WB_INT_CTRL_MASK_IPCGPM2X_CONN_REQ_TX_INTERRUPT_MASK)
#define GPHAL_ISR_IPC_GPM_HP_ISR_UNMASKED (GP_WB_IPC_UNMASKED_GPM2X_CONN_REQ_TX_INTERRUPT_MASK)
#define GPHAL_ISR_IPC_GPM_LP_ISR_MASK (GP_WB_INT_CTRL_MASK_IPCGPM2X_EVENT_PROCESSED_INTERRUPT_MASK      | \
                                       GP_WB_INT_CTRL_MASK_IPCGPM2X_LAST_CONN_EVENT_CNT_INTERRUPT_MASK  | \
                                       GP_WB_INT_CTRL_MASK_IPCGPM2X_SCAN_REQ_RX_INTERRUPT_MASK          | \
                                       GP_WB_INT_CTRL_MASK_IPCGPM2X_STAT_INTERRUPT_MASK                 | \
                                       GP_WB_INT_CTRL_MASK_IPCGPM2X_UNEXPECTED_COND_INTERRUPT_MASK)
#define GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED (GP_WB_IPC_UNMASKED_GPM2X_EVENT_PROCESSED_INTERRUPTS_MASK      | \
                                           GP_WB_IPC_UNMASKED_GPM2X_LAST_CONN_EVENT_CNT_INTERRUPTS_MASK  | \
                                           GP_WB_IPC_UNMASKED_GPM2X_SCAN_REQ_RX_INTERRUPT_MASK          | \
                                           GP_WB_IPC_UNMASKED_GPM2X_STAT_INTERRUPT_MASK                 | \
                                           GP_WB_IPC_UNMASKED_GPM2X_TX_POWER_MEAS_DONE_INTERRUPT_MASK   | \
                                           GP_WB_IPC_UNMASKED_GPM2X_UNEXPECTED_COND_INTERRUPT_MASK)
#else
#define GPHAL_ISR_IPC_GPM_HP_ISR_MASK (0x0)
#define GPHAL_ISR_IPC_GPM_HP_ISR_UNMASKED (0x0)
#define GPHAL_ISR_IPC_GPM_LP_ISR_MASK (0x0)
#define GPHAL_ISR_IPC_GPM_LP_ISR_UNMASKED (0x0)
#endif

#define GP_HAL_ENABLE_INTERRUPTS(enable)                                \
    do {                                                                \
        UInt64 intMask  = GP_WB_INT_CTRL_MASK_INT_ES_INTERRUPT_MASK     \
                        | GP_WB_INT_CTRL_MASK_INT_RCI_INTERRUPT_MASK    \
                        | GP_WB_INT_CTRL_MASK_INT_PHY_INTERRUPT_MASK    \
                        | GP_WB_INT_CTRL_MASK_INT_IPCGPM2X_INTERRUPT_MASK\
                        | GP_WB_INT_CTRL_MASK_INT_STBC_INTERRUPT_MASK;  \
        UInt32 irqMask  = (1 << ES_IRQn)                                \
                        | (1 << RCI_IRQn)                               \
                        | (1 << PHY_IRQn)                               \
                        | (1 << IPCGPM2X_IRQn)                          \
                        | (1 << STBC_IRQn);                             \
        if (enable) {                                                   \
            GP_WB_WRITE_INT_CTRL_MASK_INT_INTERRUPTS(GP_WB_READ_INT_CTRL_MASK_INT_INTERRUPTS() | intMask);  \
            NVIC->ISER[0] |= irqMask;                                   \
        } else {                                                        \
            GP_WB_WRITE_INT_CTRL_MASK_INT_INTERRUPTS(GP_WB_READ_INT_CTRL_MASK_INT_INTERRUPTS() & ~intMask); \
            NVIC->ISER[0] &= ~irqMask;                                  \
        }                                                               \
    } while (false)

#define GP_HAL_ENABLE_PIO_INT(enable) /*PIO = RCI */     \
    do {                                                       \
          GP_WB_WRITE_INT_CTRL_MASK_RCI_DATA_IND_INTERRUPT(enable);                \
          GP_WB_WRITE_INT_CTRL_MASK_RCI_DATA_CNF_0_INTERRUPT(enable); /*Data Cfm*/ \
          GP_WB_WRITE_INT_CTRL_MASK_RCI_DATA_CNF_1_INTERRUPT(enable); /*ED scan*/  \
    } while (false)

#define GP_HAL_ENABLE_EMPTY_QUEUE_CALLBACK_INTERRUPT(enable) GP_WB_WRITE_INT_CTRL_MASK_QTA_SCH0_EMPTY_INTERRUPT(enable)
#define GP_HAL_ENABLE_BUSY_TX_CALLBACK_INTERRUPT(enable)     /*No longer available ?*/

#define GP_HAL_ENABLE_EXTERNAL_EVENT_INTERRUPT(enable)       GP_WB_WRITE_INT_CTRL_MASK_ES_EXTERNAL_EVENT_INTERRUPT(enable)

//gpHal_MAC.h specific macro's

#define GP_HAL_GET_ADDRESS_RECOGNITION()          (GP_WB_READ_MACFILT_DST_PAN_ID_CHECK_ON() || GP_WB_READ_MACFILT_DST_ADDR_CHECK_ON())
//Note - bitmap given as 'filter when set' - actual register 'accepts when set' - inverted here
#define GP_HAL_SET_FRAME_TYPE_FILTER_MASK(bitmap) GP_HAL_WRITE_REG(GP_WB_MACFILT_ACCEPT_FT_BCN_ADDRESS, (UInt8)(~(bitmap)))
#define GP_HAL_GET_FRAME_TYPE_FILTER_MASK()       ((UInt8)(~(GP_HAL_READ_REG(GP_WB_MACFILT_ACCEPT_FT_BCN_ADDRESS))))
#define GP_HAL_GET_RX_ON_WHEN_IDLE()              (!!(GP_WB_READ_RIB_RX_ON_WHEN_IDLE_CH()))

#define GP_HAL_SET_PIP_MODE(pipmode)              GP_WB_WRITE_RX_EN_PIP(pipmode)
#define GP_HAL_GET_PIP_MODE()                     GP_WB_READ_RX_EN_PIP()

//Mac state containing timed, calibration and receiver status
#define GP_HAL_MAC_STATE_INIT()                 { gpHal_MacState = 0; }
#define GP_HAL_MAC_STATE_SET_TIMED()            BIT_SET(gpHal_MacState, 0)
#define GP_HAL_MAC_STATE_CLR_TIMED()            BIT_CLR(gpHal_MacState, 0)
#define GP_HAL_MAC_STATE_GET_TIMED()            BIT_TST(gpHal_MacState, 0)

#define GP_HAL_MAC_STATE_SET_CALIBRATED()       BIT_SET(gpHal_MacState, 1)
#define GP_HAL_MAC_STATE_GET_CALIBRATED()       BIT_TST(gpHal_MacState, 1)

#define GP_HAL_MAC_STATE_SET_RXON()             BIT_SET(gpHal_MacState, 2)
#define GP_HAL_MAC_STATE_CLR_RXON()             BIT_CLR(gpHal_MacState, 2)
#define GP_HAL_MAC_STATE_GET_RXON()             BIT_TST(gpHal_MacState, 2)

#define GP_HAL_MAC_STATE_SET_PROMISCUOUS_MODE() BIT_SET(gpHal_MacState, 3)
#define GP_HAL_MAC_STATE_CLR_PROMISCUOUS_MODE() BIT_CLR(gpHal_MacState, 3)
#define GP_HAL_MAC_STATE_GET_PROMISCUOUS_MODE() BIT_TST(gpHal_MacState, 3)

#define GP_HAL_MAC_STATE_SET_ACKPBM_INITIALIZED() BIT_SET(gpHal_MacState, 7)
#define GP_HAL_MAC_STATE_GET_ACKPBM_INITIALIZED() BIT_TST(gpHal_MacState, 7)

// Number of SRCIDS supported by this hal
#define GP_HAL_MAC_NUMBER_OF_RX_SRCIDS       3

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

Bool gpHal_wb_memcmp(const UInt8 *ptr0, UIntPtr addr, UInt8 length);


/** @brief Disables the HW RX MAC.
 *
 *  This functions enables the re-use of PBMs for other purposes.
 *  The use case for this is IR TX.
*/
GP_API UInt16* gpHal_ClaimPbmMemory(void);
/** @brief Enables the HW RX MAC.
*/
GP_API Bool gpHal_ReleasePbmMemory(void);

// Default, minimum and maximum Tx powers for this module
#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define GPHAL_MIN_TRANSMIT_POWER        gpHalPhy_GetMinTransmitPower()
#else  //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define GPHAL_MIN_TRANSMIT_POWER        (-24)
#endif //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define GPHAL_MAX_TRANSMIT_POWER        gpHalPhy_GetMaxTransmitPower()
#else  //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#if defined(GP_HAL_EXPECTED_CHIP_EMULATED)
#define GPHAL_MAX_TRANSMIT_POWER        (-3)
#else //defined(GP_HAL_EXPECTED_CHIP_EMULATED)

#if defined(GP_BSP_ANTENNATUNECONFIG_10DBM_SINGLE_ENDED) || defined(GP_BSP_ANTENNATUNECONFIG_10DBM_DIFFERENTIAL)
#define GPHAL_MAX_TRANSMIT_POWER        10
#else
#define GPHAL_MAX_TRANSMIT_POWER        7
#endif

#endif //defined(GP_HAL_EXPECTED_CHIP_EMULATED)
#endif //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#define GPHAL_DEFAULT_TRANSMIT_POWER    GPHAL_MAX_TRANSMIT_POWER

#define HAL_SLEEP_CB_BEFORESLEEP()
#define HAL_SLEEP_CB_AFTERWAKEUP()

void gpHal_InitRandom(void);

#if !defined(GPHAL_MIN_CHIPVERSION)
#define GPHAL_MIN_CHIPVERSION (0)
#endif //#if !defined(GPHAL_MIN_CHIPVERSION)

/** @brief Indicates if chip is emulated
*/
#ifdef GP_HAL_EXPECTED_CHIP_EMULATED
#define gpHal_IsEmulated()     true
#else
#define gpHal_IsEmulated()     false
#endif

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif  /* _GPHAL_KX_PUBLIC_H_ */
