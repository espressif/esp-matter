/*******************************************************************************
* @file  rsi_device_init_apis.c
* @brief
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

/*
  Includes
 */
#include "rsi_driver.h"
#ifdef SIM_9118
#include "rsi_board.h"
#endif
#ifdef RSI_M4_INTERFACE
#ifdef ROM_WIRELESS
#include "rsi_chip.h"
#endif
#include "core_cm4.h"
#include "rsi_board.h"
#endif
#ifdef LINUX_PLATFORM
#include <stdint.h>
#include <sys/ioctl.h>
#endif
#include "rsi_hal.h"
#include "rsi_wlan_non_rom.h"

extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
/** @addtogroup COMMON
* @{
*/
/*==============================================*/
/**
 * @brief       Power cycle the module and set the firmware image type to be loaded for WiSeConnect features. Initialize the module SPI.  This is a blocking API.
 * @pre         \ref rsi_driver_init() must be called before this API
 * @param[in]   select_option - \ref LOAD_NWP_FW            	    : To load Firmware image \n
 *                              \ref LOAD_DEFAULT_NWP_FW_ACTIVE_LOW : To load active low Firmware image. \n
 *                              Active low firmware will generate active low interrupts to indicate that packets are pending on the \n
 *                              module, instead of the default active high. \n
 * @return      **Success**  - RSI_SUCCESS \n
 *              **Failure**  - Non-Zero Value
 */

int32_t rsi_device_init(uint8_t select_option)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_DEVICE_INIT_ENTRY, COMMON, LOG_INFO);
#if (defined(RSI_SPI_INTERFACE) || defined(RSI_SDIO_INTERFACE))
  rsi_timer_instance_t timer_instance;
#endif
#if defined(RSI_M4_INTERFACE) || defined(LINUX_PLATFORM)
  uint8_t skip_bootload_sequence = 0;
  int32_t retval                 = 0;
#endif
  if ((rsi_driver_cb_non_rom->device_state != RSI_DRIVER_INIT_DONE)) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

#if defined(RSI_SPI_INTERFACE) || defined(RSI_M4_INTERFACE) || defined(RSI_UART_INTERFACE) \
  || defined(RSI_SDIO_INTERFACE)
  // Board Initialization
  rsi_hal_board_init();
#endif

#ifdef RSI_M4_INTERFACE

  SysTick_Config(SystemCoreClock / 1000);
#ifdef RSI_WITH_OS
  // Set P2P Intr priority
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_INTR_PRI);
#endif
  if (!(P2P_STATUS_REG & TA_is_active)) {
#ifdef DEBUG_UART
    DEBUGOUT("\n wakeup TA \n");
#endif
    P2P_STATUS_REG |= M4_wakeup_TA;
    skip_bootload_sequence = 1;
  }
#ifdef DEBUG_UART
  DEBUGOUT("\n Waiting for TA wakeup \n");
#endif
  while (!(P2P_STATUS_REG & TA_is_active))
    ;
#ifdef DEBUG_UART
  DEBUGOUT("\n TA wokeup \n");
#endif

  if (!skip_bootload_sequence) {
    do {
      retval = rsi_waitfor_boardready();
      if (retval == RSI_ERROR_IN_OS_OPERATION) {
#ifdef DEBUG_UART
        DEBUGOUT("\n Waiting for TA wakeup \n");
#endif
      }
      if ((retval < 0) && (retval != RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval != RSI_ERROR_IN_OS_OPERATION)) {
        SL_PRINTF(SL_DEVICE_INIT_EXIT_1, COMMON, LOG_ERROR, "retval:%d", retval);
        return retval;
      }
    } while ((retval == RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval == RSI_ERROR_IN_OS_OPERATION));
    retval = rsi_select_option(select_option);
    if (retval < 0) {
      SL_PRINTF(SL_DEVICE_INIT_EXIT_2, COMMON, LOG_ERROR, "retval:%d", retval);
      return retval;
    }
  }
  // Update TX & RX DMA descriptor address
  rsi_update_tx_dma_desc(skip_bootload_sequence);
  rsi_update_rx_dma_desc();
#endif

#ifdef LINUX_PLATFORM
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SDIO_INTERFACE))
  if (!skip_bootload_sequence) {
    do {
      retval = rsi_waitfor_boardready();
      if ((retval < 0) && (retval != RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval != RSI_ERROR_IN_OS_OPERATION)) {
        SL_PRINTF(SL_DEVICE_INIT_EXIT_3, COMMON, LOG_ERROR, "retval:%d", retval);
        return retval;
      }
    } while (retval == RSI_ERROR_WAITING_FOR_BOARD_READY);
    retval = rsi_select_option(select_option);
    if (retval < 0) {
      SL_PRINTF(SL_DEVICE_INIT_EXIT_4, COMMON, LOG_ERROR, "retval:%d", retval);
      return retval;
    }
  }
#endif
#else
#ifndef RSI_M4_INTERFACE
  // power cycle the module
  status = rsi_bl_module_power_cycle();
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_DEVICE_INIT_MODULE_POWER_CYCLE_FAILURE, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#endif
#if defined(RSI_SDIO_INTERFACE)
  // SDIO interface initialization
  status = rsi_sdio_iface_init();
#elif defined(RSI_SPI_INTERFACE)
  // SPI interface initialization
  status = rsi_spi_iface_init();
#endif
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_DEVICE_INIT_SPI_INIT_FAILURE, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

