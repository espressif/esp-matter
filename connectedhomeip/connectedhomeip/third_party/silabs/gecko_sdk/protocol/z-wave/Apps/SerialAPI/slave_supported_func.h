/**
 * @file slave_supported_func.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

/* Generated file copied from Sdk670 with std. functions supported by static controller. */
/* Following defines has been disable for build purposes:
 * SUPPORT_ZW_AES_ECB
 * SUPPORT_ZW_SET_EXT_INT_LEVEL
 */
#define SUPPORT_GET_ROUTING_TABLE_LINE                  0 /* ZW_GetRoutingInfo */
#define SUPPORT_LOCK_ROUTE_RESPONSE                     1 /* ZW_LockRoute */
#define SUPPORT_MEMORY_GET_BUFFER                       1 /* MemoryGetBuffer */
#define SUPPORT_MEMORY_GET_BYTE                         1 /* MemoryGetByte */
#define SUPPORT_MEMORY_GET_ID                           1 /* MemoryGetID */
#define SUPPORT_MEMORY_PUT_BUFFER                       1 /* MemoryPutBuffer */
#define SUPPORT_MEMORY_PUT_BYTE                         1 /* MemoryPutByte */
#define SUPPORT_NVM_GET_ID                              1 /* NVM_get_id */
#define SUPPORT_NVM_EXT_READ_LONG_BUFFER                1 /* NVM_ext_read_long_buffer */
#define SUPPORT_NVM_EXT_READ_LONG_BYTE                  1 /* NVM_ext_read_long_byte */
#define SUPPORT_NVM_EXT_WRITE_LONG_BYTE                 0 /* NVM_ext_write_long_byte */
#define SUPPORT_NVM_EXT_WRITE_LONG_BUFFER               0 /* NVM_ext_write_long_buffer*/
#define SUPPORT_NVM_BACKUP_RESTORE                      0 /* MemoryGetBuffer */
#define SUPPORT_PWR_CLK_PD                              0 /* PWR_Clk_PD */
#define SUPPORT_PWR_CLK_PUP                             0 /* PWR_Clk_PUp */
#define SUPPORT_PWR_SELECT_CLK                          0 /* PWR_Select_Clk */
#define SUPPORT_PWR_SETSTOPMODE                         0 /* PWR_SetStopMode */
#define SUPPORT_STORE_HOMEID                            0 /* ZW_StoreHomeID */
#define SUPPORT_STORE_NODEINFO                          0 /* notUsed */
#define SUPPORT_ZW_ADD_NODE_TO_NETWORK                  0 /* ZW_AddNodeToNetwork */
#define SUPPORT_ZW_AES_ECB                              0 /* ZW_AES_ECB */
#define SUPPORT_ZW_ARE_NODES_NEIGHBOURS                 0 /* notUsed */
#define SUPPORT_ZW_ASSIGN_RETURN_ROUTE                  0 /* ZW_AssignReturnRoute */
#define SUPPORT_ZW_ASSIGN_PRIORITY_RETURN_ROUTE         0 /* ZW_AssignPriorityReturnRoute */
#define SUPPORT_ZW_ASSIGN_SUC_RETURN_ROUTE              0 /* ZW_AssignSUCReturnRoute */
#define SUPPORT_ZW_ASSIGN_PRIORITY_SUC_RETURN_ROUTE     0 /* ZW_AssignPrioritySUCReturnRoute */
#define SUPPORT_ZW_CONTROLLER_CHANGE                    0 /* ZW_ControllerChange */
#define SUPPORT_ZW_CREATE_NEW_PRIMARY                   0 /* NotUsed */
#define SUPPORT_ZW_DELETE_RETURN_ROUTE                  0 /* ZW_DeleteReturnRoute */
#define SUPPORT_ZW_DELETE_SUC_RETURN_ROUTE              0 /* ZW_DeleteSUCReturnRoute */
#define SUPPORT_ZW_ENABLE_SUC                           0 /* ZW_EnableSUC */
#define SUPPORT_ZW_EXPLORE_REQUEST_INCLUSION            1 /* ZW_ExploreRequestInclusion */
#define SUPPORT_ZW_EXPLORE_REQUEST_EXCLUSION            1 /* ZW_ExploreRequestExclusion */
#define SUPPORT_ZW_GET_CONTROLLER_CAPABILITIES          0 /* ZW_GetControllerCapabilities */
#define SUPPORT_ZW_GET_PRIORITY_ROUTE                   0 /* ZW_GetPriorityRoute */
#define SUPPORT_ZW_SET_PRIORITY_ROUTE                   0 /* ZW_SetPriorityRoute */
#define SUPPORT_ZW_GET_NEIGHBOR_COUNT                   0 /*notUsed */
#define SUPPORT_ZW_GET_NODE_PROTOCOL_INFO               1 /* ZW_GetNodeProtocolInfo */
#define SUPPORT_ZW_GET_PROTOCOL_STATUS                  1 /* ZW_GetProtocolStatus */
#define SUPPORT_ZW_GET_PROTOCOL_VERSION                 1 /* ZW_GetProtocolVersion */
#define SUPPORT_ZW_GET_RANDOM                           1 /* ZW_GetRandomWord */
#define SUPPORT_ZW_GET_ROUTING_MAX                      0 /* ZW_GetRoutingMAX */
#define SUPPORT_ZW_GET_SUC_NODE_ID                      1 /* ZW_GetSUCNodeID */
#define SUPPORT_ZW_GET_VERSION                          1 /* ZW_Version */
#define SUPPORT_ZW_IS_FAILED_NODE_ID                    0 /* ZW_isFailedNode */
#define SUPPORT_ZW_IS_NODE_WITHIN_DIRECT_RANGE          0 /* NotUsed */
#define SUPPORT_ZW_IS_PRIMARY_CTRL                      0 /* NotUsed */
#define SUPPORT_ZW_IS_WUT_KICKED                        0 /* ZW_IsWutKicked */
#define SUPPORT_ZW_NEW_CONTROLLER                       0 /* ZW_NewController */
#define SUPPORT_ZW_RANDOM                               0 /* NotUsed */
#define SUPPORT_ZW_REDISCOVERY_NEEDED                   0 /* ZW_RediscoveryNeeded */
#define SUPPORT_ZW_REMOVE_FAILED_NODE_ID                0 /* ZW_RemoveFailedNode */
#define SUPPORT_ZW_REMOVE_NODE_FROM_NETWORK             0 /* ZW_RemoveNodeFromNetwork */
#define SUPPORT_ZW_REMOVE_NODE_ID_FROM_NETWORK          0 /* ZW_RemoveNodeIDFromNetwork */
#define SUPPORT_ZW_REPLACE_FAILED_NODE                  0 /* ZW_ReplaceFailedNode */
#define SUPPORT_ZW_REPLICATION_COMMAND_COMPLETE         0 /* ZW_ReplicationReceiveComplete */
#define SUPPORT_ZW_REPLICATION_SEND_DATA                0 /* ZW_ReplicationSend */
#define SUPPORT_ZW_REQUEST_NETWORK_UPDATE               1 /* ZW_RequestNetWorkUpdate */
#define SUPPORT_ZW_REQUEST_NEW_ROUTE_DESTINATIONS       0 /* NotUsed */
#define SUPPORT_ZW_REQUEST_NODE_INFO                    0 /* ZW_RequestNodeInfo */
#define SUPPORT_ZW_REQUEST_NODE_NEIGHBOR_UPDATE         0 /* ZW_RequestNodeNeighborUpdate */
#define SUPPORT_ZW_RF_POWER_LEVEL_GET                   0 /* ZW_RFPowerLevelGet */
#define SUPPORT_ZW_RF_POWER_LEVEL_REDISCOVERY_SET       0 /* NotUsed */
#define SUPPORT_ZW_RF_POWER_LEVEL_SET                   0 /* ZW_RFPowerLevelSet */
#define SUPPORT_ZW_SEND_DATA                            1 /* ZW_SendData */
#define SUPPORT_ZW_SEND_DATA_EX                         1 /* ZW_SendDataEx */
#define SUPPORT_ZW_SEND_DATA_ABORT                      1 /* ZW_SendDataAbort */
#define SUPPORT_ZW_SEND_DATA_META                       0 /* ZW_SendDataMeta */
#define SUPPORT_ZW_SEND_DATA_META_BRIDGE                0 /* ZW_SendDataMeta_Bridge */
#define SUPPORT_ZW_SEND_DATA_META_MR                    0 /* NotUsed */
#define SUPPORT_ZW_SEND_DATA_MR                         0 /* NotUsed */
#define SUPPORT_ZW_SEND_DATA_MULTI                      1 /* ZW_SendDataMulti */
#define SUPPORT_ZW_SEND_DATA_MULTI_EX                   1 /* ZW_SendDataMultiEx */
#define SUPPORT_ZW_SET_NETWORK_KEY_S0                   0 /* ZW_SetNetworkKeyS0 */
#define SUPPORT_ZW_GET_SECURITY_KEYS                    1 /* ZW_GetSecurityKeys */
#define SUPPORT_ZW_SEND_NODE_INFORMATION                1 /* ZW_SendNodeInformation */
#define SUPPORT_ZW_SEND_SLAVE_DATA                      0 /* ZW_SendSlaveData */
#define SUPPORT_ZW_SEND_SUC_ID                          0 /* ZW_SendSUCID */
#define SUPPORT_ZW_SEND_TEST_FRAME                      0 /* ZW_SendTestFrame */
#define SUPPORT_ZW_SET_DEFAULT                          1 /* ZW_SetDefault */
#define SUPPORT_ZW_SET_EXT_INT_LEVEL                    0 /* ZW_SetExtIntLevel */
#define SUPPORT_ZW_SET_LEARN_MODE                       1 /* ZW_SetLearnMode */
#define SUPPORT_ZW_SET_LEARN_NODE_STATE                 0 /* ZW_SetLearnNodeState */
#define SUPPORT_ZW_SET_PROMISCUOUS_MODE                 0 /* ZW_SetPromiscuousMode */
#define SUPPORT_ZW_SET_RF_RECEIVE_MODE                  1 /* ZW_SetRFReceiveMode */
#define SUPPORT_ZW_SET_ROUTING_INFO                     0 /* NotUsed */
#define SUPPORT_ZW_SET_ROUTING_MAX                      0 /*notUsed*/
#define SUPPORT_ZW_SET_SLEEP_MODE                       0 /* ZW_SetSleepMode */
#define SUPPORT_ZW_SET_SUC_NODE_ID                      1 /* ZW_SetSUCNodeID */
#define SUPPORT_ZW_SET_WUT_TIMEOUT                      0 /* ZW_SetWutTimeout */
#define SUPPORT_ZW_SUPPORT9600_ONLY                     0 /* ZW_Support9600Only */
#define SUPPORT_ZW_TYPE_LIBRARY                         1 /* ZW_Type_Library */
#define SUPPORT_ZW_WATCHDOG_DISABLE                     0 /* ZW_WatchDogDisable */
#define SUPPORT_ZW_WATCHDOG_ENABLE                      0 /* ZW_WatchDogEnable */
#define SUPPORT_ZW_WATCHDOG_KICK                        0 /* ZW_WatchDogKick */
#define SUPPORT_ZW_WATCHDOG_START                       1 /* ZW_WatchDogEnable */
#define SUPPORT_ZW_WATCHDOG_STOP                        1 /* ZW_WatchDogDisable */
#define SUPPORT_ZW_NVR_GET_VALUE                        0 /* ZW_NVRGetValue */
#define SUPPORT_FUNC_ID_CLEAR_TX_TIMERS                 1 /* ZW_ClearTxTimers */
#define SUPPORT_FUNC_ID_GET_TX_TIMERS                   1 /* ZW_GetTxTimer */
#define SUPPORT_ZW_CLEAR_NETWORK_STATS                  1 /* ZW_ClearNetworkStats */
#define SUPPORT_ZW_GET_NETWORK_STATS                    1 /* ZW_GetNetworkStats */
#define SUPPORT_ZW_GET_BACKGROUND_RSSI                  1 /* ZW_GetBackgroundRSSI */
/* SerialAPI functionality support definitions */
#define SUPPORT_SEND_DATA_TIMING                        1
/* Enable support for SerialAPI Startup Notification */
#define SUPPORT_SERIAL_API_STARTUP_NOTIFICATION         1

