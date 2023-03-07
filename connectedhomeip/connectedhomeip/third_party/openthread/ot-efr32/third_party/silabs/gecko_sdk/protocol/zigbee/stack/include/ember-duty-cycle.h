/***************************************************************************//**
 * @file
 * @brief EmberZNet APIs for Duty Cycle support.
 *  These apis are intended to use on SubGhz network only.
 *  For multi phy interface, these apis are only apply to SugGhz interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_EMBER_DUTY_CYCLE_H
#define SILABS_EMBER_DUTY_CYCLE_H

/**
 * @brief Per device duty cycle monitor feature limits to 32 connected end devices
 *    (excluding a local device).
 */
#define EMBER_MAX_CHILDREN_FOR_PER_DEVICE_DUTY_CYCLE_MONITOR  32
/**
 * @addtogroup ember_duty_cycle
 *
 * See ember-duty-cycle.h for source code.
 * @{
 */

/** @brief Obtains the current duty cycle state.
 *
 * @param returnedState The current duty cycle state in effect.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_BAD_ARGUMENT if
 *                 - returnedState is NULL (SoC only).
 */
EmberStatus emberGetDutyCycleState(EmberDutyCycleState* returnedState);

/** @brief Set the current duty cycle limits configuration.
 *        The Default limits set by stack if this call is
 *        not made.
 *
 * @param limits The duty cycle limits configuration to utilize.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS If config looks kosher.
 * - ::EMBER_BAD_ARGUMENT if
 *             - limits is NULL
 *             - set illegal value such as setting only one of the limits
 *               to default.
 *             - violates constraints Susp > Crit > Limi.
 * - ::EMBER_INVALID_CALL if
 *             - device is operating on 2.4Ghz
 */
EmberStatus emberSetDutyCycleLimitsInStack(const EmberDutyCycleLimits* limits);

/** @brief Obtains the current duty cycle limits that were previously set by a
 *        call to emberSetDutyCycleLimitsInStack(), or the defaults set by the
 *        stack if no set call was made.
 *
 * @param returnedLimits Return current duty cycle limits if returnedLimits
 *                       is not NULL.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_BAD_ARGUMENT if
 *                 - returnedLimits is NULL (SoC only).
 */
EmberStatus emberGetDutyCycleLimits(EmberDutyCycleLimits* returnedLimits);

/** @brief Returns the duty cycle of the stack's connected children
 *        that are being monitored, up to maxDevices. It indicates the
 *        amount of overall duty cycle they have consumed (up to the
 *        suspend limit). The first entry is always the local stack's nodeId,
 *        and thus the total aggregate duty cycle for the device.
 *        The passed pointer arrayOfDeviceDutyCycles MUST have space
 *        for maxDevices.
 *
 * @param maxDevices Number of devices to retrieve consumed duty cycle.
 *        arrayOfDeviceDutyCycles Consumed duty cycles up to maxDevices.
 *                                When the number of children that are being
 *                                monitored is less than maxDevices, the
 *                                EmberNodeId element in the EmberPerDeviceDutyCycle
 *                                will be 0xFFFF.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_BAD_ARGUMENT if
 *                 - maxDevices is greater than EMBER_MAX_CHILDREN_FOR_PER_DEVICE_DUTY_CYCLE_MONITOR + 1.
 *                 - arrayOfDeviceDutyCycles is NULL.
 */
EmberStatus emberGetCurrentDutyCycle(uint8_t maxDevices,
                                     EmberPerDeviceDutyCycle* arrayOfDeviceDutyCycles);

/** @brief Callback fires when the duty cycle state has changed.
 *        This callback is only on SoC.
 *
 * If the application includes ::emberDutyCycleHandler(), it must
 * define EMBER_APPLICATION_HAS_DUTY_CYCLE_HANDLER in its CONFIGURATION_HEADER.
 *
 * @param channelPage The channel page whose duty cycle state has changed.
 *        channel     The channel number whose duty cycle state has changed.
 *        state       The current duty cycle state.
 *
 */
void emberDutyCycleHandler(uint8_t channelPage,
                           uint8_t channel,
                           EmberDutyCycleState state);

/** @} END addtogroup */

#endif // SILABS_EMBER_DUTY_CYCLE_H
