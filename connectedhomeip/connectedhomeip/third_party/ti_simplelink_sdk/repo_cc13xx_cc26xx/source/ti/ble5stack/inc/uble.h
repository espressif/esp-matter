/******************************************************************************

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

/**
 *  @defgroup Micro_BLE_Stack Micro BLE Stack
 *  @brief This module implements the Micro BLE Stack
 *  For a detailed usage section describing how to send these commands and receive events,
 *  see the Micro BLE Stack Section of the User's Guide.
 *  @{
 *  @defgroup UBLE Stack Base
 *  @{
 *  @file  uble.h
 *  @brief  This file contains configuration, initialization,
 *          and parameter management of Micro BLE Stack. It also contains
 *          interfacing between Micro BLE Stack and the application.
 */

#ifndef UBLE_H
#define UBLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include <urfc.h>
#include <port.h>

/*-------------------------------------------------------------------
 * DEFINITIONS & CONSTANTS
 */

/** @defgroup UBLE_Constants Micro BLE Stack Constants
 * @{
 */

/** @defgroup UBLE_Params Micro BLE Stack Parameters
 * @{
 * @brief These are set via @ref uble_setParameter
 */
/// @cond NODOC
    #define UBLE_PARAM_COMMON_START       0
/// @endcond NODOC

/**
  * @brief Priority used for RF command scheduling
  *
  * @note This applies to dual mode only
  *
  * default: @ref UBLE_RF_PRI_NORMAL
  *
  * range: @ref UBLE_Radio_Priority
  */
    #define UBLE_PARAM_RFPRIORITY         (UBLE_PARAM_COMMON_START+0)

/**
  * @brief TX Power
  *
  * default: @ref TX_POWER_0_DBM
  *
  * range: @ref URF_TX_Power
  */
    #define UBLE_PARAM_TXPOWER            (UBLE_PARAM_COMMON_START+1)

/// @cond NODOC
    #define UBLE_PARAM_COMMON_END         UBLE_PARAM_TXPOWER
/// @endcond // NODOC
#if defined(FEATURE_ADVERTISER)
/// @cond NODOC
    #define UBLE_PARAM_ADV_CMN_START      (UBLE_PARAM_COMMON_END+1)
/// @endcond // NODOC

/**
  * @brief Advertising Interval (n * 0.625 ms)
  *
  * default: 1600
  *
  * range: @ref UBLE_MIN_ADV_INTERVAL - @ref UBLE_MAX_ADV_INTERVAL
  */
      #define UBLE_PARAM_ADVINTERVAL        (UBLE_PARAM_ADV_CMN_START+0)
/**
  * @brief Advertising Channel map
  *
  * default: @ref UBLE_ADV_CHAN_ALL
  *
  * range: @ref UBLE_Adv_Chans
  */
      #define UBLE_PARAM_ADVCHANMAP         (UBLE_PARAM_ADV_CMN_START+1)
/**
  * @brief Type of Advertising
  *
  * default: @ref UBLE_ADVTYPE_ADV_NC
  *
  * range: @ref UBLE_Adv_types
  */
      #define UBLE_PARAM_ADVTYPE            (UBLE_PARAM_ADV_CMN_START+2)
/**
  * @brief How early the application wants to get a notification before each advertising event happens (ms)
  *
  * The application is notified via the @ref ugapBcastAdvDoneCb_t callback.
  *
  * @note A value of 0 will disable the callback
  *
  * default: 0 (disabled)
  *
  * range: 0-256
  */
      #define UBLE_PARAM_TIMETOADV          (UBLE_PARAM_ADV_CMN_START+3)
/**
  * @brief Advertising Data
  *
  * default: {0}
  *
  * range: 0 - @ref UBLE_MAX_ADVDATA_LEN bytes
  */
      #define UBLE_PARAM_ADVDATA            (UBLE_PARAM_ADV_CMN_START+4)

/// @cond NODOC
    #define UBLE_PARAM_ADV_CMN_END        UBLE_PARAM_ADVDATA
/// @endcond NODOC
  #if defined(FEATURE_SCAN_RESPONSE)
