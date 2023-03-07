/***************************************************************************//**
 * @file
 * @brief app_menu.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdlib.h>
#include "app_menu.h"
#include "sl_component_catalog.h"
#include "rail.h"
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
#include "rail_config.h"
#include "sl_rail_util_init.h"
#endif
#include "app_process.h"
#include "app_measurement.h"
#include "em_common.h"
#if defined(SL_CATALOG_GLIB_PRESENT)
#include "app_graphics.h"
#endif
#include "printf.h"
#include "string.h"
#include <stddef.h>

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
#include "app_bluetooth.h"
#include "gatt_db.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Helper macros for counting and rounding
#define COUNTOF(a) (sizeof(a) / sizeof(a[0]))
#define ROUND_UP(VAL, STEP) (((VAL + (STEP / 2)) / STEP) * STEP)

/// Repeate TX mode
#define RANGETEST_TX_REPEAT               0xFFFF

/// Output power increment when the button is pressed
#define TX_POWER_INC           (5)       // 0.1 dBm / LSB

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
///> Show Info menu.
static bool menu_show_info(bool init);
static menu_item_icon_t menu_show_info_display(char *buff[]);
///> RX or TX Mode.
static bool menu_set_rf_mode(bool init);
static menu_item_icon_t menu_set_rf_mode_display(char *buff[]);
///> PHY selection (e.g. custom_0)
static bool menu_set_phy(bool init);
static menu_item_icon_t menu_set_phy_display(char *buff[]);
///> Antenna Output Power (e.g. +10dBm)
static bool menu_set_rf_pa(bool init);
static menu_item_icon_t menu_set_rf_pa_display(char *buff[]);
///> Modulation Frequency (e.g 868 MHz).
static bool menu_set_rf_frequency(bool init);
static menu_item_icon_t menu_set_rf_frequency_display(char *buff[]);
///> Channel number (e.g. 0..19)
static bool menu_set_channel(bool init);
static menu_item_icon_t menu_set_channel_display(char *buff[]);
///> Transmitted payload length (bytes).
static bool menu_set_packets_length(bool init);
static menu_item_icon_t menu_set_packets_length_Display(char *buff[]);
///> Transmitted packet count (e.g. 500).
static bool menu_set_packets_repeat_number(bool init);
static menu_item_icon_t menu_set_packets_repeat_number_display(char *buff[]);
///> Set ID of the device to send to.
static bool menu_set_destination_id(bool init);
static menu_item_icon_t menu_set_destination_id_display(char *buff[]);
///> Set ID of device.
static bool menu_set_source_id(bool init);
static menu_item_icon_t menu_set_source_id_display(char *buff[]);
///> Set moving average window size.
static bool menu_set_moving_average_window(bool init);
static menu_item_icon_t menu_set_moving_average_window_display(char *buff[]);
///> Turn UART logging on or off.
static bool menu_set_usart_log_enable(bool init);
static menu_item_icon_t menu_set_usart_log_enable_display(char *buff[]);
///> Start Range test.
static bool menu_start_measurment(bool init);
static menu_item_icon_t menu_start_measurment_display(char *buff[]);

/*******************************************************************************
 * Returns the number of the hidden menus in the menu structure
 ******************************************************************************/
static uint8_t menu_all_hidden_menus(void);
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// size of the phy array, calculated at beginning of main
uint8_t number_of_phys = 0;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// variable that holds all the menu items
static const menu_item_t menu_items[] = {
  { NO_HIDE, menu_show_info, menu_show_info_display, false },
  { NO_HIDE, menu_set_rf_mode, menu_set_rf_mode_display, false },
  { NO_HIDE, menu_set_phy, menu_set_phy_display, false },
  { RX_HIDE, menu_set_rf_pa, menu_set_rf_pa_display, false },
  { NO_HIDE, menu_set_rf_frequency, menu_set_rf_frequency_display, false },
  { NO_HIDE, menu_set_channel, menu_set_channel_display, false },
  { NO_HIDE, menu_set_packets_length, menu_set_packets_length_Display, false },
  { NO_HIDE, menu_set_packets_repeat_number, menu_set_packets_repeat_number_display, false },
  { NO_HIDE, menu_set_destination_id, menu_set_destination_id_display, false },
  { NO_HIDE, menu_set_source_id, menu_set_source_id_display, false },
  { TX_HIDE, menu_set_moving_average_window, menu_set_moving_average_window_display, false },
  { NO_HIDE, menu_set_usart_log_enable, menu_set_usart_log_enable_display, false },
  { NO_HIDE, menu_start_measurment, menu_start_measurment_display, true },
};

