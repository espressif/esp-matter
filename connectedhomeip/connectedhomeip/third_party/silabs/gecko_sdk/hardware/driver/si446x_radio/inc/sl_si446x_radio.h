/***************************************************************************//**
 * @file
 * @brief Si446x Radio driver
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

#ifndef  SL_SI446X_RADIO_H
#define  SL_SI446X_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "sl_enum.h"
#include "spidrv.h"

/***************************************************************************//**
 * @addtogroup si446x_radio Si446x Radio SPI Driver
 * @brief Provide functions to control Si4446x Radio via SPI interface.
 * @{
 ******************************************************************************/

//== DATA TYPES ==

/** @brief Event Handler callback type
 */
typedef void (*sl_ext_device_handler_t)(void);

/** @enum sl_ext_device_config_t
 * @brief Device GPIO Capabilities
 */
SL_ENUM(sl_ext_device_config_t) {
  SL_EXT_DEVICE_HAS_NOTHING       = 0x00u, ///< Not any of the others
  SL_EXT_DEVICE_HAS_POWER         = 0x01u, ///< Device has power input pin
  SL_EXT_DEVICE_HAS_READY         = 0x02u, ///< Device has ready output pin
  SL_EXT_DEVICE_HAS_SELECT        = 0x04u, ///< Device has select input pin
  SL_EXT_DEVICE_HAS_INTERRUPT     = 0x08u, ///< Device has interrupt output pin
  SL_EXT_DEVICE_HAS_READY_IRQ     = 0x20u, ///< An IRQ is assigned to ready pin
  SL_EXT_DEVICE_HAS_INTERRUPT_IRQ = 0x80u, ///< An IRQ is assigned to interrupt pin
};
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define SL_EXT_DEVICE_HAS_NOTHING       ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_NOTHING)
#define SL_EXT_DEVICE_HAS_POWER         ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_POWER)
#define SL_EXT_DEVICE_HAS_READY         ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_READY)
#define SL_EXT_DEVICE_HAS_SELECT        ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_SELECT)
#define SL_EXT_DEVICE_HAS_INTERRUPT     ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_INTERRUPT)
#define SL_EXT_DEVICE_HAS_READY_IRQ     ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_READY_IRQ)
#define SL_EXT_DEVICE_HAS_INTERRUPT_IRQ ((sl_ext_device_config_t) SL_EXT_DEVICE_HAS_INTERRUPT_IRQ)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @enum sl_ext_device_interrupt_depth_t
 * @brief Device interrupt depth state.
 */
SL_ENUM(sl_ext_device_interrupt_depth_t) {
  SL_EXT_DEVICE_INTERRUPT_DEPTH_ON     = 0x00u, ///< Interrupt is enabled
  SL_EXT_DEVICE_INTERRUPT_DEPTH_OFF    = 0x01u, ///< Interrupt lowest depth disabled
  //                                   : Higher values are disabled nesting depth
  SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED = 0xFFu, ///< Interrupt is not configured
};
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define SL_EXT_DEVICE_INTERRUPT_DEPTH_ON     ((sl_ext_device_interrupt_depth_t) SL_EXT_DEVICE_INTERRUPT_DEPTH_ON)
#define SL_EXT_DEVICE_INTERRUPT_DEPTH_OFF    ((sl_ext_device_interrupt_depth_t) SL_EXT_DEVICE_INTERRUPT_DEPTH_OFF)
#define SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED ((sl_ext_device_interrupt_depth_t) SL_EXT_DEVICE_INTERRUPT_UNCONFIGURED)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** Specifies GPIO pin polarity is inverted (assert = low)
 */
#define SL_EXT_DEVICE_POLARITY_INVERT false
/** Specifies GPIO pin polarity is normal (assert = high)
 */
#define SL_EXT_DEVICE_POLARITY_NORMAL true

//== MAP ABSTRACT TO SPECIFIC API ==

