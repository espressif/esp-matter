/***************************************************************************//**
 * @file
 * @brief Advanced encryption standard (AES) Compatibility Header
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

#ifndef EM_AES_COMPAT_H
#define EM_AES_COMPAT_H

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)

#define AES_IEN_FETCHERENDOFBLOCKIEN                         AES_IEN_FETCHERENDOFBLOCK
#define _AES_IEN_FETCHERENDOFBLOCKIEN_SHIFT                  _AES_IEN_FETCHERENDOFBLOCK_SHIFT
#define _AES_IEN_FETCHERENDOFBLOCKIEN_MASK                   _AES_IEN_FETCHERENDOFBLOCK_MASK
#define _AES_IEN_FETCHERENDOFBLOCKIEN_DEFAULT                _AES_IEN_FETCHERENDOFBLOCK_DEFAULT
#define AES_IEN_FETCHERENDOFBLOCKIEN_DEFAULT                 AES_IEN_FETCHERENDOFBLOCK_DEFAULT

#define AES_IEN_FETCHERSTOPPEDIEN                            AES_IEN_FETCHERSTOPPED
#define _AES_IEN_FETCHERSTOPPEDIEN_SHIFT                     _AES_IEN_FETCHERSTOPPED_SHIFT
#define _AES_IEN_FETCHERSTOPPEDIEN_MASK                      _AES_IEN_FETCHERSTOPPED_MASK
#define _AES_IEN_FETCHERSTOPPEDIEN_DEFAULT                   _AES_IEN_FETCHERSTOPPED_DEFAULT
#define AES_IEN_FETCHERSTOPPEDIEN_DEFAULT                    AES_IEN_FETCHERSTOPPED_DEFAULT

#define AES_IEN_FETCHERERRORIEN                              AES_IEN_FETCHERERROR
#define _AES_IEN_FETCHERERRORIEN_SHIFT                       _AES_IEN_FETCHERERROR_SHIFT
#define _AES_IEN_FETCHERERRORIEN_MASK                        _AES_IEN_FETCHERERROR_MASK
#define _AES_IEN_FETCHERERRORIEN_DEFAULT                     _AES_IEN_FETCHERERROR_DEFAULT
#define AES_IEN_FETCHERERRORIEN_DEFAULT                      AES_IEN_FETCHERERROR_DEFAULT

#define AES_IEN_PUSHERENDOFBLOCKIEN                          AES_IEN_PUSHERENDOFBLOCK
#define _AES_IEN_PUSHERENDOFBLOCKIEN_SHIFT                   _AES_IEN_PUSHERENDOFBLOCK_SHIFT
#define _AES_IEN_PUSHERENDOFBLOCKIEN_MASK                    _AES_IEN_PUSHERENDOFBLOCK_MASK
#define _AES_IEN_PUSHERENDOFBLOCKIEN_DEFAULT                 _AES_IEN_PUSHERENDOFBLOCK_DEFAULT
#define AES_IEN_PUSHERENDOFBLOCKIEN_DEFAULT                  AES_IEN_PUSHERENDOFBLOCK_DEFAULT

#define AES_IEN_PUSHERSTOPPEDIEN                             AES_IEN_PUSHERSTOPPED
#define _AES_IEN_PUSHERSTOPPEDIEN_SHIFT                      _AES_IEN_PUSHERSTOPPED_SHIFT
#define _AES_IEN_PUSHERSTOPPEDIEN_MASK                       _AES_IEN_PUSHERSTOPPED_MASK
#define _AES_IEN_PUSHERSTOPPEDIEN_DEFAULT                    _AES_IEN_PUSHERSTOPPED_DEFAULT
#define AES_IEN_PUSHERSTOPPEDIEN_DEFAULT                     AES_IEN_PUSHERSTOPPED_DEFAULT

#define AES_IEN_PUSHERERRORIEN                               AES_IEN_PUSHERERROR
#define _AES_IEN_PUSHERERRORIEN_SHIFT                        _AES_IEN_PUSHERERROR_SHIFT
#define _AES_IEN_PUSHERERRORIEN_MASK                         _AES_IEN_PUSHERERROR_MASK
#define _AES_IEN_PUSHERERRORIEN_DEFAULT                      _AES_IEN_PUSHERERROR_DEFAULT
#define AES_IEN_PUSHERERRORIEN_DEFAULT                       AES_IEN_PUSHERERROR_DEFAULT

#define AES_IF_FETCHERENDOFBLOCKIF                          AES_IF_FETCHERENDOFBLOCK
#define _AES_IF_FETCHERENDOFBLOCKIF_SHIFT                   _AES_IF_FETCHERENDOFBLOCK_SHIFT
#define _AES_IF_FETCHERENDOFBLOCKIF_MASK                    _AES_IF_FETCHERENDOFBLOCK_MASK
#define _AES_IF_FETCHERENDOFBLOCKIF_DEFAULT                 _AES_IF_FETCHERENDOFBLOCK_DEFAULT
#define AES_IF_FETCHERENDOFBLOCKIF_DEFAULT                  AES_IF_FETCHERENDOFBLOCK_DEFAULT

#define AES_IF_FETCHERSTOPPEDIF                             AES_IF_FETCHERSTOPPED
#define _AES_IF_FETCHERSTOPPEDIF_SHIFT                      _AES_IF_FETCHERSTOPPED_SHIFT
#define _AES_IF_FETCHERSTOPPEDIF_MASK                       _AES_IF_FETCHERSTOPPED_MASK
#define _AES_IF_FETCHERSTOPPEDIF_DEFAULT                    _AES_IF_FETCHERSTOPPED_DEFAULT
#define AES_IF_FETCHERSTOPPEDIF_DEFAULT                     AES_IF_FETCHERSTOPPED_DEFAULT

#define AES_IF_FETCHERERRORIF                               AES_IF_FETCHERERROR
#define _AES_IF_FETCHERERRORIF_SHIFT                        _AES_IF_FETCHERERROR_SHIFT
#define _AES_IF_FETCHERERRORIF_MASK                         _AES_IF_FETCHERERROR_MASK
#define _AES_IF_FETCHERERRORIF_DEFAULT                      _AES_IF_FETCHERERROR_DEFAULT
#define AES_IF_FETCHERERRORIF_DEFAULT                       AES_IF_FETCHERERROR_DEFAULT

#define AES_IF_PUSHERENDOFBLOCKIF                           AES_IF_PUSHERENDOFBLOCK
#define _AES_IF_PUSHERENDOFBLOCKIF_SHIFT                    _AES_IF_PUSHERENDOFBLOCK_SHIFT
#define _AES_IF_PUSHERENDOFBLOCKIF_MASK                     _AES_IF_PUSHERENDOFBLOCK_MASK
#define _AES_IF_PUSHERENDOFBLOCKIF_DEFAULT                  _AES_IF_PUSHERENDOFBLOCK_DEFAULT
#define AES_IF_PUSHERENDOFBLOCKIF_DEFAULT                   AES_IF_PUSHERENDOFBLOCK_DEFAULT

#define AES_IF_PUSHERSTOPPEDIF                              AES_IF_PUSHERSTOPPED
#define _AES_IF_PUSHERSTOPPEDIF_SHIFT                       _AES_IF_PUSHERSTOPPED_SHIFT
#define _AES_IF_PUSHERSTOPPEDIF_MASK                        _AES_IF_PUSHERSTOPPED_MASK
#define _AES_IF_PUSHERSTOPPEDIF_DEFAULT                     _AES_IF_PUSHERSTOPPED_DEFAULT
#define AES_IF_PUSHERSTOPPEDIF_DEFAULT                      AES_IF_PUSHERSTOPPED_DEFAULT

#define AES_IF_PUSHERERRORIF                                AES_IF_PUSHERERROR
#define _AES_IF_PUSHERERRORIF_SHIFT                         _AES_IF_PUSHERERROR_SHIFT
#define _AES_IF_PUSHERERRORIF_MASK                          _AES_IF_PUSHERERROR_MASK
#define _AES_IF_PUSHERERRORIF_DEFAULT                       _AES_IF_PUSHERERROR_DEFAULT
#define AES_IF_PUSHERERRORIF_DEFAULT                        AES_IF_PUSHERERROR_DEFAULT

#define AES_IF_CLR_FETCHERENDOFBLOCKIFC                     AES_IF_CLR_FETCHERENDOFBLOCK
#define _AES_IF_CLR_FETCHERENDOFBLOCKIFC_SHIFT              _AES_IF_CLR_FETCHERENDOFBLOCK_SHIFT
#define _AES_IF_CLR_FETCHERENDOFBLOCKIFC_MASK               _AES_IF_CLR_FETCHERENDOFBLOCK_MASK
#define _AES_IF_CLR_FETCHERENDOFBLOCKIFC_DEFAULT            _AES_IF_CLR_FETCHERENDOFBLOCK_DEFAULT
#define AES_IF_CLR_FETCHERENDOFBLOCKIFC_DEFAULT             AES_IF_CLR_FETCHERENDOFBLOCK_DEFAULT

#define AES_IF_CLR_FETCHERSTOPPEDIFC                        AES_IF_CLR_FETCHERSTOPPED
#define _AES_IF_CLR_FETCHERSTOPPEDIFC_SHIFT                 _AES_IF_CLR_FETCHERSTOPPED_SHIFT
#define _AES_IF_CLR_FETCHERSTOPPEDIFC_MASK                  _AES_IF_CLR_FETCHERSTOPPED_MASK
#define _AES_IF_CLR_FETCHERSTOPPEDIFC_DEFAULT               _AES_IF_CLR_FETCHERSTOPPED_DEFAULT
#define AES_IF_CLR_FETCHERSTOPPEDIFC_DEFAULT                AES_IF_CLR_FETCHERSTOPPED_DEFAULT

#define AES_IF_CLR_FETCHERERRORIFC                          AES_IF_CLR_FETCHERERROR
#define _AES_IF_CLR_FETCHERERRORIFC_SHIFT                   _AES_IF_CLR_FETCHERERROR_SHIFT
#define _AES_IF_CLR_FETCHERERRORIFC_MASK                    _AES_IF_CLR_FETCHERERROR_MASK
#define _AES_IF_CLR_FETCHERERRORIFC_DEFAULT                 _AES_IF_CLR_FETCHERERROR_DEFAULT
#define AES_IF_CLR_FETCHERERRORIFC_DEFAULT                  AES_IF_CLR_FETCHERERROR_DEFAULT

#define AES_IF_CLR_PUSHERENDOFBLOCKIFC                      AES_IF_CLR_PUSHERENDOFBLOCK
#define _AES_IF_CLR_PUSHERENDOFBLOCKIFC_SHIFT               _AES_IF_CLR_PUSHERENDOFBLOCK_SHIFT
#define _AES_IF_CLR_PUSHERENDOFBLOCKIFC_MASK                _AES_IF_CLR_PUSHERENDOFBLOCK_MASK
#define _AES_IF_CLR_PUSHERENDOFBLOCKIFC_DEFAULT             _AES_IF_CLR_PUSHERENDOFBLOCK_DEFAULT
#define AES_IF_CLR_PUSHERENDOFBLOCKIFC_DEFAULT              AES_IF_CLR_PUSHERENDOFBLOCK_DEFAULT

#define AES_IF_CLR_PUSHERSTOPPEDIFC                         AES_IF_CLR_PUSHERSTOPPED
#define _AES_IF_CLR_PUSHERSTOPPEDIFC_SHIFT                  _AES_IF_CLR_PUSHERSTOPPED_SHIFT
#define _AES_IF_CLR_PUSHERSTOPPEDIFC_MASK                   _AES_IF_CLR_PUSHERSTOPPED_MASK
#define _AES_IF_CLR_PUSHERSTOPPEDIFC_DEFAULT                _AES_IF_CLR_PUSHERSTOPPED_DEFAULT
#define AES_IF_CLR_PUSHERSTOPPEDIFC_DEFAULT                 AES_IF_CLR_PUSHERSTOPPED_DEFAULT

#define AES_IF_CLR_PUSHERERRORIFC                           AES_IF_CLR_PUSHERERROR
#define _AES_IF_CLR_PUSHERERRORIFC_SHIFT                    _AES_IF_CLR_PUSHERERROR_SHIFT
#define _AES_IF_CLR_PUSHERERRORIFC_MASK                     _AES_IF_CLR_PUSHERERROR_MASK
#define _AES_IF_CLR_PUSHERERRORIFC_DEFAULT                  _AES_IF_CLR_PUSHERERROR_DEFAULT
#define AES_IF_CLR_PUSHERERRORIFC_DEFAULT                   AES_IF_CLR_PUSHERERROR_DEFAULT

#endif /* _SILICON_LABS_32B_SERIES_2_CONFIG_2 */
#endif /* EM_AES_COMPAT_H */
