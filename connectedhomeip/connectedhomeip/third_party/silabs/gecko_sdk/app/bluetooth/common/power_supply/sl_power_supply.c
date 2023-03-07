/***************************************************************************//**
 * @file
 * @brief Power supply measurement
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

#include "em_device.h"
#include "em_cmu.h"
#if defined(ADC_PRESENT)
#include "em_adc.h"
#elif defined(IADC_PRESENT)
#include "em_iadc.h"
#endif

#include "sl_board_control.h"
#include "sl_sleeptimer.h"
#include "sl_si70xx.h"
#include "sl_sensor_select.h"

#include "app_assert.h"
#include "app_log.h"
#include "sl_power_supply.h"

// -----------------------------------------------------------------------------
// Private macros

#define SI7021_CMD_WRITE_USER_REG1       0xE6 ///< Write RH/T User Register 1
#define SI7021_CMD_READ_USER_REG1        0xE7 ///< Read RH/T User Register 1
#define SI7021_CMD_WRITE_HEATER_CTRL     0x51 ///< Write Heater Control Register
#define SI7021_CMD_READ_HEATER_CTRL      0x11 ///< Read Heater Control Register

#if defined(ADC_PRESENT)
// 5V reference voltage, no attenuation on AVDD, 12 bit ADC data
  #define ADC_SCALE_FACTOR   (5.0 / 4095.0)
#elif defined(IADC_PRESENT)
// 1.21V reference voltage, AVDD attenuated by a factor of 4, 12 bit ADC data
  #define ADC_SCALE_FACTOR   (4.84 / 4095.0)
#endif

// -----------------------------------------------------------------------------
// Private type definitions

typedef struct {
  float voltage;
  uint8_t capacity;
} batt_model_entry_t;

// -----------------------------------------------------------------------------
// Private variables

static float supply_voltage = 0.0f;                         ///< Supply voltage
static float supply_ir = 0.0f;                              ///< Internal resistance of the supply
static uint8_t supply_type = SL_POWER_SUPPLY_TYPE_UNKNOWN;  ///< Type of the connected supply

static batt_model_entry_t batt_model_cr2032[] =
{ { 3.0, 100 }, { 2.9, 80 }, { 2.8, 60 }, { 2.7, 40 }, { 2.6, 30 },
  { 2.5, 20 }, { 2.4, 10 }, { 2.0, 0 } };

static sl_i2cspm_t *rht_sensor;

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Initialize A/D converter.
 ******************************************************************************/
static void adc_init(void);

/***************************************************************************//**
 * Initiate an A/D conversion and read the sample when done.
 *
 * @return The output of the A/D conversion.
 ******************************************************************************/
static uint16_t get_adc_sample(void);

/***************************************************************************//**
 * Send a command and data to the Si7021 chip over the I2C bus.
 *
 * @param[in] cmd The command to be sent.
 * @param[in] cmd_len The length of the command in bytes.
 * @param[out] data The data byte(s) to be sent to the chip.
 * @param[in] data_len The number of the bytes to be sent to the chip.
 ******************************************************************************/
static void si7021_cmd_write(uint8_t *cmd, uint16_t cmd_len, uint8_t *data, uint16_t data_len);

/***************************************************************************//**
 * Measure the the internal resistance of the connected power supply.
 *
 * The internal resistance is calculated from the unloaded and loaded supply
 * voltage. The load is provided by the heater element built in the Si7021.
 *
 * @param[in] loadSetting Heater current setting of the Si7021.
 * @return The measured internal resistance of the connected supply
 ******************************************************************************/
static float measure_supply_ir(uint8_t load_setting);

/***************************************************************************//**
 * Calculate battery level based on the model of the battery.
 *
 * @param[in] voltage Battery voltage.
 * @param[in] model Battery model.
 * @param[in] model_entry_count Number of entries in the battery model.
 * @return The estimated battery capacity level in percent.
 ******************************************************************************/
static uint8_t calculate_level(float voltage, batt_model_entry_t *model, uint8_t model_entry_count);

// -----------------------------------------------------------------------------
// Private function definitions

