/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SL_WISUN_EVENT_MGR_H__
#define __SL_WISUN_EVENT_MGR_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_status.h"
#include "sl_wisun_events.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN application callback type.
 *        It is always called, User cannot register or remove them.
 *        It contains the must have implementation of the event.
 *****************************************************************************/
typedef void (*wisun_event_callback_t) (sl_wisun_evt_t *);

/**************************************************************************//**
 * @brief Wi-SUN application custom callback type.
 *        User can implement own callbacks and register them (not mandatory)
 *****************************************************************************/
typedef void (*custom_wisun_event_callback_t) (sl_wisun_evt_t *);

/**************************************************************************//**
 * @brief Event handler structure
 *****************************************************************************/
typedef struct event_handler {
  sl_wisun_msg_ind_id_t id;                        // id
  wisun_event_callback_t callback;                 // callback for internal usage
  custom_wisun_event_callback_t custom_callback;   // custom, registerable and removable callback for applications
} event_handler_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Event manager initialization
 *****************************************************************************/
void app_wisun_event_mgr_init(void);

/**************************************************************************//**
 * @brief Register custom callback based on event ID.
 * @details It registers the custom callback based on the event ID
 * @param[in] id event ID
 * @param[in] callback is the desired callback that is registered
 * @return sl_status_t SL_STATUS_OK if it is successful otherwise SL_STATUS_FAIL
 * @remark Warning: if the custom callback of the event ID has already been
 *         registered, then this write it over.
 *****************************************************************************/
sl_status_t app_wisun_em_custom_callback_register(sl_wisun_msg_ind_id_t id,
                                                  custom_wisun_event_callback_t callback);

/**************************************************************************//**
 * @brief Remove custom callback based on event ID.
 * @details It removes the custom callback of event ID.
 * @param[in] id event ID
 * @return sl_status_t SL_STATUS_OK if it is successful otherwise SL_STATUS_FAIL
 *****************************************************************************/
sl_status_t app_wisun_em_custom_callback_remove(sl_wisun_msg_ind_id_t id);

// ------------------------- event callbacks -----------------------------------

/**************************************************************************//**
 * @brief Wi-SUN network update event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_network_update_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Wi-SUN network connected event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_connected_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Data event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_socket_data_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Data avialable event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_socket_data_available_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Socket connected event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_socket_connected_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Socket connection available event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_socket_connection_available_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Socket closing event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_socket_closing_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Wi-SUN network disconnected event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_disconnected_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Socket connection lost event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_connection_lost_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Socket data sent event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_socket_data_sent_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Wi-SUN error occured event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_error_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Wi-SUN join state event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_join_state_event_hnd(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Wi-SUN regulation TX level indication
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
void sl_wisun_regulation_tx_level_hnd(sl_wisun_evt_t *evt);

#endif /* __SL_WISUN_EVENT_MGR_H__ */
