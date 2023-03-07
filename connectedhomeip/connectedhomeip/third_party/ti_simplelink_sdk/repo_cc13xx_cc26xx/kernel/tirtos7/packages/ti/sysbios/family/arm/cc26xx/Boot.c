/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQueueNTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== Boot.c ========
 */

#include <ti/devices/DeviceFamily.h>

#include <ti/sysbios/runtime/Types.h>

#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/setup.h)

#include <ti/sysbios/family/arm/cc26xx/Boot.h>

extern void SetupTrimDevice(void);

/*
 *  ======== Boot_trimDevice ========
 */
void Boot_trimDevice(void)
{
    SetupTrimDevice();
}

/*
 *  ======== Boot_getBootReason ========
 */
uint32_t Boot_getBootReason()
{
    return (SysCtrlResetSourceGet());
}

#if defined(__ti__) || defined(__clang__)

extern void _c_int00();

void ResetISR()
{
    _c_int00();
}

#elif defined(__IAR_SYSTEMS_ICC__)

extern void __iar_program_start();

void ResetISR ()
{
    __iar_program_start();
}

#endif
#ifdef Boot_customerConfig
/* pull default CCFG from driverlib */
#include DeviceFamily_constructPath(startup_files/ccfg.c)
#endif
