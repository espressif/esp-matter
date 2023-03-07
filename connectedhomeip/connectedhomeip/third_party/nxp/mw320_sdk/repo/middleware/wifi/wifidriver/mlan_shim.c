/** @file mlan_shim.c
 *
 *  @brief  This file provides  APIs to MOAL module
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

/**
 *  @mainpage MLAN Driver
 *
 *  @section overview_sec Overview
 *
 *  The MLAN is an OS independent WLAN driver for NXP 802.11
 *  embedded chipset.
 */

/********************************************************
Change log:
    10/13/2008: initial version
********************************************************/
#include <mlan_api.h>

/* Additional WMSDK header files */
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>
/********************************************************
                Local Variables
********************************************************/

/********************************************************
        Global Variables
********************************************************/
mlan_operations mlan_sta_ops = {
    /* cmd handler */
    wlan_ops_sta_prepare_cmd,
    /* rx handler */
    wlan_ops_sta_process_rx_packet,
    /* BSS role: STA */
    MLAN_BSS_ROLE_STA,
};
mlan_operations mlan_uap_ops = {
    /* cmd handler */
    wlan_ops_uap_prepare_cmd,
    /* rx handler */
    /* wlan_ops_uap_process_rx_packet, */ NULL,
    /* BSS role: uAP */
    MLAN_BSS_ROLE_UAP,
};

/** mlan function table */
mlan_operations *mlan_ops[] = {
    &mlan_sta_ops,
    &mlan_uap_ops,
    MNULL,
};

/** Global moal_assert callback */
t_void (*assert_callback)(IN t_void *pmoal_handle, IN t_u32 cond) = MNULL;
#ifdef DEBUG_LEVEL1
#ifdef DEBUG_LEVEL2
#define DEFAULT_DEBUG_MASK (0xffffffff)
#else
#define DEFAULT_DEBUG_MASK (MMSG | MFATAL | MERROR)
#endif

/** Global moal_print callback */
t_void (*print_callback)(IN t_void *pmoal_handle, IN t_u32 level, IN t_s8 *pformat, IN...) = MNULL;

/** Global moal_get_system_time callback */
mlan_status (*get_sys_time_callback)(IN t_void *pmoal_handle, OUT t_u32 *psec, OUT t_u32 *pusec) = MNULL;

/** Global driver debug mit masks */
t_u32 drvdbg = DEFAULT_DEBUG_MASK;
#endif

/********************************************************
        Local Functions
*******************************************************/

/********************************************************
        Global Functions
********************************************************/

/**
 *  @brief This function registers MOAL to MLAN module.
 *
 *  @param pmdevice        A pointer to a mlan_device structure
 *                         allocated in MOAL
 *  @param ppmlan_adapter  A pointer to a t_void pointer to store
 *                         mlan_adapter structure pointer as the context
 *
 *  @return                MLAN_STATUS_SUCCESS
 *                             The registration succeeded.
 *                         MLAN_STATUS_FAILURE
 *                             The registration failed.
 *
 * mlan_status mlan_register (
 *   IN pmlan_device     pmdevice,
 *   OUT t_void          **ppmlan_adapter
 * );
 *
 * Comments
 *   MOAL constructs mlan_device data structure to pass moal_handle and
 *   mlan_callback table to MLAN. MLAN returns mlan_adapter pointer to
 *   the ppmlan_adapter buffer provided by MOAL.
 * Headers:
 *   declared in mlan_decl.h
 * See Also
 *   mlan_unregister
 */
