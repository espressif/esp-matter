/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#ifndef _COMPAT_ATTRIBUTE_H_
#define _COMPAT_ATTRIBUTE_H_

#define __DEPRECATED__(x) __attribute__((deprecated(x)))
#define __PACKED__ __attribute__ ((packed))

#define __PACKED_START__
#define __PACKED_END__ __PACKED__

#define __UNUSED__ __attribute__((unused))
#define __MAY_ALIAS__ __attribute__((may_alias))

#define __ALIGNED__(y) __attribute__((aligned(y)))
#define __SECTION__(y) __attribute__((section(#y)))

#define __FORCED_INLINE__ inline __attribute__((always_inline))
#define __NOINLINE__ __attribute__((noinline))

#define __WEAK__ __attribute__((weak))
#define __INTERRUPT__ __attribute__((interrupt))
#define __USED__ __attribute__((used))
#define __NAKED__ __attribute__((naked))

#define __VISIBILITY__ __attribute__((visibility("default")))
#endif  /* _COMPAT_ATTRIBUTE_H_ */
