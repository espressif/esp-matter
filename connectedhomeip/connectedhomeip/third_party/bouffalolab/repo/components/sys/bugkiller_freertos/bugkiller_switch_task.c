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

void bugkiller_switch_task(unsigned int *sp)
{
    printf("set $sp=%p+120\r\n", (void *)sp);
    printf("set $pc=%p\r\n", (void *)*sp++);
    printf("set $ra=%p\r\n", (void *)*sp++);
    printf("set $t0=%p\r\n", (void *)*sp++);
    printf("set $t1=%p\r\n", (void *)*sp++);
    printf("set $t2=%p\r\n", (void *)*sp++);
    printf("set $s0=%p\r\n", (void *)*sp++);
    printf("set $s1=%p\r\n", (void *)*sp++);
    printf("set $a0=%p\r\n", (void *)*sp++);
    printf("set $a1=%p\r\n", (void *)*sp++);
    printf("set $a2=%p\r\n", (void *)*sp++);
    printf("set $a3=%p\r\n", (void *)*sp++);
    printf("set $a4=%p\r\n", (void *)*sp++);
    printf("set $a5=%p\r\n", (void *)*sp++);
    printf("set $a6=%p\r\n", (void *)*sp++);
    printf("set $a7=%p\r\n", (void *)*sp++);
    printf("set $s2=%p\r\n", (void *)*sp++);
    printf("set $s3=%p\r\n", (void *)*sp++);
    printf("set $s4=%p\r\n", (void *)*sp++);
    printf("set $s5=%p\r\n", (void *)*sp++);
    printf("set $s6=%p\r\n", (void *)*sp++);
    printf("set $s7=%p\r\n", (void *)*sp++);
    printf("set $s8=%p\r\n", (void *)*sp++);
    printf("set $s9=%p\r\n", (void *)*sp++);
    printf("set $s10=%p\r\n", (void *)*sp++);
    printf("set $s11=%p\r\n", (void *)*sp++);
    printf("set $t3=%p\r\n", (void *)*sp++);
    printf("set $t4=%p\r\n", (void *)*sp++);
    printf("set $t5=%p\r\n", (void *)*sp++);
    printf("set $t6=%p\r\n", (void *)*sp++);
    printf("printf \"mstatus\"\r\np/x %p\r\n", (void *)*sp);
}
