/***************************************************************************//**
 * @file
 * @brief Network Utility Library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_util.h>

#include  "net_util_priv.h"
#include  "net_buf_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_math.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#if defined(RTOS_MODULE_ENTROPY_AVAIL)
#include  <entropy/include/entropy.h>
#endif

#if defined(RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL)
#include  <mbedtls/entropy.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                             (NET)
#define  RTOS_MODULE_CUR                          RTOS_CFG_MODULE_NET

#define  NET_UTIL_16_BIT_ONES_CPL_NEG_ZERO            0xFFFFu
#define  NET_UTIL_32_BIT_ONES_CPL_NEG_ZERO        0xFFFFFFFFu

#define  NET_UTIL_16_BIT_SUM_ERR_NONE             DEF_BIT_NONE
#define  NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE        DEF_BIT_01
#define  NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET       DEF_BIT_02

/********************************************************************************************************
 *                                           CRC-32 DEFINES
 *
 * Note(s) : (1) IEEE 802.3 CRC-32 uses the following binary polynomial :
 *
 *               (a) x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10
 *                           + x^8  + x^7  + x^5  + x^4  + x^2  + x^1  + x^0
 *******************************************************************************************************/

#define  NET_UTIL_32_BIT_CRC_POLY                 0x04C11DB7u   // = 0000 0100 1100 0001 0001 1101 1011 0111
#define  NET_UTIL_32_BIT_CRC_POLY_REFLECT         0xEDB88320u   // = 1110 1101 1011 1000 1000 0011 0010 0000

/********************************************************************************************************
 *                                               LOCAL TABLES
 *
 * Note(s): (1) This table represents the alphabet for the base-64 encoder.
 *******************************************************************************************************/
//                                                                 See Note #1.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL)
static mbedtls_entropy_context NetUtil_EntropyContext;
static CPU_INT08U              NetUtil_EntropyInit = 0;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT32U NetUtil_GenRandom32(void);

static CPU_INT32U NetUtil_16BitSumHdrCalc(void       *p_hdr,
                                          CPU_INT16U hdr_size);

static CPU_INT32U NetUtil_16BitSumDataCalc(void        *p_data,
                                           CPU_INT16U  data_size,
                                           CPU_INT08U  *p_octet_prev,
                                           CPU_INT08U  *p_octet_last,
                                           CPU_BOOLEAN prev_octet_valid,
                                           CPU_BOOLEAN last_pkt_buf,
                                           CPU_INT08U  *p_sum_err);

static CPU_INT16U NetUtil_16BitOnesCplSumDataCalc(void       *p_data_buf,
                                                  void       *p_pseudo_hdr,
                                                  CPU_INT16U pseudo_hdr_size);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetUtil_TS_Get()
 *
 * @brief    Get current Internet Timestamp.
 *
 * @return   Internet Timestamp.
 *
 * @note     (1) "The Timestamp is a right-justified, 32-bit timestamp in milliseconds since midnight
 *               UT [Universal Time]" (RFC #791, Section 3.1 'Options : Internet Timestamp').
 *
 * @note     (2) The developer is responsible for providing a real-time clock with correct time-zone
 *               configuration to implement the Internet Timestamp, if possible.
 *******************************************************************************************************/
NET_TS NetUtil_TS_Get(void)
{
  NET_TS ts;

  ts = (NET_TS) NetUtil_TS_Get_ms();

  return (ts);
}

/****************************************************************************************************//**
 *                                           NetUtil_TS_Get_ms()
 *
 * @brief    Get current millisecond timestamp.
 *
 * @return   Timestamp, in milliseconds.
 *******************************************************************************************************/
