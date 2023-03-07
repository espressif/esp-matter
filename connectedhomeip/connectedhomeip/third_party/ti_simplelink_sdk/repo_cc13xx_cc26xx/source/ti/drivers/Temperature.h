/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  @file       Temperature.h
 *
 *  @brief      Temperature driver
 *
 *  @anchor ti_drivers_Temperature_Overview
 *  # Overview #
 *  The Temperature driver provides services related to measuring and reacting
 *  to the current temperature of the chip and changes to it.
 *
 *  The two main services provided are:
 *      - Getting the current temperature
 *      - Providing notification callbacks when the temperature changes
 *
 *  @anchor ti_drivers_Temperature_Usage
 *  # Usage #
 *
 *  ## Initialisation #
 *  Unlike most drivers, there is only a single instance of the temperature
 *  driver that is always available once #Temperature_init() is called.
 *  #Temperature_init() should be called once before using other Temperature
 *  driver APIs. Subsequent #Temperature_init() calls will have no effect.
 *
 *  ## Getting the Current Temperature #
 *  The most basic function of the driver is to provide the current temperature
 *  and return it. It is encoded as a signed integer in degrees C.
 *
 *  ## Notifications #
 *  The other major function of the Temperature driver is to notify the
 *  application when the temperature changes and crosses an application-defined
 *  threshold.
 *
 *  There are three default usecases for this:
 *      - High threshold.
 *        The application will receive a notification callback when
 *        currentTemperature >= thresholdHigh.
 *      - Low threshold.
 *        The application will receive a notification callback when
 *        currentTemperature <= thresholdLow.
 *      - Range threshold.
 *        The application will receive a notification callback when
 *        currentTemperature >= thresholdHigh || currentTemperature <=
 *        thresholdLow. This setup addresses usecases
 *        where a notification is required when the temperature changes by a
 *        certain amount regardless of whether it is up or down. Adjusting
 *        clock offsets based on temperature is a good example of this.
 *
 *  ### Registering Notifications
 *  There are three functions that register a notification for the application:
 *      - #Temperature_registerNotifyHigh()
 *      - #Temperature_registerNotifyLow()
 *      - #Temperature_registerNotifyRange()
 *
 *  Multiple notifications may be registered. The different parts of the
 *  application and drivers that need to respond to a temperature change do not
 *  need to know of one another.
 *  Each notification must have its own #Temperature_NotifyObj and must be
 *  registered individually.
 *
 *  ### Notification Callbacks
 *  Once the chip temperature crosses the smallest high threshold or largest
 *  low threshold amongst the registered notifications, the driver will
 *  iterate over the entire list of registered notification and check which
 *  ones have triggered. Notifications that have triggered are removed from
 *  the list of registered notifications and thus are no longer registered.
 *  Their callback function is then invoked.
 *
 *  If an application wishes to reregister a notification that just triggered
 *  and was unregistered, it may register it again from within the notification
 *  callback or another context.
 *
 *  It is possible to determine whether the high or low threshold triggered
 *  the notification callback as follows:
 *      - currentTemperature <= thresholdTemperature: Low threshold triggered
 *      - currentTemperature >= thresholdTemperature: High threshold triggered
 *  This information is only reasonably useful when registering a notification
 *  with both a high and low threshold using #Temperature_registerNotifyRange().
 *  Even then, the expected basic usecase only cares about the current
 *  temperature and adding an offset to it when registering the notification
 *  again.
 *
 *  ### Unregistering Notifications
 *  Registered notifications are unregistered in two ways:
 *      - Automatically when a notification triggers
 *      - By calling #Temperature_unregisterNotify()
 *
 *  Unregistered notifications may be registered again at any time.
 *
 *  # Measured vs True Temperature
 *  While the driver aims to supply and act on an accurate absolute temperature,
 *  there will be differences between the measured vs the true temperature due
 *  to inherent variances in the manufacturing process. The nature of these
 *  differences varies by device family.
 *
 *  Examples of such differences:
 *      - A constant per-chip offset between the measured and the true
 *        temperature
 *      - An temperature dependent per-chip offset between the measured and the
 *        true temperature
 *      - A variance in the measured temperature when measuring multiple times
 *        at the same chip temperature
 *
 *  It is strongly recommended to read the device-specific Temperature driver
 *  documentation for details of the temperature sensor characteristics and
 *  how they might affect choices of threshold values.
 *
 *  @anchor ti_drivers_Temperature_Synopsis
 *  # Synopsis #
 *  @anchor ti_drivers_Temperature_Synopsis_Code
 *  @code
 *  #include <ti/drivers/Temperature.h>
 *
 *  #define WINDOW_DELTA 10
 *
 *  Temperature_init();
 *
 *  currentTemperature = Temperature_getTemperature();
 *
 *  result = Temperature_registerNotifyRange(&notifyObject,
 *                                           currentTemperature + WINDOW_DELTA,
 *                                           currentTemperature - WINDOW_DELTA,
 *                                           myNotifyFxn,
 *                                           clientArg);
 *  @endcode
 *
 *  @anchor ti_drivers_Temperature_Examples
 *  # Examples #
 *
 *  ## Register a High Threshold Notification #
 *
 *  @code
 *
 *  // The notification will trigger when the temperature reaches 40 C
 *  #define THRESHOLD_CUTOFF 40
 *
 *  #include <ti/drivers/Temperature.h>
 *
 *  void thresholdNotifyFxn(int16_t currentTemperature,
 *                          int16_t thresholdTemperature,
 *                          uintptr_t clientArg,
 *                          Temperature_NotifyObj *notifyObject) {
 *     // Post a semaphore, set a flag, or otherwise act upon the temperature
 *     // change.
 *  }
 *
 *  ...
 *
 *  // Initialize the Temperature driver and register a notification.
 *
 *  Temperature_init();
 *
 *  int_fast16_t status = Temperature_registerNotifyHigh(notifyObject,
 *                                                       THRESHOLD_CUTOFF,
 *                                                       thresholdNotifyFxn,
 *                                                       NULL);
 *
 *  if (status != Temperature_STATUS_SUCCESS) {
 *      // Handle error
 *  }
 *
 *  @endcode
 *
 *  ## Register a Range Threshold Notification and Reregister in Callback #
 *
 *  @code
 *
 *  #define THRESHOLD_DELTA 5
 *
 *  #include <ti/drivers/Temperature.h>
 *
 *
 *  void deltaNotificationFxn(int16_t currentTemperature,
 *                        int16_t thresholdTemperature,
 *                        uintptr_t clientArg,
 *                        Temperature_NotifyObj *notifyObject) {
 *      int_fast16_t status;
 *
 *      status = Temperature_registerNotifyRange(notifyObject,
 *                                               currentTemperature + THRESHOLD_DELTA,
 *                                               currentTemperature - THRESHOLD_DELTA,
 *                                               deltaNotificationFxn,
 *                                               NULL);
 *
 *      if (status != Temperature_STATUS_SUCCESS) {
 *          while(1);
 *      }
 *  }
 *
 *   ...
 *
 *  // Initialize the Temperature driver and register a notification.
 *
 *  Temperature_init();
 *
 *  int16_t currentTemperature = Temperature_getTemperature();
 *
 *  int_fast16_t status = Temperature_registerNotifyRange(notifyObject,
 *                                                        currentTemperature + THRESHOLD_DELTA,
 *                                                        currentTemperature - THRESHOLD_DELTA,
 *                                                        deltaNotificationFxn,
 *                                                        NULL);
 *  @endcode
 */

