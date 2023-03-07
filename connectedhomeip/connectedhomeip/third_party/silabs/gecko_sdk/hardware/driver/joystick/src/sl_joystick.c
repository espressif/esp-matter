/***************************************************************************//**
 * @file
 * @brief Analog Joystick Driver
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

#include "sl_joystick.h"
#include "em_iadc.h"
#include "em_cmu.h"
#include "sl_status.h"

/***************************************************************************//**
 * An internal local function used for IADC BUS Allocation. It parses through
 * all bus options and allocates a bus to ADC0 if available.
 *
 * @note If no bus is available to be allocated, the function returns
 * SL_STATUS_ALLOCATION_FAILED
 *
 * @param[in] port          Analog Joystick GPIO port
 *
 * @param[in] pin           Analog Joystick GPIO pin
 *
 ******************************************************************************/
static sl_status_t _joystick_IADC_busAllocation(GPIO_Port_TypeDef port, uint8_t pin)
{
  sl_status_t status = SL_STATUS_OK;
  switch (port) {
#if (_GPIO_PORT_A_PIN_COUNT > 0)
    case gpioPortA:

      if (0 == pin % 2) {
        if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AEVEN0_MASK) == GPIO_ABUSALLOC_AEVEN0_TRISTATE) {
          GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0;
        } else if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AEVEN1_MASK) == GPIO_ABUSALLOC_AEVEN1_TRISTATE) {
          GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN1_ADC0;
        } else {
          status = SL_STATUS_ALLOCATION_FAILED;
        }
      } else {
        if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AODD0_MASK) == GPIO_ABUSALLOC_AODD0_TRISTATE) {
          GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD0_ADC0;
        } else if ((GPIO->ABUSALLOC & _GPIO_ABUSALLOC_AODD1_MASK) == GPIO_ABUSALLOC_AODD1_TRISTATE) {
          GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD1_ADC0;
        } else {
          status = SL_STATUS_ALLOCATION_FAILED;
        }
      }
      break;
#endif

#if (_GPIO_PORT_B_PIN_COUNT > 0)
    case gpioPortB:

      if (0 == pin % 2) {
        if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BEVEN0_MASK) == GPIO_BBUSALLOC_BEVEN0_TRISTATE) {
          GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN0_ADC0;
        } else if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BEVEN1_MASK) == GPIO_BBUSALLOC_BEVEN1_TRISTATE) {
          GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN1_ADC0;
        } else {
          status = SL_STATUS_ALLOCATION_FAILED;
        }
      } else {
        if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BODD0_MASK) == GPIO_BBUSALLOC_BODD0_TRISTATE) {
          GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD0_ADC0;
        } else if ((GPIO->BBUSALLOC & _GPIO_BBUSALLOC_BODD1_MASK) == GPIO_BBUSALLOC_BODD1_TRISTATE) {
          GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD1_ADC0;
        } else {
          status = SL_STATUS_ALLOCATION_FAILED;
        }
      }
      break;
#endif

#if (_GPIO_PORT_C_PIN_COUNT > 0 || _GPIO_PORT_D_PIN_COUNT > 0)
    case gpioPortC:
    case gpioPortD:

      if (0 == pin % 2) {
        if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDEVEN0_MASK) == GPIO_CDBUSALLOC_CDEVEN0_TRISTATE) {
          GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;
        } else if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDEVEN1_MASK) == GPIO_CDBUSALLOC_CDEVEN1_TRISTATE) {
          GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN1_ADC0;
        } else {
          status = SL_STATUS_ALLOCATION_FAILED;
        }
      } else {
        if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDODD0_MASK) == GPIO_CDBUSALLOC_CDODD0_TRISTATE) {
          GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0;
        } else if ((GPIO->CDBUSALLOC & _GPIO_CDBUSALLOC_CDODD1_MASK) == GPIO_CDBUSALLOC_CDODD1_TRISTATE) {
          GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD1_ADC0;
        } else {
          status = SL_STATUS_ALLOCATION_FAILED;
        }
      }
      break;
#endif
  }
  return status;
}

/***************************************************************************//**
 * Initializes the Analog Joystick driver.
 ******************************************************************************/
