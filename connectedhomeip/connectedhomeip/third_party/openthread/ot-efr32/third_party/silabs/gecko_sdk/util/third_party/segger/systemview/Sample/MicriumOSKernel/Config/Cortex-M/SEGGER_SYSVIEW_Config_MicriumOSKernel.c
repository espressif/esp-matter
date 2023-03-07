/***************************************************************************/ /**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/*********************************************************************
 *                SEGGER Microcontroller GmbH & Co. KG                *
 *                        The Embedded Experts                        *
 **********************************************************************
 *                                                                    *
 *       (c) 2015 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
 *                                                                    *
 *       www.segger.com     Support: support@segger.com               *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 *       SEGGER SystemView * Real-time application analysis           *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 * All rights reserved.                                               *
 *                                                                    *
 * SEGGER strongly recommends to not make any changes                 *
 * to or modify the source code of this software in order to stay     *
 * compatible with the RTT protocol and J-Link.                       *
 *                                                                    *
 * Redistribution and use in source and binary forms, with or         *
 * without modification, are permitted provided that the following    *
 * conditions are met:                                                *
 *                                                                    *
 * o Redistributions of source code must retain the above copyright   *
 *   notice, this list of conditions and the following disclaimer.    *
 *                                                                    *
 * o Redistributions in binary form must reproduce the above          *
 *   copyright notice, this list of conditions and the following      *
 *   disclaimer in the documentation and/or other materials provided  *
 *   with the distribution.                                           *
 *                                                                    *
 * o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
 *   nor the names of its contributors may be used to endorse or      *
 *   promote products derived from this software without specific     *
 *   prior written permission.                                        *
 *                                                                    *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                            *
 *                                                                    *
 **********************************************************************
 *                                                                    *
 *       SystemView version: V2.52a                                    *
 *                                                                    *
 **********************************************************************
   -------------------------- END-OF-HEADER -----------------------------

   File        : SEGGER_SYSVIEW_Config_MicriumOSKernel.c
   Purpose     : Sample setup configuration of SystemView with Micrium
              OS Kernel on the Silicon Labs Giant Gecko.
   Revision: $Rev: 7766 $
 */
#include "SEGGER_SYSVIEW.h"
#include  <kernel/include/os.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <cpu/include/cpu.h>
#include  <os_trace_events.h>
#include  <em_cmu.h>

extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */
// The application name to be displayed in SystemViewer
#define SYSVIEW_APP_NAME        "App Name Here"

// The target device name
#define SYSVIEW_DEVICE_NAME     "Dev Name Here"

// The lowest RAM address used for IDs (pointers)
#define SYSVIEW_RAM_BASE        (0x20000000)

/*********************************************************************
 *
 *       _cbSendSystemDesc()
 *
 *  Function description
 *    Sends SystemView description strings.
 */
static void _cbSendSystemDesc(void)
{
  SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME ",D="SYSVIEW_DEVICE_NAME ",O=Micrium OS Kernel");
  SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick IRQ");
  //
  SYSVIEW_SendResourceList();
}

/*********************************************************************
 *
 *       Global functions
 *
 **********************************************************************
 */
void SEGGER_SYSVIEW_Conf(void)
{
  U32  sys_freq;       // Frequency of the timestamp. Must match SEGGER_SYSVIEW_GET_TIMESTAMP in SEGGER_SYSVIEW_Conf.h
  U32  cpu_freq;       // System Frequency. SystemcoreClock is used in most CMSIS compatible projects.

  sys_freq = SystemCoreClockGet();
  cpu_freq = SystemCoreClockGet();

  SEGGER_SYSVIEW_Init(sys_freq,
                      cpu_freq,
                      &SYSVIEW_X_OS_TraceAPI,
                      _cbSendSystemDesc);

  SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);
}

/*************************** End of file ****************************/
