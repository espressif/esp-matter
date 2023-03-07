/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SILABS_MULTI_MAC_H
#define SILABS_MULTI_MAC_H

#if defined(PHY_SIMULATION_DUAL) || defined(PHY_DUAL)
#define MAC_DUAL_PRESENT
#endif  //defined(PHY_SIMULATION_DUAL) || defined(PHY_DUAL)

#if defined(MAC_DUAL_PRESENT) || defined(ALT_MAC)
#define MULTI_MAC_PRESENT
#define MAC_INDEX_PARAMETER_SOLO uint8_t mac_index
#define MAC_INDEX_PARAMETER uint8_t mac_index,
#define MAC_INDEX_ARGUMENT_VALUE mac_index
#define MAC_INDEX_ARGUMENT mac_index,
#define MAC_INDEX_ARGUMENT_SOLO mac_index
#ifdef MAC_TEST_COMMANDS_SUPPORT
// Few mac certification test cases require association on subghz interface
// on dual phy. It is under MAC_TEST_COMMANDS_SUPPORT since it is not real
// customer usecase.
extern uint8_t emAssociationMacIndex; // defined in app/test/mac-test-commands.c
#define MAC_INDEX_ARGUMENT_TEST emAssociationMacIndex,
#define MAC_INDEX_ARGUMENT_TEST_SOLO emAssociationMacIndex
#else // !MAC_TEST_COMMANDS_SUPPORT
#define MAC_INDEX_ARGUMENT_TEST 0,
#define MAC_INDEX_ARGUMENT_TEST_SOLO 0
#endif  //MAC_TEST_COMMANDS_SUPPORT
#else // !(defined(MAC_DUAL_PRESENT) || defined(ALT_MAC))
#define MAC_INDEX_PARAMETER_SOLO void
#define MAC_INDEX_PARAMETER
#define MAC_INDEX_ARGUMENT
#define MAC_INDEX_ARGUMENT_VALUE 0
#define MAC_INDEX_ARGUMENT_SOLO
#define MAC_INDEX_ARGUMENT_TEST
#define MAC_INDEX_ARGUMENT_TEST_SOLO
#endif  // defined(MAC_DUAL_PRESENT) || defined(ALT_MAC)

#if (PHY_PRO2PLUS || PHY_EFR32GB || PHY_RAILGB \
     || defined(PHY_TRANSCEIVER_SIM) || defined(PHY_SIMULATION_GB))
#define SINGLE_PHY_MULTIPAGE_SUPPORT 1
#else
#define SINGLE_PHY_MULTIPAGE_SUPPORT 0
#endif

#if (defined(MAC_DUAL_PRESENT) || SINGLE_PHY_MULTIPAGE_SUPPORT)
#define ACCOUNT_FOR_ACK_DC 1
#else
#define ACCOUNT_FOR_ACK_DC 0
#endif

#endif  // SILABS_MULTI_MAC_H
