/***************************************************************************//**
 * @file
 * @brief GPIOINT API definition
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

#ifndef GPIOINTERRUPT_H
#define GPIOINTERRUPT_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup gpioint
 * @{
 ******************************************************************************/
/*******************************************************************************
 *****************************   DEFINITIONS   *********************************
 ******************************************************************************/

#define INTERRUPT_UNAVAILABLE                         (0xFF)

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/

/**
 * @brief
 *  GPIO interrupt callback function pointer.
 * @details
 *   Parameters:
 *   @li intNo - The pin interrupt number the callback function is invoked for.
 */
typedef void (*GPIOINT_IrqCallbackPtr_t)(uint8_t intNo);

/**
 * @brief
 *  Extended GPIO interrupt callback function pointer.
 * @details
 *   Parameters:
 *   @li intNo - The pin interrupt number the callback function is invoked for.
 *   @li ctx   - Pointer to callback context.
 */
typedef void (*GPIOINT_IrqCallbackPtrExt_t)(uint8_t intNo, void *ctx);

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/
void GPIOINT_Init(void);
void GPIOINT_CallbackRegister(uint8_t intNo, GPIOINT_IrqCallbackPtr_t callbackPtr);
unsigned int GPIOINT_CallbackRegisterExt(uint8_t pin, GPIOINT_IrqCallbackPtrExt_t callbackPtr, void *callbackCtx);
__STATIC_INLINE void GPIOINT_CallbackUnRegister(uint8_t intNo);

/***************************************************************************//**
 * @brief
 *   Unregister user callback for a given pin interrupt number.
 *
 * @details
 *   Use this function to unregister a callback.
 *
 * @param[in] intNo
 *   Pin interrupt number for the callback.
 *
 ******************************************************************************/
__STATIC_INLINE void GPIOINT_CallbackUnRegister(uint8_t intNo)
{
  GPIOINT_CallbackRegister(intNo, 0);
}

/** @} (end addtogroup gpioint) */
#ifdef __cplusplus
}
#endif

#endif /* GPIOINTERRUPT_H */
