/***************************************************************************//**
 * @file sl_wfx_host_sdio_io.c
 * @brief Network Device Driver - WFX Wi-Fi module
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_wfx_configuration_defaults.h"

#ifdef  SL_WFX_USE_SDIO
#include  <rtos_description.h>
#include "sl_wfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include <common/source/kal/kal_priv.h>

#include <io/include/sd.h>
#include <io/include/sd_card.h>

#include "sl_wfx_host_sdio_fnct.h"
#include "sl_wfx_host_pinout.h"

#include "sl_wfx_task.h"
#ifdef SLEEP_ENABLED
#include "sl_power_manager.h"
#endif

#ifdef LIB_MEM_CFG_HEAP_SIZE
#if (LIB_MEM_CFG_HEAP_SIZE < 20480)
#error "LIB_MEM_CFG_HEAP_SIZE needs to be >= 20480"
#endif
#else
#error "LIB_MEM_CFG_HEAP_SIZE config missing"
#endif

#ifndef SL_WIFI_CFG_SD_CONTROLLER_NAME
#define SL_WIFI_CFG_SD_CONTROLLER_NAME             "sd0"
#endif

static SD_BUS_HANDLE sd_bus_handle = 0;
static bool sdio_enabled = false;
#ifdef SLEEP_ENABLED
static bool useWIRQ = false;
uint8_t wirq_irq_nb = SL_WFX_HOST_PINOUT_GPIO_WIRQ_PIN;
#endif

/****************************************************************************************************//**
 *                                     sl_wfx_host_init_bus()
 *
 * @brief    Initializes the communications bus.
 *
 * @return   sl_status_t    Error code of the operation as defined in sl_status.h.
 *******************************************************************************************************/
sl_status_t sl_wfx_host_init_bus(void)
{
  RTOS_ERR err;

  GPIO_PinOutSet(SL_WFX_HOST_PINOUT_RESET_PORT, SL_WFX_HOST_PINOUT_RESET_PIN);

  sdio_enabled = true;
  // Check to see if the bus was already added
  if (SD_BusHandleGetFromName(SL_WIFI_CFG_SD_CONTROLLER_NAME) == SD_BusHandleNull) {
    (void)SD_BusAdd(SL_WIFI_CFG_SD_CONTROLLER_NAME, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return SL_STATUS_FAIL;
    }
  }

  sdio_fnct_init(&err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return SL_STATUS_FAIL;
  }

  if (sd_bus_handle == SD_BusHandleNull) {
    sd_bus_handle = SD_BusHandleGetFromName(SL_WIFI_CFG_SD_CONTROLLER_NAME);

    SD_BusStart(sd_bus_handle, &err);

    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return SL_STATUS_FAIL;
    }
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 *                                     sl_wfx_host_deinit_bus()
 *
 * @brief    De-initializes the communications bus.
 *
 * @return   sl_status_t    Error code of the operation as defined in sl_status.h.
 *******************************************************************************************************/
sl_status_t sl_wfx_host_deinit_bus(void)
{
  RTOS_ERR err;

  if (sd_bus_handle != SD_BusHandleNull) {
    SD_BusStop(sd_bus_handle, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return SL_STATUS_FAIL;
    }
    sd_bus_handle = SD_BusHandleNull;
  }

  return SL_STATUS_OK;
}

static sl_status_t sdio_io_write_direct(uint8_t function, uint32_t address, uint8_t* data)
{
  RTOS_ERR    err;
  sl_status_t result;

  (void)function;

  sdio_fnct_wrbyte(address,
                   *data,
                   &err);

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    result = SL_STATUS_OK;
  } else {
    result = SL_STATUS_FAIL;
  }

  return result;
}

static sl_status_t sdio_io_read_direct(uint8_t function, uint32_t address, uint8_t* data)
{
  RTOS_ERR    err;
  sl_status_t result;

  (void)function;

  *data = sdio_fnct_rdbyte(address, &err);

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    result = SL_STATUS_OK;
  } else {
    result = SL_STATUS_FAIL;
  }

  return result;
}

static sl_status_t sdio_io_write_extended(uint8_t function, uint32_t address, uint8_t* data, uint32_t data_length)
{
  RTOS_ERR    err;
  sl_status_t result;
  uint32_t    block_count;

  (void)function;

  if (data_length >= 512) {
    block_count = (data_length / SL_WFX_SDIO_BLOCK_SIZE) + ( ( (data_length % SL_WFX_SDIO_BLOCK_SIZE) == 0) ? 0 : 1);
    sdio_fnct_wrblk(address,
                    data,
                    block_count,
                    1,
                    &err);
  } else {
    sdio_fnct_wr(address,
                 data,
                 data_length,
                 1,
                 &err);
  }

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    result = SL_STATUS_OK;
  } else {
    result = SL_STATUS_FAIL;
  }

  return result;
}

static sl_status_t sdio_io_read_extended(uint8_t function, uint32_t address, uint8_t* data, uint32_t data_length)
{
  RTOS_ERR    err;
  sl_status_t result;
  uint32_t    block_count;

  (void)function;

  if (data_length >= 512) {
    block_count = (data_length / SL_WFX_SDIO_BLOCK_SIZE) + ( ( (data_length % SL_WFX_SDIO_BLOCK_SIZE) == 0) ? 0 : 1);

    sdio_fnct_rdblk(address,
                    data,
                    block_count,
                    1,
                    &err);
  } else {
    sdio_fnct_rd(address,
                 data,
                 data_length,
                 1,
                 &err);
  }

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    result = SL_STATUS_OK;
  } else {
    result = SL_STATUS_FAIL;
  }

  return result;
}

