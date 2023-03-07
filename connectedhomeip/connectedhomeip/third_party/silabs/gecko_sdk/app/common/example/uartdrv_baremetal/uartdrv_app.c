/***************************************************************************//**
 * @file
 * @brief uartdrv examples functions
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
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "APP_COMMON_EXAMPLE_UARTDRV"

#include <stdio.h>
#include <string.h>
#include "uartdrv_app.h"
#include "sl_uartdrv_instances.h"
#include "sl_power_manager.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define INPUT_BUFSIZE    80

#define OUTPUT_BUFSIZE (INPUT_BUFSIZE + 40)

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Byte received
uint8_t rx_byte;

// Track number of bytes transmitted
static uint8_t tx_counter = 0;

// Input buffer
static char buffer[INPUT_BUFSIZE];

// Output buffer
char reply[OUTPUT_BUFSIZE];

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

/*******************************************************************************
 **************************   STATIC FUNCTIONS   *******************************
 ******************************************************************************/

// Callback triggered when UARTDRV has completed transmission.
static void UART_tx_callback(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)handle;
  (void)data;
  (void)transferCount;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK) {
    tx_counter++;
  }
}

// Callback triggered when UARTDRV has received data
static void UART_rx_callback(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)transferCount;
  (void)handle;
  static uint8_t index = 0;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK) {
    // If end of line is entered, echo input data
    if (*data == '\r' || *data == '\n') {
      buffer[index] = '\0';

      sprintf(reply, "\r\n\r\nYou wrote:\r\n\r\n %s \r\n\r\n> ", buffer);

      // Echo the input string
      UARTDRV_Transmit(sl_uartdrv_leuart_vcom_handle,
                       (uint8_t *)reply, strlen(reply),
                       UART_tx_callback);
      index = 0;
    } else {
      if (index < INPUT_BUFSIZE - 1) {
        buffer[index] = *data;
        index++;
      }
      // Echo the input character
      UARTDRV_Transmit(sl_uartdrv_leuart_vcom_handle,
                       data, 1,
                       UART_tx_callback);
    }
  }

  /* Receive the next byte */
  UARTDRV_Receive(sl_uartdrv_leuart_vcom_handle,
                  &rx_byte, 1,
                  UART_rx_callback);
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

// Hook for power manager. The application will not prevent the
// power manager from entering sleep.
bool app_is_ok_to_sleep(void)
{
  return true;
}

// Hook for power manager. The application will not prevent the
// power manager from re-entering sleep after an interrupt is serviced.
sl_power_manager_on_isr_exit_t app_sleep_on_isr_exit(void)
{
  return SL_POWER_MANAGER_SLEEP;
}

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void uartdrv_app_init(void)
{
  // Require at least EM2 from Power Manager
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);

  /* Output on vcom leuart instance */
  char *str1 = "\r\n\r\nUARTDRV LEUART example\r\n\r\n> ";

  // Non-blocking transmit
  UARTDRV_Transmit(sl_uartdrv_leuart_vcom_handle,
                   (uint8_t *)str1, strlen(str1),
                   UART_tx_callback);
  // Non-blocking receive
  UARTDRV_Receive(sl_uartdrv_leuart_vcom_handle,
                  &rx_byte, 1,
                  UART_rx_callback);
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void uartdrv_app_process_action(void)
{
}
