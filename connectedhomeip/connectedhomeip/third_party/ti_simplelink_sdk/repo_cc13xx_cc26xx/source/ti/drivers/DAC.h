/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
 *  @file       DAC.h
 *  @brief      Digital to Analog Conversion (DAC) Output Driver
 *
 *  @anchor ti_drivers_DAC_Overview
 *  # Overview
 *
 *  The DAC driver allows you to manage a Digital to Analog peripheral via
 *  simple and portable APIs. This driver supports converting DAC codes
 *  into an analog voltage with varying degrees of precision (microvolts
 *  or millivolts) depending on the resolution of the DAC.
 *
 *  <hr>
 *  @anchor ti_drivers_DAC_Usage
 *  # Usage
 *
 *  This documentation provides a basic @ref ti_drivers_DAC_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_DAC_Examples "examples"
 *  in the form of commented code fragments. Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor ti_drivers_DAC_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_DAC_Synopsis_Code
 *  @code
 *  // Import DAC Driver definitions
 *  #include <ti/drivers/DAC.h>
 *
 *  // Define name for DAC channel index
 *  #define VREFERENCE_OUT  0
 *
 *  // One-time init of DAC driver
 *  DAC_init();
 *
 *  // Initialize optional DAC parameters
 *  DAC_Params params;
 *  DAC_Params_init(&params);
 *
 *  // Open DAC channels for usage
 *  DAC_Handle dacHandle = DAC_open(VREFERENCE_OUT, &params);

 *  // Enable the DAC channel
 *  DAC_enable(dacHandle);
 *
 *  // Output 800mV using the DAC
 *  DAC_setVoltage(dacHandle, 800000);
 *
 *  // Output a voltage using DAC codes
 *  DAC_setCode(dacHandle, 125);
 *
 *  // Disable the DAC channel
 *  DAC_disable(dacHandle);
 *
 *  // Close the DAC channel
 *  DAC_close(dacHandle);
 *
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_DAC_Examples
 *  # Examples
 *
 *  @li @ref ti_drivers_DAC_Examples_open "Opening a DAC instance"
 *  @li @ref ti_drivers_DAC_Examples_reference "Using DAC to set a reference voltage"
 *  @li @ref ti_drivers_DAC_Examples_code "Using DAC to set a code"
 *
 *  @anchor ti_drivers_DAC_Examples_open
 *  ## Opening a DAC instance
 *
 *  @code
 *  DAC_Handle dac;
 *  DAC_Params params;
 *
 *  DAC_Params_init(&params);
 *
 *  dac = DAC_open(0, &params);
 *  if (dac == NULL) {
 *      // DAC_open() failed
 *      while (1) {}
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_DAC_Examples_reference
 *  ## Using DAC to set a reference voltage
 *
 *  To set a reference voltage first enable the DAC with DAC_enable()
 *  and then set the desired output voltage with DAC_setVoltage().
 *  The returned value can be used to check if the call was successful.
 *
 *  @code
 *  int_fast16_t res;
 *  uint32_t outputMicroVolts = 1500000;
 *
 *  DAC_enable(dac);
 *  res = DAC_setVoltage(dac, outputMicroVolts);
 *  if (res == DAC_STATUS_SUCCESS)
 *  {
 *      printf(outputMicroVolts);
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_DAC_Examples_code
 *  ## Using DAC to set a code
 *
 *  The following example shows how to set a DAC code. The use of DAC_setCode()
 *  instead of DAC_setVoltage() comes handy in scenarios such as outputting a
 *  data buffer at a particular time rate (e.g. waveform generation). Nevertheless,
 *  it's important to remember that DAC codes produce different output voltages
 *  depending on the selected voltage reference source.
 *
 *  @code
 *  int_fast16_t res;
 *  uint32_t outputCode = 127;
 *  DAC_enable(dac);
 *  res = DAC_setCode(dac, outputCode);
 *  if (res == DAC_STATUS_SUCCESS)
 *  {
 *      printf(outputCode);
 *  }
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_DAC_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 ******************************************************************************
 */

#ifndef ti_drivers_DAC__include
#define ti_drivers_DAC__include

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aux_dac.h)

#ifdef __cplusplus
extern "C" {
#endif



 /*!
  * @brief  Successful status code returned by DAC API.
  */
 #define DAC_STATUS_SUCCESS            (0)

 /*!
  * @brief  Generic error status code returned by DAC API.
  */
 #define DAC_STATUS_ERROR              (-1)

 /*!
  * @brief  The DAC is currently in use by another handle or by the sensor controller.
  */
 #define DAC_STATUS_INUSE             (-2)

 /*!
  * @brief  The desired output value is outside the DAC's output range.
  */
 #define DAC_STATUS_INVALID           (-3)

 /*! @brief  DAC Global configuration
  *
  *  The DAC_Config structure contains a set of pointers used to characterize
  *  the DAC driver implementation.
  *
  *  This structure needs to be defined before calling DAC_init() and it must
  *  not be changed thereafter.
  *
  *  @sa     DAC_init()
  */
typedef struct {
    /*! Pointer to a driver specific data object */
    void                   *object;
    /*! Pointer to a driver specific hardware attributes structure */
    void          const    *hwAttrs;
} DAC_Config;

/*!
 *  @brief      A handle that is returned from a DAC_open() call.
 */
typedef DAC_Config *DAC_Handle;

/*!
 *  @brief    Basic DAC Parameters
 *
 *  DAC parameters used with the DAC_open() call. Default values for these
 *  parameters are set using DAC_Params_init().
 *
 *  @sa       DAC_Params_init()
 */
typedef struct {
    /*!< Initial DAC code */
    uint32_t               initCode;
    /*!< Pointer to device specific custom params */
    void                  *custom;
} DAC_Params;

/*!
 *  @brief Default DAC_Params structure
 *
 *  @sa     DAC_Params_init()
 */
extern const DAC_Params DAC_defaultParams;

/*!
 *  @brief  Function to initialize the DAC module.
 *
 *  @pre    The DAC_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other DAC driver APIs. This function call does not modify any
 *          peripheral registers.
 */
extern void DAC_init(void);

/*!
 *  @brief  Function to initialize a given DAC peripheral specified by the
 *          particular index value.
 *
 *  @pre    DAC controller has been initialized
 *
 *  @param  [inout] index   Logical peripheral number for the DAC indexed into
 *                          the DAC_config table
 *
 *  @param  [in]    params  Pointer to a parameter block.
 *                          All the fields in this structure are RO (read-only).
 *                          Providing a NULL pointer cannot open the module.
 *
 *  @return A DAC_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     DAC_init()
 *  @sa     DAC_close()
 */
extern DAC_Handle DAC_open(uint_least8_t index, DAC_Params *params);

/*!
 *  @brief  Function to close a given DAC peripheral specified by the DAC
 *  handle.
 *
 *  @pre    DAC_open() had to be called first.
 *
 *  @param  [in]    handle  A DAC_Handle returned from DAC_open
 *
 *  @sa     DAC_open()
 */
extern void DAC_close(DAC_Handle handle);

/*!
 *  @brief  Function to set the DAC voltage value in microvolts.
 *
 *  @warning This function takes a value in microvolts, but the actual
 *  output is limited by the DAC's resolution.
 *
 *  @pre    DAC_enable() has to be called first.
 *
 *  @param  [in]    handle        A DAC_Handle returned from DAC_open.
 *
 *  @param [in]     uVoltOutput   Desired output voltage in uV.
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @retval #DAC_STATUS_SUCCESS The call was successful.
 *  @retval #DAC_STATUS_ERROR The call was unsuccessful.
 *  @retval #DAC_STATUS_INVALID The desired output voltage was out of bounds
 *                              for the DAC.
 *
 *  @sa     DAC_setCode()
 */
extern int_fast16_t DAC_setVoltage(DAC_Handle handle, uint32_t uVoltOutput);

/*!
 *  @brief  Function to set the DAC voltage in terms of a DAC code.
 *
 *  @warning The appropriate DAC code is device-specific and admissible values
 *  depend on the DAC's resolution.
 *
 *  @pre    DAC_open() has to be called first.
 *
 *  @param  [in]    handle        A DAC_Handle returned from DAC_open.
 *
 *  @param [in]     code          Desired output in terms of a DAC code.
 *
 *  @retval #DAC_STATUS_SUCCESS The call was successful.
 *  @retval #DAC_STATUS_ERROR The call was unsuccessful.
 *
 *
 *  @sa     DAC_open()
 */
extern int_fast16_t DAC_setCode(DAC_Handle handle, uint32_t code);

/*!
 *  @brief  Function to enable the DAC's output.
 *
 *  This function claims ownership of the DAC peripheral, and depending on
 *  the selected voltage reference source, it calculates the output voltage
 *  range.
 *
 *  Furthermore, the function configures and enables the DAC.
 *
 *  @pre    DAC_open() has to be called first.
 *
 *  @param  [in]    handle        A DAC_Handle returned from DAC_open.
 *
 *  @retval #DAC_STATUS_SUCCESS The call was successful.
 *  @retval #DAC_STATUS_ERROR The call was unsuccessful.
 *  @retval #DAC_STATUS_INUSE The call was unsuccessful because the DAC is in
 *                            use by the Sensor Controller or by another
 *                            handle.
 *
 *  @sa     DAC_disable()
 */
extern int_fast16_t DAC_enable(DAC_Handle handle);

/*!
 *  @brief  Function to disable the DAC's output.
 *
 *  @pre    DAC_enable() has to be called first.
 *
 *  @param  [in]    handle        A DAC_Handle returned from DAC_open.
 *
 *  @retval #DAC_STATUS_SUCCESS The call was successful.
 *  @retval #DAC_STATUS_ERROR The call was unsuccessful.
 *
 *  @sa     DAC_enable()
 */
extern int_fast16_t DAC_disable(DAC_Handle handle);

/*!
 *  @brief  Function to initialize the DAC_Params struct to its defaults.
 *
 *  @param  [out]   params  A pointer to DAC_Params structure for
 *                          initialization
 *
 *  Defaults values are:
 *  @code
 *  params.initCode       = 0,
 *  params.custom         = NULL;
 *  @endcode
 *
 *  @param  params  Parameter structure to initialize
 */
extern void DAC_Params_init(DAC_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_DAC__include */
