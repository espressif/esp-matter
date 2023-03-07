/******************************************************************************

 @file  uble.c

 @brief This file contains configuration, initialization,
        and parameter management of Micro BLE Stack. It also contains
        interfacing between Micro BLE Stack and the application.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h>

#include <driverlib/ioc.h>

#include <ti/drivers/rf/RF.h>

#include <bcomdef.h>
#include <ll_common.h>

#include <port.h>
#include <urfi.h>
#include <uble.h>
#include <ull.h>
#include <ugap.h>


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/* Device Address (Little Endian) */
uint8   ubleBDAddr[6];

#if defined(FEATURE_STATIC_ADDR)
/* Random Static Address (Little Endian) */
uint8   ubRSAddr[6];
#endif /* FEATURE_STATIC_ADDR */
/* RF time critical or relaxed operation */
uint8 rfTimeCrit = RF_TIME_CRITICAL;
/* Application registered callback to handle antenna switch */
ubleAntSwitchCB_t ubleAntSwitchSel = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern RF_RadioSetup urSetup;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
void uble_processLLMsg(ubleEvtMsg_t *pEvtMsg);
void uble_processGAPMsg(ubleEvtMsg_t *pEvtMsg);

/*********************************************************************
 * LOCAL VARIABLES
 */

ubleParams_t ubleParams = {
  UBLE_PARAM_DFLT_RFPRIORITY,             /* rfPriority */
  UBLE_PARAM_DFLT_TXPOWER,                /* txPower */
#if defined(FEATURE_ADVERTISER)
  UBLE_PARAM_DFLT_ADVINTERVAL,            /* advInterval */
  UBLE_PARAM_DFLT_ADVCHANMAP,             /* advChanMap */
  UBLE_PARAM_DFLT_ADVTYPE,                /* advType */
  UBLE_PARAM_DFLT_TIMETOADV,              /* timeToAdv */
  UBLE_PARAM_DFLT_ADVDATA,                /* advData */
  #if defined(FEATURE_SCAN_RESPONSE)
  UBLE_PARAM_DFLT_SCANRSPDATA,            /* scanRspData */
  UBLE_PARAM_DFLT_ADVFILTPOLICY,          /* advFiltPolicy */
  #endif /* FEATURE_SCAN_RESPONSE */
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
  UBLE_PARAM_DFLT_SCANINTERVAL,           /* scanInterval */
  UBLE_PARAM_DFLT_SCANWINDOW,             /* scanWindow */
  UBLE_PARAM_DFLT_SCANCHAN,               /* scanChanMap */
  UBLE_PARAM_DFLT_SCANTYPE,               /* scanType */
  UBLE_PARAM_DFLT_FLTDUPLICATE,           /* fltDuplicate */
  UBLE_PARAM_DFLT_SCANFLTPOLICY,          /* scanFltPolicy */
#endif /* FEATURE_SCANNER */
#if defined(FEATURE_MONITOR)
  UBLE_PARAM_DFLT_MONITOR_HANDLE,         /* monitorHandle */
  UBLE_PARAM_DFLT_MONITOR_CHAN,           /* monitorChan */
  UBLE_PARAM_DFLT_MONITOR_DURATION,       /* monitorDuration */
  UBLE_PARAM_DFLT_MONITOR_START_TIME,     /* startTime */
  UBLE_PARAM_DFLT_MONITOR_ACCESS_ADDR,    /* access address */
  UBLE_PARAM_DFLT_MONITOR_CRCINIT         /* crcInit */
#endif /* FEATURE_MONITOR */
};

/* Parameter lookup table.
 * If min is not smaller than max, a tailored range check code for that
 * parameter shall be added to uble_setParam().
 * Note that both min and max are in uint8.
 */
