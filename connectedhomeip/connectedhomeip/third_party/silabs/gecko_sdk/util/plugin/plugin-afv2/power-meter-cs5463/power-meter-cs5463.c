// Copyright 2015 Silicon Laboratories, Inc.                                *80*
//
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#if defined(CORTEXM3_EFR32)
#include "spidrv.h"
#else
#include "hal/micro/cortexm3/spi-master.h"
#endif
#include "hal/hal.h"
#include "power-meter.h"
#include "power-meter-cs5463.h"
#include "power-meter-cs5463-config.h"

// default values for all offsets and gains
#define TEMPERATURE_OFFSET_DEFAULT                  0x00F405EF
#define VOLTAGE_AC_OFFSET_DEFAULT                   0x00000000
#define CURRENT_AC_OFFSET_DEFAULT                   0x00000000
#define CURRENT_DC_OFFSET_DEFAULT                   0x00FE3500
#define VOLTAGE_DC_OFFSET_DEFAULT                   0x0006F5AA
// 8000 cycles per measurement
#define DEFAULT_CYCLE_COUNT                         0x00001F40
// 16000 cycles per measurement
#define CALIBRATE_CYCLE_COUNT                       0x00003E80
#define CURRENT_UNIT_FACTOR                         0x4000
#define VOLTAGE_GAIN_DEFAULT                        0x003E147B
#define CURRENT_UNIT_GAIN                           0x00400000
#define VOLTAGE_UNIT_GAIN                           0x00400000
#define TEMPERATURE_GAIN_DEFAULT_VALUE              23

// anyting lower than 0.5W will be treated as no load.
#define NO_LOAD_POSITIVE_THRESHOLD                  500
#define NO_LOAD_NEGATIVE_THRESHOLD                  -500

// define the boundaries for calibration measurements
#define CURRENT_CALIBRATION_MAX_MA                  16000
#define CURRENT_CALIBRATION_MIN_MA                  100

// ------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginPowerMeterCs5463InitEventControl;
EmberEventControl emberAfPluginPowerMeterCs5463ReadEventControl;

// ------------------------------------------------------------------------------
// Plugin private funcitons declaration
static void cs5463Reset(void);
static void cs5463WriteReg(uint8_t                            page,
                           uint8_t                            cs5463Register,
                           uint8_t                            * writeBuffer);
static void cs5463WriteValue(uint8_t                              page,
                             uint8_t                              cs5463Register,
                             uint32_t                             value);
static void cs5463ReadReg(uint8_t                           page,
                          uint8_t                           cs5463Register,
                          uint8_t                           * readBuffer);
static int32_t cs5463GetSignedInt32FromRegister(uint8_t cs5463Register);
static void cs5463WriteCommand(uint8_t command);
static int16_t cs5463GetTemperature(bool isCalibrate);
static uint32_t cs5463GetVrmsMilliV(void);
static uint32_t cs5463GetCrmsMilliA(void);
static uint32_t cs5463GetApparentPowerMilliW(void);
static int32_t cs5463GetActivePowerMilliW(void);
static int8_t cs5463GetPowerFactor(void);
static void cs5463TransmitSpi(uint8_t * writeBuffer, uint8_t length);
static void cs5463ReceiveSpi(uint8_t * readBuffer, uint8_t length);

#if defined(CORTEXM3_EFR32)
static void cs5463InternalSpiMasterSelectSlave(void);
static void cs563InternalSpiMasterDeselectSlave(void);

#if defined(EMBER_AF_PLUGIN_EEPROM)
// extern eeprom functions to be used
extern bool emAfIsEepromInitialized(void);
extern void emberAfPluginEepromNoteInitializedState(bool state);
extern void emberAfPluginEepromShutdown();
#endif //defined(EMBER_AF_PLUGIN_EEPROM)
#endif //defined(CORTEXM3_EFR32)

// private globle variables
static uint8_t statusOverCurrent;
static uint8_t statusOverHeat;
static uint32_t rmsVoltage;
static uint32_t rmsCurrent;
static int32_t activePower;
static uint32_t apparentPower;
static int8_t powerFactor;
static int16_t onChipTemperature;
static uint16_t calibrationCurrentReference;
static uint32_t currentGain = CURRENT_UNIT_GAIN;
static bool isCalibrating;
#if defined(CORTEXM3_EFR32)
static SPIDRV_Init_t spiInitData = SPI_CONFIG;
static SPIDRV_HandleData_t spiHandler;
#endif
// ------------------------------------------------------------------------------
// Plugin private event handlers
void emberAfPluginPowerMeterCs5463InitEventHandler(void)
{
  halPowerMeterInit();
  emberEventControlSetInactive(emberAfPluginPowerMeterCs5463InitEventControl);
  emberEventControlSetDelayMS(emberAfPluginPowerMeterCs5463ReadEventControl,
                              CS5463_MEASUREMENT_DELAY_MS);
}

