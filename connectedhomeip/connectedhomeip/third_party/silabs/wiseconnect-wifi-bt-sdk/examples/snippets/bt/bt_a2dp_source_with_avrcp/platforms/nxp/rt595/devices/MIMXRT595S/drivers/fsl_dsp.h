/*
 * Copyright 2018-2019, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DSP_H_
#define _FSL_DSP_H_

#include <stdint.h>
#include "fsl_device_registers.h"

/*!
 * @addtogroup dsp
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief dsp driver version 2.0.0. */
#define FSL_DSP_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*!
 * @brief Fusion DSP vector table select
 */
typedef enum _dsp_static_vec_sel
{
    kDSP_StatVecSelPrimary   = 0U, /* Primary static vector base 0x00000000 */
    kDSP_StatVecSelAlternate = 1U, /* Alternate static vector base 0x00400000 */
} dsp_static_vec_sel_t;

/*!
 * @brief Structure for DSP copy image to destination address
 *
 * Defines start and destination address for copying image with given size.
 */
typedef struct _dsp_copy_image
{
    uint32_t *srcAddr;
    uint32_t *destAddr;
    uint32_t size;
} dsp_copy_image_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializing DSP core.
 *
 * Power up DSP
 * Enable DSP clock
 * Reset DSP peripheral
 */
void DSP_Init(void);

/*!
 * @brief Set Fusion DSP static vector table remap.
 *
 * @param statVecSel static vector base address selection
 * @param remap static vector remap, valid value from 0x0U to 0xFFFU
 */
static inline void DSP_SetVecRemap(dsp_static_vec_sel_t statVecSel, uint32_t remap)
{
    SYSCTL0->DSP_VECT_REMAP =
        SYSCTL0_DSP_VECT_REMAP_STATVECSELECT(statVecSel) | SYSCTL0_DSP_VECT_REMAP_DSP_VECT_REMAP(remap);
}

/*!
 * @brief Copy DSP image to destination address.
 *
 * Copy DSP image from source address to destination address with given size.
 *
 * @param dspCopyImage Structure contains information for DSP copy image to destination address.
 */
void DSP_CopyImage(dsp_copy_image_t *dspCopyImage);

/*!
 * @brief Deinitializing DSP core.
 */
void DSP_Deinit(void);

/*!
 * @brief Start DSP core.
 */
static inline void DSP_Start(void)
{
    SYSCTL0->DSPSTALL = 0x0;
}

/*!
 * @brief Stop DSP core.
 */
static inline void DSP_Stop(void)
{
    SYSCTL0->DSPSTALL = 0x1;
}

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_RESET_H_ */