const static ubParamLookup_t ubParamLookup[] = {
    { offsetof(ubleParams_t, rfPriority),  sizeof(uint8),          \
      UBLE_RF_PRI_NORMAL,                  UBLE_RF_PRI_HIGHEST },
    { offsetof(ubleParams_t, txPower),     sizeof(int8),           \
      0,                                   0 }, /* check separately */
#if defined(FEATURE_ADVERTISER)
    { offsetof(ubleParams_t, advInterval), sizeof(uint16),         \
      0,                                   0 }, /* uint16. check separately */
    { offsetof(ubleParams_t, advChanMap),  sizeof(uint8),          \
      UBLE_MIN_CHANNEL_MAP,                UBLE_MAX_CHANNEL_MAP },
  #if defined(FEATURE_SCAN_RESPONSE)
    { offsetof(ubleParams_t, advType),     sizeof(uint8),          \
      UBLE_ADVTYPE_ADV_NC,                 UBLE_ADVTYPE_ADV_SCAN },
  #else /* FEATURE_SCAN_RESPONSE */
    { offsetof(ubleParams_t, advType),     sizeof(uint8),          \
      UBLE_ADVTYPE_ADV_NC,                 UBLE_ADVTYPE_ADV_NC },
  #endif /* FEATURE_SCAN_RESPONSE */
    { offsetof(ubleParams_t, timeToAdv),   sizeof(uint8),          \
      0,                                   255 },
    { offsetof(ubleParams_t, advData),     UBLE_MAX_ADVDATA_LEN,   \
      0,                                   0 }, /* array. check separately */
  #if defined(FEATURE_SCAN_RESPONSE)
    { offsetof(ubleParams_t, scanRspData), UBLE_MAX_ADVDATA_LEN,    \
      0,                                   0 }, /* array. check separately */
    { offsetof(ubleParams_t, advFltPolicy),sizeof(uint8),          \
      0,                                   0 }, /* TBD */
  #endif /* FEATURE_SCAN_RESPONSE */
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
    { offsetof(ubleParams_t, scanInterval),sizeof(uint16),         \
      0,                                   0 }, /* uint16. check separately */
    { offsetof(ubleParams_t, scanWindow),  sizeof(uint16),         \
      0,                                   0 }, /* uint16. check separately */
    { offsetof(ubleParams_t, scanChanMap), sizeof(uint8),          \
      UBLE_MIN_CHANNEL_MAP,                UBLE_MAX_CHANNEL_MAP },
    { offsetof(ubleParams_t, scanType),    sizeof(uint8),          \
      0,                                   1 }, /* TBD */
    { offsetof(ubleParams_t, fltDuplicate),  sizeof(uint8),        \
      0,                                   1 }, /* TBD */
    { offsetof(ubleParams_t, scanFltPolicy), sizeof(uint8),        \
      0,                                   1 }, /* TBD */
#endif /* FEATURE_SCANNER */
#if defined(FEATURE_MONITOR)
    { offsetof(ubleParams_t, monitorHandle), sizeof(uint8),        \
      UBLE_MIN_MONITOR_HANDLE,             UBLE_MAX_MONITOR_HANDLE },
    { offsetof(ubleParams_t, monitorChan), sizeof(uint8),        \
      UBLE_MIN_MONITOR_CHAN,               UBLE_MAX_MONITOR_CHAN },
    { offsetof(ubleParams_t, monitorDuration),  sizeof(uint16),    \
      0,                                   0 }, /* uint16. check separately */
    { offsetof(ubleParams_t, startTime),   sizeof(uint32),         \
      0,                                   0 }, /* uint32. check separately */
    { offsetof(ubleParams_t, accessAddr),  sizeof(uint32),         \
      0,                                   0 }, /* uint32. check separately */
#endif /* FEATURE_MONITOR */
};

/* Proxy for posting events from Micro BLE Stack to itself */
static ublePostEvtProxyCB_t uble_postEvtProxy;

/* Queue object used for internal messages */
static struct port_queueObject_s *qEvtMsg;

