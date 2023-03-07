// *****************************************************************************
// * sb1-gesture-sensor.c
// *
// * Routines for interfacing with an i2c based sb1 gesture sensor.  This
// * plugin will receive an interrupt when the sb1 has detected a new gesture,
// * which will cause it to perform an i2c transaction to query the message
// * received.  It will eventually generate a callback that will contain the
// * message received from the gesture sensor.
// *
// * Copyright 2015 by Silicon Laboratories. All rights reserved.           *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"
#include "app/util/serial/command-interpreter2.h"
#include EMBER_AF_API_SB1_GESTURE_SENSOR
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include EMBER_AF_API_DEBUG_PRINT

// ------------------------------------------------------------------------------
// Private plugin macros

// The following macros are absolutely necessary for the gesture sensor to
// function.  If the user does not define them, assign default values based on
// the ISA-39 reference design, and generate lots of warnings so the user knows
// they need to be assigned.

// SB1_IRQ_GPIO, should be something like HAL_GIC_GPIO_PORTB
#ifndef SB1_IRQ_GPIO_PORT
#warning "SB1_IRQ_GPIO_PORT not defined, using default: HAL_GIC_GPIO_PORTB"
#define SB1_IRQ_GPIO_PORT              HAL_GIC_GPIO_PORTB
#endif
// SB1_IRQ_PIN, should be something like GPIO_PBIN
#ifndef SB1_IRQ_GPIO_PIN
#warning "SB1_IRQ_GPIO_PIN not defined, using default: 0"
#define SB1_IRQ_GPIO_PIN               0
#endif
#if !defined(CORTEXM3_EFR32)
#ifndef SB1_IRQ_NUMBER
#warning "SB1_IRQ_NUMBER not defined, using default: HAL_GIC_IRQ_NUMA"
#define SB1_IRQ_NUMBER                 HAL_GIC_IRQ_NUMA
#endif
#endif

// SB1 datasheet specified macros
#define SB1_I2C_ADDR                   0xF0
#define SB1_GESTURE_MSG_LEN            5
#define SB1_GESTURE_MSG_IDX_GESTURE    3

// ------------------------------------------------------------------------------
// Private structure and enum declarations
typedef enum {
  SB1_BOTTOM_TOUCH = 0x01,
  SB1_TOP_TOUCH = 0x02,
  SB1_TOP_HOLD = 0x03,
  SB1_BOTTOM_HOLD = 0x04,
  SB1_EITHER_SWIPE_L = 0x05,
  SB1_EITHER_SWIPE_R = 0x06,
  SB1_NONE = 0x00,
  SB1_NOT_READY = 0xFF,
}SB1Message;

// ------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginSb1GestureSensorMessageReadyEventControl;

// ------------------------------------------------------------------------------
// callbacks that must be implemented
void emberAfPluginSb1GestureSensorGestureReceivedCallback(uint8_t gesture,
                                                          uint8_t buttonNum);

// ------------------------------------------------------------------------------
// Forward declaration of plugin CLI functions

// ------------------------------------------------------------------------------
// Global variables
static HalGenericInterruptControlIrqCfg *irqConfig;

// ------------------------------------------------------------------------------
// Plugin consumed callback implementations

// ******************************************************************************
// Plugin init function
// ******************************************************************************
void emberAfPluginSb1GestureSensorInitCallback(void)
{
  // Configure the IRQ for the gesture notification pin
#if defined(CORTEXM3_EFR32)
  irqConfig = halGenericInterruptControlIrqCfgInitialize(SB1_IRQ_GPIO_PIN,
                                                         SB1_IRQ_GPIO_PORT,
                                                         0);
#else
  irqConfig = halGenericInterruptControlIrqCfgInitialize(SB1_IRQ_GPIO_PIN,
                                                         SB1_IRQ_GPIO_PORT,
                                                         SB1_IRQ_NUMBER);
#endif
  halGenericInterruptControlIrqEventRegister(irqConfig,
                                             &emberAfPluginSb1GestureSensorMessageReadyEventControl);
  halGenericInterruptControlIrqEnable(irqConfig);
}

// ------------------------------------------------------------------------------
// Plugin event handlers

