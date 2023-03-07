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
#if defined(EFR32MG24A010F1024IM40)
#include "efr32mg24a010f1024im40.h"

#elif defined(EFR32MG24A010F1024IM48)
#include "efr32mg24a010f1024im48.h"

#elif defined(EFR32MG24A010F1536GM40)
#include "efr32mg24a010f1536gm40.h"

#elif defined(EFR32MG24A010F1536GM48)
#include "efr32mg24a010f1536gm48.h"

#elif defined(EFR32MG24A010F1536IM40)
#include "efr32mg24a010f1536im40.h"

#elif defined(EFR32MG24A010F1536IM48)
#include "efr32mg24a010f1536im48.h"

#elif defined(EFR32MG24A020F1024IM40)
#include "efr32mg24a020f1024im40.h"

#elif defined(EFR32MG24A020F1024IM48)
#include "efr32mg24a020f1024im48.h"

#elif defined(EFR32MG24A020F1536GM40)
#include "efr32mg24a020f1536gm40.h"

#elif defined(EFR32MG24A020F1536GM48)
#include "efr32mg24a020f1536gm48.h"

#elif defined(EFR32MG24A020F1536IM40)
#include "efr32mg24a020f1536im40.h"

#elif defined(EFR32MG24A020F1536IM48)
#include "efr32mg24a020f1536im48.h"

#elif defined(EFR32MG24A021F1024IM40)
#include "efr32mg24a021f1024im40.h"

#elif defined(EFR32MG24A110F1024IM48)
#include "efr32mg24a110f1024im48.h"

#elif defined(EFR32MG24A110F1536GM48)
#include "efr32mg24a110f1536gm48.h"

#elif defined(EFR32MG24A111F1536GM48)
#include "efr32mg24a111f1536gm48.h"

#elif defined(EFR32MG24A120F1536GM48)
#include "efr32mg24a120f1536gm48.h"

#elif defined(EFR32MG24A121F1536GM48)
#include "efr32mg24a121f1536gm48.h"

#elif defined(EFR32MG24A410F1536IM40)
#include "efr32mg24a410f1536im40.h"

#elif defined(EFR32MG24A410F1536IM48)
#include "efr32mg24a410f1536im48.h"

#elif defined(EFR32MG24A420F1536IM40)
#include "efr32mg24a420f1536im40.h"

#elif defined(EFR32MG24A420F1536IM48)
#include "efr32mg24a420f1536im48.h"

#elif defined(EFR32MG24A610F1536IM40)
#include "efr32mg24a610f1536im40.h"

#elif defined(EFR32MG24A620F1536IM40)
#include "efr32mg24a620f1536im40.h"

#elif defined(EFR32MG24B010F1024IM48)
#include "efr32mg24b010f1024im48.h"

#elif defined(EFR32MG24B010F1536IM40)
#include "efr32mg24b010f1536im40.h"

#elif defined(EFR32MG24B010F1536IM48)
#include "efr32mg24b010f1536im48.h"

#elif defined(EFR32MG24B020F1024IM48)
#include "efr32mg24b020f1024im48.h"

#elif defined(EFR32MG24B020F1536IM40)
#include "efr32mg24b020f1536im40.h"

#elif defined(EFR32MG24B020F1536IM48)
#include "efr32mg24b020f1536im48.h"

#elif defined(EFR32MG24B110F1536GM48)
#include "efr32mg24b110f1536gm48.h"

#elif defined(EFR32MG24B110F1536IM48)
#include "efr32mg24b110f1536im48.h"

#elif defined(EFR32MG24B120F1536IM48)
#include "efr32mg24b120f1536im48.h"

#elif defined(EFR32MG24B210F1536IM48)
#include "efr32mg24b210f1536im48.h"

#elif defined(EFR32MG24B220F1536IM48)
#include "efr32mg24b220f1536im48.h"

#elif defined(EFR32MG24B310F1536IM48)
#include "efr32mg24b310f1536im48.h"

#elif defined(EFR32MG24B610F1536IM40)
#include "efr32mg24b610f1536im40.h"

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