/* Message processing functions of all modules.
   IMPORTANT NOTE: The order of the functions in the array should coincide with
   the order of the module name definitions such as UBLE_EVTDST_LL, UBLE_EVTDST_GAP,
   etc..
*/
const static ubleProcessMsg_t ubProcessMsg[] =
{ uble_processLLMsg, uble_processGAPMsg };

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
void uble_getPublicAddr(uint8 *pPublicAddr);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @fn     uble_stackInit
 *
 * @brief  Initialize Micro BLE Stack
 *
 * @param  addrType - Type of the address which the device will use initially.
 *             Either UBLE_ADDRTYPE_PUBLIC or UBLE_ADDRTYPE_STATIC. If addrType is
 *             UBLE_ADDRTYPE_PUBLIC, the public address which is stored in CCFG
 *             preferably or in FCFG otherwise will be used for BD Addr. If
 *             addrType is UBLE_ADDRTYPE_STATIC, BD Addr will depend on pBDAddr.
 * @param  pStaticAddr - If this is NULL and addrType is UBLE_ADDRTYPE_STATIC, a
 *             random static address will be generated by the stack and will be
 *             used for BD Address. If this is not NULL and addrType is
 *             UBLE_ADDRTYPE_STATIC, the 6 byte-long data pointed to by pBDAddr
 *             will be used for BD Address as the random static address.
 * @param  pfnPostEvtProxyCB - Callback to trigger a context switch to the
 *             application task so that the messages queued by the stack can be
 *             processed by uble_processMsg() in the application task context.
 *             IMPORTANT NOTE: This callback MUST be provided
 *             by the application. The role of the callback is to post a RTOS
 *             event or semaphore to the application itself so that when
 *             the application task is activated it can call uble_processMsg()
 *             in the application task context.
 * @param  timeCritical - rf transmit operation. For strict timing on the
 *             transmission, set value to RF_TIME_CRITICAL. For relaxed timing
 *             on the transmission set value to RF_TIME_RELAXED.
 *
 * @return SUCCESS - success
 *         INVALIDPARAMETER - addrType is invalid, pfnPostEvtProxyCB is NULL,
 *                            or the given random static address is invalid
 */
bStatus_t uble_stackInit(ubleAddrType_t addrType, uint8 *pStaticAddr,
                         ublePostEvtProxyCB_t pfnPostEvtProxyCB, uint8 timeCritical)
{
#if !defined(FEATURE_STATIC_ADDR)
  (void) pStaticAddr;
#endif /* !FEATURE_STATIC_ADDR */

  if (pfnPostEvtProxyCB == NULL)
  {
    return FAILURE;
  }

  if ((timeCritical != RF_TIME_RELAXED) && (timeCritical != RF_TIME_CRITICAL))
  {
    return FAILURE;
  }

  rfTimeCrit = timeCritical;

  uble_postEvtProxy = pfnPostEvtProxyCB;

  if (addrType == UBLE_ADDRTYPE_PUBLIC)
  {
    /* Use Public Address for BD_ADDR. */
    uble_getPublicAddr(ubleBDAddr);

#if defined(FEATURE_STATIC_ADDR) && defined(FEATURE_ADVERTISER)
    /* Set TxAdd to 0 to indicate the BD address is public address */
    urfiAdvParams.advConfig.deviceAddrType = 0;

    /* Invalidate the static address whether or not it existed */
    memset(ubRSAddr, 0xFF, B_ADDR_LEN);
#endif /* FEATURE_STATIC_ADDR */
  }
#if defined(FEATURE_STATIC_ADDR) && defined(FEATURE_ADVERTISER)
  else if (addrType == UBLE_ADDRTYPE_STATIC)
  {
    /* Use Random Static Address for BD_ADDR */

    if (pStaticAddr == NULL)
    {
      uint8 i;

      /* Generate Random Static Address until successful*/
      do
      {
        for (i = 0; i < B_ADDR_LEN; i++)
        {
          ubRSAddr[i] = (uint8) (rand() & 0xFF);
        }

        /* The two most significant bits should be 1s */
        ubRSAddr[B_ADDR_LEN - 1] |= 0xC0;
      } while (uble_checkAddr(UBLE_ADDRTYPE_STATIC, ubRSAddr) != SUCCESS);
    }
    else
    {
      /* The given random static address should be compliant to the spec */
      if (uble_checkAddr(UBLE_ADDRTYPE_STATIC, pStaticAddr) != SUCCESS)
      {
        return INVALIDPARAMETER;
      }

      /* Use the given Random Static Address */
      memcpy(ubRSAddr, pStaticAddr, B_ADDR_LEN);
    }

    /* Use the Random Static Address for BD Address */
    memcpy(ubleBDAddr, ubRSAddr, B_ADDR_LEN);

    /* Set TxAdd to 1 to indicate the BD address is random address */
    urfiAdvParams.advConfig.deviceAddrType = 1;
  }
#endif /* FEATURE_STATIC_ADDR */
  else
  {
    return INVALIDPARAMETER;
  }

  /* Initilaize Micro Link Layer */
  if (ull_init() != SUCCESS)
  {
    return FAILURE;
  }

  /* Setup the queue for SetParam messages */
  qEvtMsg = port_queueCreate("qEvtMsg");

  return SUCCESS;
}