sl_status_t sl_joystick_init(sl_joystick_t *joystick_handle)
{
  sl_status_t status = SL_STATUS_OK;

  // Declare initialization structures
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;

  // Single input structure
  IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;

  // Enable IADC0 and GPIO register clock.
  CMU_ClockEnable(cmuClock_IADC0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Use the FSRC0 as the IADC clock so it can run in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Set timer cycles to configure sampling rate
  init.timerCycles = TIMER_CYCLES;

  // Shutdown between conversions to reduce current
  init.warmup = iadcWarmupNormal;

  // Set the prescaler needed for the intended IADC clock frequency
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  /*
   * Configuration 0 is used by both scan and single conversions by
   * default.  Use unbuffered AVDD as reference and specify the
   * AVDD supply voltage in mV.
   *
   * Resolution is not configurable directly but is based on the
   * selected oversampling ratio (osrHighSpeed), which defaults to
   * 2x and generates 12-bit results.
   */
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
  initAllConfigs.configs[0].vRef = REFERENCE_VOLTAGE;
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;

  /*
   * CLK_SRC_ADC must be prescaled by some value greater than 1 to
   * derive the intended CLK_ADC frequency.
   */
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                     CLK_ADC_FREQ,
                                                                     0,
                                                                     iadcCfgModeNormal,
                                                                     init.srcClkPrescale);

  // Selection of Timer for IADC trigger action
  initSingle.triggerSelect = iadcTriggerSelTimer;

  /*
   * Specify the input channel.  The negInput = iadcNegInputGnd by
   * default
   */
  singleInput.posInput   = IADC_portPinToPosInput(joystick_handle->port,
                                                  joystick_handle->pin);

  // Enable triggering of single conversion
  initSingle.start = true;

  // Allocate the analog bus for ADC0 inputs
  status = _joystick_IADC_busAllocation(joystick_handle->port,
                                        joystick_handle->pin);
  if (SL_STATUS_OK != status) {
    return status;
  }

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize a single-channel conversion
  IADC_initSingle(IADC0, &initSingle, &singleInput);

  return status;
}

/***************************************************************************//**
 * Starts Analog Joystick data acquisition.
 ******************************************************************************/
void sl_joystick_start(sl_joystick_t *joystick_handle)
{
  /* Check if already enabled */
  if (joystick_handle->state != SL_JOYSTICK_DISABLED) {
    return;
  }

  joystick_handle->state = SL_JOYSTICK_ENABLED;

  IADC_command(IADC0, iadcCmdEnableTimer);

  /*
   * To check at least one result in the single FIFO is
   * ready to be read
   */
  while (0 == (IADC0->STATUS_CLR & IADC_STATUS_SINGLEFIFODV)) ;
}

/***************************************************************************//**
 * Stops Analog Joystick data acquisition.
 ******************************************************************************/
void sl_joystick_stop(sl_joystick_t *joystick_handle)
{
  /* Check if already disabled */
  if (joystick_handle->state != SL_JOYSTICK_ENABLED) {
    return;
  }

  joystick_handle->state = SL_JOYSTICK_DISABLED;

  IADC_command(IADC0, iadcCmdDisableTimer);
}

/***************************************************************************//**
 * Get joystick position.
 ******************************************************************************/
sl_status_t sl_joystick_get_position(sl_joystick_t *joystick_handle, sl_joystick_position_t *pos)
{
  sl_joystick_position_t joystickDirection;

  IADC_Result_t sample;

  uint32_t singleResult;

  // Return error if joystick data acquisistion not started
  if (joystick_handle->state != SL_JOYSTICK_ENABLED) {
    return SL_STATUS_NOT_READY;
  }

  // Read most recent single conversion result
  sample = IADC_readSingleResult(IADC0);

  /*
   * Calculate the voltage converted as follows:
   *
   * For single-ended conversions, the result can range from 0 to
   * +Vref, i.e., for Vref = AVDD = 3300 mV, 0xFFF represents the
   * full scale value of 3300 mV.
   */
  singleResult = (sample.data * REFERENCE_VOLTAGE) / 0xFFF;

  uint32_t mV = singleResult;

  // determine which direction pad was pressed
  if ((mV <= JOYSTICK_MV_C + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_C;
  } else if ((mV >= JOYSTICK_MV_N - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_N + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_N;
  } else if ((mV >= JOYSTICK_MV_E - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_E + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_E;
  } else if ((mV >= JOYSTICK_MV_S - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_S + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_S;
  } else if ((mV >= JOYSTICK_MV_W - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_W + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_W;
  }
#if ENABLE_SECONDARY_DIRECTIONS == 1
  else if ((mV >= JOYSTICK_MV_NE - JOYSTICK_MV_ERROR) \
           && (mV <= JOYSTICK_MV_NE + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_NE;
  } else if ((mV >= JOYSTICK_MV_NW - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_NW + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_NW;
  } else if ((mV >= JOYSTICK_MV_SE - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_SE + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_SE;
  } else if ((mV >= JOYSTICK_MV_SW - JOYSTICK_MV_ERROR) \
             && (mV <= JOYSTICK_MV_SW + JOYSTICK_MV_ERROR)) {
    joystickDirection = JOYSTICK_SW;
  }
#endif
  else {
    joystickDirection = JOYSTICK_NONE;
  }

  *pos = joystickDirection;

  return SL_STATUS_OK;
}
