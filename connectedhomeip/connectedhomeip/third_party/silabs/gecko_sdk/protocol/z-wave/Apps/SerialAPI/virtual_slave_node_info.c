/**
 * @file virtual_slave_node_info.c
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include "virtual_slave_node_info.h"

#ifdef ZW_CONTROLLER_BRIDGE

uint8_t aVirtualSlaveCommandClasses[MAX_VIRTUAL_SLAVE_NODES][APPL_SLAVENODEPARM_MAX];

SVirtualSlaveNodeInfo_t aVirtualSlaveNodeInfo[MAX_VIRTUAL_SLAVE_NODES] = {{.bListening = true,
                                                                           .NodeType.generic = GENERIC_TYPE_SWITCH_MULTILEVEL,
                                                                           .NodeType.specific = SPECIFIC_TYPE_NOT_USED,
                                                                           .CommandClasses.iListLength = 0,
                                                                           .CommandClasses.pCommandClasses = &aVirtualSlaveCommandClasses[0][0]}};
SVirtualSlaveNodeInfo_t *apVirtualSlaveNodeInfo[sizeof_array(aVirtualSlaveNodeInfo)] = {&aVirtualSlaveNodeInfo[0]};

const SVirtualSlaveNodeInfoTable_t VirtualSlaveNodeInfoTable =
{
  .iTableLength = sizeof_array(apVirtualSlaveNodeInfo),
  .ppNodeInfo = (const SVirtualSlaveNodeInfo_t **)apVirtualSlaveNodeInfo
};

#endif // ifdef ZW_CONTROLLER_BRIDGE
