/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SL_IPERF_UTIL_H__
#define __SL_IPERF_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_iperf_types.h"

/**************************************************************************//**
 * @addtogroup SL_IPERF_UTIL_API iPerf - Utilities
 * @ingroup SL_IPERF
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// iPerf test log macro function
#define sl_iperf_test_log(__t, __format, ...)                        \
  do {                                                               \
    if ((__t)->log != NULL && (__t)->log->print != NULL) {           \
      sl_iperf_log_print((__t)->log, "[iPerf][Test%lu]", (__t)->id); \
      sl_iperf_log_print((__t)->log, __format, ##__VA_ARGS__);       \
    }                                                                \
  } while (0)

#if SL_IPERF_VERBOSE_MODE
#define sl_iperf_test_log_verbose(__t, __format, ...) \
  sl_iperf_test_log(__t, __format, ##__VA_ARGS__)
#else
#define sl_iperf_test_log_verbose(__t, __format, ...) \
  do { (void) __t; (void)__format; } while (0)
#endif

/// Maximum util string length definition
#define SL_IPERF_UTIL_MAX_STRING_LENGTH         (128UL)

/// iPerf time converter enumeration
typedef enum sl_iperf_time_converter {
  /// Sec to MiliSec multiplier
  SL_IPERF_TIME_S_TO_MS_ML  = 1000UL,
  /// Sec to MicroSec multiplier
  SL_IPERF_TIME_S_TO_US_ML  = 1000UL * 1000UL,
  /// Sec to NanoSec multiplier
  SL_IPERF_TIME_S_TO_NS_ML  = 1000UL * 1000UL * 1000UL,
  /// MiliSec to MicroSec multiplier
  SL_IPERF_TIME_MS_TO_US_ML = 1000UL,
  /// Milisec to NanoSec multiplier
  SL_IPERF_TIME_MS_TO_NS_ML = 1000UL * 1000UL
} sl_iperf_time_converter_t;

/// iPerf data converter enumeration
typedef enum sl_iperf_data_converter {
  /// Byte to bit multiplier
  SL_IPERF_DATA_BYTE_TO_BIT_ML   = 8LU,
  /// Kilobit to bit multiplier
  SL_IPERF_DATA_KBIT_TO_BIT_ML   = 1000LU,
  /// Megabit to bit multiplier
  SL_IPERF_DATA_MBIT_TO_BIT_ML   = 1000LU * 1000LU,
  /// Gigabit to bit multiplier
  SL_IPERF_DATA_GBIT_TO_BIT_ML   = 1000LU * 1000LU  * 1000LU,
  /// Kilobyte to Byte multiplier
  SL_IPERF_DATA_KBYTE_TO_BYTE_ML = 1024LU,
  /// Megabyte to Byte multiplier
  SL_IPERF_DATA_MBYTE_TO_BYTE_ML = 1024LU * 1024LU,
  /// Gigabyte to Byte multiplier
  SL_IPERF_DATA_GBYTE_TO_BYTE_ML = 1024LU * 1024LU  * 1024LU
} sl_iperf_data_converter_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Convert iPerf status to string
 * @details Helper function
 * @param[in] status Status
 * @return const char* String value result
 *****************************************************************************/
const char * sl_iperf_test_status_to_str(const sl_iperf_status_t status);

/**************************************************************************//**
 * @brief Convert iPerf error to string
 * @details Helper function
 * @param[in] error Error
 * @return const char* String value result
 *****************************************************************************/
const char * sl_iperf_test_error_to_str(const sl_iperf_error_t error);

/**************************************************************************//**
 * @brief Convert iPerf mode to string
 * @details Helper function
 * @param[in] mode Mode
 * @return const char* String value result
 *****************************************************************************/
const char * sl_iperf_opt_mode_to_str(const sl_iperf_mode_t mode);

/**************************************************************************//**
 * @brief Convert iPerf protocol to string
 * @details Helper function
 * @param[in] protocol Protocol
 * @return const char* String value result
 *****************************************************************************/
const char * sl_iperf_opt_protocol_to_str(const sl_iperf_protocol_t protocol);

/**************************************************************************//**
 * @brief Print iPerf test instance in json format
 * @details Helper function
 * @param[in,out] test
 *****************************************************************************/
