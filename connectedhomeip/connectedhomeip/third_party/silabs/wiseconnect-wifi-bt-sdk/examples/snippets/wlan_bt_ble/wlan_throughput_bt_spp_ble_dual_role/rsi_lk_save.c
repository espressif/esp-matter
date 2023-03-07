/*******************************************************************************
* @file  rsi_lk_save.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file    rsi_lk_save.c
 * @version 0.1
 * @date    xx Oct 2018
 *
 *
 *
 *  @brief : This file contains the interface for bluetooth link key storage commands
 *
 *
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

#include "rsi_common_config.h"
#if RSI_ENABLE_BT_TEST
#include <rsi_bt_config.h>
#if RPS_LINK_KEY_SAVE
#include <string.h>
#include <fsl_debug_console.h>
#include <rsi_bt_apis.h>

#include "ff.h"
#include "diskio.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#define LK_READ_BUF_SZ (512)
#define FALSE          0
#define TRUE           1

typedef struct link_key_save_s {
  rsi_bt_event_user_linkkey_save_t link_key_list[MAX_LINK_KEYS];
} link_key_save_t;

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
static const dev_address_type blank_addr = { 0 };
static FIL lk_fp;
static uint8_t keys_read_buff[LK_READ_BUF_SZ];
static link_key_save_t link_key_save;

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
static bool rsi_restore_link_keys(void);
static bool rsi_save_link_keys(void);
bool rsi_link_key_remove(dev_address_type addr1);
bool rsi_link_key_add(rsi_bt_event_user_linkkey_save_t *link_key_update);
bool rsi_link_key_read(rsi_bt_event_user_linkkey_save_t *peer_link_key);
void rsi_init_link_key_list(void);

/*==============================================*/
/**
 * @fn         rsi_init_link_key_list
 * @brief      Initialises the key list with the stored keys along with its addresses
 * @param[in]  None
 * @param[out] None
 * @return     none.
 * @section description
 * This callback function initialises the key list with the stored keys along with its addresses
 */
void rsi_init_link_key_list(void)
{
  memset(&link_key_save, 0, sizeof(link_key_save_t));

  //! Load keys from SD card to key list. If no keys exist, then set list to 0's
  if (!rsi_restore_link_keys()) {
    rsi_save_link_keys();
  }
}

/**
 * @fn         rsi_link_key_read
 * @brief      Reads link key from SD card based on the input address
 * @param[in/out]  remote_link_key, pointer to the Link key structure
 * @return     status, TRUE if the address is matched to key list and link key is updated.
 *                     FALSE if address is not matched to key list.
 * @section description
 * This callback function reads link key from SD card based on the input address
*/
bool rsi_link_key_read(rsi_bt_event_user_linkkey_save_t *remote_link_key)
{
  bool status = FALSE;
  uint8_t i;
  for (i = 0; i < MAX_LINK_KEYS; i++) {
    if (memcmp(remote_link_key->dev_addr, link_key_save.link_key_list[i].dev_addr, RSI_DEV_ADDR_LEN) == 0) {
      memcpy(remote_link_key, &link_key_save.link_key_list[i], sizeof(rsi_bt_event_user_linkkey_save_t));
      status = TRUE;
      break;
    }
  }
  return status;
}

/**
 * @fn         rsi_link_key_add
 * @brief      Saves the link key of the paired device into SD card using its address
 * @param[in]  remote_link_key, pointer to the Link key structure
 * @return     list_updated, TRUE if link key is stored to SD card.
 *                           FALSE if link key is not stored to SD card.
 * @section description
 * This callback function saves the link key of the paired device into SD card using the address
*/
bool rsi_link_key_add(rsi_bt_event_user_linkkey_save_t *remote_link_key)
{
  uint8_t i;
  bool list_updated;

  // Init variables
  list_updated = FALSE;
  // Check for update
  for (i = 0; i < MAX_LINK_KEYS; i++) {
    // Found previous address update link key
    if (0 == memcmp(remote_link_key->dev_addr, link_key_save.link_key_list[i].dev_addr, RSI_DEV_ADDR_LEN)) {
      memcpy(&link_key_save.link_key_list[i], remote_link_key, sizeof(rsi_bt_event_user_linkkey_save_t));
      list_updated = TRUE;
      break;
    }
  }

  // Check for new add
  if (!list_updated) {
    for (i = 0; i < MAX_LINK_KEYS; i++) {
      // Found previous address update link key
      if (memcmp(blank_addr, link_key_save.link_key_list[i].dev_addr, RSI_DEV_ADDR_LEN) == 0) {
        memcpy(&link_key_save.link_key_list[i], remote_link_key, sizeof(rsi_bt_event_user_linkkey_save_t));
        list_updated = TRUE;
        break;
      }
    }
  }

  if (list_updated) {
    rsi_save_link_keys();
  }

  return list_updated;
}

