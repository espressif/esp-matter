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
 * Bluetooth AVRC Remote control Target API
 *
 */

#ifndef WICED_BT_RC_TG_H_
#define WICED_BT_RC_TG_H_

#include "wiced_bt_cfg.h"
#include "wiced_bt_avrc_defs.h"
/****************************************************************************/
/**
 * AVRC remote control functions
 *
 * @addtogroup  wicedbt_avrc_tg       AVRCP Target Role (TG)
 * @ingroup wicedbt_avrc
 *
 * @{
 */
/****************************************************************************/

/******************************************************************************
 *                         Supported features
 ******************************************************************************/

/* APP_AVRCP10 is defined in make target when AVRCP version needs to be 1.0, default is AVRCP 1.3.
 * AVRCP 1.3 adds features to display of track (song) info, player control (for repeat, shuffle, etc.)
 * and display of play state (pause, play, song position)
 */

#ifndef APP_AVRCP10
/* Whether track info is supported */
#define APP_AVRC_TRACK_INFO_SUPPORTED

/* Whether player state change (pause/play/stop, play position) is supported */
#define APP_AVRC_PLAY_STATUS_SUPPORTED

/* Whether player settings (repeat/shuffle/etc.) is supported */
#define APP_AVRC_SETTING_CHANGE_SUPPORTED

#ifdef APP_AVRC_PLAY_STATUS_SUPPORTED
/* Whether player position updates are supported */
#define APP_AVRC_TRACK_PLAY_POS_CHANGE_SUPPORTED
#endif

/* Additional capabilities could be supported by application, compiled out */
/*
#define APP_AVRC_TRACK_REACHED_END_SUPPORTED
#define APP_AVRC_TRACK_REACHED_START_SUPPORTED
#define APP_AVRC_BATTERY_STATUS_SUPPORTED
#define APP_AVRC_SYSTEM_STATUS_SUPPORTED
*/
#endif /* APP_AVRCP10 */

#define MAX_AVRCP_VOLUME_LEVEL  0x7f


/*****************Track info data structures********************/
/* If track info is supported ..*/
#ifdef APP_AVRC_TRACK_INFO_SUPPORTED

/* Maximum number of attributes types supported in track info */
/* Track info attributes include Track title, Artist, Album, Genre, Total tracks, Current track number, Playing time*/
#define APP_AVRC_MAX_ATTR   AVRC_MAX_NUM_MEDIA_ATTR_ID

/* Maximum string length of track info attribute */
#define APP_AVRC_MAX_ATTR_LEN AVRC_MAX_MEDIA_ATTR_LEN

/* Track info attributes */
typedef struct {

    /* Type of track attribute - Track title, Artist, Album, Genre, Total tracks, Current track number, Playing time*/
    uint8_t    attr_id;
    /* String length */
    uint8_t    str_len;
    /* String name of attribute in UTF8 */
    uint8_t    p_str[APP_AVRC_MAX_ATTR_LEN+1];
} wiced_bt_avrc_tg_track_attr_t;

/* Track info array */
typedef struct {
    /* total number of track info attributes supported by app */
    uint8_t             app_attr_count;
    /* array of track info attributes */
    wiced_bt_avrc_tg_track_attr_t app_track_attr[APP_AVRC_MAX_ATTR+1];
} wiced_bt_avrc_tg_track_info_t;

#endif
/***************************************************************/

/********************Player settings data structures************/
/* If player settings (repeat/shuffle/etc.) is supported ... */
#ifdef APP_AVRC_SETTING_CHANGE_SUPPORTED

/* Maximum number of attribute values supported, see below */
#define APP_AVRC_MAX_APP_ATTR_SIZE 4

/* Player settings attribute value definition struct */
typedef struct {
    /* Attribute type (repeat, shuffle, etc.)*/
    uint8_t                 attr_id;
    /* Number of supported values for each attribute, example : Repeat Off, Repeat single track, Repeat all tracks, etc. */
    uint8_t                 num_val;
    /* Array of supported values for each attribute type */
    uint8_t                 vals[APP_AVRC_MAX_APP_ATTR_SIZE];
    /* current attribute value set by user */
    uint8_t                 curr_value;
} wiced_bt_avrc_tg_player_attr_t;