mlan_status mlan_register(IN pmlan_device pmdevice, OUT t_void **ppmlan_adapter)
{
    mlan_status ret         = MLAN_STATUS_SUCCESS;
    pmlan_adapter pmadapter = MNULL;
    pmlan_callbacks pcb     = MNULL;
    t_u8 i                  = 0;
    t_u32 j                 = 0;

    MASSERT(pmdevice);
    MASSERT(ppmlan_adapter);
    /* MASSERT(pmdevice->callbacks.moal_print); */
#ifdef DEBUG_LEVEL1
    print_callback        = pmdevice->callbacks.moal_print;
    get_sys_time_callback = pmdevice->callbacks.moal_get_system_time;
#endif
    /* assert_callback = pmdevice->callbacks.moal_assert; */

    ENTER();

    MASSERT(pmdevice->callbacks.moal_malloc);
    /* MASSERT(pmdevice->callbacks.moal_memset); */
    /* MASSERT(pmdevice->callbacks.moal_memmove); */

    /* Allocate memory for adapter structure */
    if ((pmdevice->callbacks.moal_malloc(/* pmdevice->pmoal_handle */ NULL, sizeof(mlan_adapter), MLAN_MEM_DEF,
                                         (t_u8 **)&pmadapter) != MLAN_STATUS_SUCCESS) ||
        !pmadapter)
    {
        ret = MLAN_STATUS_FAILURE;
        goto exit_register;
    }

    memset(pmadapter, pmadapter, 0, sizeof(mlan_adapter));

    pcb = &pmadapter->callbacks;

    /* Save callback functions */
    memmove(pmadapter->pmoal_handle, pcb, &pmdevice->callbacks, sizeof(mlan_callbacks));

    pmadapter->priv_num = 0;
    for (i = 0; i < MLAN_MAX_BSS_NUM; i++)
    {
        pmadapter->priv[i] = MNULL;
        if (pmdevice->bss_attr[i].active == MTRUE)
        {
            /* For valid bss_attr, allocate memory for private structure */
            if ((pcb->moal_malloc(pmadapter->pmoal_handle, sizeof(mlan_private), MLAN_MEM_DEF,
                                  (t_u8 **)&pmadapter->priv[i]) != MLAN_STATUS_SUCCESS) ||
                !pmadapter->priv[i])
            {
                ret = MLAN_STATUS_FAILURE;
                goto error;
            }

            pmadapter->priv_num++;
            memset(pmadapter, pmadapter->priv[i], 0, sizeof(mlan_private));

            pmadapter->priv[i]->adapter = pmadapter;

            /* Save bss_type, frame_type & bss_priority */
            pmadapter->priv[i]->bss_type     = (t_u8)pmdevice->bss_attr[i].bss_type;
            pmadapter->priv[i]->frame_type   = (t_u8)pmdevice->bss_attr[i].frame_type;
            pmadapter->priv[i]->bss_priority = (t_u8)pmdevice->bss_attr[i].bss_priority;
            if (pmdevice->bss_attr[i].bss_type == MLAN_BSS_TYPE_STA)
                pmadapter->priv[i]->bss_role = MLAN_BSS_ROLE_STA;
            else if (pmdevice->bss_attr[i].bss_type == MLAN_BSS_TYPE_UAP)
                pmadapter->priv[i]->bss_role = MLAN_BSS_ROLE_UAP;
            /* Save bss_index and bss_num */
            pmadapter->priv[i]->bss_index = i;
            pmadapter->priv[i]->bss_num   = (t_u8)pmdevice->bss_attr[i].bss_num;

            /* init function table */
            for (j = 0; mlan_ops[j]; j++)
            {
                if (mlan_ops[j]->bss_role == GET_BSS_ROLE(pmadapter->priv[i]))
                {
                    memcpy(pmadapter, &pmadapter->priv[i]->ops, mlan_ops[j], sizeof(mlan_operations));
                }
            }
        }
    }

    /* Initialize lock variables */
    if (wlan_init_lock_list(pmadapter) != MLAN_STATUS_SUCCESS)
    {
        ret = MLAN_STATUS_FAILURE;
        goto error;
    }

    /* Allocate memory for member of adapter structure */
    if (wlan_allocate_adapter(pmadapter))
    {
        ret = MLAN_STATUS_FAILURE;
        goto error;
    }

    /* Return pointer of mlan_adapter to MOAL */
    *ppmlan_adapter = pmadapter;

    goto exit_register;

error:
    PRINTM(MINFO, "Leave mlan_register with error\n");
    pcb->moal_mfree(pmadapter->pmoal_handle, (t_u8 *)pmadapter);

exit_register:
    LEAVE();
    return ret;
}

/**
 *  @brief This function initializes the firmware
 *
 *  @param pmlan_adapter   A pointer to a t_void pointer to store
 *                         mlan_adapter structure pointer
 *
 *  @return                MLAN_STATUS_SUCCESS
 *                             The firmware initialization succeeded.
 *                         MLAN_STATUS_PENDING
 *                             The firmware initialization is pending.
 *                         MLAN_STATUS_FAILURE
 *                             The firmware initialization failed.
 */
mlan_status mlan_init_fw(IN t_void *pmlan_adapter)
{
    mlan_status ret         = MLAN_STATUS_SUCCESS;
    mlan_adapter *pmadapter = (mlan_adapter *)pmlan_adapter;

    ENTER();
    MASSERT(pmlan_adapter);

    /* pmadapter->hw_status = WlanHardwareStatusInitializing; */

    /* Initialize firmware, may return PENDING */
    ret = wlan_init_fw(pmadapter);
    PRINTM(MINFO, "wlan_init_fw returned ret=0x%x\n", ret);

    LEAVE();
    return ret;
}