void emberAfPluginPowerMeterCs5463ReadEventHandler(void)
{
  uint16_t gainFactor;
#if defined(CORTEXM3_EFR32) && defined(EMBER_AF_PLUGIN_EEPROM)
  emberAfPluginEepromShutdown();
#endif //defined(CORTEXM3_EFR32) && defined(EMBER_AF_PLUGIN_EEPROM)
  rmsVoltage = cs5463GetVrmsMilliV();
  rmsCurrent = cs5463GetCrmsMilliA();
  activePower = cs5463GetActivePowerMilliW();
  apparentPower = cs5463GetApparentPowerMilliW();
  powerFactor = cs5463GetPowerFactor();
  onChipTemperature = cs5463GetTemperature(false);
  emberEventControlSetDelayMS(emberAfPluginPowerMeterCs5463ReadEventControl,
                              CS5463_READ_INTERVAL_MS);
  if (isCalibrating) {
    if ((rmsCurrent > CURRENT_CALIBRATION_MAX_MA)
        || (rmsCurrent < CURRENT_CALIBRATION_MIN_MA)) {
      gainFactor = CURRENT_UNIT_FACTOR;
    } else {
      gainFactor = (((uint32_t) calibrationCurrentReference
                     * CURRENT_UNIT_FACTOR)
                    / rmsCurrent);
    }
    halSetCurrentGain(gainFactor);
    isCalibrating = false;
    halPowerMeterCalibrationFinishedCallback(gainFactor);
  }
}

// ------------------------------------------------------------------------------
// Plugin defined callbacks
// The init callback, which will be called by the framework on init.
void emberAfPluginPowerMeterCs5463InitCallback(void)
{
  emberEventControlSetDelayMS(emberAfPluginPowerMeterCs5463InitEventControl,
                              CS5463_INIT_DELAY_MS);
}

void halPowerMeterInit(void)
{
  uint32_t config;

#if defined(CORTEXM3_EFR32)
  // Dessert PWR_SEN_RESET pin and delay 10ms
  GPIO_PinModeSet(PWR_SEN_RESET_PORT, PWR_SEN_RESET_PIN, gpioModePushPull, 1);
  GPIO_PinOutClear(PWR_SEN_RESET_PORT, PWR_SEN_RESET_PIN);
  halCommonDelayMilliseconds(10);
  GPIO_PinOutSet(PWR_SEN_RESET_PORT, PWR_SEN_RESET_PIN);
#else
  halSpiMasterInit(EMBER_SPI_PORT_BITBANG,
                   EMBER_SPI_CLK_50_KHZ,
                   EMBER_SPI_CLK_MODE_0,
                   false);

  // Dessert PWR_SEN_RESET pin and delay 10ms
  *((volatile uint32_t *)(GPIO_PxSET_BASE
                          + (GPIO_Px_OFFSET * (PWR_SEN_RESET / 8))))
    = BIT(PWR_SEN_RESET & 7);
#endif

  halCommonDelayMilliseconds(10);
  cs5463Reset();
  halCommonDelayMilliseconds(10);

  // all write Buffer start from index 1 as index 0 is reserved for
  // address byte, to be filled in the cs5463WriteReg()
  cs5463WriteValue(PAGE_1, CS5463_TEMP_OFFSET, TEMPERATURE_OFFSET_DEFAULT);
  cs5463WriteValue(PAGE_0, CS5463_VACOFF_REG, VOLTAGE_AC_OFFSET_DEFAULT);
  cs5463WriteValue(PAGE_0, CS5463_IACOFF_REG, CURRENT_AC_OFFSET_DEFAULT);
  cs5463WriteValue(PAGE_0,
                   CS5463_CURRENTDCOFFSET_REG,
                   CURRENT_DC_OFFSET_DEFAULT);
  cs5463WriteValue(PAGE_0,
                   CS5463_VOLTAGEDCOFFSET_REG,
                   VOLTAGE_DC_OFFSET_DEFAULT);
  cs5463WriteValue(PAGE_0, CS5463_CYCLECOUNT_REG, DEFAULT_CYCLE_COUNT);
  cs5463WriteValue(PAGE_0, CS5463_CURRENTGAIN_REG, currentGain);
  cs5463WriteValue(PAGE_0, CS5463_VOLTAGEGAIN_REG, VOLTAGE_GAIN_DEFAULT);

  config = ((PC) | (IGAIN) | (EWA) | (IMODE) | (IINV) | (ICPU) | (CDIV));

  statusOverHeat = CS5463_OVER_HEAT_TO_NORMAL;
  statusOverCurrent = CS5463_OVER_CURRENT_TO_NORMAL;
  isCalibrating = false;
  cs5463WriteValue(PAGE_0, CS5463_CONFIG_REG, config);
  halCommonDelayMilliseconds(20);
  cs5463WriteCommand(CS5463_CMD_START_CONV_CONTINUOUS); // start conversion
}

