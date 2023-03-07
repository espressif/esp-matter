/***************************************************************************//**
 * @file
 * @brief Parse source file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>

/* BG stack headers */
//#include "gecko_bglib.h"
#include "app_log.h"

/* Own header */
#include "parse.h"
#include "config.h"

/***************************************************************************************************
 * Static Function Declarations
 **************************************************************************************************/
static int parse_address(const char *str, bd_addr *addr);
static void usage(void);
static void print_configuration(void);

/***************************************************************************************************
 * Public Variables
 **************************************************************************************************/

/***************************************************************************************************
 * Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Initialize adc_sample_rate variable in configuration structure by data from argument list.
 *  \param[in]  sample rate
 **************************************************************************************************/
void init_sample_rate(adc_sample_rate_t sr)
{
  switch (sr) {
    case sr_8k:
      CONF_get()->adc_sample_rate = sr_8k;
      break;
    case sr_16k:
    default:
      CONF_get()->adc_sample_rate = sr_16k;
      break;
  }
}

/***********************************************************************************************//**
 *  \brief  Print bluetooth address on stdout.
 *  \param[in]  bluetooth address
 **************************************************************************************************/
static void print_address(bd_addr *addr)
{
  app_log("Remote address:          %02x:%02x:%02x:%02x:%02x:%02x\n", addr->addr[5], addr->addr[4], addr->addr[3], addr->addr[2], addr->addr[1], addr->addr[0]);
}

/***********************************************************************************************//**
 *  \brief  Parse bluetooth address.
 *  \param[in]  data to parse
 *  \param[out] parsed bluetooth address
 *  \return 0 if success, otherwise -1
 **************************************************************************************************/
static int parse_address(const char *str, bd_addr *addr)
{
  int a[6];
  int i;
  i = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
             &a[5],
             &a[4],
             &a[3],
             &a[2],
             &a[1],
             &a[0]
             );
  if (i != 6) {
    return -1;
  }

  for (i = 0; i < 6; i++) {
    addr->addr[i] = (uint8_t)a[i];
  }

  return 0;
}

/***********************************************************************************************//**
 *  \brief  Print example of usage that application on stdout.
 **************************************************************************************************/
static void usage(void)
{
  app_log("Example of usage:\n");
  app_log("  voice -v -p COM1 -b 115200 -a 00:0b:57:1a:8c:2d -s 16\n");
  app_log("  voice -p COM1 -b 115200 -a 00:0b:57:1a:8c:2d \n");
  app_log("  voice -h \n");
}

/***********************************************************************************************//**
 *  \brief  Print help on stdout.
 **************************************************************************************************/
void help(void)
{
  app_log("Help:\n");
  app_log("-p <port>       - COM port\n");
  app_log("-b <baud_rate>  - Baud rate.\n");
  app_log("                  Default %d b/s.\n", DEFAULT_UART_BAUD_RATE);
  app_log("-o <file_name>  - Output file name.\n");
  app_log("                  Audio data send to stdout by default.\n");
  app_log("-a <bt_address> - Remote device bluetooth address. \n");
  app_log("                  No default bluetooth address.\n");
  app_log("-s <8/16>       - ADC sampling rate.\n");
  app_log("                  8 or 16 kHz sampling rate can be used. Default - 16 kHz.\n");
  app_log("-f <1/0>        - Enable/Disable filtering.\n");
  app_log("                  Default filtering disabled. When filtering enabled HPF filter is used.\n");
  app_log("-e <1/0>        - Enable/Disable encoding.\n");
  app_log("                  Encoding enabled by default.\n");
  app_log("-t <1/0>        - Enable/Disable transfer status.\n");
  app_log("                  Transfer status enabled by default.\n");
  app_log("-h              - Help\n");
  app_log("-v              - Verbose\n");
  usage();
  exit(EXIT_SUCCESS);
}

/***********************************************************************************************//**
 *  \brief  Print configuration parameters on stdout.
 **************************************************************************************************/
