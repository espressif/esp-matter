/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __WPL_H
#define __WPL_H

#define WPL_SUCCESS 0
#define WPL_ERROR   1

#define WPL_WIFI_SSID_LENGTH     32
#define WPL_WIFI_PASSWORD_LENGTH 63

/* WPL_Init()
 *
 * Initialize WIFI interface
 */
int WPL_Init();

/* WPL_Start()
 *
 * Start WIFI interface
 */
int WPL_Start();

/* WPL_Stop()
 *
 * Stop WIFI interface
 */
int WPL_Stop();

/* WPL_Start_AP()
 *
 * Start WIFI AP
 */
int WPL_Start_AP(char *ip, char *ssid, char *password, int chan);

/* WPL_Stop_AP()
 *
 * Stop WIFI AP
 */
int WPL_Stop_AP();

/* WPL_Scan()
 *
 * Scan for Wifi APs
 */
int WPL_Scan();

/* WPL_Join()
 *
 * Join given SSID AP
 */
int WPL_Join(char *ssid, char *password);

/* WPL_Leave()
 *
 * Leave the current station network and clean up
 */
int WPL_Leave();

/* WPL_getSSIDs()
 *
 * get SSIDs as JSON
 */
char *WPL_getSSIDs();

/* WPL_StartDHCPServer(char *ip, char *net)
 *
 * Start the DHCP Server on given IP and network
 */
int WPL_StartDHCPServer(char *ip, char *net);

/* WPL_StartDHCPServer(char *ip, char *net)
 *
 * Stop the DHCP server and clean up
 */
int WPL_StopDHCPServer();

int WPL_GetIP(char *ip, int client);
#endif
