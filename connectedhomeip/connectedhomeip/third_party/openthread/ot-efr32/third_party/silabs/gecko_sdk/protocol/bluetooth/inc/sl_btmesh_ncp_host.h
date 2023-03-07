/***************************************************************************//**
 * @brief Adaptation layer between host application and SL_BTMESH_API protocol
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BTMESH_NCP_HOST_H
#define SL_BTMESH_NCP_HOST_H

/*****************************************************************************
 *
 *  This is an adaptation layer between host application and SL_BTMESH_API protocol.
 *  It provides synchronization mechanism for SL_BTMESH_API-protocol that allows
 *  using same application architecture between application in mcu and external
 *  host.
 *
 *  Synchronization is done by waiting for response after each command. If
 *  any events are received during response waiting, they are queued and
 *  delivered next time sl_btmesh_wait_event is called.
 *
 *  Queue length is controlled by defining macro "SL_BTMESH_API_QUEUE_LEN", default is 30.
 *  Queue length depends on use cases and allowed host memory usage.
 *
 *  SL_BTMESH_API usage:
 *      Register library with the SL_BT_API protocol handler.
 *
 *          SL_BTMESH_API_REGISTER();
 *
 *      The SL_BT_API protocol handler MUST have been initialized prior to registering SL_BTMESH_API.
 *      Both use the same data pipe input and output functions, defined by the SL_BT_API initializers.
 *      See the documentation in sl_bt_ncp_host.h for information about how to initialize it.
 *      In all other ways, SL_BTMESH_API behaves the same way as SL_BT_API.
 *
 *
 *  Receiving event:
 *   Events are received by sl_btmesh_wait_event-function.
 *
 *   prototype:
 *       sl_btmesh_msg_t* sl_btmesh_wait_event(void)
 *
 *   Event ID can be read from header of event by SL_BTMESH_API_MSG_ID-macro.
 *
 *   Event data can be accessed thru returned pointer.
 *
 *   Example:
 *       sl_btmesh_msg_t *p;
 *
 *       p=sl_btmesh_wait_event()
 *
 *       if(SL_BTMESH_API_MSG_ID(p->header)==sl_btmesh_evt_gatt_server_characteristic_status_id)
 *       {
 *           c=evt->evt_gatt_server_characteristic_status.connection;//accesses connection field of event data
 *       }
 *
 *  Sending commands:
 *   SL_BTMESH_API-commands can be sent by calling helper functions, that then build SL_BTMESH_API-message and call output-function.
 *   Function then waits for response and return pointer to that message.
 *
 *   Example:
 *       //Test that connection to the device is working
 *       if(sl_btmesh_system_hello() == SL_STATUS_OK)
 *       {//function call was successful
 *
 *       }
 *
 *
 ****************************************************************************/

#if _MSC_VER  //msvc
#define inline __inline
#endif

#include "sl_btmesh_api.h"
#include "sl_bt_ncp_host.h"

#ifndef SL_BTMESH_API_QUEUE_LEN
#define SL_BTMESH_API_QUEUE_LEN 30
#endif

extern bgapi_device_type_queue_t sl_btmesh_api_queue;

/**
 * Register SL_BTMESH_API to the Bluetooth Host API system.
 * Note that SL_BT_API_INITIALIZE_NONBLOCK or SL_BT_API_INITIALIZE
 * MUST have been called prior to this.
 */
#define SL_BTMESH_API_REGISTER() \
  do { sli_bgapi_register_device(&sl_btmesh_api_queue); } while (0);

void sl_btmesh_host_handle_command();
void sl_btmesh_host_handle_command_noresponse();
sl_status_t sl_btmesh_wait_event(sl_btmesh_msg_t *p);

#endif
