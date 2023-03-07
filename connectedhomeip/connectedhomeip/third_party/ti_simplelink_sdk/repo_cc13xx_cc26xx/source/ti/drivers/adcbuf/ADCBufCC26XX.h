/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       ADCBufCC26XX.h
 *
 *  @brief      ADCBuf driver implementation for a CC26XX analog-to-digital
 *              converter
 *
 * # Driver include #
 *  The ADCBuf header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/ADCBuf.h>
 *  #include <ti/drivers/adc/ADCBufCC26XX.h>
 *  @endcode
 *
 * # Overview #
 * This is a CC26XX specific implementation of the generic TI-RTOS ADCBuf driver.
 * The generic ADCBuf API specified in ti/drivers/ADCBuf.h should be called by
 * the application, not the device specific implementation in
 * ti/drivers/adcbuf/ADCBufCC26XX. The board file defines the device specific
 * configuration and casting in the general API ensures the correct device
 * specific functions are called. You should specify an
 * ADCBufCC26XX_ParamsExtension in the custom field of the ADCBuf_Params that
 * suits your application. The default settings work for many, but not all,
 * use cases.
 *
 * # General Behavior #
 * A timer and the DMA are used to trigger the ADC and fill a buffer in the
 * background (in hardware) at a specified frequency. The application may
 * execute other tasks while the hardware handles the conversions. In contrast
 * to the standard ti/drivers/ADC driver, this driver allows for precise
 * sampling of waveforms.
 *
 * | Driver         | Number of samples needed in one call    |
 * |----------------|-----------------------------------------|
 * | ADC.h          | 1                                       |
 * | ADCBuf.h       | > 1                                     |
 *
 * This ADCBuf driver provides an API interface to using the analog-to-digital
 * converter directly from the CM3 without going through the sensor controller.
 * The sensor controller can still use the ADC, support for sharing the ADC
 * resource between the sensor controller and the CM3 is built into the driver.
 * There is a hardware semaphore that the driver must acquire before beginning
 * any number of conversions. This same hardware semaphore also prevents the
 * simultaneous use of this driver and the basic ADC driver.
 *
 * The ADC drivers supports making between one and 1024 measurements once or
 * continuous measuring with returned buffer sizes between one and 1024
 * measurements.
 *
 * The application should call ADCBuf_init() once by the application to set the
 * isOpened flag to false, indicating that the driver is ready to use.
 *
 * The ADC driver is opened by calling ADCBuf_open() which will
 * set up interrupts and configure the internal components of the driver.
 * However, the ADC hardware or analog pins are not yet configured, since the
 * sensor controller or basic ADC driver might be using the ADC.
 *
 * In order to perform an ADC conversion, the application should call
 * ADCBuf_convert(). This call will request the ADC resource, configure the ADC,
 * set up the DMA and GPTimer, and perform the requested ADC conversions on the
 * selected DIO or internal signal. The DIO or internal signal is defined by the
 * ADCBuf_Conversion structure in the application code and adcBufCC26xxObjects
 * in the board file.
 *
 * @warning If the ADCBUF driver is setup in ADCBuf_RECURRENCE_MODE_CONTINUOUS
 *          mode, the user must assure that the provided callback function is
 *          completed before the next conversion completes. If the next
 *          conversion completes before the callback function finishes, the DMA
 *          will clobber the previous buffer with new data.
 *
 * If the sensor controller is using the ADC when the driver requests it at the
 * start of the ADC_convert() call, the conversion will fail and return false.
 * The ADC resource may be pre-acquired by calling the control function
 * ADCBufCC26XX_CMD_ACQUIRE_ADC_SEMAPHORE. It will be released again
 * automatically after the next conversion completes.
 *
 * In both ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS mode and
 * ADCBufCC26XX_SAMPING_MODE_ASYNCHRONOUS mode, enough sampling time must be
 * provided between conversions that each measurement may be completed before
 * the next trigger arrives.
 *
 * @note    The ADCBuf driver requires GPTimer0A to function correctly. It
 *          expects it to be configured as position 0 in the GPTimer Config
 *          Table. GPTimer0A will be unavailable for other uses.
 *
 * # Supported ADC pins #
 * Below is a table of the supported ADC IO pins for each package size, for both
 * CC26xx and CC13xx. It maps a DIO to its corresponding driverlib define for
 * the CompBInput that it is hardwired to. This table can be used to create
 * virtual channel entries in the ADCBufCC26XX_adcChannelLut table in the board
 * file.
 *
 * | DIO    | CC26xx 7x7 AUXIO CompBInput   | CC13xx 7x7 AUXIO CompBInput   | CC26xx 5x5 AUXIO CompBInput   | CC13xx 5x5 AUXIO CompBInput   | CC26xx 4x4 AUXIO CompBInput   | CC13xx 4x4 AUXIO CompBInput
 * |--------|-------------------------------|-------------------------------|-------------------------------|-------------------------------|-------------------------------|-----------------------------
 * | 0      | No                            | No                            | No                            | No                            | No                            | No
 * | 1      | No                            | No                            | No                            | No                            | No                            | No
 * | 2      | No                            | No                            | No                            | No                            | No                            | No
 * | 3      | No                            | No                            | No                            | No                            | No                            | No
 * | 4      | No                            | No                            | No                            | No                            | No                            | No
 * | 5      | No                            | No                            | No                            | No                            | ADC_COMPB_IN_AUXIO7           | ADC_COMPB_IN_AUXIO7
 * | 6      | No                            | No                            | No                            | No                            | ADC_COMPB_IN_AUXIO6           | ADC_COMPB_IN_AUXIO6
 * | 7      | No                            | No                            | ADC_COMPB_IN_AUXIO7           | ADC_COMPB_IN_AUXIO7           | ADC_COMPB_IN_AUXIO5           | ADC_COMPB_IN_AUXIO5
 * | 8      | No                            | No                            | ADC_COMPB_IN_AUXIO6           | ADC_COMPB_IN_AUXIO6           | ADC_COMPB_IN_AUXIO4           | ADC_COMPB_IN_AUXIO4
 * | 9      | No                            | No                            | ADC_COMPB_IN_AUXIO4           | ADC_COMPB_IN_AUXIO4           | ADC_COMPB_IN_AUXIO3           | ADC_COMPB_IN_AUXIO3
 * | 10     | No                            | No                            | ADC_COMPB_IN_AUXIO5           | ADC_COMPB_IN_AUXIO5           | No                            | No
 * | 11     | No                            | No                            | ADC_COMPB_IN_AUXIO3           | ADC_COMPB_IN_AUXIO3           | No                            | No
 * | 12     | No                            | No                            | ADC_COMPB_IN_AUXIO2           | ADC_COMPB_IN_AUXIO2           | No                            | No
 * | 13     | No                            | No                            | ADC_COMPB_IN_AUXIO1           | ADC_COMPB_IN_AUXIO1           | No                            | No
 * | 14     | No                            | No                            | ADC_COMPB_IN_AUXIO0           | ADC_COMPB_IN_AUXIO0           | No                            | No
 * | 15-22  | No                            | No                            | No                            | No                            | No                            | No
 * | 23     | ADC_COMPB_IN_AUXIO7           | ADC_COMPB_IN_AUXIO7           | No                            | No                            | No                            | No
 * | 24     | ADC_COMPB_IN_AUXIO6           | ADC_COMPB_IN_AUXIO6           | No                            | No                            | No                            | No
 * | 25     | ADC_COMPB_IN_AUXIO5           | ADC_COMPB_IN_AUXIO5           | No                            | No                            | No                            | No
 * | 26     | ADC_COMPB_IN_AUXIO4           | ADC_COMPB_IN_AUXIO4           | No                            | No                            | No                            | No
 * | 27     | ADC_COMPB_IN_AUXIO3           | ADC_COMPB_IN_AUXIO3           | No                            | No                            | No                            | No
 * | 28     | ADC_COMPB_IN_AUXIO2           | ADC_COMPB_IN_AUXIO2           | No                            | No                            | No                            | No
 * | 29     | ADC_COMPB_IN_AUXIO1           | ADC_COMPB_IN_AUXIO1           | No                            | No                            | No                            | No
 * | 30     | ADC_COMPB_IN_AUXIO0           | ADC_COMPB_IN_AUXIO0           | No                            | No                            | No                            | No
 *
 * # Supported Internal Signals #
 * Below is a table of internal signals that can be measured using the ADC.
 * Since we are not connecting to a DIO, there is no DIO to internal signal
 * mapping. The DIO field in the channel lookup table should be marked
 * PIN_UNASSIGNED. This table can be used to create virtual channel entries in
 * the ADCBufCC26XX_adcChannelLut table in the board file.
 *
 * | DIO                | Internal Signal CompBInput    |
 * |--------------------|-------------------------------|
 * | PIN_UNASSIGNED     | ADC_COMPB_IN_DCOUPL           |
 * | PIN_UNASSIGNED     | ADC_COMPB_IN_VSS              |
 * | PIN_UNASSIGNED     | ADC_COMPB_IN_VDDS             |
 *
 * # Error handling #
 * The following errors may occur when opening the ADC without assertions enabled:
 * - The ADC handle is already open.
 *
 * The following errors may occur when requesting an ADC conversion:
 * - The ADC is currently already doing a conversion.
 * - The ADC was not available (used by sensor controller or basic ADC).
 *
 *
 * # Power Management #
 * The TI-RTOS power management framework will try to put the device into the
 * most power efficient mode whenever possible. Please see the technical
 * reference manual for further details on each power mode.
 *
 * While converting, the ADCBufCC26XX driver sets a power constraint to keep
 * the device out of standby. When the conversion has finished, the power
 * constraint is released. The driver also sets a dependency on the DMA to
 * enable background transfers from the ADC FIFO to memory and to clear the
 * GPTimer interrupt.
 * The following statements are valid:
 *      - After ADCBuf_convert(): the device cannot enter standby.
 *      - After ADCBuf_convertCancel(): the device can enter standby again.
 *      - After a conversion finishes: the device can enter standby again.
 *
 *
 * # Supported Functions #
 * | API function                       | Description                                                           |
 * |------------------------------------|-----------------------------------------------------------------------|
 * | ADCBuf_init()                      | Initialize ADC driver                                                 |
 * | ADCBuf_open()                      | Open the ADC driver and configure driver                              |
 * | ADCBuf_convert()                   | Perform ADC conversion                                                |
 * | ADCBuf_convertCancel()             | Cancel ongoing ADC conversion                                         |
 * | ADCBuf_close()                     | Close ADC driver                                                      |
 * | ADCBuf_Params_init()               | Initialise ADCBuf_Params structure to default values                  |
 * | ADCBuf_getResolution()             | Get the resolution of the ADC of the current device                   |
 * | ADCBuf_adjustRawValues()           | Adjust the values in a returned buffer for manufacturing tolerances   |
 * | ADCBuf_convertAdjustedToMicroVolts | Convert a buffer of adjusted values to microvolts                     |
 * | ADCBuf_control()                   | Execute device specific functions                                     |
 *
 *
 *  # Not Supported Functionality #
 *     - Performing conversions on multiple channels simultaneously is not
 *       supported. In other words, the parameter channelCount must always be
 *       set to 1 when calling ADCBuf_convert(). The ADC on CC26XX devices does
 *       not support time-division multiplexing of channels or pins in hardware.
 *
 * # Use Cases #
 * ## Basic one-shot conversion #
 *  Perform one conversion on CONFIG_ADCCHANNEL_A1 in ::ADCBuf_RETURN_MODE_BLOCKING.
 *  @code
 *      #include <ti/drivers/ADCBuf.h>
 *
 *      #define ADCBUFFERSIZE   100
 *
 *      ADCBuf_Handle adcBufHandle;
 *      ADCBuf_Params adcBufParams;
 *      ADCBuf_Conversion blockingConversion;
 *      uint16_t sampleBufferOne[ADCBUFFERSIZE];
 *
 *      ADCBuf_Params_init(&adcBufParams);
 *      adcBufHandle = ADCBuf_open(CONFIG_ADCBuf0, &adcBufParams);
 *      if (adcBufHandle == NULL) {
 *          // handle error
 *      }
 *
 *      blockingConversion.arg = NULL;
 *      blockingConversion.adcChannel = CONFIG_ADCCHANNEL_A1;
 *      blockingConversion.sampleBuffer = sampleBufferOne;
 *      blockingConversion.sampleBufferTwo = NULL;
 *      blockingConversion.samplesRequestedCount = ADCBUFFERSIZE;
 *
 *      if (ADCBuf_convert(adcBufHandle, &blockingConversion, 1) != ADCBuf_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *  @endcode
 *
 * ## Using ADCBufCC26XX_ParamsExtension #
 *  This specific configuration performs one conversion on CONFIG_ADCCHANNEL_A1
 *  in ::ADCBuf_RETURN_MODE_BLOCKING. The custom parameters used here are
 *  identical to the defaults parameters. Users can of course define their own
 *  parameters.
 *  @code
 *      #include <ti/drivers/ADCBuf.h>
 *
 *      #define ADCBUFFERSIZE   100
 *
 *      ADCBuf_Handle adcBufHandle;
 *      ADCBuf_Params adcBufParams;
 *      ADCBuf_Conversion blockingConversion;
 *      uint16_t sampleBufferOne[ADCBUFFERSIZE];
 *      ADCBufCC26XX_ParamsExtension customParams;
 *
 *      ADCBuf_Params_init(&adcBufParams);
 *      customParams.samplingDuration    = ADCBufCC26XX_SAMPLING_DURATION_2P7_US;
 *      customParams.refSource           = ADCBufCC26XX_FIXED_REFERENCE;
 *      customParams.samplingMode        = ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS;
 *      customParams.inputScalingEnabled = true;
 *
 *      adcBufParams.custom = &customParams;
 *
 *      adcBufHandle = ADCBuf_open(CONFIG_ADCBuf0, &adcBufParams);
 *      if (adcBufHandle == NULL) {
 *          // handle error
 *      }
 *
 *      blockingConversion.arg = NULL;
 *      blockingConversion.adcChannel = CONFIG_ADCCHANNEL_A1;
 *      blockingConversion.sampleBuffer = sampleBufferOne;
 *      blockingConversion.sampleBufferTwo = NULL;
 *      blockingConversion.samplesRequestedCount = ADCBUFFERSIZE;
 *
 *      if (ADCBuf_convert(adcBufHandle, &blockingConversion, 1) != ADCBuf_STATUS_SUCCESS) {
 *          // handle error
 *      }
 *  @endcode
 *
 *  # Instrumentation #
 *  The ADC driver interface produces log statements if instrumentation is
 *  enabled.
 *
 *  Diagnostics Mask | Log details                          |
 *  ---------------- | -----------                          |
 *  Diags_USER1      | basic ADCBuf operations performed    |
 *  Diags_USER2      | detailed ADCBuf operations performed |
 *
 ******************************************************************************
 */

#ifndef ti_drivers_adc_adcbufcc26xx__include
#define ti_drivers_adc_adcbufcc26xx__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aux_adc.h)

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @}*/