/* Common SerialAPI functionality support definitions */
#define SUPPORT_SERIAL_API_GET_INIT_DATA                1
#define SUPPORT_SERIAL_API_APPL_NODE_INFORMATION        1

#define SUPPORT_APPLICATION_COMMAND_HANDLER_BRIDGE      0
#define SUPPORT_APPLICATION_COMMAND_HANDLER             1


#define SUPPORT_SERIAL_API_SET_TIMEOUTS                 1

#define SUPPORT_SERIAL_API_SOFT_RESET                   1
#define SUPPORT_SERIAL_API_SETUP                        1
#define SUPPORT_ZW_INITIATE_SHUTDOWN                    0
#define SUPPORT_SERIAL_API_GET_LR_NODES                 0
#define SUPPORT_SERIAL_GET_LR_CHANNEL                   0
#define SUPPORT_SERIAL_SET_LR_CHANNEL                   0
#define SUPPORT_SERIAL_SET_LR_VIRTUAL_IDS               0
#define SUPPORT_SERIAL_ENABLE_RADIO_PTI                 0

/* ZW_EnableSUC() no longer exists in the library */

/* */
#define SUPPORT_SERIAL_API_GET_APPL_HOST_MEMORY_OFFSET  0

#define SUPPORT_ZW_SET_LISTEN_BEFORE_TALK_THRESHOLD     1