#ifndef ti_drivers_Temperature__include
#define ti_drivers_Temperature__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/utils/List.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Common Temperature status code reservation offset.
 * Temperature driver implementations should offset status codes with
 * Temperature_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define TemperatureXYZ_STATUS_ERROR0    Temperature_STATUS_RESERVED - 0
 * #define TemperatureXYZ_STATUS_ERROR1    Temperature_STATUS_RESERVED - 1
 * #define TemperatureXYZ_STATUS_ERROR2    Temperature_STATUS_RESERVED - 2
 * @endcode
 */
#define Temperature_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code.
 *
 * Functions return Temperature_STATUS_SUCCESS if the function was executed
 * successfully.
 */
#define Temperature_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code.
 *
 * Functions return Temperature_STATUS_ERROR if the function was not executed
 * successfully.
 */
#define Temperature_STATUS_ERROR           (-1)


/* @cond
 *
 * Type declaration for the notification object made separately from the
 * struct definition because of the circular dependency between
 * #Temperature_NotifyFxn() and #Temperature_NotifyObj.
 */
typedef struct Temperature_NotifyObj Temperature_NotifyObj;
/* @endcond */

/*!
 *  @brief Function prototype for a notification callback.
 *
 *  @param [in]     currentTemperature      Current chip temperature
 *
 *  @param [in]     thresholdTemperature    Temperature threshold that caused
 *                                          this notification callback.
 *
 *  @param [in]     clientArg               Argument provided by the application
 *                                          during registration.
 *
 *  @param [in/out] notifyObject            The notification object that was
 *                                          registered previously. This pointer
 *                                          may be used to register the
 *                                          notification again with updated
 *                                          inputs from within the notification
 *                                          callback.
 */
