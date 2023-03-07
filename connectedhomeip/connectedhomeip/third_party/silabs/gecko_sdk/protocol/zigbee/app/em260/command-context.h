/***************************************************************************//**
 * @file
 * @brief Declarations of command context structs.
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

#ifndef SILABS_COMMAND_CONTEXT_H
#define SILABS_COMMAND_CONTEXT_H

#include "app/util/ezsp/ezsp-enum.h"

typedef struct {
  bool consumed;
  EzspStatus status;

  union {
    EzspValueId valueId;
    EzspExtendedValueId extendedValueId;
  };

  uint32_t characteristics;

  uint8_t valueLength;
  uint8_t value[128];
} EmberAfPluginEzspValueCommandContext;

typedef struct {
  bool consumed;

  EzspPolicyId policyId;
  EzspStatus status;
  EzspDecisionId decisionId;
} EmberAfPluginEzspPolicyCommandContext;

typedef struct {
  bool consumed;
  EzspConfigId configId;
  EzspStatus status;
  uint16_t value;
} EmberAfPluginEzspConfigurationValueCommandContext;

#endif /* __COMMAND_CONTEXT_H__ */
