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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "sl_string.h"
#include "sl_cli.h"
#include "sl_iperf_config.h"
#include "sl_iperf.h"
#include "sl_iperf_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Maximum argument length
#define SL_IPERF_CLI_MAX_STR_ARG_LEN                  (SL_IPERF_UTIL_MAX_STRING_LENGTH)

/// CLI iperf domain name
#define SL_IPERF_CLI_DOMAIN_OPTIONS                   "options"

/// CLI iperf domain name
#define SL_IPERF_CLI_DOMAIN_RESULTS                   "results"

/// CLI option port
#define SL_IPERF_CLI_OPT_PORT                         "port"

/// CLI option remote address
#define SL_IPERF_CLI_OPT_REMOTE_ADDR                  "remote_addr"

/// CLI option bandwidth
#define SL_IPERF_CLI_OPT_BANDWIDTH                    "bandwidth"

/// CLI option packet number
#define SL_IPERF_CLI_OPT_PACKET_NUMBER                "packet_number"

/// CLI option buffer length
#define SL_IPERF_CLI_OPT_BUFFER_LENGTH                "buffer_length"

/// CLI option duration
#define SL_IPERF_CLI_OPT_DURATION                     "duration"

/// CLI option interval
#define SL_IPERF_CLI_OPT_INTERVAL                     "interval"

/// CLI option bandwidth format
#define SL_IPERF_CLI_OPT_BW_FORMAT                    "bw_format"

/// CLI test result
#define SL_IPERF_CLI_RESULTS_JSON                     "json"

/// CLI test result
#define SL_IPERF_CLI_RESULTS_TEXT                     "text"

/// CLI Help argument label
#define SL_IPERF_CLI_HELP_ARG_LABEL                   "help"

/// CLI iPerf Server Help string
#define SL_IPERF_CLI_SERVER_HELP_STR \
  "The iperf server will listen to the options.port for incoming messages.\n"

/// CLI iPerf CLient Help string
#define SL_IPERF_CLI_CLIENT_HELP_STR \
  "The iperf client will send a number of packets for options.duration and report intermediate results every options.interval seconds.\n\
(If options.packet_number is 0, the number of packets is equal to options.bandwidth*options.duration/options.buffer_length.)\n\
At the end of the test, the client and server results will be displayed.\n"

/// CLI iPerf option flags UDP mask
#define SL_IPERF_CLI_OPT_FLAGS_UDP_MASK                (0x00000001UL)

/// CLI iPerf option flags TCP mask
#define SL_IPERF_CLI_OPT_FLAGS_TCP_MASK                (0x00000002UL)

/// CLI iPerf option flags Client mask
#define SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK               (0x00000004UL)

/// CLI iPerf option flags Server mask
#define SL_IPERF_CLI_OPT_FLAGS_SRV_MASK                (0x00000008UL)

/// CLI iPerf option flags string length
#define SL_IPERF_CLI_OPT_FLAGS_STR_LEN                 (7U)

/// CLI iPerf option flags string format
#define SL_IPERF_CLI_OPT_FLAGS_STR_FORMAT              "[%c%c%c%c]"

/// CLI iPerf Get-Set Help string
#define SL_IPERF_CLI_GET_SET_HELP \
  "Help of iperf 'get' and 'set' methods\n\n\
Available sub-domains :\n\
 options\n\
 results\n\n\
[csut]: corresponds to client/server/udp/tcp. Indicates in which case the parameter applies \n\n\
Type 'iperf [get or set] <sub-domain> help'\n\n\
eg. 'iperf get options help' to get the help of all options\n\
    'iperf get options' for all current options values\n\
    'iperf get results' for test results\n\
    'iperf get options.<option>' for a specific option value\n"

/// CLI iPerf options port help string
#define SL_IPERF_CLI_OPT_PORT_HELP \
  "Server port to listen on/connect to [uint16] (default 5001)"

/// CLI iPerf options remote address help string
#define SL_IPERF_CLI_OPT_REMOTE_ADDR_HELP \
  "IPv6 remote host address  [string]"

/// CLI iPerf options buffer length help string
#define SL_IPERF_CLI_OPT_BUFF_LENGTH_HELP \
  "Length of buffer in bytes to read or write  [uint16] (default 1450)"

