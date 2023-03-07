/*
 * \section License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holder nor the names of the
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 */

 /** ============================================================================
 *  @file       hdc2010.h
 *
 *  @brief      HDC2010 driver
 *
 *  The HDC2010 header file should be included in an application as
 *  follows:
 *  @code
 *  #include <ti/common/sail/hdc2010/hdc2010.h>
 *  @endcode
 *
 *  # Operation #
 *  The HDC2010 driver simplifies using a HDC2010 sensor to perform temperature
 *  readings. The board's I2C peripheral and pins must be configured and then
 *  initialized by using I2C_init(). Similarly, any GPIO pins must be
 *  configured and then initialized by using GPIO_init(). A HDC2010_config
 *  array should be defined by the application. The HDC2010_config array should
 *  contain pointers to a defined HDC2010_HWAttrs and allocated array for the
 *  HDC2010_Object structures. HDC2010_init() must be called prior to using
 *  HDC2010_open().
 *
 *  The APIs in this driver serve as an interface to a DPL(Driver Porting Layer)
 *  The specific implementations are responsible for creating all the RTOS
 *  specific primitives to allow for thread-safe operation.
 *
 *  For accurate operation, calibration may be necessary. Refer to SBOU142.pdf
 *  for calibration instructions.(this is valid even for HDC2010)
 *
 *  The HDC2010 out of the box does provide object temperature API.
 *  To the calculate the object temperature the algorithm specified in the 
 *  below link is used.But the API may need calibration and has not been
 *  tested for all scenarios/temperatures
 *  http://processors.wiki.ti.com/index.php/SensorTag_User_Guide
 *  http://www.ti.com/lit/ug/sbou107/sbou107.pdf
 *
 *  This driver has no dynamic memory allocation.
 *
 *  ## Defining HDC2010_Config, HDC2010_Object and HDC2010_HWAttrs #
 *  Each structure must be defined by the application. The following
 *  example is for a MSP432 in which two HDC2010 sensors are setup.
 *  The following declarations are placed in "MSP_EXP432P401R.h"
 *  and "MSP_EXP432P401R.c" respectively. How the gpioIndices are defined
 *  are detailed in the next example.
 *
 *  "MSP_EXP432P401R.h"
 *  @code
 *  typedef enum MSP_EXP432P491RLP_HDC2010Name {
 *      HDC2010_ROOMTEMP = 0, // Sensor measuring room temperature
 *      HDC2010_OUTDOORTEMP,  // Sensor measuring outside temperature
 *      MSP_EXP432P491RLP_HDC2010COUNT
 *  } MSP_EXP432P491RLP_HDC2010Name;
 *
 *  @endcode
 *
 *  "MSP_EXP432P401R.c"
 *  @code
 *  #include <ti/common/sail/hdc2010/hdc2010.h>
 *
 *  HDC2010_Object HDC2010_object[MSP_EXP432P491RLP_HDC2010COUNT];
 *
 *  const HDC2010_HWAttrs HDC2010_hwAttrs[MSP_EXP432P491RLP_HDC2010COUNT] = {
 *  {
 *      .slaveAddress = HDC2010_SA1, // 0x40
 *      .gpioIndex = MSP_EXP432P401R_HDC2010_0,
 *  },
 *  {
 *      .slaveAddress = HDC2010_SA2, // 0x41
 *      .gpioIndex = MSP_EXP432P401R_HDC2010_1,
 *  },
 *  };
 *
 *  const HDC2010_Config HDC2010_config[] = {
 *  {
 *      .hwAttrs = (void *)&HDC2010_hwAttrs[0],
 *      .objects = (void *)&HDC2010_object[0],
 *  },
 *  {
 *      .hwAttrs = (void *)&HDC2010_hwAttrs[1],
 *      .objects = (void *)&HDC2010_object[1],
 *  },
 *  {NULL, NULL},
 *  };
 *  @endcode
 *
 *  ##Setting up GPIO configurations #
 *  The following example is for a MSP432 in which two HDC2010 sensors
 *  each need a GPIO pin for alarming. The following definitions are in
 *  "MSP_EXP432P401R.h" and "MSP_EXP432P401R.c" respectively. This
 *  example uses GPIO pins 1.5 and 4.3. The GPIO_CallbackFxn table must
 *  contain as many indices as GPIO_CallbackFxns that will be specified by
 *  the application. For each data ready pin used, an index should be allocated
 *  as NULL.
 *
 *  "MSP_EXP432P401R.h"
 *  @code
 *  typedef enum MSP_EXP432P401R_GPIOName {
 *      MSP_EXP432P401R_HDC2010_0, // ALERT pin for the room temperature
 *      MSP_EXP432P401R_HDC2010_1, // ALERT pin for the outdoor temperature
 *      MSP_EXP432P401R_GPIOCOUNT
 *  } MSP_EXP432P401R_GPIOName;
 *  @endcode
 *
 *  "MSP_EXP432P401R.c"
 *  @code
 *  #include <gpio.h>
 *
 *  GPIO_PinConfig gpioPinConfigs[] = {
 *      GPIOMSP432_P1_5 | GPIO_CFG_INPUT | GPIO_CFG_IN_INT_FALLING,
 *      GPIOMSP432_P4_3 | GPIO_CFG_INPUT | GPIO_CFG_IN_INT_FALLING
 *  };
 *
 *  GPIO_CallbackFxn gpioCallbackFunctions[] = {
 *      NULL,
 *      NULL
 *  };
 *  @endcode
 *
 *  ## Opening a I2C Handle #
 *  The I2C controller must be in blocking mode. This will seamlessly allow
 *  for multiple I2C endpoints without conflicting callback functions. The
 *  I2C_open() call requires a configured index. This index must be defined
 *  by the application in accordance to the I2C driver. The default transfer
 *  mode for an I2C_Params structure is I2C_MODE_BLOCKING. In the example
 *  below, the transfer mode is set explicitly for clarity. Additionally,
 *  the HDC2010 hardware is capable of communicating at 400kHz. The default
 *  bit rate for an I2C_Params structure is I2C_100kHz.
 *
 *  @code
 *  #include <ti/drivers/I2C.h>
 *
 *  I2C_Handle      i2cHandle;
 *  I2C_Params      i2cParams;
 *
 *  I2C_Params_init(&i2cParams);
 *  i2cParams.transferMode = I2C_MODE_BLOCKING;
 *  i2cParams.bitRate = I2C_400kHz;
 *  i2cHandle = I2C_open(someI2C_configIndexValue, &i2cParams);
 *  @endcode
 *
 *  ## Opening a HDC2010 sensor with default parameters #
 *  The HDC2010_open() call must be made in a task context.
 *
 *  @code
 *  #include <hdc2010.h>
 *
 *  HDC2010_Handle hdc2010Handle;
 *
 *  HDC2010_init();
 *  hdc2010Handle = HDC2010_open(HDC2010_ROOMTEMP, i2cHandle, NULL);
 *  @endcode
 *
 *  ## Opening a HDC2010 sensor to Data Ready #
 *  In the following example, a callback function is specified in the
 *  hdc2010Params structure. This indicates to the module that
 *  the data ready pin will be used. Additionally, a user specific argument
 *  is passed in. The sensor will assert the data ready pin whenever the
 *  conversion is complete and data is ready. No data ready will be generated 
 *  until HDC2010_enableDataReady() is called.
 *
 *  @code
 *  #include <hdc2010.h>
 *
 *  HDC2010_Handle hdc2010Handle;
 *  HDC2010_Params hdc2010Params;
 *
 *  hdc2010Params.callback = gpioCallbackFxn;
 *  hdc2010Handle = HDC2010_open(HDC2010_ROOMTEMP, i2cHandle, &hdc2010Params);
 *  HDC2010_enableDataReady(hdc2010Handle);
 *  @endcode
 *  ============================================================================
 */

