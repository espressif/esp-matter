/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__
#include "hal_platform.h"
#include "hal_pinmux_define.h"

#ifdef HAL_GPIO_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * This section provides introduction to the General Purpose Input Output (GPIO) APIs, including terms and acronyms, features,
 * architecture, how to use APIs, the GPIO function groups, enums, structures and functions.
 *
 * @section HAL_GPIO_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GPIO                       |General Purpose Input Output is a generic pin on an integrated circuit defined as an input or output pin, and controlled by the user at run time. For more information, please refer to <a href="https://en.wikipedia.org/wiki/General-purpose_input/output"> General Purpose Input Output in Wikipedia </a>.|
 *
 * @section HAL_GPIO_Features_Chapter Supported features
 * - \b Support \b GPIO \b and \b peripheral \b operating \b modes. \n
 *       GPIO operates in various modes depending on the user configuration:
 *  - \b GPIO \b mode: In this mode the pin is programmed as a software controlled input or output by reading or writing to and from the corresponding data register.
 *       Both input data and output data can be digital low or digital high.  \n
 *  - \b Peripheral \b mode:   In this mode the pin operates as an embedded peripheral.
 *       The microcontroller pin connects the peripheral through a multiplexer to only one alternate function at a time. The peripheral module must configure the pinmux to the corresponding pin before it is used.
 *       Two ways to apply pinmux configuration: call #hal_pinmux_set_function() or configure the Easy pinmux tool (EPT), a software tool providing a graphical user interface for pinmux configuration.
 *       For more information about alternate functions of the pins, please refer to hal_pinmux_define.h.
 *   \n
 * - Call #hal_pinmux_set_function() to set the pin mode and call #hal_gpio_set_direction() to set the direction of GPIO. If the pin is in GPIO mode, call the #hal_gpio_get_input()
 * to get input data of the pin, and hal_gpio_set_output() to set data to output.
 *   \n
 * - \b Support \b toggle \b function. \n
 *      The toggle function inverses output data of pins for every function call.\n
 *      Call #hal_gpio_toggle_pin() to toggle the output data of the target pin.
 *   \n
 * - \b Support \b pull-up \b and \b pull-down \b functions. \n
 *      The pull-up and pull-down functions define the input state of a pin if no signal source is connected.
 *      Both the pull-up and pull-down functions are implemented using resistors.
 *      Set the pull state of the target pin by configuring the GPIO registers.
 *      \n
 *      Call #hal_gpio_pull_down() to set the pin to pull down state and #hal_gpio_pull_up() to set the pin to pull up state.
 *
 * @}
 * @}
 */

#ifdef HAL_GPIO_FEATURE_PUPD
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - \b Support \b pull-up \b and \b pull-down \b with \b different \b resistance \b functions. \n
 *      Pins can be configured to pull up or pull down state with different resistance, if needed.\n
 *      Call #hal_gpio_set_pupd_register() to set a pull state with corresponding resistance according to hardware design.
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_FEATURE_INVERSE
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - \b Support \b inversion \b function. \n
 *    Inversion function inverses input data of the GPIO until the function is disabled, it is useful when the chip is input high active while the connected device is output and low active.  \n
 *      Call #hal_gpio_enable_inversion() to inverse the input data of the pin until the function is disabled by calling #hal_gpio_disable_inversion(). \n
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_FEATURE_CLOCKOUT
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - \b Support \b output \b clock \b function.  \n
 *      There are 6 clock-out ports embedded in all GPIO pins and each of them can be configured with appropriate clock frequency to send outside the chip.
 *   \n
 *       Call #hal_gpio_set_clockout() to set the output clock source of target pin after configuring the pin to operate in output clock mode. \n
 * @}
 * @}
 */
#endif