/// CLI iPerf options bandwidth help string
#define SL_IPERF_CLI_OPT_BANDWIDTH_HELP \
  "Unused if packet_number is set. \
Otherwise used to compute packet_number as bandwidth*duration/buffer_length  [uint32] (default 20000)"

/// CLI iPerf options port help string
#define SL_IPERF_CLI_OPT_BW_FORMAT_HELP \
  "Unused if packet_number is set. \
One of [bits/s, Kbits/s, Mbits/s, Gbits/s, bytes/s, Kbytes/s, Mbytes/s, Gbytes/s] [string] (default bits/s)"

/// CLI iPerf options duration help string
#define SL_IPERF_CLI_OPT_DURATION_HELP \
  "Test duration in seconds [uint16] (default 10)"

/// CLI iPerf options interval help string
#define SL_IPERF_CLI_OPT_INTERVAL_HELP \
  "Seconds between periodic bandwidth reports  [uint16] (default 1)"

/// CLI iPerf options packet number help string
#define SL_IPERF_CLI_OPT_PACKET_NUMBER_HELP \
  "Number of packets to send. (Rules out bandwidth)  [uint16] (default 0=unset)"

/// CLI iPerf results json help string
#define SL_IPERF_CLI_RES_JSON_HELP \
  "Last test result in json format"

/// CLI iPerf results json help string
#define SL_IPERF_CLI_RES_TEXT_HELP \
  "Last test result in text format"

/// Option flags
typedef uint32_t sl_iperf_cli_opt_flags_t;

/// Perpare getter buffer macro function
#define __prepare_getter_buff(__format, __val)                             \
  do {                                                                     \
    snprintf(_getter_buff, SL_IPERF_CLI_MAX_STR_ARG_LEN, __format, __val); \
    return (const char *)_getter_buff;                                     \
  } while (0)

/// function pointer type for setter
typedef void (*sl_iperf_cli_cmd_setter) (void *val);

/// function pointer type for getter
typedef const char * (*sl_iperf_cli_cmd_getter) (void);

/// CLI argument type definition
typedef enum sl_iperf_cli_arg_type {
  /// String argument
  SL_IPERF_CLI_ARG_TYPE_STRING = 0,
  /// Uint32 argument
  SL_IPERF_CLI_ARG_TYPE_UINT32,
  /// Uint16 argument
  SL_IPERF_CLI_ARG_TYPE_UINT16
} sl_iperf_cli_arg_type_t;

/// structure for iPerf CLI settings
typedef struct sl_iperf_cli_setting {
  /// domain
  const char *domain;
  /// name
  const char * name;
  /// type
  sl_iperf_cli_arg_type_t type;
  /// setter
  sl_iperf_cli_cmd_setter setter;
  /// getter
  sl_iperf_cli_cmd_getter getter;
  /// help
  const char *help;
  /// flags
  sl_iperf_cli_opt_flags_t flags;
} sl_iperf_cli_property_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// port
/**************************************************************************//**
 * @brief Port setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_port_setter(void *val);

/**************************************************************************//**
 * @brief Port getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_port_getter(void);

// remote address
/**************************************************************************//**
 * @brief Remote address setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_remote_address_setter(void *val);

/**************************************************************************//**
 * @brief Remote address getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_remote_address_getter(void);

// bandwidth
/**************************************************************************//**
 * @brief Remote address setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_bandwidth_setter(void *val);

/**************************************************************************//**
 * @brief Remote address getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_bandwidth_getter(void);

// bytes number
/**************************************************************************//**
 * @brief Byte number setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_packet_number_setter(void *val);

/**************************************************************************//**
 * @brief Byte number getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_packet_number_getter(void);

// buffer length
/**************************************************************************//**
 * @brief Buffer length setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_buffer_length_setter(void *val);
/**************************************************************************//**
 * @brief Buffer length getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_buffer_length_getter(void);

// duration
/**************************************************************************//**
 * @brief Duration setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_duration_setter(void *val);
/**************************************************************************//**
 * @brief Duration getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_duration_getter(void);

// interval
/**************************************************************************//**
 * @brief Interval setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_interval_setter(void *val);

/**************************************************************************//**
 * @brief Interval getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_interval_getter(void);

// bandwidth format
/**************************************************************************//**
 * @brief Bandwith format setter
 * @param[in] val value
 *****************************************************************************/
