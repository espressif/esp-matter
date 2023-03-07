/***************************************************************************//**
 * @file
 * @brief EUSART Compatibility Header
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

#ifndef EM_EUSART_COMPAT_H
#define EM_EUSART_COMPAT_H

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)

#define EUSART_IF_TXCIF                             EUSART_IF_TXC
#define _EUSART_IF_TXCIF_SHIFT                      _EUSART_IF_TXC_SHIFT
#define _EUSART_IF_TXCIF_MASK                       _EUSART_IF_TXC_MASK
#define _EUSART_IF_TXCIF_DEFAULT                    _EUSART_IF_TXC_DEFAULT
#define EUSART_IF_TXCIF_DEFAULT                     EUSART_IF_TXC_DEFAULT

#define EUSART_IF_TXFLIF                            EUSART_IF_TXFL
#define _EUSART_IF_TXFLIF_SHIFT                     _EUSART_IF_TXFL_SHIFT
#define _EUSART_IF_TXFLIF_MASK                      _EUSART_IF_TXFL_MASK
#define _EUSART_IF_TXFLIF_DEFAULT                   _EUSART_IF_TXFL_DEFAULT
#define EUSART_IF_TXFLIF_DEFAULT                    EUSART_IF_TXFL_DEFAULT

#define EUSART_IF_RXFLIF                            EUSART_IF_RXFL
#define _EUSART_IF_RXFLIF_SHIFT                     _EUSART_IF_RXFL_SHIFT
#define _EUSART_IF_RXFLIF_MASK                      _EUSART_IF_RXFL_MASK
#define _EUSART_IF_RXFLIF_DEFAULT                   _EUSART_IF_RXFL_DEFAULT
#define EUSART_IF_RXFLIF_DEFAULT                    EUSART_IF_RXFL_DEFAULT

#define EUSART_IF_RXFULLIF                          EUSART_IF_RXFULL
#define _EUSART_IF_RXFULLIF_SHIFT                   _EUSART_IF_RXFULL_SHIFT
#define _EUSART_IF_RXFULLIF_MASK                    _EUSART_IF_RXFULL_MASK
#define _EUSART_IF_RXFULLIF_DEFAULT                 _EUSART_IF_RXFULL_DEFAULT
#define EUSART_IF_RXFULLIF_DEFAULT                  EUSART_IF_RXFULL_DEFAULT

#define EUSART_IF_RXOFIF                            EUSART_IF_RXOF
#define _EUSART_IF_RXOFIF_SHIFT                     _EUSART_IF_RXOF_SHIFT
#define _EUSART_IF_RXOFIF_MASK                      _EUSART_IF_RXOF_MASK
#define _EUSART_IF_RXOFIF_DEFAULT                   _EUSART_IF_RXOF_DEFAULT
#define EUSART_IF_RXOFIF_DEFAULT                    EUSART_IF_RXOF_DEFAULT

#define EUSART_IF_RXUFIF                            EUSART_IF_RXUF
#define _EUSART_IF_RXUFIF_SHIFT                     _EUSART_IF_RXUF_SHIFT
#define _EUSART_IF_RXUFIF_MASK                      _EUSART_IF_RXUF_MASK
#define _EUSART_IF_RXUFIF_DEFAULT                   _EUSART_IF_RXUF_DEFAULT
#define EUSART_IF_RXUFIF_DEFAULT                    EUSART_IF_RXUF_DEFAULT

#define EUSART_IF_TXOFIF                            EUSART_IF_TXOF
#define _EUSART_IF_TXOFIF_SHIFT                     _EUSART_IF_TXOF_SHIFT
#define _EUSART_IF_TXOFIF_MASK                      _EUSART_IF_TXOF_MASK
#define _EUSART_IF_TXOFIF_DEFAULT                   _EUSART_IF_TXOF_DEFAULT
#define EUSART_IF_TXOFIF_DEFAULT                    EUSART_IF_TXOF_DEFAULT

#define EUSART_IF_PERRIF                            EUSART_IF_PERR
#define _EUSART_IF_PERRIF_SHIFT                     _EUSART_IF_PERR_SHIFT
#define _EUSART_IF_PERRIF_MASK                      _EUSART_IF_PERR_MASK
#define _EUSART_IF_PERRIF_DEFAULT                   _EUSART_IF_PERR_DEFAULT
#define EUSART_IF_PERRIF_DEFAULT                    EUSART_IF_PERR_DEFAULT

#define EUSART_IF_FERRIF                            EUSART_IF_FERR
#define _EUSART_IF_FERRIF_SHIFT                     _EUSART_IF_FERR_SHIFT
#define _EUSART_IF_FERRIF_MASK                      _EUSART_IF_FERR_MASK
#define _EUSART_IF_FERRIF_DEFAULT                   _EUSART_IF_FERR_DEFAULT
#define EUSART_IF_FERRIF_DEFAULT                    EUSART_IF_FERR_DEFAULT

#define EUSART_IF_MPAFIF                            EUSART_IF_MPAF
#define _EUSART_IF_MPAFIF_SHIFT                     _EUSART_IF_MPAF_SHIFT
#define _EUSART_IF_MPAFIF_MASK                      _EUSART_IF_MPAF_MASK
#define _EUSART_IF_MPAFIF_DEFAULT                   _EUSART_IF_MPAF_DEFAULT
#define EUSART_IF_MPAFIF_DEFAULT                    EUSART_IF_MPAF_DEFAULT

#define EUSART_IF_CCFIF                             EUSART_IF_CCF
#define _EUSART_IF_CCFIF_SHIFT                      _EUSART_IF_CCF_SHIFT
#define _EUSART_IF_CCFIF_MASK                       _EUSART_IF_CCF_MASK
#define _EUSART_IF_CCFIF_DEFAULT                    _EUSART_IF_CCF_DEFAULT
#define EUSART_IF_CCFIF_DEFAULT                     EUSART_IF_CCF_DEFAULT

#define EUSART_IF_TXIDLEIF                          EUSART_IF_TXIDLE
#define _EUSART_IF_TXIDLEIF_SHIFT                   _EUSART_IF_TXIDLE_SHIFT
#define _EUSART_IF_TXIDLEIF_MASK                    _EUSART_IF_TXIDLE_MASK
#define _EUSART_IF_TXIDLEIF_DEFAULT                 _EUSART_IF_TXIDLE_DEFAULT
#define EUSART_IF_TXIDLEIF_DEFAULT                  EUSART_IF_TXIDLE_DEFAULT

#define EUSART_IF_STARTFIF                          EUSART_IF_STARTF
#define _EUSART_IF_STARTFIF_SHIFT                   _EUSART_IF_STARTF_SHIFT
#define _EUSART_IF_STARTFIF_MASK                    _EUSART_IF_STARTF_MASK
#define _EUSART_IF_STARTFIF_DEFAULT                 _EUSART_IF_STARTF_DEFAULT
#define EUSART_IF_STARTFIF_DEFAULT                  EUSART_IF_STARTF_DEFAULT

#define EUSART_IF_SIGFIF                            EUSART_IF_SIGF
#define _EUSART_IF_SIGFIF_SHIFT                     _EUSART_IF_SIGF_SHIFT
#define _EUSART_IF_SIGFIF_MASK                      _EUSART_IF_SIGF_MASK
#define _EUSART_IF_SIGFIF_DEFAULT                   _EUSART_IF_SIGF_DEFAULT
#define EUSART_IF_SIGFIF_DEFAULT                    EUSART_IF_SIGF_DEFAULT

#define EUSART_IF_AUTOBAUDDONEIF                    EUSART_IF_AUTOBAUDDONE
#define _EUSART_IF_AUTOBAUDDONEIF_SHIFT             _EUSART_IF_AUTOBAUDDONE_SHIFT
#define _EUSART_IF_AUTOBAUDDONEIF_MASK              _EUSART_IF_AUTOBAUDDONE_MASK
#define _EUSART_IF_AUTOBAUDDONEIF_DEFAULT           _EUSART_IF_AUTOBAUDDONE_DEFAULT
#define EUSART_IF_AUTOBAUDDONEIF_DEFAULT            EUSART_IF_AUTOBAUDDONE_DEFAULT

#define EUSART_IEN_TXCIEN                            EUSART_IEN_TXC
#define _EUSART_IEN_TXCIEN_SHIFT                     _EUSART_IEN_TXC_SHIFT
#define _EUSART_IEN_TXCIEN_MASK                      _EUSART_IEN_TXC_MASK
#define _EUSART_IEN_TXCIEN_DEFAULT                   _EUSART_IEN_TXC_DEFAULT
#define EUSART_IEN_TXCIEN_DEFAULT                    EUSART_IEN_TXC_DEFAULT

#define EUSART_IEN_TXFLIEN                           EUSART_IEN_TXFL
#define _EUSART_IEN_TXFLIEN_SHIFT                    _EUSART_IEN_TXFL_SHIFT
#define _EUSART_IEN_TXFLIEN_MASK                     _EUSART_IEN_TXFL_MASK
#define _EUSART_IEN_TXFLIEN_DEFAULT                  _EUSART_IEN_TXFL_DEFAULT
#define EUSART_IEN_TXFLIEN_DEFAULT                   EUSART_IEN_TXFL_DEFAULT

#define EUSART_IEN_RXFLIEN                           EUSART_IEN_RXFL
#define _EUSART_IEN_RXFLIEN_SHIFT                    _EUSART_IEN_RXFL_SHIFT
#define _EUSART_IEN_RXFLIEN_MASK                     _EUSART_IEN_RXFL_MASK
#define _EUSART_IEN_RXFLIEN_DEFAULT                  _EUSART_IEN_RXFL_DEFAULT
#define EUSART_IEN_RXFLIEN_DEFAULT                   EUSART_IEN_RXFL_DEFAULT

#define EUSART_IEN_RXFULLIEN                         EUSART_IEN_RXFULL
#define _EUSART_IEN_RXFULLIEN_SHIFT                  _EUSART_IEN_RXFULL_SHIFT
#define _EUSART_IEN_RXFULLIEN_MASK                   _EUSART_IEN_RXFULL_MASK
#define _EUSART_IEN_RXFULLIEN_DEFAULT                _EUSART_IEN_RXFULL_DEFAULT
#define EUSART_IEN_RXFULLIEN_DEFAULT                 EUSART_IEN_RXFULL_DEFAULT

#define EUSART_IEN_RXOFIEN                           EUSART_IEN_RXOF
#define _EUSART_IEN_RXOFIEN_SHIFT                    _EUSART_IEN_RXOF_SHIFT
#define _EUSART_IEN_RXOFIEN_MASK                     _EUSART_IEN_RXOF_MASK
#define _EUSART_IEN_RXOFIEN_DEFAULT                  _EUSART_IEN_RXOF_DEFAULT
#define EUSART_IEN_RXOFIEN_DEFAULT                   EUSART_IEN_RXOF_DEFAULT

#define EUSART_IEN_RXUFIEN                           EUSART_IEN_RXUF
#define _EUSART_IEN_RXUFIEN_SHIFT                    _EUSART_IEN_RXUF_SHIFT
#define _EUSART_IEN_RXUFIEN_MASK                     _EUSART_IEN_RXUF_MASK
#define _EUSART_IEN_RXUFIEN_DEFAULT                  _EUSART_IEN_RXUF_DEFAULT
#define EUSART_IEN_RXUFIEN_DEFAULT                   EUSART_IEN_RXUF_DEFAULT

#define EUSART_IEN_TXOFIEN                           EUSART_IEN_TXOF
#define _EUSART_IEN_TXOFIEN_SHIFT                    _EUSART_IEN_TXOF_SHIFT
#define _EUSART_IEN_TXOFIEN_MASK                     _EUSART_IEN_TXOF_MASK
#define _EUSART_IEN_TXOFIEN_DEFAULT                  _EUSART_IEN_TXOF_DEFAULT
#define EUSART_IEN_TXOFIEN_DEFAULT                   EUSART_IEN_TXOF_DEFAULT

#define EUSART_IEN_PERRIEN                           EUSART_IEN_PERR
#define _EUSART_IEN_PERRIEN_SHIFT                    _EUSART_IEN_PERR_SHIFT
#define _EUSART_IEN_PERRIEN_MASK                     _EUSART_IEN_PERR_MASK
#define _EUSART_IEN_PERRIEN_DEFAULT                  _EUSART_IEN_PERR_DEFAULT
#define EUSART_IEN_PERRIEN_DEFAULT                   EUSART_IEN_PERR_DEFAULT

#define EUSART_IEN_FERRIEN                           EUSART_IEN_FERR
#define _EUSART_IEN_FERRIEN_SHIFT                    _EUSART_IEN_FERR_SHIFT
#define _EUSART_IEN_FERRIEN_MASK                     _EUSART_IEN_FERR_MASK
#define _EUSART_IEN_FERRIEN_DEFAULT                  _EUSART_IEN_FERR_DEFAULT
#define EUSART_IEN_FERRIEN_DEFAULT                   EUSART_IEN_FERR_DEFAULT

#define EUSART_IEN_MPAFIEN                           EUSART_IEN_MPAF
#define _EUSART_IEN_MPAFIEN_SHIFT                    _EUSART_IEN_MPAF_SHIFT
#define _EUSART_IEN_MPAFIEN_MASK                     _EUSART_IEN_MPAF_MASK
#define _EUSART_IEN_MPAFIEN_DEFAULT                  _EUSART_IEN_MPAF_DEFAULT
#define EUSART_IEN_MPAFIEN_DEFAULT                   EUSART_IEN_MPAF_DEFAULT

#define EUSART_IEN_CCFIEN                            EUSART_IEN_CCF
#define _EUSART_IEN_CCFIEN_SHIFT                     _EUSART_IEN_CCF_SHIFT
#define _EUSART_IEN_CCFIEN_MASK                      _EUSART_IEN_CCF_MASK
#define _EUSART_IEN_CCFIEN_DEFAULT                   _EUSART_IEN_CCF_DEFAULT
#define EUSART_IEN_CCFIEN_DEFAULT                    EUSART_IEN_CCF_DEFAULT

#define EUSART_IEN_TXIDLEIEN                         EUSART_IEN_TXIDLE
#define _EUSART_IEN_TXIDLEIEN_SHIFT                  _EUSART_IEN_TXIDLE_SHIFT
#define _EUSART_IEN_TXIDLEIEN_MASK                   _EUSART_IEN_TXIDLE_MASK
#define _EUSART_IEN_TXIDLEIEN_DEFAULT                _EUSART_IEN_TXIDLE_DEFAULT
#define EUSART_IEN_TXIDLEIEN_DEFAULT                 EUSART_IEN_TXIDLE_DEFAULT

#define EUSART_IEN_STARTFIEN                         EUSART_IEN_STARTF
#define _EUSART_IEN_STARTFIEN_SHIFT                  _EUSART_IEN_STARTF_SHIFT
#define _EUSART_IEN_STARTFIEN_MASK                   _EUSART_IEN_STARTF_MASK
#define _EUSART_IEN_STARTFIEN_DEFAULT                _EUSART_IEN_STARTF_DEFAULT
#define EUSART_IEN_STARTFIEN_DEFAULT                 EUSART_IEN_STARTF_DEFAULT

#define EUSART_IEN_SIGFIEN                           EUSART_IEN_SIGF
#define _EUSART_IEN_SIGFIEN_SHIFT                    _EUSART_IEN_SIGF_SHIFT
#define _EUSART_IEN_SIGFIEN_MASK                     _EUSART_IEN_SIGF_MASK
#define _EUSART_IEN_SIGFIEN_DEFAULT                  _EUSART_IEN_SIGF_DEFAULT
#define EUSART_IEN_SIGFIEN_DEFAULT                   EUSART_IEN_SIGF_DEFAULT

#define EUSART_IEN_AUTOBAUDDONEIEN                   EUSART_IEN_AUTOBAUDDONE
#define _EUSART_IEN_AUTOBAUDDONEIEN_SHIFT            _EUSART_IEN_AUTOBAUDDONE_SHIFT
#define _EUSART_IEN_AUTOBAUDDONEIEN_MASK             _EUSART_IEN_AUTOBAUDDONE_MASK
#define _EUSART_IEN_AUTOBAUDDONEIEN_DEFAULT          _EUSART_IEN_AUTOBAUDDONE_DEFAULT
#define EUSART_IEN_AUTOBAUDDONEIEN_DEFAULT           EUSART_IEN_AUTOBAUDDONE_DEFAULT

#endif // _SILICON_LABS_32B_SERIES_2_CONFIG_2

#endif
