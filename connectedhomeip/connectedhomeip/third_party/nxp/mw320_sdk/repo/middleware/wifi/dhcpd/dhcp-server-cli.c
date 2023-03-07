/** @file dhcp-server-cli.c
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
#include <cli.h>
#include <cli_utils.h>
#include <dhcp-server.h>

#include "dhcp-priv.h"

/*
 * Command-Line Interface
 */
static void test_dhcp_stat(int argc, char **argv)
{
    dhcp_stat();
}

static struct cli_command dhcp_cmds[] = {
    {"dhcp-stat", NULL, test_dhcp_stat},
};

int dhcpd_cli_init(void)
{
    unsigned int i;

    for (i = 0; i < sizeof(dhcp_cmds) / sizeof(struct cli_command); i++)
        if (cli_register_command(&dhcp_cmds[i]))
            return -WM_E_DHCPD_REGISTER_CMDS;

    return WM_SUCCESS;
}