/// @cond NODOC
    #define UBLE_PARAM_ADV_SR_START       (UBLE_PARAM_ADV_CMN_END+1)
/// @endcond NODOC

/**
  * @brief Scan response Data
  *
  * default: {0}
  *
  * range: 0 - @ref UBLE_MAX_ADVDATA_LEN bytes
  */
    #define UBLE_PARAM_SCANRSPDATA        (UBLE_PARAM_ADV_SR_START+0)
/**
  * @brief Advertising Filter Policy
  *
  * not implemented
  *
  * default: TBD
  *
  * range: TBD
  */
    #define UBLE_PARAM_ADVFLTPOLICY       (UBLE_PARAM_ADV_SR_START+1)
/// @cond NODOC
    #define UBLE_PARAM_ADV_SR_END         UBLE_PARAM_ADVFLTPOLICY
/// @endcond NODOC
  #else  /* FEATURE_SCAN_RESPONSE */
/// @cond NODOC
    #define UBLE_PARAM_ADV_SR_END         UBLE_PARAM_ADV_CMN_END
/// @endcond NODOC
  #endif /* FEATURE_SCAN_RESPONSE */
#else  /* FEATURE_ADVERTISER */
/// @cond NODOC
    #define UBLE_PARAM_ADV_SR_END         UBLE_PARAM_COMMON_END
/// @endcond NODOC
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
/// @cond NODOC
    #define UBLE_PARAM_SCAN_START         (UBLE_PARAM_ADV_SR_END+1)
/// @endcond NODOC
/**
 * @brief UB scan interval (n * 0.625 ms)
 *
 * default: @ref UBLE_PARAM_DFLT_SCANINTERVAL
 *
 * range: @ref UBLE_MIN_SCAN_INTERVAL - @ref UBLE_MAX_SCAN_INTERVAL
 */
    #define UBLE_PARAM_SCANINTERVAL       (UBLE_PARAM_SCAN_START+0)
/**
 * @brief The duration of the uLL scan (n * 0.625 ms)
 *
 * default: @ref UBLE_PARAM_DFLT_SCANWINDOW
 *
 * range: @ref UBLE_MIN_SCAN_WINDOW - @ref UBLE_MAX_SCAN_WINDOW
 */
    #define UBLE_PARAM_SCANWINDOW         (UBLE_PARAM_SCAN_START+1)
/**
 * @brief Scan Channel map
 *
 * default: @ref UBLE_ADV_CHAN_ALL
 *
 * range: @ref UBLE_Adv_Chans
 */
    #define UBLE_PARAM_SCANCHANMAP        (UBLE_PARAM_SCAN_START+2)
/**
 * @brief Scan Channel map
 *
 * default: TBD
 *
 * range: @ref UBLE_Adv_Chans
 */
    #define UBLE_PARAM_SCANTYPE           (UBLE_PARAM_SCAN_START+3)
/**
 * @brief Scan filter duplicate
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_FLTDUPLICATE       (UBLE_PARAM_SCAN_START+4)
/**
 * @brief Scanning filter policy
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_SCANFLTPOLICY      (UBLE_PARAM_SCAN_START+5)
/// @cond NODOC
    #define UBLE_PARAM_SCAN_END           UBLE_PARAM_SCANFLTPOLICY
/// @endcond NODOC
#else  /*FEATURE_SCANNER */
/// @cond NODOC
    #define UBLE_PARAM_SCAN_END           UBLE_PARAM_ADV_SR_END
/// @endcond NODOC
#endif /* FEATURE_SCANNER */
    #if defined(FEATURE_MONITOR)
/// @cond NODOC
    #define UBLE_PARAM_MONITOR_START         (UBLE_PARAM_ADV_SR_END+1)
/// @endcond NODOC