/**
 * @fn          uble_timeCompare
 *
 * @brief       This function determines if the first time parameter is greater
 *              than the second time parameter, taking timer counter wrap into
 *              account. If so, TRUE is returned.
 *
 * @param       time1 - First time.
 * @param       time2 - Second time.
 *
 * @return      TRUE:  When first parameter is greater than second.
 *              FALSE: When first parmaeter is less than or equal to
 *                     the second.
 */
uint8 uble_timeCompare(uint32 time1, uint32 time2)
{
  if ( time1 > time2 )
  {
    // check if time1 is greater than time2 due to wrap; that is, time2 is
    // actually greater than time1
    // Note: LL_MAX_OVERLAP_TIME_LIMIT value is 8 minutes which is ~half of the
    // 17 minutes overlap of the RF.
    return ( (uint8)((time1-time2) <= UBLE_MAX_OVERLAP_TIME_LIMIT) );
  }
  else // time2 >= time1
  {
    // check if time2 is greater than time1 due to wrap; that is, time1 is
    // actually greater than time2
    // Note: LL_MAX_OVERLAP_TIME_LIMIT value is 8 minutes which is ~half of the
    // 17 minutes overlap of the RF.
    return( (uint8)((time2-time1) > UBLE_MAX_OVERLAP_TIME_LIMIT) );
  }
}

/**
 * @fn          uble_timeDelta
 *
 * @brief       This function determines the difference between two time
 *              parameters, taking timer counter wrap into account.
 *
 * @param       time1 - First time.
 * @param       time2 - Second time.
 *
 * @return      Difference between time1 and time2.
 */
uint32 uble_timeDelta(uint32 time1, uint32 time2)
{
  if ( time1 >= time2 )
  {
    return( time1 - time2 );
  }
  else // time2 > time1
  {
    return( (UBLE_MAX_32BIT_TIME - (time2 - time1)) + 1 );
  }
}

/**
 * @fn     uble_setParameter
 * @brief  Set a Micro BLE Stack parameter.
 *
 * @param   param - parameter ID: @ref uble_stack_parameters
 * @param   len - length of data to write
 * @param   pValue - pointer to data to write. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  SUCCESS, INVALIDPARAMETER or bleInvalidRange
 */
