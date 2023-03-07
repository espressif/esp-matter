/***************************************************************************//**
 * @file    iot_adc_drv_sar.c
 * @brief   Successive-Approximation Register ADC Driver
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
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_cmu.h"

/* ADC driver layer */
#include "iot_adc_cb.h"
#include "iot_adc_desc.h"
#include "iot_adc_drv_sar.h"

/*******************************************************************************
 *                             START OF MODULE
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES == 0) || (_SILICON_LABS_32B_SERIES == 1)

/*******************************************************************************
 *                             PIN MAPPING
 ******************************************************************************/

static const iot_adc_drv_sar_map_t iot_adc_drv_sar_map =
             IOT_ADC_DRV_SAR_MAPPING;

/*******************************************************************************
 *                  iot_adc_drv_sar_read_reference()
 ******************************************************************************/

static ADC_Ref_TypeDef iot_adc_drv_sar_read_reference(void *pvHndl)
{
  (void) pvHndl;
  return adcRefVDD;
}

/*******************************************************************************
 *                   iot_adc_drv_sar_read_acqtime()
 ******************************************************************************/

static ADC_AcqTime_TypeDef iot_adc_drv_sar_read_acqtime(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->ulAcqTime == 1) {
    return adcAcqTime1;
  } else if (pvDesc->ulAcqTime == 2) {
    return adcAcqTime2;
  } else if (pvDesc->ulAcqTime <= 4) {
    return adcAcqTime4;
  } else if (pvDesc->ulAcqTime <= 8) {
    return adcAcqTime8;
  } else if (pvDesc->ulAcqTime <= 16) {
    return adcAcqTime16;
  } else if (pvDesc->ulAcqTime <= 32) {
    return adcAcqTime32;
  } else if (pvDesc->ulAcqTime <= 64) {
    return adcAcqTime64;
  } else if (pvDesc->ulAcqTime <= 128) {
    return adcAcqTime128;
  } else if (pvDesc->ulAcqTime <= 256) {
    return adcAcqTime256;
  } else {
    return adcAcqTime4;
  }
}

/*******************************************************************************
 *                  iot_adc_drv_sar_read_resolution()
 ******************************************************************************/

static ADC_Res_TypeDef iot_adc_drv_sar_read_resolution(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  if (pvDesc->ucResolution == 12) {
    return adcRes12Bit;
  } else if (pvDesc->ucResolution == 8) {
    return adcRes8Bit;
  } else if (pvDesc->ucResolution == 6) {
    return adcRes6Bit;
  } else {
    return adcRes8Bit;
  }
}