/**
 *  @addtogroup ADCBuf_CMD
 *  ADCBufCC26XX_CMD_* macros are command codes only defined in the
 *  ADCBufCC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/adc/ADCBufCC26XX.h>
 *  @endcode
 *  @{
 */

/* Add ADCBufCC26XX_CMD_* macros here */

/*!
 *  @brief  This control function acquires the semaphore that arbitrates access
 *          to the ADC between the CM3 and the sensor controller
 *
 *  This function pre-acquires the ADC semaphore before ADCBuf_convert() is
 *  called by the application. Normally, the ADC driver acquires the ADC
 *  semaphore when calling ADCBufCC26XX_convert(). The driver may need to wait
 *  for the sensor controller to release the semaphore in order to access the
 *  ADC hardware module. Consequently, the time at which the conversion is
 *  actually made is normally non-deterministic. Pre-acquiring the semaphore
 *  makes the ADCBuf_convert() call deterministic.
 *
 *  @note This function returns an error if the handle is not open or a transfer
 *        is in progress
 */
#define ADCBufCC26XX_CMD_ACQUIRE_ADC_SEMAPHORE ADCBuf_CMD_RESERVED + 1

/*!
 *  @brief  This function makes the ADC driver keep the ADC semaphore until
 *          released
 *
 *  Calling this function will make the ADC driver keep the ADC semaphore until
 *  it is released by the application by calling the control function
 *  ADCBufCC26XX_CMD_RELEASE_ADC_SEMAPHORE. This enables multiple deterministic
 *  conversions to be made. Usually, the driver will release the semaphore after
 *  the conversion finishes.
 *
 *  @warning    The sensor controller can no longer access the ADC until the
 *              semaphore is released by the application manually.
 *
 *  @sa ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE_DISABLE
 */
