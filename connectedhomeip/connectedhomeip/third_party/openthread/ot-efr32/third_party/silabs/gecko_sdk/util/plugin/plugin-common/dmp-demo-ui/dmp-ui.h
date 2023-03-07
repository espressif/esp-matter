/****************************************************************************/
/**
 * @file dmp-ui.h
 * @brief UI interface for DMP demo
 * @version 0.0.1
 ******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef DMP_UI_H
#define DMP_UI_H

/******************************************************************************/
/**
 * @addtogroup UI Interface for DMP Demo
 * @{
 *
 * DMP UI uses the underlying DMD interface and the GLIB and exposes several
 * wrapper functions to application. These functions are used to display
 * different bitmaps for the demo.
 *
 ******************************************************************************/

#define PROT_ZIGBEE     (1)
#define PROT_RAIL       (2)
#define PROT_CONNECT    (3)
#define PROT_BLUETOOTH  (4)
#define PROT_THREAD     (5)

#ifdef EMBER_STACK_ZIGBEE
#define PROT1           (PROT_ZIGBEE)
#elif EMBER_STACK_CONNECT
#define PROT1           (PROT_CONNECT)
#elif EMBER_STACK_IP
#define PROT1           (PROT_THREAD)
#else
#define PROT1           (PROT_RAIL)
#endif

#define PROT2           (PROT_BLUETOOTH)

#define DMP_UI_PJOIN_EVENT_DURATION           (500)

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

typedef enum {
  DMP_UI_PROTOCOL1,
  DMP_UI_PROTOCOL2
} DmpUiProtocol;

typedef enum {
  DMP_UI_LIGHT_OFF,
  DMP_UI_LIGHT_ON
} DmpUiLightState_t;

typedef enum {
  DMP_UI_DIRECTION_PROT1,
  DMP_UI_DIRECTION_PROT2,
  DMP_UI_DIRECTION_SWITCH,
  DMP_UI_DIRECTION_INVALID
} DmpUiLightDirection_t;

typedef enum {
  DMP_UI_NO_NETWORK,
  DMP_UI_SCANNING,
  DMP_UI_JOINING,
  DMP_UI_FORMING,
  DMP_UI_NETWORK_UP,
  DMP_UI_STATE_UNKNOWN
} DmpUiZigBeeNetworkState_t;

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/

/**
 * @brief
 *   Initilize the GLIB and DMD interfaces.
 *
 * @param[in] void
 *
 * @return
 *      void
 */

void dmpUiInit(void);

/**
 * @brief
 *   Update the display with Silicon Labs logo and application name.
 *
 * @param[in] name name of the current application.
 *
 * @return
 *      void
 */
void dmpUiDisplayHeader(uint8_t* name);

/**
 * @brief
 *   Update the display with Help menu.
 *
 * @param[in] networkForming whether UI displays network forming related components.
 *
 * @return
 *      void
 */
void dmpUiDisplayHelp(bool networkForming);

/**
 * @brief
 *   Update the display with light bulb image.
 *
 * @param[in] on status of light bulb
 *
 * @return
 *      void
 */
void dmpUiDisplayLight(bool on);

/**
 * @brief
 *   Update the display to show if the bluetooth is connected to the mobile device.
 *
 * @param[in] bool, true if the Light is connected to mobile device, false otherwise.
 *
 * @return
 *      void
 */
void dmpUiDisplayProtocol(DmpUiProtocol protocol, bool isConnected);

/**
 * @brief
 *   Update the display to show which interface toggled the light.
 *
 * @param[in] DMP_UI_DIRECTION_BLUETOOTH or DMP_UI_DIRECTION_ZIGBEE
 *
 * @return
 *      void
 */
void dmpUiDisplayDirection(DmpUiLightDirection_t direction);

/**
 * @brief
 *   Update the display to clear signs used to indicate light toggle source.
 *
 * @param[in] DMP_UI_DIRECTION_BLUETOOTH or DMP_UI_DIRECTION_ZIGBEE
 *
 * @return
 *      void
 */
void dmpUiClearDirection(DmpUiLightDirection_t direction);

/**
 * @brief
 *   Update the display to show the device id.
 *
 * @param[in] protocol - DMP_UI_PROTOCOL1 or DMP_UI_PROTOCOL2 (left or right)
 * @param[in] id - id to show, max 9 byte long string.
 *
 * @return
 *      void
 */
void dmpUiDisplayId(DmpUiProtocol protocol, uint8_t* id);

/**
 * @brief
 *   Clear the Lcd screen and display the main screen.
 *
 * @param[in] name - application name
 * @param[in] showPROT1 - show protocol 1 related icon.
 * @param[in] showPROT2 - show protocol 2 related icon.
 *
 * @return
 *      void
 */
void dmpUiClearMainScreen(uint8_t* name, bool showPROT1, bool showPROT2);

/**
 * @brief
 *   Update the current operating channel.
 *
 * @param[in] channel
 *
 * @return
 *      void
 */
void dmpUiDisplayChan(uint8_t channel);

/**
 * @brief
 *   Update the display with flashing PAN Id sequence to indicate that the pjoin
 * is active. The duration between the PAN Id flickers is passed by the user. The
 * flashing sequence is stopped automatically once the pjoin is disabled.
 *
 * @param[in] duration in milliseconds, between the flashes of PAN Id.
 *
 * @return
 *      void
 */
//void dmpEventZigBeePjoin(uint16_t duration);

/**
 * @brief
 *   Update the display with PAN Id, after the device joins or leaves the network.
 *
 * @param[in] bool, if the PanId display is initiated by the application, a
 * display update needs to follow the PanId display, so the application should
 * request for the display update by passing true to this function.
 *
 * @return
 *      void
 */

//void dmpUiDisplayZigBeeState(DmpUiZigBeeNetworkState_t nwState);

/**
 * @brief
 *   Set BLE device name to be displayed on the LCD.
 *
 * @param[in] BLE device name.
 *
 * @return
 *      void
 */
//void dmpUiSetBleDeviceName(char *devName);

/**
 * @brief
 *   Set whether to display ZigBee PanId or not on the LCD.
 *
 * @param[in] to block display or not.
 *
 * @return
 *      void
 */
//void dmpUiDisplayPanId(bool display);
//
//void dmpUiUpdateZigbeeStatus(DmpUiZigBeeNetworkState_t nwState,
//                             bool withDisplayUpdate);
//
//bool dmpUiHelpMenuDisplayed();
//void dmpUiDisplayDirection(DmpUiLightDirection_t direction);
//DmpUiLightDirection_t dmpUiCurrentLightDirection();

#endif //DMP_UI_H
