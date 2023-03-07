/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/*******************************************************************************
* File Name: hidevent.h
*******************************************************************************/

/** \addtogroup HidEvents HID Events
 *  \ingroup wiced_bt_hid
 *  The 2042 HID application assumes that there is an event queue that
 *  holds user events until they can be placed in reports and sent over to the
 *  host. For the convenience of applications, the HID application pre-defines
 *  certain common events and a structure for each event. Application are free
 *  to define their own events.
 */

/// @{
#ifndef __HID_EVENT_H_
#define __HID_EVENT_H_

#include "brcm_fw_types.h"
#include "keyscan.h"

/////////////////////////////////////////////////////////////////////////////////
// Types and Defines
/////////////////////////////////////////////////////////////////////////////////

/// Predefined event types
enum
{
        /// Reserved event type indicating that no event type is acceptable in places where an event type is required
        HID_EVENT_NONE=0,

        /// Motion along axis 0
        HID_EVENT_MOTION_AXIS_0,

        /// Motion along axis 1
        HID_EVENT_MOTION_AXIS_1,

        /// Motion along axis 2
        HID_EVENT_MOTION_AXIS_2,

        /// Motion along axis 3
        HID_EVENT_MOTION_AXIS_3,

        /// Motion along 2 axis. Useful for any input with 2 axis.
        HID_EVENT_MOTION_AXIS_X_Y,

        /// Motion along 2 axis. Useful for a second input with 2 axis, e.g. trackball on a mouse
        HID_EVENT_MOTION_AXIS_A_B,

        /// Change in button state
        HID_EVENT_NEW_BUTTON_STATE,

        /// Change in key state
        HID_EVENT_KEY_STATE_CHANGE,

        /// Motion Sensor data available
        HID_EVENT_MOTION_DATA_AVAILABLE,

        /// Voice data available
        HID_EVENT_VOICE_DATA_AVAILABLE,

        ///the hid event for BRCM_MIC_START
        HID_EVENT_MIC_START,

        ///the hid event for BRCM_MIC_STOP
        HID_EVENT_MIC_STOP,

        ///the hid event for BRCM_RC_MIC_START_REQ
        HID_EVENT_RC_MIC_START_REQ,

        ///the hid event for BRCM_RC_MIC_STOP_REQ
        HID_EVENT_RC_MIC_STOP_REQ,

        ///the hid event for BRCM_RC_VOICEMODE_RD_ACK
        HID_EVENT_AUDIO_MODE,

        ///the hid event for BRCM_RC_CODECSETTINGS_RD_ACK
        HID_EVENT_AUDIO_CODEC_RD,

        ///the hid event for BRCM_RC_CODECSETTINGS_WT_ACK
        HID_EVENT_AUDIO_CODEC_WT,

        /// Event fifo overflow event
        HID_EVENT_EVENT_FIFO_OVERFLOW = 0xfe,

        /// Reserved event type indicating that any event type is acceptable in places where an event type is required
        HID_EVENT_ANY=0xff
};

/// Pack all events to limit the amount of memory used
#pragma pack(1)

/// Basic HID event
typedef PACKED struct
{
    /// Type of event
    BYTE    eventType;

    /// Poll sequence number. Used to identify events from seperate polls
    BYTE    pollSeqn;
}HidEvent;

/// Single axis event structure
typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    /// Motion along the axis
    INT16   motion;
}HidEventMotionSingleAxis;

/// XY motion event structure
typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    /// Motion along axis X
    INT16    motionX;

    /// Motion along axis Y
    INT16   motionY;
}HidEventMotionXY;

/// AB motion event structure
typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    /// Motion along axis A
    INT16   motionA;

    /// Motion along axis B
    INT16   motionB;
}HidEventMotionAB;

/// Button state change event structure
typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    /// New state of buttons
    UINT16    buttonState;
}HidEventButtonStateChange;

/// Key state change event structure
typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    /// Key event.
    KeyEvent keyEvent;
}HidEventKey;

/// Any generic event
typedef PACKED struct
{
  /// Base event info
  HidEvent eventInfo;

  /// The event
  UINT32 anyEvent;
}HidEventAny;

typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    void * userDataPtr;
}HidEventUserDefine;

#pragma pack()

/// @}

#endif
