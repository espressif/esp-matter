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
#include <stdio.h>

#include "platform_retarget_lock.h"
#include "spar_utils.h"
#ifdef BLE_OTA_FW_UPGRADE
#include <wiced_bt_ota_firmware_upgrade.h>
#endif
#include "wiced_bt_stack.h"
#include "wiced_hal_cpu_clk.h"
#include "wiced_hal_duart.h"
#include "wiced_hal_puart.h"
#include "wiced_hal_wdog.h"
#include "wiced_platform.h"
#include "wiced_platform_bt_cfg.h"
#include "wiced_platform_memory.h"
#include "wiced_result.h"
#include "wiced_rtos.h"

#define cr_pad_fcn_ctl_adr0                            0x00320088
#define cr_pad_fcn_ctl_adr2                            0x00320090
#define cr_pad_fcn_ctl_ext_adr                         0x00320218
#define cr_pad_fcn_ctl_lhl_adr                         0x0032021c
#define mia_dbg_adr                                    0x0033801c
#define iocfg_premux_5_adr                             0x003383c0
#define uart_hc_data_adr                               0x00352320

extern wiced_platform_gpio_t platform_gpio_pins[];
extern wiced_platform_led_config_t platform_led[];
extern wiced_platform_button_config_t platform_button[];
extern wiced_platform_gpio_config_t platform_gpio[];
extern size_t platform_gpio_pin_count;
extern size_t led_count;
extern size_t button_count;
extern size_t gpio_count;

#define WICED_PLATFORM_DEBUG_UART_BAUDRATE  3000000

#define WICED_PLATFORM_APPLICATION_THREAD_PRIORITY          (3)
#ifdef CHIP_HAVE_CONFIG_H
#define WICED_PLATFORM_APPLICATION_THREAD_STACK_SIZE        (0x00000800)
#else
#define WICED_PLATFORM_APPLICATION_THREAD_STACK_SIZE        (0x00001300)
#endif
#define WICED_PLATFORM_APPLICATION_THREAD_EVENT_WAIT_TIME   (5) // ms

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE   (8)
#endif // BITS_PER_BYTE

extern void application_start(void);
extern void wiced_app_hal_init(void );

typedef struct wiced_platform_bt_dev_vse_cb
{
    uint8_t                             evt_code;   // event code
    wiced_bt_dev_vse_callback_t         *p_cb;

    struct wiced_platform_bt_dev_vse_cb *p_next;
} wiced_platform_bt_dev_vse_cb_t;

typedef struct wiced_platform_app_thread_event_handler
{
    uint32_t                                        event_code;
    wiced_platform_application_thread_event_handler *p_hanlder;

    struct wiced_platform_app_thread_event_handler  *p_next;
} wiced_platform_app_thread_event_handler_t;

typedef struct wiced_platform_cb
{
    wiced_bt_management_cback_t     *p_bt_management_cb_user_handler;
    wiced_platform_bt_dev_vse_cb_t  *p_bt_dev_vse_cb_list;
    wiced_thread_t                  *p_app_thread_instance;
    wiced_event_flags_t             *p_app_thread_event_flags_handle;

    struct
    {
        uint32_t                                    assigned_event_code_shiftment;
        wiced_platform_app_thread_event_handler_t   *p_list;
    } app_thread_event;

    wiced_platform_application_thread_specific_handler  *p_app_specific_handler;
} wiced_platform_cb_t;

static wiced_platform_cb_t wiced_platform_cb = {0};

/* utility functions */

/**
 *  \brief Provide utility function for application to register for cb upon button interrupt
 *
 *  \param [in] button select a button from wiced_platform_button_number_t
 *  \param [in] userfn Provide the call back function
 *  \param [in] userdata Data to be provided with the call back
 *  \param [in] trigger_edge To configure interrupt on rising/falling/dual edge
 *
 *  \return none
 *
 */
