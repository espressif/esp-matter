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

/**************************************************************************//**
* \file <wiced_platform_audio_common.h>
* Type defines and interfaces common for audio hardware platform.
*
******************************************************************************/
#pragma once

enum platform_global_state {
    GLOBAL_POWER_UP_STATE = 0,
    GLOBAL_IDLE_STATE,
    GLOBAL_POWER_DOWN_STATE,
};
typedef uint8_t platform_global_state_t;

enum platform_button_id {
    /* Common Buttons */
    PLATFORM_BUTTON_POWER_ON = 0,
    PLATFORM_BUTTON_POWER_OFF,
    PLATFORM_BUTTON_PAIRING,

    PLATFORM_BUTTON_VOLUME_UP,
    PLATFORM_BUTTON_VOLUME_DOWN,

    /* WiCED HCI buttons. */
    PLATFORM_BUTTON_WICED_HCI_DETECT_ON,

    /* LRAC Key/buttons*/
    PLATFORM_BUTTON_LRAC_BUTTON_1_LONG,
    PLATFORM_BUTTON_LRAC_BUTTON_1_SHORT,
    PLATFORM_BUTTON_LRAC_BUTTON_1_REPEAT,
    PLATFORM_BUTTON_LRAC_BUTTON_2_LONG,
    PLATFORM_BUTTON_LRAC_BUTTON_2_SHORT,
    PLATFORM_BUTTON_LRAC_BUTTON_2_REPEAT,
    PLATFORM_BUTTON_LRAC_BUTTON_3_LONG,
    PLATFORM_BUTTON_LRAC_BUTTON_3_SHORT,
    PLATFORM_BUTTON_LRAC_BUTTON_3_REPEAT,
    PLATFORM_BUTTON_LRAC_BUTTON_4_LONG,
    PLATFORM_BUTTON_LRAC_BUTTON_4_REPEAT,
    /* */
    PLATFORM_BUTTON_LAST,
};
typedef uint8_t platform_button_id_t;

enum platform_audio_side {
    PLATFORM_AUDIO_SIDE_LEFT = 0,
    PLATFORM_AUDIO_SIDE_RIGHT,
};
typedef uint8_t platform_audio_side_t;

enum platform_codec_route {
    PLATFORM_CODEC_ROUTE_NONE = 0,
    PLATFORM_CODEC_ROUTE_CVSD,
    PLATFORM_CODEC_ROUTE_MSBC,
};
typedef uint8_t platform_codec_route_t;

enum platform_led_state {
    PLATFORM_LED_POWER_ON = 0,      /* Param: Primary/Secondary/Unknown */
    PLATFORM_LED_POWER_OFF,         /* No Param */
    PLATFORM_LED_PAIRING,           /* Param: Pairing/NonPairing */
    PLATFORM_LED_LRAC_CONNECTION,   /* Param: Connected/Disconnected */
    PLATFORM_LED_A2DP_CONNECTION,   /* Param: Connected/Disconnected */
    PLATFORM_LED_AVRC_CONNECTION,   /* Param: Connected/Disconnected */
    PLATFORM_LED_HFP_CONNECTION,    /* Param: Connected/Disconnected */
    PLATFORM_LED_HFP_VOICE_STATE,   /* Param: Voice/NoVoice */
    PLATFORM_LED_A2DP_STREAM_STATE, /* Param: Streaming/NotStreaming */
    PLATFORM_LED_CHARGER,           /* Param: Inserted/Removed/BatteryCharged */
};
typedef uint8_t platform_led_state_t;

enum platform_charger_event {
    PLATFORM_CHARGER_INSERTED = 0,
    PLATFORM_CHARGER_REMOVED,
    PLATFORM_CHARGER_CHARGE_COMPLETE,
};
typedef uint8_t platform_charger_event_t;

/*
 * platform_charger_callback_t callback definition
 */
typedef void (platform_charger_callback_t)(platform_charger_event_t event);

/*
 * platform_button_callback_t callback definition
 */
typedef wiced_bool_t (platform_button_callback_t)(platform_button_id_t button_id,uint32_t repeat_counter);

/*
 * platform_init
 */
wiced_result_t platform_init(void);

/*
 * wass_button_init
 */
wiced_result_t wass_button_init(platform_button_callback_t *p_callback);

/*
 * platform_charger_init
 */
wiced_result_t platform_charger_init(platform_charger_callback_t *p_callback);

/*
 * platform_state_set
 */
wiced_result_t platform_state_set(platform_global_state_t state);

/*
 * platform_state_get
 */
platform_global_state_t platform_state_get(void);

/*
 * platform_audio_side_set
 */
wiced_result_t platform_audio_side_set(platform_audio_side_t audio_side);

/*
 * platform_codec_route_set
 */
wiced_result_t platform_codec_route_set(platform_codec_route_t codec_route);

/*
 * platform_led_set
 */
void platform_led_set(platform_led_state_t led_state, uint8_t param);

/*
 * platform_handle_hci_command
 */
void platform_handle_hci_command(uint16_t cmd_opcode, uint8_t* p_data, uint32_t data_len);

/*
 * platform_switch_get
 */
wiced_result_t platform_switch_get(void *p_opaque, uint16_t *p_sync_data_len);

/*
 * platform_switch_set
 */
wiced_result_t platform_switch_set(void *p_opaque, uint16_t sync_data_len);

/*
 * wiced_platform_debug_enable
 */
void wiced_platform_debug_enable( void );

/*
 * platform_button_emulator
 *
 * Emulate the button event.
 */
void platform_button_emulator(platform_button_id_t button_id, uint32_t repeat_counter);
