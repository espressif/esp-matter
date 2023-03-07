#include <bcomdef.h>
#include <ti/display/Display.h>

#include <menu/two_btn_menu.h>
#include "simple_peripheral_menu.h"
#include "simple_peripheral.h"
#include "ti_ble_config.h"

/* Main Menu Object */
tbmMenuObj_t spMenuMain;
tbmMenuObj_t spMenuAutoConnect;
tbmMenuObj_t spMenuSelectConn;
tbmMenuObj_t spMenuPerConn;
tbmMenuObj_t spMenuConnPhy;

/*
 * Menu Lists Initializations
 */

void SimplePeripheral_buildMenu(void)
{
// Menu: Main
// upper: none
  MENU_OBJ(spMenuMain, NULL, 2, NULL)
  MENU_ITEM_SUBMENU(spMenuMain,0,&spMenuSelectConn)
  MENU_ITEM_SUBMENU(spMenuMain,1,&spMenuAutoConnect)
  MENU_OBJ_END

// Menu: SelectDev
// upper: Main
// NOTE: The number of items in this menu object shall be
//       equal to or greater than MAX_NUM_BLE_CONNS
  MENU_OBJ(spMenuSelectConn, "Work with", MAX_NUM_BLE_CONNS, &spMenuMain)
  MENU_ITEM_MULTIPLE_ACTIONS(spMenuSelectConn, MAX_NUM_BLE_CONNS, NULL, SimplePeripheral_doSelectConn)
  MENU_OBJ_END

// Menu:  AutoConnect
// upper: Main
  MENU_OBJ(spMenuAutoConnect, "Set AutoConnect", 3, &spMenuMain)
  MENU_ITEM_ACTION(spMenuAutoConnect,0,"Disable", SimplePeripheral_doAutoConnect)
  MENU_ITEM_ACTION(spMenuAutoConnect,1,"Group A", SimplePeripheral_doAutoConnect)
  MENU_ITEM_ACTION(spMenuAutoConnect,2,"Group B", SimplePeripheral_doAutoConnect)
  MENU_OBJ_END

// Menu: PerConnection
// upper: SelectDevice
  MENU_OBJ(spMenuPerConn, NULL, 1, &spMenuSelectConn)
  MENU_ITEM_SUBMENU(spMenuPerConn,0,&spMenuConnPhy)
  MENU_OBJ_END

// Menu: ConnPhy
// upper: Select Device
  MENU_OBJ(spMenuConnPhy, "Set Conn PHY Preference", 6, &spMenuPerConn)
  MENU_ITEM_ACTION(spMenuConnPhy,0,"1 Mbps",              SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,1,"2 Mbps",              SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,2,"1 & 2 Mbps",          SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,3,"Coded",               SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,4,"1 & 2 Mbps, & Coded", SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,5,"Auto PHY change",     SimplePeripheral_doSetConnPhy)
  MENU_OBJ_END
}
