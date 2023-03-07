/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//*****************************************************************************
// Includes
//*****************************************************************************
// Standard includes
#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H


//*****************************************************************************
//                 WIFI IF INTRODUCTION
//*****************************************************************************
/* This module enables an easy integration of Wi-Fi to a SimpleLink Networking
 * framework.
 * It was designed for applications that use the Wi-Fi Station role only.
 * The simple API and settings enables the user to initiate the Wi-Fi and
 * configure the provisioning method that will be used upon first connection
 * attempt.
 * Upon successful init (WIFI_IF_init()), the system enables the NWP for any
 * SL commands.
 * The NWP will be in a low power state (AUTO-CONNECT will be disabled) waiting
 * for connection request (SlNetConn_Start()).
 * User should not call sl_Start/sl_Stop when using this module. Please use
 * WIFI_IF_restart() (for reseting the NWP) or WIFI_IF_deinit() instead.
 */

//*****************************************************************************
//                WIFI IF USER SETTINGS
//*****************************************************************************

/*
 * Defines the minimum severity level allowed.
 * Use E_DEBUG to enable Wifi internal messages
 * Options: E_TRACE, E_DEBUG, E_INFO, E_WARNING, E_ERROR, E_FATAL
 */
#define WIFI_IF_DEBUG_LEVEL         E_INFO

/*
 * Defines Provisioning (initial) Parameters:
 * Mode can be:  WifiProvMode_OFF, WifiProvMode_ON, WifiProvMode_ONE_SHOT
 * Command can be one of the following:
 * SL_WLAN_PROVISIONING_CMD_START_MODE_AP,
 * SL_WLAN_PROVISIONING_CMD_START_MODE_SC,
 * SL_WLAN_PROVISIONING_CMD_START_MODE_APSC,
 * SL_WLAN_PROVISIONING_CMD_START_MODE_APSC_EXTERNAL_CONFIGURATION,
 * SL_WLAN_PROVISIONING_CMD_START_MODE_EXTERNAL_CONFIGURATION
 */
#define PROVISIONING_MODE           WifiProvMode_ONE_SHOT
#define PROVISIONING_CMD            SL_WLAN_PROVISIONING_CMD_START_MODE_APSC

/*
 * Defines Provisioning AP /SC Parameters:
 */
#define PROVISIONING_TIMEOUT        0       // 0 - use default
#define PROVISIONING_AP_PASSWORD    "1234567890"        // NULL - use default (OPEN)
#define PROVISIONING_SC_KEY         "1234567890123456"  // NULL - use defaults

/* Force provisioning by deleting existing profiles.
 * To be used for testing during development only.
 * Note: When FORCE_PROVSIONING is enabled - the following static profile
 * configurations are ignored
 */
#define FORCE_PROVISIONING          (0)

/* Static Profile setting - Method 1: Hard coded
 * Define AP_SSID and optionally AP_PASSWORD - to connect to local network
 * Hard-Coded Definition: update AP_SSID and AP_PASSWORD (NULL means OPEN, else is WPA2)
 */
#define AP_SSID                     NULL // "network-name"
#define AP_PASSWORD                 NULL // "network-password"

/* Static Profile setting  - Method 2: Configuration file
 * File format should be:
 *    "<network-name>' '<network-password>"
 * i.e. ssid and password with one space character between them
 * Do not use newline, extra space symbols or quotation mark
 */
#define AP_CFG_FILENAME             "network.cfg" // config file name
#define AP_CFG_TOKEN                12345678      // config file read access token
#define AP_CFG_MAX_SIZE             100           // config file Maximum file length

/* Define (if-needed) the external handle for TI Driver's LED for wi-fi status:
 * off: disconnected, blinking: provisionig, on: connected 
 * Comment the definition in case the auto control is not required */
//#define WIFI_LED_HANDLE 

#endif // WIFI_SETTINGS_H