bStatus_t uble_setParameter(uint8 param, uint8 len, void *pValue)
{
  bStatus_t ret = SUCCESS;

  /* Check the parameter index and the data */
  if (param >= UBLE_NUM_PARAM || pValue == NULL)
  {
    return INVALIDPARAMETER;
  }

  /* A special handling on the length is needed for ADVDATA and SCANRSPDATA */
#if defined(FEATURE_ADVERTISER)
  #if !defined(FEATURE_SCAN_RESPONSE)
  if (param == UBLE_PARAM_ADVDATA)
  #else /* FEATURE_SCAN_RESPONSE */
  if (param == UBLE_PARAM_ADVDATA || param == UBLE_PARAM_SCANRSPDATA)
  #endif /* FEATURE_SCAN_RESPONSE */
  {
    if (len > ubParamLookup[param].len)
    {
      return bleInvalidRange;
    }

    // Update the length in the RF command's parameter struct
    if (param == UBLE_PARAM_ADVDATA)
    {
      urfiAdvParams.advLen = len;
    }
  #if defined(FEATURE_SCAN_RESPONSE)
    else if (param == UBLE_PARAM_SCANRSPDATA)
    {
      urfiAdvParams.scanRspLen = len;
    }
  #endif /* FEATURE_SCAN_RESPONSE */
  }
  /* If the parameter is neighther ADVDATA nor SCANRSPDATA,
   * the desired length should be identical to the length in the lookup table.
   */
  else
#endif /* FEATURE_ADVERTISER */
  {
    if (len != ubParamLookup[param].len)
    {
      return bleInvalidRange;
    }
  }

  /* If max > min, the parameter is of uint8. The range can be checked with
   * a common rule.
   */
  if (ubParamLookup[param].min < ubParamLookup[param].max)
  {
    if (*((uint8*) pValue) < ubParamLookup[param].min ||
        ubParamLookup[param].max < *((uint8*) pValue))
    {
      ret = bleInvalidRange;
    }
  }
  /* The parameter is not of uint8.
   * Each individual parameter needs a special check.
   */
  else
  {
    uint16 temp16 = *((uint16*) pValue);

    switch (param)
    {
    case UBLE_PARAM_TXPOWER:
      temp16 = urfi_getTxPowerVal(*((int8*) pValue));
      if (temp16 == UBLE_TX_POWER_INVALID)
      {
        ret = bleInvalidRange;
      }
      else
      {
        RF_InfoVal info;

        RF_getInfo(urfiHandle, RF_GET_RADIO_STATE, &info);

        /* If radio is active, we can change TX Power right now */
        if (info.bRadioState)
        {
          rfc_CMD_SET_TX_POWER_t cmd;

          /* setup TX Power command */
          cmd.commandNo = CMD_SET_TX_POWER;
          cmd.txPower = temp16;

          /* issue immediate command */
          RF_runImmediateCmd(urfiHandle, (uint32_t*) &cmd);
        }

        /* Change TX Power in the setup command will take effect
           from the next power up */
        urSetup.common.txPower = temp16;

        RF_control(urfiHandle, RF_CTRL_UPDATE_SETUP_CMD, NULL);
      }
      break;

#if defined(FEATURE_ADVERTISER)
    case UBLE_PARAM_ADVINTERVAL:
      if (temp16 < UBLE_MIN_ADV_INTERVAL || UBLE_MAX_ADV_INTERVAL < temp16)
      {
        ret = bleInvalidRange;
      }
      break;

  #if defined(FEATURE_SCAN_RESPONSE)
    case UBLE_PARAM_ADVFLTPOLICY:
      break;
  #endif /* FEATURE_SCAN_RESPONSE */
#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
    case UBLE_PARAM_SCANINTERVAL:
      if (temp16 < UBLE_MIN_SCAN_INTERVAL || UBLE_MAX_SCAN_INTERVAL < temp16)
      {
        ret = bleInvalidRange;
      }
      break;

    case UBLE_PARAM_SCANWINDOW:
      if (temp16 < UBLE_MIN_SCAN_WINDOW || UBLE_MAX_SCAN_WINDOW < temp16)
      {
        ret = bleInvalidRange;
      }
      break;
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
    case UBLE_PARAM_MONITOR_DURATION:
      if (UBLE_MAX_MONITOR_DURATION < temp16)
      {
        ret = bleInvalidRange;
      }
      break;

    case UBLE_PARAM_MONITOR_START_TIME:
    case UBLE_PARAM_MONITOR_ACCESS_ADDR:
      /* No range check. All ranges are valid */
      break;
#endif /* FEATURE_MONITOR */

    default:
      break;
    }
  }

  if (ret == SUCCESS)
  {
    /* copy desired value to the cooresponding location of the parameter list */
    memcpy((uint8*) &ubleParams + ubParamLookup[param].offset,
           pValue, len);
  }

  return (ret);
}

/**
 * @fn     uble_getParameter
 * @brief  Get a Micro BLE Stack parameter.
 *
 * @param   param - parameter ID: @ref uble_stack_parameters
 * @param   pValue - pointer to location to get the value. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  SUCCESS or INVALIDPARAMETER
 */
bStatus_t uble_getParameter(uint8 param, void *pValue)
{
  bStatus_t ret = SUCCESS;

  /* Check the parameter index and the data */
  if (param >= UBLE_NUM_PARAM || pValue == NULL)
  {
    return INVALIDPARAMETER;
  }

  memcpy((uint8*) pValue, (uint8*) &ubleParams + ubParamLookup[param].offset,
         ubParamLookup[param].len);

  return (ret);
}

/*********************************************************************
 * @fn      uble_checkAddr
 *
 * @brief   Check if the given address is valid
 *
 * @param   addrType - UBLE_ADDRTYPE_PUBLIC, UBLE_ADDRTYPE_STATIC,
 *                     UBLE_ADDRTYPE_NONRESOLVABLE, or UBLE_ADDRTYPE_RESOLVABLE
 *          pAddr    - pointer to where the address to check resides
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
bStatus_t uble_checkAddr(ubleAddrType_t addrType, uint8 *pAddr)
{
  uint8 i, zero, one;
#if defined(FEATURE_STATIC_ADDR) || (FEATURE_PRIVATE_ADDR)
  uint8 msb2;
#endif /* FEATURE_STATIC_ADDR || FEATURE_PRIVATE_ADDR */

  if (pAddr == NULL)
  {
    return INVALIDPARAMETER;
  }