#define SUPPORT_ZW_NETWORK_MANAGEMENT_SET_MAX_INCLUSION_REQUEST_INTERVALS     1

/**************************************************************************/
/* Common for all slaves */
/* SerialAPI functionality support definitions */
#define SUPPORT_ZW_APPLICATION_UPDATE                   1
#define SUPPORT_APPLICATION_RF_NOTIFY                   0

#define SUPPORT_ZW_GET_VIRTUAL_NODES                    0 /* ZW_GetVirtualNodes */
#define SUPPORT_ZW_IS_VIRTUAL_NODE                      0 /* ZW_IsVirtualNode */
#define SUPPORT_ZW_SEND_DATA_BRIDGE                     0 /* ZW_SendData_Bridge */
#define SUPPORT_ZW_SEND_DATA_MULTI_BRIDGE               0 /* ZW_SendDataMulti_Bridge */
#define SUPPORT_ZW_SET_SLAVE_LEARN_MODE                 0 /* ZW_SetSlaveLearnMode */
#define SUPPORT_SERIAL_API_APPL_SLAVE_NODE_INFORMATION  0
/**************************************************************************/
/* Slave enhanced */
/* Specific SerialAPI functionality support definitions */

#define SUPPORT_ZW_APPLICATION_CONTROLLER_UPDATE        0

#define SUPPORT_GET_TX_COUNTER                          0
#define SUPPORT_RESET_TX_COUNTER                        0

#define SUPPORT_SERIAL_API_APPL_SLAVE_NODE_INFORMATION  0
#define SUPPORT_APPLICATION_SLAVE_COMMAND_HANDLER       0
#define SUPPORT_ZW_SEND_SLAVE_NODE_INFORMATION          0


#define SUPPORT_SERIAL_API_TEST                         0
#define SUPPORT_ZW_APPLICATION_CONTROLLER_UPDATE        0

#define SUPPORT_GET_TX_COUNTER                          0
#define SUPPORT_RESET_TX_COUNTER                        0

#define SUPPORT_SERIAL_API_APPL_SLAVE_NODE_INFORMATION  0
#define SUPPORT_APPLICATION_SLAVE_COMMAND_HANDLER       0
#define SUPPORT_ZW_SEND_SLAVE_NODE_INFORMATION          0