static inline void _opt_bw_format_setter(void *val);

/**************************************************************************//**
 * @brief Bandwith format getter
 * @return string buffer ptr
 *****************************************************************************/
static inline const char *_opt_bw_format_getter(void);

/**************************************************************************//**
 * @brief Test result getter
 * @return Always NULL
 *****************************************************************************/
static inline const char *_results_json_getter(void);

/**************************************************************************//**
 * @brief Test result getter
 * @return Always NULL
 *****************************************************************************/
static inline const char *_results_text_getter(void);

/**************************************************************************//**
 * @brief Print property value
 * @details Helper function
 * @param[in] setting CLI setting
 *****************************************************************************/
static inline void _print_property_val(const sl_iperf_cli_property_t * const setting);

/**************************************************************************//**
 * @brief Print property help
 * @details Helper function
 * @param[in] setting
 *****************************************************************************/
static inline void _print_property_help(const sl_iperf_cli_property_t * const setting);

/**************************************************************************//**
 * @brief Print properties in domain with their values
 * @details Helper function
 * @param[in] domain Domain
 * @return true If at least one valid property is found
 * @return false Properties not found
 *****************************************************************************/
static bool _print_properties_with_val(const char *domain);

/**************************************************************************//**
 * @brief Print properties in domain with their help string
 * @details Helper function
 * @param[in] domain
 * @return true
 * @return false
 *****************************************************************************/
static bool _print_properties_with_help(const char *domain);

/**************************************************************************//**
 * @brief Execute test
 * @details Helper function
 * @param[in] mode Mode
 * @param[in] protocol Protocol
 *****************************************************************************/
static void _exec_test(const sl_iperf_mode_t mode,
                       const sl_iperf_protocol_t protocol);

/**************************************************************************//**
 * @brief Is argument a help request
 * @details Helper function
 * @param[in] arg Argument string
 * @return true Help string key
 * @return false Not help string key
 *****************************************************************************/
static inline bool _is_argument_help(const char * arg);

/**************************************************************************//**
 * @brief Convert flags to string
 * @details Helper function
 * @param[in] flags Flags
 * @return const char* String representation of flags
 *****************************************************************************/
static const char *_flags_to_str(const sl_iperf_cli_opt_flags_t flags);

/**************************************************************************//**
 * @brief Get property by domain and key
 * @details Helper function
 * @param[in] domain Domain
 * @param[in] key Key
 * @return sl_iperf_cli_property_t* Property ptr if it's found or NULL
 *****************************************************************************/
static inline sl_iperf_cli_property_t * _get_property(const char *domain,
                                                      const char *key);

/**************************************************************************//**
 * @brief Get domain and key
 * @details Helper function
 * @param[in] str_src String source
 * @param[out] domain_dst
 * @param[out] key_dst
 *****************************************************************************/
static void _get_domain_key(const char *str_src,
                            char **domain_dst,
                            char **key_dst);

/**************************************************************************//**
 * @brief Check argument value of uint16 and uint32 
 * @details Helper function
 * @param[in] arg_type Argument type
 * @param[in] val Value
 * @return true Valid value
 * @return false Non-valid value
 *****************************************************************************/
static inline bool _check_arg_uint_val(const sl_iperf_cli_arg_type_t arg_type, 
                                       const int64_t val);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Getter buffer
static char _getter_buff[SL_IPERF_CLI_MAX_STR_ARG_LEN] = { 0 };

/// iperf structure with default settings of options
static sl_iperf_opt_t _options = {
  .bandwidth   = SL_IPERF_CLIENT_DEFAULT_BANDWIDTH_BPS,
  .bw_format   = SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC,
  .interval_ms = SL_IPERF_DEFAULT_INTERVAL_MS,
  .mode        = SL_IPERF_MODE_SERVER,
  .protocol    = SL_IPERF_IPROTOV6_UDP,
  .port        = SL_IPERF_SERVER_DEFAULT_PORT,
  .duration_ms = SL_IPERF_CLIENT_DEFAULT_SEND_TIME_MS,
  .buf_len     = SL_IPERF_BUFFER_SIZE
};