void wiced_platform_register_button_callback(wiced_platform_button_number_t button, void (*userfn)(void*, uint8_t), void* userdata,
                wiced_platform_button_interrupt_edge_t trigger_edge)
{
    if(button < button_count)
    {
        wiced_hal_gpio_register_pin_for_interrupt(*platform_button[button].gpio, userfn, userdata);
        wiced_hal_gpio_configure_pin(*platform_button[button].gpio, (platform_button[button].config | trigger_edge), platform_button[button].default_state);
    }
}

/**
 *  \brief Return state of the pin when button is pressed
 *
 *  \param [in] button select a button from wiced_platform_button_number_t
 *
 *  \return button pressed value
 *
 */
uint32_t wiced_platform_get_button_pressed_value(wiced_platform_button_number_t button)
{
	return platform_button[button].button_pressed_value;
}

static void wiced_platform_bt_hci_vse_callback(uint8_t len, uint8_t *p)
{
    uint8_t evt_code;
    wiced_platform_bt_dev_vse_cb_t *p_index;

    if (p == NULL)
        return;

    /* Retrieve the event code. */
    if (len < sizeof(evt_code))
    {
        return;
    }

    evt_code = *p++;
    len--;

    /* Call the target event code handler. */
    p_index = wiced_platform_cb.p_bt_dev_vse_cb_list;

    while (p_index)
    {
        if (p_index->evt_code == evt_code)
        {
            if (p_index->p_cb)
            {
                (*p_index->p_cb)(len, p);
            }
        }

        p_index = p_index->p_next;
    }
}

int __attribute__((weak)) main(int argc, char *argv[])
{
    (void) (argc);
    (void) (argv);

    application_start();
}

/*
 * Application thread main handler.
 */
static void wiced_platform_application_thread_handler(uint32_t arg)
{
    (void) (arg);

    printf("%s\n", __FUNCTION__);

    /* Call application main function. */
    main(0, NULL);
}

/*
 * Create a thread for user application.
 */
static void wiced_platform_application_thread_create(void)
{
    wiced_result_t status;

    wiced_platform_cb.p_app_thread_instance = wiced_rtos_create_thread();

    if (!wiced_platform_cb.p_app_thread_instance)
    {
        printf("%s: Fail to create app thread.\n", __FUNCTION__);
        return;
    }

    status = wiced_rtos_init_thread(wiced_platform_cb.p_app_thread_instance,
                                    WICED_PLATFORM_APPLICATION_THREAD_PRIORITY,
                                    "WiCED App",
                                    wiced_platform_application_thread_handler,
                                    WICED_PLATFORM_APPLICATION_THREAD_STACK_SIZE,
                                    (void *) NULL);

    if (status != WICED_SUCCESS)
    {
        printf("%s: Fail to init. app thread (%d).\n", __FUNCTION__, status);
        return;
    }

    /* Create application thread event flag handle. */
    wiced_platform_cb.p_app_thread_event_flags_handle = wiced_rtos_create_event_flags();

    if (!wiced_platform_cb.p_app_thread_event_flags_handle)
    {
        printf("%s: Fail to create app thread event flag handle.\n", __FUNCTION__);
        return;
    }

    /* Initialize the application thread event flags. */
    status = wiced_rtos_init_event_flags(wiced_platform_cb.p_app_thread_event_flags_handle);

    if (status != WICED_SUCCESS)
    {
        printf("%s: Fail to init. app thread event flags.\n", __FUNCTION__);
    }
}

