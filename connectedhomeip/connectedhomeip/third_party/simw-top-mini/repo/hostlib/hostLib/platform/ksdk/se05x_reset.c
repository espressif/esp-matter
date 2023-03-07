/* Copyright 2018,2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include "se05x_apis.h"
#include "ax_reset.h"
#include "se_reset_config.h"
#include <stdio.h>

#include "fsl_gpio.h"
#include "sm_timer.h"
#include "sm_types.h"
#include "smComT1oI2C.h"
#include "nxLog_smCom.h"

#if defined(SSS_USE_FTR_FILE)
#include "fsl_sss_ftr.h"
#else
#include "fsl_sss_ftr_default.h"
#endif

#if SSS_HAVE_SE05X || SSS_HAVE_LOOPBACK

void se05x_ic_reset()
{
    axReset_ResetPluseDUT();
    smComT1oI2C_ComReset(NULL);
    sm_usleep(3000);
    return;
}

#endif