#if (defined(RSI_SPI_INTERFACE) || defined(RSI_SDIO_INTERFACE))
  rsi_init_timer(&timer_instance, RSI_BOARD_READY_WAIT_TIME); //40000
  do {
    status = rsi_bl_waitfor_boardready();

    if ((status < 0) && (status != RSI_ERROR_WAITING_FOR_BOARD_READY)) {
      SL_PRINTF(SL_DEVICE_INIT_BL_WAIT_FOR_BOARD_READY_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
      return status;
    }

    if (rsi_timer_expired(&timer_instance)) {
      SL_PRINTF(SL_DEVICE_INIT_BOARD_READY_TIMEOUT, COMMON, LOG_ERROR);
      return RSI_ERROR_BOARD_READY_TIMEOUT;
    }

  } while (status == RSI_ERROR_WAITING_FOR_BOARD_READY);
#ifdef RSI_SPI_INTERFACE
  if (select_option == LOAD_NWP_FW) {
    rsi_set_intr_type(RSI_ACTIVE_HIGH_INTR);
  } else if (select_option == LOAD_DEFAULT_NWP_FW_ACTIVE_LOW) {

    status = rsi_set_intr_type(RSI_ACTIVE_LOW_INTR);
    if (status < 0) {
      SL_PRINTF(SL_DEVICE_INIT_SET_INTR_TYPE_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
      return status;
    }
  }
#endif
#if RSI_FAST_FW_UP
  status = rsi_set_fast_fw_up();
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_DEVICE_INIT_SET_FAST_FIRMWARE_UP_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#endif
  status = rsi_bl_select_option(select_option);
  if (status < 0) {
    SL_PRINTF(SL_DEVICE_INIT_BL_SELECT_OPTION_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#endif
#endif

#ifdef RSI_M4_INTERFACE
  rsi_m4_ta_interrupt_init();
  if (!(M4SS_P2P_INTR_SET_REG & RX_BUFFER_VALID)) {
    rsi_submit_rx_pkt();
  }
#else
#if ((defined RSI_SPI_INTERFACE) && defined(RSI_SPI_HIGH_SPEED_ENABLE))
  // Enable high speed SPI
  status = rsi_spi_high_speed_enable();
  if (status < 0) {
    SL_PRINTF(SL_DEVICE_INIT_HIGH_SPEED_ENABLE_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
  // Switch host clock to high frequency
  rsi_switch_to_high_clk_freq();
#endif
#ifdef RSI_SDIO_INTERFACE
  // Switch host clock to high frequency
  rsi_switch_to_high_clk_freq();
#endif
  // Configure interrupt
  rsi_hal_intr_config(rsi_interrupt_handler);

  // Unmask interrupts
  rsi_hal_intr_unmask();
#endif
  // Updating state
  rsi_driver_cb_non_rom->device_state = RSI_DEVICE_INIT_DONE;
  SL_PRINTF(SL_DEVICE_INIT_EXIT_5, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      De-Initialize the module and reset the module. Reset is driven to the module by asserting the RESET_PS pin for some duration and releasing it.
 *             Need to control the RESET_PS pin to reset the module.  This is a non-blocking API.
 * @param[in]  Void
 * @return     0              - Success  \n
 *             Non-Zero Value - Failure
 */

int32_t rsi_device_deinit(void)
{
  SL_PRINTF(SL_DEVICE_DEINIT_ENTRY, COMMON, LOG_INFO);
#ifdef RSI_WLAN_ENABLE
  uint8_t i;
#endif
  if ((rsi_driver_cb_non_rom->device_state < RSI_DEVICE_INIT_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_DEVICE_DEINIT_COMMAND_GIVEN_IN_WRONG_STATE,
              COMMON,
              LOG_ERROR,
              "state: %4x",
              rsi_driver_cb_non_rom->device_state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (rsi_driver_cb != NULL) {
#ifndef RSI_M4_INTERFACE
    // Mask the interrupt
    rsi_hal_intr_mask();
#else
    mask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT | TX_PKT_TRANSFER_DONE_INTERRUPT);
#endif
  }
#ifdef RSI_SPI_INTERFACE
  // Power cycle the module
  rsi_bl_module_power_cycle();
#endif
  if (rsi_driver_cb != NULL) {
    rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;
#ifdef RSI_WLAN_ENABLE
    rsi_driver_cb->wlan_cb->state = RSI_WLAN_STATE_NONE;
#endif
  }
#ifdef RSI_WLAN_ENABLE

  if (rsi_socket_pool != NULL) {
    // Added this loop for socket pool not memset/clear while deinit/reset
    for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
      // Memset socket info
      memset(&rsi_socket_pool[i], 0, sizeof(rsi_socket_info_t));
    }
  }
  if (rsi_socket_pool_non_rom != NULL) {
    // Added this loop for socket pool not memset/clear while deinit/reset
    for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
      // Memset socket info
      memset(&rsi_socket_pool_non_rom[i], 0, sizeof(rsi_socket_info_non_rom_t));
    }
  }

#endif
  rsi_driver_cb_non_rom->device_state = RSI_DRIVER_INIT_DONE;
  SL_PRINTF(SL_DEVICE_INIT_EXIT, COMMON, LOG_INFO);
  return RSI_SUCCESS;
}
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
/*==================================================*/
/**
 * @brief       get the interrupt status.
 * @param[in]   
 * @return      1 - when there is an interrupt \n
 *              0 - when there is no interrupt
*/

uint8_t rsi_get_intr_status()
{
#if ((defined RSI_SPI_INTERFACE) && (!(defined RSI_ACTIVE_LOW)))
  return rsi_hal_intr_pin_status();
#else
  return !rsi_hal_intr_pin_status();
#endif
}
#endif
/** @} */
