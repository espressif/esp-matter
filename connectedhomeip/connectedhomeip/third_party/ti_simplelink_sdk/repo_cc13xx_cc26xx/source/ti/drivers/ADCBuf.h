/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file       ADCBuf.h
 *  @brief      Analog to Digital Conversion Buffer (ADCBuf) Input Driver
 *
 *  @anchor ti_drivers_ADCBuf_Overview
 *  # Overview
 *
 *  The ADCBuf driver allows you to sample and convert analog signals
 *  at a specified frequency. The resulting samples are placed in
 *  a buffer provided by the application. The driver can either take @p N
 *  samples once or continuously sample by double-buffering and providing a
 *  callback to process each finished buffer.
 *
 *  <hr>
 *  @anchor ti_drivers_ADCBuf_Usage
 *  # Usage
 *
 *  This documentation provides a basic @ref ti_drivers_ADCBuf_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_ADCBuf_Examples "examples"
 *  in the form of commented code fragments. Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor ti_drivers_ADCBuf_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_ADCBuf_Synopsis_Code
 *  @code
 *  // Import ADCBuf Driver definitions
 *  #include <ti/drivers/ADCBuf.h>
 *
 *  // Define name for ADCBuf channel index
 *  #define PIEZOMETER_OUT    0
 *
 *  // Create buffer for samples
 *  #define ADCBUFFERSIZE     10
 *  uint16_t buffer[ADCBUFFERSIZE];
 *  uint32_t microvoltBuffer[ADCBUFFERSIZE];
 *
 *  // One time init of ADCBuf driver
 *  ADCBuf_init();
 *
 *  // Initialize optional ADCBuf parameters
 *  ADCBuf_Params params;
 *  ADCBuf_Params_init(&params);
 *  params.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
 *  params.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
 *
 *  // Open ADCBuf driver
 *  adcBuf = ADCBuf_open(CONFIG_ADCBUF0, &params);
 *
 *  // Setup conversion structure
 *  ADCBuf_Conversion conversion = {0};
 *  conversion.samplesRequestedCount = ADCBUFFERSIZE;
 *  conversion.sampleBuffer = buffer;
 *  conversion.adcChannel = PIEZOMETER_OUT;
 *
 *  // Start ADCBuf conversion
 *  ADCBuf_convert(adcBuf, &conversion, 1)
 *
 *  // Adjust raw ADC values and convert them to microvolts
 *  ADCBuf_adjustRawValues(handle, buffer, ADCBUFFERSIZE, PIEZOMETER_OUT);
 *  ADCBuf_convertAdjustedToMicroVolts(handle, PIEZOMETER_OUT, buffer,
 *                                     microvoltBuffer, ADCBUFFERSIZE);
 *
 *  // Close ADCBuf driver
 *  ADCBuf_close(adcbuf);
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_ADCBuf_Examples
 *  # Examples
 *
 *  @li @ref ti_drivers_ADCBuf_Examples_open "Opening an ADCBuf instance"
 *  @li @ref ti_drivers_ADCBuf_Examples_blocking "Using a blocking conversion"
 *  @li @ref ti_drivers_ADCBuf_Examples_callback "Using a callback conversion"
 *
 *  @anchor ti_drivers_ADCBuf_Examples_open
 *  ## Opening an ADCBuf instance
 *
 *  @code
 *  ADCBuf_Handle adcBufHandle;
 *  ADCBuf_Params adcBufParams;
 *
 *  ADCBuf_init();
 *
 *  ADCBuf_Params_init(&adcBufParams);
 *
 *  adcBufHandle = ADCBuf_open(0, &adcBufParams);
 *  if (adcBufHandle == NULL)
 *  {
 *      //ADCBuf_open() failed.
 *      while (1) {}
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_ADCBuf_Examples_blocking
 *  ## Using a blocking conversion
 *
 *  @code
 *  ADCBuf_Handle adcbuf;
 *  ADCBuf_Params params;
 *
 *  ADCBuf_init();
 *
 *  ADCBuf_Params_init(&params);
 *  params.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
 *  params.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
 *  adcbuf = ADCBuf_open(0, &params);
 *  if (adcbuf != NULL)
 *  {
 *      ADCBuf_Conversion conversion = {0};
 *      conversion.adcChannel = PIEZOMETER_OUT;
 *      conversion.sampleBuffer = buffer;
 *      conversion.samplesRequestedCount = ADCBUFFERSIZE;
 *
 *      if (ADCBuf_convert(adcbuf, &conversion, 1) != ADCBuf_STATUS_SUCCESS)
 *      {
 *          // ADCBuf_conver() failed
 *      }
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_ADCBuf_Examples_callback
 *  ## Using a callback conversion
 *
 *  @code
 *  // ADCBuf callback function
 *  void adcBufCallbackFxn(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
 *                         void *buffer, uint32_t channel, int_fast16_t status);
 *
 *  main()
 *  {
 *      ADCBuf_Handle adcbuf;
 *      ADCBuf_Params params;
 *
 *      ADCBuf_init();
 *
 *      ADCBuf_Params_init(&params);
 *      params.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
 *      params.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
 *      params.callbackFxn = adcBufCallbackFxn;
 *      adcbuf = ADCBuf_open(0, &params);
 *
 *      ADCBuf_Conversion conversion = {0};
 *      conversion.adcChannel = PIEZOMETER_OUT;
 *      conversion.sampleBuffer = buffer;
 *      conversion.samplesRequestedCount = ADCBUFFERSIZE;
 *
 *      if (ADCBuf_convert(adcbuf, &conversion, 1) != ADCBuf_STATUS_SUCCESS)
 *      {
 *          // ADCBuf_convert() failed
 *      }
 *
 *      // Pend on a semaphore
 *  }
 *
 *  void adcBufCallbackFxn(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
 *      void *buffer, uint32_t channel, int_fast16_t status)
 *  {
 *      // Adjust raw ADC values and convert them to microvolts
 *      ADCBuf_adjustRawValues(handle, buffer, ADCBUFFERSIZE,
 *          channel);
 *      ADCBuf_convertAdjustedToMicroVolts(handle, channel,
 *          buffer, microvoltBuffer, ADCBUFFERSIZE);
 *
 *      // Post a semaphore
 *  }
 *
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_ADCBuf_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 ******************************************************************************
 */

#ifndef ti_drivers_adcbuf__include
#define ti_drivers_adcbuf__include

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @defgroup ADCBuf_CONTROL ADCBuf_control command and status codes
 *  These ADCBuf macros are reservations for ADCBuf.h
 *  @{
 */

/*!
 * Common ADCBuf_control command code reservation offset.
 * ADC driver implementations should offset command codes with
 * ADCBuf_CMD_RESERVED growing positively
 *
 * Example implementation specific command codes:
 * @code
 * #define ADCXYZ_COMMAND0         ADCBuf_CMD_RESERVED + 0
 * #define ADCXYZ_COMMAND1         ADCBuf_CMD_RESERVED + 1
 * @endcode
 */
#define ADCBuf_CMD_RESERVED             (32)

/*!
 * Common ADCBuf_control status code reservation offset.
 * ADC driver implementations should offset status codes with
 * ADCBuf_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define ADCXYZ_STATUS_ERROR0    ADCBuf_STATUS_RESERVED - 0
 * #define ADCXYZ_STATUS_ERROR1    ADCBuf_STATUS_RESERVED - 1
 * #define ADCXYZ_STATUS_ERROR2    ADCBuf_STATUS_RESERVED - 2
 * @endcode
 */
#define ADCBuf_STATUS_RESERVED          (-32)

/*!
 * @brief  Success status code returned by:
 * ADCBuf_control()
 *
 * Functions return ADCBuf_STATUS_SUCCESS if the call was executed
 * successfully.
 *  @{
 *  @ingroup ADCBuf_CONTROL
 */
#define ADCBuf_STATUS_SUCCESS           (0)

/*!
 * @brief   Generic error status code returned by ADCBuf_control().
 *
 * ADCBuf_control() returns #ADCBuf_STATUS_ERROR if the control code was
 * not executed successfully.
 */
#define ADCBuf_STATUS_ERROR             (-1)

/*!
 * @brief   An error status code returned by ADCBuf_control() for undefined
 * command codes.
 *
 * ADCBuf_control() returns ADCBuf_STATUS_UNDEFINEDCMD if the control code is
 * not recognized by the driver implementation.
 */
#define ADCBuf_STATUS_UNDEFINEDCMD      (-2)

/*!
 * @brief   An error status code returned if the function is not supported
 *          by a particular driver implementation.
 */
#define ADCBuf_STATUS_UNSUPPORTED      (-3)
/** @}*/

/**
 *  @defgroup ADCBuf_CMD Command Codes
 *  ADCBuf_CMD_* macros are general command codes for I2C_control(). Not all
 *  ADCBuf driver implementations support these command codes.
 *  @{
 *  @ingroup ADCBuf_CONTROL
 */

/* Add ADCBuf_CMD_<commands> here */

/** @}*/

/** @}*/


/*!
 *  @brief    A handle that is returned from an ADCBuf_open() call.
 */
typedef struct ADCBuf_Config_ *ADCBuf_Handle;

/*!
 *  @brief  Defines a conversion to be used with ADCBuf_convert().
 *
 *  @sa ADCBuf_convert()
 *  @sa #ADCBuf_Recurrence_Mode
 *  @sa #ADCBuf_Return_Mode
 */
typedef struct
{
    /*!
     * Defines the number of samples to be performed on the
     * ADCBuf_Conversion.channel. The application buffers provided by
     * #ADCBuf_Conversion.sampleBuffer and #ADCBuf_Conversion.sampleBufferTwo
     * must be large enough to hold @p samplesRequestedCount samples.
     */
    uint16_t samplesRequestedCount;

    /*!
     * Buffer to store ADCBuf conversion results. This buffer must be at least
     * (#ADCBuf_Conversion.samplesRequestedCount * 2) bytes. When using
     * #ADCBuf_RECURRENCE_MODE_ONE_SHOT, only this buffer is used.
     */
    void     *sampleBuffer;

    /*!
     * Buffer to store ADCBuf conversion results. This buffer must be at least
     * (#ADCBuf_Conversion.samplesRequestedCount * 2) bytes. When using
     * #ADCBuf_RECURRENCE_MODE_ONE_SHOT, this buffer is not used. When
     * using #ADCBuf_RECURRENCE_MODE_CONTINUOUS, this must point to
     * a valid buffer.
     *
     * @sa  #ADCBuf_RECURRENCE_MODE_CONTINUOUS
     */
    void     *sampleBufferTwo;

    /*!
     * Pointer to a custom argument to be passed to the #ADCBuf_Callback
     * function via the #ADCBuf_Conversion structure.
     *
     * @note The #ADCBuf_Callback function is only called when operating in
     * #ADCBuf_RETURN_MODE_CALLBACK.
     *
     * @sa  #ADCBuf_RETURN_MODE_CALLBACK
     * @sa  #ADCBuf_Callback
     */
    void     *arg;

    /*!
     * ADCBuf channel to perform conversions on. Mapping of channel to pin or
     * internal signal is device specific. Refer to the device specific
     * implementation.
     */
    uint32_t adcChannel;
} ADCBuf_Conversion;

/*!
 *  @brief  The definition of a callback function.
 *
 *  When operating in #ADCBuf_RETURN_MODE_CALLBACK, the callback function
 *  is called when an #ADCBuf_Conversion completes. The application is
 *  responsible for declaring a #ADCBuf_Callback and providing a pointer
 *  in #ADCBuf_Params.callbackFxn.
 *
 *  @warning  The callback function is called from an interrupt context.
 *
 *  @param[out]  handle    #ADCBuf_Handle used with the initial call to
 *  ADCBuf_convert()
 *
 *  @param[out]  conversion    Pointer to the #ADCBuf_Conversion structure used
 *  with the initial call to ADCBuf_convert(). This structure also contains
 *  the custom argument specified by @p conversion.arg.
 *
 *  @param[out]  completedADCBuffer    Pointer to a buffer containing
 *  @p conversion.samplesRequestedCount ADC samples.
 *
 *  @param[out]  completedChannel    ADCBuf channel the samples were
 *  performed on.
 *
 *  @param[out]  status    Status of the ADCBuf driver during an interrupt.
 *
 *  @sa ADCBuf_convert()
 *  @sa ADCBuf_Conversion
 *  @sa ADCBuf_Recurrence_Mode
 *  @sa ADCBuf_RETURN_MODE_CALLBACK
 */
typedef void (*ADCBuf_Callback) (ADCBuf_Handle handle,
                                 ADCBuf_Conversion *conversion,
                                 void *completedADCBuffer,
                                 uint32_t completedChannel,
                                 int_fast16_t status);

/*!
 *  @brief  Recurrence behavior of a #ADCBuf_Conversion specified in the
 *  #ADCBuf_Params.
 *
 *  This enumeration defines the recurrence mode of a #ADCBuf_Conversion.
 *  After a call to ADCBuf_convert(), #ADCBuf_Conversion may either be done
 *  once or reoccur.
 */
typedef enum
{
    /*!
      *  When operating in #ADCBuf_RECURRENCE_MODE_ONE_SHOT, calls to
      *  ADCBuf_convert() will pend on a semaphore until
      *  #ADCBuf_Conversion.samplesRequestedCount samples are completed or
      *  after a duration of #ADCBuf_Params.blockingTimeout.
      *
      *  @note  When using #ADCBuf_RECURRENCE_MODE_ONE_SHOT, ADCBuf_convert()
      *  must be called from a thread context. #ADCBuf_RECURRENCE_MODE_ONE_SHOT
      *  can only be used in combination with #ADCBuf_RETURN_MODE_BLOCKING.
     */
    ADCBuf_RECURRENCE_MODE_ONE_SHOT,

    /*!
     *  When operating in #ADCBuf_RECURRENCE_MODE_CONTINUOUS, calls to
     *  ADCBuf_convert() will return immediately. The driver will continuously
     *  perform #ADCBuf_Conversion.samplesRequestedCount samples and call the
     *  #ADCBuf_Callback function when completed. The driver
     *  will automatically alternate between #ADCBuf_Conversion.sampleBuffer
     *  and #ADCBuf_Conversion.sampleBufferTwo. A
     *  #ADCBuf_RECURRENCE_MODE_CONTINUOUS conversion can only be terminated
     *  using ADCBuf_convertCancel().
     *
     *  @note  #ADCBuf_RECURRENCE_MODE_CONTINUOUS can only be used in
     *  combination with #ADCBuf_RETURN_MODE_CALLBACK.
     *
     *  @sa  #ADCBuf_RETURN_MODE_CALLBACK
     *  @sa  ADCBuf_convertCancel()
     */
    ADCBuf_RECURRENCE_MODE_CONTINUOUS
} ADCBuf_Recurrence_Mode;

/*!
 *  @brief  Return behavior for ADCBuf_convert() specified in the
 *  #ADCBuf_Params
 *
 *  This enumeration defines the return behavior for ADCBuf_convert().
 *  A call to ADCBuf_convert() may either block or return immediately.
 *
 *  @sa  ADCBuf_convert
 */
typedef enum
{
    /*!
      *  When operating in #ADCBuf_RETURN_MODE_BLOCKING, calls to
      *  ADCBuf_convert() will pend on a semaphore until
      *  #ADCBuf_Conversion.samplesRequestedCount samples are completed or
      *  after a duration of #ADCBuf_Params.blockingTimeout.
      *
      *  @note  When using #ADCBuf_RETURN_MODE_BLOCKING, ADCBuf_convert()
      *  must be called from a thread context. #ADCBuf_RETURN_MODE_BLOCKING
      *  can only be used in combination with #ADCBuf_RECURRENCE_MODE_ONE_SHOT.
      */
    ADCBuf_RETURN_MODE_BLOCKING,

    /*!
     *  When operating in #ADCBuf_RETURN_MODE_CALLBACK, calls to
     *  ADCBuf_convert() will return immediately. When
     *  #ADCBuf_Conversion.samplesRequestedCount samples are completed,
     *  the #ADCBuf_Params.callbackFxn function is called.
     *
     *  @note  #ADCBuf_RECURRENCE_MODE_CONTINUOUS can only be used in
     *  combination with #ADCBuf_RETURN_MODE_CALLBACK.
     *
     *  @sa #ADCBuf_RECURRENCE_MODE_CONTINUOUS
     */
    ADCBuf_RETURN_MODE_CALLBACK
} ADCBuf_Return_Mode;

/*!
 *  @brief ADCBuf parameters used with ADCBuf_open().
 *
 *  #ADCBuf_Params_init() must be called prior to setting fields in
 *  this structure.
 *
 *  @sa  ADCBuf_Params_init()
 */
typedef struct
{
    /*!
     *  Timeout in system clock ticks. This value is only valid when using
     *  #ADCBuf_RETURN_MODE_BLOCKING. A call to ADCBuf_convert() will block
     *  for a duration up to @p blockingTimeout ticks. The call to
     *  ADCBuf_convert() will return prior if the requested number of samples
     *  in #ADCBuf_Conversion.samplesRequestedCount are completed. The
     *  @p blockingTimeout should be large enough to allow for
     *  #ADCBuf_Conversion.samplesRequestedCount samples to be collected
     *  given the #ADCBuf_Params.samplingFrequency.
     *
     *  @sa #ADCBuf_RETURN_MODE_BLOCKING
     */
    uint32_t               blockingTimeout;

    /*!
     *  The frequency at which the ADC will sample in Hertz (Hz). After a
     *  call to ADCBuf_convert(), the ADC will perform @p samplingFrequency
     *  samples per second.
     */
    uint32_t               samplingFrequency;

    /*! #ADCBuf_Return_Mode for all conversions. */
    ADCBuf_Return_Mode     returnMode;

    /*!
     *  Pointer to a #ADCBuf_Callback function to be invoked after a
     *  conversion completes when operating in #ADCBuf_RETURN_MODE_CALLBACK.
     */
    ADCBuf_Callback        callbackFxn;

    /*! #ADCBuf_Recurrence_Mode for all conversions. */
    ADCBuf_Recurrence_Mode recurrenceMode;

    /*! Pointer to a device specific extension of the #ADCBuf_Params */
    void *custom;
} ADCBuf_Params;

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_close().
 */
typedef void (*ADCBuf_CloseFxn) (ADCBuf_Handle handle);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_open().
 */
typedef ADCBuf_Handle (*ADCBuf_OpenFxn) (ADCBuf_Handle handle,
                                         const ADCBuf_Params *params);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_control().
 */
typedef int_fast16_t (*ADCBuf_ControlFxn) (ADCBuf_Handle handle,
                                           uint_fast8_t cmd,
                                           void *arg);
/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_init().
 */
typedef void (*ADCBuf_InitFxn) (ADCBuf_Handle handle);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_convert().
 */
typedef int_fast16_t (*ADCBuf_ConvertFxn) (ADCBuf_Handle handle,
                                           ADCBuf_Conversion conversions[],
                                           uint_fast8_t channelCount);
/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_convertCancel().
 */
typedef int_fast16_t (*ADCBuf_ConvertCancelFxn)(ADCBuf_Handle handle);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_GetResolution();
 */
typedef uint_fast8_t (*ADCBuf_GetResolutionFxn) (ADCBuf_Handle handle);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_adjustRawValues();
 */
typedef int_fast16_t (*ADCBuf_adjustRawValuesFxn)(ADCBuf_Handle handle,
                                                  void *sampleBuffer,
                                                  uint_fast16_t sampleCount,
                                                  uint32_t adcChannel);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADCBuf_convertAdjustedToMicroVolts();
 */
typedef int_fast16_t (*ADCBuf_convertAdjustedToMicroVoltsFxn) (
                                               ADCBuf_Handle handle,
                                               uint32_t adcChannel,
                                               void *adjustedSampleBuffer,
                                               uint32_t outputMicroVoltBuffer[],
                                               uint_fast16_t sampleCount);

/*!
 *  @brief      The definition of an ADCBuf function table that contains the
 *              required set of functions to control a specific ADC driver
 *              implementation.
 */
typedef struct
{
    /*! Function to close the specified peripheral */
    ADCBuf_CloseFxn                       closeFxn;
    /*! Function to driver implementation specific control function */
    ADCBuf_ControlFxn                     controlFxn;
    /*! Function to initialize the given data object */
    ADCBuf_InitFxn                        initFxn;
    /*! Function to open the specified peripheral */
    ADCBuf_OpenFxn                        openFxn;
    /*! Function to start an ADC conversion with the specified peripheral */
    ADCBuf_ConvertFxn                     convertFxn;
    /*! Function to abort a conversion being carried out by the specified
        peripheral */
    ADCBuf_ConvertCancelFxn               convertCancelFxn;
    /*! Function to get the resolution in bits of the ADC */
    ADCBuf_GetResolutionFxn               getResolutionFxn;
    /*! Function to adjust raw ADC return bit values to values comparable
        between devices of the same type */
    ADCBuf_adjustRawValuesFxn             adjustRawValuesFxn;
    /*! Function to convert adjusted ADC values to microvolts */
    ADCBuf_convertAdjustedToMicroVoltsFxn convertAdjustedToMicroVoltsFxn;
} ADCBuf_FxnTable;

/*!
 *  @brief ADC driver's custom @ref driver_configuration "configuration"
 *  structure.
 *
 *  @sa     ADCBuf_init()
 *  @sa     ADCBuf_open()
 */
typedef struct ADCBuf_Config_
{
    /*! Pointer to a @ref driver_function_table "function pointer table"
     *  with driver-specific implementations of ADC APIs */
    const ADCBuf_FxnTable *fxnTablePtr;

    /*! Pointer to a driver specific @ref driver_objects "data object". */
    void                  *object;

    /*! Pointer to a driver specific @ref driver_hardware_attributes
     *  "hardware attributes structure". */
    void const            *hwAttrs;
} ADCBuf_Config;

/*!
 *  @brief  Function to close an ADCBuf driver instance
 *
 *  @pre    ADCBuf_open() has to be called first.
 *
 *  @pre    In #ADCBuf_RECURRENCE_MODE_CONTINUOUS, the application must call
 *          ADCBuf_convertCancel() first.
 *
 *  @param[in]  handle    An #ADCBuf_Handle returned from ADCBuf_open()
 *
 */
extern void ADCBuf_close(ADCBuf_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a
 *          driver instance.
 *
 *  @pre    ADCBuf_open() has to be called first.
 *
 *  @param[in]  handle  An #ADCBuf_Handle returned from ADCBuf_open()
 *
 *  @param[in]  cmd     A command value defined by the device specific
 *                      implementation
 *
 *  @param[in]  cmdArg  An optional R/W (read/write) argument that is
 *                      accompanied with @p cmd
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @retval #ADCBuf_STATUS_SUCCESS      The call was successful.
 *
 *  @retval #ADCBuf_STATUS_UNDEFINEDCMD The @p cmd value is not supported by
 *                                      the device specific implementation.
 */
extern int_fast16_t ADCBuf_control(ADCBuf_Handle handle,
                                   uint_fast16_t cmd,
                                   void *cmdArg);

/*!
 *  @brief  Function to initialize the ADCBuf driver.
 *
 *  This function must also be called before any other ADCBuf driver APIs.
 */
extern void ADCBuf_init(void);

/*!
 *  @brief  Initialize an #ADCBuf_Params structure to its default values.
 *
 *  @param[in]  params  A pointer to #ADCBuf_Params structure for
 *                      initialization
 *
 *  Default values are:
 *  @arg #ADCBuf_Params.returnMode        = #ADCBuf_RETURN_MODE_BLOCKING,
 *  @arg #ADCBuf_Params.blockingTimeout   = 25000,
 *  @arg #ADCBuf_Params.callbackFxn       = NULL,
 *  @arg #ADCBuf_Params.recurrenceMode    = #ADCBuf_RECURRENCE_MODE_ONE_SHOT,
 *  @arg #ADCBuf_Params.samplingFrequency = 10000,
 *  @arg #ADCBuf_Params.custom            = NULL
 */
extern void ADCBuf_Params_init(ADCBuf_Params *params);

/*!
 *  @brief  This function opens a given ADCBuf peripheral.
 *
 *  @param[in]  index   Index in the @p ADCBuf_Config[] array.
 *
 *  @param[in]  params  Pointer to an initialized #ADCBuf_Params structure.
 *                      If NULL, the default #ADCBuf_Params values are used.
 *
 *  @return An #ADCBuf_Handle on success or NULL on error.
 *
 *  @sa     ADCBuf_close()
 */
extern ADCBuf_Handle ADCBuf_open(uint_least8_t index, ADCBuf_Params *params);

/*!
 *  @brief  Starts ADCBuf conversions on one or more channels.
 *
 *  @note When using #ADCBuf_RETURN_MODE_BLOCKING, this must be called from a
 *        thread context.
 *
 *  @param[in]  handle          An ADCBuf handle returned from ADCBuf_open()
 *
 *  @param[in]  conversions     A pointer to an array of #ADCBuf_Conversion
 *                              structures.
 *
 *  @param[in]  channelCount    The number of channels to convert on in this
 *  call. Should be the length of the @p conversions array. Depending on the
 *  device, multiple simultaneous conversions may not be supported. See device
 *  specific implementation.
 *
 *  @retval #ADCBuf_STATUS_SUCCESS  The conversion was successful.
 *  @retval #ADCBuf_STATUS_ERROR    The conversion failed.
 *
 *  @pre    ADCBuf_open() must have been called.
 *
 *  @sa     ADCBuf_convertCancel()
 *  @sa     ADCBuf_Return_Mode
 *  @sa     ADCBuf_Recurrence_Mode
 *  @sa     ADCBuf_Conversion
 */
extern int_fast16_t ADCBuf_convert(ADCBuf_Handle handle,
                                   ADCBuf_Conversion conversions[],
                                   uint_fast8_t channelCount);

/*!
 *  @brief  Cancels all ADCBuf conversions in progress.
 *
 *  @param[in]  handle    An #ADCBuf_Handle returned from ADCBuf_open()
 *
 *  @retval #ADCBuf_STATUS_SUCCESS  The cancel was successful.
 *  @retval #ADCBuf_STATUS_ERROR    The cancel failed.
 *
 *  @sa     ADCBuf_convert()
 */
extern int_fast16_t ADCBuf_convertCancel(ADCBuf_Handle handle);

/*!
 *  @brief  Returns the resolution in bits of the specified ADCBuf instance.
 *
 *  @param[in]  handle    An #ADCBuf_Handle returned from ADCBuf_open().
 *
 *  @return The resolution in bits of the specified ADC.
 *
 *  @pre    ADCBuf_open() must have been called prior.
 */
extern uint_fast8_t ADCBuf_getResolution(ADCBuf_Handle handle);

 /*!
 *  @brief  Adjust a raw ADC output buffer. The function does
 *          the adjustment in-place.
 *
 *  @param[in]  handle          An ADCBuf_Handle returned from ADCBuf_open().
 *
 *  @param[in,out]  sampleBuf   A buffer full of raw sample values.
 *
 *  @param[in]  sampleCount     The number of samples to adjust.
 *
 *  @param[in]  adcChan         The channel the buffer was sampled on.
 *
 *  @retval #ADCBuf_STATUS_SUCCESS The operation was successful.
 *          @p sampleBuf contains valid values.
 *
 *  @retval #ADCBuf_STATUS_ERROR if an error occurred.
 *
 *  @retval #ADCBuf_STATUS_UNSUPPORTED The function is not supported by the
 *                                     device specific implementation.
 *
 *  @pre    ADCBuf_convert() must have returned a valid buffer with samples.
 */
extern int_fast16_t ADCBuf_adjustRawValues(ADCBuf_Handle handle,
                                           void *sampleBuf,
                                           uint_fast16_t sampleCount,
                                           uint32_t adcChan);

 /*!
 *  @brief  Convert an adjusted ADC output buffer to microvolts.
 *
 *  @param[in]  handle    An ADCBuf_Handle returned from ADCBuf_open()
 *
 *  @param[in]  adcChan    The ADC channel the samples were performed on.
 *
 *  @param[in]  adjustedSampleBuffer    A buffer full of adjusted samples.
 *
 *  @param[in,out]  outputMicroVoltBuffer    The output buffer.
 *
 *  @param[in]  sampleCount    The number of samples to convert.
 *
 *  @retval #ADCBuf_STATUS_SUCCESS    The operation was successful.
 *          @p outputMicroVoltBuffer contains valid values.
 *
 *  @retval  #ADCBuf_STATUS_ERROR    The operation failed.
 *
 *  @pre  ADCBuf_adjustRawValues() must be called on @p adjustedSampleBuffer.
 */
extern int_fast16_t ADCBuf_convertAdjustedToMicroVolts(
                                            ADCBuf_Handle handle,
                                            uint32_t  adcChan,
                                            void *adjustedSampleBuffer,
                                            uint32_t outputMicroVoltBuffer[],
                                            uint_fast16_t sampleCount);

#ifdef __cplusplus
}
#endif
#endif /* ti_drivers_adcbuf__include */