/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * @section HAL_GPIO_Architechture_Chapter Software architecture of GPIO
 *
 * The GPIO driver supports two modes of operation as described below.
 * -# \b GPIO \b mode \b architecture: If the pin is specified to operate in GPIO mode, it can be programmed in input and output directions.
 *    If the direction is configured as output through direction register, the data written to the output register will be
 *    output on the pin, the output data register is also accessible while an access to the output data register only gets the last written value.
 *    The output pin is push-pull type by default, push-pull output here means a type of electronic circuit that uses a pair of active devices that alternately supply current to,
 *    or absorb current from, a connected load.
 *    Push-pull outputs are present in TTL and CMOS digital logic circuits and in some types of amplifiers, and are usually implemented as a complementary pair of transistors,
 *    one dissipating or sinking current from the load to ground or a negative power supply, and the other supplying or sourcing current to the load from a positive power supply.
 *    If the direction is configured as input, the data present on the pin is received from an input data register. Unlike the output circuit,
 *    the Schmitt trigger and pull-up and pull-down resistors on the input circuit. Among them, Schmitt trigger is an active circuit which converts an analog input signal to a digital output signal.
 *    The pull-up and pull-down resistors help to set the target pin to the default voltage level (high or low) when the target pin is not connected to the external source.
 *    \n
 * -# \b Peripheral \b mode \b architecture : The pins operate in one of several onboard peripheral modes based on multiplexer settings.
 *  It can only operate in one mode at a time to avoid conflict between peripherals sharing the same pin.
 *    \n
 * @section HAL_GPIO_Driver_Usage_Chapter How to use this driver
 *
 *  - \b GPIO \b mode.
 *   - Step 1. Call #hal_gpio_init() to initialize the pin.
 *   - Step 2. Call #hal_pinmux_set_function() to configure the pin to operate in GPIO mode.
 *   - Step 3. Call #hal_gpio_set_direction() to configure the direction of GPIO.
 *   - Step 4. Call #hal_gpio_set_output() to set the data into output direction.
 *   - Step 5. Call #hal_gpio_deinit() to deinitialize the pin.
 *   - sample code:
 *    @code
 *     void gpio_application(void)
 *     {
 *
 *       hal_gpio_init(gpio_pin);
 *       hal_pinmux_set_function(gpio_pin, function_index); // Set the pin to operate in GPIO mode.
 *       hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
 *       hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
 *       hal_gpio_deinit(gpio_pin);
 *     }
 *    @endcode
 * @}
 * @}
 */


/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 *  -  \b Peripheral \b mode.
 *   - Step 1. Call #hal_gpio_init() to initialize the pin.
 *   - Step 2. Call #hal_pinmux_set_function() to configure the pin to operate in peripheral mode.
 *   - Step 3. Call #hal_gpio_deinit() to deinitialize the pin.
 *   - sample code:
 *    @code
 *     void gpio_application(void)
 *     {
 *       hal_gpio_init(gpio_pin);
 *       // For more information about pinmux, please refer to hal_pinmux_define.h
 *       hal_pinmux_set_function(gpio_pin, function_index);   // Set the pin to the peripheral mode as defined in function_index.
 *       hal_gpio_deinit(gpio_pin);
 *      }
 *   @endcode
 *  \n
 */