static wiced_result_t wiced_platform_bt_management_callback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t status = WICED_SUCCESS;
    wiced_bt_ble_advert_mode_t *p_mode;

    switch (event)
    {
    case BTM_ENABLED_EVT:
        /* Register the HCI VSE callback. */
        status = wiced_bt_dev_register_vse_callback(wiced_platform_bt_hci_vse_callback);
        if (status != WICED_SUCCESS)
        {
            printf("%s: Fail to register HCI VSE callback (%d).\n", __FUNCTION__, status);
            return status;
        }

#ifdef BLE_OTA_FW_UPGRADE
        if (!wiced_ota_fw_upgrade_init(NULL, NULL, NULL))
        {
            printf("wiced_ota_fw_upgrade_init failed\n");
        }
#endif

        /* Disable watchdog.*/
        wiced_hal_wdog_disable();

        /* Create a thread for application. */
        wiced_platform_application_thread_create();

        return WICED_SUCCESS;

     case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
        p_mode = &p_event_data->ble_advert_state_changed;
        printf( "Advertisement State Change: %d\n", *p_mode);
        if ( *p_mode == BTM_BLE_ADVERT_OFF )
        {
            printf( "Advertisement State BTM_BLE_ADVERT_OFF \n");
        }
        break;


    default:
        printf("%s: event(%d).\n", __FUNCTION__, event);
        break;
    }

    /* Call the user application stack event handler. */
    if (wiced_platform_cb.p_bt_management_cb_user_handler)
    {
        return (*wiced_platform_cb.p_bt_management_cb_user_handler)(event, p_event_data);
    }
    else
    {
        return WICED_SUCCESS;
    }
}

/**
 *  \brief Initialize all the required pins and configure their functionality
 *
 *  \return none
 *
 */
void wiced_platform_init(void)
{
    uint32_t i = 0;

    wiced_app_hal_init();

    /* Configure pins available on the platform with the chosen functionality */
    for (i = 0; i < platform_gpio_pin_count; i++)
    {
        wiced_hal_gpio_select_function(platform_gpio_pins[i].gpio_pin, platform_gpio_pins[i].functionality);
    }
    /* Initialize LEDs and turn off by default */
    for (i = 0; i < led_count; i++)
    {
        wiced_hal_gpio_configure_pin(*platform_led[i].gpio, platform_led[i].config, platform_led[i].default_state);
    }

    /* Initialize buttons with the default configuration */
    for (i = 0; i < button_count; i++)
    {
        wiced_hal_gpio_configure_pin(*platform_button[i].gpio, (platform_button[i].config), platform_button[i].default_state);
    }

    /* Initialize GPIOs with the default configuration */
    for (i = 0; i < gpio_count; i++)
    {
        wiced_hal_gpio_configure_pin(*platform_gpio[i].gpio, (platform_gpio[i].config), platform_gpio[i].default_state);
    }

    /* disable watchdog, set up SWD, wait for attach if ENABLE_DEBUG */
    SETUP_APP_FOR_DEBUG_IF_DEBUG_ENABLED();
    BUSY_WAIT_TILL_MANUAL_CONTINUE_IF_DEBUG_ENABLED();

    platform_retarget_lock_init();

#ifdef USE_BT_UART_AS_CLI
    /* Use PUART pin as Debug UART. */
    wiced_hal_duart_init(WICED_PUART_TXD, MAX_NUM_OF_GPIO, WICED_PLATFORM_DEBUG_UART_BAUDRATE);

    /* PUART through BT_UART */
    REG32(iocfg_premux_5_adr) &= ~((0xff) << 8);
    REG32(iocfg_premux_5_adr) &= ~((0xff) << 16);
    REG32(cr_pad_fcn_ctl_ext_adr) = (REG32(cr_pad_fcn_ctl_ext_adr) & 0xfffffff0);
    REG32(cr_pad_fcn_ctl_adr2) = (REG32(cr_pad_fcn_ctl_adr2) & 0xffff0000) | 0x5555;
    REG32(cr_pad_fcn_ctl_lhl_adr) = (REG32(cr_pad_fcn_ctl_lhl_adr) & 0xfffffff0);
    REG32(mia_dbg_adr) = (REG32(mia_dbg_adr) & 0xfffffdff);
#else
    /* Initialize Debug UART interface. */
    wiced_hal_duart_init(WICED_DEBUG_PIN, MAX_NUM_OF_GPIO, WICED_PLATFORM_DEBUG_UART_BAUDRATE);
#endif

    wiced_update_cpu_clock(WICED_TRUE, WICED_CPU_CLK_96MHZ);

    /* Initialize the Stack. */
    wiced_bt_stack_init(wiced_platform_bt_management_callback,
                        &wiced_platform_bt_cfg_settings,
                        wiced_platform_bt_cfg_buf_pools);
}