/**
 * @fn         rsi_link_key_remove
 * @brief      Removes the link key of the paired device from keys list using its address
 * @param[in]  dev_addr, pointer to the address of the paired device
 * @return     key_removed,  TRUE if link key is removed from key list
 *                           FALSE if link key is not removed from key list
 * @section description
 * This callback function removes the link key of the paired device from keys list using its address
*/

bool rsi_link_key_remove(dev_address_type dev_addr)
{
  bool key_removed = FALSE;
  uint8_t i;
  for (i = 0; i < MAX_LINK_KEYS; i++) {
    // Found previous address update link key
    if (memcmp(dev_addr, link_key_save.link_key_list[i].dev_addr, RSI_DEV_ADDR_LEN) == 0) {
      memset(&link_key_save.link_key_list[i], 0, sizeof(rsi_bt_event_user_linkkey_save_t));
      key_removed = TRUE;
      break;
    }
  }
  if (key_removed) {
    rsi_save_link_keys();
  }
  return key_removed;
}

/**
 * @fn         rsi_restore_link_keys
 * @brief      Restores the link key list from the SD card
 * @param[in]  None
 * @param[out] None
 * @return     TRUE if link key list is restored
 *             FALSE if link key list is not restored
 * @section description
 * This callback function restores the link key list from the SD card
*/

static bool rsi_restore_link_keys(void)
{
  FRESULT error;
  UINT bytesRead = 1;
  rsi_bt_event_user_linkkey_save_t *link_key_ptr;
  error = f_open(&lk_fp, _T("/linkKey.dat"), (FA_READ | FA_OPEN_ALWAYS));
  if (error) {
    return FALSE;
  }

  f_lseek(&lk_fp, 0U);
  link_key_ptr = &link_key_save.link_key_list[0];

  while (bytesRead > 0) {
    memset(keys_read_buff, '\0', sizeof(rsi_bt_event_user_linkkey_save_t));
    error = f_read(&lk_fp, keys_read_buff, sizeof(rsi_bt_event_user_linkkey_save_t), &bytesRead);
    if (error) {
      return FALSE;
    }

    if (bytesRead == sizeof(rsi_bt_event_user_linkkey_save_t)) {
      uint8_t str_conn_bd_addr[BD_ADDR_ARRAY_LEN];
      memcpy(link_key_ptr, keys_read_buff, sizeof(rsi_bt_event_user_linkkey_save_t));

      LOG_PRINT("[restore_lk_keys]: Read Key for -> %s\r\n",
                rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, link_key_ptr->dev_addr));
      link_key_ptr++;
    }
  }

  f_close(&lk_fp);
  return TRUE;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       rsi_save_link_keys - Saves link key structure to nonvol
*
*********************************************************************/

/**
 * @fn         rsi_save_link_keys
 * @brief      Saves the link keys from link key list to the SD card
 * @param[in]  None
 * @param[out] None
 * @return     TRUE if link key list is saved to SD card
 *             FALSE if link key list is not saved to SD card
 * @section description
 * This callback function saves the link keys from link key list to the SD card
*/

static bool rsi_save_link_keys(void)
{
  bool key_saved = TRUE;
  FRESULT error;
  uint8_t i;
  UINT bytesWritten;
  error = f_open(&lk_fp, _T("/linkKey.dat"), (FA_WRITE | FA_OPEN_ALWAYS));
  if (error) {
    key_saved = FALSE;
    return key_saved;
  }

  for (i = 0; i < MAX_LINK_KEYS; i++) {
    error = f_write(&lk_fp, &link_key_save.link_key_list[i], sizeof(rsi_bt_event_user_linkkey_save_t), &bytesWritten);
    if (error) {
      key_saved = FALSE;
      break;
    }
  }

  f_close(&lk_fp);
  return key_saved;
}
#endif
#endif
