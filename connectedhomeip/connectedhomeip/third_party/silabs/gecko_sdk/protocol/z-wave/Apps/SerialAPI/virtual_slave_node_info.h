/**
 * @file virtual_slave_node_info.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef VIRTUAL_SLAVE_NODE_INFO_H_
#define VIRTUAL_SLAVE_NODE_INFO_H_

#include <ZW_application_transport_interface.h>
#include <SizeOf.h>
#include "serialappl.h"

#ifdef ZW_CONTROLLER_BRIDGE
// Virtual slave node info storage
// array size is number of simultaneous virtual slaves supported

#define MAX_VIRTUAL_SLAVE_NODES (1)

extern uint8_t aVirtualSlaveCommandClasses[MAX_VIRTUAL_SLAVE_NODES][APPL_SLAVENODEPARM_MAX];
extern SVirtualSlaveNodeInfo_t aVirtualSlaveNodeInfo[MAX_VIRTUAL_SLAVE_NODES];
extern SVirtualSlaveNodeInfo_t *apVirtualSlaveNodeInfo[sizeof_array(aVirtualSlaveNodeInfo)];
extern const SVirtualSlaveNodeInfoTable_t VirtualSlaveNodeInfoTable;

#endif // ifdef ZW_CONTROLLER_BRIDGE

#endif /* VIRTUAL_SLAVE_NODE_INFO_H_ */
