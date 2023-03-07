/** @file mlan_remap_mem_operations.h
 *
 *  @brief This file contains redefinition of memory routines
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

/*
 * File added for wmsdk. Not present in original mlan release.
 *
 * Purpose: The mlan release source files contain non-standard (libc)
 * prototypes for memcpy, memmove, memcmp, etc. This causes problems when
 * mlan header files are included by the remaining code. We work around
 * this problem by remapping these operations to standard functions.
 *
 * IMPORTANT: Ensure that this file is included in every mlan source file
 * which used mem* operations. ENSURE that this is the last file included
 * in the include header list.
 */

#ifdef memset
#undef memset
#endif
/** Memset routine */
#define memset(adapter, s, c, len) memset(s, c, len)

#ifdef memmove
#undef memmove
#endif
/** Memmove routine */
#define memmove(adapter, dest, src, len) memmove(dest, src, len)

#ifdef memcpy
#undef memcpy
#endif
/** Memcpy routine */
#define memcpy(adapter, to, from, len) memcpy(to, from, len)

#ifdef memcmp
#undef memcmp
#endif
/** Memcmp routine */
#define memcmp(adapter, s1, s2, len) memcmp(s1, s2, len)
