/******************************************************************************
*  Filename:       rf_bt5_iq_autocopy.h
*
*  Description:    CC13x2/CC26x2 API for IQ Sample automatic copy in BLE
*
*  Copyright (c) 2015 - 2020, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __RF_BT5_IQ_AUTOCOPY_H
#define __RF_BT5_IQ_AUTOCOPY_H

#ifndef __RFC_STRUCT
#define __RFC_STRUCT
#endif

#ifndef __RFC_STRUCT_ATTR
#if defined(__GNUC__)
#define __RFC_STRUCT_ATTR __attribute__ ((aligned (4)))
#elif defined(__TI_ARM__)
#define __RFC_STRUCT_ATTR __attribute__ ((__packed__,aligned (4)))
#else
#define __RFC_STRUCT_ATTR
#endif
#endif

//! \addtogroup rfc
//! @{

//! \addtogroup rf_bt5_iq_autocopy
//! @{

#include <stdint.h>
#include "rf_mailbox.h"

#define IRQN_CTE_SAMPLES_AUTOCOPIED 15
#define IRQ_CTE_SAMPLES_AUTOCOPIED  (1U << IRQN_CTE_SAMPLES_AUTOCOPIED)

typedef struct __RFC_STRUCT rfc_iqAutoCopyDef_s rfc_iqAutoCopyDef_t;
typedef struct __RFC_STRUCT rfc_statusIqSamplesEntry_s rfc_statusIqSamplesEntry_t;

//! \addtogroup iqAutoCopyDef
//! @{
struct __RFC_STRUCT rfc_iqAutoCopyDef_s {
   struct {
      uint8_t bFlushCrcErr:1;           //!< \brief 0: Report samples regardless of CRC result<br>
                                        //!<        1: Do not report samples from packets with CRC error
      uint8_t bFlushCteInfoErr:1;       //!< \brief 0: Report samples from packets with any CTEInfo<br>
                                        //!<        1: Do not report samples from packets with invalid CTEInfo
      uint8_t bFlushAoa:1;              //!< \brief 0: Report samples from AoA packets<br>
                                        //!<        1: Do not report samples from AoA packets
      uint8_t bFlushAod1us:1;           //!< \brief 0: Report samples from AoD packets with 1 us slots<br>
                                        //!<        1: Do not report samples from AoD packets with 1 us slots
      uint8_t bFlushAod2us:1;           //!< \brief 0: Report samples from AoD packets with 2 us slots<br>
                                        //!<        1: Do not report samples from AoD packets with 2 us slots
      uint8_t bIncludeRfGain:1;         //!< \brief 0: Report gain as single-bit value in status field only<br>
                                        //!<        1: Also report an explicit gain byte in dB
      uint8_t bIncludeRssi:1;           //!< \brief 0: Don't report RSSI in status field<br>
                                        //!<        1: Report RSSI in status field
   } samplesConfig;
   uint8_t minReportSize;               //!<        Minimum value of CTETime for packets to report
   uint8_t maxReportSize;               //!<        Maximum value of CTETime for packets to report
   uint8_t cteCopyLimitCount;           //!< \brief Number of CTE that should be recieved.
                                        //!<        0x00: Do not copy any CTE, CTE will be flushed
                                        //!<        0x01 - 0xFE: Copy CTE and decrement this counter
                                        //!<        0xFF: Copy CTE and do not decrement this counter
   uint32_t* pSamplesQueue;             //!<        Pointer to queue for copying samples. 0: Do not copy samples
   uint8_t numPktCopied;                //!<        Number of packets copied. Updated by CM0.
   uint8_t numPktFlushed;               //!<        Number of packets not copied due to the settings in samplesConfig. Updated by CM0.
   uint8_t numPktBufFull;               //!<        Number of packets not copied because no buffer was available. Updated by CM0.
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup statusIqSamplesEntry
//! @{
struct __RFC_STRUCT rfc_statusIqSamplesEntry_s {
   struct {
      uint8_t bCrcErr:1;                //!<        1 if the samples came from a packet received with CRC error; 0 otherwise
      uint8_t bCopyTrunk:1;             //!<        1 if the entry was too short to hold all the samples in RF core RAM; 0 otherwise
      uint8_t bSamplTrunk:1;            //!<        1 if the entry contains samples of a CTE that was too long for the available RF core RAM; 0 otherwise
      uint8_t rfRamInfo:2;              //!< \brief 0: Reserved<br>
                                        //!<        1: Packet was contained in MCE RAM only<br>
                                        //!<        2: Packet was contained in RFE RAM only<br>
                                        //!<        3: Packet was contained in both RAMs<br>
      uint8_t rfPhy:1;                  //!< \brief PHY of the packet containing the samples<br>
                                        //!<        0: 1 Mbps<br>
                                        //!<        1: 2 Mbps
      uint8_t rfGainStep:1;             //!< \brief RF gain of the received samples<br>
                                        //!<        0: High gain<br>
                                        //!<        1: Low gain
   } status;
} __RFC_STRUCT_ATTR;

//! @}

//! @}
//! @}
#endif
