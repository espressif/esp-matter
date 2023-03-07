/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
/*${header:start}*/
#include "FreeRTOS.h"
#include "task.h"
#include "wpl.h"
#include "fsl_debug_console.h"
#include "network_cfg.h"
/*${header:end}*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
/*${prototype:end}*/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*${variable:start}*/
/* Set to true for AP mode, false for client mode */
bool wifi_ap_mode = WIFI_AP_MODE;
/*${variable:end}*/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*${function:start}*/
int initNetwork()
{
    int result;
    char ip[16];

    PRINTF("Initializing WiFi connection... \r\n");
    WPL_Init();
    if ((result = WPL_Start()) != WPL_SUCCESS)
    {
        PRINTF("Could not initialize WiFi module %d\r\n", (uint32_t)result);
        return -1;
    }
    else
    {
        PRINTF("Successfully initialized WiFi module\r\n");
    }

    if (wifi_ap_mode)
    {
        /* AP mode */
        uint8_t ap_channel = WIFI_AP_CHANNEL;

        PRINTF("Starting Access Point: SSID: %s, Chnl: %d\r\n", WIFI_SSID, ap_channel);

        /* Start the access point */
        result = WPL_Start_AP(WIFI_AP_IP_ADDR, WIFI_SSID, WIFI_PASSWORD, ap_channel);
        if (result != WPL_SUCCESS)
        {
            PRINTF("Failed to start access point\r\n");
            return -1;
        }

        /* Start DHCP server */
        WPL_StartDHCPServer(WIFI_AP_IP_ADDR, WIFI_AP_NET_MASK);

        WPL_GetIP(ip, 0);
        PRINTF("Network ready IP: %s\r\n", ip);

        return 0;
    }

#if WIFI_SCAN_NETWORKS
    int result = WPL_Scan();
    if (result != WPL_SUCCESS)
    {
        PRINTF(" Scan Error\r\n");
        return -1;
    }
#endif

    /* Client mode */
    PRINTF("Joining: " WIFI_SSID "\r\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    result = WPL_Join(WIFI_SSID, WIFI_PASSWORD);
    if (result != WPL_SUCCESS)
    {
        PRINTF("Failed to join: " WIFI_SSID "\r\n");
        return -1;
    }
    else
    {
        PRINTF("Successfully joined: " WIFI_SSID "\r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    return 0;
}
/*${function:end}*/
