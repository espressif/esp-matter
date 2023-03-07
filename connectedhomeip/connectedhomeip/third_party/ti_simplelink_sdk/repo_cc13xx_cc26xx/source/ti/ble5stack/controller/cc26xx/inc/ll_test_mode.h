/******************************************************************************

 @file  ll_test_mode.h

 @brief This file contains the Link Layer (LL) types, constants,
        API's etc. to support LL-only Test Mode.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef LL_TEST_MODE_H
#define LL_TEST_MODE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "comdef.h"
#include "ll.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// Test Mode
#define NUM_BYTES_FOR_CHAN_MAP               5
#define ENC_RAND_LEN                         8
#define ENC_EDIV_LEN                         2
#define ENC_LTK_LEN                          16
#define ENC_IV_LEN                           8
#define ENC_SKD_LEN                          16
#define ENC_SK_LEN                           16

/*******************************************************************************
 * TYPEDEFS
 */

/*
** Test Mode Data
*/
typedef struct
{
  // Encryption stuff
  struct
  {
    uint8 IV[ ENC_IV_LEN ];                         // combined master and slave IV values concatenated
    uint8 SKD [ ENC_SKD_LEN ];                      // combined master and slave SKD values concatenated
    uint8 RAND[ ENC_RAND_LEN ];                     // random vector from Master
    uint8 EDIV[ ENC_EDIV_LEN ];                     // encrypted diversifier from Master
    uint8 SK[ ENC_SK_LEN ];                         // session key derived from LTK and SKD
    uint8 LTK[ ENC_LTK_LEN ];
  } encInfo;

  // Master stuff
  uint8  winSize;                                    // specify a window size value
  uint16 winOffset;                                  // specify a window offset value
  uint8  updateChanMap[ NUM_BYTES_FOR_CHAN_MAP ];    // specify a data channel map update
  uint8  SCA;                                        // specify a sleep clock accuracy
  uint16 instant;                                    // specify the number of events before a control update takes place
  uint8  scanChanOverride;                           // flag to indicate scan channel is being overridden
  uint8  scanChanValue;                              // scan channel to be used during an override
  uint8  flipTxMIC;                                  // flag to indicate a MIC error should be forced

  // Slave stuff

} testInfo_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * Test Callbacks
 */

/*******************************************************************************
 * @fn          LL_TEST_ConnectionEventCback Callback
 *
 * @brief       This Callback is used by the LL for test support. It provides
 *              the current connection ID and the current event count for test
 *              software.
 *
 * input parameters
 *
 * @param       connId       - The current connection ID.
 * @param       currentEvent - The current event count.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void LL_TEST_ConnectionEventCback( uint16 connId,
                                          uint16 currentEvent );


/*******************************************************************************
 * @fn          LL_TEST_AdvEventCback Callback
 *
 * @brief       This Callback is used by the LL for test support. It provides
 *              an opportunity for the test software to make updates while
 *              advertising.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void LL_TEST_AdvEventCback( void );


/*******************************************************************************
 * @fn          LL_TEST_ScanEventCback Callback
 *
 * @brief       This Callback is used by the LL for test support. It provides
 *              an opportunity for the test software to make updates while
 *              scanning.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void LL_TEST_ScanEventCback( void );


/*******************************************************************************
 * @fn          LL_TEST_PostScanEventCback Callback
 *
 * @brief       This Callback is used by the LL for test support. It provides
 *              an opportunity for the test software to make updates while
 *              scanning after the Task Done post-processing has completed.
 *              This is necessary for test cases that require the scanner to
 *              stop then start again, which should not be done within the
 *              context of Task Done post-processing.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void LL_TEST_PostScanEventCback( void );


#ifdef __cplusplus
}
#endif

#endif /* LL_TEST_MODE_H */