#define ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE ADCBuf_CMD_RESERVED + 2

/*!
 *  @brief  This function makes the ADC driver no longer keep the ADC semaphore
 *          until released
 *
 *  This function effectively reverses a call to ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE_DISABLE.
 *
 *  @sa ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE
 */
#define ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE_DISABLE ADCBuf_CMD_RESERVED + 3

/*!
 *  @brief  This function releases the ADC semaphore
 *
 *  @note   This function returns an error if the handle is not open or a
 *          transfer is in progress
 */
#define ADCBufCC26XX_CMD_RELEASE_ADC_SEMAPHORE ADCBuf_CMD_RESERVED + 4

/** @}*/

/*!
 *  @brief Resolution in bits of the CC26XX ADC
 */
#define ADCBufCC26XX_RESOLUTION            12

#define ADCBufCC26XX_BYTES_PER_SAMPLE      2

/*
 * =============================================================================
 * Constants
 * =============================================================================
 */

/* ADCBuf function table pointer */
extern const ADCBuf_FxnTable ADCBufCC26XX_fxnTable;


/*
 * =============================================================================
 * Enumerations
 * =============================================================================
 */

/*!
 *  @brief  Specifies whether the internal reference of the ADC is sourced from
 *          the battery voltage or a fixed internal source.
 *
 *  The CC26XX ADC can operate in two different ways with regards to the
 *  sampling phase of the ADC conversion process:
 *      - It can spend a fixed amount of time sampling the signal after getting
 *        the start conversion trigger.
 *      - It can constantly keep sampling and immediately start the conversion
 *        process after getting the trigger.
 *
 *  In ADCBufCC26XX_SYNCHRONOUS mode, the ADC goes into IDLE in between
 *  conversions and uses less power. The minimum sample time for full precision
 *  in ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS is dependent on the input load.
 */
