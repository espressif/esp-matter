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

#include "sl_btmesh_ncp_host.h"
#include "sl_status.h"

sl_btmesh_msg_t _sl_btmesh_cmd_msg;
sl_btmesh_msg_t _sl_btmesh_rsp_msg;
sl_btmesh_msg_t *sl_btmesh_cmd_msg = &_sl_btmesh_cmd_msg;
sl_btmesh_msg_t *sl_btmesh_rsp_msg = &_sl_btmesh_rsp_msg;
uint8_t _sl_btmesh_queue_buffer[SL_BTMESH_API_QUEUE_LEN * (SL_BGAPI_MSG_HEADER_LEN + SL_BGAPI_MAX_PAYLOAD_SIZE)];

bgapi_device_type_queue_t sl_btmesh_api_queue = {
  sl_bgapi_dev_type_btmesh,
  0,
  0,
  (sl_bt_msg_t *)_sl_btmesh_queue_buffer,
  SL_BTMESH_API_QUEUE_LEN
};

bool sl_btmesh_event_pending(void)
{
  if (sli_bgapi_device_queue_has_events(&sl_btmesh_api_queue)) {//event is waiting in queue
    return true;
  }

  //something in uart waiting to be read
  if (sl_bt_api_peek && sl_bt_api_peek()) {
    return true;
  }

  return false;
}

sl_status_t sl_btmesh_wait_event(sl_btmesh_msg_t* event)
{
  return sli_bgapi_get_event(1, (sl_bt_msg_t*)event, &sl_btmesh_api_queue);
}

sl_status_t sl_btmesh_pop_event(sl_btmesh_msg_t* event)
{
  return sli_bgapi_get_event(0, (sl_bt_msg_t*)event, &sl_btmesh_api_queue);
}

/**
 * This function will block until a response is found from the data pipe.
 * Any events that arrive before the response will be put into their corresponding
 * event queues.
 */
sl_btmesh_msg_t* sl_btmesh_wait_response(void)
{
  sl_btmesh_msg_t* p;
  while (1) {
    p = (sl_btmesh_msg_t*)sli_wait_for_bgapi_message((sl_bt_msg_t*)sl_btmesh_rsp_msg);
    if (p) {
      return p;
    }
  }
}

void sl_btmesh_host_handle_command()
{
  //packet in sl_btmesh_cmd_msg is waiting for output
  sl_bt_api_output(SL_BGAPI_MSG_HEADER_LEN + SL_BGAPI_MSG_LEN(sl_btmesh_cmd_msg->header), (uint8_t*)sl_btmesh_cmd_msg);
  sl_btmesh_wait_response();
}

void sl_btmesh_host_handle_command_noresponse()
{
  //packet in sl_btmesh_cmd_msg is waiting for output
  sl_bt_api_output(SL_BGAPI_MSG_HEADER_LEN + SL_BGAPI_MSG_LEN(sl_btmesh_cmd_msg->header), (uint8_t*)sl_btmesh_cmd_msg);
}
