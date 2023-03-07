/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 * */
/*
 *  ======== UiaProductView.xs ========
 *
 */
function module$meta$init()
{
    this.ti_uia_sysbios_LoggerStreamer.elemName = "LoggerStreamer";
    this.ti_uia_sysbios_LoggerStreamer.moduleName = "ti.uia.sysbios.LoggerStreamer";
    this.ti_uia_sysbios_LoggerStreamer.iconFileName = "icons/world.png";

    this.ti_uia_sysbios_LoggerStreamer2.elemName = "LoggerStreamer2";
    this.ti_uia_sysbios_LoggerStreamer2.moduleName = "ti.uia.sysbios.LoggerStreamer2";
    this.ti_uia_sysbios_LoggerStreamer2.iconFileName = "icons/world.png";

    this.ti_uia_sysbios_LoggerIdle.elemName = "LoggerIdle";
    this.ti_uia_sysbios_LoggerIdle.moduleName = "ti.uia.sysbios.LoggerIdle";
    this.ti_uia_sysbios_LoggerIdle.iconFileName = "icons/world.png";

    this.ti_uia_sysbios_LoggingSetup.elemName = "LoggingSetup";
    this.ti_uia_sysbios_LoggingSetup.moduleName = "ti.uia.sysbios.LoggingSetup";
    this.ti_uia_sysbios_LoggingSetup.iconFileName = "icons/world.png";

    this.ti_uia_services_Rta.elemName = "Rta";
    this.ti_uia_services_Rta.moduleName = "ti.uia.services.Rta";
    this.ti_uia_services_Rta.iconFileName = "icons/world.png";

    this.ti_uia_loggers_LoggerRunMode.elemName = "LoggerRunMode";
    this.ti_uia_loggers_LoggerRunMode.moduleName = "ti.uia.loggers.LoggerRunMode";
    this.ti_uia_loggers_LoggerRunMode.iconFileName = "icons/world.png";

    this.ti_uia_loggers_LoggerStopMode.elemName = "LoggerStopMode";
    this.ti_uia_loggers_LoggerStopMode.moduleName = "ti.uia.loggers.LoggerStopMode";
    this.ti_uia_loggers_LoggerStopMode.iconFileName = "icons/world.png";

    this.ti_uia_loggers_LoggerMin.elemName = "LoggerMin";
    this.ti_uia_loggers_LoggerMin.moduleName = "ti.uia.loggers.LoggerMin";
    this.ti_uia_loggers_LoggerMin.iconFileName = "icons/world.png";

    this.ti_uia_runtime_LoggerSM.elemName = "LoggerSM";
    this.ti_uia_runtime_LoggerSM.moduleName = "ti.uia.runtime.LoggerSM";
    this.ti_uia_runtime_LoggerSM.iconFileName = "icons/world.png";

    this.ti_uia_runtime_ServiceMgr.elemName = "ServiceMgr";
    this.ti_uia_runtime_ServiceMgr.moduleName = "ti.uia.runtime.ServiceMgr";
    this.ti_uia_runtime_ServiceMgr.iconFileName = "icons/world.png";

    /* loggers */
    this.loggerGroup.elemName = "Loggers";
    this.loggerGroup.moduleName = null;
    this.loggerGroup.iconFileName = "icons/bios.gif";
    this.loggerGroup.elemArray = [
        this.ti_uia_sysbios_LoggerStreamer,
        this.ti_uia_sysbios_LoggerStreamer2,
        this.ti_uia_sysbios_LoggerIdle,
        this.ti_uia_loggers_LoggerRunMode,
        this.ti_uia_loggers_LoggerStopMode,
        this.ti_uia_loggers_LoggerMin,
        this.ti_uia_runtime_LoggerSM
    ];

    /* services */
    this.serviceGroup.elemName = "Services";
    this.serviceGroup.moduleName = null;
    this.serviceGroup.iconFileName = "icons/bios.gif";
    this.serviceGroup.elemArray = [
        this.ti_uia_services_Rta,
        this.ti_uia_runtime_ServiceMgr
    ];

    /* product */
    this.uiaProduct.elemName = "UIA";
    this.uiaProduct.moduleName = null;
    this.uiaProduct.iconFileName = "icons/bios.gif";
    this.uiaProduct.elemArray = [
        this.loggerGroup,
        this.serviceGroup,
        this.ti_uia_sysbios_LoggingSetup
    ];
}

/*
 *  ======== getProductDescriptor ========
 */
function getProductDescriptor()
{
    return this.uiaProduct;
}