#endif
/***************************************************************/

/********************Player status data structures**************/
/* If player state change (pause/play/stop, play position) is supported ... */
#ifdef APP_AVRC_PLAY_STATUS_SUPPORTED
typedef struct {
    /* Song length */
    uint32_t                song_len;
    /* Song position */
    uint32_t                song_pos;
    /* Play state (Play, pause, stop */
    uint8_t                 play_state;
} wiced_bt_avrc_tg_play_status_t;
#endif
/***************************************************************/

/* Event handling, events sent to the app */

/* Event IDs */
#define APP_AVRC_EVENT_DEVICE_CONNECTED          1  /* peer device connected */
#define APP_AVRC_EVENT_REPEAT_SETTINGS_CHANGED   2  /* peer changed repeat settings */
#define APP_AVRC_EVENT_SHUFFLE_SETTINGS_CHANGED  3  /* peer changed shuffle settings */
#define APP_AVRC_EVENT_DEVICE_DISCONNECTED       4  /* peer device disconnected */
#define APP_AVRC_EVENT_PASSTHROUGH_RESPONSE      5  /* passthrough command response from peer */
#define APP_AVRC_EVENT_PASSTHROUGH_CMD           6  /* passthrough command response from peer */
#define APP_AVRC_EVENT_ABS_VOLUME_CHANGED        7  /* peer device changed absolute volume */

#define APP_AVRC_EVENT_ABS_VOL_CHANGED           APP_AVRC_EVENT_ABS_VOLUME_CHANGED // TODO HARC

/* Paththrough Cmd sub event */
#define APP_AVRC_EVENT_PASSTHROUGH_CMD_PLAY             1  /* Passthrough Command Play */
#define APP_AVRC_EVENT_PASSTHROUGH_CMD_PAUSE            2  /* Passthrough Command Pause */
#define APP_AVRC_EVENT_PASSTHROUGH_CMD_STOP             3  /* Passthrough Command Stop */
#define APP_AVRC_EVENT_PASSTHROUGH_CMD_NEXT_TRACK       4  /* Passthrough Command Next Track */
#define APP_AVRC_EVENT_PASSTHROUGH_CMD_PREVIOUS_TRACK   5  /* Passthrough Command Previous Track */

/* Passthrough Command event data */
typedef struct {
    uint16_t                    handle;
    uint8_t                     command;
} wiced_bt_avrc_tg_passthrough_cmd_t;

/* Absolute Volume event data */
typedef struct {
    uint16_t                    handle;
    uint8_t                     volume;
} wiced_bt_avrc_tg_absolute_volume_t;

/* Event data */
typedef struct {
    /* Peer connected device info, sent to MCU app (used with APP_AVRC_EVENT_DEVICE_CONNECTED) */
    wiced_bt_device_address_t   bd_addr;
    uint16_t                    handle;
    /* Repeat or shuffle setting, senf to MCU app (used with APP_AVRC_EVENT_REPEAT_SETTINGS_CHANGED or APP_AVRC_EVENT_SHUFFLE_SETTINGS_CHANGED) */
    uint8_t                     setting_val;
    /* AVRCP Supported Features Attribute */
    wiced_bool_t                attribute_search_completed;
    uint16_t                    supported_features;
    /* Passthrough command response */
    uint8_t                     passthrough_response;
    /* Passthrough Command received (used with APP_AVRC_EVENT_PASSTHROUGH_CMD event) */
    wiced_bt_avrc_tg_passthrough_cmd_t passthrough_command;
    /* Absolute Volume [0..100] */
    wiced_bt_avrc_tg_absolute_volume_t absolute_volume;

} wiced_bt_rc_event_t;

/*
 * AVRC event callback to receive events from AVRC profile. Application must implement this api
 */
typedef void ( wiced_bt_avrc_tg_event_cback_t) (uint8_t event_id,  wiced_bt_rc_event_t *_event);


/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Function        wiced_bt_avrc_tg_init