/**
 * @brief Monitor session ID. 1..4
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_SESSIONID         (UBLE_PARAM_MONITOR_START+0)
/**
 * @brief Monitor channel. 0..39
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_MONITOR_CHAN      (UBLE_PARAM_MONITOR_START+1)
/**
 * @brief Monitor duration is (n * 0.625 ms)
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_MONITOR_DURATION  (UBLE_PARAM_MONITOR_START+2)
/**
 * @brief Monitor start time
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_MONITOR_START_TIME  (UBLE_PARAM_MONITOR_START+3)
/**
 * @brief Monitor access address
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UBLE_PARAM_MONITOR_ACCESS_ADDR (UBLE_PARAM_MONITOR_START+4)
/// @cond NODOC
    #define UBLE_PARAM_MONITOR_END           UBLE_PARAM_MONITOR_ACCESS_ADDR
/// @endcond NODOC
#else  /* FEATURE_MONITOR */
/// @cond NODOC
    #define UBLE_PARAM_MONITOR_END           UBLE_PARAM_ADV_SR_END
/// @endcond NODOC
#endif /* FEATURE_MONITOR */
/// @cond NODOC
    #define UBLE_NUM_PARAM  (UBLE_PARAM_MONITOR_END+1)
/// @endcond NODOC
/** @} End UBLE_Params */

/** @defgroup UBLE_Radio_Priority Micro BLE Stack Radio Priorities
 * @{
 */
#define UBLE_RF_PRI_NORMAL      ((uint8) RF_PriorityNormal)  //!< Normal Priority
#define UBLE_RF_PRI_HIGH        ((uint8) RF_PriorityHigh)    //!< High Priority
#define UBLE_RF_PRI_HIGHEST     ((uint8) RF_PriorityHighest) //!< Highest Priority
/** @} End UBLE_Radio_Priority */

/** @defgroup UBLE_Radio_Criticality Micro BLE Stack Radio Criticality
 * @{
 */
#define RF_TIME_RELAXED    0    //!< Relaxed Timing
#define RF_TIME_CRITICAL   1    //!< Critical Timing
/** @} End UBLE_Radio_Criticality */

/** @brief Invalid TX Power Register value */
#define UBLE_TX_POWER_INVALID 0xFFFF

/** @defgroup UBLE_Address_types Micro BLE Stack Address Types
 * @{
 */
#define UBLE_ADDRTYPE_PUBLIC          0 //!< Public Address
#define UBLE_ADDRTYPE_STATIC          1 //!< Random Static Address
#define UBLE_ADDRTYPE_NONRESOLVABLE   2 //!< Random Private Nonresolvable Address
#define UBLE_ADDRTYPE_RESOLVABLE      3 //!< Random Private Resolvable Address
#define UBLE_ADDRTYPE_BD               4 //!< Device that is currently being used
/** @} End UBLE_Address_types */

/** @defgroup UBLE_Adv_types Micro BLE Stack Advertising Types
 * @{
 */
#define UBLE_ADVTYPE_ADV_NC   5   //!< associated with CMD_BLE_ADV_NC
#define UBLE_ADVTYPE_ADV_SCAN 6   //!< associated with CMD_BLE_ADV_SCAN
/** @} End UBLE_Adv_types */

/** @defgroup UBLE_Adv_Chans Micro BLE Stack Advertising Channel Masks
 * @{
 * Applicable to Advertising and Scanning events
 */
#define UBLE_ADV_CHAN_37   0x01 //!< Channel 37
#define UBLE_ADV_CHAN_38   0x02 //!< Channel 38
#define UBLE_ADV_CHAN_39   0x04 //!< Channel 39
#define UBLE_ADV_CHAN_ALL  (UBLE_ADV_CHAN_37 | UBLE_ADV_CHAN_38 | UBLE_ADV_CHAN_39)  //!< All Channels
/** @} End UBLE_Adv_Chans */