static void print_configuration(void)
{
  app_log("Parameters:\n");
  app_log("  Baud rate:               %d\n", CONF_get()->baud_rate);
  app_log("  UART port:               %s\n", CONF_get()->uart_port);
  if ( CONF_get()->output_to_stdout == true) {
    app_log("  Audio data send to:      stdout\n");
  } else {
    app_log("  Audio data store into:   %s\n", CONF_get()->out_file_name);
  }
  app_log("  "); print_address(&CONF_get()->remote_address);
  app_log("  Audio data notification: %s\n", CONF_get()->audio_data_notification ? "Enabled" : "Disabled");
  app_log("  ADC sample rate:         %d[kHz]\n", CONF_get()->adc_sample_rate);
  app_log("  Filtering:               %s\n", CONF_get()->filter_enabled ? "Enabled" : "Disabled");
  app_log("  Encoding:                %s\n", CONF_get()->encoding_enabled ? "Enabled" : "Disabled");
  app_log("  Transfer status:         %s\n", CONF_get()->transfer_status ? "Enabled" : "Disabled");
  app_log("\n");
  return;
}

/***********************************************************************************************//**
 *  \brief  Add extension to output file depending on application parameters.
 **************************************************************************************************/
static void add_extension_to_file(void)
{
  char *ptr = NULL;

  if ( strcmp(CONF_get()->out_file_name, "-") == 0 ) {
    CONF_get()->output_to_stdout = true;
    return;
  }

  size_t ptrLen = 1 + strlen(CONF_get()->out_file_name) + 4;
  ptr = (char *)malloc(ptrLen);

  if ( ptr == NULL) {
    DEBUG_ERROR("Memory allocation failed. Exiting.\n,");
    exit(1);
  }

  strcpy(ptr, CONF_get()->out_file_name);

  if ( CONF_get()->encoding_enabled ) {
    strcat(ptr, IMA_FILE_EXTENSION);
  } else {
    strcat(ptr, S16_FILE_EXTENSION);
  }

  if ( CONF_get()->out_file_name != NULL) {
    free(CONF_get()->out_file_name);
    CONF_get()->out_file_name = NULL;
  }

  CONF_get()->out_file_name = malloc(1 + strlen(ptr));
  strcpy(CONF_get()->out_file_name, ptr);

  free(ptr);
}

/***********************************************************************************************//**
 *  \brief  Parse application parameters.
 *  \param[in] argc Argument count.
 *  \param[in] argv Buffer contaning application parameters.
 **************************************************************************************************/
void PAR_parse(int argc, char **argv)
{
  static char uart_port_name[STR_UART_PORT_SIZE];
  bool verbose = false;

  if (argc == 1) {
    help();
  }

  for (uint8_t i = 0; i < argc; i++) {
    if (argv[i][0] == '-') {
      if ( argv[i][1] == 'p') {
        size_t len_arg = strlen(argv[i + 1]);
        size_t len = (len_arg > (STR_UART_PORT_SIZE - 1)) ? STR_UART_PORT_SIZE : len_arg;

        memcpy(uart_port_name, argv[i + 1], len);
        uart_port_name[len + 1] = '\0';
        CONF_get()->uart_port = uart_port_name;
      }

      if ( argv[i][1] == 'o') {
        size_t fLen = strlen(argv[i + 1]) + 1;
        CONF_get()->out_file_name = malloc(fLen);
        memcpy(CONF_get()->out_file_name, argv[i + 1], fLen);
      }

      if ( argv[i][1] == 'a') {
        if (parse_address(argv[i + 1], &CONF_get()->remote_address)) {
          DEBUG_ERROR("Unable to parse address %s", argv[i + 1]);
          exit(EXIT_FAILURE);
        } else {
          CONF_get()->remote_address_set = true;
        }
      }

      if ( argv[i][1] == 'b') {
        CONF_get()->baud_rate = (int)atoi(argv[i + 1]);
      }

      if ( argv[i][1] == 's') {
        init_sample_rate(atoi(argv[i + 1]));
      }

      if ( argv[i][1] == 'f') {
        CONF_get()->filter_enabled = (bool)atoi(argv[i + 1]);
      }

      if ( argv[i][1] == 'e') {
        CONF_get()->encoding_enabled = (bool)atoi(argv[i + 1]);
      }

      if ( argv[i][1] == 't') {
        CONF_get()->transfer_status = (bool)atoi(argv[i + 1]);
      }

      if ( argv[i][1] == 'h') {
        help();
      }

      if ( argv[i][1] == 'v') {
        verbose = true;
      }
    }
  }

  add_extension_to_file();

  if (verbose) {
    print_configuration();
  }
}