** Description    Called to initialize AV RC profile
*******************************************************************************/
void wiced_bt_avrc_tg_init( wiced_bt_avrc_tg_event_cback_t *p_cb );

/*******************************************************************************
* Function        wiced_bt_avrc_tg_register

** Description    Called to register AVRC profile with the BT stack
*******************************************************************************/
void wiced_bt_avrc_tg_register(void);

/*******************************************************************************
* Function        wiced_bt_avrc_tg_initiate_open

** Description    Called to initiate connection to given BDA
*******************************************************************************/
void wiced_bt_avrc_tg_initiate_open(wiced_bt_device_address_t peer_addr);

/*******************************************************************************
* Function        wiced_bt_avrc_tg_initiate_close

** Description    Called to disconnect AVRC connection
*******************************************************************************/
void wiced_bt_avrc_tg_initiate_close( void );

#ifdef APP_AVRC_TRACK_INFO_SUPPORTED
/*******************************************************************************
* Function        wiced_bt_rc_set_track_info

** Description    Called to set current playing track information
*******************************************************************************/
void wiced_bt_rc_set_track_info(wiced_bt_avrc_tg_track_attr_t *p_track_attr);
#endif

#ifdef APP_AVRC_SETTING_CHANGE_SUPPORTED
/*******************************************************************************
* Function        wiced_bt_rc_set_player_settings

** Description    Called to set player settings (repeat, shuffle, etc).
*******************************************************************************/
void wiced_bt_rc_set_player_settings(wiced_bt_avrc_tg_player_attr_t *p_info);

/*******************************************************************************
* Function        wiced_bt_rc_player_setting_changed

** Description    Called when player setting (repeat, shuffle) is changed
** Paramaters
    uint8_t attr_id : Atrribute type (repeat, shuffle, etc.)
    uint8_t value   : Attrbute value (off, single, all, etc.)
*******************************************************************************/
void wiced_bt_rc_player_setting_changed(uint8_t attr_id, uint8_t value);

#endif

#ifdef APP_AVRC_PLAY_STATUS_SUPPORTED
/*******************************************************************************
* Function        wiced_bt_rc_set_player_status

** Description    Called to set player status (pause/play/stop, song position)
*******************************************************************************/
void wiced_bt_rc_set_player_status(wiced_bt_avrc_tg_play_status_t *p_info);

#endif

#ifdef APP_AVRC_TRACK_INFO_SUPPORTED
/*******************************************************************************
* Function        wiced_bt_rc_track_changed

** Description    Called when current track is changed
*******************************************************************************/
void wiced_bt_rc_track_changed(void);
#endif

/*******************************************************************************
* Function        wiced_bt_avrc_tg_register_absolute_volume_change

** Description    register for absolute volume change notifications
*******************************************************************************/
void wiced_bt_avrc_tg_register_absolute_volume_change(void);

/*******************************************************************************
* Function        wiced_bt_avrc_tg_absolute_volume_change

** Description    Called when volume is changed, send Absolute volume request to peer

** Paramaters
    uint16_t handle : handle of connection
    uint8_t  volume : volume set by application as a percentage

*******************************************************************************/
wiced_result_t wiced_bt_avrc_tg_absolute_volume_changed(uint16_t handle,   uint8_t  volume );

/*******************************************************************************
* Function        wiced_bt_avrc_tg_is_peer_absolute_volume_capable

** Description    return non zero if peer is absolute volume capable
*******************************************************************************/
uint8_t wiced_bt_avrc_tg_is_peer_absolute_volume_capable( void );

#ifdef CATEGORY_2_PASSTROUGH
/*******************************************************************************
* Function        wiced_bt_avrc_tg_volume_button_press

** Description    Called when volume up/down button is pressed
** Paramaters
    uint8_t op_id : operation ID  AVRC_ID_VOL_UP or AVRC_ID_VOL_DOWN
*******************************************************************************/
wiced_result_t wiced_bt_avrc_tg_volume_button_press(uint8_t op_id);
#endif
/**@} wicedbt_avrc_tg */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WICED_BT_RC_TG_H_ */
