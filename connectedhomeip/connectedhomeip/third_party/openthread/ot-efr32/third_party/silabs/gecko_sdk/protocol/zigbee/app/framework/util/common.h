/***************************************************************************//**
 * @file
 * @brief This file contains the includes that are common to all clusters
 * in the util.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SILABS_COMMON_H
#define SILABS_COMMON_H

// App framework
#include "app/framework/include/af.h"
#include "app/framework/util/util.h"
#include "app/framework/util/time-util.h"
#include "app/framework/util/attribute-table.h"
#include "app/framework/util/attribute-storage.h"
#ifdef UC_BUILD
#include "sl_endianness.h"
#endif // UC_BUILD

// the variables used to setup and send responses to cluster messages
extern EmberApsFrame emberAfResponseApsFrame;
extern uint8_t  appResponseData[EMBER_AF_RESPONSE_BUFFER_LEN];
extern uint16_t appResponseLength;
extern EmberNodeId emberAfResponseDestination;

#endif //SILABS_COMMON_H