typedef enum {
    ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS,
    ADCBufCC26XX_SAMPING_MODE_ASYNCHRONOUS
} ADCBufCC26XX_Sampling_Mode;

/*!
 *  @brief  Amount of time the ADC spends sampling the analogue input.
 *
 *  The analogue to digital conversion process consists of two phases in the
 *  CC26XX ADC, the sampling and conversion phases. During the sampling phase,
 *  the ADC samples the analogue input signal. Larger input loads require longer
 *  sample times for the most accurate results.
 *  In ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS mode, this enum specifies the
 *  sampling times available.
 */
typedef enum {
    ADCBufCC26XX_SAMPLING_DURATION_2P7_US    = AUXADC_SAMPLE_TIME_2P7_US,
    ADCBufCC26XX_SAMPLING_DURATION_5P3_US    = AUXADC_SAMPLE_TIME_5P3_US,
    ADCBufCC26XX_SAMPLING_DURATION_10P6_US   = AUXADC_SAMPLE_TIME_10P6_US,
    ADCBufCC26XX_SAMPLING_DURATION_21P3_US   = AUXADC_SAMPLE_TIME_21P3_US,
    ADCBufCC26XX_SAMPLING_DURATION_42P6_US   = AUXADC_SAMPLE_TIME_42P6_US,
    ADCBufCC26XX_SAMPLING_DURATION_85P3_US   = AUXADC_SAMPLE_TIME_85P3_US,
    ADCBufCC26XX_SAMPLING_DURATION_170_US    = AUXADC_SAMPLE_TIME_170_US,
    ADCBufCC26XX_SAMPLING_DURATION_341_US    = AUXADC_SAMPLE_TIME_341_US,
    ADCBufCC26XX_SAMPLING_DURATION_682_US    = AUXADC_SAMPLE_TIME_682_US,
    ADCBufCC26XX_SAMPLING_DURATION_1P37_MS   = AUXADC_SAMPLE_TIME_1P37_MS,
    ADCBufCC26XX_SAMPLING_DURATION_2P73_MS   = AUXADC_SAMPLE_TIME_2P73_MS,
    ADCBufCC26XX_SAMPLING_DURATION_5P46_MS   = AUXADC_SAMPLE_TIME_5P46_MS,
    ADCBufCC26XX_SAMPLING_DURATION_10P9_MS   = AUXADC_SAMPLE_TIME_10P9_MS
} ADCBufCC26XX_Sampling_Duration;


