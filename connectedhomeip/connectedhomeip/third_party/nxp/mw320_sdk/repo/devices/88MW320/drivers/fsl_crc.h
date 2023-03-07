/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CRC_H_
#define _FSL_CRC_H_

#include "fsl_common.h"

/*!
 * @addtogroup crc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_CRC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief  CRC mode type definition  */
typedef enum _crc_mode
{
    kCRC_16BitsCcitt  = 0U, /*!< CRC mode: CRC-16-CCITT */
    kCRC_16BitsIbm    = 1U, /*!< CRC mode: CRC-16-IBM */
    kCRC_16BitsT10Dif = 2U, /*!< CRC mode: CRC-16-T10-DIF */
    kCRC_32BitsIeee   = 3U, /*!< CRC mode: CRC-32-IEEE */
    kCRC_16BitsDnp    = 4U, /*!< CRC mode: CRC-16-DNP */
} crc_mode_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization Interfaces
 * @{
 */

/*!
 * @brief Initializes CRC module.
 *
 * Call this function to initialize the CRC module.
 *
 * @param base CRC peripheral base address
 */
void CRC_Init(CRC_Type *base);

/*!
 * @brief Disable the interrupt and clear all the interrupts
 *
 * @param base CRC peripheral base address
 */
void CRC_Deinit(CRC_Type *base);

/* @} */

/*!
 * @name CRC Operation Interfaces
 * @{
 */

/*!
 * @brief  Set CRC mode.
 *
 * @param base CRC peripheral base address.
 * @param crcMode The CRC work mode to set.
 */
static inline void CRC_SetMode(CRC_Type *base, crc_mode_t crcMode)
{
    /* set CRC mode */
    base->CTRL = CRC_CTRL_MODE(crcMode);
}

/*!
 * @brief Enable the CRC module.
 *
 * This function enable/disables the CRC.
 *
 * @param base CRC peripheral base address.
 * @param enable Use true to enable, false to disable.
 */
static inline void CRC_Enable(CRC_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= CRC_CTRL_ENABLE_MASK;
    }
    else
    {
        base->CTRL &= ~CRC_CTRL_ENABLE_MASK;
    }
}

/*!
 * @brief  Feed data in CRC stream.
 *
 * @param  base CRC peripheral address.
 * @param  data input data.
 */
static inline void CRC_FeedData(CRC_Type *base, uint32_t data)
{
    /* feed data in CRC stream */
    base->STREAM_IN = data;
}

/*!
 * @brief  Set CRC stream length.
 *
 * @param  base    CRC peripheral address.
 * @param  strLen  stream length in byte.
 */
static inline void CRC_SetStreamLen(CRC_Type *base, uint32_t strLen)
{
    /* set CRC stream length */
    base->STREAM_LEN_M1 = strLen - 1;
}

/*!
 * @brief  Get CRC result.
 *
 * @param  base  CRC peripheral base address.
 *
 * @return CRC calculation result.
 */
static inline uint32_t CRC_GetResult(CRC_Type *base)
{
    /* return CRC result value */
    return base->RESULT;
}

/*!
 * @brief Calculate the CRC value for input data block.
 *
 * @param  base      CRC peripheral base address.
 * @param  dataStr   input data stream.
 * @param  dataLen   data length in byte.
 *
 * @return CRC calculation result.
 */
uint32_t CRC_Calculate(CRC_Type *base, const uint8_t *dataStr, uint32_t dataLen);

/* @} */

/*!
 * @name Interrupt Control Interfaces
 * @{
 */

/*!
 * @brief   Enable the CRC Interrupt.
 *
 * @param  base  CRC peripheral base address.
 */
static inline void CRC_EnableInterrupt(CRC_Type *base)
{
    base->IMR |= CRC_IMR_MASK_MASK;
}

/*!
 * @brief   Disable the CRC Interrupt.
 *
 * @param  base  CRC peripheral base address.
 */
static inline void CRC_DisableInterrupt(CRC_Type *base)
{
    base->IMR &= ~CRC_IMR_MASK_MASK;
}

/*!
 * brief  Clears interrupt flags.
 *
 * @param  base   CRC peripheral base address.
 */
static inline void CRC_ClearInterruptStatus(CRC_Type *base)
{
    base->ICR = CRC_ICR_CLEAR_MASK;
}

/*!
 * @brief  Get base interrupt status.
 *
 * @param  base CRC peripheral base address.
 *
 * @return true if interrupt flag is asserted, else false.
 *
 */
static inline bool CRC_GetInterruptStatus(CRC_Type *base)
{
    return ((base->ISR & CRC_ISR_STATUS_MASK) != 0U);
}

/*!
 * @brief  Get CRC raw interrupt status.
 *
 * This function is similar with @ref CRC_GetInterruptStatus, the only
 * difference is, the raw interrupt status is not affected by interrupt
 * enable status.
 *
 * @param  base CRC peripheral base address.
 *
 * @return true if CRC status is asserted, else false.
 */
static inline bool CRC_GetRawInterruptStatus(CRC_Type *base)
{
    return ((base->IRSR & CRC_IRSR_STATUS_RAW_0_MASK) != 0U);
}

/*! @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_CRC_H_ */
