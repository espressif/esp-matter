/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 */

/** @file "qorvo_internals.h"
 *
 */

#ifndef _QORVO_INTERNALS_H_
#define _QORVO_INTERNALS_H_

/*
 * Enabled components
 */


/*
 * Components numeric ids
 */

#define GP_COMPONENT_ID_APP                                1
#define GP_COMPONENT_ID_ASSERT                             29
#define GP_COMPONENT_ID_BSP                                8
#define GP_COMPONENT_ID_GPHAL                              7
#define GP_COMPONENT_ID_HALCORTEXM4                        6
#define GP_COMPONENT_ID_LOG                                11
#define GP_COMPONENT_ID_RESET                              33
#define GP_COMPONENT_ID_SCHED                              9
#define GP_COMPONENT_ID_STAT                               22
#define GP_COMPONENT_ID_UTILS                              4

/*
 * Component: gpUtils
 */

#define GP_UTILS_DIVERSITY_LINKED_LIST

/*
 * Component: mbedtls_alt_qpg6105
 */

#define MBEDTLS_CONFIG_FILE                                "qpg6105-mbedtls-config.h"

/*
 * Other flags
 */

#define GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS     0
#define GP_GIT_SHA                                         2469438626c13beea2391947d34e05a62039041a
#define GP_GIT_SHA_SHORT                                   2469438
#define GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS                 0

#endif //_QORVO_INTERNALS_H_