/*!
 *  @brief  Specifies whether the internal reference of the ADC is sourced from
 *          the battery voltage or a fixed internal source.
 *
 *  - In practice, using the internal fixed voltage reference sets the upper
 *    range of the ADC to a fixed value. That value is 4.3V with input scaling
 *    enabled and ~1.4785V with input scaling disabled. In this mode, the output
 *    is a function of the input voltage multiplied by the resolution in
 *    alternatives (not bits) divided by the upper voltage range of the ADC.
 *    Output = Input (V) * 2^12 / (ADC range (V))
 *
 *  - Using VDDS as a reference scales the upper range of the ADC with the
 *    battery voltage. As the battery depletes and its voltage drops, so does
 *    the range of the ADC. This is helpful when measuring signals that are
 *    generated relative to the battery voltage. In this mode, the output is a
 *    function of the input voltage multiplied by the resolution in alternatives
 *    (not bits) divided by VDDS multiplied by a scaling factor derived from the
 *    input scaling. Output = Input (V) * 2^12 / (VDDS (V) * Scaling factor),
 *    where the scaling factor is ~1.4785/4.3 for input scaling disabled and 1
 *    for input scaling enabled.
 *
 *  @note   The actual reference values are slightly different for each device
 *          and are higher than the values specified above. This gain is saved
 *          in the FCFG. The function ADCBuf_convertRawToMicroVolts() must be
 *          used to derive actual voltage values. Do not attempt to compare raw
 *          values between devices or derive a voltage from them yourself. The
 *          results of doing so will only be approximately correct.
 *
 *  @warning    Even though the upper voltage range of the ADC is 4.3 volts in
 *              fixed mode with input scaling enabled, the input should never
 *              exceed VDDS as per the data sheet.
 */