// ******************************************************************************
// Event handler called when IRQ goes active.  This is used to prevent I2C
// transactions from occurring within interrupt context.
// ******************************************************************************
void emberAfPluginSb1GestureSensorMessageReadyEventHandler(void)
{
  uint8_t slave_addr;
  uint8_t packet_buffer[SB1_GESTURE_MSG_LEN] = { 0 };
  uint8_t i2cStatus;
  uint8_t buttonNum;
  SB1Message rawMsg;
  Gesture tGestureRxd;

  // We're guaranteed a separate interrupt if the sb1 has another message for
  // us, so we're ok to only execute this once.
  emberEventControlSetInactive(
    emberAfPluginSb1GestureSensorMessageReadyEventControl);

  // Verify that a message still exists that needs to be read
  if (halGenericInterruptControlIrqReadGpio(irqConfig)) {
    return;
  }

  // Perform the i2c read
  slave_addr = SB1_I2C_ADDR;
  i2cStatus = halI2cReadBytes(slave_addr, packet_buffer, SB1_GESTURE_MSG_LEN);

  // Verify transaction succeeded
  if (i2cStatus != I2C_DRIVER_ERR_NONE) {
    emberAfCorePrintln("I2C failed with err code: %d", i2cStatus);

    // If a message is still waiting, either because the i2c transaction failed
    // or because the sb1 had more than one message ready, reschedule this
    // event.
    halSb1GestureSensorCheckForMsg();
    return;
  }

  // Extract raw command from packet and parse it into a generic gesture
  rawMsg = (SB1Message)packet_buffer[SB1_GESTURE_MSG_IDX_GESTURE];
  switch (rawMsg) {
    case SB1_TOP_TOUCH:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_TOUCH;
      break;
    case SB1_BOTTOM_TOUCH:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_BOTTOM;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_TOUCH;
      break;
    case SB1_TOP_HOLD:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_HOLD;
      break;
    case SB1_BOTTOM_HOLD:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_BOTTOM;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_HOLD;
      break;
    case SB1_EITHER_SWIPE_L:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_SWIPE_L;
      break;
    case SB1_EITHER_SWIPE_R:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_SWIPE_R;
      break;
    case SB1_NONE:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_ERR;
      break;
    case SB1_NOT_READY:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_ERR;
      break;
    default:
      buttonNum = SB1_GESTURE_SENSOR_SWITCH_TOP;
      tGestureRxd = SB1_GESTURE_SENSOR_GESTURE_ERR;
      break;
  }

  emberAfCorePrintln("Sb1 gesture 0x%02x on button 0x%02x\n",
                     tGestureRxd, buttonNum);

  // Send the parsed gesture up to the gesture received callback
  emberAfPluginSb1GestureSensorGestureReceivedCallback((uint8_t)tGestureRxd,
                                                       buttonNum);

  // If a message is still waiting, either because the i2c transaction failed
  // or because the sb1 had more than one message ready, reschedule this
  // event.
  halSb1GestureSensorCheckForMsg();
}

// ------------------------------------------------------------------------------
// Plugin public API function implementations

uint8_t halSb1GestureSensorMsgReady(void)
{
#if defined(SB1_IRQ_IN_REG) && defined(SB1_IRQ_PIN)
  // clear int before read to avoid potential of missing interrupt
  if (SB1_IRQ_IN_REG & BIT(SB1_IRQ_PIN & 7)) {
    return(false);
  } else {
    return(true);
  }
#else
  return(false);
#endif
}

uint8_t halSb1GestureSensorCheckForMsg(void)
{
  if (halSb1GestureSensorMsgReady()) {
    emberEventControlSetActive(
      emberAfPluginSb1GestureSensorMessageReadyEventControl);
    return(true);
  }
  return(false);
}

// ------------------------------------------------------------------------------
// Plugin CLI function implementations

// ******************************************************************************
// Cli command to print the state of the IRQ: message ready or no message rdy
// ******************************************************************************
void emAfPluginSb1MessageReady(void)
{
#if defined(SB1_IRQ_IN_REG) && defined(SB1_IRQ_PIN)
  // clear int before read to avoid potential of missing interrupt
  if (SB1_IRQ_IN_REG & BIT(SB1_IRQ_PIN & 7)) {
    emberAfCorePrintln("No msg rdy");
    // interrupt is not active
  } else {
    emberAfCorePrintln("msg rdy");
    // interrupt is active (active low)
  }
#else
  emberAfCorePrintln("ERR: no IRQpin defined for gesture sensor!");
#endif
}

// ******************************************************************************
// CLI command to read a single I2C message.  Good for getting raw trace or
// toggling an unserviced IRQ that didn't get picked up by the int handler
// ******************************************************************************
void emAfPluginSb1ReadMessage(void)
{
  uint8_t slave_addr;
  uint8_t data[SB1_GESTURE_MSG_LEN] = { 0 };
  uint8_t i2cStatus;
  uint8_t i;

  // If I2C interrupt is triggered, parse command
  slave_addr = SB1_I2C_ADDR;
  i2cStatus = halI2cReadBytes(slave_addr, data, SB1_GESTURE_MSG_LEN);
  if (i2cStatus == I2C_DRIVER_ERR_NONE) {
    emberAfCorePrint("Read ");
    for (i = 0; i < SB1_GESTURE_MSG_LEN; i++) {
      emberAfCorePrint("0x%02x ", data[i]);
    }
    emberAfCorePrint("\n");
  } else {
    emberAfCorePrintln("I2C failed with code: 0x%02x\n", i2cStatus);
  }
}

// ******************************************************************************
// plugin sb1 send-gest <gesture> <button>
// This will simulate a gesture being recognized on a button and cause a
// gesture received callback with matching parameters to be generated.  This
// function will perform no sanity checking to verify that the gesture or
// button are sane and exist on the attached sb1 gesture recognition sensor.
// ******************************************************************************
void emAfPluginSb1SendGesture(void)
{
  uint8_t gesture = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t button = (uint8_t)emberUnsignedCommandArgument(1);
  emberAfPluginSb1GestureSensorGestureReceivedCallback(gesture, button);
}