/* Min/Max for parameters */
#define UBLE_MIN_ADV_INTERVAL    160  //!< Minimum Advertising interval
#define UBLE_MAX_ADV_INTERVAL    16384 //!< Maximum Advertising Interval
#define UBLE_MIN_CHANNEL_MAP     UBLE_ADV_CHAN_37 //!< Minimum Channel Map
#define UBLE_MAX_CHANNEL_MAP     UBLE_ADV_CHAN_ALL    //!< Maximum Channel Map
#define UBLE_MIN_TX_POWER        TX_POWER_MINUS_21_DBM //!< -21 dBm
#define UBLE_MAX_TX_POWER        TX_POWER_5_DBM        //!<   5 dBm
#define UBLE_MIN_SCAN_INTERVAL   4 //!< Minimum Scanning interval
#define UBLE_MAX_SCAN_INTERVAL   16384 //!< Maximum Scanning Interval
#define UBLE_MIN_SCAN_WINDOW     4 //!< Minimum Scanning window
#define UBLE_MAX_SCAN_WINDOW     16384 //!< Maximum Scanning window
#define UBLE_MIN_MONITOR_HANDLE    0 //!< Minimum Monitor handle
#define UBLE_MAX_MONITOR_HANDLE    8 //!< Maximum Monitor handle
#define UBLE_MIN_MONITOR_CHAN      0 //!< Minimum Monitor channel
#define UBLE_MAX_MONITOR_CHAN      39 //!< Maximum Monitor channel
#define UBLE_MIN_MONITOR_DURATION  0 //!< Minimum Monitor duration
#define UBLE_MAX_MONITOR_DURATION  64000 //!< Maximum Monitor duration

/* Default Parameter Values */
#define UBLE_PARAM_DFLT_RFPRIORITY      UBLE_RF_PRI_NORMAL  //!< RF_PriorityNormal
#define UBLE_PARAM_DFLT_RFTIMECRIT      RF_TIME_RELAXED   //!< Time-relaxed
#define UBLE_PARAM_DFLT_TXPOWER         TX_POWER_0_DBM    //!< 0 dBm
#define UBLE_PARAM_DFLT_ADVINTERVAL     1600              //!< 1 sec
#define UBLE_PARAM_DFLT_ADVCHANMAP      UBLE_ADV_CHAN_ALL   //!< All Channels
#define UBLE_PARAM_DFLT_ADVTYPE         UBLE_ADVTYPE_ADV_NC //!< ADV_NONCONN_IND
#define UBLE_PARAM_DFLT_TIMETOADV       0                 //!< Disabled
#define UBLE_PARAM_DFLT_ADVDATA         {0}               //!< No data
#define UBLE_PARAM_DFLT_SCANRSPDATA     {0}               //!< No data
#define UBLE_PARAM_DFLT_ADVFLTPOLICY    0                 //!< Filter policy TBD
#define UBLE_PARAM_DFLT_SCANINTERVAL    16                //!< Scan interval
#define UBLE_PARAM_DFLT_SCANWINDOW      16                //!< Scan window
#define UBLE_PARAM_DFLT_SCANCHAN        UBLE_ADV_CHAN_ALL   //!< All Channels
#define UBLE_PARAM_DFLT_SCANTYPE        0                 //!< Scan type
#define UBLE_PARAM_DFLT_FLTDUPLICATE    0                 //!< Filter duplicate TBD
#define UBLE_PARAM_DFLT_SCANFLTPOLICY   0                 //!< Scan filter policy TBD
#define UBLE_PARAM_DFLT_MONITOR_HANDLE    0               //!< Monitor session handle
#define UBLE_PARAM_DFLT_MONITOR_CHAN      37              //!< Channel monitored
#define UBLE_PARAM_DFLT_MONITOR_DURATION  320             //!< Monitor duration 200 ms
#define UBLE_PARAM_DFLT_MONITOR_START_TIME  6400          //!< Monitor start time 4 s
#define UBLE_PARAM_DFLT_MONITOR_ACCESS_ADDR 0x8E89BED6    //!< Adv address
#define UBLE_PARAM_DFLT_MONITOR_CRCINIT     0x555555      //!< crcInit value


/**
 * Event Message Destination
 */
#define UBLE_EVTDST_LL       0      //!< LL
#define UBLE_EVTDST_GAP      1      //!< GAP
#define UBLE_NUM_EVTDST      2      //!< Number of Event Destinations

/**
 * Unit conversions
 */
