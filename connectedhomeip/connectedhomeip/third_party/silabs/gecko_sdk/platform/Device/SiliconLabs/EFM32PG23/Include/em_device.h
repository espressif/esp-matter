/**************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M Peripheral Access Layer for Silicon Laboratories
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * @verbatim
 * Example: Add "-DEFM32G890F128" to your build options, to define part
 *          Add "#include "em_device.h" to your source files

 *
 * @endverbatim
 ******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
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
 *****************************************************************************/

#ifndef EM_DEVICE_H
#define EM_DEVICE_H
#if defined(EFM32PG23B200F128IM40)
#include "efm32pg23b200f128im40.h"

#elif defined(EFM32PG23B200F256IM40)
#include "efm32pg23b200f256im40.h"

#elif defined(EFM32PG23B200F512IM40)
#include "efm32pg23b200f512im40.h"

#elif defined(EFM32PG23B200F64IM40)
#include "efm32pg23b200f64im40.h"

#elif defined(EFM32PG23B210F128IM48)
#include "efm32pg23b210f128im48.h"

#elif defined(EFM32PG23B210F256IM48)
#include "efm32pg23b210f256im48.h"

#elif defined(EFM32PG23B210F512IM48)
#include "efm32pg23b210f512im48.h"

#elif defined(EFM32PG23B210F64IM48)
#include "efm32pg23b210f64im48.h"

#elif defined(EFM32PG23B310F128IM48)
#include "efm32pg23b310f128im48.h"

#elif defined(EFM32PG23B310F256IM48)
#include "efm32pg23b310f256im48.h"

#elif defined(EFM32PG23B310F512IM48)
#include "efm32pg23b310f512im48.h"

#elif defined(EFM32PG23B310F64IM48)
#include "efm32pg23b310f64im48.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif

#if defined(SL_CATALOG_TRUSTZONE_SECURE_CONFIG_PRESENT) && defined(SL_TRUSTZONE_NONSECURE)
#error "Can't define SL_CATALOG_TRUSTZONE_SECURE_CONFIG_PRESENT and SL_TRUSTZONE_NONSECURE MACRO at the same time."
#endif

#if defined(SL_TRUSTZONE_SECURE) && defined(SL_TRUSTZONE_NONSECURE)
#error "Can't define SL_TRUSTZONE_SECURE and SL_TRUSTZONE_NONSECURE MACRO at the same time."
#endif
#endif /* EM_DEVICE_H */
