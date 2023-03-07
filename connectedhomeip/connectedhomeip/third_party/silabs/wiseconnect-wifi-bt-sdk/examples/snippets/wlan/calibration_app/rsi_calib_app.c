/*******************************************************************************
* @file  rsi_calib_app.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * Include files
 * */

//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"
#include "rsi_driver.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//! Transmit test power
#define RSI_TX_TEST_POWER 18

//! Transmit test rate
#define RSI_TX_TEST_RATE RSI_RATE_1

//! Transmit test length
#define RSI_TX_TEST_LENGTH 30

//! Transmit test mode
#define RSI_TX_TEST_MODE RSI_BURST_MODE

//! Transmit test channel
#define RSI_TX_TEST_CHANNEL 1

//! Select Intenal antenna or uFL connector
#define RSI_ANTENNA 1

//! Antenna gain in 2.4GHz band
#define RSI_ANTENNA_GAIN_2G 0

//! Antenna gain in 5GHz band
#define RSI_ANTENNA_GAIN_5G 0

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
#define CALIB_MAX_COMMAND_LENGTH 200
#define CALIB_MIN_COMMAND_LENGTH (strlen("rsi_freq_offset=") + 2 /*atleast one char of input data and CR */)
//! buffer to gather host command received on UART
uint8_t uart_recv_buf[CALIB_MAX_COMMAND_LENGTH];
//! UART receive buffer index
uint32_t uart_recv_buf_indx = 0;
//! buffer to copy the host command, so that it doesn't change during processing
uint8_t calib_host_cmd_buf[CALIB_MAX_COMMAND_LENGTH];
//! Event map for calib application
uint32_t rsi_calib_app_event_map;
#define RSI_CALIB_HOST_CMD_EVENT 0

#ifdef RSI_WITH_OS
#define RSI_FOREVER 1
rsi_semaphore_handle_t event_sem;
#else
#define RSI_FOREVER 0
#endif

#define NO_OF_CALIB_HOST_COMMANDS 2
typedef struct rsi_calib_host_commands_s {
  uint8_t cmd[CALIB_MAX_COMMAND_LENGTH];
} rsi_calib_host_commands_t;
enum rsi_CALIB_host_cmd_types { FREQ_OFFSET, CALIB_WRITE };
rsi_calib_host_commands_t rsi_calib_host_commands[NO_OF_CALIB_HOST_COMMANDS] = { { "rsi_freq_offset=" },
                                                                                 { "rsi_calib_write=" } };

// function prototypes
void rsi_calib_host_cmd_event_handler(void);

/*==============================================*/
/**
 * @fn         rsi_calib_display usage
 * @brief      this function displays usage of calib commands
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function displays usage of calib commands
 */
void rsi_calib_display_usage(void)
{
  LOG_PRINT("Usage\r\n");
  LOG_PRINT("rsi_freq_offset=<freq_offset>\r\n");
  LOG_PRINT("rsi_calib_write=<target>,<flags>,<gain_offset>\r\n");
  LOG_PRINT("OR\r\n");
  LOG_PRINT("rsi_calib_write=<target>,<flags>,<gain_offset>,<xo_ctune>\r\n");
  LOG_PRINT("\r\n");
}

/*==============================================*/
/**
 * @fn         rsi_calib_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BT initialization.
 */
static void rsi_calib_app_init_events()
{
  rsi_calib_app_event_map = 0;
}

/*==============================================*/
/**
 * @fn         rsi_calib_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_calib_app_set_event(uint32_t event_num)
{
  if (event_num <= 31) {
    rsi_calib_app_event_map |= BIT(event_num);
  }
}

/*==============================================*/
/**
 * @fn         rsi_calib_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_calib_app_clear_event(uint32_t event_num)
{
  if (event_num <= 31) {
    rsi_calib_app_event_map &= ~BIT(event_num);
  }
}

/*==============================================*/
/**
 * @fn         rsi_calib_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_calib_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (ix <= 31) {
      if (rsi_calib_app_event_map & (1 << ix)) {
        return ix;
      }
    }
  }

  return (RSI_FAILURE);
}

/*==============================================*/
/**
 * @fn         rsi_calib_host_cmd_recv_isr
 * @brief      sets the calib host command event to process host cmd
 * @param[in]  *cmd command string
 * @return     none.
 * @section description
 * This function sets the event for host cmd processing
 */
