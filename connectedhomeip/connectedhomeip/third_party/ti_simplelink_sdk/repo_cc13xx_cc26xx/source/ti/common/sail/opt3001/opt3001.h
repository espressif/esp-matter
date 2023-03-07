/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** ============================================================================
 *  @file       opt3001.h
 *
 *  @brief      OPT3001 Ambient Light Sensor Driver
 *
 *  The OPT3001 is a sensor that measures the intensity of visible light. The
 *  spectral response of the sensor tightly matches the photopic response of
 *  the human eye and includes significant infrared rejection.
 *
 *  The OPT3001 header file should be included in an application as
 *  follows:
 *  @code
 *  #include <ti/common/sail/opt3001/opt3001.h>
 *  @endcode
 *
 *  # Operation
 *  The OPT3001 driver simplifies using a OPT3001 sensor to perform lux
 *  readings. The board's I2C peripheral and pins must be configured and then
 *  initialized by using I2C_init(). Similarly, any GPIO pins must be
 *  configured and then initialized by using GPIO_init(). A OPT3001_config
 *  array should be defined by the application. The OPT3001_config array should
 *  contain pointers to a defined OPT3001_HwAttrs and allocated array for the
 *  OPT3001_Object structures. OPT3001_init() must be called prior to using
 *  OPT3001_open().
 *
 *  The APIs in this driver serve as an interface to a DPL(Driver Porting Layer)
 *  The specific implementations are responsible for creating all the RTOS
 *  specific primitives to allow for thread-safe operation.
 *
 *  This driver has no dynamic memory allocation.
 *
 *  ## Defining OPT3001_Config, OPT3001_Object and OPT3001_HwAttrs #
 *  Each structure must be defined by the application. The following
 *  example is for a MSP432 in which an OPT3001 sensor is setup.
 *  The following declarations are placed in "MSP_EXP432P401R.h"
 *  and "MSP_EXP432P401R.c" respectively. How the gpioIndex is defined
 *  is detailed in the next example.
 *
 *  "MSP_EXP432P401R.h"
 *  @code
 *  typedef enum MSP_EXP432P401R_OPT3001Name {
 *      OPT3001_AMBIENT = 0, // Sensor measuring ambient light
 *      MSP_EXP432P401R_OPT3001COUNT
 *  } MSP_EXP432P401R_OPT3001Name;
 *
 *  @endcode
 *
 *  "MSP_EXP432P401R.c"
 *  @code
 *  #include <opt3001.h>
 *
 *  OPT3001_Object OPT3001_object[MSP_EXP432P401R_OPT3001COUNT];
 *
 *  const OPT3001_HWAttrs OPT3001_hwAttrs[MSP_EXP432P401R_OPT3001COUNT] = {
 *      {
 *          .slaveAddress = OPT3001_SA1, // 0x42
 *          .gpioIndex = MSP_EXP432P401R_OPT3001_0,
 *      },
 *  };
 *
 *  const OPT3001_Config OPT3001_config[] = {
 *      {
 *          .hwAttrs = &OPT3001_hwAttrs[0],
 *          .objects = &OPT3001_object[0],
 *      },
 *      {NULL, NULL},
 *  };
 *  @endcode
 *
 *  ##Setting up GPIO configurations #
 *  The following example is for a MSP432 in which an OPT3001 sensors
 *  needs a GPIO pin for alarming. The following definitions are in
 *  "MSP_EXP432P401R.h" and "MSP_EXP432P401R.c" respectively. This
 *  example uses GPIO pin 1.5. The GPIO_CallbackFxn table must
 *  contain as many indices as GPIO_CallbackFxns that will be specified by
 *  the application. For each INT pin used, an index should be allocated
 *  as NULL.
 *
 *  "MSP_EXP432P401R.h"
 *  @code
 *  typedef enum MSP_EXP432P401R_GPIOName {
 *      MSP_EXP432P401R_OPT3001_0, // Pin for sensor measuring ambient light
 *      MSP_EXP432P401R_GPIOCOUNT
 *  } MSP_EXP432P401R_GPIOName;
 *  @endcode
 *
 *  "MSP_EXP432P401R.c"
 *  @code
 *  #include <gpio.h>
 *
 *  GPIO_PinConfig gpioPinConfigs[] = {
 *      GPIOMSP432_P1_5 | GPIO_CFG_INPUT | GPIO_CFG_IN_INT_FALLING
 *  };
 *
 *  GPIO_CallbackFxn gpioCallbackFunctions[] = {
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
 *  the OPT3001 hardware is capable of communicating at 400kHz. The default
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
 *  ## Opening a OPT3001 sensor with default parameters #
 *  The OPT3001_open() call must be made in a task context.
 *
 *  @code
 *  #include <opt3001.h>
 *
 *  OPT3001_Handle opt3001Handle;
 *
 *  OPT3001_init();
 *  opt3001Handle = OPT3001_open(OPT3001_AMBIENT, i2cHandle, NULL);
 *  @endcode
 *
 *  ## Opening a OPT3001 sensor to ALERT #
 *  In the following example, a callback function is specified in the
 *  opt3001Params structure. This indicates to the module that
 *  the INT pin will be used. The sensor will assert the INT pin whenever the
 *  lux exceeds 44178.8 (lx). The low limit is ignored. No interrupt will be
 *  generated until OPT3001_enableInterrupt() is called.
 *
 *  @code
 *  #include <opt3001.h>
 *
 *  OPT3001_Handle opt3001Handle;
 *  OPT3001_Params opt3001Params;
 *
 *  OPT3001_Params_init(&opt3001Params);
 *  opt3001Params.callback = gpioCallbackFxn;
 *  opt3001Handle = OPT3001_open(Board_OPT3001_LIGHT, i2cHandle, &opt3001Params);
 *
 *  OPT3001_setLuxLimits(opt3001Handle, 44178.8, OPT3001_IGNORE);
 *  OPT3001_enableInterrupt(opt3001Handle);
 *  @endcode
 *  ============================================================================
 */

#ifndef OPT3001_H_
#define OPT3001_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Driver Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>

/* OPT3001 Register Addresses */
#define OPT3001_RESULT    0x0000  /* Result Register (Read-Only)  */
#define OPT3001_CONFIG    0x0001  /* Configuration Register       */
#define OPT3001_LOLIMIT   0x0002  /* Low-Limit Register           */
#define OPT3001_HILIMIT   0x0003  /* High-Limit Register          */
#define OPT3001_MFTID     0x007E  /* Manufacturer ID Register     */
#define OPT3001_DEVICEID  0x007F  /* Device ID Register           */

/* OPT3001 Configuration Register Bits */
#define OPT3001_FL        0x0020  /* Flag Low Field               */
#define OPT3001_FH        0x0040  /* Flag High Field              */
#define OPT3001_CRF       0x0080  /* Conversion Ready Field       */
#define OPT3001_OVF       0x0100  /* Overflow Flag Field          */

/*!
 *  @brief    Ignore lux limit define
 *
 *  OPT3001_IGNORE should be used to ignore or unset a lux limit.
 *
 *  In the following example, the lux limits are set and reset:
 *  @code
 *  OPT3001_setLuxLimits(handle, 43210, 76543.7);
 *  OPT3001_enableInterrupt(handle);
 *  //Additional application code...
 *  OPT3001_setLuxLimits(handle, OPT3001_IGNORE, OPT3001_IGNORE);
 *  @endcode
 */
#define OPT3001_IGNORE         0xFFFF

/*!
 *  @brief    OPT3001 ADC conversion mode
 *
 *  The conversion mode controls whether the device is operating in continuous
 *  conversion, single-shot, or low-power shutdown mode. The hardware default
 *  is shutdown mode such that upon power-up, the device does not perform
 *  conversions. When single-shot mode is selected, the device performs a
 *  conversion and then enters shutdown mode upon completion.
 */
typedef enum OPT3001_ConversionMode {
    OPT3001_SHUTDOWN   = 0x0000, /*!< No conversions (hardware default)     */
    OPT3001_SINGLESHOT = 0x0200, /*!< One conversion, then shutdown         */
    OPT3001_CONTINUOUS = 0x0600  /*!< Continuous conversions                */
} OPT3001_ConversionMode;

/*!
 *  @brief    OPT3001 Conversion Ready Interrupt Mode
 *
 *  Using OPT3001_CONV_EN generates an interrupt when the conversion ready
 *  field indicates a conversion is complete. The conversion ready field is
 *  set to 1 at the end of a conversion and cleared when the configuration
 *  register is subsequuently read or written with any value except one
 *  containing the OPT3001_SHUTDOWN field command. This mode can be used in
 *  conjuction with OPT3001_LATCH or OPT3001_NOLATCH.
 */
typedef enum OPT3001_ConversionReady {
    OPT3001_CONVRD_DIS = 0x000,
    OPT3001_CONVRD_EN = 0xC000
} OPT3001_ConversionReady;

/*!
 *  @brief    OPT3001 Conversion Time
 *
 *  The conversion time determines the length of the light to digital
 *  conversion process. A longer conversion time allows for a lower noise
 *  measurement. The conversion time also relates to the effective resolution
 *  of the data conversion process. Using 800ms ensures full resolution of
 *  the result register while 100ms may limit the resolution based upon
 *  the range.
 */
typedef enum OPT3001_ConversionTime {
    OPT3001_100MS =  0,      /*!< 100 ms conversion time (0.1 s)  */
    OPT3001_800MS =  0x0800  /*!< 800 ms conversion time (0.8 s)  */
} OPT3001_ConversionTime;


/*!
 *  @brief    OPT3001 Fault Count
 *
 *  The fault count field instructs the device as to how many consecutive fault
 *  events are required to trigger the interrupt reporting mechanism: the INT
 *  pin, the flag high field, and flag low field. When the fault count is
 *  reached, the INT pin is asserted.
 */
typedef enum OPT3001_FaultCount {
    OPT3001_FAULT1   = 0x0000,  /*!< One fault count (default)  */
    OPT3001_FAULT2   = 0x0001,  /*!< Two fault counts           */
    OPT3001_FAULT4   = 0x0002,  /*!< Four fault counts          */
    OPT3001_FAULT8   = 0x0003   /*!< Eight fault counts         */
} OPT3001_FaultCount;

/*!
 *  @brief    OPT3001 Result Register Range
 *
 *  The range field selects the full-scale lux range of the device. The result
 *  register may contain a value up to that specified by the range. Greater
 *  range constitutes lower resolution while less range consitutes higher
 *  resolution.
 *
 *  Automatic scaling will determine the optimum range based on a 10-ms
 *  measurement of current lighting conditions. With every measurement there
 *  after, the range is automatically adjusted based on the result (E.g If the
 *  result is towards the upper side of the range, the full-scale range is
 *  increased. If the result is towards the lower side of the range, the
 *  full-scale range is decreased). If the result is outside of the
 *  automatically set range, the current result is aborted and a 10-ms
 *  measurement is taken to re-assess and properly reset the full-scale range.
 *  Therefore, during fast optical transients in automatic mode, a measurement
 *  can possibly take 10 ms rather the time set in OPT3001_ConversionTime.
 */
typedef enum OPT3001_FullRange {
    OPT3001_RANGE0   = 0x0000,  /*!< Range = 00040.95,  lux per LSB = 00.01  */
    OPT3001_RANGE1   = 0x1000,  /*!< Range = 00081.90,  lux per LSB = 00.02  */
    OPT3001_RANGE2   = 0x2000,  /*!< Range = 00163.80,  lux per LSB = 00.04  */
    OPT3001_RANGE3   = 0x3000,  /*!< Range = 00327.60,  lux per LSB = 00.08  */
    OPT3001_RANGE4   = 0x4000,  /*!< Range = 00665.20,  lux per LSB = 00.16  */
    OPT3001_RANGE5   = 0x5000,  /*!< Range = 01310.40,  lux per LSB = 00.32  */
    OPT3001_RANGE6   = 0x6000,  /*!< Range = 02620.80,  lux per LSB = 00.64  */
    OPT3001_RANGE7   = 0x7000,  /*!< Range = 05241.60,  lux per LSB = 01.28  */
    OPT3001_RANGE8   = 0x8000,  /*!< Range = 10483.20,  lux per LSB = 02.56  */
    OPT3001_RANGE9   = 0x9000,  /*!< Range = 20966.40,  lux per LSB = 05.12  */
    OPT3001_RANGE10  = 0xA000,  /*!< Range = 41932.80,  lux per LSB = 10.24  */
    OPT3001_RANGE11  = 0xB000,  /*!< Range = 83865.60,  lux per LSB = 20.48  */
    OPT3001_AUTO     = 0xC000   /*!< Automatic Scaling                       */
} OPT3001_FullRange;

/*!
 *  @brief    OPT3001 Interrupt Mode
 *
 *  Interrupt mode controls the behavior of the INT pin.
 *
 *  If no latch (0), the INT pin is asserted whenever the enabled alert
 *  conditions are met. The INT pin will de-assert automatically when the
 *  condition clears.
 *
 *  In latched mode (1), the INT pin is asserted whenever the appropriate
 *  conditions are met. The INT pin will remain asserted until
 *  the application performs a clearing event (E.g reading the configuration
 *  register).
 */
typedef enum OPT3001_InterruptMode {
    OPT3001_NOLATCH  =  0,      /*!< Non-Latch Mode */
    OPT3001_LATCH    =  0x0010  /*!< Latch Mode     */
} OPT3001_InterruptMode;


/*!
 *  @brief    A handle that is returned from a OPT3001_open() call.
 */
typedef struct OPT3001_Config    *OPT3001_Handle;

/*!
 *  @brief    OPT3001 I2C slave addresses.
 *
 *  The OPT3001 Slave Address is determined by the input to the ADDR pin.
 *  In two cases, the ADDR pin may be coupled with the SDA or SCL bus.
 *  OPT3001 sensors on the same I2C bus cannot share the same slave address.
 */
typedef enum OPT3001_SlaveAddress {
    OPT3001_SA1 = 0x44,  /*!< ADDR = GND */
    OPT3001_SA2 = 0x45,  /*!< ADDR = VDD */
    OPT3001_SA3 = 0x46,  /*!< ADDR = SDA */
    OPT3001_SA4 = 0x47,  /*!< ADDR = SCL */
} OPT3001_SlaveAddress;

/*!
 *  @brief    OPT3001 configuration
 *
 *  The OPT3001_Config structure contains a set of pointers used to characterize
 *  the OPT3001 driver implementation.
 *
 *  This structure needs to be defined and provided by the application.
 */
typedef struct OPT3001_Config {
    /*! Pointer to a driver specific hardware attributes structure */
    void const    *hwAttrs;

    /*! Pointer to an uninitialized user defined OPT3001_Object struct */
    void          *object;
} OPT3001_Config;

/*!
 *  @brief    Hardware specific settings for a OPT3001 sensor.
 *
 *  This structure should be defined and provided by the application. The
 *  gpioIndex should be defined in accordance of the GPIO driver. The pin
 *	must be configured as GPIO_CFG_INPUT and GPIO_CFG_IN_INT_FALLING.
 */
typedef struct OPT3001_HWAttrs {
    OPT3001_SlaveAddress    slaveAddress;    /*!< I2C slave address */
    unsigned int            gpioIndex;       /*!< GPIO configuration index */
} OPT3001_HWAttrs;

/*!
 *  @brief    Members should not be accessed by the application.
 */
typedef struct OPT3001_Object {
    I2C_Handle                i2cHandle;
    GPIO_CallbackFxn          callback;
    OPT3001_FullRange         range;
} OPT3001_Object;

/*!
 *  @brief  OPT3001 Parameters
 *
 *  OPT3001 parameters are used with the OPT3001_open() call. Default values for
 *  these parameters are set using OPT3001_Params_init(). The GPIO_CallbackFxn
 *  should be defined by the application only if the ALERT functionality is
 *  desired. A gpioIndex must be defined in the OPT3001_HWAttrs for the
 *  corresponding OPT3001Index. The GPIO_CallbackFxn is in the context of an
 *  interrupt handler.
 *
 *  @sa     OPT3001_Params_init()
 */
typedef struct OPT3001_Params {
        OPT3001_ConversionMode    conversionMode;
        OPT3001_ConversionReady   conversionReady;
        OPT3001_ConversionTime    conversionTime;
        OPT3001_FaultCount        faultCount;
        OPT3001_FullRange         range;
        OPT3001_InterruptMode     interruptMode;
        GPIO_CallbackFxn          callback;
} OPT3001_Params;

/*!
 *  @brief  Function to close a OPT3001 sensor specified by the OPT3001 handle
 *
 *  The OPT3001 hardware will be placed in a low power state in which ADC
 *  conversions are disabled. If the pin is configured to interrupt, the GPIO
 *  pin interrupts will be disabled. The I2C handle is not affected.
 *
 *  @pre    OPT3001_open() had to be called first.
 *
 *  @param  handle    A OPT3001_Handle returned from OPT3001_open()
 *
 *  @return true on success or false upon failure.
 */
extern bool OPT3001_close(OPT3001_Handle handle);

/*!
 *  @brief  Function to disable the GPIO interrupt.
 *
 *  Interrupts on the OPT3001 specific GPIO index will be disabled. The
 *  lux limits set are unaffected.
 *
 *  @sa     OPT3001_enableInterrupt()
 *
 *  @pre    Had to call OPT3001_enableInterrupt() first for this function to
 *          have an effect.
 *
 *  @param  handle    A OPT3001_Handle
 *
 *  @return true on success or false upon failure.
 */
extern bool OPT3001_disableInterrupt(OPT3001_Handle handle);

/*!
 *  @brief  Enable interrupts from the specified OPT3001
 *
 *  Interrupts on the OPT3001 specific GPIO index will be enabled.
 *
 *  @sa     OPT3001_disableInterrupt()
 *
 *  @pre    OPT3001_setLuxLimits() must have been called first to recieve
 *          interrupts from the OPT3001 INT pin.
 *
 *  @param  handle    A OPT3001_Handle
 *
 *  @return true on success or false upon failure.
 */
extern bool OPT3001_enableInterrupt(OPT3001_Handle handle);

/*!
 *  @brief  Function to get the sensor's lux reading
 *
 *  @param  handle    A OPT3001_Handle
 *
 *  @param  data      A pointer to a float to store the lux value.
 *
 *  @return true on success, false on failure
 */
extern bool OPT3001_getLux(OPT3001_Handle handle, float *data);

/*!
 *  @brief  Function to get the sensor's lux limits
 *
 *  @param  handle    A OPT3001_Handle
 *
 *  @param  high      A pointer to a float to store the high limit
 *
 *  @param  low       A pointer to a float to store the low limit
 *
 *  @return true on success, false on failure
 */
extern bool OPT3001_getLuxLimits(OPT3001_Handle handle,
                                float *high, float *low);

/*!
 *  @brief  Function to initialize OPT3001 driver.
 *
 *  This function will initialize the OPT3001 driver.
 */
extern void OPT3001_init();

/*!
 *  @brief  Function to open a given OPT3001 sensor
 *
 *  Function to initialize a given OPT3001 sensor specified by the particular
 *  index value. This function must be called from a task context.
 *  If one intends to use the INT pin, a callback function must be specified
 *  in the OPT3001_Params structure. Additionally, a gpioIndex must be setup
 *  and specified in the OPT3001_HWAttrs structure.
 *
 *  The I2C controller must be operating in BLOCKING mode. Failure to ensure
 *  the I2C_Handle is in BLOCKING mode will result in undefined behavior.
 *
 *  The user should ensure that each sensor has its own slaveAddress,
 *  gpioIndex (if alarming) and OPT3001Index.
 *
 *  @pre    OPT3001_init() has to be called first
 *
 *  @param  OPT3001Index    Logical sensor number for the OPT3001 indexed into
 *                          the OPT3001_config table
 *
 *  @param  i2cHandle       An I2C_Handle opened in BLOCKING mode
 *
 *  @param  params          A pointer to OPT3001_Params structure. If NULL, it
 *                          will use default values.
 *
 *  @return  A OPT3001_Handle on success, or a NULL on failure.
 *
 *  @sa      OPT3001_init()
 *  @sa      OPT3001_Params_init()
 *  @sa      OPT3001_close()
 */
extern OPT3001_Handle OPT3001_open(unsigned int index,
        I2C_Handle i2cHandle, OPT3001_Params *params);

/*!
 *  @brief  Function to initialize a OPT3001_Params struct to its defaults
 *
 *  @param  params      A pointer to OPT3001_Params structure for
 *                      initialization.
 *
 *  Default values are:
 *
 *       conversionMode    = OPT3001_CONTINUOUS
 *       conversionReady   = OPT3001_CONVRD_DIS
 *       conversionTime    = OPT3001_800MS
 *       faultCount        = OPT3001_FAULT1
 *       range             = OPT3001_AUTO
 *       interruptMode     = OPT3001_LATCH
 *       callback          = NULL
 */
extern void OPT3001_Params_init(OPT3001_Params *params);

/*!
 *  @brief  Read the specified register from a OPT3001 sensor.
 *
 *  @param  handle             A OPT3001_Handle
 *
 *  @param  registerAddress    Register address
 *
 *  @param  data               A pointer to a data register in which received
 *                             data will be written to. Must be 16 bits.
 *
 *  @return true on success or false upon failure.
 */
extern bool OPT3001_readRegister(OPT3001_Handle handle,
        uint16_t *data, uint8_t registerAddress);

/*!
 *  @brief  Function to change the conversion mode
 *
 *  This function will set the OPT3001 hardware to operate in the conversion
 *  mode specified by OPT3001_ConversionMode.
 *
 *  @pre    OPT3001_open has to be called first.
 *
 *  @param  handle    A OPT3001_Handle
 *
 *  @param  mode      A OPT3001_ConversionMode specifying the mode to
 *                    operate in.
 *
 *  @return true on success or false upon failure.
 */
extern bool OPT3001_setConversionMode(OPT3001_Handle handle,
        OPT3001_ConversionMode mode);

/*!
 *  @brief  Function to get the sensor's lux limits
 *
 *  @param  handle    A OPT3001_Handle
 *
 *  @param  high      A float specifying the high limit. Use
 *                    OPT3001_IGNORE if high limit is not desired.
 *
 *  @param  low       A float specifying the low limit. Use
 *                    OPT3001_IGNORE if a low limit is not desired.
 *
 *  @return true on success, false on failure
 */
extern bool OPT3001_setLuxLimits(OPT3001_Handle handle, float high, float low);

/*!
 *  @brief  Set the lux measurement result range.
 *
 *  @param  handle               A OPT3001_Handle
 *
 *  @param  OPT3001_FullRange    A range value specifying max possible result
 *
 *  @return true on success or false upon failure.
 */
extern bool OPT3001_setRange(OPT3001_Handle handle, OPT3001_FullRange range);

/*!
 *  @brief  Write the specified data to a OPT3001 sensor.
 *
 *  This function is not thread safe. When writing to a handle, it is possible
 *  to overwrite data written by another task.
 *
 *  For example: Task A and B are writing to the configuration register.
 *  Task A has a higher priority than Task B. Task B is running and
 *  reads the configuration register. Task A then preempts Task B and reads
 *  the configuration register, performs a logical OR and writes to the
 *  configuration register. Task B then resumes execution and performs its
 *  logical OR and writes to the configuration register--overwriting the data
 *  written by Task A.
 *
 *  Such instances can be prevented through the use of Semaphores. Below is an
 *  example which utilizes an initialized Semaphore_handle, OPT3001Lock.
 *
 *  @code
 *  if (0 == sem_wait(&OPT3001Lock)) 
 *  {  
 *      //Perform read/write operations
 *      sem_post(OPT3001Lock);
 *  }
 *  else
 *  {
 *      //handle error scenario
 *  }
 *  @endcode
 *
 *  @param  handle             A OPT3001_Handle
 *
 *  @param  data               A uint16_t to be written to the OPT3001 sensor
 *
 *  @param  registerAddress    OPT3001 register address

 *  @return true on success or false upon failure.
 */
extern bool OPT3001_writeRegister(OPT3001_Handle handle, uint16_t data,
        uint8_t registerAddress);

#ifdef __cplusplus
}
#endif

#endif /* OPT3001_H_ */
