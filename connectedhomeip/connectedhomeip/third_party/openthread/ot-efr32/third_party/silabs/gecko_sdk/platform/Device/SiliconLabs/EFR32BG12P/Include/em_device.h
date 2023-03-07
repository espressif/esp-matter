/***************************************************************************//**
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
 * @endverbatim
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EM_DEVICE_H
#define EM_DEVICE_H

#if defined(EFR32BG12P132F1024GL125)
#include "efr32bg12p132f1024gl125.h"

#elif defined(EFR32BG12P132F1024GM48)
#include "efr32bg12p132f1024gm48.h"

#elif defined(EFR32BG12P232F1024GL125)
#include "efr32bg12p232f1024gl125.h"

#elif defined(EFR32BG12P232F1024GM48)
#include "efr32bg12p232f1024gm48.h"

#elif defined(EFR32BG12P232F1024GM68)
#include "efr32bg12p232f1024gm68.h"

#elif defined(EFR32BG12P232F1024IM68)
#include "efr32bg12p232f1024im68.h"

#elif defined(EFR32BG12P232F512GM68)
#include "efr32bg12p232f512gm68.h"

#elif defined(EFR32BG12P232F512IM68)
#include "efr32bg12p232f512im68.h"

#elif defined(EFR32BG12P332F1024GL125)
#include "efr32bg12p332f1024gl125.h"

#elif defined(EFR32BG12P332F1024GM48)
#include "efr32bg12p332f1024gm48.h"

#elif defined(EFR32BG12P332F1024IM48)
#include "efr32bg12p332f1024im48.h"

#elif defined(EFR32BG12P432F1024GL125)
#include "efr32bg12p432f1024gl125.h"

#elif defined(EFR32BG12P432F1024GM48)
#include "efr32bg12p432f1024gm48.h"

#elif defined(EFR32BG12P433F1024GL125)
#include "efr32bg12p433f1024gl125.h"

#elif defined(EFR32BG12P433F1024GM48)
#include "efr32bg12p433f1024gm48.h"

#elif defined(EFR32BG12P433F1024GM68)
#include "efr32bg12p433f1024gm68.h"

#elif defined(EFR32BG12P433F1024IM68)
#include "efr32bg12p433f1024im68.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif

#endif /* EM_DEVICE_H */