static void cs5463TransmitSpi(uint8_t * writeBuffer, uint8_t length)
{
#if defined(CORTEXM3_EFR32)
#if defined(EMBER_AF_PLUGIN_EEPROM)
  emberAfPluginEepromNoteInitializedState(false);
#endif //defined(EMBER_AF_PLUGIN_EEPROM)
  SPIDRV_Init(&spiHandler, &spiInitData);
  cs5463InternalSpiMasterSelectSlave();
  SPIDRV_MTransmitB(&spiHandler, writeBuffer, length);
  cs563InternalSpiMasterDeselectSlave();
  SPIDRV_DeInit(&spiHandler);
#else
  halSpiMasterTxBuf(EMBER_SPI_PORT_BITBANG,
                    sizeof(writeBuffer),
                    &writeBuffer[0],
                    EMBER_SPI_OP_NO_FLAGS,
                    NULL);
#endif
}

static void cs5463ReceiveSpi(uint8_t * readBuffer, uint8_t length)
{
#if defined(CORTEXM3_EFR32)
#if defined (EMBER_AF_PLUGIN_EEPROM)
  emberAfPluginEepromNoteInitializedState(false);
#endif //defined(EMBER_AF_PLUGIN_EEPROM)
  SPIDRV_Init(&spiHandler, &spiInitData);
  cs5463InternalSpiMasterSelectSlave();
  SPIDRV_MTransmitB(&spiHandler, &readBuffer[0], sizeof(readBuffer[0]));
  SPIDRV_MReceiveB(&spiHandler, &readBuffer[1], sizeof(readBuffer) - 1);
  cs563InternalSpiMasterDeselectSlave();
  SPIDRV_DeInit(&spiHandler);
#else
  halSpiMasterRxBuf(EMBER_SPI_PORT_BITBANG,
                    WRITE_BUFFER_SIZE,
                    readBuffer,
                    EMBER_SPI_OP_NO_FLAGS,
                    NULL);
#endif
}

#if defined(CORTEXM3_EFR32)
static void cs5463InternalSpiMasterSelectSlave(void)
{
  GPIO_PinModeSet(SPI_NSEL_PORT, SPI_NSEL_BIT, gpioModePushPull, 1);
  GPIO_PinOutClear(SPI_NSEL_PORT, SPI_NSEL_BIT);
}

static void cs563InternalSpiMasterDeselectSlave(void)
{
  GPIO_PinModeSet(SPI_NSEL_PORT, SPI_NSEL_BIT, gpioModePushPull, 1);
  GPIO_PinOutSet(SPI_NSEL_PORT, SPI_NSEL_BIT);
}

#endif

static void cs5463Reset(void)
{
  uint8_t resetSequence[4];

  resetSequence[0] = CS5463_CMD_SYNC1;
  resetSequence[1] = CS5463_CMD_SYNC1;
  resetSequence[2] = CS5463_CMD_SYNC1;
  resetSequence[3] = CS5463_CMD_SYNC0;
  cs5463TransmitSpi(resetSequence, sizeof(resetSequence));
}

static void cs5463WriteReg(uint8_t                            page,
                           uint8_t                            cs5463Register,
                           uint8_t                            * writeBuffer)
{
  uint8_t buffer[WRITE_BUFFER_SIZE];

  buffer[0] = (CS5463_PAGE_NUM << CS5463_REG_ADDR_BIT)
              | (CS5463_WRITE << CS5463_WR_BIT);
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = page;
  cs5463TransmitSpi(buffer, sizeof(buffer));
  writeBuffer[0] = (cs5463Register << CS5463_REG_ADDR_BIT)
                   | (CS5463_WRITE << CS5463_WR_BIT);
  cs5463TransmitSpi(writeBuffer, sizeof(writeBuffer));
}