/** @brief 1 us = 4 RAT tick */
#define US_TO_RAT        4
/** @brief 1 ms = 4000 RAT tick */
#define MS_TO_RAT        4000
/** @brief 1 BLE tick(0.625 ms) = 2500 RAT tick */
#define BLE_TO_RAT       2500
/** @brief 1 system tick = (Clock_tickPeriod * US_TO_RAT) RAT tick */
#define SYSTICK_TO_RAT   (Clock_tickPeriod * US_TO_RAT)
/** @brief 1 ms = (1000 / Clock_tickPeriod) system tick */
#define MS_TO_SYSTICK    (1000 / Clock_tickPeriod)
  /** @brief 0.625 ms = (625 / Clock_tickPeriod) system tick */
#define BLE_TO_SYSTICK   (625 / Clock_tickPeriod)
/** @brief 1 byte = 8 us = 32 RAT tick, assuming 1 Mbps PHY */
#define BYTE_TO_RAT      (8 * US_TO_RAT)

/* Length of ADV Data */
#define UBLE_MAX_ADVDATA_LEN         B_MAX_ADV_LEN  //!< Max Advertising Data Length

#define UBLE_MAX_OVERLAP_TIME_LIMIT     0x7270E000  // 8 minutes in 625us ticks. chosen to be half of 17 minutes RF overlap.
#define UBLE_MAX_32BIT_TIME             0xFFFFFFFF

#define BLE_ROLE_PERIPHERAL  4
#define BLE_ROLE_CENTRAL     8

/** @} End UBLE_Constants */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

/** @defgroup UBLE_Structures Micro BLE Stack Structures
 * @{
 */

/** @brief Address Type */
typedef uint8 ubleAddrType_t;

/** @brief Recipient of Micro BLE Stack Event */
typedef uint8 ubleEvtDst_t;

/** @brief Micro BLE Stack Event */
typedef uint8 ubleEvt_t;

PACKED_TYPEDEF_STRUCT {
  port_queueElem_t pElem;  //!< queue element pointer
  ubleEvtDst_t     dst;    //!< destination
  ubleEvt_t        evt;    //!< event
} ubleEvtHdr_t;  //!< Type of Micro BLE Stack event header

/** @brief Type of Micro BLE Stack message */
typedef uint8 ubleMsg_t;

PACKED_TYPEDEF_STRUCT {
  ubleEvtHdr_t  hdr;      //!< header
  ubleMsg_t     msg[];    //!< message
} ubleEvtMsg_t; //!< Type of Micro BLE Stack event message

/** @} End UBLE_Structures */

/** @defgroup UBLE_Callbacks Micro BLE Stack Callbacks
 * @{
 */

/**
 * @brief Type of the callback to have @ref uble_processMsg called in the application
 * task context.
 *
 * @warning The application should provide a valid callback when calling
 * @ref uble_stackInit
 */
typedef void (*ublePostEvtProxyCB_t)(void);

/**
 * @brief Type of the callback to switch antenna in the application.
 *
 * @ref uble_registerAntSwitchCB
 */
typedef void (*ubleAntSwitchCB_t)(void);

/**
 * @brief Function to process event messages destined to the calle module.
 */
typedef void (*ubleProcessMsg_t)(ubleEvtMsg_t *pEvtMsg);

/** @} End UBLE_Callbacks */

