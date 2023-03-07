/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifdef GP_ASSERT
#error include this file first, once
#endif

#include "gpAssert.h"

void Nvm_EnableSafetyNet(void);
void Nvm_DisableSafetyNet(void);

Bool Nvm_IsSafetyNetEnabled(void);
void Nvm_SafetyNetHandler(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line);

#undef GP_ASSERT
// override default assert macro
#define GP_ASSERT(info, check)                                                                  \
do {                                                                                            \
    if(GP_DIVERSITY_GLOBAL_ASSERT_LEVEL >= GP_ASSERT_GET_LEVEL(info))                           \
    {                                                                                           \
        if (!(check))                                                                           \
        {                                                                                       \
            if (Nvm_IsSafetyNetEnabled())                                                       \
            {                                                                                   \
                Nvm_SafetyNetHandler(info, GP_COMPONENT_ID, GP_ASSERT_FILENAME, __LINE__);      \
            }                                                                                   \
            GP_ASSERT_DO_ASSERT_HANDLING(info, GP_COMPONENT_ID, GP_ASSERT_FILENAME, __LINE__);  \
        }                                                                                       \
    }                                                                                           \
} while (false)