/*******************************************************************************
 *                    iot_adc_drv_sar_driver_init()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_driver_init(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* reset ADC configs to default */
  pvDesc->ulAcqTime    = pvDesc->ulDefaultAcqTime;
  pvDesc->ucResolution = pvDesc->ucDefaultResolution;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_adc_drv_sar_driver_deinit()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_driver_deinit(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* reset ADC config to 0 */
  pvDesc->ulAcqTime    = 0;
  pvDesc->ucResolution = 0;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_config_set()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_config_set(void *pvHndl,
                                          uint32_t ulAq,
                                          uint8_t ucRs)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* store sample time */
  pvDesc->ulAcqTime = ulAq;

  /* store resolution */
  pvDesc->ucResolution = ucRs;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_config_get()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_config_get(void *pvHndl,
                                          uint32_t *pulAq,
                                          uint8_t *pucRs)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* restore sample time */
  *pulAq = pvDesc->ulAcqTime;

  /* restore resolution */
  *pucRs = pvDesc->ucResolution;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_adc_drv_sar_hw_enable()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_hw_enable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc    = pvHndl;
  uint32_t                    ulAdcFreq = 16000;
  ADC_Init_TypeDef            xAdcInit  = ADC_INIT_DEFAULT;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* enable ADC clock */
  CMU_ClockEnable(pvDesc->xClock, true);

  /* update init structure */
  xAdcInit.prescale = ADC_PrescaleCalc(ulAdcFreq, 0);
  xAdcInit.timebase = ADC_TimebaseCalc(0);

  /* initialize the peripheral */
  ADC_Init(pvDesc->pxPeripheral, &xAdcInit);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_hw_disable()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_hw_disable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* reset ADC h/w */
  ADC_Reset(pvDesc->pxPeripheral);

  /* disable ADC clock */
  CMU_ClockEnable(pvDesc->xClock, false);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_sar_bus_alloc()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_bus_alloc(void *pvHndl, uint8_t ucChannel)
{
  /* do nothing */
  (void) pvHndl;
  (void) ucChannel;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_bus_dealloc()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_bus_dealloc(void *pvHndl, uint8_t ucChannel)
{
  /* do nothing */
  (void) pvHndl;
  (void) ucChannel;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_adc_drv_sar_single_init()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_single_init(void *pvHndl, uint8_t ucCh)
{
  IotAdcDescriptor_t *pvDesc      = pvHndl;
  ADC_InitSingle_TypeDef      xInitSingle = ADC_INITSINGLE_DEFAULT;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

#if (_SILICON_LABS_32B_SERIES < 1)
  /* setup xinitSingle struct */
  xInitSingle.reference = iot_adc_drv_sar_read_reference(pvDesc);
  xInitSingle.acqTime = iot_adc_drv_sar_read_acqtime(pvDesc);
  xInitSingle.resolution = iot_adc_drv_sar_read_resolution(pvDesc);
  xInitSingle.diff = false;

  /* specify channel */
  xInitSingle.input = iot_adc_drv_sar_map[ucCh].singleInput;

  /* initialize scan mode */
  ADC_InitSingle(pvDesc->pxPeripheral, &xInitSingle);
#else
  /* setup xinitSingle struct */
  xInitSingle.reference = iot_adc_drv_sar_read_reference(pvDesc);
  xInitSingle.acqTime = iot_adc_drv_sar_read_acqtime(pvDesc);
  xInitSingle.resolution = iot_adc_drv_sar_read_resolution(pvDesc);
  xInitSingle.fifoOverwrite = true;
  xInitSingle.diff = false;

  /* specify channel */
  xInitSingle.posSel = iot_adc_drv_sar_map[ucCh][pvDesc->ucPeripheralNo].posSel;
  xInitSingle.negSel = adcNegSelVSS;

  /* initialize scan mode */
  ADC_InitSingle(pvDesc->pxPeripheral, &xInitSingle);
#endif

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_single_start()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_single_start(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* start next conversion (single mode) */
  ADC_Start(pvDesc->pxPeripheral, adcStartSingle);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_single_read()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_single_read(void *pvHndl, uint16_t *pusSample)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* wait until conversion is complete */
  while(!(pvDesc->pxPeripheral->STATUS & _ADC_STATUS_SINGLEDV_MASK));

  /* read single sample */
  *pusSample = ADC_DataSingleGet(pvDesc->pxPeripheral);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_multi_init()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_multi_init(void *pvHndl, uint8_t ucCh)
{
  IotAdcDescriptor_t *pvDesc    = pvHndl;
  ADC_InitScan_TypeDef        xInitScan = ADC_INITSCAN_DEFAULT;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

#if (_SILICON_LABS_32B_SERIES < 1)
  /* setup xInitScan struct (parameters) */
  xInitScan.reference = iot_adc_drv_sar_read_reference(pvDesc);
  xInitScan.acqTime = iot_adc_drv_sar_read_acqtime(pvDesc);
  xInitScan.resolution = iot_adc_drv_sar_read_resolution(pvDesc);
  xInitScan.diff = false;

  /* add channel to scan table */
  xInitScan.input = iot_adc_drv_sar_map[ucCh].singleInput;

  /* initialize scan mode */
  ADC_InitScan(pvDesc->pxPeripheral, &xInitScan);
#else
  /* setup xInitScan struct (parameters) */
  xInitScan.reference = iot_adc_drv_sar_read_reference(pvDesc);
  xInitScan.acqTime = iot_adc_drv_sar_read_acqtime(pvDesc);
  xInitScan.resolution = iot_adc_drv_sar_read_resolution(pvDesc);
  xInitScan.fifoOverwrite = true;
  xInitScan.diff = false;

  /* add channel to scan table */
  ADC_ScanSingleEndedInputAdd(
       &xInitScan,
       adcScanInputGroup0,
       iot_adc_drv_sar_map[ucCh][pvDesc->ucPeripheralNo].posSel);

  /* initialize scan mode */
  ADC_InitScan(pvDesc->pxPeripheral, &xInitScan);
#endif

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_sar_multi_start()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_multi_start(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* start next conversion (scan mode) */
  ADC_Start(pvDesc->pxPeripheral, adcStartScan);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_multi_read()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_multi_read(void *pvHndl, uint16_t *pusSample)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* read sample (scan mode) */
  *pusSample = ADC_DataScanGet(pvDesc->pxPeripheral);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_irq_enable()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_irq_enable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* enable scan interrupts */
  ADC_IntEnable(pvDesc->pxPeripheral, ADC_IEN_SCAN);

  /* clear any pending interrupt in NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xIRQn);

  /* enable ADC interrupts in NVIC */
  NVIC_EnableIRQ(pvDesc->xIRQn);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_sar_irq_disable()
 ******************************************************************************/

sl_status_t iot_adc_drv_sar_irq_disable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* enable scan interrupts */
  ADC_IntDisable(pvDesc->pxPeripheral, ADC_IEN_SCAN);

  /* clear any pending interrupt in NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xIRQn);

  /* disable ADC interrupts in NVIC */
  NVIC_DisableIRQ(pvDesc->xIRQn);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                            ADC0_IRQHandler()
 ******************************************************************************/

#ifdef ADC0
void ADC0_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_adc_desc_get(++lInstNum)->pxPeripheral != ADC0);

  /* invoke callback function */
  IOT_ADC_CB(lInstNum);
}
#endif

/*******************************************************************************
 *                            ADC1_IRQHandler()
 ******************************************************************************/

#ifdef ADC1
void ADC1_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_adc_desc_get(++lInstNum)->pxPeripheral != ADC1);

  /* invoke callback function */
  IOT_ADC_CB(lInstNum);
}
#endif

/*******************************************************************************
 *                             END OF MODULE
 ******************************************************************************/

#endif