/// iPerf settings that contains the parameters for test
static sl_iperf_cli_property_t _iperf_properties[] = {
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_PORT,
    .type   = SL_IPERF_CLI_ARG_TYPE_UINT16,
    .setter = _opt_port_setter,
    .getter = _opt_port_getter,
    .help   = SL_IPERF_CLI_OPT_PORT_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_SRV_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_REMOTE_ADDR,
    .type   = SL_IPERF_CLI_ARG_TYPE_STRING,
    .setter = _opt_remote_address_setter,
    .getter = _opt_remote_address_getter,
    .help   = SL_IPERF_CLI_OPT_REMOTE_ADDR_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_BANDWIDTH,
    .type   = SL_IPERF_CLI_ARG_TYPE_UINT32,
    .setter = _opt_bandwidth_setter,
    .getter = _opt_bandwidth_getter,
    .help   = SL_IPERF_CLI_OPT_BANDWIDTH_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_BW_FORMAT,
    .type   = SL_IPERF_CLI_ARG_TYPE_STRING,
    .setter = _opt_bw_format_setter,
    .getter = _opt_bw_format_getter,
    .help   = SL_IPERF_CLI_OPT_BW_FORMAT_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_SRV_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_PACKET_NUMBER,
    .type   = SL_IPERF_CLI_ARG_TYPE_UINT16,
    .setter = _opt_packet_number_setter,
    .getter = _opt_packet_number_getter,
    .help   = SL_IPERF_CLI_OPT_PACKET_NUMBER_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_BUFFER_LENGTH,
    .type   = SL_IPERF_CLI_ARG_TYPE_UINT16,
    .setter = _opt_buffer_length_setter,
    .getter = _opt_buffer_length_getter,
    .help   = SL_IPERF_CLI_OPT_BUFFER_LENGTH,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_DURATION,
    .type   = SL_IPERF_CLI_ARG_TYPE_UINT16,
    .setter = _opt_duration_setter,
    .getter = _opt_duration_getter,
    .help   = SL_IPERF_CLI_OPT_DURATION_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_OPTIONS,
    .name   = SL_IPERF_CLI_OPT_INTERVAL,
    .type   = SL_IPERF_CLI_ARG_TYPE_UINT16,
    .setter = _opt_interval_setter,
    .getter = _opt_interval_getter,
    .help   = SL_IPERF_CLI_OPT_INTERVAL_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_SRV_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_RESULTS,
    .name   = SL_IPERF_CLI_RESULTS_JSON,
    .type   = SL_IPERF_CLI_ARG_TYPE_STRING,
    .setter = NULL,
    .getter = _results_json_getter,
    .help   = SL_IPERF_CLI_RES_JSON_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_SRV_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  },
  {
    .domain = SL_IPERF_CLI_DOMAIN_RESULTS,
    .name   = SL_IPERF_CLI_RESULTS_TEXT,
    .type   = SL_IPERF_CLI_ARG_TYPE_STRING,
    .setter = NULL,
    .getter = _results_text_getter,
    .help   = SL_IPERF_CLI_RES_TEXT_HELP,
    .flags  = SL_IPERF_CLI_OPT_FLAGS_UDP_MASK
              | SL_IPERF_CLI_OPT_FLAGS_SRV_MASK
              | SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK
  }
};

/// iPerf options setting paramters number
static const size_t _iperf_properties_size = sizeof(_iperf_properties) / sizeof(sl_iperf_cli_property_t);

/// iPerf last test result
static sl_iperf_test_t _last_test = { 0U };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief iPerf CLI set parameter
 * @details CLI function
 * @param[in] arguments Arguments
 *****************************************************************************/
