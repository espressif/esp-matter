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

#include <stdio.h>
#include <hosal_rng.h>
#include <bl_sec.h>

int hosal_rng_init(void)
{
    bl_sec_init();
    return 0;
}

int hosal_random_num_read(void *buf, uint32_t bytes)
{
    uint32_t tmp;
    uint32_t *val;
    int i, nwords;

	if(buf == NULL)
	{
		return -1;
	}
    
    val = (uint32_t *)buf;

    nwords = bytes / 4;

    for (i = 0; i < nwords; i++) {
        val[i] = bl_rand();
    }
    val += nwords;
    
    tmp = bl_rand();
    
    if (bytes % 4 == 1) {
        *((uint8_t *)val) = (uint8_t)tmp;
    }
    if (bytes % 4 == 2) {
        *((uint8_t *)val) = (uint8_t)tmp;
        *((uint8_t *)val + 1) = (uint8_t)(tmp >> 8);
    }
    if (bytes % 4 == 3) {
        *((uint8_t *)val) = (uint8_t)tmp;
        *((uint8_t *)val + 1) = (uint8_t)(tmp >> 16);
        *((uint8_t *)val + 2) = (uint8_t)(tmp >> 24);
    }
   
    return 0;
}