#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gpio_enum Enum
  * @{
  */

/** @brief This enum defines the GPIO direction. */
typedef enum {
    HAL_GPIO_DIRECTION_INPUT  = 0,              /**<  GPIO input direction. */
    HAL_GPIO_DIRECTION_OUTPUT = 1               /**<  GPIO output direction. */
} hal_gpio_direction_t;


/** @brief This enum defines the data type of GPIO. */
typedef enum {
    HAL_GPIO_DATA_LOW  = 0,                     /**<  GPIO data low. */
    HAL_GPIO_DATA_HIGH = 1                      /**<  GPIO data high. */
} hal_gpio_data_t;

/** @brief This enum defines the function index of GPIO. */
typedef enum {
    HAL_GPIO_MODE_0 = 0,                     /**<  GPIO mode 0. */
    HAL_GPIO_MODE_1 = 1,                     /**<  GPIO mode 1. */
    HAL_GPIO_MODE_2 = 2,                     /**<  GPIO mode 2. */
    HAL_GPIO_MODE_3 = 3,                     /**<  GPIO mode 3. */
    HAL_GPIO_MODE_4 = 4,                     /**<  GPIO mode 4. */
    HAL_GPIO_MODE_5 = 5,                     /**<  GPIO mode 5. */
    HAL_GPIO_MODE_6 = 6,                     /**<  GPIO mode 6. */
    HAL_GPIO_MODE_7 = 7                      /**<  GPIO mode 7. */
} hal_gpio_mode_t;


/** @brief This enum defines the return type of GPIO API. */
typedef enum {
    HAL_GPIO_STATUS_ERROR             = -3,     /**< The GPIO function failed to execute.*/
    HAL_GPIO_STATUS_ERROR_PIN         = -2,     /**< Invalid input pin number. */
    HAL_GPIO_STATUS_INVALID_PARAMETER = -1,     /**< Invalid input parameter. */
    HAL_GPIO_STATUS_OK                = 0       /**< The GPIO function executed successfully. */
} hal_gpio_status_t;


/** @brief This enum defines the return type of pinmux API. */
typedef enum {
    HAL_PINMUX_STATUS_ERROR             = -3,   /**< The pinmux function failed to execute.*/
    HAL_PINMUX_STATUS_ERROR_PORT        = -2,   /**< Invalid input pin port. */
    HAL_PINMUX_STATUS_INVALID_FUNCTION  = -1,   /**< Invalid input function. */
    HAL_PINMUX_STATUS_OK                = 0     /**< The pinmux function executed successfully. */
} hal_pinmux_status_t;

#ifdef HAL_GPIO_FEATURE_SET_DRIVING
/** @brief This enum defines driving current. */
typedef enum {
    HAL_GPIO_DRIVING_CURRENT_2MA    = 0,        /**< Defines GPIO driving current as 2mA.  */
    HAL_GPIO_DRIVING_CURRENT_4MA    = 1,        /**< Defines GPIO driving current as 4mA.  */
    HAL_GPIO_DRIVING_CURRENT_6MA    = 2,        /**< Defines GPIO driving current as 6mA. */
    HAL_GPIO_DRIVING_CURRENT_8MA    = 3,         /**< Defines GPIO driving current as 8mA. */
    HAL_GPIO_DRIVING_CURRENT_10MA   = 4,        /**< Defines GPIO driving current as 10mA.  */
    HAL_GPIO_DRIVING_CURRENT_12MA   = 5,        /**< Defines GPIO driving current as 12mA.  */
    HAL_GPIO_DRIVING_CURRENT_14MA   = 6,        /**< Defines GPIO driving current as 14mA. */
    HAL_GPIO_DRIVING_CURRENT_16MA   = 7,         /**< Defines GPIO driving current as 16mA. */
    HAL_GPIO_DRIVING_CURRENT_18MA   = 8,        /**< Defines GPIO driving current as 18mA.  */
    HAL_GPIO_DRIVING_CURRENT_20MA   = 9,        /**< Defines GPIO driving current as 20mA.  */
    HAL_GPIO_DRIVING_CURRENT_22MA   = 10,        /**< Defines GPIO driving current as 22mA. */
    HAL_GPIO_DRIVING_CURRENT_24MA   = 11,         /**< Defines GPIO driving current as 24mA. */
    HAL_GPIO_DRIVING_CURRENT_26MA   = 12,        /**< Defines GPIO driving current as 26mA.  */
    HAL_GPIO_DRIVING_CURRENT_28MA   = 13,        /**< Defines GPIO driving current as 28mA.  */
    HAL_GPIO_DRIVING_CURRENT_30MA   = 14,        /**< Defines GPIO driving current as 30mA. */
    HAL_GPIO_DRIVING_CURRENT_32MA   = 15,         /**< Defines GPIO driving current as 32mA. */
} hal_gpio_driving_current_t;
#endif

/**
  * @}
  */



/**
 * @brief     This function initializes the GPIO hardware with basic functionality. The target pin must be initialized before use.
 * @param[in] gpio_pin specifies the pin number to initialize.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_init(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function deinitializes the GPIO hardware to its default status. The target pin must be deinitialized if not used.
 * @param[in] gpio_pin specifies pin number to deinitialize.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_deinit(hal_gpio_pin_t gpio_pin);

/**
 * @brief     This function configures the pinmux of target GPIO.
 *            Pin Multiplexer (pinmux) connects the pin and the onboard peripherals,
 *            so the pin will operate in a specific mode once the pin is programmed to a peripheral's function.
 *            The alternate functions of every pin are provided in hal_pinmux_define.h.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @param[in] function_index specifies the function for the pin.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PINMUX_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PINMUX_STATUS_INVALID_FUNCTION, a wrong alternate function is given, the parameter must be verified.
 *            If the return value is #HAL_PINMUX_STATUS_ERROR_PORT, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_PINMUX_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */

hal_pinmux_status_t hal_pinmux_set_function(hal_gpio_pin_t gpio_pin, uint8_t function_index);

/**
 * @brief     This function gets the pinmux value of target GPIO.
 * @param[in] gpio_pin specifies the pin number to operate.
 * @param[in] gpio_mode is the function_index received from the target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_function(hal_gpio_pin_t gpio_pin, hal_gpio_mode_t *gpio_mode);

/**
 * @brief     This function gets the input data of target GPIO when the direction of the GPIO is input.
 * @param[in] gpio_pin specifies the pin number to operate.
 * @param[in] gpio_data is the input data received from the target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_input(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data);

/**
 * @brief     This function sets the output data of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to operate.
 * @param[in] gpio_data is the output data of the target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_set_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t gpio_data);


/**
 * @brief     This function gets the output data of the target GPIO when the direction of the GPIO is output.
 * @param[in] gpio_pin specifies the pin number to operate.
 * @param[in] gpio_data is output data of the target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data);


/**
 * @brief     This function sets the direction of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to set.
 * @param[in] gpio_direction is the direction of the target GPIO, the direction can be input or output.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_set_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t gpio_direction);


/**
 * @brief     This function gets the direction of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to operate.
 * @param[in] gpio_direction is the direction of target GPIO, the direction can be input or output.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t *gpio_direction);


#ifdef HAL_GPIO_FEATURE_HIGH_Z
/**
 * @brief     This function sets the target GPIO to high impedance state.
 *            High impedance can prevent the target GPIO from electric leakage.
 *            The pin in high impedance state acts as an open circuit, although it is connected to a low impedance circuit, it will not be affected.
 *            It is recommended to put the pin into high impedance state, if the pin is not in use to optimize the power consumption.
 * @param[in] gpio_pin specifies the pin number to set.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_set_high_impedance(hal_gpio_pin_t gpio_pin);

/**
 * @brief     This function removes the high impedance state for the target GPIO.
 *            High impedance can prevent the target GPIO from electric leakage.
 *            It is necessary to call this function before further configuration if the pin is in high impedance state.
 * @param[in] gpio_pin specifies the pin number to set.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_set_high_impedance(gpio_pin);
 *            // Do something else
 *            ret = hal_gpio_clear_high_impedance(gpio_pin); // Put the target GPIO out of high impedance state before other configuration.
 *            ret = hal_gpio_deinit(gpio_pin);
 *
 * @endcode
 */
hal_gpio_status_t hal_gpio_clear_high_impedance(hal_gpio_pin_t gpio_pin);
#endif


/**
 * @brief     This function toggles the output data of the target GPIO when the direction of the pin is output. After this function,
 *            the output data of the target GPIO will be inversed.
 * @param[in] gpio_pin specifies the pin number to toggle.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); // Set the pin to GPIO mode.
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
 *            ret = hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
 *            ret = hal_gpio_toggle_pin(gpio_pin);  // Output data of gpio_pin will be toggled to low from high.
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_toggle_pin(hal_gpio_pin_t gpio_pin);


#ifdef HAL_GPIO_FEATURE_INVERSE
/**
 * @brief     This function enables the input data inversion of the target GPIO, after this function,
 *            the input data of the target GPIO will always be inversed until the inverse function is disabled.
 * @param[in] gpio_pin specifies the pin number to inverse.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_enable_inversion(gpio_pin);
 *            // Do something else
 *            ret = hal_gpio_disable_inversion(gpio_pin);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_enable_inversion(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function disables the input data inversion of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_enable_inversion(gpio_pin);
 *            // Do something else
 *            ret = hal_gpio_disable_inversion(gpio_pin);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_disable_inversion(hal_gpio_pin_t gpio_pin);
#endif


/**
 * @brief     This function sets the target GPIO to pull-up state, after this function,
 *            the input data of the target pin will be equivalent to high if the pin is disconnected.
 *            This function operates on the pins with only one pull-up resistor.
 * @param[in] gpio_pin specifies the pin number to set.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); // Set the pin to GPIO mode.
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_pull_up(gpio_pin);   // Pull state of the target GPIO is set to pull-up.
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_pull_up(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function sets the target GPIO to the pull-down state, after this function,
 *            the input data of the target pin will be equivalent to low if the pin is disconnected.
 *            This function operates on the pin with one pull-down resistor.
 * @param[in] gpio_pin specifies the pin number to set.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); // Set the pin to GPIO mode.
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_pull_down(gpio_pin);   // Pull state of the target GPIO is set to pull-down.
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_pull_down(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function disables pull-up or pull-down of the target GPIO.
 *            This function operates on the pins with one pull-up and one pull-down resistors.
 * @param[in] gpio_pin specifies the pin number to set.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); // Set the pin to GPIO mode.
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_pull_down(gpio_pin);
 *            ret = hal_gpio_disable_pull(gpio_pin);   // Pull state of the target GPIO is disabled.
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_disable_pull(hal_gpio_pin_t gpio_pin);


#ifdef HAL_GPIO_FEATURE_PUPD
/**
 * @brief     This function sets the pull up/down state of the GPIO that has more than one pull-up or pull-down resistor.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @param[in] gpio_pupd specifies the pull-up or pull-down of the target GPIO.
 * @param[in] gpio_r0 works with gpio_r1 to specify the pull-up and pull-down resistor of the target GPIO.
 * @param[in] gpio_r1 works with gpio_r0 to specify the pull-up and pull-down resistor of the target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, invalid input pin number, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); // Set the pin to GPIO mode.
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_set_pupd_register(gpio_pin,gpio_pupd,gpio_r0,gpio_r1); // Pull state of the target GPIO is set to a state determined by the combination of gpio_pupd,gpio_r0 and gpio_r1.
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_set_pupd_register(hal_gpio_pin_t gpio_pin, uint8_t gpio_pupd, uint8_t gpio_r0, uint8_t gpio_r1);
#endif


#ifdef HAL_GPIO_FEATURE_CLOCKOUT
/**
 * @brief     This function sets the clock-out source of the target GPIO.
 *            The software can configure which clock to send outside the chip.
 *            There are 6 clock-out ports embedded in all GPIO pins, and each clock-out can be programmed to output appropriate clock source.
 *            This function can only be used after configuring the pin to operate in output clock mode.
 * @param[in] gpio_clock_num specifies pin clock number to set.
 * @param[in] clock_mode specifies the clock mode to set to the target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, a wrong parameter (except for pin number) is given, the parameter must be verified.
 *            If the return value is #HAL_GPIO_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index_of_clockout);  // Set the pin to work in clock output mode
 *            ret = hal_gpio_set_clockout(gpio_clock_num, clock_mode);  // The clock-out frequency is defined in clock_mode and gpio_clock_num is determined by the previous step for a given pin number.
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_set_clockout(hal_gpio_clock_t gpio_clock_num, hal_gpio_clock_mode_t clock_mode);
#endif


#ifdef HAL_GPIO_FEATURE_SET_DRIVING
/**
 * @brief     This function sets the driving current of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @param[in] driving specifies the driving current to set to target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_set_driving(gpio_pin, HAL_GPIO_DRIVING_16MA);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_set_driving_current(hal_gpio_pin_t gpio_pin, hal_gpio_driving_current_t driving);

/**
 * @brief     This function gets the driving current of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @param[in] driving specifies the driving current to be set to target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *            hal_gpio_driving_current_t *driving;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_get_driving(gpio_pin, &driving);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_get_driving_current(hal_gpio_pin_t gpio_pin, hal_gpio_driving_current_t *driving);
#endif


#ifdef HAL_GPIO_FEATURE_GET_PULL
/**
 * @brief     This function gets the pull state of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @param[out] driving specifies the driving current to be set to target GPIO.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_gpio_pull_t *pull;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_get_pull(gpio_pin, &pull);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_get_pull(hal_gpio_pin_t gpio_pin, hal_gpio_pull_t *pull_state);
#endif

/**
 * @brief     This function sets the analog function of the target GPIO.
 * @param[in] gpio_pin specifies the pin number to configure.
 * @param[in] analog function to be set to target GPIO.
 *            true: enable analog function; false: disable analog function.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_GPIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, the operation failed.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_set_analog(gpio_pin, true);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_set_analog(hal_gpio_pin_t gpio_pin, bool enable);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_GPIO_MODULE_ENABLED*/
#endif /* __HAL_GPIO_H__ */

