/***************************************************************************//**
 * @file
 * @brief Definitions for the Time Server plugin.
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

/**
 * @defgroup time-server Time Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Time Cluster Server Component
 *
 * Silicon Labs implementation of Time server cluster.  This component
 * is not recommended for production because it uses a software system
 * to keep track of time.  Instead, the user should hook
 * the cluster callbacks into a set of hardware routines to maintain the
 * device's system clock.  This software implementation is inappropriate
 * for sleepy devices since it prevents them from sleeping for
 * longer than one second. During initialization, the Time Server component
 * writes the TimeStatus attribute based on the component options,
 * overriding any attribute default that may have been configured.
 */

/**
 * @addtogroup time-server
 * @{
 */

/** @} */ // end of time-server

uint32_t emAfTimeClusterServerGetCurrentTime(void);
void emAfTimeClusterServerSetCurrentTime(uint32_t utcTime);
