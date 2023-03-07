/***************************************************************************//**
 * @file
 * @brief app_menu.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef APP_MENU_H
#define APP_MENU_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "sl_component_catalog.h"
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
#include "rail_config.h"
#endif
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Enum to use to tell which menu items are visible in which Range Test modes.
typedef enum {
  NO_HIDE,                          ///> Menu item visible in all modes.
  RX_HIDE,                          ///> Item hidden in RX mode.
  TX_HIDE,                          ///> Item hidden in TX mode.
  TRX_HIDE,                         ///> Unused.
} menu_hide_t;

/// Displayable Icon types in an enum.
typedef enum {
  ICON_DOWN,                        ///> Down arrow for menu navigation.
  ICON_PLUS,                        ///> Plus sign if increment is allowed.
  ICON_GO,                          ///> Unused.
  ICON_SHOW,                        ///> 'SHOW' is printed.
  ICON_START,                       ///> 'START' is printed.
  ICON_STOP,                        ///> 'STOP' is printed.
  ICON_BACK,                        ///> 'BACK' is printed.
  ICON_NONE,                        ///> Nothing is printed or drawn.
} menu_item_icon_t;

typedef struct {
  menu_hide_t   menu_hidden;     ///> The mode in which this menu item is hidden.
  bool (*action)(bool);      ///> Handle triggers
  menu_item_icon_t (*display)(char **);  ///> Display function
  bool is_last;
} menu_item_t;

#if defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_4FSK_9P6KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_2FSK_4P8KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_2FSK_2P4KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_450MHZ_2FSK_4P8KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_450MHZ_4FSK_9P6KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_896MHZ_2FSK_40KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_915MHZ_2FSK_10KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_920MHZ_4FSK_400KBPS)
#undef RAIL0_CHANNEL_GROUP_1_PROFILE_BASE
#define RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK
#endif

/// Default payload length value
#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN) || defined(RAIL0_CHANNELS_FOR_915_PROFILE_WISUN) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FAN_1_0) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_HAN)
#define RANGETEST_PAYLOAD_LEN_MIN   (16U)
#else
#define RANGETEST_PAYLOAD_LEN_MIN   (7U)
#endif
#define RANGETEST_PAYLOAD_LEN_MAX   (64U)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern uint8_t number_of_phys;
// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  Initilization of the menu on the display by going through the actions
 *         of all menu items.
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
void menu_init();

/*******************************************************************************
 * @brief  Returns the index of the currently selected menu item.
 *
 * @param  None.
 *
 * @return The index of the currently selected menu item.
 ******************************************************************************/
uint8_t menu_get_actual_id();

/*******************************************************************************
 * @brief  Returns the index of the currently selected menu item offseted by
 *         the hidden menus
 *
 * @param  None.
 *
 * @return The index of the currently selected menu item.
 ******************************************************************************/
uint8_t menu_get_id_with_hidden(uint8_t current_id);

/*******************************************************************************
 * @brief  Returns the menu index that is the top visible menu line.
 *
 * @param  None.
 *
 * @return Pointer to (global) string buffer.
 ******************************************************************************/
uint8_t menu_get_dispaly_start_id();

/*******************************************************************************
 * @brief  Checks isf the given item is the last element.
 *
 * @param  index: Menu item which is to be cheked if last or not.
 *
 * @return True if this is the last item.
 ******************************************************************************/
bool menu_is_last_item(uint8_t index);

/*******************************************************************************
 * @brief  Triggers the display function of the selected item but it just
 *         determines the type of action icon used for this menu.
 *
 * @param  item: The current menu item index.
 *
 * @return The icon of the chosen item.
 ******************************************************************************/
uint8_t menu_item_icon(uint8_t item);

/*******************************************************************************
 * @brief  Handles the list display for menu states that have string values.
 *
 * @param  item: Item string.
 *
 * @return Pointer to (global) string buffer.
 ******************************************************************************/
char *menu_item_str(uint8_t item);

/*******************************************************************************
 * @brief  Tell if menu is hidden in this mode
 *
 * @param  index: which menu item
 *
 * @return bool: true if it is hidden
 ******************************************************************************/
bool menu_item_is_hidden(uint8_t index);

/*******************************************************************************
 * @brief  Handles the list display for menu states that have string values.
 *         Extra aligment is done to pad out the value to  given
 *
 * @param  item: Item string.
 * @param  value: Value string.
 * @param  col: cClumn to align the value to.
 *
 * @return Pointer to (global) string buffer.
 ******************************************************************************/
char* menu_print_aligned(char* item, char* value, uint8_t col);

/*******************************************************************************
 * @brief  Iterates to the next menu item
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
void menu_next_item(void);

/*******************************************************************************
 * @brief  Executes current menu item action
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
bool menu_item_action(void);

/*******************************************************************************
 * @brief  Return the number of the phy used by the program
 *
 * @param  None.
 *
 * @return number_of_phys
 ******************************************************************************/
uint8_t get_number_of_phys(void);

/*******************************************************************************
 * @brief  Helper function to apply all settings for the current phy
 *
 * @param init: Init is needed or not
 *
 * @return None
 ******************************************************************************/
void apply_new_phy(bool init);

#endif  // APP_MENU_H
