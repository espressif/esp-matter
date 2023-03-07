/***************************************************************************//**
 * @file
 * @brief Header for CPC Host functions
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SILABS_CPC_HOST_H
#define SILABS_CPC_HOST_H

extern bool ncpSleepEnabled;

#define CPCD_INSTANCE_LEN           16

// TODO michael not sure if I need these right now
// Bits in traceFlags to enable various host trace outputs
#define TRACE_FRAMES_BASIC        1   /*!< frames sent and received */
#define TRACE_FRAMES_VERBOSE      2   /*!< basic frames + internal variables */
#define TRACE_EVENTS              4   /*!< events */
#define TRACE_EZSP                8   /*!< EZSP commands, responses and callbacks */
#define TRACE_EZSP_VERBOSE        16  /*!< additional EZSP information */

#endif // SILABS_CPC_HOST_H