void sl_iperf_cli_set(sl_cli_command_arg_t *arguments)
{
  uint8_t arg_cnt                = 0U;
  char *arg0_str                 = NULL;
  char *arg1_str                 = NULL;
  char *domain                   = NULL;
  char *key                      = NULL;
  sl_iperf_cli_property_t * prop = NULL;
  int64_t i_val                  = 0LL;

  arg_cnt = (uint8_t) sl_cli_get_argument_count(arguments);

  switch (arg_cnt) {
    case 0U:
      // case:
      // $ iperf set
      // print short help of get-set
      printf("%s\n", SL_IPERF_CLI_GET_SET_HELP);
      return;
    case 1U:
      arg0_str = sl_cli_get_argument_string(arguments, 0U);
      break;
    case 2U:
      arg0_str = sl_cli_get_argument_string(arguments, 0U);
      arg1_str = sl_cli_get_argument_string(arguments, 1U);
      break;
    default:
      printf("[Not valid argument count for 'iperf set']\n");
      return;
  }

  // get domain and key
  if (arg0_str != NULL) {
    _get_domain_key(arg0_str, &domain, &key);
  }

  // get property
  if (key != NULL) {
    prop = _get_property(domain, key);
    if (prop == NULL) {
      printf("[Not valid iPerf domain and key]\n");
      return;
    }
  }

  // check setter
  if (prop != NULL && prop->setter == NULL) {
    printf("[iPerf property cannot be set]\n");
    return;
  }

  // case:
  // $ iperf set help
  // print short help of get-set
  if (domain != NULL && _is_argument_help(domain) && key == NULL && arg1_str == NULL) {
    printf("%s\n", SL_IPERF_CLI_GET_SET_HELP);
    return;

    // case:
    // $ iperf set <domain.key>
    // print only one property
  } else if (domain != NULL && key != NULL && arg1_str == NULL) {
    _print_property_val(prop);
    return;

    // case:
    // $ iperf set <domain.key> help
    // print only one property
  } else if (domain != NULL
             && key != NULL
             && arg1_str != NULL
             && _is_argument_help(arg1_str)) {
    _print_property_help(prop);
    return;

    // case:
    // $ iperf set <domain> help
    // print only one property
  } else if (domain != NULL
             && key == NULL
             && arg1_str != NULL
             && _is_argument_help(arg1_str)) {
    _print_properties_with_help(domain);
    return;

    // case:
    // $ iperf set <domain.key> value
    // print only one property
  } else if (domain != NULL
             && key != NULL
             && arg1_str != NULL) {
    switch (prop->type) {
      case SL_IPERF_CLI_ARG_TYPE_STRING:
        prop->setter((void *) arg1_str);
        break;
      case SL_IPERF_CLI_ARG_TYPE_UINT16:
      case SL_IPERF_CLI_ARG_TYPE_UINT32:
        i_val = atoll(arg1_str);
        if (_check_arg_uint_val(prop->type, i_val)) {
          prop->setter((void *) &i_val);
        } else {
          printf("[Not valid iPerf command argument value]\n");
        }
        break;
      default:
        printf("[Not valid iPerf command argument type]\n");
        break;
    }

    _print_property_val(prop);
    return;
  }

  printf("[Not valid iPerf command]\n");
}

/**************************************************************************//**
 * @brief iPerf CLI get parameter
 * @details CLI function
 * @param[in] arguments Arguments
 *****************************************************************************/
