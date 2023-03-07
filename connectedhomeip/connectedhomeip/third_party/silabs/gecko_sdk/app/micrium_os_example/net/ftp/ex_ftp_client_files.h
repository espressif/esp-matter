/***************************************************************************//**
 * @file
 * @brief Example - FTP Client Files
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef  APP_STATIC_FILES_PRESENT
#define  APP_STATIC_FILES_PRESENT

#define  STATIC_LOGO_GIF_NAME               "\\logo.gif"
#define  STATIC_INDEX_HTML_NAME             "\\index.html"

#define  STATIC_LOGO_GIF_LEN                2066u
#define  STATIC_INDEX_HTML_LEN              3080u

extern const unsigned char Ex_FTP_Client_LogoGif[];
extern const unsigned char Ex_FTP_Client_IndexHtml[];

#endif
