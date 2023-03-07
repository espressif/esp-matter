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
/** ============================================================================
 *  @file       TemperatureCC26X2.h
 *
 *  @brief      Temperature driver implementation for the CC26X2 family
 *
 *  The temperature driver on CC26X2 is a part of the battery monitoring
 *  system in AON (always on). It periodically takes measurements of the
 *  temperature of the chip and will issue interrupts if the configured
 *  upper limit or lower limit is crossed.
 *
 *  # Standby Power Mode Behavior #
 *  The temperature measurement is active while in standby power mode as well.
 *  The interrupt used by the temperature module is capable of bringing the
 *  device out of standby and into active mode to handle it. That means that
 *  an application will not miss a change in temperature just because the device
 *  has transitioned to standby power mode.
 *  While in standby, the temperature will only be sampled during a VDDR
 *  recharge pulse. This means that the sampling frequency in standby will be
 *  determined by the temperature as leakage increases with temperature and
 *  requires more frequent recharging of VDDR.
 *
 *  # Measurement Confidence Bounds
 *  There is an inherent inaccuracy in the temperature measurements reported
 *  by the device for any given chip temperature. This inaccuracy varies
 *  by chip.
 *  In order to set accurate threshold values and act upon provided
 *  temperatures, the following table provides a 99% confidence interval for
 *  the upper and lower bounds of the measured temperature by true
 *  temperature. These figures are given across the range of manufacturing
 *  variances.
 *
 *  | Ambient Temperature | -40 | -30 | -20 | -10 | 0  | 10 | 20 | 30 | 40 | 50 | 60 | 70 | 80 | 90 | 100 | 110 | 120 |
 *  |---------------------|-----|-----|-----|-----|----|----|----|----|----|----|----|----|----|----|-----|-----|-----|
 *  | Upper               | -28 | -20 | -12 | -3  | 7  | 17 | 26 | 36 | 46 | 56 | 65 | 75 | 85 | 95 | 105 | 115 | 125 |
 *  | Lower               | -45 | -36 | -26 | -16 | -5 | 4  | 14 | 24 | 35 | 45 | 54 | 65 | 75 | 85 | 94  | 104 | 113 |
 *
 *  # Measurement-to-Measurement Variations #
 *  For each chip, there is a distribution of temperature readings each
 *  measurement will yield for each true temperature. This means that if a
 *  notification threshold close to the current temperature is chosen, the
 *  hardware may cause an interrupt only for the driver to read out the
 *  temperature again with a value that does not cross the set threshold.
 *  In this case, the driver will not issue a notification. This does cost
 *  cpu cycles and energy though.
 *
 *  Because of this measurement-to-measurement variability, it is not
 *  recommended to set a threshold closer than 5 degrees to the current
 *  temperature when registering a notification.
 */

#ifndef ti_drivers_temperature_TemperatureCC26X2__include
#define ti_drivers_temperature_TemperatureCC26X2__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/Temperature.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief Temperature driver configuration struct for CC26X2
 *
 *  This struct globally controls configuration settings for the CC26X2
 *  Temperature driver.
 *
 *  The CC26X2 Temperature driver implementation links against a structure
 *  of this type named TemperatureCC26X2_config.
 *
 *  This structure must be allocated and configured by the application. If
 *  SysConfig is used, this struct will be automatically created when the
 *  Temperature module is used in SysConfig.
 */
typedef struct {
    /*! @brief  Temperature sensor's interrupt priority.
     *
     *  The interrupt line is shared between the temperature sensor and the
     *  battery voltage monitor on CC26X2
     *
     *  The CC26X2 uses three of the priority bits, meaning ~0 has the same
     *  effect as (7 << 5).
     *
     *  (7 << 5) will apply the lowest priority.
     *
     *  (1 << 5) will apply the highest priority.
     *
     *  Setting the priority to 0 is not supported by this driver.
     *
     *  HWI's with priority 0 ignore the HWI dispatcher to support zero-latency
     *  interrupts, thus invalidating the critical sections.
     */
    uint8_t intPriority;
} TemperatureCC26X2_Config;


#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_temperature_TemperatureCC26X2__include */
