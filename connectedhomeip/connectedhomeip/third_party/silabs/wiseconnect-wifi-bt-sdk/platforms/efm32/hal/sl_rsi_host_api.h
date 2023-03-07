/*******************************************************************************
* @file sl_rsi_host_api.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com
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
#include <stdio.h>
#include <stdlib.h>

#include "sdiodrv.h"
#include "sl_status.h"

/**
 * @brief SDIO block size
 * @warning This should be configured from the Bus SDIO plugin options.
 */
#ifndef SL_RSI_SDIO_BLOCK_SIZE
#define SL_RSI_SDIO_BLOCK_SIZE            256
#endif

/* SDIO CCCR register offsets */
#define SL_RSI_SDIO_CCCR_IO_QUEUE_ENABLE        (0x02)
#define SL_RSI_SDIO_CCCR_IOR_ENABLE             (0x03)
#define SL_RSI_SDIO_CCCR_IRQ_ENABLE             (0x04)
#define SL_RSI_SDIO_CCCR_BUS_INTERFACE_CONTROL  (0x07)
#define SL_RSI_SDIO_CCCR_HIGH_SPEED_ENABLE      (0x13)

/* SDIO FBR1 register offsets */
#define SL_RSI_SDIO_FBR1_BLOCK_SIZE_LSB (0x110)   /* Function 1 16-bit block size LSB */
#define SL_RSI_SDIO_FBR1_BLOCK_SIZE_MSB (0x111)   /* Function 1 16-bit block size MSB */

//typedef uint32_t sl_status_t;

//uint8_t sdio_init_done;

#define SL_RSI_ERROR_CHECK(__status__)               \
  do {                                               \
    if (((sl_status_t)__status__) != SL_STATUS_OK) { \
      goto error_handler;                            \
    }                                                \
  } while (0)

typedef enum {
  SL_RSI_BUS_WRITE = (1 << 0),
  SL_RSI_BUS_READ  = (1 << 1),
  SL_RSI_BUS_WRITE_AND_READ = SL_RSI_BUS_WRITE | SL_RSI_BUS_READ,
} sl_rsi_host_bus_transfer_type_t;

/* WF200 host bus API */
/**************************************************************************//**
 * @brief Initialize the host bus
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called once during the driver initialization phase
 *****************************************************************************/
sl_status_t sl_rsi_host_init_bus(void);

/**************************************************************************//**
 * @brief Deinitialize the host bus
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *
 * @note Called if an error occurs during the initialization phase
 *****************************************************************************/
sl_status_t sl_rsi_host_deinit_bus(void);

/* RSI host SDIO bus API */
/**************************************************************************//**
 * @brief Send command 52 on the SDIO bus
 *
 * @param type is the type of bus action (see ::sl_wfx_host_bus_transfer_type_t)
 * @param function is the function to use in the SDIO command
 * @param address is the address to use in the SDIO command
 * @param buffer is a pointer to the buffer data
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_rsi_host_sdio_transfer_cmd52(sl_rsi_host_bus_transfer_type_t type,
                                            uint8_t  function,
                                            uint32_t address,
                                            uint8_t *buffer);
/**************************************************************************//**
 * @brief Send command 53 on the SDIO bus
 *
 * @param type is the type of bus action (see ::slx_rsi_host_bus_transfer_type_t)
 * @param function is the function to use in the SDIO command
 * @param address is the address to use in the SDIO command
 * @param buffer is a pointer to the buffer data
 * @param buffer_length is the length of the buffer data
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_rsi_host_sdio_transfer_cmd53(sl_rsi_host_bus_transfer_type_t type,
                                            uint8_t  function,
                                            uint32_t address,
                                            uint16_t *buffer,
                                            uint16_t buffer_length);

/**************************************************************************//**
 * @brief Enable the SDIO high-speed mode
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_rsi_host_sdio_enable_high_speed_mode(void);

/**************************************************************************//**
 * @brief Enable the bus interrupt
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_rsi_host_enable_platform_interrupt(void);

/**************************************************************************//**
 * @brief Disable the bus interrupt
 * @returns Returns SL_STATUS_OK if successful, SL_STATUS_FAIL otherwise
 *****************************************************************************/
sl_status_t sl_rsi_host_disable_platform_interrupt(void);