static void adc_init(void)
{
#if defined(ADC_PRESENT)
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef init_single = ADC_INITSINGLE_DEFAULT;

  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  // Only configure the ADC if it is not already running.
  if ( ADC0->CTRL == _ADC_CTRL_RESETVALUE ) {
    init.timebase = ADC_TimebaseCalc(0);
    init.prescale = ADC_PrescaleCalc(1000000, 0);
    ADC_Init(ADC0, &init);
  }

  init_single.acqTime = adcAcqTime16;
  init_single.reference = adcRef5VDIFF;
  init_single.posSel = adcPosSelAVDD;
  init_single.negSel = adcNegSelVSS;
  init_single.prsEnable = true;
  init_single.prsSel = adcPRSSELCh4;

  ADC_InitSingle(ADC0, &init_single);
#elif defined(IADC_PRESENT)
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t all_configs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t init_single = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t input = IADC_SINGLEINPUT_DEFAULT;

  CMU_ClockEnable(cmuClock_IADC0, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  // Only configure the ADC if it is not already running
  if ( IADC0->CTRL == _IADC_CTRL_RESETVALUE ) {
    IADC_init(IADC0, &init, &all_configs);
  }

  input.posInput = iadcPosInputAvdd;

  IADC_initSingle(IADC0, &init_single, &input);
  IADC_enableInt(IADC0, IADC_IEN_SINGLEDONE);
#endif
  return;
}

static uint16_t get_adc_sample(void)
{
#if defined(ADC_PRESENT)
  // Start conversion and wait for result
  ADC_Start(ADC0, adcStartSingle);
  while ( !(ADC_IntGet(ADC0) & ADC_IF_SINGLE) ) ;

  return ADC_DataSingleGet(ADC0);
#elif defined(IADC_PRESENT)
  // Clear single done interrupt
  IADC_clearInt(IADC0, IADC_IF_SINGLEDONE);

  // Start conversion and wait for result
  IADC_command(IADC0, iadcCmdStartSingle);
  while ( !(IADC_getInt(IADC0) & IADC_IF_SINGLEDONE) ) ;

  return IADC_readSingleData(IADC0);
#endif
}

static void si7021_cmd_write(uint8_t *cmd, uint16_t cmd_len, uint8_t *data, uint16_t data_len)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7021_ADDR << 1;
  seq.buf[0].data = cmd;
  seq.buf[0].len = cmd_len;

  if ( data_len > 0 ) {
    seq.flags = I2C_FLAG_WRITE_WRITE;
    seq.buf[1].data = data;
    seq.buf[1].len = data_len;
  } else {
    seq.flags = I2C_FLAG_WRITE;
  }

  ret = I2CSPM_Transfer(rht_sensor, &seq);

  app_assert(ret == i2cTransferDone,
             "[E: 0x%04x] Failed to write to SI7021\n",
             (int)ret);
}

static float measure_supply_ir(uint8_t loadSetting)
{
  float supplyVoltage;
  float supplyVoltageLoad;
  float i, r;

  uint8_t cmd;
  uint8_t data;

  sl_sleeptimer_delay_millisecond(250);
  supplyVoltage = sl_power_supply_measure_voltage(16);

  // Enable heater in Si7021 - 9.81 mA
  cmd = SI7021_CMD_WRITE_HEATER_CTRL;
  data = loadSetting;
  si7021_cmd_write(&cmd, 1, &data, 1);

  cmd = SI7021_CMD_WRITE_USER_REG1;
  data = 0x04;
  si7021_cmd_write(&cmd, 1, &data, 1);

  // Wait for battery voltage to settle.
  sl_sleeptimer_delay_millisecond(250);
  supplyVoltageLoad = sl_power_supply_measure_voltage(16);

  // Turn off heater.
  cmd = SI7021_CMD_WRITE_USER_REG1;
  data = 0x00;
  si7021_cmd_write(&cmd, 1, &data, 1);

  i = 0.006074 * loadSetting + 0.00309;
  r = (supplyVoltage - supplyVoltageLoad) / i;

  app_log_info(" sv = %.3f  svl = %.3f   i = %.3f   r = %.3f\n",
               supplyVoltage, supplyVoltageLoad, i, r);

  return r;
}