#define sl_ext_device_init                  sl_si446x_init
#define sl_ext_device_power_down            sl_si446x_power_down
#define sl_ext_device_power_up              sl_si446x_power_up
#define sl_ext_device_is_ready              sl_si446x_is_ready
#define sl_ext_device_not_ready             sl_si446x_not_ready
#define sl_ext_device_wait_ready            sl_si446x_wait_ready
#define sl_ext_device_select                sl_si446x_select
#define sl_ext_device_deselect              sl_si446x_deselect
#define sl_ext_device_is_selected           sl_si446x_is_selected
#define sl_ext_device_not_selected          sl_si446x_not_selected
#define sl_ext_device_is_interrupt_pending  sl_si446x_is_interrupt_pending
#define sl_ext_device_no_interrupt_pending  sl_si446x_no_interrupt_pending
#define sl_ext_device_disable_interrupt     sl_si446x_disable_interrupt
#define sl_ext_device_enable_interrupt      sl_si446x_enable_interrupt
#define sl_ext_device_spi_config            sl_si446x_spi_config

//== GLOBALS ==

/** @brief Device SPI driver configuration
 */
extern const SPIDRV_Init_t sl_ext_device_spi_config;

/** @brief Device SPI driver instance handle (pointer)
 */
extern SPIDRV_Handle_t sl_ext_device_spi_handle;

//== API FUNCTIONS ==

/** @brief Initialize Device GPIOs
 * @param  interrupt_handler The handler for device general interrupt
 *                     (NULL to disable)
 * @param  ready_handler The handler for device ready interrupt
 *                     (NULL to disable)
 * @return bitmask of configured features for this device
 * @note First initialization after bootup leaves the device powered down
 *       and unselected.  Subsequent inits don't touch the device powered
 *       or selected states and can be used to reconfigure callback(s),
 *       which always clears any stale/pending events.  For ready_handler
 *       its interrupt is enabled upon configuration; for interrupt_handler
 *       sl_ext_device_enable_interrupt() must subsequently be called to enable
 *       it.
 */
extern sl_ext_device_config_t sl_ext_device_init(sl_ext_device_handler_t interrupt_handler,
                                                 sl_ext_device_handler_t ready_handler);

/** @brief Power down the device per GPIO
 */
extern void sl_ext_device_power_down(void);

/** @brief Power up the device per GPIO
 */
extern void sl_ext_device_power_up(void);

/** @brief Indicate if the device is ready per GPIO
 * @return true if device is ready, false otherwise
 */
extern bool sl_ext_device_is_ready(void);

/** @brief Indicate if the device is not ready per GPIO
 * @return true if device is not ready, false otherwise
 */
inline bool sl_ext_device_not_ready(void)
{
  return (!sl_ext_device_is_ready());
}

/** @brief Wait for the device to become ready per GPIO
 */
extern void sl_ext_device_wait_ready(void);

/** @brief Select the external device
 */
extern void sl_ext_device_select(void);

/** @brief Unselect the external device
 */
extern void sl_ext_device_deselect(void);

/** @brief Indicate if the device is selected
 */
extern bool sl_ext_device_is_selected(void);

/** @brief Indicate if the device is not selected
 */
inline bool sl_ext_device_not_selected(void)
{
  return (!sl_ext_device_is_selected());
}

/** @brief Indicate if device interrupt GPIO is pending
 * @return true if device interrupt is pending, false otherwise
 */
extern bool sl_ext_device_is_interrupt_pending(void);

/** @brief Indicate if device interrupt is not pending
 * @return true if device interrupt is not pending, false otherwise
 */
inline bool sl_ext_device_no_interrupt_pending(void)
{
  return (!sl_ext_device_is_interrupt_pending());
}

/** @brief Disable device interrupt and increment interrupt nesting level.
 * @return interrupt level prior to disabling (0=interrupts were enabled)
 */
extern sl_ext_device_interrupt_depth_t sl_ext_device_disable_interrupt(void);

/** @brief Decrement interrupt nesting level and, if 0, enable device
 * interrupt.
 * @param  clearPending if true clears any "stale" pending interrupt just
 *         prior to enabling device interrupt.
 * @return interrupt level prior to enabling (0=interrupts were enabled)
 */
extern sl_ext_device_interrupt_depth_t sl_ext_device_enable_interrupt(bool clearPending);

#ifdef __cplusplus
}
#endif
#endif // SL_SI446X_RADIO_H