#if defined(FEATURE_STATIC_ADDR) || (FEATURE_PRIVATE_ADDR)
  /* Retrieve the two most significant bits to check */
  msb2 = pAddr[B_ADDR_LEN - 1] & 0xC0;
#endif /* FEATURE_STATIC_ADDR || FEATURE_PRIVATE_ADDR */

  switch (addrType)
  {
  case UBLE_ADDRTYPE_PUBLIC:
    break;

#if defined(FEATURE_STATIC_ADDR)
  case UBLE_ADDRTYPE_STATIC:
    /* The two most significant bits should be 11 */
    if (msb2 != 0xC0)
    {
      return FAILURE;
    }
    break;
#endif /* FEATURE_STATIC_ADDR */

#if defined(FEATURE_PRIVATE_ADDR)
  /* The two most significant bits should be 00 */
  case UBLE_ADDRTYPE_NONRESOLVABLE:
    if (msb2 != 0x00)
    {
      return FAILURE;
    }
    break;

  /* The two most significant bits should be 01 */
  case UBLE_ADDRTYPE_RESOLVABLE:
    if (msb2 != 0x40)
    {
      return FAILURE;
    }
    break;
#endif /* FEATURE_PRIVATE_ADDR */

  default:
    return INVALIDPARAMETER;
  }

#if defined(FEATURE_STATIC_ADDR) || (FEATURE_PRIVATE_ADDR)
  if (addrType != UBLE_ADDRTYPE_PUBLIC)
  {
    /* Clear the two most significant bits for convenience */
    zero = pAddr[B_ADDR_LEN - 1] & 0x37;
    /* Set the two most significant bits for convenience */
    one  = pAddr[B_ADDR_LEN - 1] | 0xC0;
  }
  else
#endif /* FEATURE_STATIC_ADDR || FEATURE_PRIVATE_ADDR */
  {
    zero = one = pAddr[B_ADDR_LEN - 1];
  }

  for (i = 0; i < B_ADDR_LEN - 1; i++)
  {
    zero |= pAddr[i];
    one  &= pAddr[i];
  }

  /* All bits other than the two most significant bits shouldn't be
     all 0s or all 1s. */
  if (zero == 0 || one == 0xFF)
  {
    return FAILURE;
  }

  return SUCCESS;
}

#if defined(FEATURE_PRIVATE_ADDR)
/**
 * @fn     uble_setPrivateAddr
 * @brief  Set BD Addr with the given Private Address.
 *
 * @param   addrType - UBLE_ADDRTYPE_NONRESOLVABLE or UBLE_ADDRTYPE_RESOLVABLE
 * @param   pPrivateAddr - Pointer to the memory location where the desired
 *          Random Private Address is prepared.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
bStatus_t uble_setPrivateAddr(ubleAddrType_t addrType, uint8* pPrivateAddr)
{
  bStatus_t status;

  if (addrType != UBLE_ADDRTYPE_NONRESOLVABLE &&
      addrType != UBLE_ADDRTYPE_RESOLVABLE)
  {
    return INVALIDPARAMETER;
  }

  /* Check if the given address is compliant to the spec */
  status = uble_checkRandomAddr(addrType, pPrivateAddr);

  if (status == SUCCESS)
  {
    /* Use the Private Address for BD Address */
    memcpy(ubleBDAddr, pPrivateAddr, B_ADDR_LEN);

    /* Set TxAdd to 1 to indicate the BD address is random address */
    urfiAdvParams.advConfig.deviceAddrType = 1;
  }

  return status;
}

/**
 * @fn     uble_resetAddr
 * @brief  Make BD Addr revert to Random Static Address if exists or
 *         Public Address otherwise.
 *
 * @param   none
 *
 * @return  none
 */
