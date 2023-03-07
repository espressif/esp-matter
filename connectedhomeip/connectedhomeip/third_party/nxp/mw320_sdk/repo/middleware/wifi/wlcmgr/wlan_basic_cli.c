/** @file wlan_basic_cli.c
 *
 *  @brief  This file provides Connection Manager CLI
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

#include <wlan.h>
#include <cli.h>

void test_wfa_wlan_version(int argc, char **argv)
{
    unsigned int i;
    char version_str[MLAN_MAX_VER_STR_LEN];
    char *ext_ver_str = wlan_get_firmware_version_ext();
    strcpy(version_str, ext_ver_str);
    for (i = 0; i < strlen(version_str); i++)
    {
        if (version_str[i] == '.' && version_str[i + 1] == 'p')
        {
            version_str[i] = '\0';
            break;
        }
    }

    PRINTF("WLAN Version : %s\r\n", version_str);
}

void test_wlan_version(int argc, char **argv)
{
    char *version_str;

    version_str = wlan_get_firmware_version_ext();

    PRINTF("WLAN Driver Version   : %s\r\n", WLAN_DRV_VERSION);
    PRINTF("WLAN Firmware Version : %s\r\n", version_str);
}

static void test_wlan_get_mac_address(int argc, char **argv)
{
    uint8_t mac[6];

    PRINTF("MAC address\r\n");
    if (wlan_get_mac_address(mac))
        PRINTF("Error: unable to retrieve MAC address\r\n");
    else
        PRINTF("%02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static struct cli_command wlan_wfa_basic_commands[] = {
    {"wlan-version", NULL, test_wfa_wlan_version},
    {"wlan-mac", NULL, test_wlan_get_mac_address},
};

static struct cli_command wlan_basic_commands[] = {
    {"wlan-version", NULL, test_wlan_version},
    {"wlan-mac", NULL, test_wlan_get_mac_address},
};

static bool wlan_wfa_basic_cli_init_done = false;

int wlan_wfa_basic_cli_init(void)
{
    unsigned int i;

    for (i = 0; i < sizeof(wlan_wfa_basic_commands) / sizeof(struct cli_command); i++)
        if (cli_register_command(&wlan_wfa_basic_commands[i]))
            return WLAN_ERROR_ACTION;

    wlan_wfa_basic_cli_init_done = true;

    return WLAN_ERROR_NONE;
}

int wlan_basic_cli_init(void)
{
    unsigned int i;

    if (wlan_wfa_basic_cli_init_done)
        return WLAN_ERROR_NONE;

    for (i = 0; i < sizeof(wlan_basic_commands) / sizeof(struct cli_command); i++)
        if (cli_register_command(&wlan_basic_commands[i]))
            return WLAN_ERROR_ACTION;

    return WLAN_ERROR_NONE;
}
