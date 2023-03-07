/***************************************************************************//**
 * @file    iot_adc_drv_inc.c
 * @brief   Incremental ADC driver.
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
#include "iot_adc_drv_inc.h"

/*******************************************************************************
 *                             START OF MODULE
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES == 2)

/*******************************************************************************
 *                             PIN MAPPING
 ******************************************************************************/

static const iot_adc_drv_inc_map_t iot_adc_drv_inc_map =
             IOT_ADC_DRV_INC_MAPPING;

/*******************************************************************************
 *                      iot_adc_drv_inc_driver_init()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_driver_init(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* reset IADC configs to default */
  pvDesc->ulAcqTime    = pvDesc->ulDefaultAcqTime;
  pvDesc->ucResolution = pvDesc->ucDefaultResolution;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_adc_drv_inc_driver_deinit()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_driver_deinit(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* reset IADC config to 0 */
  pvDesc->ulAcqTime    = 0;
  pvDesc->ucResolution = 0;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_config_set()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_config_set(void *pvHndl,
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
 *                       iot_adc_drv_inc_config_get()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_config_get(void *pvHndl,
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
 *                       iot_adc_drv_inc_hw_enable()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_hw_enable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc                  = pvHndl;
  uint32_t                    ulIAdcFreq      = 1000000;
  IADC_Init_t                 xIAdcInit       = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t           xInitAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* enable IADC clock */
  CMU_ClockEnable(pvDesc->xClock, true);

  /* Select clock for IADC (FSRCO - 20MHz) */
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  /* reset IADC h/w */
  IADC_reset(pvDesc->pxPeripheral);

  /* update xIAdcInit structure */
  xIAdcInit.warmup         = iadcWarmupKeepWarm;
  xIAdcInit.srcClkPrescale = IADC_calcSrcClkPrescale(pvDesc->pxPeripheral,
                                                     ulIAdcFreq,
                                                     0);

  /* update config 0 (used by both single and scan modes) */
  xInitAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
  xInitAllConfigs.configs[0].adcClkPrescale =
          IADC_calcAdcClkPrescale(pvDesc->pxPeripheral,
                                  ulIAdcFreq,
                                  0,
                                  iadcCfgModeNormal,
                                  xIAdcInit.srcClkPrescale);

  /* initialize the IADC */
  IADC_init(pvDesc->pxPeripheral, &xIAdcInit, &xInitAllConfigs);

  /* disable all interrupts */
  IADC_disableInt(pvDesc->pxPeripheral, _IADC_IF_MASK);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_hw_disable()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_hw_disable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* reset IADC h/w */
  IADC_reset(pvDesc->pxPeripheral);

  /* disable IADC clock */
  CMU_ClockEnable(pvDesc->xClock, false);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_adc_drv_inc_bus_alloc()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_bus_alloc(void *pvHndl, uint8_t ucChannel)
{
  CORE_DECLARE_IRQ_STATE;

  /* pvHndl not used here */
  (void) pvHndl;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* allocate the analog bus */
  switch (ucChannel & 0xF0) {
    case 0xA0:
      if (ucChannel & 1) {
        GPIO->ABUSALLOC  |= GPIO_ABUSALLOC_AODD0_ADC0;
      } else {
        GPIO->ABUSALLOC  |= GPIO_ABUSALLOC_AEVEN0_ADC0;
      }
      break;
    case 0xB0:
      if (ucChannel & 1) {
        GPIO->BBUSALLOC  |= GPIO_BBUSALLOC_BODD0_ADC0;
      } else {
        GPIO->BBUSALLOC  |= GPIO_BBUSALLOC_BEVEN0_ADC0;
      }
      break;
    case 0xC0:
    case 0xD0:
      if (ucChannel & 1) {
        GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0;
      } else {
        GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;
      }
      break;
    default:
      break;
  }

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_adc_drv_inc_bus_dealloc()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_bus_dealloc(void *pvHndl, uint8_t ucChannel)
{
  CORE_DECLARE_IRQ_STATE;

  /* pvHndl not used here */
  (void) pvHndl;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* free up the analog bus */
  switch (ucChannel & 0xF0) {
    case 0xA0:
      if (ucChannel & 1) {
        GPIO->ABUSALLOC  &= ~(GPIO_ABUSALLOC_AODD0_ADC0);
      } else {
        GPIO->ABUSALLOC  &= ~(GPIO_ABUSALLOC_AEVEN0_ADC0);
      }
      break;
    case 0xB0:
      if (ucChannel & 1) {
        GPIO->BBUSALLOC  &= ~(GPIO_BBUSALLOC_BODD0_ADC0);
      } else {
        GPIO->BBUSALLOC  &= ~(GPIO_BBUSALLOC_BEVEN0_ADC0);
      }
      break;
    case 0xC0:
    case 0xD0:
      if (ucChannel & 1) {
        GPIO->CDBUSALLOC &= ~(GPIO_CDBUSALLOC_CDODD0_ADC0);
      } else {
        GPIO->CDBUSALLOC &= ~(GPIO_CDBUSALLOC_CDEVEN0_ADC0);
      }
      break;
    default:
      break;
  }

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_single_init()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_single_init(void *pvHndl, uint8_t ucCh)
{
  IotAdcDescriptor_t  *pvDesc           = pvHndl;
  IADC_InitSingle_t            xInitSingle      = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t           xInitSingleInput = IADC_SINGLEINPUT_DEFAULT;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* specify channel */
  xInitSingleInput.posInput = iot_adc_drv_inc_map[ucCh].posInput;
  xInitSingleInput.negInput = iadcNegInputGnd;

  /* initialize single mode */
  IADC_initSingle(pvDesc->pxPeripheral, &xInitSingle, &xInitSingleInput);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_single_start()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_single_start(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* start next conversion (single mode) */
  IADC_command(pvDesc->pxPeripheral, iadcCmdStartSingle);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_single_read()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_single_read(void *pvHndl, uint16_t *pusSample)
{
  IotAdcDescriptor_t *pvDesc  = pvHndl;
  uint32_t                    ulMaskC = _IADC_STATUS_CONVERTING_MASK;
  uint32_t                    ulMaskS = _IADC_STATUS_SINGLEFIFODV_MASK;
  uint32_t                    ulBusy  = IADC_STATUS_SINGLEFIFODV;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* wait until conversion is complete */
  while ((pvDesc->pxPeripheral->STATUS & (ulMaskC|ulMaskS)) != ulBusy);

  /* read single sample */
  *pusSample = IADC_pullSingleFifoResult(pvDesc->pxPeripheral).data;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_multi_init()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_multi_init(void *pvHndl, uint8_t ucCh)
{
  IotAdcDescriptor_t *pvDesc         = pvHndl;
  IADC_InitScan_t             xInitScan      = IADC_INITSCAN_DEFAULT;
  IADC_ScanTable_t            xInitScanTable = IADC_SCANTABLE_DEFAULT;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* setup xInitScan struct (parameters) */
  xInitScan.showId = true;

  /* add channel to scan table */
  xInitScanTable.entries[0].posInput = iot_adc_drv_inc_map[ucCh].posInput;
  xInitScanTable.entries[0].negInput = iadcNegInputGnd;
  xInitScanTable.entries[0].includeInScan = true;

  /* initialize scan mode */
  IADC_initScan(pvDesc->pxPeripheral, &xInitScan, &xInitScanTable);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_adc_drv_inc_multi_start()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_multi_start(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* start next conversion (scan mode) */
  IADC_command(pvDesc->pxPeripheral, iadcCmdStartScan);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_adc_drv_inc_multi_read()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_multi_read(void *pvHndl, uint16_t *pusSample)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* read sample (scan mode) */
  *pusSample = IADC_pullScanFifoResult(pvDesc->pxPeripheral).data;

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                     iot_adc_drv_inc_irq_enable()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_irq_enable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* enable scan interrupts */
  IADC_enableInt(pvDesc->pxPeripheral, IADC_IEN_SCANTABLEDONE);

  /* clear any pending interrupt in NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xIRQn);

  /* enable IADC interrupts in NVIC */
  NVIC_EnableIRQ(pvDesc->xIRQn);

  /* exit critical section */
  CORE_EXIT_ATOMIC();

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                    iot_adc_drv_inc_irq_disable()
 ******************************************************************************/

sl_status_t iot_adc_drv_inc_irq_disable(void *pvHndl)
{
  IotAdcDescriptor_t *pvDesc = pvHndl;
  CORE_DECLARE_IRQ_STATE;

  /* enter critical section */
  CORE_ENTER_ATOMIC();

  /* disable scan interrupts */
  IADC_disableInt(pvDesc->pxPeripheral, IADC_IEN_SCANTABLEDONE);

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
 *                            IADC_IRQHandler()
 ******************************************************************************/

#ifdef IADC0
void IADC_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_adc_desc_get(++lInstNum)->pxPeripheral != IADC0);

  /* invoke callback function */
  IOT_ADC_CB(lInstNum);

  /* clear interrupt flags */
  IADC_clearInt(IADC0, _IADC_IF_MASK);
}
#endif

/*******************************************************************************
 *                             END OF MODULE
 ******************************************************************************/

#endif