void uble_resetAddr(void)
{
  if (uble_checkAddr(UBLE_ADDRTYPE_STATIC, ubRSAddr) == SUCCESS)
  {
    /* A valid Random Static Address exists. Set BD Addr with
       the Random Static Address. */
    memcpy(ubleBDAddr, ubRSAddr, B_ADDR_LEN);
  }
  else
  {
    /* Set BD Addr with the Public Address */
    uble_getPublicAddr(ubleBDAddr);

    /* Set TxAdd to 0 to indicate the BD address is public address */
    urfiAdvParams.advConfig.deviceAddrType = 0;
  }
}
#endif /* FEATURE_PRIVATE_ADDR */

/**
 * @fn     uble_getAddr
 * @brief  Retrieve the address corresponding to the given address type
 *
 * @param   addrType - UBLE_ADDRTYPE_PUBLIC, UBLE_ADDRTYPE_STATIC or UBLE_ADDRTYPE_BD
 *
 * @return  SUCCESS, INVALIDPARAMETER OR FAILURE
 */
bStatus_t uble_getAddr(ubleAddrType_t addrType, uint8* pAddr)
{
  bStatus_t status = INVALIDPARAMETER;

  if (pAddr == NULL)
  {
    return status;
  }

  switch (addrType)
  {
  case UBLE_ADDRTYPE_PUBLIC:
    uble_getPublicAddr(pAddr);
    status = SUCCESS;
    break;

#if defined(FEATURE_STATIC_ADDR)
  case UBLE_ADDRTYPE_STATIC:
    status = uble_checkAddr(UBLE_ADDRTYPE_STATIC, ubRSAddr);
    if (status == SUCCESS)
    {
      memcpy(pAddr, ubRSAddr, B_ADDR_LEN);
    }
    break;
#endif /* FEATURE_STATIC_ADDR */

  case UBLE_ADDRTYPE_BD:
    memcpy(pAddr, ubleBDAddr, B_ADDR_LEN);
    status = SUCCESS;
    break;

  default:
    break;
  }

  return status;
}

/**
 * @fn     uble_registerAntSwitchCB
 * @brief  Register callback for antenna switch
 *
 * @param  pUbleAntSwitchCb - application antenna switch callback
 *
 * @return SUCCESS
 */
bStatus_t uble_registerAntSwitchCB(ubleAntSwitchCB_t pfnAntSwitchCB)
{
  ubleAntSwitchSel = pfnAntSwitchCB;

  return SUCCESS;
}

/**
 * @fn     uble_processMsg
 * @brief  Let the Micro Stack process the queued messages.
 *         IMPORTANT NOTE: The application MUST call this function when it
 *         process the event posted by the stack event proxy function registered
 *         through uble_initMicroBLEStack().
 *
 * @param   none
 *
 * @return  none
 */