/// @cond NODOC
/** @brief Type of Micro BLE Stack Parameters */
PACKED_TYPEDEF_STRUCT {
  uint8  rfPriority;              //!< RF Priority
  int8   txPower;                 //!< TX Power in dBm
#if defined(FEATURE_ADVERTISER)
  uint16 advInterval;             //!< Advertising Interval. 0.625 ms unit. Range: 0x0020 to 0x4000
  uint8  advChanMap;              //!< Advertising Channel Map. 37, 38 and 39 correspond to bit 0, 1 and 2
  uint8  advType;                 //!< Advertising Type - ADV_NONCONN_IND or ADV_SCAN_IND
  uint8  timeToAdv;               //!< How early to notify before the next advertising in ms
  uint8  advData[UBLE_MAX_ADVDATA_LEN];     //!< Advertising Data
#if defined(FEATURE_SCAN_RESPONSE)
  uint8  scanRspData[UBLE_MAX_ADVDATA_LEN];   //!< Scan Response Data
  uint8  advFltPolicy;            //!< Advertising Filter Policy
#endif /* FEATURE_SCAN_RESPONSE */
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
  uint16 scanInterval;            //!< Scan Interval. 0.625 ms unit. Range: 0x0004 to 0x4000
  uint16 scanWindow;              //!< Scan Window. 0.625 ms unit. Range: 0x0004 to 0x4000
  uint8  scanChanMap;             //!< Scanning Channel Map. 37, 38 and 39 correspond to bit 0, 1 and 2
  uint8  scanType;                //!< Scanning Type - SCAN_PASSIVE or SCAN_ACTIVE
  uint8  fltDuplicate;            //!< Whether to filter out duplicate advertising report or not
  uint8  scanFltPolicy;           //!< Scan Filter Policy
#endif /* FEATURE_SCANNER */
#if defined(FEATURE_MONITOR)
  uint8  monitorHandle;           //!< Monitor handle. Range: 1..CM_MAX_SESSIONS
  uint8  monitorChan;             //!< Monitor channel. Range: 0..39
  uint16 monitorDuration;         //!< Monitor duration. 0.625 ms unit. Range: 0x0004 to 0x4000
  uint32 startTime;               //!< Monitor start time in RAT ticks
  uint32 accessAddr;              //!< Monitor access address
  uint32 crcInit;
#endif /* FEATURE_MONITOR */
} ubleParams_t;

/**
 * @brief Lookup for a parameter.
 *
 * @note Min/max work only for uint8-type parameters
 */
PACKED_TYPEDEF_STRUCT {
  uint8  offset;
  uint8  len;
  uint8  min;
  uint8  max;
} ubParamLookup_t;
/// @endcond // NODOC

/*-------------------------------------------------------------------
 * API FUNCTIONS
 */

/**
 * @brief  Initialize Micro BLE Stack
 *
 * @param  addrType - @ref UBLE_Address_types <br>
 *             Type of the address which the device will use initially.
 *             Either @ref UBLE_ADDRTYPE_PUBLIC or @ref UBLE_ADDRTYPE_STATIC . If addrType is
 *             @ref UBLE_ADDRTYPE_PUBLIC , the public address which is stored in CCFG
 *             preferably or in FCFG otherwise will be used for BD Addr. If
 *             addrType is @ref UBLE_ADDRTYPE_STATIC , BD Addr will depend on pBDAddr.
 * @param  pStaticAddr - If this is NULL and addrType is @ref UBLE_ADDRTYPE_STATIC , a
 *             random static address will be generated by the stack and will be
 *             used for BD Address. If this is not NULL and addrType is
 *             @ref UBLE_ADDRTYPE_STATIC , the 6 byte-long data pointed to by pBDAddr
 *             will be used for BD Address as the random static address.
 * @param  pfnPostEvtProxyCB - Callback to trigger a context switch to the
 *             application task so that the messages queued by the stack can be
 *             processed by @ref uble_processMsg in the application task context.
 *             @warning This callback MUST be provided
 *             by the application. The role of the callback is to post a RTOS
 *             event or semaphore to the application itself so that when
 *             the application task is activated it can call @ref uble_processMsg
 *             in the application task context.
 * @param  timeCritical - rf transmit operation. For strict timing on the
 *             transmission, set value to RF_TIME_CRITICAL. For relaxed timing
 *             on the transmission set value to RF_TIME_RELAXED.
 *
 * @return @ref SUCCESS - success
 * @return @ref INVALIDPARAMETER - addrType is invalid or pfnPostEvtProxyCB is NULL
 */
bStatus_t uble_stackInit(ubleAddrType_t addrType, uint8* pStaticAddr,
                         ublePostEvtProxyCB_t pfnPostEvtProxyCB, uint8 timeCritical);

/**
 * @brief   This function determines if the first time parameter is greater
 *          than the second time parameter, taking timer counter wrap into
 *          account. If so, TRUE is returned.
 *
 * @param  time1 - First time.
 * @param  time2 - Second time.
 *
 * @return  TRUE:  When first parameter is greater than second.
 *          FALSE: When first parmaeter is less than or equal to
 *                 the second.
 */
uint8 uble_timeCompare(uint32 time1, uint32 time2);