void rsi_calib_host_cmd_recv_isr(const char *cmd)
{
  memset(calib_host_cmd_buf, 0, sizeof(calib_host_cmd_buf));
  memcpy(calib_host_cmd_buf, cmd, strlen(cmd));
  rsi_calib_app_set_event(RSI_CALIB_HOST_CMD_EVENT);
#ifdef RSI_WITH_OS
  rsi_semaphore_post_from_isr(&event_sem);
#endif
}

/*==============================================*/
/**
 * @fn         rsi_calib_uart_recv_isr
 * @brief      ISR for UART receive interrupt
 * @param[in]  cmd_char char received on UART line
 * @return     none.
 * @section description
 * This function populates the buffer with the characters received on UART
 */
void rsi_calib_uart_recv_isr(uint8_t cmd_char)
{
  //! 'CR' indicates end of a command sequence
  if ((cmd_char == '\n') && (uart_recv_buf[uart_recv_buf_indx - 1] == '\r')) {
    if (uart_recv_buf_indx < CALIB_MIN_COMMAND_LENGTH) {
      LOG_PRINT("Command too short\r\n");
      uart_recv_buf_indx = 0;
      return;
    }
    uart_recv_buf[uart_recv_buf_indx - 1] = 0;
    uart_recv_buf_indx                    = 0;
    //! Indicate host command received
    rsi_calib_host_cmd_recv_isr((char *)&uart_recv_buf[0]);
    //! ignore
  } else {
    uart_recv_buf[uart_recv_buf_indx] = cmd_char;
    uart_recv_buf_indx++;
    if (uart_recv_buf_indx >= CALIB_MAX_COMMAND_LENGTH) {
      LOG_PRINT("Command too long\r\n");
      uart_recv_buf_indx = 0;
    }
  }
}

/*==============================================*/
/**
 * @fn         rsi_calib_event_scheduler
 * @brief      event scheduler for calibration app
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function invokes the handlers for calib application events
 */
void rsi_calib_event_scheduler()
{
  do {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&event_sem, 0);
#endif
    switch (rsi_calib_app_get_event()) {
      case RSI_CALIB_HOST_CMD_EVENT:
        rsi_calib_host_cmd_event_handler();
        break;
      default:
        break;
    }
  } while (RSI_FOREVER);
}

/*==============================================*/
/**
 * @fn         rsi_calib_host_cmd_event_handler
 * @brief      this function processes host commands
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is the handler for calib host command event
 */
