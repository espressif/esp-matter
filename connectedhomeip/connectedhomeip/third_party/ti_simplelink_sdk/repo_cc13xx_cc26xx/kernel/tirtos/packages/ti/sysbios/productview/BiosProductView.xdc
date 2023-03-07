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
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== BiosProductView.xdc ========
 */

metaonly module BiosProductView inherits xdc.tools.product.IProductView
{
    /*
     *  ======== homeModule ========
     *  Specify the home module for SYS/BIOS
     */
    override config String homeModule = "ti.sysbios.BIOS";

    /*
     *  ======== linksToArray ========
     *  Specify other products that are "linked to" from SYS/BIOS
     */
    override config String linksToArray[] = ["org.eclipse.rtsc.xdctools"];
    
/* System modules */    

    config ProductElemDesc ti_sysbios_BIOS;
    config ProductElemDesc ti_sysbios_rom_ROM;
    config ProductElemDesc xdc_cfg_Program;
    config ProductElemDesc xdc_runtime_Defaults;
    config ProductElemDesc xdc_runtime_System;
    config ProductElemDesc xdc_runtime_Startup;
    config ProductElemDesc xdc_runtime_Main;

/* System Providers modules */  

    config ProductElemDesc xdc_runtime_SysMin;
    config ProductElemDesc xdc_runtime_SysStd;

/* Diagnostics modules */

    config ProductElemDesc xdc_runtime_Log;
    config ProductElemDesc xdc_runtime_LoggerBuf;
    config ProductElemDesc xdc_runtime_LoggerSys;
    config ProductElemDesc xdc_runtime_Assert;
    config ProductElemDesc xdc_runtime_Diags;
    config ProductElemDesc xdc_runtime_Error;
    config ProductElemDesc xdc_runtime_Timestamp;

/* Memory Management modules */

    config ProductElemDesc ti_sysbios_hal_Cache;
    config ProductElemDesc xdc_runtime_Memory;
    config ProductElemDesc xdc_runtime_HeapMin;
    config ProductElemDesc ti_sysbios_heaps_HeapBuf;
    config ProductElemDesc ti_sysbios_heaps_HeapCallback;
    config ProductElemDesc ti_sysbios_heaps_HeapMem;
    config ProductElemDesc ti_sysbios_heaps_HeapMultiBuf;
    config ProductElemDesc ti_sysbios_heaps_HeapNull;
    config ProductElemDesc ti_sysbios_heaps_HeapTrack;

/* Scheduling modules */

    config ProductElemDesc ti_sysbios_knl_Task;
    config ProductElemDesc ti_sysbios_knl_Swi;
    config ProductElemDesc ti_sysbios_hal_Hwi;
    config ProductElemDesc ti_sysbios_knl_Idle;
    config ProductElemDesc ti_sysbios_knl_Clock;
    config ProductElemDesc ti_sysbios_hal_Seconds;
    config ProductElemDesc ti_sysbios_hal_Timer;

/* Synchronization modules */

    config ProductElemDesc ti_sysbios_knl_Semaphore;
    config ProductElemDesc ti_sysbios_knl_Mailbox;
    config ProductElemDesc ti_sysbios_knl_Queue;
    config ProductElemDesc ti_sysbios_knl_Event;
    config ProductElemDesc ti_sysbios_gates_GateAll;
    config ProductElemDesc ti_sysbios_gates_GateHwi;
    config ProductElemDesc ti_sysbios_gates_GateSwi;
    config ProductElemDesc ti_sysbios_gates_GateTask;
    config ProductElemDesc ti_sysbios_gates_GateMutex;
    config ProductElemDesc ti_sysbios_gates_GateMutexPri;

/* Communication Synchronization modules */

    config ProductElemDesc ti_sysbios_io_DEV;
    config ProductElemDesc ti_sysbios_io_GIO;

/* Syncs modules */

    config ProductElemDesc xdc_runtime_knl_SyncGeneric;
    config ProductElemDesc ti_sysbios_syncs_SyncEvent;
    config ProductElemDesc ti_sysbios_syncs_SyncSem;
    config ProductElemDesc ti_sysbios_syncs_SyncSwi;

/* Utilities modules */

    config ProductElemDesc ti_sysbios_utils_Load;

/* Target modules */

    config ProductElemDesc ti_sysbios_hal_unicache_Cache;
    config ProductElemDesc ti_sysbios_hal_ammu_AMMU;
    config ProductElemDesc ti_sysbios_rts_gnu_SemiHostSupport;
    config ProductElemDesc ti_sysbios_timers_dmtimer_Timer;
    config ProductElemDesc ti_sysbios_timers_gptimer_Timer;
    config ProductElemDesc ti_sysbios_timers_timer64_Timer;
    config ProductElemDesc ti_sysbios_timers_timer64_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_c28_Hwi;
    config ProductElemDesc ti_sysbios_family_c28_Timer;
    config ProductElemDesc ti_sysbios_family_c28_TimestampProvider;
    config ProductElemDesc ti_sysbios_family_c28_f28m35x_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_c64p_Hwi;
    config ProductElemDesc ti_sysbios_family_c64p_Cache;
    config ProductElemDesc ti_sysbios_family_c64p_MemoryProtect;
    config ProductElemDesc ti_sysbios_family_c64p_EventCombiner;
    config ProductElemDesc ti_sysbios_family_c64p_Exception;
    config ProductElemDesc ti_sysbios_family_c64p_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_c66_Cache;
    config ProductElemDesc ti_sysbios_family_c66_tci66xx_CpIntc;

    config ProductElemDesc ti_sysbios_family_c674_Power;

    config ProductElemDesc ti_sysbios_family_msp430_ClockFreqs;
    config ProductElemDesc ti_sysbios_family_msp430_Hwi;
    config ProductElemDesc ti_sysbios_family_msp430_Power;
    config ProductElemDesc ti_sysbios_family_msp430_Timer;
    config ProductElemDesc ti_sysbios_family_msp430_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_arm9_Cache;
    config ProductElemDesc ti_sysbios_family_arm_arm9_Mmu;
    config ProductElemDesc ti_sysbios_family_arm_dm6446_Hwi;

    config ProductElemDesc ti_sysbios_family_arm_da830_Hwi;
    config ProductElemDesc ti_sysbios_family_arm_da830_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_f28m35x_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_v7a_Pmu;

    config ProductElemDesc ti_sysbios_family_arm_a8_Cache;
    config ProductElemDesc ti_sysbios_family_arm_a8_Mmu;
    config ProductElemDesc ti_sysbios_family_arm_a8_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_a8_intcps_Hwi;

    config ProductElemDesc ti_sysbios_family_arm_a9_Cache;
    config ProductElemDesc ti_sysbios_family_arm_a9_Timer;

    config ProductElemDesc ti_sysbios_family_arm_a15_Cache;
    config ProductElemDesc ti_sysbios_family_arm_a15_Mmu;
    config ProductElemDesc ti_sysbios_family_arm_a15_TimestampProvider;
    config ProductElemDesc ti_sysbios_family_arm_a15_tci66xx_CpIntc;

    config ProductElemDesc ti_sysbios_family_arm_exc_Exception;

    config ProductElemDesc ti_sysbios_family_arm_gic_Hwi;

    config ProductElemDesc ti_sysbios_family_arm_systimer_Timer;

    config ProductElemDesc ti_sysbios_family_arm_m3_Hwi;
    config ProductElemDesc ti_sysbios_family_arm_m3_Timer;
    config ProductElemDesc ti_sysbios_family_arm_m3_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_lm3_Timer;
    config ProductElemDesc ti_sysbios_family_arm_lm3_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_lm4_Timer;
    config ProductElemDesc ti_sysbios_family_arm_lm4_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_ducati_Core;
    config ProductElemDesc ti_sysbios_family_arm_ducati_Timer;
    config ProductElemDesc ti_sysbios_family_arm_ducati_TimestampProvider;

    config ProductElemDesc ti_sysbios_family_arm_cc26xx_Alarm;
    config ProductElemDesc ti_sysbios_family_arm_cc26xx_Boot;
    config ProductElemDesc ti_sysbios_family_arm_cc26xx_Timer;
    config ProductElemDesc ti_sysbios_family_arm_cc26xx_Seconds;
    config ProductElemDesc ti_sysbios_family_arm_cc26xx_TimestampProvider;

/* groups */

    config ProductElemDesc systemGroup;
    config ProductElemDesc diagnosticsGroup;
    config ProductElemDesc memoryGroup;
    config ProductElemDesc schedulingGroup;
    config ProductElemDesc synchronizationGroup;
    config ProductElemDesc ioGroup;
    config ProductElemDesc targetGroup;

/* sub groups */

    config ProductElemDesc syncsGroup;
    config ProductElemDesc devDriversGroup;
    config ProductElemDesc gatesGroup;
    config ProductElemDesc heapsGroup;
    config ProductElemDesc loggersGroup;
    config ProductElemDesc rtaGroup;
    config ProductElemDesc systemProvidersGroup;
    config ProductElemDesc timersGroup;

    config ProductElemDesc c28Group;
    config ProductElemDesc c64Group;
    config ProductElemDesc c64pGroup;
    config ProductElemDesc c64tGroup;
    config ProductElemDesc c66Group;
    config ProductElemDesc c674Group;
    config ProductElemDesc msp430Group;
    config ProductElemDesc armGroup;
    config ProductElemDesc arm9davinciGroup;
    config ProductElemDesc arm9da830Group;
    config ProductElemDesc arm9Group;
    config ProductElemDesc a8Group;
    config ProductElemDesc a9Group;
    config ProductElemDesc a15Group;
    config ProductElemDesc m3Group;
    config ProductElemDesc lm3Group;
    config ProductElemDesc lm4Group;
    config ProductElemDesc ducatiGroup;
    config ProductElemDesc cc26xxGroup;

/* product */

    config ProductElemDesc biosProduct;

        
};