#ifndef HDC2010_H_
#define HDC2010_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Driver Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>

/***************************************************************/
/**\name    GET AND SET BITSLICE FUNCTIONS       */
/***************************************************************/
#define HDC2010_GET_BITSLICE(regvar, bitname)\
        ((regvar & bitname##__MSK) >> bitname##__POS)

#define HDC2010_SET_BITSLICE(regvar, bitname, val)\
        ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

/****************************************************/
/**\name    DEFINITIONS FOR ARRAY SIZE OF DATA   */
/***************************************************/
#define HDC2010_HUMIDITY_DATA_SIZE                (2U)
#define HDC2010_TEMPERATURE_DATA_SIZE             (2U)

#define HDC2010_TEMPERATURE_LSB_DATA              (0U)
#define HDC2010_TEMPERATURE_MSB_DATA              (1U)

#define HDC2010_HUMIDITY_LSB_DATA                 (0U)
#define HDC2010_HUMIDITY_MSB_DATA                 (1U)

/****************************************************/
/**\name    Data Rate Definitions  */
/***************************************************/
#define HDC2010_DATA_RATE__REG                    (HDC2010_RST_DRDY_INT_CONF_REG)
#define HDC2010_DATA_RATE__MSK                    (0x70U)
#define HDC2010_DATA_RATE__POS                    (4U)

/****************************************************/
/**\name    Resolution Definitions  */
/***************************************************/
#define HDC2010_TEMPERATURE_RES__REG              (HDC2010_MEAS_CONFIG_REG)
#define HDC2010_TEMPERATURE_RES__MSK              (0xC0U)
#define HDC2010_TEMPERATURE_RES__POS              (6U)

#define HDC2010_HUMIDITY_RES__REG                 (HDC2010_MEAS_CONFIG_REG)
#define HDC2010_HUMIDITY_RES__MSK                 (0x30U)
#define HDC2010_HUMIDITY_RES__POS                 (4U)

/****************************************************/
/**\name    Configuration Definitions  */
/***************************************************/
#define HDC2010_MEASURE_CONF__REG                 (HDC2010_MEAS_CONFIG_REG)
#define HDC2010_MEASURE_CONF__MSK                 (0x06U)
#define HDC2010_MEASURE_CONF__POS                 (1U)

/****************************************************/
/**\name    Measurement Definitions  */
/***************************************************/
#define HDC2010_MEASURE_TRIG__REG                 (HDC2010_MEAS_CONFIG_REG)
#define HDC2010_MEASURE_TRIG__MSK                 (0x01U)
#define HDC2010_MEASURE_TRIG__POS                 (0U)

#define HDC2010_START_BIT                         (0x01U)
#define HDC2010_STOP_BIT                          (0x00U)

/****************************************************/
/**\name    Interrupt Definitions  */
/***************************************************/
#define HDC2010_DRD_MASK__REG                     (HDC2010_INT_MASK_REG)
#define HDC2010_DRD_MASK__MSK                     (0x80U)
#define HDC2010_DRD_MASK__POS                     (7U)

#define HDC2010_TH_MASK__REG                      (HDC2010_INT_MASK_REG)
#define HDC2010_TH_MASK__MSK                      (0x40U)
#define HDC2010_TH_MASK__POS                      (6U)

#define HDC2010_TL_MASK__REG                      (HDC2010_INT_MASK_REG)
#define HDC2010_TL_MASK__MSK                      (0x20U)
#define HDC2010_TL_MASK__POS                      (5U)

#define HDC2010_HH_MASK__REG                      (HDC2010_INT_MASK_REG)
#define HDC2010_HH_MASK__MSK                      (0x10U)
#define HDC2010_HH_MASK__POS                      (4U)

#define HDC2010_HL_MASK__REG                      (HDC2010_INT_MASK_REG)
#define HDC2010_HL_MASK__MSK                      (0x08U)
#define HDC2010_HL_MASK__POS                      (3U)

#define HDC2010_INT_MASK__POS                     (3U)

#define HDC2010_INT_EN__REG                       (HDC2010_RST_DRDY_INT_CONF_REG)
#define HDC2010_INT_EN__MSK                       (0x04U)
#define HDC2010_INT_EN__POS                       (2U)

#define HDC2010_INT_POL__REG                      (HDC2010_RST_DRDY_INT_CONF_REG)
#define HDC2010_INT_POL__MSK                      (0x02U)
#define HDC2010_INT_POL__POS                      (1U)

#define HDC2010_INT_MODE__REG                     (HDC2010_RST_DRDY_INT_CONF_REG)
#define HDC2010_INT_MODE__MSK                     (0x01U)
#define HDC2010_INT_MODE__POS                     (0U)

#define HDC2010_INT__POS                          (0U)

/****************************************************/
/**\name    DEV ID DEFINITIONS  */
/***************************************************/
#define HDC2010_DEV_ID_L                          (0xD0U)
#define HDC2010_DEV_ID_H                          (0x07U)

/****************************************************/
/**\name    MANUFACTURER ID DEFINITIONS  */
/***************************************************/
#define HDC2010_MANUFACTURER_ID                   (0x5449U)

/****************************************************/
/**\name    REGISTER ADDRESS DEFINITIONS  */
/***************************************************/
#define HDC2010_TEMPERATURE_LSB_REG              (0x00U)  /*Temperature MSB Reg */
#define HDC2010_TEMPERATURE_MSB_REG              (0x01U)  /*Temperature LSB Reg */
#define HDC2010_HUMIDITY_LSB_REG                 (0x02U)  /*Humidity MSB Reg */
#define HDC2010_HUMIDITY_MSB_REG                 (0x03U)  /*Humidity LSB Reg */

#define HDC2010_DRDY_INT_STATUS_REG              (0x04U)  /*DataReady and Interrupt Status */
#define HDC2010_MAX_TEMPERATURE_REG              (0x05U)  /*Max Temperature Value measured(Peak Detector) */
#define HDC2010_MAX_HUMIDITY_REG                 (0x06U)  /*Max Humidity Value measured(Peak Detector) */
#define HDC2010_INT_MASK_REG                     (0x07U)  /*Interrupt Mask Register */

#define HDC2010_TEMPERATURE_OFF_REG              (0x08U)  /*Temperature Offset Adjustment*/
#define HDC2010_HUMIDITY_OFF_REG                 (0x09U)  /*Humidity Offset Adjustment*/

#define HDC2010_TEMPERATURE_THR_L_REG            (0x0AU)  /*Temperature Threshold Low*/
#define HDC2010_TEMPERATURE_THR_H_REG            (0x0BU)  /*Temperature Threshold High */
#define HDC2010_HUMIDITY_THR_L_REG               (0x0CU)  /*Humidity Threshold Low */
#define HDC2010_HUMIDITY_THR_H_REG               (0x0DU)  /*Humidity Threshold High */

#define HDC2010_RST_DRDY_INT_CONF_REG            (0x0EU)  /*Soft Reset and Interrupt Configuration */
#define HDC2010_MEAS_CONFIG_REG                  (0x0FU)  /*Measurement Configuration Register */

#define HDC2010_MANUFACTURER_ID_L_REG            (0xFCU)  /*Manufacturer ID LSB Register */
#define HDC2010_MANUFACTURER_ID_H_REG            (0xFDU)  /*Manufacturer ID MSB Registerr */
#define HDC2010_DEV_ID_L_REG                     (0xFEU)  /*Device ID LSB Register */
#define HDC2010_DEV_ID_H_REG                     (0xFFU)  /*Device ID MSB Register */

#define HDC2010_TEMP                             (0x00U)
/*!
 *  @brief    HDC2010 I2C slave addresses.
 *
 *  The HDC2010 Slave Address is determined by the input to the ADR0 and ADR1
 *  input pins of the HDC2010 hardware. A '1' indicates a supply voltage of
 *  up to 7.5 V while '0' indicates ground. In some cases, the ADR0 pin may
 *  be coupled with the SDA or SCL bus to achieve a particular slave address.
 *  HDC2010 sensors on the same I2C bus cannot share the same slave address.
 */
typedef enum HDC2010_SlaveAddress {
    HDC2010_SA1 = 0x40U,                            /*!< ADR0 = 0   */
    HDC2010_SA2 = 0x41U                             /*!< ADR0 = 1   */
} HDC2010_SlaveAddress;

/*!
 *  @brief    HDC2010 temperature settings
 *
 *  This enumeration defines the scaling for reading and writing temperature
 *  values with a HDC2010 sensor.
 */
typedef enum HDC2010_TempScale {
    HDC2010_CELSIUS = 0U,
    HDC2010_KELVIN  = 1U,
    HDC2010_FAHREN  = 2U
} HDC2010_TempScale;

/**************************************************************/
/**\name    STRUCTURE DEFINITIONS                         */
/**************************************************************/
/*!
 *  @brief    A handle that is returned from a HDC2010_open() call.
 */
typedef struct HDC2010_Config    *HDC2010_Handle;

/*!
 *  @brief    HDC2010 Measurement Mode
 *
 *  This enumeration defines the Measurement Mode of the Device
 *  The MEAS_CONF[1:0]  bits control in what mode does the HDC2010 operate in
 *
 *  Measurement configuration:
 *  00: Humidity + Temperature
 *  01: Temperature only
 *  10: Humidity Only
 *  11: NA
 *
 */
typedef enum  HDC2010_MeasurementMode {
    HDC2010_HT_MODE  = 0x00U,                       /* Humidity and Temperature Mode */
    HDC2010_T_MODE   = 0x02U,                       /* Temperature Only Mode */
    HDC2010_H_MODE   = 0x04U                        /* Humidity Only Mode */
}  HDC2010_MeasurementMode;

/*!
 *  @brief    HDC2010 Temperature Resolution
 *
 *  This enumeration defines the Temperature Resolution of the Device
 *  The TRES[1:0] bits control in what resolution does the HDC2010 operate in
 *
 *  Temperature resolution
 *  00: 14 bit
 *  01: 11 bit
 *  10: 9 bit
 *  11: NA (TBC)
 *
 */
typedef enum HDC2010_TemperatureResolution {
    HDC2010_T14_BITS  = 0x00U,                      /* Temperature 14 Bit Resolution */
    HDC2010_T11_BITS  = 0x40U,                      /* Temperature 14 Bit Resolution */
    HDC2010_T09_BITS  = 0x80U                       /* Temperature 14 Bit Resolution */
} HDC2010_TemperatureResolution;

/*!
 *  @brief    HDC2010 Humidity Resolution
 *
 *  This enumeration defines the Humidity Resolution of the Device
 *  The HRES[1:0] bits control in what resolution does the HDC2010 operate in
 *
 *  Humidity resolution
 *  00: 14 bit
 *  01: 11 bit
 *  10: 9 bit
 *  11: NA (TBC)
 *
 */
typedef enum HDC2010_HumidityResolution {
    HDC2010_H14_BITS  = 0x00U,                      /* Humidity 14 Bit Resolution */
    HDC2010_H11_BITS  = 0x10U,                      /* Humidity 11 Bit Resolution */
    HDC2010_H09_BITS  = 0x20U                       /* Humidity 9 Bit Resolution  */
} HDC2010_HumidityResolution;

/*!
 *  @brief    HDC2010 Output Data Rate
 *
 *  This enumeration defines the Output Data Ratee of the Device
 *  The ODR[2:0]  bits control in what Output Data Rate does the HDC2010 operate in
 *
 *  Output Data Rate
 *  000 = No repeated measurements. Trigger on demand
 *  001 = 1/120Hz (1 samples every 2 minutes)
 *  010 = 1/60Hz (1 samples every minute)
 *  011 = 0.1Hz (1 samples every 10 seconds)
 *  100 = 0.2 Hz (1 samples every 5 second)
 *  101 = 1Hz (1 samples every second)
 *  110 = 2Hz (2 samples every second)
 *  111 = 5Hz (5 samples every second)
 *
 */
typedef enum HDC2010_OutputDataRate {
    HDC2010_TRIGG_ON_DEMAND = 0x00U,
    HDC2010_TRIGG_1_120_HZ  = 0x10U,                /*1/120 Hz (1 samples every 2 min)*/
    HDC2010_TRIGG_1_60_HZ   = 0x20U,                /*1/60 Hz (1 samples every 1 min)*/
    HDC2010_TRIGG_0p1_HZ    = 0x30U,                /*0.1 Hz (1 samples every 10 sec)*/
    HDC2010_TRIGG_0p2_HZ    = 0x40U,                /*0.2 Hz (1 samples every 5 sec)*/
    HDC2010_TRIGG_1_HZ      = 0x50U,                /*1 Hz (1 samples every 1 sec)*/
    HDC2010_TRIGG_2_HZ      = 0x60U,                /*2 HZ (2 samples every 1 sec)*/
    HDC2010_TRIGG_5_HZ      = 0x70U                 /*5 HZ (3 samples every 1 sec)*/
} HDC2010_OutputDataRate;

/*!
 *  @brief    HDC2010 Interrupt En
 *
 *  This enumeration defines the Interrupt En of the Device
 *  The DRDY/INT_EN bit control in what mode does the HDC2010 operate in
 *
 *  DRDY/INT_EN pin configuration
 *  0 = High Z
 *  1 = Enable
 *
 */
typedef enum HDC2010_InterruptEn {
    HDC2010_HI_Z_MODE    = 0x00U,                  /* High Z Mode */
    HDC2010_ENABLE_MODE  = 0x04U                   /* Enable Interrupt */
} HDC2010_InterruptEn;

/*!
 *  @brief    HDC2010 Interrupt Mask
 *
 *  This enumeration defines the Interrupt Mask of the Device
 *  The Interrupt Configuration Register control in what mode does the HDC2010 operate in
 *
 *  DataReady Interrupt mask
 *  0 = DataReady Interrupt generator disable
 *  1 = DaReady Interrupt generator enable
 *
 *  Temperature threshold HIGH Interrupt mask
 *  0 = Temperature high Interrupt generator enable
 *  1 = Temperature high Interrupt generator disable
 *
 *  Temperature threshold LOW Interrupt mask
 *  0 = Temperature low Interrupt generator enable
 *  1 = Temperature low Interrupt generator disable
 *
 *  Humidity threshold HIGH Interrupt mask
 *  0 = Humidity high Interrupt generator enable
 *  1 = Humidity high Interrupt generator disable
 *
 *  Humidity threshold LOW Interrupt mask
 *  0 = Humidity low Interrupt generator enable
 *  1 = Humidity Low Interrupt generator disable
 *
 */
typedef enum HDC2010_InterruptMask {
    HDC2010_DRDY_MASK = 0x80U,                  /* Data Ready Mask */
    HDC2010_TH_MASK   = 0x40U,                  /* Temperature High Mask */
    HDC2010_TL_MASK   = 0x20U,                  /* Temperature Low Mask */
    HDC2010_HH_MASK   = 0x10U,                  /* Humidity High Mask */       
    HDC2010_HL_MASK   = 0x08U,                  /* Humidity Low Mask */
    HDC2010_NO_MASK   = 0x00U                   /* No Mask */
} HDC2010_InterruptMask;

/*!
 *  @brief    HDC2010 Interrupt Mode
 *
 *  This enumeration defines the Interrupt Mode of the Device
 *  The INT_MODE bit control in what mode does the HDC2010 operate in
 *
 *  Interrupt mode
 *  0 = Level sensitive
 *  1 = Comparator mode
 *
 */
typedef enum HDC2010_InterruptMode  {
    HDC2010_LEVEL_MODE  = 0x00U,                /* Level Mode */
    HDC2010_COMP_MODE   = 0x01U                 /* Comparator Mode */
} HDC2010_InterruptMode ;

/*!
 *  @brief    HDC2010 Interrupt Pin Polarity
 *
 *  This enumeration defines the Interrupt E of the Device
 *  The INT_POL bit control in what mode does the HDC2010 operate in
 *
 *  Interrupt polarity
 *  0 = Active Low
 *  1 = Active High
 *
 *
 */
typedef enum HDC2010_InterruptPinPolarity {
    HDC2010_ACTIVE_LO  = 0x00U,                 /* Active Low Mode */
    HDC2010_ACTIVE_HI  = 0x02U                  /* Active High Mode */
} HDC2010_InterruptPinPolarity;


/*!
 *  @brief    HDC2010 configuration
 *
 *  The HDC2010_Config structure contains a set of pointers used to characterize
 *  the HDC2010 driver implementation.
 *
 *  This structure needs to be defined and provided by the application.
 */
typedef struct HDC2010_Config {
    /*! Pointer to a driver specific hardware attributes structure */
    void const    *hwAttrs;

    /*! Pointer to an uninitialized user defined HDC2010_Object struct */
    void          *object;
} HDC2010_Config;

/*!
 *  @brief    Hardware specific settings for a HDC2010 sensor.
 *
 *  This structure should be defined and provided by the application. The
 *  gpioIndex should be defined in accordance of the GPIO driver. The pin
 *  must be configured as GPIO_CFG_INPUT and GPIO_CFG_IN_INT_FALLING.
 */
typedef struct HDC2010_HWAttrs {
    HDC2010_SlaveAddress    slaveAddress;       /*!< I2C slave address */
    unsigned int           gpioIndex;           /*!< GPIO configuration index */
} HDC2010_HWAttrs;

/*!
 *  @brief    Members should not be accessed by the application.
 */
typedef struct HDC2010_Object {
    I2C_Handle          i2cHandle;
    GPIO_CallbackFxn    callback;
} HDC2010_Object;

/*!
 *  @brief  HDC2010 Parameters
 *
 *  HDC2010 parameters are used with the HDC2010_open() call. Default values for
 *  these parameters are set using HDC2010_Params_init(). The GPIO_CallbackFxn
 *  should be defined by the application only if the ALERT functionality is
 *  desired. A gpioIndex must be defined in the HDC2010_HWAttrs for the
 *  corresponding HDC2010Index. The GPIO_CallbackFxn is in the context of an
 *  interrupt handler.
 *
 *  @sa     HDC2010_Params_init()
 */
typedef struct HDC2010_Params {
    HDC2010_MeasurementMode       measurementMode;
    HDC2010_TemperatureResolution tempResolution;           /**< temperature sampling rate*/
    HDC2010_HumidityResolution    humResolution;            /**< humidity sampling rate*/
    HDC2010_OutputDataRate        outputDataRate;
    HDC2010_InterruptEn           interruptEn;
    HDC2010_InterruptMask         interruptMask;
    HDC2010_InterruptMode         interruptMode;
    HDC2010_InterruptPinPolarity  interruptPinPolarity;
    GPIO_CallbackFxn              callback;                 /*!< Pointer to GPIO callback                */
} HDC2010_Params;

/**************************************************************/
/**\name    FUNCTION DECLARATIONS                         */
/**************************************************************/


/*!
 *  @brief  Function to initialize HDC2010 driver.
 *
 *  This function will initialize the HDC2010 driver. This should be called
 *  before BIOS_start().
 */
extern void HDC2010_init();

/*!
 *  @brief  Function to initialize a HDC2010_Params struct to its defaults
 *
 * HDC2010_defaultParams = {
 *           HDC2010_HT_MODE,                     !< Continuous Conversion Mode   
 *           HDC2010_T14_BITS,                    !< Conversion Cycle Time 15.5ms 
 *           HDC2010_H14_BITS,                    !< 0 Samples per conversion 
 *           HDC2010_TRIGG_2_HZ,                  !< Alert Mode                
 *           HDC2010_HI_Z_MODE,                   !< Alert Pin Mode              
 *           HDC2010_NO_MASK,                     !< Active Low on Alert            
 *           HDC2010_LEVEL_MODE,
 *           HDC2010_ACTIVE_HI,
 *           NULL,                                !< Pointer to GPIO callback       
 * }
 *
 *  @param  params      A pointer to HDC2010_Params structure for
 *                      initialization.
 *
 */
extern void HDC2010_Params_init(HDC2010_Params *params);

/*!
 *  @brief  Function to open a given HDC2010 sensor
 *
 *  Function to initialize a given HDC2010 sensor specified by the particular
 *  index value. This function must be called from a task context.
 *  If one intends to use the ALERT pin, a callBack function must be specified
 *  in the HDC2010_Params structure. Additionally, a gpioIndex must be setup
 *  and specified in the HDC2010_HWAttrs structure.
 *
 *  The I2C controller must be operating in BLOCKING mode. Failure to ensure
 *  the I2C_Handle is in BLOCKING mode will result in undefined behavior.
 *
 *  The user should ensure that each sensor has its own slaveAddress,
 *  gpioIndex (if alarming) and HDC2010Index.
 *
 *  @pre    HDC2010_init() has to be called first
 *
 *  @param  HDC2010Index   Logical sensor number for the HDC2010 indexed into
 *                         the HDC2010_config table
 *
 *  @param  i2cHandle      An I2C_Handle opened in BLOCKING mode
 *
 *  @param  *params        A pointer to HDC2010_Params structure. If NULL, it
 *                         will use default values.
 *
 *  @return  A HDC2010_Handle on success, or a NULL on failure.
 *
 *  @sa      HDC2010_init()
 *  @sa      HDC2010_Params_init()
 *  @sa      HDC2010_close()
 */

extern HDC2010_Handle HDC2010_open(unsigned int HDC2010Index, I2C_Handle i2cHandle, 
    HDC2010_Params *params);

/*!
 *  @brief  Function to close a HDC2010 sensor specified by the HDC2010 handle
 *
 *  The HDC2010 hardware will be placed in a low power state in which ADC
 *  conversions are disabled. If the pin is configured to alarm, the ALERT pin
 *  and GPIO pin interrupts will be disabled. The I2C handle is not affected.
 *
 *  @pre    HDC2010_open() had to be called first.
 *
 *  @param  handle      A HDC2010_Handle returned from HDC2010_open()
 *
 *  @return true on success or false upon failure.
 */

extern bool HDC2010_close(HDC2010_Handle handle);

/*!
 *  @brief This API is used to read temperature
 *
 *  @param  handle    A HDC2010_Handle
 *
 *  @param  unit      A HDC2010_TempScale field specifying the temperature
 *                    return format.
 *
 *  @param  data      A pointer to a float in which temperature data will be
 *                    stored in.
 * 
 *  @return true on success or false upon failure.
 */
extern bool HDC2010_getTemp(HDC2010_Handle handle, HDC2010_TempScale unit, float *data);

/*!
 *  @brief This API is used to read humidity
 *
 *  @param  handle    A HDC2010_Handle
 *
 *  @param  data      A pointer to a float in which humidity data will be
 *                    stored in.
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_getHum(HDC2010_Handle handle, float *data);

/*!
 *  @brief This API is used to read max temperature value measured
 *
 *  @param  handle    A HDC2010_Handle
 *
 *  @param  unit      A HDC2010_TempScale field specifying the temperature
 *                    return format.
 *
 *  @param  data      A pointer to a float in which temperature data will be
 *                    stored in.
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_getMaxTemp(HDC2010_Handle handle, HDC2010_TempScale unit, float *data);

/*!
 *  @brief This API is used to read max humidity value measured
 * 
 *  @param  handle    A HDC2010_Handle
 *
 *  @param  data      A pointer to a float in which humidity data will be
 *                    stored in.
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_getMaxHum(HDC2010_Handle handle, float *data);

/*!
 *  @brief  This function will set the temperature limits.
 *
 *  This function will write the specified high and low temperature limits to the
 *  specified HDC2010 sensor.
 *
 *  @param  handle    A HDC2010_Handle
 *
 *  @param  unit      A HDC2010_TempScale field specifying the temperature
 *                    return format.
 *
 *  @param  high      A float that specifies the high limit.
 *
 *  @param  low       A float that specifies the low limit. 
 *
 *  @return true on success or false upon failure.
 */
extern bool HDC2010_setTempLimit(HDC2010_Handle handle, HDC2010_TempScale unit, float high, float low);

/*!
 *  @brief  This function will set the humidity limits.
 *
 *  This function will write the specified high and low humidity limits to the
 *  specified HDC2010 sensor.
 *
 *  @param  handle    A HDC2010_Handle
 *
 *  @param  high      A float that specifies the high limit.
 *
 *  @param  low       A float that specifies the low limit. 
 *
 *  @return true on success or false upon failure.
 */
extern bool HDC2010_setHumLimit(HDC2010_Handle handle, float high, float low);

/*!
 *  @brief This API is used to set
 *  the output data sampling rate setting in the register 0x0E
 *  bits from 4 to 6
 *
 *  value               |   Output Data Sampling Rate
 * ---------------------|---------------------------------
 *  0x00                | No Repeated Measurements. Trigger On Demand
 *  0x01                | 1/120Hz
 *  0x02                | 1/60Hz
 *  0x03                | 0.1Hz
 *  0x00                | 0.2Hz
 *  0x01                | 1Hz
 *  0x02                | 2Hz
 *  0x03                | 5Hz
 *
 *
 *  @param  handle             A HDC2010_Handle 
 *
 *  @param  outputDataRate     A HDC2010_OutputDataRate
 *
 *
 * @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_configODR(HDC2010_Handle handle, HDC2010_OutputDataRate outputDataRate);

/*!
 *  @brief This API is used to set
 *  the temperature resolution setting in the register 0x0F
 *  bits from 6 to 7
 *
 *  value               |   Temperature resolution
 * ---------------------|---------------------------------
 *  0x00                | 14 bit
 *  0x01                | 11 bit
 *  0x02                | 9 bit
 *  0x03                | NA (TBC)
 *
 *
 *  @param  handle             A HDC2010_Handle 
 *
 *  @param  tempResolution     A HDC2010_TemperatureResolution
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_configTempResolution(HDC2010_Handle handle, HDC2010_TemperatureResolution tempResolution);

/*!
 *  @brief This API is used to set
 *  the humidity resolution setting in the register 0x0F
 *  bits from 4 to 5
 *
 *  value               |   Humidity resolution
 * ---------------------|---------------------------------
 *  0x00                | 14 bit
 *  0x01                | 11 bit
 *  0x02                | 9 bit
 *  0x03                | NA (TBC)
 *
 *
 *  @param  handle            A HDC2010_Handle 
 *
 *  @param  humResolution     A HDC2010_HumidityResolution
 *
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_configHumResolution(HDC2010_Handle handle, HDC2010_HumidityResolution humResolution);

/*!
 *  @brief This API is used to set
 *  the Measurement configuration setting in the register 0x0F
 *  bits from 1 to 2
 *
 *  value               |   Measurement configuration
 * ---------------------|---------------------------------
 *  0x00                | Humidity + Temperature
 *  0x01                | Temperature Only
 *  0x02                | Humidity Only
 *  0x03                | NA
 *
 *
 *  @param  handle              A HDC2010_Handle 
 *
 *  @param  measurementMode     A HDC2010_MeasurementMode
 *
 *
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_configMeasurementMode(HDC2010_Handle handle, HDC2010_MeasurementMode measurementMode);

/*!
 *  @brief This API is used to set
 *  the Measurement configuration setting in the register 0x0F
 *  bits from 1 to 2
 *
 *
 *  @param  handle                   A HDC2010_Handle 
 *
 *  @param  interruptEn              A HDC2010_InterruptEn
 *
 *  @param  interruptMode            A HDC2010_InterruptMode
 *
 *  @param  interruptPinPolarity     A HDC2010_InterruptPinPolarity
 *
 *  @param  interruptMask            A HDC2010_InterruptMask
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_configInterrupt(HDC2010_Handle handle, HDC2010_InterruptEn interruptEn, HDC2010_InterruptMode interruptMode,
                                    HDC2010_InterruptPinPolarity interruptPinPolarity, HDC2010_InterruptMask interruptMask);

/*!
 *  @brief This API is used to set
 *  the Measurement trigger in the register 0x0F
 *  bit 0
 *
 *
 *
 *  @return true on success or false upon failure.
 *
 *
 */
extern bool HDC2010_triggerMeasurement(HDC2010_Handle handle);
#endif /* HDC2010_H_ */
