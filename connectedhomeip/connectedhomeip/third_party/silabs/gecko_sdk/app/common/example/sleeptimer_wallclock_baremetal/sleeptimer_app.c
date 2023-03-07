/***************************************************************************//**
 * @file
 * @brief Sleeptimer examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "sl_cli.h"
#include "sl_cli_instances.h"
#include "sl_sleeptimer.h"

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

// Handler for CLI command set_unix_time
void sleeptimer_app_set_time_unix(sl_cli_command_arg_t *arguments)
{
  // Get the first argument (index 0)
  sl_sleeptimer_timestamp_t time = sl_cli_get_argument_uint32(arguments, 0);

  if (SL_STATUS_OK == sl_sleeptimer_set_time(time)) {
    printf("Time set to %lu (Unix epoch)\r\n", time);
  } else {
    printf("Error setting time\r\n");
  }
}

// Handler for CLI command get_unix_time
void sleeptimer_app_get_time_unix(sl_cli_command_arg_t *arguments)
{
  (void)&arguments;

  printf("Current time (Unix epoch) is %lu\r\n", sl_sleeptimer_get_time());
}

// Handler for CLI command set_ntp_time
void sleeptimer_app_set_time_ntp(sl_cli_command_arg_t *arguments)
{
  // Get the first argument (index 0)
  uint32_t ntp_time = sl_cli_get_argument_uint32(arguments, 0);
  sl_sleeptimer_timestamp_t unix_time;

  // Must convert NTP time to Unix epoch to set time in sleeptimer
  if (SL_STATUS_OK == sl_sleeptimer_convert_ntp_time_to_unix(ntp_time, &unix_time) && (SL_STATUS_OK == sl_sleeptimer_set_time(unix_time))) {
    printf("Time set to %lu (NTP epoch)\r\n", ntp_time);
  } else {
    printf("Error setting time\r\n");
  }
}

// Handler for CLI command get_ntp_time
void sleeptimer_app_get_time_ntp(sl_cli_command_arg_t *arguments)
{
  (void)&arguments;

  sl_sleeptimer_timestamp_t unix_time = sl_sleeptimer_get_time();
  uint32_t ntp_time;

  if (SL_STATUS_OK == sl_sleeptimer_convert_unix_time_to_ntp(unix_time, &ntp_time)) {
    printf("Current time (NTP epoch) is %lu\r\n", ntp_time);
  } else {
    printf("Error getting NTP epoch time\r\n");
  }
}

// Handler for CLI command get_datetime
void sleeptimer_app_get_datetime(sl_cli_command_arg_t *arguments)
{
  (void)&arguments;

  //Refer to strftime() from UNIX for format rules
  const uint8_t *format = (const uint8_t *)"%Y-%m-%d %T";
  sl_sleeptimer_date_t date;
  char date_string[64];

  if (0 == sl_sleeptimer_get_datetime(&date)
      && 0 != sl_sleeptimer_convert_date_to_str(date_string, sizeof(date_string), format, &date)) {
    printf("Current date is %s\r\n", date_string);
  } else {
    printf("Error getting current date\r\n");
  }
}

// Handler for CLI command set_datetime
void sleeptimer_app_set_datetime(sl_cli_command_arg_t *arguments)
{
  char *date_ptr = sl_cli_get_argument_string(arguments, 0);
  char *time_ptr = sl_cli_get_argument_string(arguments, 1);

  int year, month, day, hour, minute, second;
  sl_sleeptimer_date_t date = { 0 };

  // expecting format "YYYY-MM-DD H:M:S", eg "2020-7-10 11:30:00"
  const char *date_format = "%4d-%2d-%2d";
  const char *time_format = "%2d:%2d:%2d";

  // sscanf returns the number of successful matches
  if (sscanf(date_ptr, date_format, &year, &month, &day)
      && sscanf(time_ptr, time_format, &hour, &minute, &second)) {
    // adjust year to 1900 epoch
    date.year = year - 1900;
    // month enum begins at 0
    date.month = month - 1;
    date.month_day = day;
    date.hour = hour;
    date.min = minute;
    date.sec = second;
    if (0 == sl_sleeptimer_set_datetime(&date)) {
      printf("Datetime set to %s %s\r\n", date_ptr, time_ptr);
    } else {
      printf("Failed to set date and time\r\n");
    }
  } else {
    printf("Invalid format for date time. See help.\r\n");
  }
}

/******************************************************************************
* Initialize example.
******************************************************************************/
void sleeptimer_app_init(void)
{
  printf("\r\nWelcome to the sleeptimer wallclock sample application\r\n");
  printf("Type 'help' to see available commands\r\n");
}

/******************************************************************************
* Ticking function.
******************************************************************************/
void sleeptimer_app_process_action(void)
{
}
