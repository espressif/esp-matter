/*
 * Copyright (c) 2018, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPHAL_KX_RAP_H_
#define _GPHAL_KX_RAP_H_

#define rap_h_wait_us HAL_WAIT_US
#include "gpHal_kx_Rap_constants.h"
#include "gpBsp.h"

#define rap_h_assert_dev(x)  GP_ASSERT_DEV_INT(x)
#define rap_h_assert_sys(x)  GP_ASSERT_SYSTEM(x)

//void rap_radio_init(void);
//void rap_radio_override_defaults(void);
void rap_sys_hal_init(void);
void rap_sys_reset_early(void);
void rap_sys_backup_late(void);
void rap_bbpll_resume(void);
void rap_bbpll_start(void);
void rap_bbpll_stop(void);
void rap_rx_zb_set_mode(UInt8 mode);
void rap_tx_cw_enable(UInt8 chidx, UInt8 power, Bool pa_low, Bool pa_ultralow, Bool antenna, Bool modulated, Bool tx);
void rap_tx_cw_disable(void);
void rap_tx_force_antenna(Bool antenna);
void rap_tx_unforce_antenna(void);
void rap_fll_set_channel(UInt8 idx, UInt8 fll_channel);
// Manual!!! (currently no .psc files availanble for dsfix)
void rap_dsfix_cal_desense_ch(UInt8 *channel);
void rap_dsfix_setup(UInt32 dsfix_mem_ptr);

#endif //_GPHAL_KX_RAP_H_