typedef enum {
    ADCBufCC26XX_FIXED_REFERENCE       = AUXADC_REF_FIXED,
    ADCBufCC26XX_VDDS_REFERENCE        = AUXADC_REF_VDDS_REL
} ADCBufCC26XX_Reference_Source;



/*
 * =============================================================================
 * Structs
 * =============================================================================
 */

 /*!
 *  @brief  Table entry that maps a virtual adc channel to a dio and its
 *          corresponding internal analogue signal
 *
 *  Non-dio signals can be used as well. To do this, compBInput is set to the
 *  driverlib define corresponding to the desired non-dio signal and dio is set
 *  to PIN_UNASSIGNED.
 */
typedef struct{
    uint8_t dio;            /*!< DIO that this virtual channel is mapped to */
    uint8_t compBInput;     /*!< CompBInput that this virtual channel is mapped to */
} ADCBufCC26XX_AdcChannelLutEntry;

/*!
 *  @brief      CC26XX specfic extension to ADCBuf_Params
 *
 *  To use non-default CC26XX specific parameters when calling ADCBuf_open(),
 *  a pointer to an instance of this struct must be specified in
 *  ADCBuf_Params::custom. Alternatively, these values can be set using the
 *  control function after calling ADCBuf_open().
 */
typedef struct{
    /*! Amount of time the ADC spends sampling the analogue input */
    ADCBufCC26XX_Sampling_Duration     samplingDuration;
    /*! Specifies whether the ADC spends a fixed amount of time sampling or the
     *  entire time since the last conversion */
    ADCBufCC26XX_Sampling_Mode         samplingMode;
    /*! Specifies whether the internal reference of the ADC is sourced from the
     *  battery voltage or a fixed internal source */
    ADCBufCC26XX_Reference_Source      refSource;
    /*!
     *  Disable input scaling. Input scaling scales an external analogue
     *  signal between 0 and 4.3V to an internal signal of 0 to ~1.4785V.
     *  Since the largest permissible input to any pin is VDDS, the maximum
     *  range of the ADC is effectively less than 3.8V and continues to shrink
     *  as the battery voltage drops.
     *  With input scaling disabled, the external analogue signal is passed
     *  on directly to the internal electronics. Signals larger than ~1.4785V
     *  will damage the device with input scaling disabled.
     *
     *  | Input scaling status      | Maximum permissible ADC input voltage |
     *  |---------------------------|---------------------------------------|
     *  | Enabled                   | VDDS (Battery voltage level)          |
     *  | Disabled                  | 1.4785V                               |
     */
    bool                                inputScalingEnabled;
} ADCBufCC26XX_ParamsExtension;

