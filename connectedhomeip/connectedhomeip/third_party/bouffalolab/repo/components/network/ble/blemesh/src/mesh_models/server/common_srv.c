/**
 ****************************************************************************************
 *
 * @file common_srv.c
 *
 * @brief Implement common handling of mesh server models 
 *
 * Copyright (C) Bouffalo Lab 2020
 *
 * History: 2020-02 crealted by llgong @ Shanghai
 *
 ****************************************************************************************
 */
#include "common_srv.h"
#include "bl_port.h"

void bt_mesh_srv_update_last_rcvd_msg(struct bt_mesh_last_rcvd_msg *last_msg, u8_t tid, u16_t src_addr, u16_t dst_addr,
                                int64_t rcvd_time)
{
    last_msg->tid = tid;
    last_msg->src_addr = src_addr;
    last_msg->dst_addr = dst_addr;
    last_msg->rcvd_time = rcvd_time;
}

bool bt_mesh_srv_check_rcvd_msg_with_last(u8_t tid, u16_t src_addr, u16_t dst_addr,

                                   int64_t *rcvd_time, struct bt_mesh_last_rcvd_msg *last_msg)
{
    /*In 3.3.1.2.2 of MMDL spec, When a Generic OnOff Server receives a Generic OnOff Set message or a Generic OnOff Set
      Unacknowledged message, it shall set the Generic OnOff state to the OnOff field of the message,
      unless the message has the same value for the SRC, DST, and TID fields as the previous message
      received within the past 6 seconds.*/
      
    *rcvd_time = k_uptime_get();

    if(tid == last_msg->tid && src_addr == last_msg->src_addr && 
       dst_addr == last_msg->dst_addr &&
       *rcvd_time - last_msg->rcvd_time <= K_SECONDS(6))
    {
        return true;
    }

    return false;
}
