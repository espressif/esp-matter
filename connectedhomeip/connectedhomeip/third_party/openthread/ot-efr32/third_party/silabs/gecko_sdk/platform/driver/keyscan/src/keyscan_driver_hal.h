/***************************************************************************//**
 * @file
 * @brief KEYSCAN driver HAL definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef KEYSCAN_DRIVER_HAL_H
#define KEYSCAN_DRIVER_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <em_device.h>

/*******************************************************************************
 *******************************   DEFINES  ************************************
 ******************************************************************************/

// Maximum number of columns and rows given by hardware
#define KEYSCAN_MAX_NUMBER_COLUMN  KEYSCAN_COLNUM
#define KEYSCAN_MAX_NUMBER_ROW     KEYSCAN_ROWNUM

// Set of internal event flags corresponding to the interrupt flags
#define KEYSCAN_DRIVER_EVENT_WAKEUP  (0x01)
#define KEYSCAN_DRIVER_EVENT_SCANNED (0x02)
#define KEYSCAN_DRIVER_EVENT_KEY     (0x04)
#define KEYSCAN_DRIVER_EVENT_NOKEY   (0x08)

/*******************************************************************************
 *****************************   PROTOTYPES  ***********************************
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initializes the Keyscan peripheral.
 ******************************************************************************/
void sli_keyscan_driver_hal_init(void);

/***************************************************************************//**
 * Initializes the Keyscan peripheral GPIO.
 ******************************************************************************/
void sli_keyscan_driver_hal_init_gpio(void);

/***************************************************************************//**
 * Enables Keyscan interrupts by passing a set of flags.
 *
 * @param[in] local_flags  Set of interrupt flags.
 ******************************************************************************/
void sli_keyscan_driver_hal_enable_interrupts(uint8_t local_flags);

/***************************************************************************//**
 * Gets the column and row information from the peripheral when a key is pressed.
 *
 * @param[out] p_column  Pointer to the variable in which the column info will
 *                       be written.
 *
 * @param[out] p_row     Pointer to the variable in which the row info will
 *                       be written.
 ******************************************************************************/
void sli_keyscan_driver_hal_get_column_row(uint8_t *p_column,
                                           uint8_t *p_row);

/***************************************************************************//**
 * Indicates if keyscan scanning is currently running or not.
 *
 * @return true, if scan is running.
 *         false, otherwise.
 ******************************************************************************/
bool sli_keyscan_driver_hal_is_scan_running(void);

/***************************************************************************//**
 * Starts the keyscan scan.
 *
 * @param enable  Enable the keyscan module at the same time.
 ******************************************************************************/
void sli_keyscan_driver_hal_start_scan(bool enable);

/***************************************************************************//**
 * Stops the keyscan scan.
 ******************************************************************************/
void sli_keyscan_driver_hal_stop_scan(void);

/***************************************************************************//**
 * Processes the events from the keyscan interrupt.
 *
 * @param[in] local_flags  Set of event flags.
 ******************************************************************************/
void sli_keyscan_process_irq(uint8_t local_flags);

/***************************************************************************//**
 * Gets the info on auto-start feature enable status.
 *
 * @return  true, if auto-start feature is enabled.
 *          false, otherwise.
 ******************************************************************************/
__STATIC_INLINE bool sli_keyscan_driver_hal_is_autostart_enabled(void)
{
  return (KEYSCAN->CFG & _KEYSCAN_CFG_AUTOSTART_MASK) ? true : false;
}

/***************************************************************************//**
 * Gets the info on single-press feature enable status.
 *
 * @return  true, if single-press feature is enabled.
 *          false, if multi-press feature is enabled instead.
 ******************************************************************************/
__STATIC_INLINE bool sli_keyscan_driver_hal_is_singlepress_enabled(void)
{
  return (KEYSCAN->CFG & _KEYSCAN_CFG_SINGLEPRESS_MASK) ? true : false;
}

#ifdef __cplusplus
}
#endif

#endif // KEYSCAN_DRIVER_HAL_H
