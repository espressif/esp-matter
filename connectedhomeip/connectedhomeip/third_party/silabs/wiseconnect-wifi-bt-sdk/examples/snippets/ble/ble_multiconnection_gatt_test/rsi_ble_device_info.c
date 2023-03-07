/*******************************************************************************
* @file  rsi_ble_device_info.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @file         rsi_ble_device_info.c
 * @version      0.1
 * @date         19 Mar 2021 *
 *
 *
 *  @brief : This file handles the ble connection identifiers
 *
 *  @section Description  This file handles the ble connection identifiers and status of connection
 */

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include "rsi_driver.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_device_info.h"

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
uint8_t RSI_NULL_BLE_ADDR[RSI_REM_DEV_ADDR_LEN]          = { 0 };
rsi_ble_conn_info_t rsi_ble_conn_info[TOTAL_CONNECTIONS] = { 0 };

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
/*==============================================*/
/**
 * @fn         rsi_get_ble_conn_id
 * @brief      This function returns the ble connection identifier if bd address found
 * @param[in] 	remote_dev_addr -  remote device address
 *				remote_name - remote name
 *				size - size of remote name
 * @param[out] connection identifier (slave1 - 0, slave2 - 1, slave3 - 2, master1 - 3, master2 -4)
 * @return     none.
 * @section description
 * This function returns the ble connection identifier if bd address found
 */

uint8_t rsi_get_ble_conn_id(uint8_t *remote_dev_addr, uint8_t *remote_name, uint8_t size)
{
  uint8_t conn_id = 0xFF; //! Max connections (0xFF -1)
  uint8_t i       = 0;

  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
    if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN)) {
      conn_id = i;
      break;
    }
  }

  //! if bd_addr not found, add to the list
  if (conn_id == 0xFF) {
#if (CONNECT_OPTION == CONN_BY_NAME)
    conn_id = rsi_add_ble_conn_id(remote_dev_addr, remote_name, size);
#else
    conn_id = rsi_add_ble_conn_id(remote_dev_addr, NULL, 0);
#endif
  }

  return conn_id;
}

/*==============================================*/
/**
 * @fn         rsi_get_remote_device_role
 * @brief      This function returns the ble connection role
 * @param[in] 	remote_dev_addr -  remote device address
 * @param[out] role (MASTER_ROLE or SLAVE_ROLE)
 * @return     none.
 * @section description
 * This function returns the ble connection role
 */
uint8_t rsi_get_remote_device_role(uint8_t *remote_dev_addr)
{
  uint8_t role = MASTER_ROLE, i;

  //! Loop all structures and if the device addr is matched for slave structure, then return slave role or else master role
  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
    if (memcmp(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN) == 0) {
      return rsi_ble_conn_info[i].remote_device_role;
    }
  }
  return role; //! Returning role as master
}

/*==============================================*/
/**
 * @fn         rsi_add_ble_conn_id
 * @brief      This function adds the remote_dev_addr/remote_name to local list
 * @param[in] 	remote_dev_addr -  remote device address
 *				remote_name - remote name
 *				size - size of remote name
 * @param[out] connection identifier (slave1 - 0, slave2 - 1, slave3 - 2, master1 - 3, master2 -4)
 * @return     none.
 * @section description
 * This function returns the ble connection identifier if bd address added in list
 */

uint8_t rsi_add_ble_conn_id(uint8_t *remote_dev_addr, uint8_t *remote_name, uint8_t size)
{
  uint8_t conn_id = 0xFF; //! Max connections (0xFF -1)
  uint8_t i       = 0;

  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
    if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, RSI_NULL_BLE_ADDR, RSI_REM_DEV_ADDR_LEN)) {
      memcpy(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN);
#if (CONNECT_OPTION == CONN_BY_NAME)

      rsi_ble_conn_info[i].rsi_remote_name = (uint8_t *)rsi_malloc((size + 1) * sizeof(uint8_t));
      memset(rsi_ble_conn_info[i].rsi_remote_name, 0, size + 1);
      memcpy(rsi_ble_conn_info[i].rsi_remote_name, remote_name, size);
#endif
      conn_id = i;
      break;
    }
  }

  if (conn_id == 0xff) {
    LOG_PRINT("\r\n failed to add dev to connection buffer");
  }

  return conn_id;
}

/*==============================================*/
/**
 * @fn         rsi_remove_ble_conn_id
 * @brief      This function removes the remote_dev_addr from local list
 * @param[in] 	remote_dev_addr -  remote device address
 * @param[out] connection identifier (slave1 - 0, slave2 - 1, slave3 - 2, master1 - 3, master2 -4)
 * @return     none.
 * @section description
 * This function removes the remote_dev_addr from local list
 */
uint8_t rsi_remove_ble_conn_id(uint8_t *remote_dev_addr)
{
  uint8_t conn_id = 0xFF; //! Max connections (0xFF -1)
  uint8_t i       = 0;

  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
    if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN)) {
      memset(rsi_ble_conn_info[i].remote_dev_addr, 0, RSI_REM_DEV_ADDR_LEN);
#if (CONNECT_OPTION == CONN_BY_NAME)
      memset(rsi_ble_conn_info[i].rsi_remote_name, 0, sizeof(uint32_t));
#endif
      conn_id = i;
      break;
    }
  }

  return conn_id;
}

/*==============================================*/
/**
 * @fn         rsi_check_ble_conn_status
 * @brief      This function check the status of connection
 * @param[in] 	connection_id -  ble connection identifier
 * @param[out] RSI_SUCCESS(0) - if passed
 * 				0xff  - 	if failed
 * @return     none.
 * @section description
 * This function check the status of connection
 */
uint8_t rsi_check_ble_conn_status(uint8_t connection_id)
{
  uint8_t i = 0;

  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
    if (rsi_ble_conn_info[i].conn_id == connection_id) {
      return RSI_SUCCESS;
    }
  }

  return 0xFF;
}
