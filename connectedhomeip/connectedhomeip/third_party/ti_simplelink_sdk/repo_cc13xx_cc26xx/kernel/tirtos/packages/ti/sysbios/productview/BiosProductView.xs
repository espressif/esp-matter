/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== BiosProductView.xs ========
 */

/* Scheduling elements */

var ProdQuery;
function module$meta$init()
{
    
/* Modules */

/* ROM Module */
    this.ti_sysbios_rom_ROM.elemName = "ROM";
    this.ti_sysbios_rom_ROM.moduleName = "ti.sysbios.rom.ROM";
    this.ti_sysbios_rom_ROM.iconFileName = "icons/mem.ico";

/* System Providers Sub Group Modules */

    this.xdc_runtime_SysMin.elemName = "SysMin";
    this.xdc_runtime_SysMin.moduleName = "xdc.runtime.SysMin";
    this.xdc_runtime_SysMin.iconFileName = "icons/sys.ico";

    this.xdc_runtime_SysStd.elemName = "SysStd";
    this.xdc_runtime_SysStd.moduleName = "xdc.runtime.SysStd";
    this.xdc_runtime_SysStd.iconFileName = "icons/sys.ico";

/* System Group Modules */

    this.xdc_cfg_Program.elemName = "Program";
    this.xdc_cfg_Program.moduleName = "xdc.cfg.Program";
    this.xdc_cfg_Program.iconFileName = "icons/cfg.ico";

    this.xdc_runtime_Main.elemName = "Main";
    this.xdc_runtime_Main.moduleName = "xdc.runtime.Main";
    this.xdc_runtime_Main.iconFileName = "icons/cfg.ico";

    this.xdc_runtime_Defaults.elemName = "Defaults";
    this.xdc_runtime_Defaults.moduleName = "xdc.runtime.Defaults";
    this.xdc_runtime_Defaults.iconFileName = "icons/prj.ico";
        
    this.xdc_runtime_System.elemName = "System";
    this.xdc_runtime_System.moduleName = "xdc.runtime.System";
    this.xdc_runtime_System.iconFileName = "icons/sys.ico";

    this.xdc_runtime_Startup.elemName = "Startup";
    this.xdc_runtime_Startup.moduleName = "xdc.runtime.Startup";
    this.xdc_runtime_Startup.iconFileName = "icons/ACCEL.ico";

/* Loggers Sub Group Modules */

    this.xdc_runtime_LoggerBuf.elemName = "LoggerBuf";
    this.xdc_runtime_LoggerBuf.moduleName = "xdc.runtime.LoggerBuf";
    this.xdc_runtime_LoggerBuf.iconFileName = "icons/log.ico";

    this.xdc_runtime_LoggerSys.elemName = "LoggerSys";
    this.xdc_runtime_LoggerSys.moduleName = "xdc.runtime.LoggerSys";
    this.xdc_runtime_LoggerSys.iconFileName = "icons/log.ico";

/* Realtime Analysis */
    this.ti_sysbios_utils_Load.elemName = "Load";
    this.ti_sysbios_utils_Load.moduleName = "ti.sysbios.utils.Load";
    this.ti_sysbios_utils_Load.iconFileName = "icons/timer.ico";

/* Diagnostics Group Modules */

    this.xdc_runtime_Log.elemName = "Log";
    this.xdc_runtime_Log.moduleName = "xdc.runtime.Log";
    this.xdc_runtime_Log.iconFileName = "icons/log.ico";
    
    this.xdc_runtime_Assert.elemName = "Assert";
    this.xdc_runtime_Assert.moduleName = "xdc.runtime.Assert";
    this.xdc_runtime_Assert.iconFileName = "icons/hwi.ico";

    this.xdc_runtime_Diags.elemName = "Diags";
    this.xdc_runtime_Diags.moduleName = "xdc.runtime.Diags";
    this.xdc_runtime_Diags.iconFileName = "icons/sts.ico";

    this.xdc_runtime_Error.elemName = "Error";
    this.xdc_runtime_Error.moduleName = "xdc.runtime.Error";
    this.xdc_runtime_Error.iconFileName = "icons/hwi.ico";

    this.xdc_runtime_Timestamp.elemName = "Timestamp";
    this.xdc_runtime_Timestamp.moduleName = "xdc.runtime.Timestamp";
    this.xdc_runtime_Timestamp.iconFileName = "icons/rtc.ico";

/* Heaps Sub Group Modules */

    this.xdc_runtime_HeapMin.elemName = "HeapMin";
    this.xdc_runtime_HeapMin.moduleName = "xdc.runtime.HeapMin";
    this.xdc_runtime_HeapMin.iconFileName = "icons/mem.ico";

    this.ti_sysbios_heaps_HeapNull.elemName = "HeapNull";
    this.ti_sysbios_heaps_HeapNull.moduleName = "ti.sysbios.heaps.HeapNull";
    this.ti_sysbios_heaps_HeapNull.iconFileName = "icons/mem.ico";

    this.ti_sysbios_heaps_HeapMem.elemName = "HeapMem";
    this.ti_sysbios_heaps_HeapMem.moduleName = "ti.sysbios.heaps.HeapMem";
    this.ti_sysbios_heaps_HeapMem.iconFileName = "icons/mem.ico";

    this.ti_sysbios_heaps_HeapBuf.elemName = "HeapBuf";
    this.ti_sysbios_heaps_HeapBuf.moduleName = "ti.sysbios.heaps.HeapBuf";
    this.ti_sysbios_heaps_HeapBuf.iconFileName = "icons/mem.ico";

    this.ti_sysbios_heaps_HeapCallback.elemName = "HeapCallback";
    this.ti_sysbios_heaps_HeapCallback.moduleName = "ti.sysbios.heaps.HeapCallback";
    this.ti_sysbios_heaps_HeapCallback.iconFileName = "icons/mem.ico";

    this.ti_sysbios_heaps_HeapMultiBuf.elemName = "HeapMultiBuf";
    this.ti_sysbios_heaps_HeapMultiBuf.moduleName =
        "ti.sysbios.heaps.HeapMultiBuf";
    this.ti_sysbios_heaps_HeapMultiBuf.iconFileName = "icons/mem.ico";
    
    this.ti_sysbios_heaps_HeapTrack.elemName = "HeapTrack";
    this.ti_sysbios_heaps_HeapTrack.moduleName = "ti.sysbios.heaps.HeapTrack";
    this.ti_sysbios_heaps_HeapTrack.iconFileName = "icons/mem.ico";

/* Memory Management Group Modules */

    this.ti_sysbios_hal_Cache.elemName = "Cache";
    this.ti_sysbios_hal_Cache.moduleName = "ti.sysbios.hal.Cache";
    this.ti_sysbios_hal_Cache.iconFileName = "icons/mem.ico";

    this.xdc_runtime_Memory.elemName = "Memory";
    this.xdc_runtime_Memory.moduleName = "xdc.runtime.Memory";
    this.xdc_runtime_Memory.iconFileName = "icons/mem.ico";

/* Scheduling Group Modules */

    this.ti_sysbios_knl_Task.elemName = "Task";
    this.ti_sysbios_knl_Task.moduleName = "ti.sysbios.knl.Task";
    this.ti_sysbios_knl_Task.iconFileName = "icons/tsk.ico";

    this.ti_sysbios_knl_Swi.elemName = "Swi";
    this.ti_sysbios_knl_Swi.moduleName = "ti.sysbios.knl.Swi";
    this.ti_sysbios_knl_Swi.iconFileName = "icons/swi.ico";

    this.ti_sysbios_hal_Hwi.elemName = "Hwi";
    this.ti_sysbios_hal_Hwi.moduleName = "ti.sysbios.hal.Hwi";
    this.ti_sysbios_hal_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_knl_Idle.elemName = "Idle";
    this.ti_sysbios_knl_Idle.moduleName = "ti.sysbios.knl.Idle";
    this.ti_sysbios_knl_Idle.iconFileName = "icons/idl.ico";

    this.ti_sysbios_knl_Clock.elemName = "Clock";
    this.ti_sysbios_knl_Clock.moduleName = "ti.sysbios.knl.Clock";
    this.ti_sysbios_knl_Clock.iconFileName = "icons/clk.ico";

    this.ti_sysbios_hal_Seconds.elemName = "Seconds";
    this.ti_sysbios_hal_Seconds.moduleName = "ti.sysbios.hal.Seconds";
    this.ti_sysbios_hal_Seconds.iconFileName = "icons/clk.ico";

    this.ti_sysbios_hal_Timer.elemName = "Timer";
    this.ti_sysbios_hal_Timer.moduleName = "ti.sysbios.hal.Timer";
    this.ti_sysbios_hal_Timer.iconFileName = "icons/rtc.ico";

/* Gates Sub Group Modules */

    this.ti_sysbios_gates_GateAll.elemName = "GateAll";
    this.ti_sysbios_gates_GateAll.moduleName = "ti.sysbios.gates.GateAll";
    this.ti_sysbios_gates_GateAll.iconFileName = "icons/lck.ico";

    this.ti_sysbios_gates_GateHwi.elemName = "GateHwi";
    this.ti_sysbios_gates_GateHwi.moduleName = "ti.sysbios.gates.GateHwi";
    this.ti_sysbios_gates_GateHwi.iconFileName = "icons/lck.ico";

    this.ti_sysbios_gates_GateSwi.elemName = "GateSwi";
    this.ti_sysbios_gates_GateSwi.moduleName = "ti.sysbios.gates.GateSwi";
    this.ti_sysbios_gates_GateSwi.iconFileName = "icons/lck.ico";

    this.ti_sysbios_gates_GateTask.elemName = "GateTask";
    this.ti_sysbios_gates_GateTask.moduleName = "ti.sysbios.gates.GateTask";
    this.ti_sysbios_gates_GateTask.iconFileName = "icons/lck.ico";

    this.ti_sysbios_gates_GateMutex.elemName = "GateMutex";
    this.ti_sysbios_gates_GateMutex.moduleName = "ti.sysbios.gates.GateMutex";
    this.ti_sysbios_gates_GateMutex.iconFileName = "icons/lck.ico";

    this.ti_sysbios_gates_GateMutexPri.elemName = "GateMutexPri";
    this.ti_sysbios_gates_GateMutexPri.moduleName =
        "ti.sysbios.gates.GateMutexPri";
    this.ti_sysbios_gates_GateMutexPri.iconFileName = "icons/lck.ico";

/* Synchronization Group Modules */

    this.ti_sysbios_knl_Semaphore.elemName = "Semaphore";
    this.ti_sysbios_knl_Semaphore.moduleName = "ti.sysbios.knl.Semaphore";
    this.ti_sysbios_knl_Semaphore.iconFileName = "icons/sem.ico";

    this.ti_sysbios_knl_Event.elemName = "Event";
    this.ti_sysbios_knl_Event.moduleName = "ti.sysbios.knl.Event";
    this.ti_sysbios_knl_Event.iconFileName = "icons/sem.ico";

    this.ti_sysbios_knl_Mailbox.elemName = "Mailbox";
    this.ti_sysbios_knl_Mailbox.moduleName = "ti.sysbios.knl.Mailbox";
    this.ti_sysbios_knl_Mailbox.iconFileName = "icons/mbx.ico";

    this.ti_sysbios_knl_Queue.elemName = "Queue";
    this.ti_sysbios_knl_Queue.moduleName = "ti.sysbios.knl.Queue";
    this.ti_sysbios_knl_Queue.iconFileName = "icons/que.ico";

/* Communication and I/O Group Modules */

    this.ti_sysbios_io_DEV.elemName = "DEV";
    this.ti_sysbios_io_DEV.moduleName = "ti.sysbios.io.DEV";
    this.ti_sysbios_io_DEV.iconFileName = "icons/dev.ico";

    this.ti_sysbios_io_GIO.elemName = "GIO";
    this.ti_sysbios_io_GIO.moduleName = "ti.sysbios.io.GIO";
    this.ti_sysbios_io_GIO.iconFileName = "icons/sio.ico";

/* Syncs Sub Group Modules */

    this.xdc_runtime_knl_SyncGeneric.elemName = "SyncGeneric";
    this.xdc_runtime_knl_SyncGeneric.moduleName = "xdc.runtime.knl.SyncGeneric";
    this.xdc_runtime_knl_SyncGeneric.iconFileName = "icons/sem.ico";

    this.ti_sysbios_syncs_SyncSem.elemName = "SyncSem";
    this.ti_sysbios_syncs_SyncSem.moduleName = "ti.sysbios.syncs.SyncSem";
    this.ti_sysbios_syncs_SyncSem.iconFileName = "icons/sem.ico";

    this.ti_sysbios_syncs_SyncEvent.elemName = "SyncEvent";
    this.ti_sysbios_syncs_SyncEvent.moduleName = "ti.sysbios.syncs.SyncEvent";
    this.ti_sysbios_syncs_SyncEvent.iconFileName = "icons/sem.ico";

    this.ti_sysbios_syncs_SyncSwi.elemName = "SyncSwi";
    this.ti_sysbios_syncs_SyncSwi.moduleName = "ti.sysbios.syncs.SyncSwi";
    this.ti_sysbios_syncs_SyncSwi.iconFileName = "icons/swi.ico";

/* Family/Device-Specific modules */

    this.ti_sysbios_hal_unicache_Cache.elemName = "Unicache";
    this.ti_sysbios_hal_unicache_Cache.moduleName = "ti.sysbios.hal.unicache.Cache";
    this.ti_sysbios_hal_unicache_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_hal_ammu_AMMU.elemName = "AMMU";
    this.ti_sysbios_hal_ammu_AMMU.moduleName = "ti.sysbios.hal.ammu.AMMU";
    this.ti_sysbios_hal_ammu_AMMU.iconFileName = "icons/mem.ico";

    this.ti_sysbios_rts_gnu_SemiHostSupport.elemName = "SemiHostSupport";
    this.ti_sysbios_rts_gnu_SemiHostSupport.moduleName = "ti.sysbios.rts.gnu.SemiHostSupport";
    this.ti_sysbios_rts_gnu_SemiHostSupport.iconFileName = "icons/hst.ico";

    this.ti_sysbios_timers_dmtimer_Timer.elemName = "DM Timer";
    this.ti_sysbios_timers_dmtimer_Timer.moduleName = "ti.sysbios.timers.dmtimer.Timer";
    this.ti_sysbios_timers_dmtimer_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_timers_gptimer_Timer.elemName = "GP Timer";
    this.ti_sysbios_timers_gptimer_Timer.moduleName = "ti.sysbios.timers.gptimer.Timer";
    this.ti_sysbios_timers_gptimer_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_timers_timer64_Timer.elemName = "Timer64 Timer";
    this.ti_sysbios_timers_timer64_Timer.moduleName = "ti.sysbios.timers.timer64.Timer";
    this.ti_sysbios_timers_timer64_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_timers_timer64_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_timers_timer64_TimestampProvider.moduleName = "ti.sysbios.timers.timer64.TimestampProvider";
    this.ti_sysbios_timers_timer64_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_c28_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_c28_Hwi.moduleName = "ti.sysbios.family.c28.Hwi";
    this.ti_sysbios_family_c28_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_c28_Timer.elemName = "Timer";
    this.ti_sysbios_family_c28_Timer.moduleName = "ti.sysbios.family.c28.Timer";
    this.ti_sysbios_family_c28_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_c28_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_c28_TimestampProvider.moduleName = "ti.sysbios.family.c28.TimestampProvider";
    this.ti_sysbios_family_c28_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_c28_f28m35x_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_c28_f28m35x_TimestampProvider.moduleName = "ti.sysbios.family.c28.f28m35x.TimestampProvider";
    this.ti_sysbios_family_c28_f28m35x_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_c64p_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_c64p_Hwi.moduleName = "ti.sysbios.family.c64p.Hwi";
    this.ti_sysbios_family_c64p_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_c64p_Cache.elemName = "Cache";
    this.ti_sysbios_family_c64p_Cache.moduleName = "ti.sysbios.family.c64p.Cache";
    this.ti_sysbios_family_c64p_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_c64p_MemoryProtect.elemName = "MemoryProtect";
    this.ti_sysbios_family_c64p_MemoryProtect.moduleName = "ti.sysbios.family.c64p.MemoryProtect";
    this.ti_sysbios_family_c64p_MemoryProtect.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_c64p_EventCombiner.elemName = "EventCombiner";
    this.ti_sysbios_family_c64p_EventCombiner.moduleName = "ti.sysbios.family.c64p.EventCombiner";
    this.ti_sysbios_family_c64p_EventCombiner.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_c64p_Exception.elemName = "Exception";
    this.ti_sysbios_family_c64p_Exception.moduleName = "ti.sysbios.family.c64p.Exception";
    this.ti_sysbios_family_c64p_Exception.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_c66_Cache.elemName = "Cache";
    this.ti_sysbios_family_c66_Cache.moduleName = "ti.sysbios.family.c66.Cache";
    this.ti_sysbios_family_c66_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_c66_tci66xx_CpIntc.elemName = "CpIntc";
    this.ti_sysbios_family_c66_tci66xx_CpIntc.moduleName = "ti.sysbios.family.c66.tci66xx.CpIntc";
    this.ti_sysbios_family_c66_tci66xx_CpIntc.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_c674_Power.elemName = "Power";
    this.ti_sysbios_family_c674_Power.moduleName = "ti.sysbios.family.c674.Power";
    this.ti_sysbios_family_c674_Power.iconFileName = "icons/pwr.ico";

    this.ti_sysbios_family_msp430_ClockFreqs.elemName = "ClockFreqs";
    this.ti_sysbios_family_msp430_ClockFreqs.moduleName = "ti.sysbios.family.msp430.ClockFreqs";
    this.ti_sysbios_family_msp430_ClockFreqs.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_msp430_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_msp430_Hwi.moduleName = "ti.sysbios.family.msp430.Hwi";
    this.ti_sysbios_family_msp430_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_msp430_Power.elemName = "Power";
    this.ti_sysbios_family_msp430_Power.moduleName = "ti.sysbios.family.msp430.Power";
    this.ti_sysbios_family_msp430_Power.iconFileName = "icons/pwr.ico";

    this.ti_sysbios_family_msp430_Timer.elemName = "Timer";
    this.ti_sysbios_family_msp430_Timer.moduleName = "ti.sysbios.family.msp430.Timer";
    this.ti_sysbios_family_msp430_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_msp430_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_msp430_TimestampProvider.moduleName = "ti.sysbios.family.msp430.TimestampProvider";
    this.ti_sysbios_family_msp430_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_arm9_Cache.elemName = "Cache";
    this.ti_sysbios_family_arm_arm9_Cache.moduleName = "ti.sysbios.family.arm.arm9.Cache";
    this.ti_sysbios_family_arm_arm9_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_arm9_Mmu.elemName = "Arm9 MMU";
    this.ti_sysbios_family_arm_arm9_Mmu.moduleName = "ti.sysbios.family.arm.Arm9.Mmu";
    this.ti_sysbios_family_arm_arm9_Mmu.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_dm6446_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_arm_dm6446_Hwi.moduleName = "ti.sysbios.family.arm.dm6446.Hwi";
    this.ti_sysbios_family_arm_dm6446_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_da830_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_arm_da830_Hwi.moduleName = "ti.sysbios.family.arm.da830.Hwi";
    this.ti_sysbios_family_arm_da830_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_da830_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_da830_TimestampProvider.moduleName = "ti.sysbios.family.arm.da830.TimestampProvider";
    this.ti_sysbios_family_arm_da830_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_v7a_Pmu.elemName = "Performance Monitor Unit";
    this.ti_sysbios_family_arm_v7a_Pmu.moduleName = "ti.sysbios.family.arm.v7a.Pmu";
    this.ti_sysbios_family_arm_v7a_Pmu.iconFileName = "icons/timer.ico";

    this.ti_sysbios_family_arm_a8_Cache.elemName = "Cache";
    this.ti_sysbios_family_arm_a8_Cache.moduleName = "ti.sysbios.family.arm.a8.Cache";
    this.ti_sysbios_family_arm_a8_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_a8_Mmu.elemName = "A8 MMU";
    this.ti_sysbios_family_arm_a8_Mmu.moduleName = "ti.sysbios.family.arm.a8.Mmu";
    this.ti_sysbios_family_arm_a8_Mmu.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_a8_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_a8_TimestampProvider.moduleName = "ti.sysbios.family.arm.a8.TimestampProvider";
    this.ti_sysbios_family_arm_a8_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_a8_intcps_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_arm_a8_intcps_Hwi.moduleName = "ti.sysbios.family.arm.a8.intcps.Hwi";
    this.ti_sysbios_family_arm_a8_intcps_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_a9_Cache.elemName = "Cache";
    this.ti_sysbios_family_arm_a9_Cache.moduleName = "ti.sysbios.family.arm.a9.Cache";
    this.ti_sysbios_family_arm_a9_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_a9_Timer.elemName = "A9 Private Timer";
    this.ti_sysbios_family_arm_a9_Timer.moduleName = "ti.sysbios.family.arm.a9.Timer";
    this.ti_sysbios_family_arm_a9_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_a15_Cache.elemName = "Cache";
    this.ti_sysbios_family_arm_a15_Cache.moduleName = "ti.sysbios.family.arm.a15.Cache";
    this.ti_sysbios_family_arm_a15_Cache.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_a15_Mmu.elemName = "A15 MMU";
    this.ti_sysbios_family_arm_a15_Mmu.moduleName = "ti.sysbios.family.arm.a15.Mmu";
    this.ti_sysbios_family_arm_a15_Mmu.iconFileName = "icons/mem.ico";

    this.ti_sysbios_family_arm_a15_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_a15_TimestampProvider.moduleName = "ti.sysbios.family.arm.a15.TimestampProvider";
    this.ti_sysbios_family_arm_a15_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_a15_tci66xx_CpIntc.elemName = "CpIntc";
    this.ti_sysbios_family_arm_a15_tci66xx_CpIntc.moduleName = "ti.sysbios.family.arm.a15.tci66xx.CpIntc";
    this.ti_sysbios_family_arm_a15_tci66xx_CpIntc.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_gic_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_arm_gic_Hwi.moduleName = "ti.sysbios.family.arm.gic.Hwi";
    this.ti_sysbios_family_arm_gic_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_systimer_Timer.elemName = "Timer";
    this.ti_sysbios_family_arm_systimer_Timer.moduleName = "ti.sysbios.family.arm.systimer.Timer";
    this.ti_sysbios_family_arm_systimer_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_exc_Exception.elemName = "Exception";
    this.ti_sysbios_family_arm_exc_Exception.moduleName = "ti.sysbios.family.arm.exc.Exception";
    this.ti_sysbios_family_arm_exc_Exception.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_m3_Hwi.elemName = "Hwi";
    this.ti_sysbios_family_arm_m3_Hwi.moduleName = "ti.sysbios.family.arm.m3.Hwi";
    this.ti_sysbios_family_arm_m3_Hwi.iconFileName = "icons/hwi.ico";

    this.ti_sysbios_family_arm_m3_Timer.elemName = "Timer";
    this.ti_sysbios_family_arm_m3_Timer.moduleName = "ti.sysbios.family.arm.m3.Timer";
    this.ti_sysbios_family_arm_m3_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_m3_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_m3_TimestampProvider.moduleName = "ti.sysbios.family.arm.m3.TimestampProvider";
    this.ti_sysbios_family_arm_m3_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_lm3_Timer.elemName = "Timer";
    this.ti_sysbios_family_arm_lm3_Timer.moduleName = "ti.sysbios.family.arm.lm3.Timer";
    this.ti_sysbios_family_arm_lm3_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_lm3_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_lm3_TimestampProvider.moduleName = "ti.sysbios.family.arm.lm3.TimestampProvider";
    this.ti_sysbios_family_arm_lm3_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_lm4_Timer.elemName = "Timer";
    this.ti_sysbios_family_arm_lm4_Timer.moduleName = "ti.sysbios.family.arm.lm4.Timer";
    this.ti_sysbios_family_arm_lm4_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_lm4_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_lm4_TimestampProvider.moduleName = "ti.sysbios.family.arm.lm4.TimestampProvider";
    this.ti_sysbios_family_arm_lm4_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_ducati_Core.elemName = "Core";
    this.ti_sysbios_family_arm_ducati_Core.moduleName = "ti.sysbios.family.arm.ducati.Core";
    this.ti_sysbios_family_arm_ducati_Core.iconFileName = "icons/prj.ico";

    this.ti_sysbios_family_arm_ducati_Timer.elemName = "Timer";
    this.ti_sysbios_family_arm_ducati_Timer.moduleName = "ti.sysbios.family.arm.ducati.Timer";
    this.ti_sysbios_family_arm_ducati_Timer.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_ducati_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_ducati_TimestampProvider.moduleName = "ti.sysbios.family.arm.ducati.TimestampProvider";
    this.ti_sysbios_family_arm_ducati_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_f28m35x_TimestampProvider.elemName = "TimestampProvider";
    this.ti_sysbios_family_arm_f28m35x_TimestampProvider.moduleName = "ti.sysbios.family.arm.f28m35x.TimestampProvider";
    this.ti_sysbios_family_arm_f28m35x_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_cc26xx_TimestampProvider.elemName = "RTC TimestampProvider";
    this.ti_sysbios_family_arm_cc26xx_TimestampProvider.moduleName = "ti.sysbios.family.arm.cc26xx.TimestampProvider";
    this.ti_sysbios_family_arm_cc26xx_TimestampProvider.iconFileName = "icons/clk.ico";

    this.ti_sysbios_family_arm_cc26xx_Boot.elemName = "Boot";
    this.ti_sysbios_family_arm_cc26xx_Boot.moduleName = "ti.sysbios.family.arm.cc26xx.Boot";
    this.ti_sysbios_family_arm_cc26xx_Boot.iconFileName = "icons/ACCEL.ico";

    this.ti_sysbios_family_arm_cc26xx_Alarm.elemName = "Alarm";
    this.ti_sysbios_family_arm_cc26xx_Alarm.moduleName = "ti.sysbios.family.arm.cc26xx.Alarm";
    this.ti_sysbios_family_arm_cc26xx_Alarm.iconFileName = "icons/rtc.ico";

    this.ti_sysbios_family_arm_cc26xx_Seconds.elemName = "Seconds";
    this.ti_sysbios_family_arm_cc26xx_Seconds.moduleName = "ti.sysbios.family.arm.cc26xx.Seconds";
    this.ti_sysbios_family_arm_cc26xx_Seconds.iconFileName = "icons/clk.ico";

/* Sub Groups */

    this.systemProvidersGroup.elemName = "System Providers";
    this.systemProvidersGroup.moduleName = null;
    this.systemProvidersGroup.iconFileName = "icons/hst.ico";
    this.systemProvidersGroup.elemArray = [
        this.xdc_runtime_SysMin,
        this.xdc_runtime_SysStd
    ];

    this.loggersGroup.elemName = "Loggers";
    this.loggersGroup.moduleName = null;
    this.loggersGroup.iconFileName = "icons/log.ico";
    this.loggersGroup.elemArray = [
        this.xdc_runtime_LoggerBuf,
        this.xdc_runtime_LoggerSys
    ];

    this.rtaGroup.elemName = "Realtime Analysis";
    this.rtaGroup.moduleName = null;
    this.rtaGroup.iconFileName = "icons/log.ico";
    this.rtaGroup.elemArray = [
        this.ti_sysbios_utils_Load
    ];

    this.heapsGroup.elemName = "Heaps";
    this.heapsGroup.moduleName = null;
    this.heapsGroup.iconFileName = "icons/mem.ico";
    this.heapsGroup.elemArray = [
        this.xdc_runtime_HeapMin,
        this.ti_sysbios_heaps_HeapBuf,
        this.ti_sysbios_heaps_HeapCallback,
        this.ti_sysbios_heaps_HeapMem,
        this.ti_sysbios_heaps_HeapMultiBuf,
        this.ti_sysbios_heaps_HeapNull,
        this.ti_sysbios_heaps_HeapTrack
    ];

    this.syncsGroup.elemName = "Syncs";
    this.syncsGroup.moduleName = null;
    this.syncsGroup.iconFileName = "icons/sem.ico";
    this.syncsGroup.elemArray = [
        this.xdc_runtime_knl_SyncGeneric,
        this.ti_sysbios_syncs_SyncEvent,
        this.ti_sysbios_syncs_SyncSem,
        this.ti_sysbios_syncs_SyncSwi
    ];

    this.gatesGroup.elemName = "Gates";
    this.gatesGroup.moduleName = null;
    this.gatesGroup.iconFileName = "icons/lck.ico";
    this.gatesGroup.elemArray = [
        this.ti_sysbios_gates_GateAll,
        this.ti_sysbios_gates_GateHwi,
        this.ti_sysbios_gates_GateMutex,
        this.ti_sysbios_gates_GateMutexPri,
        this.ti_sysbios_gates_GateSwi,
        this.ti_sysbios_gates_GateTask
    ];

/* Groups */

    this.systemGroup.elemName = "System";
    this.systemGroup.moduleName = null;
    this.systemGroup.iconFileName = "icons/gbl.ico";
    this.systemGroup.elemArray = [
        this.xdc_cfg_Program,
        this.xdc_runtime_Main,
        this.xdc_runtime_Defaults,
        this.xdc_runtime_System,
        this.systemProvidersGroup,
        this.xdc_runtime_Startup
    ];

    this.diagnosticsGroup.elemName = "Diagnostics";
    this.diagnosticsGroup.moduleName = null;
    this.diagnosticsGroup.iconFileName = "icons/sdb.ico";
    this.diagnosticsGroup.elemArray = [
        this.xdc_runtime_Log,        
        this.loggersGroup,
        this.xdc_runtime_Diags,
        this.xdc_runtime_Assert,
        this.xdc_runtime_Error,
        this.xdc_runtime_Timestamp
    ];

    this.memoryGroup.elemName = "Memory Management";
    this.memoryGroup.moduleName = null;
    this.memoryGroup.iconFileName = "icons/mem.ico";
    /* elemArray is modified to add Cache as necessary below */
    this.memoryGroup.elemArray = [
        this.xdc_runtime_Memory,
        this.heapsGroup
    ];
    
    this.schedulingGroup.elemName = "Scheduling";
    this.schedulingGroup.moduleName = null;
    this.schedulingGroup.iconFileName = "icons/swi.ico";
    this.schedulingGroup.elemArray = [
        this.ti_sysbios_knl_Task,
        this.ti_sysbios_knl_Swi,
        this.ti_sysbios_hal_Hwi,
        this.ti_sysbios_knl_Idle,
        this.ti_sysbios_knl_Clock,
        this.ti_sysbios_hal_Seconds,
        this.ti_sysbios_hal_Timer

    ];

    this.synchronizationGroup.elemName = "Synchronization";
    this.synchronizationGroup.moduleName = null;
    this.synchronizationGroup.iconFileName = "icons/sem.ico";
    this.synchronizationGroup.elemArray = [
        this.ti_sysbios_knl_Semaphore,
        this.ti_sysbios_knl_Event,
        this.ti_sysbios_knl_Mailbox,
        this.ti_sysbios_knl_Queue,
        this.gatesGroup,
        this.syncsGroup
    ];

    this.ioGroup.elemName = "I/O";
    this.ioGroup.moduleName = null;
    this.ioGroup.iconFileName = "icons/dev.ico";
    this.ioGroup.elemArray = [
        this.ti_sysbios_io_DEV,
        this.ti_sysbios_io_GIO
    ];

    this.timersGroup.elemName = "Timers";
    this.timersGroup.moduleName = null;
    this.timersGroup.iconFileName = "icons/rtc.ico";
    this.timersGroup.elemArray = [
        this.ti_sysbios_timers_dmtimer_Timer,
        this.ti_sysbios_timers_gptimer_Timer,
        this.ti_sysbios_timers_timer64_Timer
    ];

    this.c28Group.elemName = "C28";
    this.c28Group.moduleName = null;
    this.c28Group.iconFileName = "icons/csl_fldr.ico";
    this.c28Group.elemArray = [
        this.ti_sysbios_family_c28_Hwi,
        this.ti_sysbios_family_c28_Timer,
        this.ti_sysbios_family_c28_TimestampProvider,
        this.ti_sysbios_family_c28_f28m35x_TimestampProvider
    ];

    this.c64pGroup.elemName = "C64P";
    this.c64pGroup.moduleName = null;
    this.c64pGroup.iconFileName = "icons/csl_fldr.ico";
    this.c64pGroup.elemArray = [
        this.ti_sysbios_family_c64p_Hwi,
        this.ti_sysbios_family_c64p_Cache,
        this.ti_sysbios_family_c64p_MemoryProtect,
        this.timersGroup,
        this.ti_sysbios_family_c64p_EventCombiner,
        this.ti_sysbios_family_c64p_Exception
    ];

    this.c64tGroup.elemName = "C64T";
    this.c64tGroup.moduleName = null;
    this.c64tGroup.iconFileName = "icons/csl_fldr.ico";
    this.c64tGroup.elemArray = [
        this.ti_sysbios_family_c64p_Hwi,
        this.ti_sysbios_hal_unicache_Cache,
        this.ti_sysbios_timers_dmtimer_Timer,
        this.ti_sysbios_family_c64p_EventCombiner,
        this.ti_sysbios_family_c64p_Exception
    ];

    this.c66Group.elemName = "C66";
    this.c66Group.moduleName = null;
    this.c66Group.iconFileName = "icons/csl_fldr.ico";
    this.c66Group.elemArray = [
        this.ti_sysbios_family_c64p_Hwi,
        this.ti_sysbios_family_c66_tci66xx_CpIntc,
        this.ti_sysbios_family_c66_Cache,
        this.ti_sysbios_family_c64p_MemoryProtect,
        this.timersGroup,
        this.ti_sysbios_family_c64p_EventCombiner,
        this.ti_sysbios_family_c64p_Exception
    ];

    this.c674Group.elemName = "C674";
    this.c674Group.moduleName = null;
    this.c674Group.iconFileName = "icons/csl_fldr.ico";
    this.c674Group.elemArray = [
        this.ti_sysbios_family_c64p_Hwi,
        this.ti_sysbios_family_c64p_Cache,
        this.ti_sysbios_family_c64p_MemoryProtect,
        this.timersGroup,
        this.ti_sysbios_family_c64p_EventCombiner,
        this.ti_sysbios_family_c64p_Exception,
        this.ti_sysbios_family_c674_Power
    ];

    this.msp430Group.elemName = "MSP430";
    this.msp430Group.moduleName = null;
    this.msp430Group.iconFileName = "icons/csl_fldr.ico";
    this.msp430Group.elemArray = [
        this.ti_sysbios_family_msp430_Hwi,
        this.ti_sysbios_family_msp430_Power,
        this.ti_sysbios_family_msp430_Timer,
        this.ti_sysbios_family_msp430_TimestampProvider,
        this.ti_sysbios_family_msp430_ClockFreqs
    ];

    this.arm9davinciGroup.elemName = "Arm9 (daVinci)";
    this.arm9davinciGroup.moduleName = null;
    this.arm9davinciGroup.iconFileName = "icons/csl_fldr.ico";
    this.arm9davinciGroup.elemArray = [
        this.ti_sysbios_family_arm_arm9_Cache,
        this.ti_sysbios_family_arm_arm9_Mmu,
        this.ti_sysbios_family_arm_dm6446_Hwi,
        this.ti_sysbios_family_arm_exc_Exception,
        this.ti_sysbios_timers_timer64_Timer,
        this.ti_sysbios_timers_timer64_TimestampProvider
    ];

    this.arm9da830Group.elemName = "Arm9 (DA830)";
    this.arm9da830Group.moduleName = null;
    this.arm9da830Group.iconFileName = "icons/csl_fldr.ico";
    this.arm9da830Group.elemArray = [
        this.ti_sysbios_family_arm_arm9_Cache,
        this.ti_sysbios_family_arm_arm9_Mmu,
        this.ti_sysbios_family_arm_da830_Hwi,
        this.ti_sysbios_family_arm_exc_Exception,
        this.ti_sysbios_timers_timer64_Timer,
        this.ti_sysbios_family_arm_da830_TimestampProvider
    ];

    this.a8Group.elemName = "A8";
    this.a8Group.moduleName = null;
    this.a8Group.iconFileName = "icons/csl_fldr.ico";
    this.a8Group.elemArray = [
        this.ti_sysbios_family_arm_a8_Cache,
        this.ti_sysbios_family_arm_a8_Mmu,
        this.ti_sysbios_family_arm_a8_intcps_Hwi,
        this.ti_sysbios_family_arm_exc_Exception,
        this.ti_sysbios_timers_dmtimer_Timer,
        this.ti_sysbios_timers_gptimer_Timer,
        this.ti_sysbios_family_arm_a8_TimestampProvider,
        this.ti_sysbios_family_arm_v7a_Pmu
    ];

    this.a9Group.elemName = "A9";
    this.a9Group.moduleName = null;
    this.a9Group.iconFileName = "icons/csl_fldr.ico";
    this.a9Group.elemArray = [
        this.ti_sysbios_family_arm_a9_Cache,
        this.ti_sysbios_family_arm_a8_Mmu,
        this.ti_sysbios_family_arm_gic_Hwi,
        this.ti_sysbios_family_arm_exc_Exception,
        this.ti_sysbios_family_arm_a9_Timer,
        this.ti_sysbios_timers_dmtimer_Timer,
        this.ti_sysbios_timers_gptimer_Timer,
        this.ti_sysbios_family_arm_a15_TimestampProvider,
        this.ti_sysbios_family_arm_v7a_Pmu,
        this.ti_sysbios_rts_gnu_SemiHostSupport
    ];

    this.a15Group.elemName = "A15";
    this.a15Group.moduleName = null;
    this.a15Group.iconFileName = "icons/csl_fldr.ico";
    this.a15Group.elemArray = [
        this.ti_sysbios_family_arm_a15_Cache,
        this.ti_sysbios_family_arm_a15_Mmu,
        this.ti_sysbios_family_arm_a15_tci66xx_CpIntc,
        this.ti_sysbios_family_arm_gic_Hwi,
        this.ti_sysbios_family_arm_systimer_Timer,
        this.ti_sysbios_family_arm_exc_Exception,
        this.ti_sysbios_timers_dmtimer_Timer,
        this.ti_sysbios_family_arm_a15_TimestampProvider,
        this.ti_sysbios_family_arm_v7a_Pmu,
        this.ti_sysbios_rts_gnu_SemiHostSupport
    ];

    this.m3Group.elemName = "M3 (Native)";
    this.m3Group.moduleName = null;
    this.m3Group.iconFileName = "icons/csl_fldr.ico";
    this.m3Group.elemArray = [
        this.ti_sysbios_family_arm_m3_Hwi,
        this.ti_sysbios_family_arm_m3_Timer,
        this.ti_sysbios_family_arm_m3_TimestampProvider
    ];

    this.lm3Group.elemName = "M3 (Stellaris and Concerto)";
    this.lm3Group.moduleName = null;
    this.lm3Group.iconFileName = "icons/csl_fldr.ico";
    this.lm3Group.elemArray = [
        this.ti_sysbios_family_arm_m3_Hwi,
        this.ti_sysbios_family_arm_lm3_Timer,
        this.ti_sysbios_family_arm_lm3_TimestampProvider,
        this.ti_sysbios_family_arm_f28m35x_TimestampProvider
    ];

    this.cc26xxGroup.elemName = "M3 (CC26xx)";
    this.cc26xxGroup.moduleName = null;
    this.cc26xxGroup.iconFileName = "icons/csl_fldr.ico";
    this.cc26xxGroup.elemArray = [
        this.ti_sysbios_rom_ROM,
        this.ti_sysbios_family_arm_m3_Hwi,
        this.ti_sysbios_family_arm_cc26xx_Boot,
        this.ti_sysbios_family_arm_cc26xx_Alarm,
        this.ti_sysbios_family_arm_cc26xx_Seconds,
        this.ti_sysbios_family_arm_cc26xx_TimestampProvider
    ];

    this.lm4Group.elemName = "M4 (Tiva)";
    this.lm4Group.moduleName = null;
    this.lm4Group.iconFileName = "icons/csl_fldr.ico";
    this.lm4Group.elemArray = [
        this.ti_sysbios_family_arm_m3_Hwi,
        this.ti_sysbios_family_arm_lm4_Timer,
        this.ti_sysbios_family_arm_lm4_TimestampProvider
    ];

    this.ducatiGroup.elemName = "M3 (Ducati)";
    this.ducatiGroup.moduleName = null;
    this.ducatiGroup.iconFileName = "icons/csl_fldr.ico";
    this.ducatiGroup.elemArray = [
        this.ti_sysbios_family_arm_m3_Hwi,
        this.ti_sysbios_family_arm_ducati_Timer,
        this.ti_sysbios_family_arm_ducati_TimestampProvider,
        this.ti_sysbios_family_arm_ducati_Core,
        this.ti_sysbios_hal_unicache_Cache,
        this.ti_sysbios_hal_ammu_AMMU
    ];

    /* elemArray populated in getProductDescriptor() */
    this.armGroup.elemName = "Arm";
    this.armGroup.moduleName = null;
    this.armGroup.iconFileName = "icons/csl_fldr.ico";
    this.armGroup.elemArray = [];

    this.targetGroup.elemName = "Target Specific Support";
    this.targetGroup.moduleName = null;
    this.targetGroup.iconFileName = "icons/csl_fldr.ico";
    this.targetGroup.elemArray = [
        this.armGroup,
        this.c28Group,
        this.c64pGroup,
        this.c66Group,
        this.c674Group,
        this.c64tGroup,
        this.msp430Group
    ];

/* product */

    /* xgonf "home" module must be first module at top-level */
    this.ti_sysbios_BIOS.elemName = "BIOS";
    this.ti_sysbios_BIOS.moduleName = "ti.sysbios.BIOS";
    this.ti_sysbios_BIOS.iconFileName = "icons/gbl.ico";

    this.biosProduct.elemName = "SYS/BIOS";
    this.biosProduct.moduleName = null;
    this.biosProduct.iconFileName = "icons/bios.gif";
    this.biosProduct.elemArray = [
        this.ti_sysbios_BIOS,
        this.systemGroup,
        this.diagnosticsGroup,
        this.schedulingGroup,
        this.synchronizationGroup,
        this.ioGroup
    ]; 
}

