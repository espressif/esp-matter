/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_UTIL_INT_DEFS_H
#define  _CC_UTIL_INT_DEFS_H

typedef enum  {
    UTIL_USER_KEY = 0,
    UTIL_ROOT_KEY = 1,
    UTIL_KCP_KEY = 2,
    UTIL_KCE_KEY = 3,
    UTIL_KPICV_KEY = 4,
    UTIL_KCEICV_KEY = 5,
    UTIL_END_OF_KEY_TYPE = 0x7FFFFFFF
}UtilKeyType_t;


#endif /*_CC_UTIL_INT_DEFS_H*/
