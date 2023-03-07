/***************************************************************************//**
 * @file
 * @brief CMU Compatibility Header
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

#ifndef EM_CMU_COMPAT_H
#define EM_CMU_COMPAT_H

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)

#define CMU_IF_CALRDYIF                         CMU_IF_CALRDY
#define _CMU_IF_CALRDYIF_SHIFT                  _CMU_IF_CALRDY_SHIFT
#define _CMU_IF_CALRDYIF_MASK                   _CMU_IF_CALRDY_MASK
#define _CMU_IF_CALRDYIF_DEFAULT                _CMU_IF_CALRDY_DEFAULT
#define CMU_IF_CALRDYIF_DEFAULT                 CMU_IF_CALRDY_DEFAULT

#define CMU_IF_CALOFIF                          CMU_IF_CALOF
#define _CMU_IF_CALOFIF_SHIFT                   _CMU_IF_CALOF_SHIFT
#define _CMU_IF_CALOFIF_MASK                    _CMU_IF_CALOF_MASK
#define _CMU_IF_CALOFIF_DEFAULT                 _CMU_IF_CALOF_DEFAULT
#define CMU_IF_CALOFIF_DEFAULT                  CMU_IF_CALOF_DEFAULT

#define CMU_IEN_CALRDYIEN                       CMU_IEN_CALRDY
#define _CMU_IEN_CALRDYIEN_SHIFT                _CMU_IEN_CALRDY_SHIFT
#define _CMU_IEN_CALRDYIEN_MASK                 _CMU_IEN_CALRDY_MASK
#define _CMU_IEN_CALRDYIEN_DEFAULT              _CMU_IEN_CALRDY_DEFAULT
#define CMU_IEN_CALRDYIEN_DEFAULT               CMU_IEN_CALRDY_DEFAULT

#define CMU_IEN_CALOFIEN                        CMU_IEN_CALOF
#define _CMU_IEN_CALOFIEN_SHIFT                 _CMU_IEN_CALOF_SHIFT
#define _CMU_IEN_CALOFIEN_MASK                  _CMU_IEN_CALOF_MASK
#define _CMU_IEN_CALOFIEN_DEFAULT               _CMU_IEN_CALOF_DEFAULT
#define CMU_IEN_CALOFIEN_DEFAULT                CMU_IEN_CALOF_DEFAULT


#define HFRCO_IF_RDYIF                          HFRCO_IF_RDY
#define _HFRCO_IF_RDYIF_SHIFT                   _HFRCO_IF_RDY_SHIFT
#define _HFRCO_IF_RDYIF_MASK                    _HFRCO_IF_RDY_MASK
#define _HFRCO_IF_RDYIF_DEFAULT                 _HFRCO_IF_RDY_DEFAULT
#define HFRCO_IF_RDYIF_DEFAULT                  HFRCO_IF_RDY_DEFAULT

#define HFRCO_IEN_RDYIEN                        HFRCO_IEN_RDY
#define _HFRCO_IEN_RDYIEN_SHIFT                 _HFRCO_IEN_RDY_SHIFT
#define _HFRCO_IEN_RDYIEN_MASK                  _HFRCO_IEN_RDY_MASK
#define _HFRCO_IEN_RDYIEN_DEFAULT               _HFRCO_IEN_RDY_DEFAULT
#define HFRCO_IEN_RDYIEN_DEFAULT                HFRCO_IEN_RDY_DEFAULT


#define LFRCO_IF_RDYIF                          LFRCO_IF_RDY
#define _LFRCO_IF_RDYIF_SHIFT                   _LFRCO_IF_RDY_SHIFT
#define _LFRCO_IF_RDYIF_MASK                    _LFRCO_IF_RDY_MASK
#define _LFRCO_IF_RDYIF_DEFAULT                 _LFRCO_IF_RDY_DEFAULT
#define LFRCO_IF_RDYIF_DEFAULT                  LFRCO_IF_RDY_DEFAULT

#define LFRCO_IF_POSEDGEIF                      LFRCO_IF_POSEDGE
#define _LFRCO_IF_POSEDGEIF_SHIFT               _LFRCO_IF_POSEDGE_SHIFT
#define _LFRCO_IF_POSEDGEIF_MASK                _LFRCO_IF_POSEDGE_MASK
#define _LFRCO_IF_POSEDGEIF_DEFAULT             _LFRCO_IF_POSEDGE_DEFAULT
#define LFRCO_IF_POSEDGEIF_DEFAULT              LFRCO_IF_POSEDGE_DEFAULT

#define LFRCO_IF_NEGEDGEIF                      LFRCO_IF_NEGEDGE
#define _LFRCO_IF_NEGEDGEIF_SHIFT               _LFRCO_IF_NEGEDGE_SHIFT
#define _LFRCO_IF_NEGEDGEIF_MASK                _LFRCO_IF_NEGEDGE_MASK
#define _LFRCO_IF_NEGEDGEIF_DEFAULT             _LFRCO_IF_NEGEDGE_DEFAULT
#define LFRCO_IF_NEGEDGEIF_DEFAULT              LFRCO_IF_NEGEDGE_DEFAULT

#define LFRCO_IF_TCDONEIF                       LFRCO_IF_TCDONE
#define _LFRCO_IF_TCDONEIF_SHIFT                _LFRCO_IF_TCDONE_SHIFT
#define _LFRCO_IF_TCDONEIF_MASK                 _LFRCO_IF_TCDONE_MASK
#define _LFRCO_IF_TCDONEIF_DEFAULT              _LFRCO_IF_TCDONE_DEFAULT
#define LFRCO_IF_TCDONEIF_DEFAULT               LFRCO_IF_TCDONE_DEFAULT

#define LFRCO_IF_CALDONEIF                      LFRCO_IF_CALDONE
#define _LFRCO_IF_CALDONEIF_SHIFT               _LFRCO_IF_CALDONE_SHIFT
#define _LFRCO_IF_CALDONEIF_MASK                _LFRCO_IF_CALDONE_MASK
#define _LFRCO_IF_CALDONEIF_DEFAULT             _LFRCO_IF_CALDONE_DEFAULT
#define LFRCO_IF_CALDONEIF_DEFAULT              LFRCO_IF_CALDONE_DEFAULT

#define LFRCO_IF_TEMPCHANGEIF                   LFRCO_IF_TEMPCHANGE
#define _LFRCO_IF_TEMPCHANGEIF_SHIFT            _LFRCO_IF_TEMPCHANGE_SHIFT
#define _LFRCO_IF_TEMPCHANGEIF_MASK             _LFRCO_IF_TEMPCHANGE_MASK
#define _LFRCO_IF_TEMPCHANGEIF_DEFAULT          _LFRCO_IF_TEMPCHANGE_DEFAULT
#define LFRCO_IF_TEMPCHANGEIF_DEFAULT           LFRCO_IF_TEMPCHANGE_DEFAULT

#define LFRCO_IF_SCHEDERRIF                     LFRCO_IF_SCHEDERR
#define _LFRCO_IF_SCHEDERRIF_SHIFT              _LFRCO_IF_SCHEDERR_SHIFT
#define _LFRCO_IF_SCHEDERRIF_MASK               _LFRCO_IF_SCHEDERR_MASK
#define _LFRCO_IF_SCHEDERRIF_DEFAULT            _LFRCO_IF_SCHEDERR_DEFAULT
#define LFRCO_IF_SCHEDERRIF_DEFAULT             LFRCO_IF_SCHEDERR_DEFAULT

#define LFRCO_IF_TCOORIF                        LFRCO_IF_TCOOR
#define _LFRCO_IF_TCOORIF_SHIFT                 _LFRCO_IF_TCOOR_SHIFT
#define _LFRCO_IF_TCOORIF_MASK                  _LFRCO_IF_TCOOR_MASK
#define _LFRCO_IF_TCOORIF_DEFAULT               _LFRCO_IF_TCOOR_DEFAULT
#define LFRCO_IF_TCOORIF_DEFAULT                LFRCO_IF_TCOOR_DEFAULT

#define LFRCO_IF_CALOORIF                       LFRCO_IF_CALOOR
#define _LFRCO_IF_CALOORIF_SHIFT                _LFRCO_IF_CALOOR_SHIFT
#define _LFRCO_IF_CALOORIF_MASK                 _LFRCO_IF_CALOOR_MASK
#define _LFRCO_IF_CALOORIF_DEFAULT              _LFRCO_IF_CALOOR_DEFAULT
#define LFRCO_IF_CALOORIF_DEFAULT               LFRCO_IF_CALOOR_DEFAULT

#define LFRCO_IEN_RDYIEN                        LFRCO_IEN_RDY
#define _LFRCO_IEN_RDYIEN_SHIFT                 _LFRCO_IEN_RDY_SHIFT
#define _LFRCO_IEN_RDYIEN_MASK                  _LFRCO_IEN_RDY_MASK
#define _LFRCO_IEN_RDYIEN_DEFAULT               _LFRCO_IEN_RDY_DEFAULT
#define LFRCO_IEN_RDYIEN_DEFAULT                LFRCO_IEN_RDY_DEFAULT

#define LFRCO_IEN_POSEDGEIEN                    LFRCO_IEN_POSEDGE
#define _LFRCO_IEN_POSEDGEIEN_SHIFT             _LFRCO_IEN_POSEDGE_SHIFT
#define _LFRCO_IEN_POSEDGEIEN_MASK              _LFRCO_IEN_POSEDGE_MASK
#define _LFRCO_IEN_POSEDGEIEN_DEFAULT           _LFRCO_IEN_POSEDGE_DEFAULT
#define LFRCO_IEN_POSEDGEIEN_DEFAULT            LFRCO_IEN_POSEDGE_DEFAULT

#define LFRCO_IEN_NEGEDGEIEN                    LFRCO_IEN_NEGEDGE
#define _LFRCO_IEN_NEGEDGEIEN_SHIFT             _LFRCO_IEN_NEGEDGE_SHIFT
#define _LFRCO_IEN_NEGEDGEIEN_MASK              _LFRCO_IEN_NEGEDGE_MASK
#define _LFRCO_IEN_NEGEDGEIEN_DEFAULT           _LFRCO_IEN_NEGEDGE_DEFAULT
#define LFRCO_IEN_NEGEDGEIEN_DEFAULT            LFRCO_IEN_NEGEDGE_DEFAULT

#define LFRCO_IEN_TCDONEIEN                     LFRCO_IEN_TCDONE
#define _LFRCO_IEN_TCDONEIEN_SHIFT              _LFRCO_IEN_TCDONE_SHIFT
#define _LFRCO_IEN_TCDONEIEN_MASK               _LFRCO_IEN_TCDONE_MASK
#define _LFRCO_IEN_TCDONEIEN_DEFAULT            _LFRCO_IEN_TCDONE_DEFAULT
#define LFRCO_IEN_TCDONEIEN_DEFAULT             LFRCO_IEN_TCDONE_DEFAULT

#define LFRCO_IEN_CALDONEIEN                    LFRCO_IEN_CALDONE
#define _LFRCO_IEN_CALDONEIEN_SHIFT             _LFRCO_IEN_CALDONE_SHIFT
#define _LFRCO_IEN_CALDONEIEN_MASK              _LFRCO_IEN_CALDONE_MASK
#define _LFRCO_IEN_CALDONEIEN_DEFAULT           _LFRCO_IEN_CALDONE_DEFAULT
#define LFRCO_IEN_CALDONEIEN_DEFAULT            LFRCO_IEN_CALDONE_DEFAULT

#define LFRCO_IEN_TEMPCHANGEIEN                 LFRCO_IEN_TEMPCHANGE
#define _LFRCO_IEN_TEMPCHANGEIEN_SHIFT          _LFRCO_IEN_TEMPCHANGE_SHIFT
#define _LFRCO_IEN_TEMPCHANGEIEN_MASK           _LFRCO_IEN_TEMPCHANGE_MASK
#define _LFRCO_IEN_TEMPCHANGEIEN_DEFAULT        _LFRCO_IEN_TEMPCHANGE_DEFAULT
#define LFRCO_IEN_TEMPCHANGEIEN_DEFAULT         LFRCO_IEN_TEMPCHANGE_DEFAULT

#define LFRCO_IEN_SCHEDERRIEN                   LFRCO_IEN_SCHEDERR
#define _LFRCO_IEN_SCHEDERRIEN_SHIFT            _LFRCO_IEN_SCHEDERR_SHIFT
#define _LFRCO_IEN_SCHEDERRIEN_MASK             _LFRCO_IEN_SCHEDERR_MASK
#define _LFRCO_IEN_SCHEDERRIEN_DEFAULT          _LFRCO_IEN_SCHEDERR_DEFAULT
#define LFRCO_IEN_SCHEDERRIEN_DEFAULT           LFRCO_IEN_SCHEDERR_DEFAULT

#define LFRCO_IEN_TCOORIEN                      LFRCO_IEN_TCOOR
#define _LFRCO_IEN_TCOORIEN_SHIFT               _LFRCO_IEN_TCOOR_SHIFT
#define _LFRCO_IEN_TCOORIEN_MASK                _LFRCO_IEN_TCOOR_MASK
#define _LFRCO_IEN_TCOORIEN_DEFAULT             _LFRCO_IEN_TCOOR_DEFAULT
#define LFRCO_IEN_TCOORIEN_DEFAULT              LFRCO_IEN_TCOOR_DEFAULT

#define LFRCO_IEN_CALOORIEN                     LFRCO_IEN_CALOOR
#define _LFRCO_IEN_CALOORIEN_SHIFT              _LFRCO_IEN_CALOOR_SHIFT
#define _LFRCO_IEN_CALOORIEN_MASK               _LFRCO_IEN_CALOOR_MASK
#define _LFRCO_IEN_CALOORIEN_DEFAULT            _LFRCO_IEN_CALOOR_DEFAULT
#define LFRCO_IEN_CALOORIEN_DEFAULT             LFRCO_IEN_CALOOR_DEFAULT

#endif /* _SILICON_LABS_32B_SERIES_2_CONFIG_2 */

#endif