static void cs5463WriteValue(uint8_t  page,
                             uint8_t  cs5463Register,
                             uint32_t value)
{
  uint8_t writeBuffer[WRITE_BUFFER_SIZE];

  writeBuffer[1] = BYTE_2(value);
  writeBuffer[2] = BYTE_1(value);
  writeBuffer[3] = BYTE_0(value);
  cs5463WriteReg(page, cs5463Register, writeBuffer);
}

static void cs5463ReadReg(uint8_t page,
                          uint8_t cs5463Register,
                          uint8_t * readBuffer)
{
  uint8_t buffer[WRITE_BUFFER_SIZE];

  buffer[0] = (CS5463_PAGE_NUM << CS5463_REG_ADDR_BIT)
              | (CS5463_WRITE << CS5463_WR_BIT);
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = page;
  cs5463TransmitSpi(buffer, sizeof(buffer));
  readBuffer[0] = (cs5463Register << CS5463_REG_ADDR_BIT)
                  | (CS5463_READ << CS5463_WR_BIT);
  readBuffer[1] = 0xFF;
  readBuffer[2] = 0xFF;
  readBuffer[3] = 0xFF;
  cs5463ReceiveSpi(readBuffer, sizeof(readBuffer));
}

static void cs5463WriteCommand(uint8_t command)
{
  uint8_t writeByte;
  writeByte = command;

  cs5463TransmitSpi(&writeByte, sizeof(writeByte));
}

static int16_t cs5463GetTemperature(bool isCalibrate)
{
  int32_t temperature;

  temperature = cs5463GetSignedInt32FromRegister(CS5463_TEMPERATURE_REG);

  // the adc value is represented in two's complement notation and in the
  // range of -128.0 and 128.0, with the binary point to the right of the
  // eighth MSB. To take it as an integer, it actually equals to a value
  // mulitplied by 2 ^ 16 (65536)

  // to get a centiC temperature, it should be divided by 655.36(=*50/32768)
  temperature = (temperature * 50) >> 15;
  // we will not trigger overheat if it's calibrating.
  if (isCalibrate) {
    return (int16_t)temperature;
  } else {
    if ((statusOverHeat == CS5463_NORMAL_TO_OVER_HEAT)
        && (temperature < CS5463_OVER_HEAT_THRESHOLD)) {
      statusOverHeat = CS5463_OVER_HEAT_TO_NORMAL;
      halPowerMeterOverHeatStatusChangeCallback(statusOverHeat);
    } else if ((statusOverHeat == CS5463_OVER_HEAT_TO_NORMAL)
               && (temperature > CS5463_OVER_HEAT_THRESHOLD)) {
      statusOverHeat = CS5463_NORMAL_TO_OVER_HEAT;
      halPowerMeterOverHeatStatusChangeCallback(statusOverHeat);
    } else if ((statusOverHeat == CS5463_NORMAL_TO_OVER_HEAT)
               && (temperature > CS5463_OVER_HEAT_THRESHOLD)) {
      halPowerMeterOverHeatStatusChangeCallback(statusOverHeat);
    }
  }
  return (int16_t)temperature;
}

static int32_t cs5463GetSignedInt32FromRegister(uint8_t cs5463Register)
{
  int32_t registerValue;
  uint8_t readBuffer[WRITE_BUFFER_SIZE];

  cs5463ReadReg(PAGE_0, cs5463Register, readBuffer);
  registerValue = INT8U_TO_INT32U(0,
                                  readBuffer[1],
                                  readBuffer[2],
                                  readBuffer[3]);
  if (registerValue & BIT32(ADC_REGISTER_SIGN_BIT)) {
    registerValue = CONVERT_NEG_24_BIT_TO_NEG_32_BIT(registerValue);
  }
  return registerValue;
}

uint32_t halGetVrmsMilliV(void)
{
  return rmsVoltage;
}

uint32_t halGetCrmsMilliA(void)
{
  return rmsCurrent;
}

uint32_t halGetApparentPowerMilliW(void)
{
  return apparentPower;
}

int32_t halGetActivePowerMilliW(void)
{
  return activePower;
}

int8_t halGetPowerFactor(void)
{
  return powerFactor;
}