typedef void (*Temperature_NotifyFxn) (int16_t currentTemperature,
                                       int16_t thresholdTemperature,
                                       uintptr_t clientArg,
                                       Temperature_NotifyObj *notifyObject);

/*!
 *  @brief Temperature notify object structure.
 *
 *  This structure specification is for internal use. Notification clients must
 *  pre-allocate a notify object when registering for a notification;
 *  #Temperature_registerNotifyHigh(), #Temperature_registerNotifyLow(),
 *  or #Temperature_registerNotifyRange() will take care initializing the
 *  internal elements appropriately.
 */
struct Temperature_NotifyObj {
    List_Elem link;                     /*!< For placing on the notify list */
    Temperature_NotifyFxn notifyFxn;    /*!< Application callback function */
    int16_t thresholdHigh;              /*!< High threshold in degrees C */
    int16_t thresholdLow;               /*!< Low threshold in degrees C */
    uintptr_t clientArg;                /*!< Application provided arg */
    bool isRegistered;                  /*!< Is the notification active */
};


/*!
 *  @brief This function initializes the Temperature driver.
 *
 *  This function initializes the internal state of the Temperature driver.
 *  It must be called before calling any other Temperature functions. Calling
 *  this function multiple times will only have an effect the first time.
 */
void Temperature_init();

/*!
 *  @brief Gets the current temperature in degrees C.
 *
 *  @return Current temperature in degrees C
 */
int16_t Temperature_getTemperature(void);

/*!
 *  @brief Registers a notification with a high threshold.
 *
 *  This function registers a Temperature notification with a high threshold.
 *  Once the chip temperature rises above @c thresholdHigh, @c notifyFxn is
 *  called and the notification is automatically unregistered.
 *
 *  @param  notifyObject        Structure to be initialized. After returning,
 *                              it will contain the data necessary to issue a
 *                              notification callback. The memory of the
 *                              structure must persist while the notification
 *                              is registered.
 *
 *  @param  [in] thresholdHigh  Threshold temperature in degrees C
 *
 *  @param  [in] notifyFxn      Callback function that is called once the
 *                              chip temperature rises above
 *                              @c thresholdHigh.
 *
 *  @param  [in] clientArg      Application-specified argument
 *
 *  @retval #Temperature_STATUS_SUCCESS The notification was successfully
 *                                      registered.
 *  @retval #Temperature_STATUS_ERROR   There was an error during registration.
 *
 *  @pre Temperature_init() called
 */
int_fast16_t Temperature_registerNotifyHigh(Temperature_NotifyObj *notifyObject,
                                            int16_t thresholdHigh,
                                            Temperature_NotifyFxn notifyFxn,
                                            uintptr_t clientArg);

/*!
 *  @brief Registers a notification with a low threshold.
 *
 *  This function registers a Temperature notification with a low threshold.
 *  Once the chip temperature falls below @c thresholdLow, @c notifyFxn is
 *  called and the notification is automatically unregistered.
 *
 *  @param  notifyObject        Structure to be initialized. After returning,
 *                              it will contain the data necessary to issue a
 *                              notification callback. The memory of the
 *                              structure must persist while the notification
 *                              is registered.
 *
 *  @param  [in] thresholdLow   Threshold temperature in degrees C
 *
 *  @param  [in] notifyFxn      Callback function that is called once the
 *                              chip temperature falls below
 *                              @c thresholdLow.
 *
 *  @param  [in] clientArg      Application-specified argument
 *
 *  @retval #Temperature_STATUS_SUCCESS The notification was successfully
 *                                      registered.
 *  @retval #Temperature_STATUS_ERROR   There was an error during registration.
 *
 *  @pre Temperature_init() called
 */
int_fast16_t Temperature_registerNotifyLow(Temperature_NotifyObj *notifyObject,
                                           int16_t thresholdLow,
                                           Temperature_NotifyFxn notifyFxn,
                                           uintptr_t clientArg);

/*!
 *  @brief Registers a notification with both a high and low threshold.
 *
 *  This function registers a Temperature notification with a high and low
 *  threshold. Once the chip temperature rises above @c thresholdHigh or
 *  falls below @c thresholdLow, @c notifyFxn is called and the notification is
 *  automatically unregistered.
 *
 *  @param  notifyObject        Structure to be initialized. After returning,
 *                              it will contain the data necessary to issue a
 *                              notification callback. The memory of the
 *                              structure must persist while the notification
 *                              is registered.
 *
 *  @param  [in] thresholdHigh  High threshold temperature in degrees C
 *
 *  @param  [in] thresholdLow   Low threshold temperature in degrees C
 *
 *  @param  [in] notifyFxn      Callback function that is called once the
 *                              chip temperature falls below
 *                              @c thresholdLow, or rises above
 *                              @c thresholdHigh.
 *
 *  @param  [in] clientArg      Application-specified argument
 *
 *  @retval #Temperature_STATUS_SUCCESS The notification was successfully
 *                                      registered
 *  @retval #Temperature_STATUS_ERROR   There was an error during registration
 *
 *  @pre Temperature_init() called
 */
