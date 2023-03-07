/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

 /** @file
 *
 */

#ifndef _WICED_SLEEP_H_
#define _WICED_SLEEP_H_

/**
 * @addtogroup wiced_sleep_config       AIROC Sleep Configuration
 * @ingroup HardwareDrivers
 *
 *  Defines functions for using the AIROC Sleep Framework
 * @{
 */

#define WICED_SLEEP_MAX_TIME_TO_SLEEP   ~0

/** Wake sources.*/
#define WICED_SLEEP_WAKE_SOURCE_KEYSCAN  (1<<0)  /**< Enable wake from keyscan */
#define WICED_SLEEP_WAKE_SOURCE_QUAD     (1<<1)  /**< Enable wake from quadrature sensor */
#define WICED_SLEEP_WAKE_SOURCE_GPIO     (1<<2)  /**< Enable wake from GPIO */
#define WICED_SLEEP_WAKE_SOURCE_MASK     (WICED_SLEEP_WAKE_SOURCE_GPIO | \
                                         WICED_SLEEP_WAKE_SOURCE_KEYSCAN | \
                                         WICED_SLEEP_WAKE_SOURCE_QUAD) /**< All wake sources */

/** Boot mode */
typedef enum
{
    WICED_SLEEP_COLD_BOOT, /**< Cold boot */
    WICED_SLEEP_FAST_BOOT  /**< Fast boot */
}wiced_sleep_boot_type_t;

/** Sleep modes */
typedef enum
{
    WICED_SLEEP_MODE_NO_TRANSPORT, /**< Used for HID use cases. When a transport is connected, sleep is always disallowed*/
    WICED_SLEEP_MODE_TRANSPORT     /**< This mode allows sleep when transport is connected and uses device wake line to wake up*/
}wiced_sleep_mode_type_t;

/** Active interrupt level for Wake through GPIO*/
typedef enum
{
    WICED_SLEEP_WAKE_ACTIVE_LOW, /**< Active low interrupt wakes the chip */
    WICED_SLEEP_WAKE_ACTIVE_HIGH /**< Active high interrupt wakes the chip*/
}wiced_sleep_wake_type_t;

/** Sleep poll type */
typedef enum
{
    WICED_SLEEP_POLL_TIME_TO_SLEEP,      /**< Polling for maximum allowed sleep duration */
    WICED_SLEEP_POLL_SLEEP_PERMISSION    /**< Polling for permission to sleep */
} wiced_sleep_poll_type_t;

/** Sleep permission
*    Note: SHUTDOWN mode sleep puts the chip in the lowest power saving sleep mode. This turns off most
*    hardware including parts of SRAM. Hence, if the application requires to preserve some context data for it
*    to resume normal operation after wake up, app should store this context data in Always On Memory (the part that is not turned off).
*    This is done by declaring the context variable with the attribute  __attribute__ ((section(".data_in_retention_ram"))).
*    The memory in AON available is limited, the maximum that application can use are 256 bytes.
*    When mode WICED_SLEEP_ALLOWED_WITH_SHUTDOWN is selected, the FW puts the chip in Shutdown sleep mode
*    if that is possible, else the chip will be put in a non-shutdown sleep mode.
*    To prevent FW from attempting to put the chip in Shutdown sleep mode, select WICED_SLEEP_ALLOWED_WITHOUT_SHUTDOWN.
*/
typedef enum
{
    WICED_SLEEP_NOT_ALLOWED,               /**< Sleep is not allowed */
    WICED_SLEEP_ALLOWED_WITHOUT_SHUTDOWN,  /**< Sleep is allowed, but shutdown mode sleep is not allowed */
    WICED_SLEEP_ALLOWED_WITH_SHUTDOWN      /**< Slepp allowed, including shutdown mode sleep */
}wiced_sleep_permission_type_t;

/**
 *
 *  Application implements call back of this type to allow or disallow the chip to go to sleep.
 *
 * @param[in]       type:  Poll type (see #wiced_sleep_poll_type_t)
 *
 * @return          if type == WICED_SLEEP_POLL_TIME_TO_SLEEP, application should return the maximum time allowed to sleep in micro seconds.
 *                     WICED_SLEEP_MAX_TIME_TO_SLEEP allows the Firmware to determine the duration it can sleep.
                       Note: When allowing SDS sleep, always return WICED_SLEEP_MAX_TIME_TO_SLEEP.
 *                  if type == WICED_SLEEP_POLL_SLEEP_PERMISSION, application should return one of the values in wiced_sleep_permission_type_t
 *
 * Note:- Application shall return immediately, with the return value specifying whether
 *        it allows/disallows sleep. Immediate return is required to allow the maximum
 *        time to sleep. Applications shall ensure that all peripheral activity (on PUART, SPI, .. )
 *        is paused before allowing sleep. Any pending activity on the peripherals
 *        is lost when the device goes into sleep.
 */
typedef uint32_t (*wiced_sleep_allow_check_callback ) (wiced_sleep_poll_type_t type );


/** Sleep configuration parameters */
typedef struct
{
    wiced_sleep_mode_type_t                  sleep_mode;             /**< Requested sleep mode */

    /* host_wake_mode, device_wake_mode and device_wake_gpio_num are applicable only
    in case of WICED_SLEEP_MODE_TRANSPORT. For WICED_SLEEP_MODE_NO_TRANSPORT
    use case, application should configure wake pin using
    wiced_hal_gpio_xxx APIS and/or wiced_hal_key_scan_xxx APIs and/or
    wiced_hal_quadrature_xxx depending on LHL, keyscan, qudrature hardware as
    wake source */
    wiced_sleep_wake_type_t                  host_wake_mode;         /**< Active level for host wake */
    wiced_sleep_wake_type_t                  device_wake_mode;       /**< Active level for device wake */
    uint8_t                                  device_wake_source;     /**< Device wake source(s). GPIO mandatory for
                                                                          WICED_SLEEP_MODE_TRANSPORT */
    uint32_t                                 device_wake_gpio_num;   /**< GPIO# for host to wake, mandatory for
                                                                          WICED_SLEEP_MODE_TRANSPORT */
    wiced_sleep_allow_check_callback         sleep_permit_handler;   /**< Call back to be called by sleep framework
                                                                          to poll for sleep permission */

}wiced_sleep_config_t;


#ifdef __cplusplus
extern "C" {
#endif


/** API to configure sleep mode parameters.
 *
 * @param[in]       p_sleep_config: see @wiced_sleep_config_t
 *
 * @return          WICED_SUCCESS or WICED_ERROR
 */
wiced_result_t wiced_sleep_configure( wiced_sleep_config_t *p_sleep_config );


/** API to request reboot type.
 *
 * Returns Cold boot or Warm/Fast boot.
 *
 * @return          wiced_sleep_boot_type_t
 */
wiced_sleep_boot_type_t wiced_sleep_get_boot_mode(void);

/**
* Function         wiced_hal_set_external_lpo_oscillator_bias
*
*                  Set external LPO oscillator bias. Default 0 = 50nA.
*
* @param[in]       bias       max allowed value is 3 (0=50nA, 1=100nA, 2=150nA, 3=200nA)
*
* @return
*                  WICED_SUCCESS if bias is set successfully, WICED_ERROR otherwise.
*/
wiced_result_t wiced_hal_set_external_lpo_oscillator_bias(uint8_t bias);

/** @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //_WICED_SLEEP_H_