NET_TS_MS NetUtil_TS_Get_ms(void)
{
#if CPU_CFG_TS_32_EN == DEF_ENABLED
  static CPU_BOOLEAN ts_active = DEF_NO;
  CPU_INT32U         ts_delta;
  static CPU_INT32U  ts_prev = 0u;
  CPU_TS32           ts_cur;
  CPU_TS_TMR_FREQ    ts_tmr_freq;
  static NET_TS_MS   ts_ms_tot = 0u;
  static NET_TS_MS   ts_ms_delta_rem_tot = 0u;
  NET_TS_MS          ts_ms_delta_rem_ovf;
  NET_TS_MS          ts_ms_delta_rem;
  NET_TS_MS          ts_ms_delta_num;
  NET_TS_MS          ts_ms_delta;
  NET_TS_MS          freq_ms;
  RTOS_ERR           local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ts_cur = CPU_TS_Get32();
  ts_tmr_freq = CPU_TS_TmrFreqGet(&local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ts_tmr_freq = 0;
  }

  if (ts_tmr_freq > 0u) {
    if (ts_active == DEF_YES) {                                         // If active, calc & update ts :
      ts_delta = ts_cur - ts_prev;                                      // Calc time delta (in TS).

      if ((DEF_TIME_NBR_mS_PER_SEC >= ts_tmr_freq)
          && ((DEF_TIME_NBR_mS_PER_SEC %  ts_tmr_freq) == 0u)) {
        //                                                         Calc   ts delta (in ms).
        ts_ms_delta = (NET_TS_MS)(ts_delta * (DEF_TIME_NBR_mS_PER_SEC / ts_tmr_freq));
        ts_ms_tot += (NET_TS_MS) ts_ms_delta;                           // Update ts tot   (in ms) [see Note #2a].
      } else {
        //                                                         Check for possible overflow when delta is multiple
        //                                                         by DEF_TIME_NBR_mS_PER_SEC.
        if (ts_delta > (DEF_INT_32U_MAX_VAL / DEF_TIME_NBR_mS_PER_SEC)) {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          freq_ms = ts_tmr_freq / DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = (NET_TS_MS)(ts_delta / freq_ms);
          ts_ms_tot += (NET_TS_MS) ts_ms_delta;                         // Update ts tot   (in ms) [see Note #2a].
                                                                        // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = (NET_TS_MS)(ts_delta % freq_ms);
          ts_ms_delta_rem_tot += ts_ms_delta_rem * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / ts_tmr_freq;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * ts_tmr_freq;
          ts_ms_tot += ts_ms_delta_rem_ovf;                             // ... & adj  ts tot by ovf    (see Note #2b).
        } else {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          ts_ms_delta_num = (NET_TS_MS)(ts_delta        * DEF_TIME_NBR_mS_PER_SEC);
          ts_ms_delta = (NET_TS_MS)(ts_ms_delta_num / ts_tmr_freq);
          ts_ms_tot += (NET_TS_MS) ts_ms_delta;                         // Update ts tot   (in ms) [see Note #2a].
                                                                        // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = (NET_TS_MS)(ts_ms_delta_num % ts_tmr_freq);
          ts_ms_delta_rem_tot += ts_ms_delta_rem;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / ts_tmr_freq;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * ts_tmr_freq;
          ts_ms_tot += ts_ms_delta_rem_ovf;                             // ... & adj  ts tot by ovf    (see Note #2b).
        }
      }
    } else {
      ts_active = DEF_YES;
    }

    ts_prev = ts_cur;                                                   // Save cur time for next ts update.
  } else {
    ts_ms_tot += (NET_TS_MS)ts_cur;
  }

  return (ts_ms_tot);

#else
  static NET_TS_MS   ts_ms_delta_rem_tot = 0u;
  static CPU_BOOLEAN ts_active = DEF_NO;
  NET_TS_MS          ts_ms_delta;
  static NET_TS_MS   ts_ms_tot = 0u;
  KAL_TICK           tick_cur;
  static KAL_TICK    tick_prev = 0u;
  KAL_TICK           tick_delta;
  KAL_TICK_RATE_HZ   tick_rate;
  NET_TS_MS          freq_ms;
  RTOS_ERR           local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  tick_rate = KAL_TickRateGet();
  tick_cur = KAL_TickGet(&local_err);
  PP_UNUSED_PARAM(local_err);

  if (tick_rate > 0) {
    if (ts_active == DEF_YES) {                                 // If active, calc & update ts :
      tick_delta = tick_cur - tick_prev;                        // Calc time delta (in OS ticks).

      if ( (DEF_TIME_NBR_mS_PER_SEC >= tick_rate)
           && ((DEF_TIME_NBR_mS_PER_SEC %  tick_rate) == 0u)) {
        //                                                         Calc   ts delta (in ms).
        ts_ms_delta = (NET_TS_MS)(tick_delta  * (DEF_TIME_NBR_mS_PER_SEC / tick_rate));
        ts_ms_tot += (NET_TS_MS) ts_ms_delta;                   // Update ts tot   (in ms) [see Note #2a].
      } else {
        NET_TS_MS ts_ms_delta_rem_ovf;
        NET_TS_MS ts_ms_delta_rem;
        NET_TS_MS ts_ms_delta_num;

        //                                                         Check for possible overflow when delta is multiple
        //                                                         by DEF_TIME_NBR_mS_PER_SEC.
        if (tick_delta > (DEF_INT_32U_MAX_VAL / DEF_TIME_NBR_mS_PER_SEC)) {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          freq_ms = tick_rate / DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = (NET_TS_MS)(tick_delta / freq_ms);
          ts_ms_tot += (NET_TS_MS) ts_ms_delta;                         // Update ts tot   (in ms) [see Note #2a].
                                                                        // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = (NET_TS_MS)(tick_delta % freq_ms);
          ts_ms_delta_rem_tot += ts_ms_delta_rem * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / tick_rate;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * tick_rate;
          ts_ms_tot += ts_ms_delta_rem_ovf;                             // ... & adj  ts tot by ovf    (see Note #2b).
        } else {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          ts_ms_delta_num = tick_delta   * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = ts_ms_delta_num / tick_rate;
          ts_ms_tot += ts_ms_delta;                             // Update ts tot   (in ms) [see Note #2a].
                                                                // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = ts_ms_delta_num % tick_rate;
          ts_ms_delta_rem_tot += ts_ms_delta_rem;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / tick_rate;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * tick_rate;
          ts_ms_tot += ts_ms_delta_rem_ovf;                     // ... & adj  ts tot by ovf    (see Note #2b).
        }
      }
    } else {
      ts_active = DEF_YES;
    }

    tick_prev = tick_cur;                                       // Save cur time for next ts update.
  } else {
    ts_ms_tot += tick_cur;
  }

  return (ts_ms_tot);
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetUtil_TS_Get_ms_Internal()
 *
 * @brief    Get the timestamp value in milliseconds.
 *
 * @return   Current Timestamp value in milliseconds.
 *******************************************************************************************************/
CPU_INT32U NetUtil_TS_Get_ms_Internal(void)
{
  static CPU_BOOLEAN ts_active = DEF_NO;
  static CPU_INT32U  ts_ms_tot = 0u;
  static CPU_INT32U  ts_ms_delta_rem_tot = 0u;
  CPU_INT32U         ts_ms_delta_rem_ovf;
  CPU_INT32U         ts_ms_delta_rem;
  CPU_INT32U         ts_ms_delta_num;
  CPU_INT32U         ts_ms_delta;
  CPU_INT32U         freq_ms;
  RTOS_ERR           local_err;

#if (CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_64_EN != DEF_ENABLED)
  CPU_TS_TMR_FREQ ts_tmr_freq;
  CPU_TS32        ts_delta;
  static CPU_TS32 ts_prev = 0u;
  CPU_TS32        ts_cur;
#elif (CPU_CFG_TS_64_EN == DEF_ENABLED)
  CPU_TS_TMR_FREQ ts_tmr_freq;
  CPU_TS64        ts_delta;
  static CPU_TS64 ts_prev = 0u;
  CPU_TS64        ts_cur;
#else
  KAL_TICK         tick_cur;
  static KAL_TICK  tick_prev = 0u;
  KAL_TICK         tick_delta;
  KAL_TICK_RATE_HZ tick_rate;
#endif

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

#if (CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_64_EN != DEF_ENABLED)

  ts_cur = CPU_TS_Get32();
  ts_tmr_freq = CPU_TS_TmrFreqGet(&local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ts_tmr_freq = 0;
  }

  if (ts_tmr_freq > 0u) {
    if (ts_active == DEF_YES) {                                 // If active, calc & update ts :
                                                                // Calc time delta (in TS).
      if (ts_prev > ts_cur) {
        ts_delta = (DEF_INT_32U_MAX_VAL - ts_prev) + ts_cur;            // With overflow.
      } else {
        ts_delta = ts_cur - ts_prev;                                    // Without overflow.
      }

      if ((DEF_TIME_NBR_mS_PER_SEC >= ts_tmr_freq)
          && ((DEF_TIME_NBR_mS_PER_SEC %  ts_tmr_freq) == 0u)) {
        //                                                         Calc   ts delta (in ms).
        ts_ms_delta = ts_delta * (DEF_TIME_NBR_mS_PER_SEC / ts_tmr_freq);
        ts_ms_tot += ts_ms_delta;                               // Update ts tot   (in ms) [see Note #2a].
      } else {
        //                                                         Check for possible overflow when delta is multiple
        //                                                         by DEF_TIME_NBR_mS_PER_SEC.
        if (ts_delta > (DEF_INT_32U_MAX_VAL / DEF_TIME_NBR_mS_PER_SEC)) {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          freq_ms = ts_tmr_freq / DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = ts_delta / freq_ms;
          ts_ms_tot += ts_ms_delta;                             // Update ts tot   (in ms) [see Note #2a].
                                                                // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = ts_delta % freq_ms;
          ts_ms_delta_rem_tot += ts_ms_delta_rem * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / ts_tmr_freq;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * ts_tmr_freq;
          ts_ms_tot += ts_ms_delta_rem_ovf;                     // ... & adj  ts tot by ovf    (see Note #2b).
        } else {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          ts_ms_delta_num = ts_delta * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = ts_ms_delta_num / ts_tmr_freq;
          ts_ms_tot += ts_ms_delta;                             // Update ts tot   (in ms) [see Note #2a].
                                                                // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = ts_ms_delta_num % ts_tmr_freq;
          ts_ms_delta_rem_tot += ts_ms_delta_rem;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / ts_tmr_freq;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * ts_tmr_freq;
          ts_ms_tot += ts_ms_delta_rem_ovf;                     // ... & adj  ts tot by ovf    (see Note #2b).
        }
      }
    } else {
      ts_active = DEF_YES;
    }

    ts_prev = ts_cur;                                           // Save cur time for next ts update.
  } else {
    ts_ms_tot += ts_cur;
  }

#elif (CPU_CFG_TS_64_EN == DEF_ENABLED)

  ts_cur = CPU_TS_Get64();
  ts_tmr_freq = CPU_TS_TmrFreqGet(&local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ts_tmr_freq = 0;
  }

  if (ts_tmr_freq > 0u) {
    if (ts_active == DEF_YES) {                                 // If active, calc & update ts :
                                                                // Calc time delta (in TS).
      if (ts_prev > ts_cur) {
        ts_delta = (DEF_INT_64U_MAX_VAL - ts_prev) + ts_cur;            // With overflow.
      } else {
        ts_delta = ts_cur - ts_prev;                                    // Without overflow.
      }

      if ((DEF_TIME_NBR_mS_PER_SEC >= ts_tmr_freq)
          && ((DEF_TIME_NBR_mS_PER_SEC %  ts_tmr_freq) == 0u)) {
        //                                                         Calc   ts delta (in ms).
        ts_ms_delta = ts_delta * (DEF_TIME_NBR_mS_PER_SEC / ts_tmr_freq);
        ts_ms_tot += ts_ms_delta;                               // Update ts tot   (in ms) [see Note #2a].
      } else {
        //                                                         Check for possible overflow when delta is multiple
        //                                                         by DEF_TIME_NBR_mS_PER_SEC.
        if (ts_delta > (DEF_INT_64U_MAX_VAL / DEF_TIME_NBR_mS_PER_SEC)) {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          freq_ms = ts_tmr_freq / DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = ts_delta / freq_ms;
          ts_ms_tot += ts_ms_delta;                             // Update ts tot   (in ms) [see Note #2a].
                                                                // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = ts_delta % freq_ms;
          ts_ms_delta_rem_tot += ts_ms_delta_rem * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / ts_tmr_freq;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * ts_tmr_freq;
          ts_ms_tot += ts_ms_delta_rem_ovf;                     // ... & adj  ts tot by ovf    (see Note #2b).
        } else {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          ts_ms_delta_num = ts_delta * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = ts_ms_delta_num / ts_tmr_freq;
          ts_ms_tot += ts_ms_delta;                             // Update ts tot   (in ms) [see Note #2a].
                                                                // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = ts_ms_delta_num % ts_tmr_freq;
          ts_ms_delta_rem_tot += ts_ms_delta_rem;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / ts_tmr_freq;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * ts_tmr_freq;
          ts_ms_tot += ts_ms_delta_rem_ovf;                     // ... & adj  ts tot by ovf    (see Note #2b).
        }
      }
    } else {
      ts_active = DEF_YES;
    }

    ts_prev = ts_cur;                                           // Save cur time for next ts update.
  } else {
    ts_ms_tot += ts_cur;
  }
#else

  tick_rate = KAL_TickRateGet();
  tick_cur = KAL_TickGet(&local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  if (tick_rate > 0) {
    if (ts_active == DEF_YES) {                                     // If active, calc & update ts :
      if (tick_prev > tick_cur) {
        tick_delta = (DEF_INT_32U_MAX_VAL - tick_prev) + tick_cur;
      } else {
        tick_delta = tick_cur - tick_prev;                          // Calc time delta (in OS ticks).
      }

      if ( (DEF_TIME_NBR_mS_PER_SEC >= tick_rate)
           && ((DEF_TIME_NBR_mS_PER_SEC %  tick_rate) == 0u)) {
        //                                                         Calc   ts delta (in ms).
        ts_ms_delta = (NET_TS_MS)(tick_delta  * (DEF_TIME_NBR_mS_PER_SEC / tick_rate));
        ts_ms_tot += (NET_TS_MS) ts_ms_delta;                       // Update ts tot   (in ms) [see Note #2a].
      } else {
        //                                                         Check for possible overflow when delta is multiple
        //                                                         by DEF_TIME_NBR_mS_PER_SEC.
        if (tick_delta > (DEF_INT_32U_MAX_VAL / DEF_TIME_NBR_mS_PER_SEC)) {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          freq_ms = tick_rate / DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = (NET_TS_MS)(tick_delta / freq_ms);
          ts_ms_tot += (NET_TS_MS) ts_ms_delta;                             // Update ts tot   (in ms) [see Note #2a].
                                                                            // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = (NET_TS_MS)(tick_delta % freq_ms);
          ts_ms_delta_rem_tot += ts_ms_delta_rem * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / tick_rate;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * tick_rate;
          ts_ms_tot += ts_ms_delta_rem_ovf;                                 // ... & adj  ts tot by ovf    (see Note #2b).
        } else {
          //                                                       Calc   ts delta (in ms) [see Note #2a1].
          ts_ms_delta_num = tick_delta   * DEF_TIME_NBR_mS_PER_SEC;
          ts_ms_delta = ts_ms_delta_num / tick_rate;
          ts_ms_tot += ts_ms_delta;                                 // Update ts tot   (in ms) [see Note #2a].
                                                                    // Calc   ts delta rem ovf (in ms) ...
          ts_ms_delta_rem = ts_ms_delta_num % tick_rate;
          ts_ms_delta_rem_tot += ts_ms_delta_rem;
          ts_ms_delta_rem_ovf = ts_ms_delta_rem_tot / tick_rate;
          ts_ms_delta_rem_tot -= ts_ms_delta_rem_ovf * tick_rate;
          ts_ms_tot += ts_ms_delta_rem_ovf;                         // ... & adj  ts tot by ovf    (see Note #2b).
        }
      }
    } else {
      ts_active = DEF_YES;
    }

    tick_prev = tick_cur;                                           // Save cur time for next ts update.
  } else {
    ts_ms_tot += tick_cur;
  }
#endif

  return (ts_ms_tot);
}

/****************************************************************************************************//**
 *                                           NetUtil_TS_GetMaxDly_ms()
 *
 * @brief    Get the maximum delta value between two Timestamps to avoid counter overflow.
 *           This is based on the maximum value of the TS counter and the TS frequency.
 *
 * @return   Maximum delta value in milliseconds.
 *******************************************************************************************************/
CPU_INT32U NetUtil_TS_GetMaxDly_ms(void)
{
  CPU_INT32U dly_ms;
#if (CPU_CFG_TS_32_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_64_EN == DEF_ENABLED)
  CPU_TS_TMR_FREQ ts_tmr_freq;
  RTOS_ERR        local_err;
#else
  KAL_TICK_RATE_HZ tick_rate;
#endif
#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
  CPU_TS64 dly_tmp;
#endif

#if (CPU_CFG_TS_32_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_64_EN == DEF_ENABLED)
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
#endif
#if (CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_64_EN != DEF_ENABLED)

  ts_tmr_freq = CPU_TS_TmrFreqGet(&local_err);

  dly_ms = (DEF_INT_32U_MAX_VAL - 1) / (ts_tmr_freq / DEF_TIME_NBR_mS_PER_SEC);

#elif (CPU_CFG_TS_64_EN == DEF_ENABLED)

  ts_tmr_freq = CPU_TS_TmrFreqGet(&local_err);

  dly_tmp = (CPU_TS64)((DEF_INT_64U_MAX_VAL - 1) / (ts_tmr_freq / DEF_TIME_NBR_mS_PER_SEC));

  if (dly_tmp >= DEF_INT_32U_MAX_VAL) {
    dly_ms = DEF_INT_32U_MAX_VAL - 1;
  } else {
    dly_ms = dly_tmp;
  }
#else

  tick_rate = KAL_TickRateGet();

  dly_ms = (DEF_INT_32U_MAX_VAL - 1) / (tick_rate / DEF_TIME_NBR_mS_PER_SEC);
#endif

  return (dly_ms);
}

/****************************************************************************************************//**
 *                                   NetUtil_16BitOnesCplChkSumHdrCalc()
 *
 * @brief    Calculate 16-bit one's-complement check-sum on packet header.
 *               - (1) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
 *                     check-sum & algorithm.
 *               - (2) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any
 *                     octet-length & word-alignment, the check-sums MUST be calculated in network-order
 *                     on headers that are arranged in network-order (see also 'NetUtil_16BitSumHdrCalc()
 *                     Note #5b').
 *
 * @param    p_hr        Pointer to packet header.
 *
 * @param    hdr_size    Size of packet header.
 *
 *
 * Argument(s) : p_hr        Pointer to packet header.
 *
 *               hdr_size    Size of packet header.
 *
 * Return(s)   : 16-bit one's-complement check-sum, if NO error(s).
 *               0, otherwise.
 *
 * Note(s)     : (3) Since the 16-bit sum calculation is returned as a 32-bit network-order value
 *                   (see 'NetUtil_16BitSumHdrCalc()  Note #5c1'), the final check-sum MUST be converted
 *                   to host-order but MUST NOT be re-converted back to network-order (see
 *                   'NetUtil_16BitSumHdrCalc()  Note #5c3').
 *******************************************************************************************************/
NET_CHK_SUM NetUtil_16BitOnesCplChkSumHdrCalc(void       *p_hdr,
                                              CPU_INT16U hdr_size)
{
  CPU_INT32U  sum;
  NET_CHK_SUM chk_sum;
  NET_CHK_SUM chk_sum_host;

  //                                                               --------- CALC HDR'S 16-BIT ONE'S-CPL SUM ----------
  sum = NetUtil_16BitSumHdrCalc(p_hdr, hdr_size);               // Calc  16-bit sum (see Note #3).

  while (sum >> 16u) {                                          // While 16-bit sum ovf's, ...
    sum = (sum & 0x0000FFFFu) + (sum >> 16u);                   // ... sum ovf bits back into 16-bit one's-cpl sum.
  }

  chk_sum = (NET_CHK_SUM)(~((NET_CHK_SUM)sum));                 // Perform one's cpl on one's-cpl sum.
  chk_sum_host = NET_UTIL_NET_TO_HOST_16(chk_sum);              // Conv back to host-order (see Note #3).

  return (chk_sum_host);                                        // Rtn 16-bit chk sum (see Note #3).
}

/****************************************************************************************************//**
 *                                   NetUtil_16BitOnesCplChkSumHdrVerify()
 *
 * @brief    - (1) Verify 16-bit one's-complement check-sum on packet header :
 *               - (a) Calculate one's-complement sum on packet header
 *               - (b) Verify check-sum by comparison of one's-complement sum to one's-complement
 *                       '-0' value (negative zero)
 *           - (2) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
 *                 check-sum & algorithm.
 *           - (3) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any
 *                 octet-length & word-alignment, the check-sums MUST be calculated in network-order
 *                 on headers that are arranged in network-order (see also 'NetUtil_16BitSumHdrCalc()
 *                 Note #5b').
 *
 * @param    p_hdr       Pointer to packet header.
 *
 * @param    hdr_size    Size of packet header.
 *
 * @return   DEF_OK,   if valid check-sum.
 *           DEF_FAIL, otherwise.
 *
 * @note     (4) Since the 16-bit sum calculation is returned as a 32-bit network-order value
 *               (see 'NetUtil_16BitSumHdrCalc()  Note #5c1'), the check-sum MUST be converted to
 *               host-order but MUST NOT be re-converted back to network-order for the final
 *               check-sum comparison (see 'NetUtil_16BitSumHdrCalc()  Note #5c3').
 *******************************************************************************************************/
CPU_BOOLEAN NetUtil_16BitOnesCplChkSumHdrVerify(void       *p_hdr,
                                                CPU_INT16U hdr_size)
{
  CPU_INT32U  sum;
  NET_CHK_SUM chk_sum;
  NET_CHK_SUM chk_sum_host;
  CPU_BOOLEAN valid;

  //                                                               -------- VERIFY HDR'S 16-BIT ONE'S-CPL SUM ---------
  sum = NetUtil_16BitSumHdrCalc(p_hdr, hdr_size);               // Calc 16-bit sum (see Note #4).

  while (sum >> 16u) {                                          // While 16-bit sum ovf's, ...
    sum = (sum & 0x0000FFFFu) + (sum >> 16u);                   // ... sum ovf bits back into 16-bit one's-cpl sum.
  }

  chk_sum = (NET_CHK_SUM)sum;
  chk_sum_host = NET_UTIL_NET_TO_HOST_16(chk_sum);              // Conv back to host-order (see Note #4).

  //                                                               Verify chk sum (see Note #1b).
  valid = (chk_sum_host == NET_UTIL_16_BIT_ONES_CPL_NEG_ZERO) ? DEF_OK : DEF_FAIL;

  return (valid);
}

/****************************************************************************************************//**
 *                                   NetUtil_16BitOnesCplChkSumDataCalc()
 *
 * @brief    Calculate 16-bit one's-complement check-sum on packet data.
 *               - (1) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
 *                     check-sum & algorithm.
 *               - (2) Check-sum calculated on packet data encapsulated in :
 *                   - (a) One or more network buffers             Support non-fragmented & fragmented packets
 *                   - (b) Transport layer pseudo-header           See RFC #768, Section 'Fields : Checksum' &
 *                                                                       RFC #793, Section 3.1 'Header Format :
 *                                                                           Checksum'.
 *               - (3) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any
 *                     octet-length & word-alignment, the check-sums MUST be calculated in network-order on
 *                     data & headers that are arranged in network-order (see also 'NetUtil_16BitSumDataCalc()
 *                     Note #5b').
 *
 * @param    p_data_buf          Pointer to packet data network buffer(s) (see Note #2a).
 *
 * @param    p_pseudo_hdr        Pointer to transport layer pseudo-header (see Note #2b).
 *
 * @param    pseudo_hdr_size     Size    of transport layer pseudo-header.
 *
 * @return   16-bit one's-complement check-sum, if NO error(s).
 *           0, otherwise.
 *
 * @note     (4) Since the 16-bit one's-complement check-sum calculations are returned in host-
 *               order, the returned check-sum MUST NOT be re-converted back to network-order.
 *******************************************************************************************************/
NET_CHK_SUM NetUtil_16BitOnesCplChkSumDataCalc(void       *p_data_buf,
                                               void       *p_pseudo_hdr,
                                               CPU_INT16U pseudo_hdr_size)
{
  CPU_INT16U  sum;
  NET_CHK_SUM chk_sum = 0u;

  //                                                               Calc 16-bit one's-cpl sum (see Note #4).
  sum = NetUtil_16BitOnesCplSumDataCalc(p_data_buf, p_pseudo_hdr, pseudo_hdr_size);

  chk_sum = (NET_CHK_SUM)(~((NET_CHK_SUM)sum));                 // Perform one's cpl on one's-cpl sum.

  return (chk_sum);                                             // Rtn 16-bit chk sum (see Note #4).
}

/****************************************************************************************************//**
 *                                   NetUtil_16BitOnesCplChkSumDataVerify()
 *
 * @brief    - (1) Verify 16-bit one's-complement check-sum on packet data :
 *               - (a) Calculate one's-complement sum on packet data & packet pseudo-header
 *               - (b) Verify check-sum by comparison of one's-complement sum to one's-complement
 *                       '-0' value (negative zero)
 *           - (2) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
 *                 check-sum & algorithm.
 *           - (3) Check-sum calculated on packet data encapsulated in :
 *               - (a) One or more network buffers             Support non-fragmented & fragmented packets
 *               - (b) Transport layer pseudo-header           See RFC #768, Section 'Fields : Checksum' &
 *                                                                   RFC #793, Section 3.1 'Header Format :
 *                                                                       Checksum'.
 *           - (4) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any
 *                   octet-length & word-alignment, the check-sums MUST be calculated in network-order on
 *                   data & headers that are arranged in network-order (see also 'NetUtil_16BitSumDataCalc()
 *                   Note #5b').
 *
 * @param    p_data_buf          Pointer to packet data network buffer(s) (see Note #3a).
 *
 * @param    p_pseudo_hdr        Pointer to transport layer pseudo-header (see Note #3b).
 *
 * @param    pseudo_hdr_size     Size    of transport layer pseudo-header.
 *
 * @return   DEF_OK,   if valid check-sum.
 *           DEF_FAIL, otherwise.
 *
 * @note     (5) Since the 16-bit one's-complement check-sum calculations are returned in host-
 *               order, the returned check-sum MUST NOT be re-converted back to network-order for
 *               the final check-sum comparison.
 *******************************************************************************************************/
CPU_BOOLEAN NetUtil_16BitOnesCplChkSumDataVerify(void       *p_data_buf,
                                                 void       *p_pseudo_hdr,
                                                 CPU_INT16U pseudo_hdr_size)
{
  CPU_INT16U  sum;
  NET_CHK_SUM chk_sum;
  CPU_BOOLEAN valid = DEF_FAIL;

  //                                                               Calc 16-bit one's-cpl sum (see Note #5).
  sum = NetUtil_16BitOnesCplSumDataCalc(p_data_buf, p_pseudo_hdr, pseudo_hdr_size);

  //                                                               Verify chk sum (see Notes #1b & #5).
  chk_sum = (NET_CHK_SUM)sum;
  valid = (chk_sum == NET_UTIL_16_BIT_ONES_CPL_NEG_ZERO) ? DEF_OK : DEF_FAIL;

  return (valid);
}

/****************************************************************************************************//**
 *                                           NetUtil_32BitCRC_Calc()
 *
 * @brief    Calculate 32-bit CRC.
 *
 * @param    p_data      Pointer to data to CRC.
 *
 * @param    data_len    Length  of data to CRC.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   32-bit CRC, if NO error(s).
 *           0, otherwise.
 *
 * @note     (1) IEEE 802.3 CRC-32 uses the following binary polynomial :
 *           - (a) x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10
 *                       + x^8  + x^7  + x^5  + x^4  + x^2  + x^1  + x^0
 *******************************************************************************************************/
CPU_INT32U NetUtil_32BitCRC_Calc(CPU_INT08U *p_data,
                                 CPU_INT32U data_len)
{
  CPU_INT32U crc;
  CPU_INT32U poly;
  CPU_INT32U crc_data_val;
  CPU_INT32U crc_data_val_bit_zero;
  CPU_INT32U i;
  CPU_INT32U j;
  CPU_INT08U *p_data_val;

  RTOS_ASSERT_DBG((p_data != DEF_NULL), RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG((data_len >= 1), RTOS_ERR_INVALID_ARG, 0u);

  //                                                               ----------------- CALC 32-BIT CRC ------------------
  crc = NET_UTIL_32_BIT_ONES_CPL_NEG_ZERO;                      // Init CRC to neg zero.
  poly = NET_UTIL_32_BIT_CRC_POLY_REFLECT;                      // Init reflected poly.

  p_data_val = p_data;
  for (i = 0u; i < data_len; i++) {
    crc_data_val = (CPU_INT32U)((crc ^ *p_data_val) & DEF_OCTET_MASK);

    for (j = 0u; j < DEF_OCTET_NBR_BITS; j++) {
      crc_data_val_bit_zero = crc_data_val & DEF_BIT_00;
      if (crc_data_val_bit_zero > 0) {
        crc_data_val = (crc_data_val >> 1u) ^ poly;
      } else {
        crc_data_val = (crc_data_val >> 1u);
      }
    }

    crc = (crc >> DEF_OCTET_NBR_BITS) ^ crc_data_val;
    p_data_val++;
  }

  return (crc);
}

/****************************************************************************************************//**
 *                                       NetUtil_32BitCRC_CalcCpl()
 *
 * @brief    Calculate 32-bit CRC with complement.
 *
 * @param    p_data      Pointer to data to CRC.
 *
 * @param    data_len    Length  of data to CRC.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   32-bit complemented CRC, if NO error(s).
 *           0, otherwise.
 *******************************************************************************************************/
CPU_INT32U NetUtil_32BitCRC_CalcCpl(CPU_INT08U *p_data,
                                    CPU_INT32U data_len)
{
  CPU_INT32U crc = 0u;

  RTOS_ASSERT_DBG((p_data != DEF_NULL), RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG((data_len >= 1), RTOS_ERR_INVALID_ARG, 0u);

  crc = NetUtil_32BitCRC_Calc(p_data, data_len);                // Calc CRC.

  crc ^= NET_UTIL_32_BIT_ONES_CPL_NEG_ZERO;                     // Cpl  CRC.

  return (crc);
}

/****************************************************************************************************//**
 *                                           NetUtil_32BitReflect()
 *
 * @brief    Calculate 32-bit reflection.
 *
 * @param    val     32-bit value to reflect.
 *
 * @return   32-bit reflection.
 *******************************************************************************************************/
CPU_INT32U NetUtil_32BitReflect(CPU_INT32U val)
{
  CPU_INT32U val_reflect;
  CPU_INT32U bit;
  CPU_INT32U bit_nbr;
  CPU_INT32U bit_val;
  CPU_INT32U bit_reflect;
  CPU_DATA   i;

  val_reflect = 0u;
  bit_nbr = sizeof(val) * DEF_OCTET_NBR_BITS;
  bit = DEF_BIT(0u);
  bit_reflect = DEF_BIT(bit_nbr - 1u);

  for (i = 0u; i < bit_nbr; i++) {
    bit_val = val & bit;
    if (bit_val > 0) {                                          // If val's bit set, ...
      val_reflect |= bit_reflect;                               // ... set corresponding reflect bit.
    }
    bit <<= 1u;
    bit_reflect >>= 1u;
  }

  return (val_reflect);
}

/****************************************************************************************************//**
 *                                       NetUtil_TimeSec_uS_To_ms()
 *
 * @brief    Convert seconds and microseconds values to milliseconds.
 *
 * @param    time_sec    seconds
 *
 * @param    time_us     microseconds
 *
 * @return   Number of milliseconds
 *******************************************************************************************************/
CPU_INT32U NetUtil_TimeSec_uS_To_ms(CPU_INT32U time_sec,
                                    CPU_INT32U time_us)
{
  CPU_INT32U time_us_to_ms;
  CPU_INT32U time_us_to_ms_max;
  CPU_INT32U time_sec_to_ms;
  CPU_INT32U time_sec_to_ms_max;
  CPU_INT32U time_dly_ms;

  if ((time_sec == NET_TMR_TIME_INFINITE)
      && (time_us == NET_TMR_TIME_INFINITE)) {
    time_dly_ms = NET_TMR_TIME_INFINITE;
    goto exit;
  }
  //                                                               Calculate us time delay's millisecond value, ..
  //                                                               .. rounded up to next millisecond.
  time_us_to_ms = ((time_us % DEF_TIME_NBR_uS_PER_SEC) + ((DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC) - 1u))
                  /  (DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC);
  time_sec_to_ms = time_sec * DEF_TIME_NBR_mS_PER_SEC;

  time_us_to_ms_max = DEF_INT_32U_MAX_VAL - time_sec_to_ms;
  time_sec_to_ms_max = DEF_INT_32U_MAX_VAL - time_us_to_ms;

  if ((time_us_to_ms < time_us_to_ms_max)                       // If NO        time delay integer overflow.
      && (time_sec_to_ms < time_sec_to_ms_max)) {
    time_dly_ms = time_sec_to_ms + time_us_to_ms;
  } else {                                                      // Else limit to maximum time delay values.
    time_dly_ms = NET_TMR_TIME_INFINITE;
  }

exit:
  return (time_dly_ms);
}

/****************************************************************************************************//**
 *                                           NetUtil_InitSeqNbrGet()
 *
 * @brief    Initialize the TCP Transmit Initial Sequence Counter, 'NetTCP_TxSeqNbrCtr'.
 *               - (1) Possible initialization methods include :
 *                   - (a) Time-based initialization is one preferred method since it more appropriately
 *                         provides a pseudo-random initial sequence number.
 *                   - (b) Hardware-generated random number initialization is NOT a preferred method since it
 *                         tends to produce a discrete set of pseudo-random initial sequence numbers--often
 *                         the same initial sequence number.
 *                   - (c) Hard-coded initial sequence number is NOT a preferred method since it is NOT random.
 *
 *               See also 'net_tcp.h  NET_TCP_TX_GET_SEQ_NBR()  Note #1'.
 *******************************************************************************************************/
CPU_INT32U NetUtil_InitSeqNbrGet(void)
{
#ifndef  NET_UTIL_INIT_SEQ_NBR_0
  CPU_INT32U val;

  val = NetUtil_GenRandom32();

  return (val);
#else
  return (0u);
#endif
}

/****************************************************************************************************//**
 *                                           NetUtil_RandomRangeGet()
 *
 * @brief    Get a random value in a specific range
 *
 * @param    min     Minimum value
 *
 * @param    max     Maximum value
 *
 * @return   Random value in the specified range.
 *******************************************************************************************************/
CPU_INT32U NetUtil_RandomRangeGet(CPU_INT32U min,
                                  CPU_INT32U max)
{
  CPU_INT32U diff;
  CPU_INT32U rand;
  CPU_INT32U val;

  diff = (max - min) + 1;

  rand = NetUtil_GenRandom32();

  val  = rand % diff + min;

  return (val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetUtil_GenRandom()
 *
 * @brief    Generate 32-bit random number using available sources.
 *******************************************************************************************************/
static CPU_INT32U NetUtil_GenRandom32(void)
{
  CPU_INT32U val = 0;
  RTOS_ERR   err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

#if defined(RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL)
  if (val == 0) {
    // Use mbedTLS
    if (!NetUtil_EntropyInit) {
      mbedtls_entropy_init(&NetUtil_EntropyContext);
      NetUtil_EntropyInit = 1;
    }
    mbedtls_entropy_func(&NetUtil_EntropyContext, (CPU_INT08U *) &val, sizeof(val));
  }
#endif

#if defined(RTOS_MODULE_ENTROPY_AVAIL)
  if (val == 0) {
    // Use Micrium Entropy module.
    Entropy_Generate((CPU_INT08U *) &val, sizeof(val), &err);
  }
#endif

#if CPU_CFG_TS_32_EN == DEF_ENABLED
  if (val == 0) {
    // Use time-based random number generator.
    val = Math_Rand();
    val += (CPU_INT32U)CPU_TS_Get32();
    Math_RandSetSeed(val);
    val = Math_Rand();
  }
#endif

#if CPU_CFG_TS_32_EN != DEF_ENABLED
  if (val == 0) {
    // Use kernel tick (this is our last hope).
    val = Math_Rand();
    val += (CPU_INT32U)KAL_TickGet(&err);
    Math_RandSetSeed(val);
    val = Math_Rand();
  }
#endif

  PP_UNUSED_PARAM(err);

  return val;
}

/****************************************************************************************************//**
 *                                           NetUtil_16BitSumHdrCalc()
 *
 * @brief    Calculate 16-bit sum on packet header memory buffer.
 *
 *       - (1) Calculates the sum of consecutive 16-bit values.
 *
 *       - (2) 16-bit sum is returned as a 32-bit value to preserve possible 16-bit summation overflow
 *               in the upper 16-bits.
 *
 * @param    p_hdr       Pointer to packet header.
 *
 * @param    hdr_size    Size    of packet header.
 *
 * @return   16-bit sum (see Note #2), if NO error(s).
 *           0, otherwise.
 *
 * @note     (3) Since many word-aligned processors REQUIRE that multi-octet words be located on word-
 *               aligned addresses, 16-bit sum calculation MUST ensure that 16-bit words are accessed
 *               on addresses that are multiples of 2 octets.
 *
 *               If packet header memory buffer does NOT start on a 16-bit word address boundary, then
 *               16-bit sum calculation MUST be performed by concatenating two consecutive 8-bit values.
 *
 * @note     (4) Modulo arithmetic is used to determine whether a memory buffer starts on the desired
 *               word-aligned address boundary.
 *
 *               Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus
 *               address values MUST be cast to an appropriately-sized integer value PRIOR to any
 *               modulo arithmetic operation.
 *
 * @note     (6) RFC #1071, Section 4.1 explicitly casts & sums the last odd-length octet in a check-sum
 *               calculation as a 16-bit value.
 *
 *               However, this contradicts the following sections which state that "if the total
 *               length is odd, ... the last octet is padded on the right with ... one octet of zeros
 *               ... to form a 16 bit word for ... purposes ... [of] computing the checksum" :
 *
 *           - (a) RFC #768, Section     'Fields                     : Checksum'
 *           - (b) RFC #792, Section     'Echo or Echo Reply Message : Checksum'
 *           - (c) RFC #793, Section 3.1 'Header Format              : Checksum'
 *
 *               See also 'NetUtil_16BitSumDataCalc()  Note #8'.
 *******************************************************************************************************/
static CPU_INT32U NetUtil_16BitSumHdrCalc(void       *p_hdr,
                                          CPU_INT16U hdr_size)
{
  CPU_INT32U sum_32;
  CPU_INT32U sum_val_32;
  CPU_INT16U hdr_val_16;
  CPU_INT16U size_rem;
  CPU_INT16U *p_hdr_16;
  CPU_INT08U *p_hdr_08;
  CPU_DATA   mod_16;

  //                                                               ---------------- VALIDATE SIZE -----------------
  if (hdr_size < 1) {
    return (0u);
  }

  size_rem = hdr_size;
  sum_32 = 0u;

  mod_16 = (CPU_INT08U)((CPU_ADDR)p_hdr % sizeof(CPU_INT16U));      // See Note #4.
  if (mod_16 == 0u) {                                               // If pkt hdr on 16-bit word boundary (see Note #3),
    p_hdr_16 = (CPU_INT16U *)p_hdr;
    while (size_rem >= sizeof(CPU_INT16U)) {
      hdr_val_16 = (CPU_INT16U)*p_hdr_16++;
      sum_val_32 = (CPU_INT32U) NET_UTIL_HOST_TO_NET_16(hdr_val_16);            // Conv to net-order (see Note #5b).
      sum_32 += (CPU_INT32U) sum_val_32;                            // ... calc sum with 16-bit data words.
      size_rem -= (CPU_INT16U) sizeof(CPU_INT16U);
    }
    p_hdr_08 = (CPU_INT08U *)p_hdr_16;
  } else {                                                          // Else if pkt hdr NOT on 16-bit word boundary, ...
    p_hdr_08 = (CPU_INT08U *)p_hdr;
    while (size_rem >= sizeof(CPU_INT16U)) {
      sum_val_32 = (CPU_INT32U)*p_hdr_08++;
      sum_val_32 <<= DEF_OCTET_NBR_BITS;
      sum_val_32 += (CPU_INT32U)*p_hdr_08++;
      sum_32 += (CPU_INT32U) sum_val_32;                            // ... calc sum with  8-bit data vals.
      size_rem -= (CPU_INT16U) sizeof(CPU_INT16U);
    }
  }

  if (size_rem > 0) {                                               // Sum last octet, if any (see Note #6).
    sum_32 += ((CPU_INT32U)*p_hdr_08 << 8);
  }

  return (sum_32);                                                  // Rtn 16-bit sum (see Note #5c1).
}

/****************************************************************************************************//**
 *                                       NetUtil_16BitSumDataCalc()
 *
 * @brief    Calculate 16-bit sum on packet data memory buffer.
 *
 *       - (1) Calculates the sum of consecutive 16-bit values.
 *
 *       - (2) 16-bit sum is returned as a 32-bit value to preserve possible 16-bit summation overflow
 *               in the upper 16-bits.
 *
 * @param    p_data              Pointer to packet data.
 *
 * @param    data_size           Size    of packet data (in this network buffer only
 *                               [see 'NetUtil_16BitOnesCplSumDataCalc()  Note #1a']).
 *
 * @param    p_octet_prev        Pointer to last octet from a fragmented packet's previous buffer.
 *
 * @param    p_octet_last        Pointer to variable that will receive the value of the last octet from a
 *                               fragmented packet's current buffer.
 *
 * @param    prev_octet_valid    Indicate whether pointer to the last octet of the packet's previous
 *                               buffer is valid.
 *
 * @param    last_pkt_buf        Indicate whether the current packet buffer is the last packet buffer.
 *
 * @param    p_sum_err           Pointer to variable that will receive the error return code(s) from this function :
 *                               NET_UTIL_16_BIT_SUM_ERR_NONE            No error return codes.
 *                               The following error return codes are bit-field codes logically OR'd & MUST
 *                               be individually tested by bit-wise tests :
 *                               NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE       Packet buffer's data size is
 *                               a zero size.
 *                               NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET      Last odd-length octet in packet
 *                               buffer is available; check
 *                               'p_octet_last' return value.
 *
 * @return   16-bit sum (see Note #2), if NO error(s).
 *           0, otherwise.
 *
 * @note     (3) Since many word-aligned processors REQUIRE that multi-octet words be located on word-
 *               aligned addresses, 16-bit sum calculation MUST ensure that 16-bit words are accessed
 *               on addresses that are multiples of 2 octets.
 *
 *               If packet data memory buffer does NOT start on a 16-bit word address boundary, then
 *               16-bit sum calculation MUST be performed by concatenating two consecutive 8-bit values.
 *
 * @note     (4) Modulo arithmetic is used to determine whether a memory buffer starts on the desired
 *               word-aligned address boundary.
 *
 *               Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus
 *               address values MUST be cast to an appropriately-sized integer value PRIOR to any
 *               modulo arithmetic operation.
 *
 * @note     (6) Optimized 32-bit sum calculations implemented in the network protocol suite's network-
 *               specific library port optimization file(s).
 *
 *               See also 'net_util.h  FUNCTION PROTOTYPES  DEFINED IN PRODUCT'S  net_util_a.*  Note #1'.
 *
 * @note     (7) Since pointer arithmetic is based on the specific pointer data type & inherent pointer
 *               data type size, pointer arithmetic operands :
 *
 *           - (a) MUST be in terms of the specific pointer data type & data type size; ...
 *           - (b) SHOULD NOT & in some cases MUST NOT be cast to other data types or data type sizes.
 *
 * @note     (8) The following sections state that "if the total length is odd, ... the last octet
 *               is padded on the right with ... one octet of zeros ... to form a 16 bit word for
 *               ... purposes ... [of] computing the checksum" :
 *
 *           - (a) RFC #768, Section     'Fields                     : Checksum'
 *           - (b) RFC #792, Section     'Echo or Echo Reply Message : Checksum'
 *           - (c) RFC #793, Section 3.1 'Header Format              : Checksum'
 *******************************************************************************************************/
static CPU_INT32U NetUtil_16BitSumDataCalc(void        *p_data,
                                           CPU_INT16U  data_size,
                                           CPU_INT08U  *p_octet_prev,
                                           CPU_INT08U  *p_octet_last,
                                           CPU_BOOLEAN prev_octet_valid,
                                           CPU_BOOLEAN last_pkt_buf,
                                           CPU_INT08U  *p_sum_err)
{
  CPU_INT08U mod_32;
#if (NET_CFG_OPTIMIZE_ASM_EN == DEF_ENABLED)
  CPU_INT16U size_rem_32_offset;
  CPU_INT16U size_rem_32;
#else
  CPU_INT32U *p_data_32;
  CPU_INT32U data_val_32;
#endif
  CPU_INT32U  sum_32;
  CPU_INT32U  sum_val_32 = 0;
  CPU_INT16U  data_val_16;
  CPU_INT16U  size_rem;
  CPU_INT16U  *p_data_16;
  CPU_INT08U  *p_data_08;
  CPU_DATA    mod_16;
  CPU_BOOLEAN pkt_aligned_16;

  sum_32 = 0u;

  if (data_size < 1) {                                          // ------------ HANDLE NULL-SIZE DATA PKT -------------
    *p_sum_err = NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE;

    if (prev_octet_valid != DEF_NO) {                           // If null size & last octet from prev pkt buf avail ..
      if (last_pkt_buf != DEF_NO) {                             // ...   & on last pkt buf,              ...
        sum_val_32 = (CPU_INT32U)*p_octet_prev;                 // ...   cast prev pkt buf's last octet, ...
        sum_val_32 <<= DEF_OCTET_NBR_BITS;                      // ... pad odd-len pkt len (see Note #5) ...
        sum_32 = sum_val_32;                                    // ...  & rtn prev pkt buf's last octet as last sum.
      } else {                                                  // ... & NOT on last pkt buf, ...
        *p_octet_last = *p_octet_prev;                          // ... rtn last octet from prev pkt buf as last octet.
        DEF_BIT_SET(*p_sum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
      }
    } else {
      ;                                                         // If null size & NO prev octet, NO action(s) req'd.
    }

    return (sum_32);                                            // Rtn 16-bit sum (see Note #5c1).
  }

  //                                                               ----------- HANDLE NON-NULL DATA PKT -----------
  size_rem = data_size;
  *p_sum_err = NET_UTIL_16_BIT_SUM_ERR_NONE;

  //                                                               See Notes #3 & #4.
  mod_16 = (CPU_INT08U)((CPU_ADDR)p_data % sizeof(CPU_INT16U));
  pkt_aligned_16 = (((mod_16 == 0u) && (prev_octet_valid == DEF_NO))
                    || ((mod_16 != 0u) && (prev_octet_valid == DEF_YES))) ? DEF_YES : DEF_NO;

  p_data_08 = (CPU_INT08U *)p_data;
  if (prev_octet_valid == DEF_YES) {                                // If last octet from prev pkt buf avail,   ...
    sum_val_32 = (CPU_INT32U)*p_octet_prev;
    sum_val_32 <<= DEF_OCTET_NBR_BITS;                              // ... prepend last octet from prev pkt buf ...

    sum_val_32 += (CPU_INT32U)*p_data_08++;
    sum_32 += (CPU_INT32U) sum_val_32;                              // ... to first octet in cur pkt buf.

    size_rem -= (CPU_INT16U) sizeof(CPU_INT08U);
  }

  if (pkt_aligned_16 == DEF_YES) {                                  // If pkt data aligned on 16-bit boundary, ..
                                                                    // .. calc sum with 16- & 32-bit data words.
    p_data_16 = (CPU_INT16U *)p_data_08;
    mod_32 = (CPU_INT08U)((CPU_ADDR)p_data_16 % sizeof(CPU_INT32U));         // See Note #4.
    if ((mod_32 != 0u)                                              // If leading 16-bit pkt data avail, ..
        && (size_rem >= sizeof(CPU_INT16U))) {
      data_val_16 = (CPU_INT16U)*p_data_16++;
      sum_val_32 = (CPU_INT32U) NET_UTIL_HOST_TO_NET_16(data_val_16);           // Conv to net-order (see Note #5b).
      sum_32 += (CPU_INT32U) sum_val_32;                            // .. start calc sum with leading 16-bit data word.
      size_rem -= (CPU_INT16U) sizeof(CPU_INT16U);
    }

#if (NET_CFG_OPTIMIZE_ASM_EN == DEF_ENABLED)
    //                                                             Calc optimized 32-bit size rem.
    size_rem_32_offset = (CPU_INT16U)(size_rem % sizeof(CPU_INT32U));
    size_rem_32 = (CPU_INT16U)(size_rem - size_rem_32_offset);
    //                                                             Calc optimized 32-bit sum (see Note #6).
    sum_val_32 = (CPU_INT32U)NetUtil_16BitSumDataCalcAlign_32((void *)p_data_16,
                                                              (CPU_INT32U)size_rem_32);
    sum_32 += (CPU_INT32U)sum_val_32;
    size_rem -= (CPU_INT32U)size_rem_32;

    p_data_08 = (CPU_INT08U *)p_data_16;
    p_data_08 += size_rem_32;                                       // MUST NOT cast ptr operand (see Note #7b).
    p_data_16 = (CPU_INT16U *)p_data_08;

#else
    p_data_32 = (CPU_INT32U *)p_data_16;
    while (size_rem >= sizeof(CPU_INT32U)) {                        // While pkt data aligned on 32-bit boundary; ...
      data_val_32 = (CPU_INT32U) *p_data_32++;                      // ... get sum data with 32-bit data words,   ...

      data_val_16 = (CPU_INT16U)((data_val_32 >> 16u) & 0x0000FFFFu);
      sum_val_32 = (CPU_INT32U)  NET_UTIL_HOST_TO_NET_16(data_val_16);          // Conv to net-order (see Note #5b).
      sum_32 += (CPU_INT32U)  sum_val_32;                           // ... & calc sum with upper 16-bit data word ...

      data_val_16 = (CPU_INT16U) (data_val_32         & 0x0000FFFFu);
      sum_val_32 = (CPU_INT32U)  NET_UTIL_HOST_TO_NET_16(data_val_16);          // Conv to net-order (see Note #5b).
      sum_32 += (CPU_INT32U)  sum_val_32;                           // ...               & lower 16-bit data word.

      size_rem -= (CPU_INT16U)  sizeof(CPU_INT32U);
    }
    p_data_16 = (CPU_INT16U *)p_data_32;
#endif

    while (size_rem >= sizeof(CPU_INT16U)) {                        // While pkt data aligned on 16-bit boundary; ..
      data_val_16 = (CPU_INT16U)*p_data_16++;
      sum_val_32 = (CPU_INT32U) NET_UTIL_HOST_TO_NET_16(data_val_16);           // Conv to net-order (see Note #5b).
      sum_32 += (CPU_INT32U) sum_val_32;                            // .. calc sum with 16-bit data words.
      size_rem -= (CPU_INT16U) sizeof(CPU_INT16U);
    }
    if (size_rem > 0) {
      sum_val_32 = (CPU_INT32U)(*((CPU_INT08U *)p_data_16));
    }
  } else {                                                          // Else pkt data NOT aligned on 16-bit boundary, ..
    while (size_rem >= sizeof(CPU_INT16U)) {
      sum_val_32 = (CPU_INT32U)*p_data_08++;
      sum_val_32 <<= DEF_OCTET_NBR_BITS;
      sum_val_32 += (CPU_INT32U)*p_data_08++;
      sum_32 += (CPU_INT32U) sum_val_32;                            // .. calc sum with  8-bit data vals.
      size_rem -= (CPU_INT16U) sizeof(CPU_INT16U);
    }
    if (size_rem > 0) {
      sum_val_32 = (CPU_INT32U)*p_data_08;
    }
  }

  if (size_rem > 0) {
    if (last_pkt_buf != DEF_NO) {                                   // If last pkt buf, ...
      sum_val_32 <<= DEF_OCTET_NBR_BITS;                            // ... pad odd-len pkt len (see Note #8).
      sum_32 += (CPU_INT32U)sum_val_32;
    } else {
      *p_octet_last = (CPU_INT08U)sum_val_32;                       // Else rtn last octet.
      DEF_BIT_SET(*p_sum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
    }
  }

  return (sum_32);                                                  // Rtn 16-bit sum (see Note #5c1).
}

/****************************************************************************************************//**
 *                                       NetUtil_16BitOnesCplSumDataCalc()
 *
 * @brief    Calculate 16-bit one's-complement sum on packet data.
 *
 *       - (1) Calculates the 16-bit one's-complement sum of packet data encapsulated in :
 *
 *           - (a) One or more network buffers             Support non-fragmented & fragmented packets
 *           - (b) Transport layer pseudo-header           See RFC #768, Section 'Fields : Checksum' &
 *                                                               RFC #793, Section 3.1 'Header Format :
 *                                                                   Checksum'.
 *
 * @param    p_data_buf          Pointer to packet data network buffer(s) (see Notes #1a & #2).
 *
 * @param    p_pseudo_hdr        Pointer to transport layer pseudo-header (see Note  #1b).
 *
 * @param    pseudo_hdr_size     Size    of transport layer pseudo-header.
 *
 * @return   16-bit one's-complement sum, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) Pointer to network buffer packet NOT validated as a network buffer.  However, no memory
 *               corruption occurs since no write operations are performed.
 *
 * @note     (3) Network buffer packet's configuration:
 *           - (a) The following network buffer packet header fields MUST be configured BEFORE any
 *                   packet data checksum is calculated :
 *
 *               - (1) Packet's currently configured protocol index
 *               - (2) Packet's currently configured protocol header length
 *               - (3) Packet's current data length
 *
 *           - (b) The network buffer packet's currently configured protocol header length & current
 *                   data length do NOT need to be individually correct but MUST be synchronized such
 *                   that their sum equals the current protocol's total length--i.e. the total number
 *                   of octets in the packet for the current protocol.
 *
 *                   For example, a protocol layer receive may NOT yet have configured a packet's
 *                   protocol header length which would still be set to zero (0).  However, it will
 *                   NOT have offset the current protocol header length from the packet's current
 *                   data length.  Therefore, the sum of the packet's current protocol header length
 *                   & current data length will still equal the current protocol's total length.
 *
 * @note     (4) Default case already invalidated in earlier functions.  However, the default case is
 *               included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *
 * @note     (5) Since the 16-bit sum calculations are returned as 32-bit network-order values
 *               (see 'NetUtil_16BitSumDataCalc()  Note #5c1'), the one's-complement sum MUST be
 *               converted to host-order but MUST NOT be re-converted back to network-order (see
 *               'NetUtil_16BitSumDataCalc()  Note #5c3').
 *******************************************************************************************************/
static CPU_INT16U NetUtil_16BitOnesCplSumDataCalc(void       *p_data_buf,
                                                  void       *p_pseudo_hdr,
                                                  CPU_INT16U pseudo_hdr_size)
{
  NET_BUF     *p_buf;
  NET_BUF     *p_buf_next;
  NET_BUF_HDR *p_buf_hdr;
  void        *p_data;
  CPU_INT32U  sum;
  CPU_INT32U  sum_val;
  CPU_INT16U  sum_ones_cpl;
  CPU_INT16U  sum_ones_cpl_host = 0u;
  CPU_INT16U  data_ix = 0u;
  CPU_INT16U  data_len = 0u;
  CPU_INT08U  sum_err;
  CPU_INT08U  octet_prev;
  CPU_INT08U  octet_last;
  CPU_BOOLEAN octet_prev_valid;
  CPU_BOOLEAN octet_last_valid;
  CPU_BOOLEAN mem_buf_last;

  //                                                               ------ CALC PKT DATA 16-BIT ONE'S-CPL SUM ------
  p_buf = (NET_BUF *)p_data_buf;
  sum = 0u;
  octet_prev = 0u;
  octet_last = 0u;
  octet_prev_valid = DEF_NO;
  octet_last_valid = DEF_NO;
  mem_buf_last = DEF_NO;

  if (p_pseudo_hdr != DEF_NULL) {                                   // Calc pkt's pseudo-hdr 16-bit sum (see Note #1b).
    sum_val = NetUtil_16BitSumDataCalc(p_pseudo_hdr,
                                       pseudo_hdr_size,
                                       &octet_prev,
                                       &octet_last,
                                       octet_prev_valid,
                                       mem_buf_last,
                                       &sum_err);
    sum += sum_val;

    octet_last_valid = DEF_BIT_IS_SET(sum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
    if (octet_last_valid == DEF_YES) {                              // If last octet from pseudo-hdr avail, ...
      octet_prev = octet_last;                                      // ... prepend last octet to first pkt buf.
      octet_prev_valid = DEF_YES;
    } else {
      octet_prev = 0u;
      octet_prev_valid = DEF_NO;
    }
  }

  while (p_buf != DEF_NULL) {                                       // Calc ALL data pkts' 16-bit sum  (see Note #1a).
    p_buf_hdr = &p_buf->Hdr;
    switch (p_buf_hdr->ProtocolHdrType) {                           // Demux pkt buf's protocol ix/len (see Note #3b).
      case NET_PROTOCOL_TYPE_ICMP_V4:
      case NET_PROTOCOL_TYPE_ICMP_V6:
        data_ix = p_buf_hdr->ICMP_MsgIx;
        data_len = p_buf_hdr->ICMP_HdrLen    + (CPU_INT16U)p_buf_hdr->DataLen;
        break;

      case NET_PROTOCOL_TYPE_UDP_V4:
      case NET_PROTOCOL_TYPE_UDP_V6:
#ifdef  NET_TCP_MODULE_EN
      case NET_PROTOCOL_TYPE_TCP_V4:
      case NET_PROTOCOL_TYPE_TCP_V6:
#endif
        data_ix = p_buf_hdr->TransportHdrIx;
        data_len = p_buf_hdr->TransportHdrLen + (CPU_INT16U)p_buf_hdr->DataLen;
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_NONE:
        data_ix = p_buf_hdr->TotLen - p_buf_hdr->DataLen;
        data_len = p_buf_hdr->DataLen;
        break;

      default:                                                      // See Note #4.
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
    }

    p_data = &p_buf->DataPtr[data_ix];
    p_buf_next = p_buf_hdr->NextBufPtr;
    mem_buf_last = (p_buf_next == DEF_NULL) ? DEF_YES : DEF_NO;
    //                                                             Calc pkt buf's 16-bit sum.
    sum_val = NetUtil_16BitSumDataCalc(p_data,
                                       data_len,
                                       &octet_prev,
                                       &octet_last,
                                       octet_prev_valid,
                                       mem_buf_last,
                                       &sum_err);

    if (mem_buf_last != DEF_YES) {                                  // If NOT on last pkt buf &                     ...
      octet_last_valid = DEF_BIT_IS_SET(sum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
      if (octet_last_valid == DEF_YES) {                            // ...         last octet from cur  pkt buf avail,
        octet_prev = octet_last;                                    // ... prepend last octet to   next pkt buf.
        octet_prev_valid = DEF_YES;
      } else {
        octet_prev = 0u;
        octet_prev_valid = DEF_NO;
      }
    }

    sum += sum_val;
    p_buf = p_buf_next;
  }

  while (sum >> 16u) {                                              // While 16-bit sum ovf's, ...
    sum = (sum & 0x0000FFFFu) + (sum >> 16u);                       // ... sum ovf bits back into 16-bit one's-cpl sum.
  }

  sum_ones_cpl = (CPU_INT16U)sum;
  sum_ones_cpl_host = NET_UTIL_NET_TO_HOST_16(sum_ones_cpl);        // Conv back to host-order  (see Note #5b).

  return (sum_ones_cpl_host);                                       // Rtn 16-bit one's-cpl sum (see Note #1).
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