void rsi_calib_host_cmd_event_handler(void)
{
  uint8_t offset     = 0;
  uint8_t index      = 0;
  uint8_t cmd_len    = 0;
  uint8_t num_params = 0;
  uint8_t target;
  uint8_t flags;
  int8_t gain_offset;
  int8_t xo_ctune     = 80;
  int32_t status      = 0;
  int32_t freq_offset = 0;

  if (!rsi_strncasecmp((const char *)calib_host_cmd_buf, (const char *)"rsi_", strlen("rsi_"))) {
    for (index = 0; index < NO_OF_CALIB_HOST_COMMANDS; index++) {
      cmd_len = rsi_strlen((const unsigned char *)rsi_calib_host_commands[index].cmd);
      if (!strncasecmp((const char *)&calib_host_cmd_buf, (char *)rsi_calib_host_commands[index].cmd, cmd_len)) {
        offset = cmd_len;
        switch (index) {
          case FREQ_OFFSET:
            if (calib_host_cmd_buf[offset] != '\0') {
              offset += rsi_parse(&freq_offset, RSI_PARSE_4_BYTES, &calib_host_cmd_buf[offset]);
              status = rsi_send_freq_offset(freq_offset);
              if (status == RSI_SUCCESS) {
                LOG_PRINT("Ok\r\n");
              } else {
                LOG_PRINT("Error %lx\r\n", status);
              }
              //! exit loop
              goto exit;
            } else {
              goto error;
            }

          case CALIB_WRITE:
            if (calib_host_cmd_buf[offset] != '\0') {
              offset += rsi_parse(&target, RSI_PARSE_1_BYTES, &calib_host_cmd_buf[offset]);
              num_params++;
            }
            if (calib_host_cmd_buf[offset] != '\0') {
              offset += rsi_parse(&flags, RSI_PARSE_1_BYTES, &calib_host_cmd_buf[offset]);
              num_params++;
            }
            if (calib_host_cmd_buf[offset] != '\0') {
              offset += rsi_parse(&gain_offset, RSI_PARSE_1_BYTES, &calib_host_cmd_buf[offset]);
              num_params++;
            }
            if (calib_host_cmd_buf[offset] != '\0') {
              offset += rsi_parse(&xo_ctune, RSI_PARSE_4_BYTES, &calib_host_cmd_buf[offset]);
              num_params++;
            }
            if (num_params >= 3) {
              status = rsi_calib_write(target, flags, gain_offset, xo_ctune);
              if (status == RSI_SUCCESS) {
                LOG_PRINT("Ok\r\n");
              } else {
                LOG_PRINT("Error %lx\r\n", status);
              }
              //! exit loop
              goto exit;
            } else {
              goto error;
            }
        } //! switch ends
      }   //! if block ends
    }     //! for loop ends
  }
error:
  LOG_PRINT("Invalid command\r\n");
  rsi_calib_display_usage();
exit:
  rsi_calib_app_clear_event(RSI_CALIB_HOST_CMD_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_calib_app_task
 * @brief      tasklet for calib application processing
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function processes the calib application events
 */
int32_t rsi_calib_app_task()
{
  int32_t status = RSI_SUCCESS;
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
  //! module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("Device init failed %lx\r\n", status);
    return status;
  }
  LOG_PRINT("Device init success\r\n");

#ifdef RSI_WITH_OS
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)(uint32_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(8, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("Wireless init failed %lx\r\n", status);
    return status;
  }

  //! initialize calib application events
  rsi_calib_app_init_events();

  //! Wlan radio init
  status = rsi_wlan_radio_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("Radio init failed %lx\r\n", status);
    //! Return the status if error in sending command occurs
    return status;
  }

  //! To selct Internal antenna or uFL connector
  status = rsi_wireless_antenna(RSI_ANTENNA, RSI_ANTENNA_GAIN_2G, RSI_ANTENNA_GAIN_5G);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("Wireless Antenna config failed %lx\r\n", status);
    return status;
  }

  //! transmit test start
  status = rsi_transmit_test_start(RSI_TX_TEST_POWER,
                                   RSI_TX_TEST_RATE,
                                   RSI_TX_TEST_LENGTH,
                                   RSI_TX_TEST_MODE,
                                   RSI_TX_TEST_CHANNEL);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("Transmit test start failed %lx\r\n", status);
    return status;
  }
  LOG_PRINT("Transmission started\r\n");

  // Display command usage
  rsi_calib_display_usage();

  // Event scheduler for calib application
  rsi_calib_event_scheduler();

  return status;
}

void main_loop(void)
{
  while (1) {
    ////////////////////////
    //! Application code ///
    ////////////////////////
    rsi_calib_event_scheduler();
    //! event loop
    rsi_wireless_driver_task();
  }
}

int main()
{
  int32_t status;
#ifdef RSI_WITH_OS
  rsi_task_handle_t wlan_task_handle = NULL;
#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

#ifdef RSI_WITH_OS
  rsi_semaphore_create(&event_sem, 0);
  //! OS case
  //! Task created for calib host cmd task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_calib_app_task,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! run calibration application
  status = rsi_calib_app_task();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("calib app failed %lx\r\n", status);
    return status;
  }
  //! Application main loop
  main_loop();
#endif
  return status;
}
