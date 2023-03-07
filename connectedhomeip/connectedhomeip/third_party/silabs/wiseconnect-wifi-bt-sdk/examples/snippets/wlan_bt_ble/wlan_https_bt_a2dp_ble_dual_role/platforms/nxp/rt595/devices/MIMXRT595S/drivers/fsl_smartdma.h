/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SMARTDMA_H_
#define _FSL_SMARTDMA_H_

#include "fsl_common.h"

/*!
 * @addtogroup smartdma
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief SMARTDMA driver version */
#define FSL_SMARTDMA_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */
/*@}*/

/*! @brief The firmware used for FlexIO MCULCD. */
extern const uint8_t s_smartdmaFlexioMcuLcdFirmware[];

/*! @brief The s_smartdmaFlexioMcuLcdFirmware firmware memory address. */
#define SMARTDMA_FLEXIO_MCULCD_MEM_ADDR 0x24100000

/*! @brief Size of s_smartdmaFlexioMcuLcdFirmware */
#define SMARTDMA_FLEXIO_MCULCD_FIRMWARE_SIZE 3564

/*!
 * @brief The API index when using s_smartdmaFlexioMcuLcdFirmware.
 */
enum _smartdma_flexio_mculcd_api
{
    kSMARTDMA_FlexIO_DMA_Endian_Swap,
    kSMARTDMA_FlexIO_DMA_Reverse32,
    kSMARTDMA_FlexIO_DMA,
    kSMARTDMA_LightOn,
    kSMARTDMA_LightOff,
    kSMARTDMA_Notify,
    kSMARTDMA_Test,
    kSMARTDMA_RGB565To888,
    kSMARTDMA_FlexIO_DMA_RGB565To888,
};

/*!
 * @brief Parameter for FlexIO MCULCD
 */
typedef struct _smartdma_flexio_mculcd_param
{
    uint32_t *p_buffer;
    uint32_t buffersize;
    uint32_t *smartdma_stack;
} smartdma_flexio_mculcd_param_t;

/*!
 * @brief Parameter for RGB565To888
 */
typedef struct _smartdma_rgb565_rgb888_param
{
    uint32_t *inBuf;
    uint32_t *outBuf;
    uint32_t buffersize;
    uint32_t *smartdma_stack;
} smartdma_rgb565_rgb888_param_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Initialize the SMARTDMA.
 *
 * @param apiMemAddr The address firmware will be copied to.
 * @param firmware The firmware to use.
 * @param firmwareSizeByte Size of firmware.
 */
void SMARTDMA_Init(uint32_t apiMemAddr, const void *firmware, uint32_t firmwareSizeByte);

/*!
 * @brief Boot the SMARTDMA to run program.
 *
 * @param apiIndex Index of the API to call.
 * @param pParam Pointer to the parameter.
 * @param mask Value set to SMARTDMA_ARM2SMARTDMA[0:1].
 */
void SMARTDMA_Boot(uint32_t apiIndex, void *pParam, uint8_t mask);

/*!
 * @brief Deinitialize the SMARTDMA.
 */
void SMARTDMA_Deinit(void);

/*!
 * @brief Reset the SMARTDMA.
 */
void SMARTDMA_Reset(void);

#if defined(__cplusplus)
}
#endif

/* @} */

#endif /* _FSL_SMARTDMA_H_ */