/**
 *  \brief Register the BT stack event handler.
 *
 *  \param [in] user specific handler (callback)
 *
 *  \return None
 *
 */
void wiced_platform_register_bt_management_callback(wiced_bt_management_cback_t *p_callback)
{
    wiced_platform_cb.p_bt_management_cb_user_handler = p_callback;
}

/**
 *  \brief Register the HCI VSE event handler
 *
 *  \param [in] HCI VSE event code
 *  \param [in] target event code handler (callback)
 *
 *  \return WICED_TRUE  : Success
 *          WICED_FALSE : Fail
 *
 */
wiced_bool_t wiced_platform_register_hci_vse_callback(uint8_t evt_code, wiced_bt_dev_vse_callback_t *p_callback)
{
    wiced_platform_bt_dev_vse_cb_t *p_index;

    /* Allocate memory. */
    p_index = (wiced_platform_bt_dev_vse_cb_t *) wiced_platform_memory_allocate(sizeof(wiced_platform_bt_dev_vse_cb_t));
    if (p_index == NULL)
    {
        return WICED_FALSE;
    }

    /* Fill in paramter. */
    p_index->evt_code   = evt_code;
    p_index->p_cb       = p_callback;

    /* Add to the head of list. */
    p_index->p_next = wiced_platform_cb.p_bt_dev_vse_cb_list;
    wiced_platform_cb.p_bt_dev_vse_cb_list = p_index;

    return WICED_TRUE;
}

/**
 *  \brief Helper function to check if current utility is executed under application thread.
 *
 *  \return WICED_TRUE  : Current utility is executed under application thread.
 *          WICED_FALSE : Current utility is NOT executed under application thread.
 *
 */
wiced_bool_t wiced_platform_application_thread_check(void)
{

#ifdef CHIP_HAVE_CONFIG_H
    /*
     * Skip the check for Matter applications because
     * both Matter and Thread tasks would access OpenThread APIs.
     */
    return WICED_TRUE;
#else
    if (wiced_rtos_is_current_thread(wiced_platform_cb.p_app_thread_instance) == WICED_SUCCESS)
    {
        return WICED_TRUE;
    }
    else
    {
        printf("Err: Current operation is NOT under application thread.\n");
        return WICED_FALSE;
    }
#endif
}

/**
 *  \brief Get application thread event code and the register the corresponding event handler if
 *         provided.
 *
 *  \param [out]    allocated event code
 *  \param [in]     user specified event handler
 *
 *  \return WICED_TRUE  : Success
 *          WICED_FALSE : Fail.
 *
 */
wiced_bool_t wiced_platform_application_thread_event_register(uint32_t *p_event_code,
        wiced_platform_application_thread_event_handler *p_event_handler)
{
    wiced_platform_app_thread_event_handler_t *p_new;

    /* Check parameter. */
    if (!p_event_code)
    {
        return WICED_FALSE;
    }

    /* Check availability of event code. */
    if (wiced_platform_cb.app_thread_event.assigned_event_code_shiftment ==
        (sizeof(uint32_t) * BITS_PER_BYTE))
    {
        return WICED_FALSE;
    }

    if (!p_event_handler)
    {
        *p_event_code = (1 << wiced_platform_cb.app_thread_event.assigned_event_code_shiftment++);

        return WICED_TRUE;
    }

    /* Allocate memory. */
    p_new = (wiced_platform_app_thread_event_handler_t *) wiced_platform_memory_allocate(sizeof(wiced_platform_app_thread_event_handler_t));

    if (!p_new)
    {
        return WICED_FALSE;
    }

    p_new->event_code   = (1 << wiced_platform_cb.app_thread_event.assigned_event_code_shiftment++);
    p_new->p_hanlder    = p_event_handler;
    p_new->p_next       = wiced_platform_cb.app_thread_event.p_list;

    wiced_platform_cb.app_thread_event.p_list = p_new;

    *p_event_code = p_new->event_code;

    return WICED_TRUE;
}