/*!
 *  @brief      ADCBufCC26XX Hardware Attributes
 *
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For CC26xxWare these definitions are found in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *
 *  A sample structure is shown below:
 *  @code
 *  const ADCBufCC26XX_HWAttrs ADCBufCC26XXHWAttrs[] = {
 *      {
 *          .intPriority = ~0,
 *          .swiPriority = 0,
 *          .gpTimerUnit = CC2650_GPTIMER0A,
 *          .gptDMAChannelMask = 1 << UDMA_CHAN_TIMER0_A,
 *      }
 *  };
 *  @endcode
 */
typedef struct{
    /*! @brief ADC SWI priority.
        The higher the number, the higher the priority.
        The minimum is 0 and the maximum is 15 by default.
        The maximum can be reduced to save RAM by adding or modifying
        Swi.numPriorities in the kernel configuration file.
    */
    uint32_t            swiPriority;
    /*! @brief ADC peripheral's interrupt priority.

        The CC26xx uses three of the priority bits,
        meaning ~0 has the same effect as (7 << 5).

        (7 << 5) will apply the lowest priority.

        (1 << 5) will apply the highest priority.

        Setting the priority to 0 is not supported by this driver.

        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency
        interrupts, thus invalidating the critical sections in this driver.
    */
    uint8_t             intPriority;
    /*! Pointer to a table of ADCBufCC26XX_AdcChannelLutEntry's mapping internal
     *  CompBInput to DIO */
    ADCBufCC26XX_AdcChannelLutEntry  const *adcChannelLut;
} ADCBufCC26XX_HWAttrs;



/*!
 *  @brief      ADCBufCC26XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct{
    /* ADC control variables */
    /*!< Has the obj been opened */
    bool                            isOpen;
    /*!< Is the ADC currently doing conversions */
    bool                            conversionInProgress;
    /*!< Is the analogue input scaled */
    bool                            inputScalingEnabled;
    /*!< Should the driver keep the ADC semaphore after a conversion */
    bool                            keepADCSemaphore;
    /*!< Does the driver currently possess the ADC semaphore */
    bool                            adcSemaphoreInPossession;
    /*!< The current virtual channel the ADCBuf driver is sampling on */
    uint8_t                         currentChannel;
    /*!< Reference source for the ADC to use */
    ADCBufCC26XX_Reference_Source   refSource;
    /*!< Synchronous or asynchronous sampling mode */
    ADCBufCC26XX_Sampling_Mode      samplingMode;
    /*!< Time the ADC spends sampling in ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS */
   ADCBufCC26XX_Sampling_Duration   samplingDuration;
   /*!< Pointer to callback function */
    ADCBuf_Callback                 callbackFxn;
    /*!< Should we convert continuously or one-shot */
    ADCBuf_Recurrence_Mode          recurrenceMode;
    /*!< Mode for all conversions */
    ADCBuf_Return_Mode              returnMode;
    /*!< The last complete sample buffer used by the DMA */
    uint16_t                        *activeSampleBuffer;

    /* ADC SYS/BIOS objects */
    /*!< Hwi object */
    HwiP_Struct                      hwi;
    /*!< Swi object */
    SwiP_Struct                      swi;
    /*!< ADC semaphore */
    SemaphoreP_Struct                conversionComplete;
    /*!< Pointer to the current conversion struct */
    ADCBuf_Conversion               *currentConversion;

    /* PIN driver state object and handle */
    /*!< Pin state object */
    PIN_State                       pinState;
    /*!< Pin handle */
    PIN_Handle                      pinHandle;

    /* UDMA driver handle */
    /*!< UDMA handle */
    UDMACC26XX_Handle               udmaHandle;

    /* GPTimer driver handle */
    /*!< Handle to underlying GPTimer peripheral */
    GPTimerCC26XX_Handle            timerHandle;

    /*!< Timeout for read semaphore in ::ADCBuf_RETURN_MODE_BLOCKING */
    uint32_t                        semaphoreTimeout;
    /*!< Frequency in Hz at which the ADC is triggered */
    uint32_t                        samplingFrequency;
} ADCBufCC26XX_Object, *ADCBufCC26XX_Handle;

/*
 * =============================================================================
 * Functions
 * =============================================================================
 */


#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_adc_ADCBufCC26XX__include */
