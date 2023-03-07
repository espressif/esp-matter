/***************************************************************************//**
 * @file
 * @brief USB Host Handle
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_HANDLE_H_
#define  _USBH_CORE_HANDLE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           SPECIAL HANDLE VALUES
 *******************************************************************************************************/

//                                                                 Invalid handle.
#define  USBH_CORE_HANDLE_INVALID                 0x0000FFFFu
#define  USBH_CORE_HANDLE_INVALID_MSK             0x0000FFFFu

#define  USBH_HC_HANDLE_INVALID                   0xFFu
#define  USBH_DEV_HANDLE_INVALID                  USBH_CORE_HANDLE_INVALID
#define  USBH_FNCT_HANDLE_INVALID                 USBH_CORE_HANDLE_INVALID
#define  USBH_EP_HANDLE_INVALID                   USBH_CORE_HANDLE_INVALID

#define  USBH_DEV_HANDLE_INVALID_MSK              USBH_CORE_HANDLE_INVALID_MSK
#define  USBH_FNCT_HANDLE_INVALID_MSK             USBH_CORE_HANDLE_INVALID_MSK
#define  USBH_EP_HANDLE_INVALID_MSK               USBH_CORE_HANDLE_INVALID_MSK

#define  USBH_EP_HANDLE_CTRL                      0x00000000u   // EP handle for ctrl endpoints.

#define  USBH_FNCT_HANDLE_DEV                     0x0001u       // Fnct handle when class is defined at dev level.

#define  USBH_DEV_HANDLE_NOTIFICATION             0X00000080u   // Dev handle to use to get info on dev being enum'd.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       USB HOST CONTROLLER HANDLE
 *
 * Note(s) : (1) The host controller handle is a 8 bit wide value that contains the host and host
 *               controller index:
 *
 *               MSB                           LSB
 *               ---------------------------------
 *               | POSITION  | 8 .. 4  | 3 .. 0  |
 *               ---------------------------------
 *               | USAGE     |   HC    |  Host   |
 *               |           |  Index  |  Index  |
 *               ---------------------------------
 *******************************************************************************************************/

typedef CPU_INT08U USBH_HC_HANDLE;                              // HC handle. See note (1).

/********************************************************************************************************
 *                                       USB HOST CORE GEN HANDLE
 *
 * Note(s) : (1) Handles are 32 bit wide value that contains various information, including a 16 bit
 *               counter used for validation:
 *
 *               MSB                              LSB
 *               ------------------------------------
 *               | POSITION  | 31 .. 16  | 15 ..0   |
 *               -----------------------------------|
 *               | USAGE     | Validate  |   Type   |
 *               |           | Cnt       | Specific |
 *               ------------------------------------
 *******************************************************************************************************/

typedef CPU_INT32U USBH_CORE_HANDLE;                            // Core generic handle. See note (1).

/********************************************************************************************************
 *                                       USB HOST DEVICE HANDLE
 *
 * Note(s) : (1) The device handle is a 32 bit wide value that contains the device address, the host
 *               controller index, and the host index:
 *
 *               MSB                                                  LSB
 *               ---------------------------------------------------------
 *               | POSITION  | 31 .. 16  | 15 ..11 | 11 .. 8  | 7 .. 0   |
 *               --------------------------------------------------------|
 *               | USAGE     | Validate  |   HC    |  Host    | Device   |
 *               |           | Cnt       |  Index  |  Index   | Address  |
 *               ---------------------------------------------------------
 *******************************************************************************************************/

typedef USBH_CORE_HANDLE USBH_DEV_HANDLE;                       // Device handle. See note (1).

/********************************************************************************************************
 *                                       USB HOST FUNCTION HANDLE
 *
 * Note(s) : (1) The function handle is a 32 bit wide value that contains the function index and the
 *               configuration number:
 *
 *               MSB                                           LSB
 *               -------------------------------------------------
 *               | POSITION  | 31 .. 16  | 15 ..8    | 7 .. 0    |
 *               -------------------------------------------------
 *               | USAGE     | Validate  | Config #  | Function  |
 *               |           |   Cnt     |           |  Index    |
 *               -------------------------------------------------
 *******************************************************************************************************/

typedef USBH_CORE_HANDLE USBH_FNCT_HANDLE;                      // Function handle. See note (1).

/********************************************************************************************************
 *                                       USB HOST ENDPOINT HANDLE
 *
 * Note(s) : (1) The device handle is a 32 bit wide value that contains the endpoint physical number, the
 *               interface index and the validation count:
 *
 *               MSB                                           LSB
 *               -------------------------------------------------
 *               | POSITION  | 31 .. 16  | 15 ..8    | 7 .. 0    |
 *               -------------------------------------------------
 *               | USAGE     | Validate  | Interface | Endpoint  |
 *               |           |   Cnt     |   Index   |  Phy #    |
 *               -------------------------------------------------
 *******************************************************************************************************/

typedef USBH_CORE_HANDLE USBH_EP_HANDLE;                        // Endpoint handle. See note (1).

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