void sl_iperf_cli_get(sl_cli_command_arg_t *arguments)
{
  uint8_t arg_cnt                = 0U;
  char *arg0_str                 = NULL;
  char *arg1_str                 = NULL;
  char *domain                   = NULL;
  char *key                      = NULL;
  sl_iperf_cli_property_t * prop = NULL;

  arg_cnt = (uint8_t) sl_cli_get_argument_count(arguments);

  switch (arg_cnt) {
    case 0U:
      // case:
      // $ iperf get
      // print short help of get-set
      printf("%s\n", SL_IPERF_CLI_GET_SET_HELP);
      return;
    case 1U:
      arg0_str = sl_cli_get_argument_string(arguments, 0U);
      break;
    case 2U:
      arg0_str = sl_cli_get_argument_string(arguments, 0U);
      arg1_str = sl_cli_get_argument_string(arguments, 1U);
      break;
    default:
      printf("[Not valid argument count for 'iperf get']\n");
      return;
  }

  // get domain and key
  if (arg0_str != NULL) {
    _get_domain_key(arg0_str, &domain, &key);
  }

  // get property
  if (key != NULL) {
    prop = _get_property(domain, key);
    if (prop == NULL) {
      printf("[Not valid iPerf domain and key]\n");
      return;
    }
  }

  // case:
  // $ iperf get help
  // print short help of get-set
  if (domain != NULL && _is_argument_help(domain) && key == NULL && arg1_str == NULL) {
    printf("%s\n", SL_IPERF_CLI_GET_SET_HELP);
    return;

    // case:
    // $ iperf get <domain.key>
    // print only one property
  } else if (domain != NULL && key != NULL && arg1_str == NULL) {
    _print_property_val(prop);
    return;

    // case:
    // $ iperf get <domain>
    // print all properties of given domain
  } else if (domain != NULL && key == NULL && arg1_str == NULL) {
    if (_print_properties_with_val(domain)) {
      return;
    }

    // case:
    // $ iperf get <domain.key> help
    // print expected property help
  } else if (domain != NULL
             && key != NULL
             && arg1_str != NULL
             && _is_argument_help(arg1_str)) {
    _print_property_help(prop);
    return;

    // case:
    // $ iperf get <domain> help
    // print all properties help
  } else if (domain != NULL
             && key == NULL
             && arg1_str != NULL
             && _is_argument_help(arg1_str)) {
    if (_print_properties_with_help(domain)) {
      return;
    }
  }
  printf("[Not valid iPerf command]\n");
}

/**************************************************************************//**
 * @brief iPerf CLI execute server test
 * @details CLI function
 * @param[in] arguments Arguments
 *****************************************************************************/
void sl_iperf_cli_exec_server(sl_cli_command_arg_t *arguments)
{
  uint8_t arg_cnt     = 0U;
  const char *str_arg = NULL;
  arg_cnt = (uint8_t) sl_cli_get_argument_count(arguments);
  if (arg_cnt) {
    str_arg = sl_cli_get_argument_string(arguments, 0);
    printf("%s\n", str_arg != NULL && _is_argument_help(str_arg)
           ? SL_IPERF_CLI_SERVER_HELP_STR
           : "[Bad argument for execute Server]\n");
    return;
  }
  _exec_test(SL_IPERF_MODE_SERVER, SL_IPERF_IPROTOV6_UDP);
}

/**************************************************************************//**
 * @brief iPerf CLI execute server test
 * @details CLI function
 * @param[in] arguments Arguments
 *****************************************************************************/
