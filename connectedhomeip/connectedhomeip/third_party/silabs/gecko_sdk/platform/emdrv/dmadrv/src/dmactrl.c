/***************************************************************************//**
 * @file
 * @brief DMA control data block.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_device.h"
#include "sl_common.h"
#include "dmactrl.h"

#if !defined(SL_DMADRV_USE_CUSTOM_DMA_CONTROL_BLOCK)

/***************************************************************************//**
 * @addtogroup dmadrv
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#if ( (DMA_CHAN_COUNT > 0) && (DMA_CHAN_COUNT <= 4) )
#define DMACTRL_CH_CNT      4
#define DMACTRL_ALIGNMENT   128

#elif ( (DMA_CHAN_COUNT > 4) && (DMA_CHAN_COUNT <= 8) )
#define DMACTRL_CH_CNT      8
#define DMACTRL_ALIGNMENT   256

#elif ( (DMA_CHAN_COUNT > 8) && (DMA_CHAN_COUNT <= 16) )
#define DMACTRL_CH_CNT      16
#define DMACTRL_ALIGNMENT   256

#elif ( (DMA_CHAN_COUNT > 16) && (DMA_CHAN_COUNT <= 32) )
#define DMACTRL_CH_CNT      32
#define DMACTRL_ALIGNMENT   512

#else
#error "Unsupported DMA channel count"
#endif

SL_ALIGN(DMACTRL_ALIGNMENT)
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMACTRL_CH_CNT * 2] SL_ATTRIBUTE_ALIGN(DMACTRL_ALIGNMENT);

/// @endcond
/** @} (end group dmadrv) */

#endif
