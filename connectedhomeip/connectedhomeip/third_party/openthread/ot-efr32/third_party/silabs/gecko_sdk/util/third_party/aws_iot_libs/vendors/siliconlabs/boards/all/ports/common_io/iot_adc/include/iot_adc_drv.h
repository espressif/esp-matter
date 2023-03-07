/***************************************************************************//**
 * @file    iot_adc_drv.h
 * @brief   ADC driver header file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_ADC_DRV_H_
#define _IOT_ADC_DRV_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "em_core.h"
#include "em_device.h"

#include "sl_status.h"

#include "iot_adc_drv_sar.h"
#include "iot_adc_drv_inc.h"

/*******************************************************************************
 *                               PROTOTYPES
 ******************************************************************************/

/************************ iot_adc_drv_driver_init *****************************/

static inline
sl_status_t iot_adc_drv_driver_init(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_driver_init(pvHndl);
#else
  return iot_adc_drv_inc_driver_init(pvHndl);
#endif
}

/*********************** iot_adc_drv_driver_deinit ****************************/

static inline
sl_status_t iot_adc_drv_driver_deinit(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_driver_deinit(pvHndl);
#else
  return iot_adc_drv_inc_driver_deinit(pvHndl);
#endif
}

/************************ iot_adc_drv_config_set ******************************/

static inline
sl_status_t iot_adc_drv_config_set(void *pvHndl,
                                   uint32_t ulAq,
                                   uint8_t ucRs)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_config_set(pvHndl, ulAq, ucRs);
#else
  return iot_adc_drv_inc_config_set(pvHndl, ulAq, ucRs);
#endif
}

/************************ iot_adc_drv_config_get ******************************/

static inline
sl_status_t iot_adc_drv_config_get(void *pvHndl,
                                   uint32_t *pulAq,
                                   uint8_t *pucRs)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_config_get(pvHndl, pulAq, pucRs);
#else
  return iot_adc_drv_inc_config_get(pvHndl, pulAq, pucRs);
#endif
}

/************************* iot_adc_drv_hw_enable ******************************/

static inline
sl_status_t iot_adc_drv_hw_enable(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_hw_enable(pvHndl);
#else
  return iot_adc_drv_inc_hw_enable(pvHndl);
#endif
}

/************************ iot_adc_drv_hw_disable ******************************/

static inline
sl_status_t iot_adc_drv_hw_disable(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_hw_disable(pvHndl);
#else
  return iot_adc_drv_inc_hw_disable(pvHndl);
#endif
}

/************************* iot_adc_drv_bus_alloc ******************************/

static inline
sl_status_t iot_adc_drv_bus_alloc(void *pvHndl, uint8_t ucCh)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_bus_alloc(pvHndl, ucCh);
#else
  return iot_adc_drv_inc_bus_alloc(pvHndl, ucCh);
#endif
}

/************************ iot_adc_drv_bus_dealloc *****************************/

static inline
sl_status_t iot_adc_drv_bus_dealloc(void *pvHndl, uint8_t ucCh)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_bus_dealloc(pvHndl, ucCh);
#else
  return iot_adc_drv_inc_bus_dealloc(pvHndl, ucCh);
#endif
}

/************************ iot_adc_drv_single_init *****************************/

static inline
sl_status_t iot_adc_drv_single_init(void *pvHndl, uint8_t ucCh)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_single_init(pvHndl, ucCh);
#else
  return iot_adc_drv_inc_single_init(pvHndl, ucCh);
#endif
}

/************************ iot_adc_drv_single_start ****************************/

static inline
sl_status_t iot_adc_drv_single_start(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_single_start(pvHndl);
#else
  return iot_adc_drv_inc_single_start(pvHndl);
#endif
}

/************************ iot_adc_drv_single_read *****************************/

static inline
sl_status_t iot_adc_drv_single_read(void *pvHndl, uint16_t *pusSample)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_single_read(pvHndl, pusSample);
#else
  return iot_adc_drv_inc_single_read(pvHndl, pusSample);
#endif
}

/************************ iot_adc_drv_multi_init ******************************/

static inline
sl_status_t iot_adc_drv_multi_init(void *pvHndl, uint8_t ucCh)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_multi_init(pvHndl, ucCh);
#else
  return iot_adc_drv_inc_multi_init(pvHndl, ucCh);
#endif
}

/************************ iot_adc_drv_multi_start *****************************/

static inline
sl_status_t iot_adc_drv_multi_start(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_multi_start(pvHndl);
#else
  return iot_adc_drv_inc_multi_start(pvHndl);
#endif
}

/************************ iot_adc_drv_multi_read ******************************/

static inline
sl_status_t iot_adc_drv_multi_read(void *pvHndl, uint16_t *pusSample)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_multi_read(pvHndl, pusSample);
#else
  return iot_adc_drv_inc_multi_read(pvHndl, pusSample);
#endif
}

/************************ iot_adc_drv_irq_enable ******************************/

static inline
sl_status_t iot_adc_drv_irq_enable(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_irq_enable(pvHndl);
#else
  return iot_adc_drv_inc_irq_enable(pvHndl);
#endif
}

/************************ iot_adc_drv_irq_disable *****************************/

static inline
sl_status_t iot_adc_drv_irq_disable(void *pvHndl)
{
#if (_SILICON_LABS_32B_SERIES < 2)
  return iot_adc_drv_sar_irq_disable(pvHndl);
#else
  return iot_adc_drv_inc_irq_disable(pvHndl);
#endif
}

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_ADC_DRV_H_ */