static uint32_t cs5463GetVrmsMilliV(void)
{
  uint32_t vrms;
  uint8_t readBuffer[WRITE_BUFFER_SIZE];

  cs5463ReadReg(PAGE_0, CS5463_RMSVOLTAGE_REG, readBuffer);
  vrms = INT8U_TO_INT32U(0, readBuffer[1], readBuffer[2], readBuffer[3]);

  // the 24 bit adc value from the chip is actually the fraction of the full
  // dynamic range, the actual full dynamic range voltage is fixed by hardware
  // design, and it's defined as VOLTAGE_SCALE. so the formular will be:
  //
  // vrms(in milliVolt) = adc * VOLTAGE_SCALE * 1000 / 2^24
  //
  // if we multiply everything first, we might overrun the 32 bit value
  // but if we do all the bit shift first, we will lost to much accuracy
  // so we do the this step by step.
  //
  // the last 5 bits are insignificant to milliV, and good enough for us to do
  // a mulitplication without overrun.
  vrms = vrms >> 5;
  vrms = vrms * VOLTAGE_SCALE;

  // 10 more bit shift to allow us for mulitplication by 1000
  vrms = vrms >> 10;
  vrms = vrms * 1000;

  // shift the rest 9 bits
  vrms = vrms >> 9;
  return vrms;
}

static uint32_t cs5463GetCrmsMilliA(void)
{
  uint32_t crms;
  uint8_t readBuffer[WRITE_BUFFER_SIZE];

  cs5463ReadReg(PAGE_0, CS5463_RMSCURRENT_REG, readBuffer);
  crms = INT8U_TO_INT32U(0, readBuffer[1], readBuffer[2], readBuffer[3]);
  // the 24 bit adc value from the chip is actually the fraction of the full
  // dynamic range, the actual full dynamic range voltage is fixed by hardware
  // design, and it's defined as CURRENT_SCALE. so the formular will be:
  //
  // crms(in milliAmp) = adc * CURRENT_SCALE * 1000 / 2^24
  //
  // if we multiply everything first, we might overrun the 32 bit value
  // but if we do all the bit shift first, we will lost to much accuracy
  // so we do the this step by step.
  //
  // CURRENT SCALE is less than 100, we will not overrun here
  crms = crms * CURRENT_SCALE;

  // 10 more bit shift to allow us for mulitplication by 1000
  crms = crms >> 10;
  crms = crms * 1000;

  // shift the rest 14 bits
  crms = crms >> 14;

  if ((activePower < NO_LOAD_POSITIVE_THRESHOLD)
      && (activePower > NO_LOAD_NEGATIVE_THRESHOLD)) {
    crms = 0;
  }

  if ((statusOverCurrent == CS5463_NORMAL_TO_OVER_CURRENT)
      && (crms < CS5463_OVER_CURRENT_THRESHOLD)) {
    statusOverCurrent = CS5463_OVER_CURRENT_TO_NORMAL;
    halPowerMeterOverCurrentStatusChangeCallback(statusOverCurrent);
  } else if ((statusOverCurrent == CS5463_OVER_CURRENT_TO_NORMAL)
             && (crms > CS5463_OVER_CURRENT_THRESHOLD)) {
    statusOverCurrent = CS5463_NORMAL_TO_OVER_CURRENT;
    halPowerMeterOverCurrentStatusChangeCallback(statusOverCurrent);
  } else if ((statusOverCurrent == CS5463_NORMAL_TO_OVER_CURRENT)
             && (crms > CS5463_OVER_CURRENT_THRESHOLD)) {
    halPowerMeterOverCurrentStatusChangeCallback(statusOverCurrent);
  }
  return crms;
}

static uint32_t cs5463GetApparentPowerMilliW(void)
{
  uint32_t apparentPower;
  uint8_t readBuffer[WRITE_BUFFER_SIZE];

  cs5463ReadReg(PAGE_0, CS5463_APPARENTPOWER_REG, readBuffer);
  apparentPower = INT8U_TO_INT32U(0,
                                  readBuffer[1],
                                  readBuffer[2],
                                  readBuffer[3]);
  // apparent power register is in 23 bits, msb is always 0
  //
  // formula for power :
  // power(in milliWatt) = adc * CURRENT_SCALE * VOLTAGE_SCALE * 1000 / 2^23
  //
  // We will overrun the 32 bit signed value if we directly mulitply the adc
  // with CURRENT_SCALE and VOTAGE_SCALE, so we shift 9 bits(/512) before
  // VOLTAGE_SCALE(299) multplication
  apparentPower = apparentPower * CURRENT_SCALE;
  apparentPower = apparentPower >> 9;
  apparentPower = apparentPower * VOLTAGE_SCALE;

  // 10 more bit shift to allow us for mulitplication by 1000
  apparentPower = apparentPower >> 10;
  apparentPower = apparentPower * 1000;
  // shift the rest 4 bits
  apparentPower = apparentPower >> 4;
  return apparentPower;
}

