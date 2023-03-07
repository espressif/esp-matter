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
#ifndef OTA_SETTINGS_H
#define OTA_SETTINGS_H

/*
 * Defines the minimum severity level allowed.
 * Use E_DEBUG to enable OTA internal messages
 * Options: E_TRACE, E_DEBUG, E_INFO, E_WARNING, E_ERROR, E_FATAL
 */
#define OTA_IF_DEBUG_LEVEL                  E_INFO

#define CLOUD_OTA_SUPPORT                   (0) // HTTP client - Download content from a (remote) file server
#define LOCAL_OTA_SUPPORT                   (0) // HTTP server - Upload from HTTP client (a mobile device connects on the local network)
#define INTERNAL_UPDATE_SUPPORT             (0) // File System - load content from a local (tar) file

#define OTA_VERSION_LEN                     14  /* sizeof "YYYYMMDDHHMMSS" */
#define OTA_URL_LEN                         900

#if CLOUD_OTA_SUPPORT
    /**
     * Compiles Github and Dropbox for usage in code
     */
    #define OTA_VENDOR_GITHUB_SUPPORT       (1)
    #define OTA_VENDOR_DROPBOX_SUPPORT      (1)

    //#define USE_TI_EXAMPLE
    #if  OTA_VENDOR_GITHUB_SUPPORT
        #ifdef USE_TI_EXAMPLE
            /* The following are GITHUB parameters for the project in:
             * https://github.com/SimpleLink/CC3X20/tree/master/SEC_OTA_CC3220SF
             */
            #define GITHUB_USER_NAME        "SimpleLink"
            #define GITHUB_USER_TOKEN_B64   "Z2hwX0hjeEd4a3RRYjR4bXBIcEhOa3pySFQ2NldyRkkzbDBsNXg5aAo=" // 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
            #define GITHUB_USER_REPO        "CC3X20"
            #define GITHUB_USER_PATH        "SEC_OTA_CC3235SF"
        #else
            //#define GITHUB_USER_NAME      // GITHUB User Name

            // Note: Personal Access Token should be provided in base64 format.
            // To generate the base64 format, use the following bash command (any other base64 converter is acceptable):
            // "echo <personal-access-token> | base64"
            // e.g. "echo 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 | base64"
            // Important: make sure the token has "No Expiration" enables read-only access (no cope should be enabled)
            //#define GITHUB_USER_TOKEN_B64 // GITHUB User Token in Base64 format

            //#define GITHUB_USER_REPO      // GITHUB Repository Name
            //#define GITHUB_USER_PATH      // Relative path within the repository to the
                                            // folder containing the tar file
            #ifndef GITHUB_USER_NAME
            #error "Please define your personal GITHUB account parameters in \"ota_settings.h\" (above)"
            #endif
        #endif
    #endif  /* OTA_VENDOR_GITHUB_SUPPORT */

    #if  OTA_VENDOR_DROPBOX_SUPPORT
        #ifdef USE_TI_EXAMPLE
            /* The following are DROPBOX parameters for the project in:
             * https://www.dropbox.com/home/Apps/R3-SEC-OTA-NEW/SEC_OTA_CC3220SF
             */
            #define DROPBOX_USER_TOKEN      "apCeaH3vKRsAAAAAAAAAAd6VJmPMR-vsXc54ce3kjpVMO5oA6nv53eV5IWLpCuHd"
            #define DROPBOX_USER_PATH       "SEC_OTA_CC3235SF"
        #else
            //#define DROPBOX_USER_TOKEN    // DROPBOX APP token
            //#define DROPBOX_USER_PATH     // Relative path within the app to the
                                            // folder containing the tar file
            #ifndef DROPBOX_USER_TOKEN
            #error "Please define your personal DROPBOX account parameters in \"ota_settings.h\" (above)"
            #endif
        #endif
    #endif /* OTA_VENDOR_DROPBOX_SUPPORT */
#else
    #define OTA_VENDOR_GITHUB_SUPPORT       (0)
    #define OTA_VENDOR_DROPBOX_SUPPORT      (0)
#endif // CLOUD_OTA_SUPPORT

/* MORE OTA Settings */
#define OTA_DISABLE_DOWNGRADE_PROTECTION    (0) // if set to (1) - the OTA versions will not be verified
                                                //                 and older versions can be installed.
#define OTA_COMMIT_TIMEOUT                   50 // Watchdog timeout in seconds (max: 104 sec)

/* The following is an internal definition -
 * Please do not change and update the supported OTA methods above */
#define OTA_SUPPORT                         (CLOUD_OTA_SUPPORT|LOCAL_OTA_SUPPORT|INTERNAL_UPDATE_SUPPORT)

#endif /* OTA_SETTINGS_H */