sl_status_t sl_wfx_host_sdio_transfer_cmd52(sl_wfx_host_bus_transfer_type_t type, uint8_t function, uint32_t address, uint8_t* buffer)
{
  sl_status_t status;

  if (type == SL_WFX_BUS_WRITE) {
    status = sdio_io_write_direct(function, address, buffer);
  } else {
    status = sdio_io_read_direct(function, address, buffer);
  }

  return status;
}

sl_status_t sl_wfx_host_sdio_transfer_cmd53(sl_wfx_host_bus_transfer_type_t type, uint8_t function, uint32_t address, uint8_t* buffer, uint16_t buffer_length)
{
  sl_status_t status;

  if (type == SL_WFX_BUS_WRITE) {
    status = sdio_io_write_extended(function, address, buffer, buffer_length);
  } else {
    status = sdio_io_read_extended(function, address, buffer, buffer_length);
  }

  return status;
}

sl_status_t sl_wfx_host_sdio_enable_high_speed_mode(void)
{
  sl_status_t result;
  uint8_t     value_u8;

  result = sdio_io_read_direct(0, 0x13, &value_u8);
  value_u8 |= 0x2;   // Set EHS to 1
  result = sdio_io_write_direct(0, 0x13, &value_u8);
  SDIO->HOSTCTRL1 |= SDIO_HOSTCTRL1_HSEN;   // Enable HS mode at the host
  return result;
}

static void sdio_irq_callback(void* arg)
{
  RTOS_ERR err;

  (void)arg;

  OSSemPost(&wfx_wakeup_sem, OS_OPT_POST_ALL, &err);
  OSFlagPost(&bus_events, SL_WFX_BUS_EVENT_FLAG_RX, OS_OPT_POST_FLAG_SET, &err);
}

/****************************************************************************************************//**
 *                                  sl_wfx_host_enable_platform_interrupt()
 *
 * @brief    Enable interrupts on the host side.
 *
 * @return   sl_status_t    Error code of the operation as defined in sl_status.h.
 *******************************************************************************************************/
sl_status_t sl_wfx_host_enable_platform_interrupt(void)
{
#ifdef SLEEP_ENABLED
  if (useWIRQ) {
//      GPIO_ExtIntConfig(SL_WFX_HOST_PINOUT_GPIO_WIRQ_PORT,
//                        SL_WFX_HOST_PINOUT_GPIO_WIRQ_PIN,
//                        wirq_irq_nb,
//                        true,
//                        false,
//                        true);
    return SL_STATUS_OK;
  } else
#endif
  {
    RTOS_ERR err;
    sdio_fnct_int_reg((void*)sdio_irq_callback);

    sdio_fnct_int_en(1, &err);
    SDIO->IEN |= SDIO_IFCR_CARDINT;
    SDIO->IFENC |= SDIO_IFENC_CARDINTEN;

    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_FAIL;
}

/****************************************************************************************************//**
 *                                 sl_wfx_host_disable_platform_interrupt()
 *
 * @brief    Disable interrupts on the host side.
 *
 * @return   sl_status_t    Error code of the operation as defined in sl_status.h.
 *******************************************************************************************************/
sl_status_t sl_wfx_host_disable_platform_interrupt(void)
{
#ifdef SLEEP_ENABLED
  if (useWIRQ) {
    GPIO_IntDisable(1 << wirq_irq_nb);
    return SL_STATUS_OK;
  } else
#endif
  {
    RTOS_ERR err;
    SDIO->IEN &= ~(SDIO_IFCR_CARDINT);
    SDIO->IFENC &= ~(SDIO_IFENC_CARDINTEN);
    sdio_fnct_int_en(0, &err);

    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_FAIL;
}

#ifdef SLEEP_ENABLED
sl_status_t sl_wfx_host_switch_to_wirq(void)
{
  uint32_t value32;

  GPIO_ExtIntConfig(SL_WFX_HOST_PINOUT_GPIO_WIRQ_PORT,
                    SL_WFX_HOST_PINOUT_GPIO_WIRQ_PIN,
                    wirq_irq_nb,
                    true,
                    false,
                    true);

  sl_wfx_reg_read_32(SL_WFX_CONFIG_REG_ID, &value32);
  value32 |= (1 << 15);
  sl_wfx_reg_write_32(SL_WFX_CONFIG_REG_ID, value32);
  useWIRQ = true;
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_enable_sdio(void)
{
  if (sdio_enabled == false) {
    // Prevent the host to use lower EM than EM1
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
    // Re-enable SDIO clock
    CMU_ClockEnable(cmuClock_SDIO, true);
    while ((CMU->STATUS & CMU_STATUS_SDIOCLKENS) == 0UL) ;
    SDIO->CLOCKCTRL |= SDIO_CLOCKCTRL_INTCLKEN;
    while ((SDIO->CLOCKCTRL & SDIO_CLOCKCTRL_INTCLKSTABLE) == 0) ;
    SDIO->CLOCKCTRL |= SDIO_CLOCKCTRL_SDCLKEN;
    sdio_enabled = true;
  }
  return SL_STATUS_OK;
}

sl_status_t sl_wfx_host_disable_sdio(void)
{
  if (sdio_enabled == true) {
    // Disable internal SDIO peripheral clock
    SDIO->CLOCKCTRL &= ~(_SDIO_CLOCKCTRL_SDCLKEN_MASK | _SDIO_CLOCKCTRL_INTCLKEN_MASK);
    // Disable MCU clock tree SDIO clock
    CMU_ClockEnable(cmuClock_SDIO, false);
    while ((CMU->STATUS & CMU_STATUS_SDIOCLKENS) != 0) ;
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
    sdio_enabled = false;

    // Allow the host to use the lowest allowed EM
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
  }
  return SL_STATUS_OK;
}

#endif

#endif