/// current menu number
static uint8_t menu_id = 0;
/// which menu number should be at the top of the dislay
static uint8_t menu_display_start = 0;
/// print buffer to write out a line
static char print_aligned_buffer[22U];

// -----------------------------------------------------------------------------
//                          Weak Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void menu_set_std_phy(bool init)
{
  (void)init;  // Unused parameter.
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void print_standard_name(char *print_buffer)
{
  (void)print_buffer;  // Unused parameter.
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void reset_payload_length_for_standard(void)
{
  return;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void handle_payload_length_for_standard(void)
{
  return;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void set_standard_phy_channel(void)
{
  return;
}

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  Return the number of the phy used by the program
 *
 * @param  None.
 *
 * @return number_of_phys
 ******************************************************************************/
uint8_t get_number_of_phys(void)
{
  return number_of_phys;
}

/*******************************************************************************
 * @brief  Initilization of the menu on the display by going through the actions
 *         of all menu items.
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
void menu_init()
{
  // Go through menu item handlers and let them init themselves
  uint8_t i = 0U;
  while (true) {
    menu_items[i].action(true);
    if (menu_items[i].is_last) {
      break;
    }
    i++;
  }
}

/*******************************************************************************
 * @brief  Iterates to the next menu item
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
void menu_next_item(void)
{
  if (menu_items[menu_id].is_last) {
    menu_id = 0;
    menu_display_start = 0;
  } else {
    while (true) {
      menu_id++;
      if (!menu_item_is_hidden(menu_id)) {
        break;
      }
    }
#if defined(SL_CATALOG_GLIB_PRESENT)
    if ( (menu_get_id_with_hidden(menu_id) >= (GRAPHICS_MENU_DISP_SIZE / 2U))
         && (((COUNTOF(menu_items) - menu_all_hidden_menus()) - menu_get_id_with_hidden(menu_id)) > (GRAPHICS_MENU_DISP_SIZE / 2U)) ) {
      menu_display_start++;
    }
#endif
  }
}

/*******************************************************************************
 * @brief  Tell if menu is hidden in this mode
 *
 * @param  index: which menu item
 *
 * @return bool: true if it is hidden
 ******************************************************************************/
bool menu_item_is_hidden(uint8_t index)
{
  if (range_test_settings.radio_mode == RADIO_MODE_RX) {
    if (menu_items[index].menu_hidden == RX_HIDE) {
      return true;
    } else {
      return false;
    }
  } else if (range_test_settings.radio_mode == RADIO_MODE_TX) {
    if (menu_items[index].menu_hidden == TX_HIDE) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

/*******************************************************************************
 * @brief  Executes current menu item action
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
bool menu_item_action(void)
{
  return menu_items[menu_id].action(false);
}

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
char* menu_print_aligned(char* item, char* value, uint8_t col)
{
  uint8_t ii = 0U;

  while (*item) {
    print_aligned_buffer[ii] = *item;
    item++;
    ii++;
  }

  while (ii < (col - strlen(value))) {
    print_aligned_buffer[ii] = ' ';
    ii++;
  }

  while (*value) {
    print_aligned_buffer[ii] = *value;
    value++;
    ii++;
  }
  print_aligned_buffer[ii] = 0U;

  return print_aligned_buffer;
}

/*******************************************************************************
 * @brief  Returns the index of the currently selected menu item.
 *
 * @param  None.
 *
 * @return The index of the currently selected menu item.
 ******************************************************************************/
uint8_t menu_get_actual_id()
{
  return menu_id;
}

/*******************************************************************************
 * @brief  Returns the index of the currently selected menu item offseted by
 *         the hidden menus
 *
 * @param  None.
 *
 * @return The index of the currently selected menu item.
 ******************************************************************************/
uint8_t menu_get_id_with_hidden(uint8_t current_id)
{
  uint8_t hidden_menus = 0;
  for (int i = 0; i < current_id; ++i) {
    if (menu_item_is_hidden(i)) {
      hidden_menus++;
    }
  }
  return (menu_id - hidden_menus);
}

/*******************************************************************************
 * @brief  Returns the menu index that is the top visible menu line.
 *
 * @param  None.
 *
 * @return Pointer to (global) string buffer.
 ******************************************************************************/
uint8_t menu_get_dispaly_start_id()
{
  return menu_display_start;
}

/*******************************************************************************
 * @brief  Checks isf the given item is the last element.
 *
 * @param  index: Menu item which is to be cheked if last or not.
 *
 * @return True if this is the last item.
 ******************************************************************************/
bool menu_is_last_item(uint8_t index)
{
  return menu_items[index].is_last;
}

/*******************************************************************************
 * @brief  Handles the list display for menu states that have string values.
 *
 * @param  item: Item string.
 *
 * @return Pointer to (global) string buffer.
 ******************************************************************************/
char *menu_item_str(uint8_t item)
{
  char *ret;

  menu_items[item].display(&ret);
  return ret;
}

/*******************************************************************************
 * @brief  Triggers the display function of the selected item but it just
 *         determines the type of action icon used for this menu.
 *
 * @param  item: The current menu item index.
 *
 * @return The icon of the chosen item.
 ******************************************************************************/
uint8_t menu_item_icon(uint8_t item)
{
  return menu_items[item].display(NULL);
}

/*******************************************************************************
 * @brief  Helper function to apply all settings for the current phy
 *
 * @param init: Init is needed or not
 *
 * @return None
 ******************************************************************************/
void apply_new_phy(bool init)
{
  menu_set_std_phy(init);
  set_power_level_to_max(init);
  // Initialise all dependent menu items.
  menu_set_rf_frequency(true);
  menu_set_rf_pa(true);
  menu_set_channel(true);
  menu_set_packets_length(true);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  Returns the number of the hidden menus in the menu structure
 *
 * @param  None.
 *
 * @return all hidden menus count
 ******************************************************************************/
static uint8_t menu_all_hidden_menus(void)
{
  uint8_t hidden_menu_counts = 0;
  uint8_t index = 0;
  while (true) {
    if (menu_item_is_hidden(index)) {
      hidden_menu_counts++;
    }
    if (menu_is_last_item(index)) {
      break;
    } else {
      index++;
    }
  }
  return hidden_menu_counts;
}

/*******************************************************************************
 * @brief  Handles the list display for menu states that have string values.
 *
 * @param  item: Item string.
 * @param  value: Value string.
 *
 * @return Pointer to (global) string buffer.
 ******************************************************************************/
static char* menu_print(char* item, char* value)
{
  return (menu_print_aligned(item, value, 20U));
}

/*******************************************************************************
 * @brief  Trigger function that shows the info screen upon a key press
 *         when the Show Information menu item is selected.
 *
 * @param  init: Request resetting.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_show_info(bool init)
{
  if (init) {
    return true;
  }
  set_next_state(INFO_SCREEN);
  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         to show some information about the Range Test Example.
 *
 * @param  buff[]: Buffer that is to be filled out with the string
 *                 that will be printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_show_info_display(char *buff[])
{
  if (buff) {
    *buff = "Show Information";
  }
  return ICON_SHOW;
}

/*******************************************************************************
 * @brief  This function selects between available radio modes.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_rf_mode(bool init)
{
  range_test_settings.radio_mode++;
  if ((range_test_settings.radio_mode >= (RADIO_MODE_TX + 1)) || (init)) {
    range_test_settings.radio_mode = RADIO_MODE_RX;  // 0U
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_radioMode);
#endif

  return true;
}

/*******************************************************************************
 * @brief  This function shows selected radio mode.
 *
 * @param  buff[]: Buffer that is to be filled out with the string
 *                 that will be printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
menu_item_icon_t menu_set_rf_mode_display(char *buff[])
{
  if (buff) {
    *buff = menu_print("Mode:",
                       (range_test_settings.radio_mode == RADIO_MODE_TX) ? ("TX") \
                       : ((range_test_settings.radio_mode == RADIO_MODE_RX) ? ("RX") : ("TRX")));
  }
  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects (cycles) which PHY to use for TX/RX.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_phy(bool init)
{
  range_test_settings.current_phy++;
  if ((range_test_settings.current_phy >= number_of_phys) || (init)) {
    // Initialize the channel to the first one in channel config
    range_test_settings.current_phy = 0;
  }

  apply_new_phy(init);

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_phy);
#endif
  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         that sets the PHY.
 *
 * @param  buff[]: Buffer that is to be filled out with the string
 *                 that will be printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_set_phy_display(char *buff[])
{
  char pVal[15U];
  if (buff) {
    if (!is_current_phy_standard()) {
      snprintf(pVal, sizeof(pVal), "custom_%u", (range_test_settings.current_phy));
    } else {
      print_standard_name(pVal);
    }
    *buff = menu_print("PHY:", pVal);
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function sets the output power.
 *
 * @param  init: User (re)init request.
 *
 * @return True if value was changed, false if not.
 ******************************************************************************/
static bool menu_set_rf_pa(bool init)
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  if (init) {
    // Init the rangetest power field to the current power level
    // to sync demanded and current value.
    range_test_settings.tx_power = RAIL_GetTxPowerDbm(rail_handle);
    if ((range_test_settings.tx_power % TX_POWER_INC) != 0 ) {
      range_test_settings.tx_power = ROUND_UP(range_test_settings.tx_power, TX_POWER_INC);
    }
  } else {
    range_test_settings.tx_power += TX_POWER_INC;
    // Limit and cycle requested power.
    if ( range_test_settings.tx_power > get_max_tx_power_deci_dbm()) {
      range_test_settings.tx_power = get_min_tx_power_deci_dbm();
    }
  }

  update_tx_power();

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_txPower);
#endif
  return true;
}

/*******************************************************************************
 * @brief  This function sets the display for PA
 *
 * @param  *buff[]: where to print the power value
 *
 * @return menu_item_icon_t what icon is needed in the right corner
 ******************************************************************************/
static menu_item_icon_t menu_set_rf_pa_display(char *buff[])
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  // Enough to hold a sign and two digits,
  // one decimal point and one decimal digist and dBm */
  char pVal[15U];
  // Temporary power variable for actual RAIL output power.
  int16_t power;
  // Temporary power variable for the requested power.
  int16_t reqpower;

  if (buff) {
    power = (int16_t) RAIL_GetTxPowerDbm(rail_handle);
    reqpower = range_test_settings.tx_power;
    snprintf(pVal, sizeof(pVal),
             "%+i.%d/%+i.%ddBm",
             (reqpower / 10),
             (((reqpower > 0) ? (reqpower) : (-reqpower)) % 10),
             (power / 10),
             (((power > 0) ? (power) : (-power)) % 10));
    *buff = menu_print("Power:", pVal);
  }
  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function would be used to select from radio frequencies.
 * @note   This function is not implemented (read-only).
 *
 * @param  init: User (re)init request.
 *
 * @return Always false.
 ******************************************************************************/
static bool menu_set_rf_frequency(bool init)
{
  (void) init;
  return false;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         to shows the frequency.
 *
 * @param  buff[]: Buffer that is to be filled out with the string
 *                 that will be printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu
 *         ICON_NONE means that this menu item is read-only.
 ******************************************************************************/
static menu_item_icon_t menu_set_rf_frequency_display(char *buff[])
{
  char pVal[11U];

  if (buff) {
    // Check if frequency has fractional part in MHz scale
    uint32_t base_frequency = 0;
    uint32_t channel_spacing = 0;
    int16_t power = 0;

    get_rail_config_data(&base_frequency, &channel_spacing, &power);

    if (base_frequency % 1000000U) {
      snprintf(pVal, sizeof(pVal),
               "%u.%02uMHz",
               (uint16_t) (base_frequency / 1000000U),
               (uint16_t) ((base_frequency % 1000000U) / 10000U));
    } else {
      snprintf(pVal, sizeof(pVal),
               "%uMHz",
               (uint16_t) (base_frequency / 1000000U));
    }

    *buff = menu_print("Frequency:", pVal);
  }

  return ICON_NONE;
}

/*******************************************************************************
 * @brief  This function selects (cycles) which channel to use for TX/RX.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_channel(bool init)
{
  if (!is_current_phy_standard()) {
    range_test_settings.channel++;
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
    uint16_t channel = range_test_settings.channel;
    if ((channel > channelConfigs[range_test_settings.current_phy]->configs[0U].channelNumberEnd)
        || (init)) {
      // Initialize the channel to the first one in channel config
      range_test_settings.channel = channelConfigs[range_test_settings.current_phy]->configs[0U].channelNumberStart;
    }
#endif
  } else {
    set_standard_phy_channel();
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_channel);
#endif

  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         that sets the channel.
 *
 * @param  buff[]: Buffer that is to be filled out with the string
 *                 that will be printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_set_channel_display(char *buff[])
{
  char pVal[4U];

  if (buff) {
    snprintf(pVal, sizeof(pVal), "%u", range_test_settings.channel);
    *buff = menu_print("Channel number:", pVal);
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects (cycles) between packet lengths.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_packets_length(bool init)
{
  range_test_settings.payload_length++;
  if (is_current_phy_standard()) {
    if (init) {
      reset_payload_length_for_standard();
    } else {
      handle_payload_length_for_standard();
    }
  } else {
    if ((range_test_settings.payload_length > RANGETEST_PAYLOAD_LEN_MAX) || (init)) {
      range_test_settings.payload_length = RANGETEST_PAYLOAD_LEN_MIN;
    }
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_payload);
#endif

  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         that shows/sets the channel.
 *
 * @param  Buffer that is to be filled out with the string that will be printed
 *         in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_set_packets_length_Display(char *buff[])
{
  char pVal[3U];

  if (buff) {
    snprintf(pVal, sizeof(pVal), "%u", range_test_settings.payload_length);
    *buff = menu_print("Payload length:", pVal);
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects (cycles) between trasmitted packet count
 *         options.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_packets_repeat_number(bool init)
{
  static uint16_t pktsNum[8U] =
  { 500U, 1000U, 2500U, 5000U, 10000U, 25000U, 50000U, RANGETEST_TX_REPEAT };
  static uint8_t i = 1U;

  if (!init) {
    i++;
    if (i >= sizeof(pktsNum) / sizeof(*pktsNum)) {
      i = 0U;
    }
  }
  range_test_settings.packets_repeat_number = pktsNum[i];

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_pktsReq);
#endif

  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         that shows/sets the number of packets to be transmitted.
 *
 * @param  buff[]: Buffer that is to be filled out with the string
 *                 that will be printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_set_packets_repeat_number_display(char *buff[])
{
  char pVal[6U];

  if (buff) {
    if (range_test_settings.packets_repeat_number != RANGETEST_TX_REPEAT) {
      snprintf(pVal, sizeof(pVal), "%u", range_test_settings.packets_repeat_number);
    }
    *buff = menu_print("Packet Count:",
                       (range_test_settings.packets_repeat_number != RANGETEST_TX_REPEAT) ? (pVal) : ("Repeat"));
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects (cycles) between destination IDs.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_destination_id(bool init)
{
  range_test_settings.destination_id++;
  if ((range_test_settings.destination_id > 32U) || (init)) {
    range_test_settings.destination_id = 0U;
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_destID);
#endif

  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu to sets/shows
 *         the destination ID.
 *
 * @param  buff[]: Buffer that is to be filled out with the string that will be
 *                 printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_set_destination_id_display(char *buff[])
{
  char pVal[4U];

  if (buff) {
    snprintf(pVal, sizeof(pVal), "%u", range_test_settings.destination_id);
    *buff = menu_print("Remote ID:", pVal);
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects (cycles) between source IDs.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_source_id(bool init)
{
  range_test_settings.source_id++;
  if ((range_test_settings.source_id > 32U) || (init)) {
    range_test_settings.source_id = 0U;
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_srcID);
#endif
  return true;
}

/*******************************************************************************
 * @brief      This function sets the printed menu text for the menu
 *             to sets/shows the source ID.
 *
 * @param[out] buff[]: Buffer that is to be filled out with the string
 *                           that will be printed in th menu
 *
 * @return     Icon type to be used by the caller for this menu
 ******************************************************************************/
static menu_item_icon_t menu_set_source_id_display(char *buff[])
{
  char pVal[4U];

  if (buff) {
    snprintf(pVal, sizeof(pVal), "%u", range_test_settings.source_id);
    *buff = menu_print("Self ID:", pVal);
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects (cycles) between moving average window
 *         sizes.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_moving_average_window(bool init)
{
  if ((range_test_settings.moving_average_window_size == 128U) || (init)) {
    range_test_settings.moving_average_window_size = 32U;
  } else {
    range_test_settings.moving_average_window_size <<= 1U;
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_maSize);
#endif

  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu to sets/shows
 *         the size of the moving average window.
 *
 * @param  buff[]: Buffer that is to be filled out with the string that will be
 *                 printed in th menu
 *
 * @return Icon type to be used by the caller for this menu
 ******************************************************************************/
static menu_item_icon_t menu_set_moving_average_window_display(char *buff[])
{
  char pVal[4U];

  if (buff) {
    snprintf(pVal, sizeof(pVal), "%u", range_test_settings.moving_average_window_size);
    *buff = menu_print("MA Window size:", pVal);
  }

  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function selects if logging is enabled or not.
 *
 * @param  init: User (re)init request.
 *
 * @return Always true.
 ******************************************************************************/
static bool menu_set_usart_log_enable(bool init)
{
  range_test_settings.usart_log_enable = !range_test_settings.usart_log_enable;

  if (init) {
    range_test_settings.usart_log_enable = false;
  }
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_log);
#endif
  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu
 *         to shows/sets .
 *
 * @param  buff[]: Buffer that is to be filled out with the string that will be
 *                 printed in th menu.
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_set_usart_log_enable_display(char *buff[])
{
  if (buff) {
    *buff = menu_print("UART log enable:",
                       (range_test_settings.usart_log_enable) ? ("Yes") : ("No"));
  }
  return ICON_PLUS;
}

/*******************************************************************************
 * @brief  This function executes the Range Test in the selected mode.
 *
 * @param  init: User (re)init request.
 *
 * @return False when init requested, true if not.
 ******************************************************************************/
static bool menu_start_measurment(bool init)
{
  // Nothing to init here
  if (init) {
    return false;
  }

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  add_bluetooth_indication(gattdb_isRunning);
#endif

  set_next_state(START_MEASURMENT);
  return true;
}

/*******************************************************************************
 * @brief  This function sets the printed menu text for the menu for executing
 *         the Range Test.
 *
 * @param  buff[]: Buffer that is to be filled out with the string that will be
 *                 printed in th menu
 *
 * @return Icon type to be used by the caller for this menu.
 ******************************************************************************/
static menu_item_icon_t menu_start_measurment_display(char *buff[])
{
  if (buff) {
    *buff = "Start Range Test";
  }
  return ICON_START;
}
