// *******************************************************************
// * sb1-gesture-sensor.h
// *
// * Routines for interfacing with an i2c based sb1 gesture sensor.  This
// * plugin will receive an interrupt when the sb1 has detected a new gesture,
// * which will cause it to perform an i2c transaction to query the message
// * received.  It will eventually generate a callback that will contain the
// * message received from the gesture sensor.
// *
// * Copyright 2015 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

#ifndef __SB1_GESTURE_SENSOR_H__
#define __SB1_GESTURE_SENSOR_H__

// ------------------------------------------------------------------------------
// Public plugin macros

// Define which button code corresponds to which button (top or bottom) off_type
// the sb1 dimmer switch board
#define SB1_GESTURE_SENSOR_SWITCH_TOP     0x00
#define SB1_GESTURE_SENSOR_SWITCH_BOTTOM  0X01

// ------------------------------------------------------------------------------
// Public plugin structure and enum declarations

// Enum used to define possible gestures the sb1 can deliver
typedef enum {
  SB1_GESTURE_SENSOR_GESTURE_TOUCH = 0x01,
  SB1_GESTURE_SENSOR_GESTURE_HOLD = 0x02,
  SB1_GESTURE_SENSOR_GESTURE_SWIPE_R = 0x03,
  SB1_GESTURE_SENSOR_GESTURE_SWIPE_L = 0x04,
  SB1_GESTURE_SENSOR_GESTURE_NONE = 0x00,
  SB1_GESTURE_SENSOR_GESTURE_ERR = 0xFF
}Gesture;

// ------------------------------------------------------------------------------
// Public plugin function declarations

/** @brief Determine if a new gestures is ready to be read.
 *
 * This function can be used to determine if the sb1 has a message
 * pending.  It makes that determination based on the state of the IRQ pin.
 *
 * @return  One if a message is pending, zero if no messages are pending.
 */
uint8_t halSb1GestureSensorMsgReady(void);

/** @brief Determine if a gesture is ready, and if so read it.
 *
 * This function will check if a message is pending from the sb1, and if so
 * schedule an internal event to handle the message.
 *
 * @return  One if a message was pending and thus a read event was scheduled, or
 * zero if no messages were pending (and thus no events were scheduled).
 */
uint8_t halSb1GestureSensorCheckForMsg(void);

#endif // __SB1_GESTURE_SENSOR_H__
