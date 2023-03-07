//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"

EmberEventControl emberAfPluginButtonInterfaceButton0PressedEventControl;
EmberEventControl emberAfPluginButtonInterfaceButton0ReleasedEventControl;
EmberEventControl emberAfPluginButtonInterfaceButton1PressedEventControl;
EmberEventControl emberAfPluginButtonInterfaceButton1ReleasedEventControl;
EmberEventControl emberAfPluginButtonInterfaceButtonTimeoutEventControl;

void emberAfPluginButtonInterfaceButtonTimeoutEventHandler(void)
{
}

void emberAfPluginButtonInterfaceButton0PressedEventHandler(void)
{
}

void emberAfPluginButtonInterfaceButton0ReleasedEventHandler(void)
{
}

void emberAfPluginButtonInterfaceButton1PressedEventHandler(void)
{
}

void emberAfPluginButtonInterfaceButton1ReleasedEventHandler(void)
{
}

typedef enum {
  EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_LO = 0x00,
  EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_HI = 0x01,
} HalButtonInterfacePolarity;

uint8_t halPluginButtonInterfaceButtonPoll(uint8_t button)
{
}

void halPluginButtonInterfaceSetButtonPolarity(
  uint8_t                    button,
  HalButtonInterfacePolarity polarity)
{
}

void emberAfHalButtonIsrCallback(uint8_t button, uint8_t state)
{
}