void sl_iperf_cli_exec_client(sl_cli_command_arg_t *arguments)
{
  uint8_t arg_cnt     = 0U;
  const char *str_arg = NULL;
  arg_cnt = (uint8_t) sl_cli_get_argument_count(arguments);
  if (arg_cnt) {
    str_arg = sl_cli_get_argument_string(arguments, 0);
    printf("%s\n", str_arg != NULL && _is_argument_help(str_arg)
           ? SL_IPERF_CLI_CLIENT_HELP_STR
           : "[Bad argument for execute Client]\n");
    return;
  }
  _exec_test(SL_IPERF_MODE_CLIENT, SL_IPERF_IPROTOV6_UDP);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

// port
static inline void _opt_port_setter(void *val)
{
  _options.port = *(uint16_t*)val;
}

static inline const char *_opt_port_getter(void)
{
  __prepare_getter_buff("%u", _options.port);
}

// remote address
static inline void _opt_remote_address_setter(void *val)
{
  (void)strncpy(_options.remote_addr, val, SL_IPERF_IP_STR_BUFF_LEN);
}

static inline const char *_opt_remote_address_getter(void)
{
  __prepare_getter_buff("\"%s\"", _options.remote_addr);
}

// bandwidth
static inline void _opt_bandwidth_setter(void *val)
{
  _options.bandwidth =  *(uint32_t*)val;
}

static inline const char *_opt_bandwidth_getter(void)
{
  __prepare_getter_buff("%lu", _options.bandwidth);
}

// packet number
static inline void _opt_packet_number_setter(void *val)
{
  _options.packet_nbr = *(uint16_t*)val;
}

static inline const char *_opt_packet_number_getter(void)
{
  __prepare_getter_buff("%u", _options.packet_nbr);
}

// buffer length
static inline void _opt_buffer_length_setter(void *val)
{
  uint16_t value = *(uint16_t*)val;
  if (!value || value > SL_IPERF_BUFFER_SIZE) {
    printf("[Not valid buffer size]\n");
    return;
  }
  _options.buf_len = value;
}

static inline const char *_opt_buffer_length_getter(void)
{
  __prepare_getter_buff("%u bytes", _options.buf_len);
}

// duration
static inline void _opt_duration_setter(void *val)
{
  uint32_t val32 = 0UL;
  val32 = *(uint32_t*) val * SL_IPERF_TIME_S_TO_MS_ML;
  if (val32 > UINT16_MAX) {
    printf("[Duration Time value is overflowed]\n");
  } else {
    _options.duration_ms = (uint16_t) val32;
  }
}

static inline const char *_opt_duration_getter(void)
{
  __prepare_getter_buff("%u s", _options.duration_ms  / SL_IPERF_TIME_S_TO_MS_ML);
}

// interval
static inline void _opt_interval_setter(void *val)
{
  uint32_t val32 = 0UL;
  val32 = *(uint32_t*) val * SL_IPERF_TIME_S_TO_MS_ML;
  if (val32 > UINT16_MAX) {
    printf("[Interval Time value is overflowed]\n");
  } else {
    _options.interval_ms = (uint16_t) val32;
  }
}

static inline const char *_opt_interval_getter(void)
{
  __prepare_getter_buff("%u s", _options.interval_ms / SL_IPERF_TIME_S_TO_MS_ML);
}

// bandwidth format
static inline void _opt_bw_format_setter(void *val)
{
  if (!sl_iperf_opt_bw_format_from_str(val, &_options.bw_format)) {
    printf("[Wrong bandwidth format added]\n");
  }
}

static inline sl_iperf_cli_property_t * _get_property(const char *domain, const char *key)
{
  for (size_t i = 0U; i < _iperf_properties_size; ++i) {
    if (!strncmp(domain, _iperf_properties[i].domain, SL_IPERF_CLI_MAX_STR_ARG_LEN)
        && !strncmp(key, _iperf_properties[i].name, SL_IPERF_CLI_MAX_STR_ARG_LEN)) {
      return &_iperf_properties[i];
    }
  }
  return NULL;
}

static inline void _print_property_val(const sl_iperf_cli_property_t * const setting)
{
  const char * res = NULL;
  if (setting->getter != NULL) {
    res = setting->getter();
    if (res != NULL) {
      printf("%s.%s = %s\n", setting->domain, setting->name, res);
    }
  }
}

static inline void _print_property_help(const sl_iperf_cli_property_t * const setting)
{
  printf("%s %s.%-24s %s\n",
         _flags_to_str(setting->flags),
         setting->domain,
         setting->name,
         setting->help == NULL ? "" : setting->help);
}

static bool _print_properties_with_val(const char *domain)
{
  const char *get_res = NULL;
  bool res            = false;
  for (size_t i = 0U; i < _iperf_properties_size; ++i) {
    if (strncmp(_iperf_properties[i].domain, domain, SL_IPERF_CLI_MAX_STR_ARG_LEN)
        || _iperf_properties[i].getter == NULL) {
      continue;
    }
    get_res = _iperf_properties[i].getter();
    if (get_res != NULL) {
      printf("%s %s.%s = %s\n",
            _flags_to_str(_iperf_properties[i].flags),
            _iperf_properties[i].domain,
            _iperf_properties[i].name,
            get_res);
    }
    if (!res) {
      res = true;
    }
  }

  return res;
}

static bool _print_properties_with_help(const char *domain)
{
  bool res = false;
  for (size_t i = 0U; i < _iperf_properties_size; ++i) {
    if (strncmp(_iperf_properties[i].domain, domain, SL_IPERF_CLI_MAX_STR_ARG_LEN)) {
      continue;
    }
    printf("%s %s.%-24s %s\n",
           _flags_to_str(_iperf_properties[i].flags),
           _iperf_properties[i].domain,
           _iperf_properties[i].name,
           _iperf_properties[i].help == NULL ? "''" : _iperf_properties[i].help);
    if (!res) {
      res = true;
    }
  }
  return res;
}

static inline const char *_results_json_getter(void)
{
  sl_iperf_print_test_log_json(&_last_test);
  return NULL;
}

static inline const char *_results_text_getter(void)
{
  // print connection string with addresses
  sl_iperf_test_print_udp_conn_str(&_last_test);

  // print report header
  if (_last_test.opt.mode == SL_IPERF_MODE_SERVER) {
    sl_iperf_test_print_udp_srv_report_hdr(&_last_test);
  } else {
    sl_iperf_test_print_udp_clnt_report_hdr(&_last_test);
  }
  // print report summary
  sl_iperf_test_calculate_average_bandwidth(&_last_test);
  return NULL;
}

static inline const char *_opt_bw_format_getter(void)
{
  __prepare_getter_buff("%s", sl_iperf_opt_bw_format_to_str(_options.bw_format));
}

static void _exec_test(const sl_iperf_mode_t mode,
                       const sl_iperf_protocol_t protocol)
{
  sl_iperf_test_t test = { 0U };

  _options.mode = mode;
  _options.protocol = protocol;
  // init test
  sl_iperf_test_init(&test, _options.mode, _options.protocol);

  // set stored options
  memcpy(&test.opt, &_options, sizeof(sl_iperf_opt_t));

  // adds the test to the queue
  if (!sl_iperf_test_add(&test)) {
    printf("[Adding test to queue failed]\n");
    return;
  }

  if (!sl_iperf_test_get(&_last_test)) {
    printf("[Getting test to queue failed]\n");
    return;
  }
}

static inline bool _is_argument_help(const char * arg)
{
  return (bool)(!strncmp(arg, SL_IPERF_CLI_HELP_ARG_LABEL,
                         SL_IPERF_CLI_MAX_STR_ARG_LEN));
}

static const char *_flags_to_str(const sl_iperf_cli_opt_flags_t flags)
{
  static char buff[SL_IPERF_CLI_OPT_FLAGS_STR_LEN] = { 0U };

  snprintf(buff,
           SL_IPERF_CLI_OPT_FLAGS_STR_LEN,
           SL_IPERF_CLI_OPT_FLAGS_STR_FORMAT,
           flags & SL_IPERF_CLI_OPT_FLAGS_CLNT_MASK ? 'c' : ' ',
           flags & SL_IPERF_CLI_OPT_FLAGS_SRV_MASK  ? 's' : ' ',
           flags & SL_IPERF_CLI_OPT_FLAGS_UDP_MASK  ? 'u' : ' ',
           flags & SL_IPERF_CLI_OPT_FLAGS_TCP_MASK  ? 't' : ' ');

  return (const char *) buff;
}

static void _get_domain_key(const char *str_src,
                            char **domain_dst,
                            char **key_dst)
{
  static char domain_key_buff[SL_IPERF_CLI_MAX_STR_ARG_LEN] = { 0 };

  strncpy(domain_key_buff, str_src, SL_IPERF_CLI_MAX_STR_ARG_LEN);

  *domain_dst = domain_key_buff;
  *key_dst = NULL;

  for (size_t i = 0; i < SL_IPERF_CLI_MAX_STR_ARG_LEN - 1; ++i) {
    if (domain_key_buff[i] == '.') {
      domain_key_buff[i] = '\0';
      *key_dst = &domain_key_buff[i + 1];
      break;
    }
  }

  domain_key_buff[SL_IPERF_CLI_MAX_STR_ARG_LEN - 1] = '\0';
}

static inline bool _check_arg_uint_val(const sl_iperf_cli_arg_type_t arg_type, 
                                       const int64_t val)
{
  if (val < 0LL || 
      (arg_type == SL_IPERF_CLI_ARG_TYPE_UINT32 && val > UINT32_MAX) || 
      (arg_type == SL_IPERF_CLI_ARG_TYPE_UINT16 && val > UINT16_MAX))  {
    return false;
  }
  return true;
}