void sl_iperf_print_test_log_json(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Change byte order in 32bit Integer
 * @details Helper function
 * @param[in] i32val Input value
 * @return int32_t Converted value
 *****************************************************************************/
int32_t sl_iperf_i32_change_byte_order(const int32_t i32val);

/**************************************************************************//**
 * @brief Change byte order in 16bit Integer
 * @details Helper function
 * @param[in] i16val Input value
 * @return int16_t Converted value
 *****************************************************************************/
int16_t sl_iperf_i16_change_byte_order(const int16_t i16val);

/**************************************************************************//**
 * @brief Update iPerf bandwidth
 * @details Calculation should be used in test cases, like iPerf UDP Server
 * @param[in,out] test Test descriptor
 *****************************************************************************/
void sl_iperf_test_update_status(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Calculate average bandwidth
 * @details calculate bandwidth using elapsed time and received count of bytes
 * @param[in,out] test Test descriptor
 *****************************************************************************/
void sl_iperf_test_calculate_average_bandwidth(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Convert bandwidth unit enum to string
 * @details Conversion by bandwidth format
 * @param[in] format Expected bandwidth format
 * @return const char* String value of bandwidth unit
 *****************************************************************************/
const char * sl_iperf_opt_bw_format_to_str(const sl_iperf_opt_bw_format format);

/**************************************************************************//**
 * @brief Set iPerf test error and status
 * @details Helper function
 * @param[in,out] test Test descriptor
 * @param[in] err Error value
 * @param[in] status Status value
 *****************************************************************************/
static inline void sl_iperf_test_set_err_and_stat(sl_iperf_test_t * const test,
                                                  sl_iperf_error_t err,
                                                  sl_iperf_status_t status)
{
  test->err = err;
  test->status = status;
}

/**************************************************************************//**
 * @brief Calculate time duration in seconds
 * @details Helper function
 * @param[in] test Test descriptor
 * @return uint32_t Time duration in seconds
 *****************************************************************************/
static inline uint32_t sl_iperf_test_calc_time_duration_sec(const sl_iperf_test_t * const test)
{
  return (test->statistic.ts_end_ms - test->statistic.ts_start_ms) / SL_IPERF_TIME_S_TO_MS_ML;
}

/**************************************************************************//**
 * @brief Calculate time duration in miliseconds
 * @details Helper function
 * @param[in] test Test descriptor
 * @return uint32_t Time duration in miliseconds
 *****************************************************************************/
static inline uint32_t sl_iperf_test_calc_time_duration_ms(const sl_iperf_test_t * const test)
{
  return (test->statistic.ts_end_ms - test->statistic.ts_start_ms);
}

/**************************************************************************//**
 * @brief Calculate time data from miliseconds
 * @details Helper function
 * @param[out] dst_time Time data structure
 * @param[in] ms Miliseconds
 *****************************************************************************/
static inline void sl_iperf_calc_time_from_ms(sl_iperf_time_t * const dst_time, const sl_iperf_ts_ms_t ms)
{
  dst_time->sec = ms / SL_IPERF_TIME_S_TO_MS_ML;
  dst_time->usec = (ms - (dst_time->sec * SL_IPERF_TIME_S_TO_MS_ML)) * SL_IPERF_TIME_MS_TO_US_ML;
}

/**************************************************************************//**
 * @brief Calculate milisec value from time structure
 * @details Helper function
 * @param[in] time Time data
 * @return sl_iperf_ts_ms_t Time in miliseconds
 *****************************************************************************/
static inline sl_iperf_ts_ms_t sl_iperf_calc_ms_from_time(const sl_iperf_time_t * const time)
{
  return (sl_iperf_ts_ms_t)time->sec * SL_IPERF_TIME_S_TO_MS_ML + time->usec / SL_IPERF_TIME_MS_TO_US_ML;
}

/**************************************************************************//**
 * @brief Test is UDP Server
 * @details Helper function
 * @param[in] test Test
 * @return true UDP Server mode
 * @return false Not UDP Server mode
 *****************************************************************************/
static inline bool sl_iperf_test_is_udp_srv(const sl_iperf_test_t * const test)
{
  return (bool)((test->opt.protocol == SL_IPERF_IPROTOV6_UDP
                 || test->opt.protocol == SL_IPERF_IPROTOV4_UDP)
                && test->opt.mode == SL_IPERF_MODE_SERVER);
}

/**************************************************************************//**
 * @brief Test is UDP Client
 * @details Helper function
 * @param[in] test Test
 * @return true UDP Client mode
 * @return false Not UDP Client mode
 *****************************************************************************/
static inline bool sl_iperf_test_is_udp_clnt(const sl_iperf_test_t * const test)
{
  return (bool)((test->opt.protocol == SL_IPERF_IPROTOV6_UDP
                 || test->opt.protocol == SL_IPERF_IPROTOV4_UDP)
                && test->opt.mode == SL_IPERF_MODE_CLIENT);
}

/**************************************************************************//**
 * @brief Test is TCP Server
 * @details Helper function
 * @param[in] test Test
 * @return true TCP Server mode
 * @return false Not TCP Server mode
 *****************************************************************************/
static inline bool sl_iperf_test_is_tcp_srv(const sl_iperf_test_t * const test)
{
  return (bool)((test->opt.protocol == SL_IPERF_IPROTOV6_TCP
                 || test->opt.protocol == SL_IPERF_IPROTOV4_TCP)
                && test->opt.mode == SL_IPERF_MODE_SERVER);
}

/**************************************************************************//**
 * @brief Test is TCP Client
 * @details Helper function
 * @param[in] test Test
 * @return true TCP Client mode
 * @return false Not TCP Client mode
 *****************************************************************************/
static inline bool sl_iperf_test_is_tcp_clnt(const sl_iperf_test_t * const test)
{
  return (bool)((test->opt.protocol == SL_IPERF_IPROTOV6_TCP
                 || test->opt.protocol == SL_IPERF_IPROTOV4_TCP)
                && test->opt.mode == SL_IPERF_MODE_CLIENT);
}

/**************************************************************************//**
 * @brief Fill Test buffer with standard pattern
 * @details Helper function for Clients
 * @param[in,out] test Test
 *****************************************************************************/
void sl_iperf_test_fill_buffer_with_pattern(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Check test start time stamp
 * @details Elapsed time calculation and comparison with
 * @param test_start_ts Test start time stamp
 * @return true There is time left, if 'SL_IPERF_MAX_TEST_TIMEOUT_MS' is 0,
 *              always true
 * @return false Time out
 *****************************************************************************/
bool sl_iperf_test_check_time(const sl_iperf_ts_ms_t test_start_ts);

/**************************************************************************//**
 * @brief Calculate data bytes from formatted bandwidth
 * @details Helper function
 * @param[in] format Bandwidth format
 * @param[in] amount_bandwidth Bandwidth format
 * @return uint32_t Bytes amount
 *****************************************************************************/
uint32_t sl_iperf_bytes_from_formatted_bandwidth(const sl_iperf_opt_bw_format format,
                                                 const uint32_t amount_bandwidth);

/**************************************************************************//**
 * @brief Calculate data bytes from formatted bandwidth
 * @details Helper function
 * @param[in] format Bandwidth format
 * @param[in] amount_bandwidth Bandwidth format
 * @return uint32_t Bytes amount
 *****************************************************************************/
uint32_t sl_iperf_bytes_from_formatted_bandwidth(const sl_iperf_opt_bw_format format,
                                                 const uint32_t amount_bandwidth);

/**************************************************************************//**
 * @brief Check test logger pointer
 * @details Helper function
 * @param test Test
 * @return true On Success
 * @return false On Failure
 *****************************************************************************/
static inline bool sl_iperf_check_test_logger(const sl_iperf_test_t * const test)
{
  return (bool) (test != NULL && test->log != NULL && test->log->print != NULL);
}

/**************************************************************************//**
 * @brief Parse of protocol option
 * @details It parses the protocol from string
 * @param[in] str source string
 * @param[out] protocol got protocol
 * @return true if it is successful
 * @return false if it is failed
 *****************************************************************************/
bool sl_iperf_opt_protocol_from_str(const char *str,
                                    sl_iperf_protocol_t * const protocol);

/**************************************************************************//**
 * @brief Parse of bandwith format option
 * @details It parses the bandwith format from string
 * @param[in] str source string
 * @param[out] bw_format got bandwith format
 * @return true if it is successful
 * @return false if it is failed
 *****************************************************************************/
bool sl_iperf_opt_bw_format_from_str(const char *str,
                                     sl_iperf_opt_bw_format * const bw_format);

/**************************************************************************//**
 * @brief Print UDP Server report header
 * @details Formatted printing of UDP Server report header
 * @param[in,out] test Test descriptor
 *****************************************************************************/
void sl_iperf_test_print_udp_srv_report_hdr(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Print UDP Client report header
 * @details Formatted printing of UDP Client report header
 * @param[in,out] test Test descriptor
 *****************************************************************************/
void sl_iperf_test_print_udp_clnt_report_hdr(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Print UDP connections string
 * @details Printing UDP Server/Client connection parameters,
 *          like IPv6 address and port
 * @param test Test descriptor
 *****************************************************************************/
void sl_iperf_test_print_udp_conn_str(sl_iperf_test_t * const test);

#if SL_IPERF_VERBOSE_MODE

/**************************************************************************//**
 * @brief Print test server header in json format
 * @details This feature can be enabled/disabled in sl_iperf_config.h
 * @param[in] test Test
 * @param[in] hdr Header
 *****************************************************************************/
void sl_iperf_print_test_srv_header_json(sl_iperf_test_t * const test,
                                         const sl_iperf_udp_srv_hdr_t * const hdr);

/**************************************************************************//**
 * @brief Print test client header in json format
 * @details This feature can be enabled/disabled in sl_iperf_config.h
 * @param[in] test Test
 * @param[in] hdr Header
 *****************************************************************************/
void sl_iperf_print_test_clnt_header_json(sl_iperf_test_t * const test,
                                          const sl_iperf_udp_clnt_hdr_t * const hdr);

/**************************************************************************//**
 * @brief Dump test connection buffer in hex format
 * @details This feature can be enabled/disabled in sl_iperf_config.h
 * @param[in,out] test Test descriptor
 * @param[in] size Count of bytes to dump
 *****************************************************************************/
void sl_iper_test_dump_buff(sl_iperf_test_t * const test, const size_t size);

#endif

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
