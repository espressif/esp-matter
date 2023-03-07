/******************************************************************************
*  Filename:       rf_bt5_coex.h
*
*  Description:    CC13x2/CC26x2 structures for bt5 coexistence support
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

#ifndef __BT5_COEX_H
#define __BT5_COEX_H

//! \addtogroup rfCoreHal
//! @{

//! \addtogroup bt5_coex
//! @{

#include <stdint.h>
#include "rf_mailbox.h"

// Error code for GRANT signal not given
#define BLE_ERROR_NO_GRANT	0x1808

typedef struct  rfCoreHal_bleCoExConfig_s rfCoreHal_bleCoExConfig_t;


//! \addtogroup bleCoExConfig
//! @{
struct  rfCoreHal_bleCoExConfig_s {
   struct {
      uint8_t bCoExEnable:1;            //!< \brief 0: CoEx disabled
                                        //!<        1: CoEx enabled
      uint8_t bUseREQUEST:1;            //!< \brief 0: REQUEST signal inactive
                                        //!<        1: REQUEST signal active
      uint8_t bUseGRANT:1;              //!< \brief 0: GRANT signal inactive
                                        //!<        1: GRANT signal active
      uint8_t bUsePRIORITY:1;           //!< \brief 0: PRIORITY signal inactive
                                        //!<        1: PRIORITY signal active
      uint8_t bRequestForChain:1;       //!< \brief 0: Deassert REQUEST after each RF command
                                        //!<        1: Keep REQUEST asserted for entire command chain
   } coExEnable;
   uint8_t coExTxRxIndication;          //!< \brief 0 = RX indication is 0, TX indication is 0
                                        //!<        1 = RX indication is 0, TX indication is 1
                                        //!<        2 = RX indication is 1, TX indication is 0
                                        //!<        3 = RX indication is 1, RX indication is 1
   uint16_t priorityIndicationTime;     //!<        Time (in us) that the PRIORITY signal will indicate the priority.
   struct {
      uint8_t bRequestAsserted:1;       //!< \brief READ ONLY. 0 = REQUEST not asserted, 1 = REQUEST is asserted.
                                        //!<        Will indicate if REQUEST would have been asserted, except if signal is not used.
      uint8_t bIgnoreGrantInRxAsserted:1;//!< \brief READ ONLY. 0 = GRANT is checked in RX, 1 = GRANT is not checked in RX
                                        //!<        Will indicate if the current running command is ignoring GRANT in RX
   } rfCoreCoExStatus;
   struct {
      uint8_t bUseOverridePriority:1;   //!< \brief Bit to override default priority
                                        //!<        0: Use default priority
                                        //!<        1: use overridePriority priority for entire chain
      uint8_t overridePriority:1;       //!< \brief Priority to use if priority is overridden
                                        //!<        0: Low priority if bUseOverridePriority = 1
                                        //!<        1: High priority if bUseOverridePriority = 1
      uint8_t bUseOverrideRequestForRx:1;//!< \brief Bit to override default request for RX
                                        //!<        0: Use default request for RX
                                        //!<        1: use overrideRequestForRx for entire chain
      uint8_t overrideRequestForRx:1;   //!< \brief REQUEST signal override if bUseOverrideRequestForRx = 1, used for entire chain
                                        //!<        0: Don't request for RX if bUseOverrideRequestForRx = 1
                                        //!<        1: Request for RX if bUseOverrideRequestForRx = 1
   } overrideConfig;
   struct {
      uint8_t defaultPriority:1;        //!< \brief 0: Default low priority
                                        //!<        1: Default high priority
      uint8_t bAssertRequestForRx:1;    //!< \brief Default "request for RX" behaviour
                                        //!<        0: Assert REQUEST for TX operations only
                                        //!<        1: Assert REQUEST for both RX and TX operations
      uint8_t bIgnoreGrantInRx:1;       //!< \brief 0: Check GRANT in RX and TX
                                        //!<        1: Ignore GRANT in RX, check GRANT in TX. Independent of asserting REQUEST for RX.
      uint8_t bKeepRequestIfNoGrant:1;  //!< \brief 0: Deassert REQUEST if GRANT was not given
                                        //!<        1: Keep REQUEST asserted if no GRANT was given
   } cmdBleMasterSlaveConfig;
   struct {
      uint8_t defaultPriority:1;        //!< \brief 0: Default low priority
                                        //!<        1: Default high priority
      uint8_t bAssertRequestForRx:1;    //!< \brief Default "request for RX" behaviour
                                        //!<        0: Assert REQUEST for TX operations only
                                        //!<        1: Assert REQUEST for both RX and TX operations
      uint8_t bIgnoreGrantInRx:1;       //!< \brief 0: Check GRANT in RX and TX
                                        //!<        1: Ignore GRANT in RX, check GRANT in TX. Independent of asserting REQUEST for RX.
      uint8_t bKeepRequestIfNoGrant:1;  //!< \brief 0: Deassert REQUEST if GRANT was not given
                                        //!<        1: Keep REQUEST asserted if no GRANT was given
   } cmdBleAdvConfig;
   struct {
      uint8_t defaultPriority:1;        //!< \brief 0: Default low priority
                                        //!<        1: Default high priority
      uint8_t bAssertRequestForRx:1;    //!< \brief Default "request for RX" behaviour
                                        //!<        0: Assert REQUEST for TX operations only
                                        //!<        1: Assert REQUEST for both RX and TX operations
      uint8_t bIgnoreGrantInRx:1;       //!< \brief 0: Check GRANT in RX and TX
                                        //!<        1: Ignore GRANT in RX, check GRANT in TX. Independent of asserting REQUEST for RX.
      uint8_t bKeepRequestIfNoGrant:1;  //!< \brief 0: Deassert REQUEST if GRANT was not given
                                        //!<        1: Keep REQUEST asserted if no GRANT was given
   } cmdBleScanConfig;
   struct {
      uint8_t defaultPriority:1;        //!< \brief 0: Default low priority
                                        //!<        1: Default high priority
      uint8_t bAssertRequestForRx:1;    //!< \brief Default "request for RX" behaviour
                                        //!<        0: Assert REQUEST for TX operations only
                                        //!<        1: Assert REQUEST for both RX and TX operations
      uint8_t bIgnoreGrantInRx:1;       //!< \brief 0: Check GRANT in RX and TX
                                        //!<        1: Ignore GRANT in RX, check GRANT in TX. Independent of asserting REQUEST for RX.
      uint8_t bKeepRequestIfNoGrant:1;  //!< \brief 0: Deassert REQUEST if GRANT was not given
                                        //!<        1: Keep REQUEST asserted if no GRANT was given
   } cmdBleInitConfig;
   struct {
      uint8_t defaultPriority:1;        //!< \brief 0: Default low priority
                                        //!<        1: Default high priority
      uint8_t bAssertRequestForRx:1;    //!< \brief Default "request for RX" behaviour
                                        //!<        0: Assert REQUEST for TX operations only
                                        //!<        1: Assert REQUEST for both RX and TX operations
      uint8_t bIgnoreGrantInRx:1;       //!< \brief 0: Check GRANT in RX and TX
                                        //!<        1: Ignore GRANT in RX, check GRANT in TX. Independent of asserting REQUEST for RX.
      uint8_t bKeepRequestIfNoGrant:1;  //!< \brief 0: Deassert REQUEST if GRANT was not given
                                        //!<        1: Keep REQUEST asserted if no GRANT was given
   } cmdBleGenericRxConfig;
   struct {
      uint8_t defaultPriority:1;        //!< \brief 0: Default low priority
                                        //!<        1: Default high priority
      uint8_t bAssertRequestForRx:1;    //!< \brief Default "request for RX" behaviour
                                        //!<        0: Assert REQUEST for TX operations only
                                        //!<        1: Assert REQUEST for both RX and TX operations
      uint8_t bIgnoreGrantInRx:1;       //!< \brief 0: Check GRANT in RX and TX
                                        //!<        1: Ignore GRANT in RX, check GRANT in TX. Independent of asserting REQUEST for RX.
      uint8_t bKeepRequestIfNoGrant:1;  //!< \brief 0: Deassert REQUEST if GRANT was not given
                                        //!<        1: Keep REQUEST asserted if no GRANT was given
   } cmdBleTxTestConfig;
   uint8_t grantLatencyTime;            //!< \brief Reserved for future use. Writes to this variable have no effect.
} ;

//! @}

//! @}
//! @}
#endif
