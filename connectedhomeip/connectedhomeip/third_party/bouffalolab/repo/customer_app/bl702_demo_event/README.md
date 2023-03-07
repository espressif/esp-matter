# Build Script Description
```html
genblehogp:        BLE as slave, enable HOGP service.
genblem0s1:        1 BLE connection is supported, BL702 can only be slave in this connection.
genblem0s1p:       based on genblem0s1, add BLE PDS(power down sleep) feature.
genblem0s1s:       based on genblem0s1, add BLE scan feature.
genblem16s1:       16 BLE connections are suppprted, BL702 can be master or slave in each connection. Please modify __EM_SIZE to 16K in ld file. For example,__EM_SIZE = DEFINED(ble_controller_init) ? 16K : 0K; in components\platform\soc\bl702\bl702\evb\ld\flash.ld
genblemesh:        build mesh application without meshmodel code.
genblemeshmodel:   build mesh application with meshmodel code.
genblestd：        2 BLE connections are suppprted, BL702 can be master or slave in each connection.
genbleperipheral:  support the BLE connection as a slave role, without the need to manually enter CLI commands.
geneth:            build enthernet application.
genflashmap:       build Zigbee and BLE. BLE support all roles, 2 BLE connection is supported, enable tp and OAD service.
genromap:          Used to generate sdk.
genzb:             build generic Zigbee image; after boot, if device haven't join a network, user need to use the "zb_set_role" CLI command to set device type, use the "zb_register_dev" CLI command to register endpoint and ZCL clusters, then use the "zb_start" or "zb_form" CLI command to join/form a network; if device already in a network, user need to use the "zb_register_dev" CLI command to register endpoint and ZCL clusters, then use the "zb_start" CLI command to restore network.
genzcstartup:      build Zigbee Coordinator image; after boot, if haven't form a network, user can either use use the "zb_form" CLI command to form a nwk manually by specifying channel and panId, or use the "zb_start" CLI command to let stack select the channel and panId, if already in a network, restore network and resume opertion.
genzcstartuppsram: build Zigbee Coordinator image with psram support; It makes C++ new/detele mallo/release from psram, not sram after system_init. After boot, if haven't form a network, user can either use use the "zb_form" CLI command to form a nwk manually by specifying channel and panId, or use the "zb_start" CLI command to let stack select the channel and panId, if already in a network, restore network and resume opertion.
genzrstartup:      build Zigbee Router image; after boot, device will scan and join a network automatically if haven't join a network, otherwise, restore network and resume operation.
genznsedstartup:   build Zigbee Non-Sleepy End Device image; after boot, device will scan and join a network automatically if haven't join a network, otherwise, restore network and resume operation.
genzsedstartupwithhbn:          build Zigbee Sleepy End Device image; after boot, device will scan and join a network automatically if haven't join a network, otherwise, restore network and resume operation. If device is not active in a zigbee network, it enters into pds31 when it can sleep. If device is active in a zigbee network,it enters into hbn0 when it can sleep.
genzsedstartupwithpds:          build Zigbee Sleepy End Device image; after boot, device will scan and join a network automatically if haven't join a network, otherwise, restore network and resume operation. Device enters into pds31 when it can sleep.
genzsedstartupwithpds3:         build Zigbee Sleepy End Device image; after boot, device will scan and join a network automatically if haven't join a network, otherwise, restore network and resume operation. Device enters into pds3 when it can sleep.
genzsedstartupblescanwithpds3:  build image that supports both Zigbee Sleepy End Device and ble observer. After boot,Device will do zigbee scan and join a zigbee network automatically if haven't join a zigbee network, otherwise, restore network and resume operation. At the meantime, device will do ble scan. Device enters into pds3 when it can sleep.

#Common Flags
CONFIG_PDS_ENABLE: To make device be able to enter into Power Dwon Sleep mode.
CONFIG_PDS_LEVEL: To define the level of power down sleep mode that the device enters in.
CONFIG_HBN_ENABLE: To make device be able to enter into Hibernate Sleep mode.
CONFIG_USE_PSRAM: To support PSRAM. Make sure psram exists if this flag is enabled.
CONFIG_USB_CDC: To enable USB.
CONFIG_SYS_VFS_ENABLE: To enable virtual file system.
CONFIG_SYS_VFS_UART_ENABLE: To operate uart as a virtual file.
CONFIG_SYS_AOS_CLI_ENABLE: To enable cli module in sdk.
CONFIG_SYS_AOS_LOOP_ENABLE: To enable yloop in sdk.

#Bluetooth Flags
CONFIG_BT: To enable bluetooth in sdk.
CONFIG_BLE_TP_SERVER: To enable ble throughput service.
CONFIG_HOGP_SERVER: To enable ble HID over GATT Profile service.
CONFIG_BT_BAS_SERVER: To enable ble Battery Service.
CONFIG_BT_DIS_SERVER: To enable ble Device Information Service.
CONFIG_BT_SCPS_SERVER: To enable ble Scan Parameters Service.
CONFIG_BT_OAD_SERVER: To enable ble ota service.
CONFIG_BLECONTROLLER_LIB: To build specific ble controller library.
CONFIG_BT_ALLROLES: To enable all ble device roles.
CONFIG_BT_CENTRAL: To enable ble Central role.
CONFIG_BT_PERIPHERAL: To enable ble Peripheral role. 
CONFIG_BT_OBSERVER: To enable ble Observer role. 
CONFIG_BT_BROADCASTER: To enable ble Broadcaster role.
CONFIG_BT_CONN: To set the maximal number of ble connetion. If CONFIG_BT_CONN is 0, connection is not supported.
CONFIG_DISABLE_BT_SMP: To disable ble SMP(Security Manager Protocol) feature.
CONFIG_DISABLE_BT_HOST_PRIVACY: To disable ble privacy feature from host side.
CONFIG_BLE_MULTI_ADV：To enable ble muti-adv.
CONFIG_BT_SETTINGS: To store the information required by bluetooth or mesh to flash. For example, paired information, mesh network information and so on.
CONFIG_BLE_PERIPHERAL_AUTORUN : To enable ble Peripheral role and automatically runs the BLE connection and BLE SMP(Security Manager Protocol) process.

#Bluetooth Mesh Flags
CONFIG_BT_MESH: To enable ble MESH.
CONFIG_BT_MESH_MODEL: To enable ble Mesh Model Layer.
CONFIG_BT_MESH_PROVISIONER: To enable mesh provisioner.

#Zigbee Flags
CONFIG_ZIGBEE: To enable zigbee in sdk.
CONFIG_ZBSTACK_LIB: If define CONFIG_ZBSTACK_LIB=gpp, means to support GreenPower Proxy function.
CONFIG_FLASH_SIZE: To make sure the value of CONFIG_FLASH_SIZE is the same with the flash size used in partition table.
CONFIG_MANUFACTURER_ID: To define manufacturer id.
CONFIG_ZIGBEE_COORDINATOR_STARTUP: To make zigbee coordinator restore network and resume opertion if already in a network after boot.If haven't form a network, user can either use the "zb_form" CLI command to form a nwk manually by specifying channel and panId, or use the "zb_start" CLI command to let stack select the channel and panId.
CONFIG_ZIGBEE_ROUTER_STARTUP: To make zigbee router device scan and join a network automatically after boot if haven't join a network, otherwise, restore network and resume operation. 
CONFIG_NONSLEEPY_ZIGBEE_END_DEVICE_STARTUP: To make zigbee non sleepy end device scan and join a network automatically after boot if it hasn't joined a network, otherwise, restore network and resume operation. 
CONFIG_ZIGBEE_SLEEPY_END_DEVICE_STARTUP: To make zigbee sleepy end device scan and join a network automatically after boot if it hasn't joined a network, otherwise, restore network and resume operation. 
CONFIG_ZIGBEE_CLI: To enable zigbee cli commands.

#Ethernet Flags
CONFIG_ETHERNET: To enable Ethernet.
```
# Notice
For BouffaloLabDevCube version <= 1.5.3, please use partition table files (partition_cfg_*.toml) under this folder.
