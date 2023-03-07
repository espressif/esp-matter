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

/** @file "qorvo_config.h"
 *
 */

#ifndef _QORVO_CONFIG_H_
#define _QORVO_CONFIG_H_

/*
 * Version info
 */

#define GP_CHANGELIST                                      0
#define GP_VERSIONINFO_APP                                 UMB_QPG6105DK_B01_nrt_flash_compr_armgccemb
#define GP_VERSIONINFO_BASE_COMPS                          0,0,0,0
#define GP_VERSIONINFO_DATE                                2022-12-07
#define GP_VERSIONINFO_GLOBAL_VERSION                      0,0,0,0
#define GP_VERSIONINFO_HOST                                UNKNOWN
#define GP_VERSIONINFO_PROJECT                             Components/Qorvo/Bootloader
#define GP_VERSIONINFO_USER                                UNKNOWN@UNKNOWN


/*
 * Component: gpAssert
 */

/* Choose exit as default assert action */
#define GP_DIVERSITY_ASSERT_ACTION_EXIT

/* Choose to do nothing as default assert reporting */
#define GP_DIVERSITY_ASSERT_REPORTING_NOTHING


/*
 * Component: gpBsp
 */

/* Contains filename of BSP header file to include */
#define GP_BSP_FILENAME                                    "gpBsp_QPG6105DK_B01.h"


/*
 * Component: gpSched
 */

/* Callback after every main loop iteration. */
#define GP_SCHED_NR_OF_IDLE_CALLBACKS                      0


/*
 * Component: gpUpgrade
 */

/* Don't use gpExtStorage, even if available */
#define GP_UPGRADE_DIVERSITY_USE_INTSTORAGE


/*
 * Component: halCortexM4
 */

/* set custom stack size */
#define GP_KX_STACK_SIZE                                   30720


#include "qorvo_internals.h"

#endif //_QORVO_CONFIG_H_
