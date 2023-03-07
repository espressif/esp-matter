/** @file dhcp-server-main.c
 *
 *  @brief This file provides CLI based APIs for the DHCP Server
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

/** dhcp-server-main.c: CLI based APIs for the DHCP Server
 */
#include <string.h>

#include <wm_os.h>
#include <wm_net.h>
#include <dhcp-server.h>

#include "dhcp-priv.h"

os_thread_t dhcpd_thread;
static os_thread_stack_define(dhcp_stack, 2048);
static bool dhcpd_running;
/*
 * API
 */

int dhcp_server_start(void *intrfc_handle)
{
    int ret;

    dhcp_d("DHCP server start request");
    if (dhcpd_running || dhcp_server_init(intrfc_handle))
        return -WM_E_DHCPD_SERVER_RUNNING;

    ret = os_thread_create(&dhcpd_thread, "dhcp-server", dhcp_server, 0, &dhcp_stack, OS_PRIO_3);
    if (ret)
    {
        dhcp_free_allocations();
        return -WM_E_DHCPD_THREAD_CREATE;
    }

    dhcpd_running = 1;
    return WM_SUCCESS;
}

void dhcp_server_stop(void)
{
    dhcp_d("DHCP server stop request");
    if (dhcpd_running)
    {
        if (dhcp_send_halt() != WM_SUCCESS)
        {
            dhcp_w("failed to send halt to DHCP thread");
            return;
        }

        os_thread_sleep(os_msec_to_ticks(50));

        if (os_thread_delete(&dhcpd_thread) != WM_SUCCESS)
            dhcp_w("failed to delete thread");
        dhcpd_running = 0;
    }
    else
    {
        dhcp_w("server not dhcpd_running.");
    }
}