/**
 * @brief   This function determines the difference between two time
 *          parameters, taking timer counter wrap into account.
 *
 * @param   time1 - First time.
 * @param   time2 - Second time.
 *
 * @return  Difference between time1 and time2.
 */
uint32 uble_timeDelta(uint32 time1, uint32 time2);

/**
 * @brief  Set a Micro BLE Stack parameter.
 *
 * @param   param - parameter ID: @ref UBLE_Params
 * @param   len - length of data to write
 * @param   pValue - pointer to data to write. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref bleInvalidRange
 */
bStatus_t uble_setParameter(uint8 param, uint8 len, void *pValue);

/**
 * @brief  Get a Micro BLE Stack parameter.
 *
 * @param   param - parameter ID: @ref UBLE_Params
 * @param   pValue - pointer to location to get the value. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
bStatus_t uble_getParameter(uint8 param, void *pValue);

/**
 * @brief   Check if the given address is valid
 *
 * @param   addrType - @ref UBLE_ADDRTYPE_PUBLIC , @ref UBLE_ADDRTYPE_STATIC ,
 *                     @ref UBLE_ADDRTYPE_NONRESOLVABLE , or @ref UBLE_ADDRTYPE_RESOLVABLE
 * @param   pAddr    - pointer to where the address to check resides
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref FAILURE
 */
bStatus_t uble_checkAddr(ubleAddrType_t addrType, uint8 *pAddr);

/**
 * @brief  Make BD Addr revert to Random Static Address if exists or
 *         Public Address otherwise.
 */
void uble_resetAddr(void);

/**
 * @brief  Retrieve the address corresponding to the given address type
 *
 * @param addrType - @ref UBLE_ADDRTYPE_PUBLIC , @ref UBLE_ADDRTYPE_STATIC or @ref UBLE_ADDRTYPE_BD
 * @param pAddr pointer to memory location to return address. This should be a uint8_t[6]
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref FAILURE
 */
bStatus_t uble_getAddr(ubleAddrType_t addrType, uint8* pAddr);

/**
 * @brief  Register callback for antenna switch
 *
 * @param  pfnAntSwitchCB - application antenna switch callback
 *
 * @return SUCCESS
 */
bStatus_t uble_registerAntSwitchCB(ubleAntSwitchCB_t pfnAntSwitchCB);

/**
 * @brief  Let the Micro Stack process the queued messages.
 *
 * @warning The application MUST call this function when it
 *         process the event posted by the stack event proxy function registered
 *         through @ref uble_stackInit
 */
void uble_processMsg(void);

/*-------------------------------------------------------------------
 * BLE_LOG FUNCTIONS
 */
//#define BLE_LOG
#ifdef BLE_LOG

#define BLE_LOG_MODULE_CTRL            0x01
#define BLE_LOG_MODULE_HOST            0x02
#define BLE_LOG_MODULE_APP             0x04
#define BLE_LOG_MODULE_OSAL_TASK       0x08
#define BLE_LOG_MODULE_RF_CMD          0x10

void bleLog_int_int(void *handle, uint32_t type, uint8_t *format, uint32_t param1, uint32_t param2);
void bleLog_int_str(void *handle, uint32_t type, uint8_t *format, uint32_t param1, char *param2);
void bleLog_int_time(void *handle, uint32_t type, uint8_t *start_str, uint32_t param1);

#define BLE_LOG_INT_INT(handle, type, format, param1, param2)         bleLog_int_int(handle, type, format, param1, param2)
#define BLE_LOG_INT_STR(handle, type, format, param1, param2)         bleLog_int_str(handle, type, format, param1, param2)
#define BLE_LOG_INT_TIME(handle, type, start_str, param1)             bleLog_int_time(handle, type, start_str, param1)
#else
#define BLE_LOG_INT_INT(handle, type, format, param1, param2)
#define BLE_LOG_INT_STR(handle, type, format, param1, param2)
#define BLE_LOG_INT_TIME(handle, type, start_str, param1)

#endif //BLE_LOG

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UBLE_H */

/** @} End UBLE */

/** @} End Micro_BLE_Stack */
