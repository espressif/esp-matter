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
 * Bluetooth AVRC Remote Control Application Programming Interface
 *
 * @addtogroup  wicedbt_avrc_ct       AVRCP Controller Role (CT)
 * @ingroup wicedbt_avrc
 *
 * AIROC Bluetooth AVRC Remote Control CT Functions
 *
 * @{
 */

#pragma once

#include "wiced.h"
#include "wiced_bt_types.h"
#include "wiced_result.h"
#include "bt_types.h"
#include "wiced_bt_avrc_defs.h"
#include "wiced_bt_avrc.h"


/*****************************************************************************
**  Constants and data types
*****************************************************************************/

#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

/** AVRC remote control feature mask */
typedef enum
{
    REMOTE_CONTROL_FEATURE_TARGET       = 0x0001, /**< Select to support the features of a Target */
    REMOTE_CONTROL_FEATURE_CONTROLLER   = 0x0002, /**< Select to support the features of a Controller */
    /* TODO: We need to add the AVRCP feature bits */
} wiced_bt_avrc_ct_features_t;

/** AVRC remote control connection state */
typedef enum
{
    REMOTE_CONTROL_DISCONNECTED = 0, /**< Remote control is disconnected from peer */
    REMOTE_CONTROL_CONNECTED    = 1, /**< Remote control is connected to peer */
    REMOTE_CONTROL_INITIALIZED  = 2  /**< Remote control is idle (not connected to any peer) */
} wiced_bt_avrc_ct_connection_state_t;

/** Callback for connection state */
typedef void (*wiced_bt_avrc_ct_connection_state_cback_t)( uint8_t handle, wiced_bt_device_address_t remote_addr,
                            wiced_result_t status, wiced_bt_avrc_ct_connection_state_t connection_state,
                            uint32_t peer_features);

/** Response callback from peer device for AVRCP commands */
typedef void (*wiced_bt_avrc_ct_rsp_cback_t)    ( uint8_t handle, wiced_bt_avrc_response_t *avrc_rsp);

/** Callback when peer device sends AVRCP commands */
typedef void (*wiced_bt_avrc_ct_cmd_cback_t)    ( uint8_t handle, wiced_bt_avrc_command_t *avrc_cmd);

/** Callback when peer device sends response to AVRCP passthrough commands */
typedef void (*wiced_bt_avrc_ct_pt_rsp_cback_t) ( uint8_t handle, wiced_bt_avrc_msg_pass_t *avrc_pass_rsp );

#if AVRC_ADV_CTRL_INCLUDED == TRUE
/* Callback used to indicates some peer features */
typedef enum
{
    /* This event is sent to indicate that the peer device supports Absolute Volume */
    WICED_BT_AVRC_CT_FEATURES_ABS_VOL_SUPPORTED = 1,
} wiced_bt_avrc_ct_features_event_t;

/* Data associated with WICED_BT_AVRC_CT_FEATURES_ABS_VOL_SUPPORTED event */
typedef struct
{
    uint8_t handle;
    wiced_bool_t supported;
} wiced_bt_avrc_ct_features_abs_vol_t;

typedef union
{
    wiced_bt_avrc_ct_features_abs_vol_t abs_vol_supported;
} wiced_bt_avrc_ct_features_data_t;

typedef void (*wiced_bt_avrc_ct_features_cback_t) (wiced_bt_avrc_ct_features_event_t event,
        wiced_bt_avrc_ct_features_data_t *p_data);
#endif /* AVRC_ADV_CTRL_INCLUDED == TRUE */

/** Callback when peer device sends AVRCP passthrough commands op code */
typedef void (*wiced_bt_avrc_ct_pt_evt_cback_t)(uint8_t handle, uint8_t op_id);

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/**
 * AVRC remote control functions
 *
 */
/****************************************************************************/

/**
 * Function         wiced_bt_avrc_ct_init
 *
 *                  Initialize the AVRC controller and start listening for incoming connections
 *
 * @param[in]       local_features      : Local supported features mask
 *                                        Combination of wiced_bt_avrc_ct_features_t
 * @param[in]       supported_events    : Flag map of events that will be serviced if registered
 * @param[in]       p_connection_cback  : Callback for connection state
 * @param[in]       p_rsp_cb            : Callback from peer device in response to AVRCP commands
 * @param[in]       p_cmd_cb            : Callback when peer device sends AVRCP commands
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_init(uint32_t local_features,
                    uint8_t *supported_events,
                    wiced_bt_avrc_ct_connection_state_cback_t p_connection_cb,
                    wiced_bt_avrc_ct_cmd_cback_t p_cmd_cb,
                    wiced_bt_avrc_ct_rsp_cback_t p_rsp_cb,
                    wiced_bt_avrc_ct_pt_rsp_cback_t p_ptrsp_cb);

#if AVRC_ADV_CTRL_INCLUDED == TRUE
/**
 * Function         wiced_bt_avrc_ct_features_register
 *
 *                  Register for AVRC Feature events.
 *                  This, optional, function must be called after wiced_bt_avrc_ct_init
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_features_register(
        wiced_bt_avrc_ct_features_cback_t features_callback);
#endif

#ifdef WICED_BT_AVRC_CT_DEINIT_SUPPORT
/**
 * Function         wiced_bt_avrc_ct_deinit
 *
 *                  Deinit the AVRC controller and stop listening for incoming connections
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_deinit(void);
#endif // WICED_BT_AVRC_CT_DEINIT_SUPPORT

/**
 * Function         wiced_bt_avrc_ct_connect
 *
 *                  Initiate connection to the peer AVRC target device.
 *                  After connection establishment, stop listening for incoming connections
 *
 * @param[in]       remote_addr : Bluetooth address of peer device
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_connect(wiced_bt_device_address_t remote_addr);

/**
 * Function         wiced_bt_avrc_ct_disconnect
 *
 *                  Disconnect from the peer AVRC target device
 *                  After disconnection , start listening for incoming connections
 *
 * @param[in]       handle    : Connection handle
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_disconnect( uint8_t handle );

/**
 * Function         wiced_bt_avrc_ct_send_pass_through_cmd
 *
 *                  Send PASS THROUGH command
 *
 * @param[in]       handle          : Connection handle
 * @param[in]       cmd             : Pass through command id (see #AVRC_ID_XX)
 * @param[in]       state           : State of the pass through command (see #AVRC_STATE_XX)
 * @param[in]       data_field_len  : Data field length
 * @param[in]       data_field      : Data field
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_send_pass_through_cmd( uint8_t handle, uint8_t cmd, uint8_t state, uint8_t data_len, uint8_t *data );

/**
 * Function         wiced_bt_avrc_ct_send_unit_info_cmd
 *
 *                  Send Unit Info Command
 *
 * @param[in]       handle          : Connection handle
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_send_unit_info_cmd( uint16_t handle );

/**
 * Function         wiced_bt_avrc_ct_send_sub_unit_info_cmd
 *
 *                  Send Sub Unit Info Command
 *
 * @param[in]       handle          : Connection handle
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_send_sub_unit_info_cmd( uint16_t handle );

/**
 * Function         wiced_bt_avrc_ct_get_element_attr_cmd
 *
 *                  Requests the target device to provide the attributes
 *                  of the element specified in the parameter
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       element_id  : Element id
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Media attribute ids (see #AVRC_MEDIA_ATTR_ID)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_element_attr_cmd( uint8_t handle, wiced_bt_avrc_uid_t element_id, uint8_t num_attr, uint8_t *p_attrs);

/**
 * Function         wiced_bt_avrc_ct_get_play_status_cmd
 *
 *                  Get the status of the currently playing media at the TG
 *
 * @param[in]       handle      : Connection handle
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_play_status_cmd( uint8_t handle );

/*****************************************************************************
 *  APPLICATION SETTINGS FUNCTIONS
 ****************************************************************************/

/**
 * Function         wiced_bt_avrc_ct_list_player_attrs_cmd
 *
 *                  Request the target device to provide target supported
 *                  player application setting attributes
 *
 * @param[in]       handle      : Connection handle
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_list_player_attrs_cmd( uint8_t handle );

/**
 * Function         wiced_bt_avrc_ct_list_player_values_cmd
 *
 *                  Requests the target device to list the
 *                  set of possible values for the requested player application setting attribute
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       attr        : Player application setting attribute
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_list_player_values_cmd( uint8_t handle, uint8_t attr);

/**
 * Function         wiced_bt_avrc_ct_get_player_value_cmd
 *
 *                  Requests the target device to provide the current set values
 *                  on the target for the provided player application setting attributes list
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Player attribute ids (see #AVRC_PLAYER_SETTING_XX)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_player_value_cmd( uint8_t handle, uint8_t num_attr, uint8_t *p_attrs );

/**
 * Function         wiced_bt_avrc_ct_set_player_value_cmd
 *
 *                  Requests to set the player application setting list
 *                  of player application setting values on the target device
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       p_vals      : Player application setting values
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_set_player_value_cmd( uint8_t handle, wiced_bt_avrc_player_app_param_t *p_vals );

/**
 * Function         wiced_bt_avrc_ct_get_player_attrs_text_cmd
 *
 *                  Requests the target device to provide the current set values
 *                  on the target for the provided player application setting attributes list
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Player attribute ids (see #AVRC_PLAYER_SETTING_XX)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_player_attrs_text_cmd( uint8_t handle, uint8_t num_attr, uint8_t *p_attrs );

/**
 * Function         wiced_bt_avrc_ct_get_player_values_text_cmd
 *
 *                  Request the target device to provide target supported player
 *                  application setting value displayable text
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       attr        : player application setting attribute
 * @param[in]       num_attr    : Number of values
 * @param[in]       p_attrs     : Player value scan value ids (see #AVRC_PLAYER_VAL_XX)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_player_values_text_cmd( uint8_t handle, uint8_t attr, uint8_t num_val, uint8_t *p_values );

/*****************************************************************************
 *  AVRCP 1.5 BROWSING FUNCTIONS
 ****************************************************************************/

/**
 * Function         wiced_bt_avrc_ct_set_addressed_player_cmd
 *
 *                  Set the player id to the player to be addressed on the target device
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       player_id   : Player id
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_set_addressed_player_cmd( uint8_t handle, uint16_t player_id );

/**
 * Function         wiced_bt_avrc_ct_set_browsed_player_cmd
 *
 *                  Set the player id to the browsed player to be addressed on the target device
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       player_id   : Player id
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_set_browsed_player_cmd( uint8_t handle, uint16_t player_id );

/**
 * Function         wiced_bt_avrc_ct_change_path_cmd
 *
 *                  Change the path in the Virtual file system being browsed
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       direction   : Direction of path change
 * @param[in]       path_uid    : Path uid
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_change_path_cmd( uint8_t handle, uint8_t direction, wiced_bt_avrc_uid_t path_uid );

/**
 * Function         wiced_bt_avrc_ct_get_folder_items_cmd
 *
 *                  Retrieves a listing of the contents of a folder
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       scope       : Scope of the folder
 * @param[in]       start_item  : Start item index
 * @param[in]       end_item    : End item index
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Media attribute ids (see #AVRC_MEDIA_ATTR_ID)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_folder_items_cmd( uint8_t handle, uint8_t scope, uint32_t start_item,
                                                    uint32_t end_item, uint8_t num_attr, uint32_t *p_attrs );

/**
 * Function         wiced_bt_avrc_ct_get_item_attributes_cmd
 *
 *                  Retrieves the metadata attributes for a
 *                  particular media element item or folder item
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       scope       : Scope of the item
 * @param[in]       path_uid    : Path of the item
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Media attribute ids (see #AVRC_MEDIA_ATTR_ID)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_get_item_attributes_cmd( uint8_t handle, uint8_t scope, wiced_bt_avrc_uid_t path_uid,
                                                    uint8_t num_attr, uint32_t *p_attrs );

/**
 * Function         wiced_bt_avrc_ct_search_cmd
 *
 *                  Performs search from the current folder
 *                  in the Browsed Player's virtual file system
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       search_string   : Search string
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_search_cmd( uint8_t handle, wiced_bt_avrc_full_name_t search_string );

/**
 * Function         wiced_bt_avrc_ct_play_item_cmd
 *
 *                  Starts playing an item indicated by the UID
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       scope       : Scope of the item (see #AVRC_SCOPE_XX)
 * @param[in]       item_uid    : UID of the item
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_play_item_cmd( uint8_t handle, uint8_t scope, wiced_bt_avrc_uid_t item_uid );


/**
 * Function         wiced_bt_avrc_ct_add_to_now_playing_cmd
 *
 *                  Adds an item indicated by the UID to the Now Playing queue
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       scope       : Scope of the item (see #AVRC_SCOPE_XX)
 * @param[in]       item_uid    : UID of the item
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_add_to_now_playing_cmd( uint8_t handle, uint8_t scope, wiced_bt_avrc_uid_t item_uid );

/*****************************************************************************
 *  VOLUME FUNCTIONS
 ****************************************************************************/
/**
 * Function         wiced_bt_avrc_ct_set_volume_cmd
 *
 *                  Set volume for peer device
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       volume      : Volume
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_avrc_ct_set_volume_cmd( uint8_t handle, uint8_t volume );

/* Utility Functions */

/** API To get LRAC Switch data
 *
 *  Called by the application to get the LRAC Switch Data
 *
 *  @param p_opaque     Pointer to a buffer which will be filled with LRAC Switch data (current
 *                      A2DP Sink State)
 *  @param p_opaque     Size of the buffer (IN), size filled (OUT)
 *
 *  @return none
 */
wiced_result_t wiced_bt_avrc_ct_lrac_switch_get(void *p_opaque, uint16_t *p_sync_data_len);

/** API To set LRAC Switch data
 *
 *  Called by the application to set the LRAC Switch Data
 *
 *  @param p_opaque     Pointer to a buffer which contains LRAC Switch data (new
 *                      A2DP Sink State)
 *  @param p_opaque     Size of the buffer (IN)
 *
 *  @return none
 */
wiced_result_t wiced_bt_avrc_ct_lrac_switch_set(void *p_opaque, uint16_t sync_data_len);

#ifdef CT_HANDLE_PASSTHROUGH_COMMANDS
/**
 * wiced_bt_avrc_ct_register_passthrough_event_callback
 *
 * @param pt_evt_cb callback
 */
void wiced_bt_avrc_ct_register_passthrough_event_callback( wiced_bt_avrc_ct_pt_evt_cback_t pt_evt_cb);
#endif // CT_HANDLE_PASSTHROUGH_COMMANDS
/** @}*/

#ifdef __cplusplus
}
#endif