/**
 *  \brief Set an application thread event
 *
 *  \param [in] event code (get by register utility)
 *
 */
void wiced_platform_application_thread_event_set(uint32_t event_code)
{
    assert(!wiced_rtos_check_for_stack_overflow());

    if (wiced_rtos_set_event_flags(wiced_platform_cb.p_app_thread_event_flags_handle, event_code) != WICED_SUCCESS)
    {
        printf("Err: Fail to set app thread event (%lu).\n", event_code);
    }
}

/**
 *  \brief Wait an application thread event
 *
 *  \param [in] event code (get by register utility)
 *
 */
void wiced_platform_application_thread_event_wait(uint32_t event_code)
{
    uint32_t flags_set;

    wiced_rtos_wait_for_event_flags(wiced_platform_cb.p_app_thread_event_flags_handle,
                                    event_code,
                                    &flags_set,
                                    WICED_TRUE,
                                    WAIT_FOR_ALL_EVENTS,
                                    WICED_WAIT_FOREVER);
}

/**
 * \brief Register a user application periodical handler under application thread.
 *        The register handler will be executed periodically (defined in
 *        WICED_PLATFORM_APPLICATION_THREAD_EVENT_WAIT_TIME) if the application thread
 *        is id idle state.
 *
 * @param p_handler - user application handler
 *
 */
void wiced_platform_application_thread_specific_handler_register(wiced_platform_application_thread_specific_handler *p_handler)
{
    wiced_platform_cb.p_app_specific_handler = p_handler;
}

void wiced_platform_application_thread_event_dispatch(void)
{
    uint32_t flags_set;
    wiced_platform_app_thread_event_handler_t *p_index;

    if (wiced_platform_cb.p_app_specific_handler)
    {
        (*wiced_platform_cb.p_app_specific_handler)();
    }

    /* Wait for event. */
    flags_set = 0;
    wiced_rtos_wait_for_event_flags(wiced_platform_cb.p_app_thread_event_flags_handle,
            0xffffffff,
            &flags_set,
            WICED_TRUE,
            WAIT_FOR_ANY_EVENT,
            WICED_PLATFORM_APPLICATION_THREAD_EVENT_WAIT_TIME);

    /* Process this event if registered. */
    p_index = wiced_platform_cb.app_thread_event.p_list;
    while (p_index != NULL && flags_set != 0)
    {
        if (p_index->event_code & flags_set)
        {
            (*p_index->p_hanlder)();

            flags_set &= ~p_index->event_code;
        }

        p_index = p_index->p_next;
    }
}

void wiced_platform_puart_post_config(void)
{
    wiced_hal_puart_flow_off();

#ifdef USE_BT_UART_AS_CLI
    // INTR source from core domain
    REG32(mia_dbg_adr) = (REG32(mia_dbg_adr) & 0xfffffdff);
#endif
}

void wiced_platform_puart_init(void (*puart_rx_cbk)(void*))
{
    wiced_hal_puart_init();
#ifdef VALTO_ENABLED
    wiced_hal_puart_configuration(115200, PARITY_NONE, STOP_BIT_1);
#else
    wiced_hal_puart_configuration(3000000, PARITY_NONE, STOP_BIT_2);
#endif
    wiced_hal_puart_register_interrupt(puart_rx_cbk);
    wiced_hal_puart_set_watermark_level(1);
    wiced_platform_puart_post_config();
}