int_fast16_t Temperature_registerNotifyRange(Temperature_NotifyObj *notifyObject,
                                             int16_t thresholdHigh,
                                             int16_t thresholdLow,
                                             Temperature_NotifyFxn notifyFxn,
                                             uintptr_t clientArg);


/*!
 *  @brief Unregisters a currently registered notification.
 *
 *  This function unregisters a currently registered notification. It should not
 *  be called on a @c notifyObject that is not currently registered.
 *
 *  @param  notifyObject    Notification to unregister.
 *
 *  @retval #Temperature_STATUS_SUCCESS The notification was successfully
 *                                      unregistered.
 *  @retval #Temperature_STATUS_ERROR   There was an error during
 *                                      unregistration.
 *
 *  @pre Register @c notifyObject with #Temperature_registerNotifyHigh(),
 *       #Temperature_registerNotifyLow(), or #Temperature_registerNotifyRange()
 */
int_fast16_t Temperature_unregisterNotify(Temperature_NotifyObj *notifyObject);

/*!
 *  @brief Get the high threshold of a notification.
 *
 *  This function should not be called on a @c notifyObject registered with
 *  #Temperature_registerNotifyLow(). The high threshold value returned in
 *  that case will be a device-specific invalid temperature.
 *
 *  @param  notifyObject    Notification to get the high threshold of.
 *
 *  @return High threshold in degrees C.
 *
 *  @pre Register @c notifyObject with #Temperature_registerNotifyHigh(),
 *       or #Temperature_registerNotifyRange()
 */
int16_t Temperature_getThresholdHigh(Temperature_NotifyObj *notifyObject);

/*!
 *  @brief Get the low threshold of a notification.
 *
 *  This function should not be called on a @c notifyObject registered with
 *  #Temperature_registerNotifyHigh(). The low threshold value returned in
 *  that case will be a device-specific invalid temperature.
 *
 *  @param  notifyObject Notification to get the low threshold of.
 *
 *  @return Low threshold in degrees C.
 *
 *  @pre Register @c notifyObject with #Temperature_registerNotifyLow(),
 *       or #Temperature_registerNotifyRange()
 */
int16_t Temperature_getThresholdLow(Temperature_NotifyObj *notifyObject);

/*!
 *  @brief Get the high and low threshold of a notification.
 *
 *  This function should not be called on a @c notifyObject registered with
 *  #Temperature_registerNotifyLow() or #Temperature_registerNotifyHigh().
 *  The unconfigured threshold value returned in that case will be a
 *  device-specific invalid temperature.
 *
 *  @param  notifyObject Notification to get the high and low threshold of.
 *
 *  @param [out]    thresholdHigh High threshold value in degrees C written back
 *                                by this function.
 *
 *  @param [out]    thresholdLow  Low threshold value in degrees C written back
 *                                by this function.
 *
 *  @pre Register @c notifyObject with #Temperature_registerNotifyRange()
 */
void Temperature_getThresholdRange(Temperature_NotifyObj *notifyObject,
                                   int16_t *thresholdHigh,
                                   int16_t *thresholdLow);

/*!
 *  @brief Get the application-provided clientArg of a notification.
 *
 *  @param  notifyObject Notification to get the clientArg of.
 *
 *  @return The clientArg provided during registration.
 *
 *  @pre Register @c notifyObject with #Temperature_registerNotifyHigh(),
 *       #Temperature_registerNotifyLow(), or #Temperature_registerNotifyRange()
 */
uintptr_t Temperature_getClientArg(Temperature_NotifyObj *notifyObject);

/*!
 *  @brief Get the notifyFxn provided during registration.
 *
 *  @param  notifyObject Notification to get the notifyFxn of.
 *
 *  @return The notifyFxn provided during registration
 *
 *  @pre Register @c notifyObject with #Temperature_registerNotifyHigh(),
 *       #Temperature_registerNotifyLow(), or #Temperature_registerNotifyRange()
 */
Temperature_NotifyFxn Temperature_getNotifyFxn(Temperature_NotifyObj *notifyObject);


#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_Temperature__include */