static uint8_t calculate_level(float voltage, batt_model_entry_t *model, uint8_t model_entry_count)
{
  uint8_t res = 0;

  if (voltage >= model[0].voltage) {
    // Return with max capacity if voltage is greater than the max voltage in the model.
    res = model[0].capacity;
  } else if (voltage <= model[model_entry_count - 1].voltage) {
    // Return with min capacity if voltage is smaller than the min voltage in the model.
    res = model[model_entry_count - 1].capacity;
  } else {
    uint8_t i;
    // Otherwise find the 2 points in the model where the voltage level fits in between,
    // and do linear interpolation to get the estimated capacity value.
    for (i = 0; i < (model_entry_count - 1); i++) {
      if ((voltage < model[i].voltage) && (voltage >= model[i + 1].voltage)) {
        res = (uint8_t)((voltage - model[i + 1].voltage)
                        * (model[i].capacity - model[i + 1].capacity)
                        / (model[i].voltage - model[i + 1].voltage));
        res += model[i + 1].capacity;
        break;
      }
    }
  }

  return res;
}

// -----------------------------------------------------------------------------
// Public function definitions

/***************************************************************************//**
 * Probe the connected supply and determine its type.
 ******************************************************************************/
void sl_power_supply_probe(void)
{
  sl_status_t sc;
  uint8_t type = SL_POWER_SUPPLY_TYPE_UNKNOWN;
  float r, v;

  rht_sensor = sl_sensor_select(SL_BOARD_SENSOR_RHT);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_RHT);
  app_assert((SL_STATUS_OK == sc) && (NULL != rht_sensor),
             "[E: %#04x] Si7021 sensor not available\n",
             sc);
  sc = sl_si70xx_init(rht_sensor, SI7021_ADDR);

  if (sc == SL_STATUS_OK) {
    // Try to measure using 9.18 mA first.
    v = sl_power_supply_measure_voltage(16);
    r = measure_supply_ir(0x00);
    if ( r > 5.0 ) {
      type = SL_POWER_SUPPLY_TYPE_CR2032;
    } else if (r > 0.5) {
      type = SL_POWER_SUPPLY_TYPE_AAA;
    } else {
      type = SL_POWER_SUPPLY_TYPE_USB;
    }

    // Store measurement results in global variables.
    supply_voltage = v;
    supply_ir = r;
    supply_type = type;
  } else {
    app_log_warning("Si7021 sensor initialization failed. "
                    "Unable to detect power supply type." APP_LOG_NL);
  }
}

/***************************************************************************//**
 * Retrieve the supply characteristic variables.
 ******************************************************************************/
void sl_power_supply_get_characteristics(uint8_t *type, float *voltage, float *ir)
{
  *type    = supply_type;
  *voltage = supply_voltage;
  *ir      = supply_ir;
}

/***************************************************************************//**
 * Getter for the power supply type.
 ******************************************************************************/
uint8_t sl_power_supply_get_type(void)
{
  return supply_type;
}

/***************************************************************************//**
 * Checks if the current power supply has low power capability.
 ******************************************************************************/
bool sl_power_supply_is_low_power(void)
{
  return supply_type == SL_POWER_SUPPLY_TYPE_CR2032;
}

/***************************************************************************//**
 * Measure the supply voltage by averaging multiple readings.
 ******************************************************************************/
float sl_power_supply_measure_voltage(unsigned int avg)
{
  uint16_t adc_data;
  float voltage;
  unsigned int i;

  adc_init();

  voltage = 0;

  for ( i = 0; i < avg; i++ ) {
    adc_data = get_adc_sample();
    voltage += (float) adc_data * ADC_SCALE_FACTOR;
  }

  voltage = voltage / (float) avg;

  return voltage;
}

/***************************************************************************//**
 * Measure the battery level.
 ******************************************************************************/
uint8_t sl_power_supply_get_battery_level(void)
{
  float voltage = sl_power_supply_measure_voltage(1);
  uint8_t level = calculate_level(voltage, batt_model_cr2032, sizeof(batt_model_cr2032) / sizeof(batt_model_entry_t));

  return level;
}
