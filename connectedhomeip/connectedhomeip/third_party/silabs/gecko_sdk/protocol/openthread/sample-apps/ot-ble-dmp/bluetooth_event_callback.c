/***************************************************************************//**
 * @file
 * @brief Bluetooth event callback support
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

#if SL_OPENTHREAD_BLE_CLI_ENABLE

#include <openthread/cli.h>
#include "sl_bluetooth.h"
#include "sl_bt_api.h"

extern void printBleAddress(bd_addr address);

void sl_bt_on_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header))
    {
    case sl_bt_evt_system_boot_id:
      {
      	sl_status_t hello_rsp;
      	sl_status_t address_rsp;

		// Call these two APIs upon boot for testing purposes.
		hello_rsp = sl_bt_system_hello();
		otCliOutputFormat("BLE hello: %s\r\n",
			  	(hello_rsp == SL_STATUS_OK) ? "success" : "error");

    	bd_addr address;
    	uint8_t type;
    	address_rsp = sl_bt_system_get_identity_address(&address, &type);

    	if (address_rsp == SL_STATUS_OK) {
        	otCliOutputFormat("BLE address: ");
        	printBleAddress(address);
    	} else {
        	otCliOutputFormat("error: 0x%2x", address_rsp);
    	}
    	otCliOutputFormat("\r\n");
      }
      break;
    case sl_bt_evt_connection_opened_id:
      {
		sl_bt_evt_connection_opened_t *conn_evt =
	  		(sl_bt_evt_connection_opened_t*)&(evt->data);
		otCliOutputFormat("BLE connection opened handle=%d address=", conn_evt->connection);
		printBleAddress(conn_evt->address);
		otCliOutputFormat(" address_type=%d master=%d advertising_set=%d\r\n", conn_evt->connection, conn_evt->master, conn_evt->advertiser);		  
      }
      break;
    case sl_bt_evt_connection_closed_id:
      {
		sl_bt_evt_connection_closed_t *conn_evt =
	  		(sl_bt_evt_connection_closed_t*)&(evt->data);
		otCliOutputFormat("BLE connection closed handle=%d reason=0x%2x\r\n",
			conn_evt->connection, conn_evt->reason);
      }
      break;
    case sl_bt_evt_connection_parameters_id:
      {
		sl_bt_evt_connection_parameters_t *params_evt =
	  		(sl_bt_evt_connection_parameters_t*)&(evt->data);
		otCliOutputFormat("BLE connection parameters handle=%d interval=%d latency=%d timeout=%d security_mode=%d txsize=%d\r\n", params_evt->connection, params_evt->interval, params_evt->latency, params_evt->timeout, params_evt->security_mode, params_evt->txsize);
      }
      break;
    case sl_bt_evt_scanner_scan_report_id:
      {
		sl_bt_evt_scanner_scan_report_t *rsp_evt =
	  		(sl_bt_evt_scanner_scan_report_t *)&(evt->data);
		otCliOutputFormat("BLE scan response address=");
		printBleAddress(rsp_evt->address);
		otCliOutputFormat(" address_type=%d\r\n", rsp_evt->address_type);
      }
      break;
    case sl_bt_evt_gatt_procedure_completed_id:
      {
		sl_bt_evt_gatt_procedure_completed_t* proc_comp_evt =
	  		(sl_bt_evt_gatt_procedure_completed_t*)&(evt->data);
        otCliOutputFormat("BLE procedure completed handle=%d result=0x%2x\r\n",
			  proc_comp_evt->connection,
			  proc_comp_evt->result);
      }
      break;
    default:
      otCliOutputFormat("BLE event: 0x%04x\r\n", SL_BT_MSG_ID(evt->header));
  }
}

#endif // SL_OPENTHREAD_BLE_CLI_ENABLE