void uble_processMsg(void)
{
  port_key_t key;

  while (!port_queueEmpty(qEvtMsg))
  {
    // Dequeue event message
    ubleEvtMsg_t *pEvtMsg;

    port_queueGet(qEvtMsg, (port_queueElem_t **)&pEvtMsg);

    if (pEvtMsg != NULL)
    {
      ubProcessMsg[pEvtMsg->hdr.dst](pEvtMsg);

      key = port_enterCS_HW();
      // Free record
#ifdef USE_ICALL
      ICall_free(pEvtMsg);
#else
      free(pEvtMsg);
#endif /* USE_ICALL */
      port_exitCS_HW(key);
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @fn     uble_getPublicAddr
 * @brief  Retrieve the public address of the device and copy it to
 *         the memory pointed to by pPublicAddr.
 *
 * @param   pPublicAddr - memory location to have the Public Address. It is the
 *          caller's responsibility to allocate the memory.
 *
 * @return  none
 */
void uble_getPublicAddr(uint8 *pPublicAddr)
{
  /* Retrieve Public Address from CCFG first */
  memcpy(pPublicAddr, (uint8*) (CCFG_BASE + LL_BADDR_PAGE_OFFSET), B_ADDR_LEN);

  /* Check if BDADDR is valid */
  if (uble_checkAddr(UBLE_ADDRTYPE_PUBLIC, pPublicAddr) != SUCCESS)
  {
    /* Retrieve Public Address from FCFG then */
    memcpy(pPublicAddr, (uint8*) (FCFG1_BASE + LL_BDADDR_OFFSET), B_ADDR_LEN);
  }
}

/**
 * @fn     uble_buildAndPostEvt
 * @brief  Build an event message based on the parameters, put it in the
 *         message queue, and call the event proxy function that resides in
 *         the application so that the event message is sent to the stack
 *         in the application task's context after context switching. Note
 *         this function should be entered in SW critical section.
 *
 * @param   evtDst - Desitination of the event message. Only UBLE_EVTDST_GAP is
 *                   available for now.
 * @param   evt    - Event ID. uGAP Broadcaster events start with UGB_EVT_.
 * @param   pMsg   - Message payload pointer
 * @param   len    - Size of the message in byte
 *
 * @return  SUCCESS or bleMemAllocError
 */
bStatus_t uble_buildAndPostEvt(ubleEvtDst_t evtDst, ubleEvt_t evt,
                               ubleMsg_t* pMsg, uint16 len)
{
  port_key_t key;
  ubleEvtMsg_t *pEvtMsg;

  // This function is entered in SW critical section.
  key = port_enterCS_HW();
#if USE_ICALL
  if ((pEvtMsg = ICall_malloc(sizeof(ubleEvtMsg_t) + len)) == NULL)
#else /* USE_ICALL */
  if ((pEvtMsg = (ubleEvtMsg_t*) malloc(sizeof(ubleEvtMsg_t) + len)) == NULL)
#endif /* USE_ICALL */
  {
    port_exitCS_HW(key);
    return bleMemAllocError;
  }
  port_exitCS_HW(key);

  pEvtMsg->hdr.dst = evtDst;
  pEvtMsg->hdr.evt = evt;

  if (pMsg != NULL)
  {
    memcpy(&(pEvtMsg->msg), pMsg, len);
  }

  port_queuePut(qEvtMsg, (port_queueElem_t *)pEvtMsg, sizeof(ubleEvtMsg_t) + len);
  uble_postEvtProxy();

  return SUCCESS;
}

/*-------------------------------------------------------------------
 * BLE_LOG FUNCTIONS
 */
#ifdef BLE_LOG
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/sysbios/knl/Clock.h>

#define LOG_BUF_SIZE    0x6800

uint8 gBleLogBuffer[LOG_BUF_SIZE+0x100]; // 256 bytes spare for overflow
uint32 gBleLogIndex = 0;
uint32 gBleLogWrapCount = 0;
uint32 gBleLogMask = BLE_LOG_MODULE_CTRL | BLE_LOG_MODULE_HOST | BLE_LOG_MODULE_APP /* | BLE_LOG_MODULE_OSAL_TASK*/ /* | BLE_LOG_MODULE_RF_CMD*/;
void bleLog_handleCyclicBuf(uint32_t len);

void bleLog_int_int(void *handle, uint32_t type, uint8_t *format, uint32_t param1, uint32_t param2)
{
  uint32_t len;

  if ((type & gBleLogMask) == 0)
    return;

  len = sprintf((char *)&gBleLogBuffer[gBleLogIndex], (const char *)format, param1, param2);
  bleLog_handleCyclicBuf(len);
}

void bleLog_int_str(void *handle, uint32_t type, uint8_t *format, uint32_t param1, char *param2)
{
  uint32_t len;

  if ((type & gBleLogMask) == 0)
    return;

  len = sprintf((char *)&gBleLogBuffer[gBleLogIndex], (const char *)format, param1, param2);
  bleLog_handleCyclicBuf(len);
}

void bleLog_int_time(void *handle, uint32_t type, uint8_t *start_str, uint32_t param1)
{
  uint32_t len;
  uint32_t timemsec = Clock_getTicks()/100;

  if ((type & gBleLogMask) == 0)
    return;

  len = sprintf((char *)&gBleLogBuffer[gBleLogIndex], (const char *)"%s %d, ---- time[msec]=%d\n", start_str, param1, timemsec);
  bleLog_handleCyclicBuf(len);
}

void bleLog_handleCyclicBuf(uint32_t len)
{
  gBleLogIndex += len;
  sprintf((char *)&gBleLogBuffer[gBleLogIndex], "----- last\n");
  if (gBleLogIndex >= LOG_BUF_SIZE)
  {
    len = sprintf((char *)&gBleLogBuffer[0], "----- gBleLogWrapCount=%d\n", ++gBleLogWrapCount);
    gBleLogIndex = len;
  }
}

#endif //BLE_LOG

/*********************************************************************
*********************************************************************/
