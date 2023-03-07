/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/**************************************************************************//**
* This simulation device is only meant for internal use in the simulator.
*
* The simlator device is born out of existing code which used a reduced
* EM357 regs.h to model register behavior.
* Simple uint32_t arrays are used as the memory space to
* fake registers in the simulator.
*
* The only registers the simulator actually needs are the GPIO,
* serial controller, and serial controller's events.
*
* This file also takes the place of what would normally be defined as a
* distinct device.  But since such a small set of code
* is needed, all the code is placed in this file.
*
* This is a generated Device header using a subset of the same generation
* proccess that produces an EM357 Device.  The Simulation Device is closest
* to an EM357.
******************************************************************************/

#ifndef __EM_DEVICE_H
#define __EM_DEVICE_H

#define EXT_IRQ_COUNT 0

#include <stdint.h>

#define __INLINE        inline
#define __STATIC_INLINE static inline

typedef uint8_t IRQn_Type;

#endif