static int32_t cs5463GetActivePowerMilliW(void)
{
  int32_t power;

  power = cs5463GetSignedInt32FromRegister(CS5463_ACTIVEPOWER_REG);

  // active power register is in 23 bits, msb is sign bit
  //
  // formula for power :
  // power(in milliWatt) = adc * CURRENT_SCALE * VOLTAGE_SCALE * 1000 / 2^23
  //
  // We will overrun the 32 bit signed value if we directly mulitply the adc
  // with CURRENT_SCALE and VOTAGE_SCALE, so we shift 9 bits(/512) before
  // VOLTAGE_SCALE(299) multplication
  power = power * CURRENT_SCALE;
  power = power >> 9;
  power = power * VOLTAGE_SCALE;
  // 10 more bit shift to allow us for mulitplication by 1000
  power = power >> 10;
  power = power * 1000;
  // shift the rest 4 bits
  power = power >> 4;

  if ((power < NO_LOAD_POSITIVE_THRESHOLD)
      && (power > NO_LOAD_NEGATIVE_THRESHOLD)) {
    power = 0;
  }
  return power;
}

static int8_t cs5463GetPowerFactor(void)
{
  int32_t powerFactor;

  powerFactor = cs5463GetSignedInt32FromRegister(CS5463_POWERFACTOR_REG);

  // power factor is a 24 bit signed fraction between -1 to 1, it's full
  // dynamic range is represented in 23 bit with a sign bit. it's actually
  // equals to 1 x 2 ^ 23 or -1 x 2 ^ 23. we will need to return the power
  // factor as integer between -100 to 100, we will check the sign bit and
  // make the local 32bit powerFactor a negatve number, mulitply it by 100
  // and then shift it by 23 bits
  // no need to worry overrun in this case.
  powerFactor = (100 * powerFactor) >> 23;
  return (int8_t)powerFactor;
}

bool halPowerMeterCalibrateCurrentGain(uint16_t referenceCurrentMa)
{
  if ((referenceCurrentMa > CURRENT_CALIBRATION_MAX_MA)
      || (referenceCurrentMa < CURRENT_CALIBRATION_MIN_MA)) {
    return false;
  }
  emberEventControlSetInactive(emberAfPluginPowerMeterCs5463ReadEventControl);
  halSetCurrentGain(CURRENT_UNIT_FACTOR);
  halPowerMeterInit();
  calibrationCurrentReference = referenceCurrentMa;
  isCalibrating = true;
  emberEventControlSetDelayMS(emberAfPluginPowerMeterCs5463ReadEventControl,
                              CS5463_CALIBRATE_INTERVAL_MS);
  return true;
}

int16_t halGetPowerMeterTempCentiC(void)
{
  return onChipTemperature;
}

uint8_t halGetPowerMeterStatus(void)
{
  uint32_t status;
  uint8_t returnStatus = 0;
  uint8_t readBuffer[WRITE_BUFFER_SIZE];

  cs5463ReadReg(PAGE_0, CS5463_STATUS_REG, readBuffer);
  status = (int32_t)INT8U_TO_INT32U(0,
                                    readBuffer[1],
                                    readBuffer[2],
                                    readBuffer[3]);
  if ((status & CS5463_ERRORBIT_VSAG)
      || (status & CS5463_ERRORBIT_EOR)
      || (status & CS5463_ERRORBIT_VOR)
      || (status & CS5463_ERRORBIT_VROR)) {
    returnStatus = returnStatus
                   | HAL_POWER_METER_POWER_QUALITY_ELECTRICITY_FAULT;
  }
  return returnStatus;
}

void halSetCurrentGain(uint16_t factor)
{
  uint32_t gain;

  if (factor == HAL_POWER_METER_CURRENT_GAIN_DEFAULT) {
    gain = CURRENT_UNIT_GAIN;
  } else {
    gain = (uint32_t)factor * (CURRENT_UNIT_GAIN / CURRENT_UNIT_FACTOR);
  }
  currentGain = gain;
  cs5463WriteValue(PAGE_0, CS5463_CURRENTGAIN_REG, gain);
}
