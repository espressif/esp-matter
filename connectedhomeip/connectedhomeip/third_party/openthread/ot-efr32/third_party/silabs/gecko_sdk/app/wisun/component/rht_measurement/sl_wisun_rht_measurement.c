/***************************************************************************//**
 * @file sl_wisun_rht_measurement.c
 * @brief Wi-SUN temperature and relative humidity measurement modul
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_wisun_rht_measurement.h"
#include "sl_si70xx.h"
#include "sl_i2cspm_instances.h"
#include "sl_board_control.h"
#include "sl_component_catalog.h"
#include "sl_udelay.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

#include <assert.h>
#include <stddef.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Sensor I2C Address
 *****************************************************************************/
#define SL_WISUN_RHT_SENSOR_ADDRESS        (SI7021_ADDR)

/**************************************************************************//**
 * @brief Delay in us to initialize the I2C sensor
 *****************************************************************************/
#define SL_WISUN_RHT_SENSOR_INIT_DELAY_US  (80000UL)

/**************************************************************************//**
 * @brief Internal sensor instance
 *****************************************************************************/
static sl_i2cspm_t *_rht_sensor = NULL;

#if defined(SL_CATALOG_KERNEL_PRESENT)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief RHT sensor mutex lock
 * @details
 *****************************************************************************/
static inline void _rht_mutex_acquire(void);

/**************************************************************************//**
 * @brief RHT sensor mutex unlock
 * @details
 *****************************************************************************/
static inline void _rht_mutex_release(void);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/**
 * @brief RHT sensor mutex
 */
static osMutexId_t _rht_sensor_mtx;

/**
 * @brief RHT sensor mutex control block
 */
__ALIGNED(8) static uint8_t _rht_sensor_mtx_cb[osMutexCbSize] = { 0 };

/**
 * @brief RHT sensor mutex attribute
 */
static const osMutexAttr_t _rht_sensor_mtx_attr = {
  .name      = "RhtSensorMtx",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _rht_sensor_mtx_cb,
  .cb_size   = sizeof(_rht_sensor_mtx_cb)
};

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Mutex acquire */
static inline void _rht_mutex_acquire(void)
{
  assert(osMutexAcquire(_rht_sensor_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _rht_mutex_release(void)
{
  assert(osMutexRelease(_rht_sensor_mtx) == osOK);
}
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Init Sensor */
void sl_wisun_rht_init(void)
{
  bool sensor_init_res = true;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  osKernelState_t kernel_state = osKernelLocked;

  // get kernel state
  kernel_state = osKernelGetState();

  _rht_sensor_mtx = osMutexNew(&_rht_sensor_mtx_attr);
  assert(_rht_sensor_mtx != NULL);

  // lock only if the kernel is running
  if (kernel_state == osKernelRunning) {
    _rht_mutex_acquire();
  }
#endif

  _rht_sensor = sl_i2cspm_sensor;
  assert(_rht_sensor != NULL);
  // init board
  assert(sl_board_enable_sensor(SL_BOARD_SENSOR_RHT) == SL_STATUS_OK);

  // init sensor
  if (!sl_si70xx_present(_rht_sensor, SL_WISUN_RHT_SENSOR_ADDRESS, NULL)) {
    // Wait for sensor to become ready
    sl_udelay_wait(SL_WISUN_RHT_SENSOR_INIT_DELAY_US); // 80 ms and try again
    if (!sl_si70xx_present(_rht_sensor, SL_WISUN_RHT_SENSOR_ADDRESS, NULL)) {
      sensor_init_res = false;
    }
  }
  assert(sensor_init_res == true);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (kernel_state == osKernelRunning) {
    _rht_mutex_release();
  }
#endif
}

/* Deinit Sensor */
void sl_wisun_rht_deinit(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  // get kernel state
  osKernelState_t kernel_state = osKernelLocked;
  kernel_state = osKernelGetState();

  // lock if needed (kernel is running)
  if (kernel_state == osKernelRunning) {
    _rht_mutex_acquire();
  }
#endif
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_RHT);
#if defined(SL_CATALOG_KERNEL_PRESENT)

  // unlock
  if (kernel_state == osKernelRunning) {
    _rht_mutex_release();
  }
#endif
}

/* Get measurement data */
sl_status_t sl_wisun_rht_get(uint32_t *rh, int32_t *t)
{
  sl_status_t stat = SL_STATUS_FAIL;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _rht_mutex_acquire();
#endif
  stat = sl_si70xx_measure_rh_and_temp(_rht_sensor, SL_WISUN_RHT_SENSOR_ADDRESS, rh, t);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _rht_mutex_release();
#endif
  return stat;
}
