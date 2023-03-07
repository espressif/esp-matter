/***************************************************************************//**
 * @file
 * @brief Flash Controller (MSC) Compatibility Header
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

#ifndef EM_MSC_COMPAT_H
#define EM_MSC_COMPAT_H

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)

#define MSC_IF_PWROFFIF                         MSC_IF_PWROFF
#define _MSC_IF_PWROFFIF_SHIFT                  _MSC_IF_PWROFF_SHIFT
#define _MSC_IF_PWROFFIF_MASK                   _MSC_IF_PWROFF_MASK
#define _MSC_IF_PWROFFIF_DEFAULT                _MSC_IF_PWROFF_DEFAULT
#define MSC_IF_PWROFFIF_DEFAULT                 MSC_IF_PWROFF_DEFAULT

#define MSC_IEN_PWROFFIEN                       MSC_IEN_PWROFF
#define _MSC_IEN_PWROFFIEN_SHIFT                _MSC_IEN_PWROFF_SHIFT
#define _MSC_IEN_PWROFFIEN_MASK                 _MSC_IEN_PWROFF_MASK
#define _MSC_IEN_PWROFFIEN_DEFAULT              _MSC_IEN_PWROFF_DEFAULT
#define MSC_IEN_PWROFFIEN_DEFAULT               MSC_IEN_PWROFF_DEFAULT


#define ICACHE_IEN_RAMERRORIEN                  ICACHE_IEN_RAMERROR
#define _ICACHE_IEN_RAMERRORIEN_SHIFT           _ICACHE_IEN_RAMERROR_SHIFT
#define _ICACHE_IEN_RAMERRORIEN_MASK            _ICACHE_IEN_RAMERROR_MASK
#define _ICACHE_IEN_RAMERRORIEN_DEFAULT         _ICACHE_IEN_RAMERROR_DEFAULT
#define ICACHE_IEN_RAMERRORIEN_DEFAULT          ICACHE_IEN_RAMERROR_DEFAULT


#define SYSCFG_IF_FRCRAMERR1BIF                 SYSCFG_IF_FRCRAMERR1B
#define _SYSCFG_IF_FRCRAMERR1BIF_SHIFT          _SYSCFG_IF_FRCRAMERR1B_SHIFT
#define _SYSCFG_IF_FRCRAMERR1BIF_MASK           _SYSCFG_IF_FRCRAMERR1B_MASK
#define _SYSCFG_IF_FRCRAMERR1BIF_DEFAULT        _SYSCFG_IF_FRCRAMERR1B_DEFAULT
#define SYSCFG_IF_FRCRAMERR1BIF_DEFAULT         SYSCFG_IF_FRCRAMERR1B_DEFAULT

#define SYSCFG_IF_FRCRAMERR2BIF                 SYSCFG_IF_FRCRAMERR2B
#define _SYSCFG_IF_FRCRAMERR2BIF_SHIFT          _SYSCFG_IF_FRCRAMERR2B_SHIFT
#define _SYSCFG_IF_FRCRAMERR2BIF_MASK           _SYSCFG_IF_FRCRAMERR2B_MASK
#define _SYSCFG_IF_FRCRAMERR2BIF_DEFAULT        _SYSCFG_IF_FRCRAMERR2B_DEFAULT
#define SYSCFG_IF_FRCRAMERR2BIF_DEFAULT         SYSCFG_IF_FRCRAMERR2B_DEFAULT

#define SYSCFG_IEN_FRCRAMERR1BIEN               SYSCFG_IEN_FRCRAMERR1B
#define _SYSCFG_IEN_FRCRAMERR1BIEN_SHIFT        _SYSCFG_IEN_FRCRAMERR1B_SHIFT
#define _SYSCFG_IEN_FRCRAMERR1BIEN_MASK         _SYSCFG_IEN_FRCRAMERR1B_MASK
#define _SYSCFG_IEN_FRCRAMERR1BIEN_DEFAULT      _SYSCFG_IEN_FRCRAMERR1B_DEFAULT
#define SYSCFG_IEN_FRCRAMERR1BIEN_DEFAULT       SYSCFG_IEN_FRCRAMERR1B_DEFAULT

#define SYSCFG_IEN_FRCRAMERR2BIEN               SYSCFG_IEN_FRCRAMERR2B
#define _SYSCFG_IEN_FRCRAMERR2BIEN_SHIFT        _SYSCFG_IEN_FRCRAMERR2B_SHIFT
#define _SYSCFG_IEN_FRCRAMERR2BIEN_MASK         _SYSCFG_IEN_FRCRAMERR2B_MASK
#define _SYSCFG_IEN_FRCRAMERR2BIEN_DEFAULT      _SYSCFG_IEN_FRCRAMERR2B_DEFAULT
#define SYSCFG_IEN_FRCRAMERR2BIEN_DEFAULT       SYSCFG_IEN_FRCRAMERR2B_DEFAULT

#endif /* _SILICON_LABS_32B_SERIES_2_CONFIG_2 */
#endif /* EM_MSC_COMPAT_H */
