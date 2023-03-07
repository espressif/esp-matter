/***************************************************************************//**
 * @file sl_wisun_version.h
 * @brief Wi-SUN version
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_VERSION_H
#define SL_WISUN_VERSION_H

#ifndef SL_WISUN_VERSION_MAJOR
#define SL_WISUN_VERSION_MAJOR 1
#endif

#ifndef SL_WISUN_VERSION_MINOR
#define SL_WISUN_VERSION_MINOR 3
#endif

#ifndef SL_WISUN_VERSION_PATCH
#define SL_WISUN_VERSION_PATCH 1
#endif

#define FORMAT_WISUN_STACK_VERSION(a,b,c) (((a) << 16) + ((b) << 8) +  (c))
#define SL_WISUN_VERSION FORMAT_WISUN_STACK_VERSION(SL_WISUN_VERSION_MAJOR, SL_WISUN_VERSION_MINOR, SL_WISUN_VERSION_PATCH)

#endif  // SL_WISUN_VERSION_H