/*
 *  ======== getProductDescriptor ========
 */
function getProductDescriptor() 
{
    var Settings = xdc.module("ti.sysbios.family.Settings");

    /* if the group name exists for the family, only show one group */
    var groupName = Settings.getFamilyName() + "Group";
    if (groupName in this) {
        this.targetGroup.elemArray = [
            this[groupName]
        ];
    }

    if (groupName == "c64pGroup") {
        /* "c64p" family represents _any_ C6x device (sigh) */
        this.targetGroup.elemArray = [
            this.c64pGroup,
            this.c66Group,
            this.c674Group,
            this.c64tGroup
        ];
    }

    /* add Cache unless we are an MSP or a C28 */
    if (groupName != "msp430Group" && groupName != "c28Group") {
        // print("adding Cache to this view ...");
        this.memoryGroup.elemArray.$add(this.ti_sysbios_hal_Cache);
    }
    else {
        // print("excluding Cache from this view ...");
    }

    if (groupName == "armGroup") {
        this.armGroup.elemArray = [];

        /* Add SemiHostingSupport module to list only for GNU targets */
        if (Program.build.target.$name.match(/gnu/)) {
            this.a8Group.elemArray.$add(this.ti_sysbios_rts_gnu_SemiHostSupport);
            this.m3Group.elemArray.$add(this.ti_sysbios_rts_gnu_SemiHostSupport);
            this.lm3Group.elemArray.$add(this.ti_sysbios_rts_gnu_SemiHostSupport);
            this.lm4Group.elemArray.$add(this.ti_sysbios_rts_gnu_SemiHostSupport);
            this.cc26xxGroup.elemArray.$add(this.ti_sysbios_rts_gnu_SemiHostSupport);
        }

        switch (Program.build.target.name) {
            case "Arm9":
            case "Arm9t":
                this.armGroup.elemArray.$add(this.arm9davinciGroup);
                this.armGroup.elemArray.$add(this.arm9da830Group);
                break;
            case "A8F":
            case "A8Ft":
            case "A8Fv":
            case "A8Fnv":
                this.armGroup.elemArray.$add(this.a8Group);
                break;
            case "A9F":
                this.armGroup.elemArray.$add(this.a9Group);
                break;
            case "A15F":
                this.armGroup.elemArray.$add(this.a15Group);
                break;
            case "M3":
                if (Program.platform.$name.match(/ti\.platforms\.simplelink/)) {
                    this.armGroup.elemArray.$add(this.cc26xxGroup);
                }
                else {
                    this.armGroup.elemArray.$add(this.m3Group);
                    this.armGroup.elemArray.$add(this.lm3Group);
                    if (!Program.build.target.$name.match(/gnu/)) {
                        this.armGroup.elemArray.$add(this.ducatiGroup);
                    }
                }
                break;
            case "M4":
                if (!Program.build.target.$name.match(/gnu/)) {
                    this.armGroup.elemArray.$add(this.ducatiGroup);
                }
            case "M4F":
                this.armGroup.elemArray.$add(this.lm4Group);
                break;
            default:
                /*
                 * Cannot recognize target. Populate armGroup with all known
                 * target modules.
                 */
                this.armGroup.elemArray = [
                    this.arm9davinciGroup,
                    this.arm9da830Group,
                    this.a8Group,
                    this.a9Group,
                    this.a15Group,
                    this.m3Group,
                    this.lm3Group,
                    this.lm4Group,
                    this.ducatiGroup,
                    this.cc26xxGroup
                ];
        }

        this.targetGroup.elemArray = [
            this.armGroup
        ];
    }

    this.biosProduct.elemArray.$add(this.targetGroup);
    this.biosProduct.elemArray.$add(this.memoryGroup);
    this.biosProduct.elemArray.$add(this.rtaGroup);

    return this.biosProduct;
}
