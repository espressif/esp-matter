/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  ======== Hwi.rov.js ========
 */

var moduleName = "ti.sysbios.family.arm.v6m::Hwi";

var viewMap = [
    {name: "Basic", fxn: "viewHwiBasicInfo", structName: "BasicInfo"},
    {name: "Detailed", fxn: "viewHwiDetailedInfo", structName: "DetailedInfo"},
    {name: "Module", fxn: "viewHwiModuleInfo", structName: "ModuleInfo"},
    {name: "Exception Info", fxn: "viewExceptionInfo", structName: "ExceptionInfo"},
    {name: "Exception Call Stack", fxn: "viewCallStack", structName: "CallStackFrame"}
];

/*
 * Hwi Stack Information
 */
function StackInfo()
{
    this.hwiStackBase = null;
    this.hwiStackSize = 0;
    this.hwiStackPeak = 0;
}

/*
 * Hwi Module Information
 */
function ModuleInfo()
{
    this.options = null;
    this.processorState = null;
    this.activeInterrupt = 0;
    this.pendingInterrupt = 0;
    this.exception = null;
    this.hwiStackBase = null;
    this.hwiStackSize = 0;
    this.hwiStackPeak = 0;
}

/*
 * Hwi Object Information
 */
function BasicInfo()
{
    this.handle = null;
    this.name = null;
    this.type = null;
    this.intNum = 0;
    this.priority = null;
    this.group = 0;
    this.subPriority = 0;
    this.fxn = null;
    this.arg = null;
}

/*
 * Hwi Detailed Information
 */
function DetailedInfo()
{
    this.handle = null;
    this.name = null;
    this.type = null;
    this.intNum = 0;
    this.priority = null;
    this.group = 0;
    this.subPriority = 0;
    this.fxn = null;
    this.arg = null;
    this.irp = null;
    this.status = null;
}

/*
 * CallStackFrame constructor
 */
function CallStackFrame()
{
    this.frame = null;
}

/*
 * ExceptionInfo constructor
 */
function ExceptionInfo()
{
    this.exceptionInfo = null;
}

/*
 * ======== toHexString ========
 * converts a number to a hex string with "0x" prefix.
 */
function toHexString(num) {
    return ("0x" + Number(num).toString(16));
}

/*
 * ======== toPaddedHexString ========
 * converts a number to a hex string with "0x" prefix.
 * inserts up to (len - 1) leading zeros.
 * up to a max of 7 zeros.
 */
function toPaddedHexString(number, len)
{
    return ("0x" + ("0000000" + number.toString(16)).substr(-len));
}

/* used by ROV view Code */
var subPriMasks = [0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff];

/* used by ROV view Code */
var numPriTable = {
    "2" : {
        mask : 0x80,
        shift : 7
    },
    "4" : {
        mask : 0xc0,
        shift : 6
    },
    "8" : {
        mask : 0xe0,
        shift : 5
    },
    "16" : {
        mask : 0xf0,
        shift : 4
    },
    "32" : {
        mask : 0xf8,
        shift : 3
    },
    "64" : {
        mask : 0xfc,
        shift : 2
    },
    "128" : {
        mask : 0xfe,
        shift : 1
    },
    "256" : {
        mask : 0xff,
        shift : 0
    }
}

/*
 *  ======== viewGetPriority ========
 */
function viewGetPriority(view, intNum)
{
    var priority = 0;
    var registerBaseAddr;

    try {
        registerBaseAddr = 0xe000e400;
        var IPR = Program.fetchFromAddr(registerBaseAddr, "uint32_t", 60);
        registerBaseAddr = 0xe000ed18;
        var SHPR = Program.fetchFromAddr(registerBaseAddr, "uint32_t", 3);
        registerBaseAddr = 0xe000ed0c;
        var AIRCR = Program.fetchFromAddr(registerBaseAddr, "uint32_t", 1);
    }
    catch (e) {
        print("Error: Problem fetching priorities: " + e.toString());

        Program.displayError(view, "priority",  "Unable to read Hwi priority " +
            "registers at 0x" + registerBaseAddr.toString(16));
    }

    if (intNum >= 16) {
        priority = IPR[(intNum-16)/4] >> (intNum & 0x3);
    }
    else if (intNum >= 4) {
        priority = SHPR[(intNum-4)/4] >> (intNum & 0x3);
    }

    view.priority = toHexString(priority);

    var mask = numPriTable[8].mask;

    var shift = numPriTable[8].shift;
    
    var priGroup = (AIRCR & 0x0000ff00) >> 8;
    
    if (priGroup + 1 > shift) {
        view.group = priority >> (priGroup + 1);
    }
    else {
        view.group = priority >> shift;
    }
}

/*
 *  ======== viewNvicFetch ========
 *  Called from viewInitModule()
 */
function viewNvicFetch(that)
{
    try {
        that.ISER = Program.fetchFromAddr(0xe000e100, "uint32_t", 8);
        that.ISPR = Program.fetchFromAddr(0xe000e200, "uint32_t", 8);
        that.IABR = Program.fetchFromAddr(0xe000e300, "uint32_t", 8);
        that.ICSR = Program.fetchFromAddr(0xe000ed04, "uint32_t", 1);
        that.STCSR = Program.fetchFromAddr(0xe000e010, "uint32_t", 1);
        that.SHCSR = Program.fetchFromAddr(0xe000ed24, "uint32_t", 1);
        that.VTOR = Program.fetchFromAddr(0xe000ed08, "uint32_t", 1);
    }
    catch (e) {
        print("Error: Problem fetching NVIC: " + e.toString());
    }
}

/*
 * ======== viewGetHwiStatus ========
 */
function viewGetHwiStatus(view)
{
    viewNvicFetch(this);

    var enabled = false;
    var active = false;
    var pending = false;

    if (view.intNum >= 16) {
        var index = (view.intNum-16) >> 5;
        var mask = 1 << ((view.intNum-16) & 0x1f);
        enabled = this.ISER[index] & mask;
        active = this.IABR[index] & mask;
        pending = this.ISPR[index] & mask;
    }
    else {
        switch(view.intNum) {
            case 15: /* SysTick */
                pending = this.ICSR & 0x100000000;
                enabled = this.STCSR & 0x00000002;
                active = this.SHCSR & 0x00000800;
                break;
            default:
                view.status = "unknown";
                return;
                break;
        }
    }

    if (enabled) {
        view.status = "Enabled";
    }
    else {
        view.status = "Disabled";
    }

    if (active) {
        view.status += ", Active";
    }

    if (pending) {
        view.status += ", Pending";
    }
}

/*
 * ======== viewGetHwiType ========
 */
function viewGetHwiType(view, obj)
{
    if (obj.intNum >= 0) {
        view.type = "Dispatched";
    }
    else {
        if (view.priority < 0x20) {
            view.type = "Zero Latency";
        }
        else {
            view.type = "Non Dispatched";
        }
    }
}

/*
 *  ======== viewGetHwiObjectArray ========
 */
function viewGetHwiObjectArray()
{
    hwiArray = new Array();

    var dispatchTable = Program.fetchVariable("ti_sysbios_family_arm_v6m_Hwi_dispatchTable");
    var numInts = dispatchTable.length;
    var dispatcherAddress = Program.lookupSymbolValue("ti_sysbios_family_arm_v6m_Hwi_dispatch");

    for (var i = 0; i < numInts; i++) {
        if (Number(dispatchTable[i]) != 0) {
            var hwi = new Object();
            hwiAddr = dispatchTable[i];
            hwi = Program.fetchFromAddr(hwiAddr, "ti_sysbios_family_arm_v6m_Hwi_Struct", 1);
            hwi.$addr = hwiAddr;
            hwiArray.push(hwi);
        }
    }

    return (hwiArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.handle = toHexString(obj.$addr);
    view.name = String(Program.lookupDataSymbol(Number(obj.$addr)));
    view.fxn = String(Program.lookupFuncName(Number(obj.fxn)));
    view.arg = toHexString(obj.arg);
    view.intNum = obj.intNum;
    viewGetPriority(view, obj.intNum);
    viewGetHwiType(view, obj);
}

/*
 *  ======== viewHwiBasicInfo ========
 */
function viewHwiBasicInfo()
{
    view = new Array();

    var hwis = viewGetHwiObjectArray();

    for (var i = 0; i < hwis.length; i++) {
        var obj = hwis[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewInitDetailed ========
 */
function viewInitDetailed(view, obj)
{
    viewInitBasic(view, obj);
    view.irp = toHexString(obj.irp);
    viewGetHwiStatus(view);
}

/*
 *  ======== viewHwiDetailedInfo ========
 */
function viewHwiDetailedInfo()
{
    view = new Array();

    var hwis = viewGetHwiObjectArray();

    for (var i = 0; i < hwis.length; i++) {
        var obj = hwis[i];
        var detailedView = new DetailedInfo();
        viewInitDetailed(detailedView, obj);
        view.push(detailedView);
    }

    return (view);
}

/* Export the Hwi_viewGetHwiStackInfo function */
xdc.global.Hwi_viewGetHwiStackInfo = viewGetHwiStackInfo;

/*!
 *  ======== viewGetHwiStackInfo ========
 */
function viewGetHwiStackInfo()
{
    var stackInfo = new StackInfo();

    /* Fetch needed info from Hwi module state */
    try {
        var modState = Program.fetchVariable("ti_sysbios_family_arm_v6m_Hwi_Module_state");
        var size = Number(modState.isrStackSize);
        var stackBase = modState.isrStackBase;
        var stackData = Program.fetchFromAddr(stackBase, "uint32_t", size/4);
    }
    catch (e) {
        stackInfo.hwiStackSize = 0;     /* signal error to caller */
        return (stackInfo);
    }

    var index = 0;

    /*
     * The stack is filled with 0xbe.
     */
    while (stackData[index] == 0xbebebebe) {
        index++;
    }

    stackInfo.hwiStackPeak = size - index*4;
    stackInfo.hwiStackSize = size;
    stackInfo.hwiStackBase = toHexString(stackBase);

    return (stackInfo);
}

/*
 * Exception Context constructor
 */
function ExcContext()
{
    /* Internal Registers */
    this.R0 = 0;
    this.R1 = 0;
    this.R2 = 0;
    this.R3 = 0;
    this.R4 = 0;
    this.R5 = 0;
    this.R6 = 0;
    this.R7 = 0;
    this.R8 = 0;
    this.R9 = 0;
    this.R10 = 0;
    this.R11 = 0;
    this.R12 = 0;
    this.SP = 0;
    this.LR = 0;
    this.PC = 0;
    this.PSR = 0;

    /* NVIC registers */
    this.ICSR = 0;
    this.SHCSR = 0;
    this.MMFSR = 0;
    this.BFSR = 0;
    this.UFSR = 0;
    this.HFSR = 0;
    this.DFSR = 0;
    this.MMAR = 0;
    this.BFAR = 0;
    this.AFSR = 0;
    this.SFSR = 0;
    this.SFAR = 0;
}

/*
 * ======== toPaddedHexString ========
 * converts a number to a hex string with "0x" prefix.
 * inserts up to (len - 1) leading zeros.
 * up to a max of 7 zeros.
 */
function toPaddedHexString(number, len)
{
    return ("0x" + ("0000000" + Number(number).toString(16)).substr(-len));
}

/*
 * ======== getExcContext ========
 * returns an ExcContext filled with the register set that was valid
 * at the time an exception occurred. Registers R4-R11 and the various
 * status registers are only guranteed to be valid if the exception
 * was a secure fault.
 */
function getExcContext()
{
    var hwiModState = Program.fetchVariable("ti_sysbios_family_arm_v6m_Hwi_Module_state");

    if (hwiModState.excActive == 0) return (null);

    var hwiExcContext = Program.fetchFromAddr(hwiModState.excContext, "ti_sysbios_family_arm_v6m_Hwi_ExcContext", 1);

    var excContext = new ExcContext();

    excContext.ICSR = Program.fetchFromAddr(0xe000ed04, "uint32_t", 1);

    var nvicRegs = Program.fetchFromAddr(0xe000ed24, "uint32_t", 7);

    excContext.SHCSR = nvicRegs[0];
    excContext.MMFSR = nvicRegs[1] & 0xff;
    excContext.BFSR = (nvicRegs[1] & 0x0000ff00) >> 8;
    excContext.UFSR = (nvicRegs[1] & 0xffff0000) >> 16;

    /* untouched S_Ptr types render as HEX */
    excContext.HFSR = nvicRegs[2];
    excContext.DFSR = nvicRegs[3];
    excContext.MMAR = nvicRegs[4];
    excContext.BFAR = nvicRegs[5];
    excContext.AFSR = nvicRegs[6];

    var sfRegs = Program.fetchFromAddr(0xe000ede4, "uint32_t", 6);
    excContext.SFSR = sfRegs[0];
    excContext.SFAR = sfRegs[1];

    excContext.R0 = hwiExcContext.r0;
    excContext.R1 = hwiExcContext.r1;
    excContext.R2 = hwiExcContext.r2;
    excContext.R3 = hwiExcContext.r3;
    excContext.R4 = hwiExcContext.r4;
    excContext.R5 = hwiExcContext.r5;
    excContext.R6 = hwiExcContext.r6;
    excContext.R7 = hwiExcContext.r7;
    excContext.R8 = hwiExcContext.r8;
    excContext.R9 = hwiExcContext.r9;
    excContext.R10 = hwiExcContext.r10;
    excContext.R11 = hwiExcContext.r11;
    excContext.R12 = hwiExcContext.r12;
    excContext.SP = hwiExcContext.sp;
    excContext.LR = hwiExcContext.lr;
    excContext.PC = hwiExcContext.pc;
    excContext.PSR = hwiExcContext.psr;

    return (excContext);
}

/*
 *  ======== viewExceptionInfo ========
 */
function viewExceptionInfo()
{
    var excContext = getExcContext();

    if (excContext == null) {
        var instView = new ExceptionInfo();
        instView.exceptionInfo = "No Interpretable Exception Pending";
        return (instView);
    }

    var view = new Array();

    /* Add decoded exception */
    var decodeView = new ExceptionInfo();
    decodeView.exceptionInfo = "Decoded Exception:";
    view.push(decodeView);

    decodeView = new ExceptionInfo();
    decodeView.exceptionInfo = viewDecodeException(excContext);
    view.push(decodeView);

    var registersView = new ExceptionInfo();
    registersView.exceptionInfo = "---";
    view.push(registersView);

    /* Add register dump */
    registersView = new ExceptionInfo();
    registersView.exceptionInfo = "Registers:";
    view.push(registersView);

    for (var z in excContext) {
        var instView = new ExceptionInfo();
        instView.exceptionInfo = z + " = " + toPaddedHexString(excContext[z], 8);
        view.push(instView);
    }

    return (view);
}

/*
 *  ======== viewDecodeNMI ========
 */
function viewDecodeNMI(excContext)
{
    return("NMI Exception");
}

/*
 *  ======== viewDecodeHardFault ========
 */
function viewDecodeHardFault(excContext)
{
    var fault = "Hard Fault: ";

    if (excContext.HFSR & 0x40000000) {
        fault += "FORCED: ";
        fault += viewDecodeUsageFault(excContext);
        fault += viewDecodeBusFault(excContext);
        fault += viewDecodeMemFault(excContext);
        return (fault);
    }
    else if (excContext.HFSR & 0x80000000) {
        fault += "DEBUGEVT: ";
        fault += viewDecodeDebugMon(excContext);
        return (fault);
    }
    else if (excContext.HFSR & 0x00000002) {
        fault += "VECTBL";
    }
    else {
        fault += "Unknown";
    }

    return (fault);
}

/*
 *  ======== viewDecodeMemFault ========
 */
function viewDecodeMemFault(excContext)
{
    var fault = ""

    if (excContext.MMFSR != 0) {

        fault = "MEMFAULT: ";
        if (excContext.MMFSR & 0x10) {
            fault += "MSTKERR";
        }
        else if (excContext.MMFSR & 0x08) {
            fault += "MUNSTKERR";
        }
        else if (excContext.MMFSR & 0x02) {
            fault += "DACCVIOL ";
            fault += "Data Access Error. Address = 0x" + Number(excContext.MMAR).toString(16);
        }
        else if (excContext.MMFSR & 0x01) {
            fault += "IACCVIOL ";
            fault += "Instruction Fetch Error. Address = 0x" + Number(excContext.MMAR).toString(16);
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeBusFault ========
 */
function viewDecodeBusFault(excContext)
{
    var fault = ""

    if (excContext.BFSR != 0) {

        fault = "BUSFAULT: ";

        if (excContext.BFSR & 0x10) {
            fault += "STKERR";
        }
        else if (excContext.BFSR & 0x08) {
            fault += "UNSTKERR";
        }
        else if (excContext.BFSR & 0x04) {
            fault += "IMPRECISERR";
        }
        else if (excContext.BFSR & 0x02) {
            fault += "PRECISERR.";
            fault += "Data Access Error. Address = 0x" + Number(excContext.BFAR).toString(16);
        }
        else if (excContext.BFSR & 0x01) {
            fault += "IBUSERR";
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeUsageFault ========
 */
function viewDecodeUsageFault(excContext)
{
    var fault = ""

    if (excContext.UFSR != 0) {
        fault = "USAGE: ";
        if (excContext.UFSR & 0x0001) {
            fault += "UNDEFINSTR";
        }
        else if (excContext.UFSR & 0x0002) {
            fault += "INVSTATE";
        }
        else if (excContext.UFSR & 0x0004) {
            fault += "INVPC";
        }
        else if (excContext.UFSR & 0x0008) {
            fault += "NOCP";
        }
        else if (excContext.UFSR & 0x0010) {
            fault += "STKOF";
        }
        else if (excContext.UFSR & 0x0100) {
            fault += "UNALIGNED";
        }
        else if (excContext.UFSR & 0x0200) {
            fault += "DIVBYZERO";
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeSecureFault ========
 */
function viewDecodeSecureFault(excContext)
{
    var fault = ""

    var SFSR = excContext.SFSR;

    if (SFSR != 0) {
        fault = "SECFAULT:";
        if (SFSR & 0x80) {
            fault += " LSERR";
        }
        if (SFSR & 0x20) {
            fault += " LSPERR";
        }
        if (SFSR & 0x10) {
            fault += " INVTRAN";
        }
        if (SFSR & 0x08) {
            fault += " AUVIOL";
        }
        if (SFSR & 0x04) {
            fault += " INVERR";
        }
        if (SFSR & 0x02) {
            fault += " INVIS";
        }
        if (SFSR & 0x01) {
            fault += " INVEP (Jump to invalid SG address: " +
                      toPaddedHexString(excContext.PC,8) + ")";
        }
    }

    if (SFSR & 0x40) {
        fault += " Access error at address = " +
                  toPaddedHexString(excContext.SFAR, 8);
    }

    return (fault);
}

/*
 *  ======== viewDecodeSvCall ========
 */
function viewDecodeSvCall(excContext)
{
    return("SV Call Exception, pc = " + Number(excContext.pc).toString(16));
}

/*
 *  ======== viewDecodeDebugMon ========
 */
function viewDecodeDebugMon(excContext)
{
    var fault = "";

    if (excContext.DFSR != 0) {

        fault = "DEBUG: ";

        if (excContext.DFSR & 0x00000010) {
            fault += "EXTERNAL";
        }
        else if (excContext.DFSR & 0x00000008) {
            fault += "VCATCH";
        }
        else if (excContext.DFSR & 0x00000004) {
            fault += "DWTTRAP";
        }
        else if (excContext.DFSR & 0x00000002) {
            fault += "BKPT";
        }
        else if (excContext.DFSR & 0x00000001) {
            fault += "HALTED";
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeReserved ========
 */
function viewDecodeReserved(excContext, excNum)
{
    return ("Reserved vector: " + String(excNum));
}

/*
 *  ======== viewDecodeSecureIsr ========
 */
function viewDecodeSecureIsr(excContext, excNum)
{
    var vtor = Program.fetchFromAddr(0xE000ED08, "uint32_t", 1);
    var vectorTable = Program.fetchFromAddr(vtor, "uint32_t", 120);
    var vectorFunc = Program.lookupFuncName(Number(vectorTable[excNum]));
    return ("IRQ Number: " + String(excNum) + " (" + vectorFunc + ")");
}

/*
 *  ======== viewDecodeNoIsr ========
 */
function viewDecodeNoIsr(excContext, excNum)
{
    return ("Undefined Hwi: " + String(excNum));
}

/*
 *  ======== viewDecodeException ========
 */
function viewDecodeException(excContext)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var excNum = String(excContext.ICSR & 0xff);

    switch (excNum) {
        case "2":
            return (viewDecodeNMI(excContext));         /* NMI */
            break;
        case "3":
            return (viewDecodeHardFault(excContext));   /* Hard Fault */
            break;
        case "4":
            return (viewDecodeMemFault(excContext));    /* Mem Fault */
            break;
        case "5":
            return (viewDecodeBusFault(excContext));    /* Bus Fault */
            break;
        case "6":
            return (viewDecodeUsageFault(excContext));  /* Usage Fault */
            break;
        case "7":
            return (viewDecodeSecureFault(excContext)); /* Secure Fault */
            break;
        case "11":
            return (viewDecodeSvCall(excContext));      /* SVCall */
            break;
        case "12":
            return (viewDecodeDebugMon(excContext));    /* Debug Mon */
            break;
        case "8":
        case "9":
        case "10":
        case "13":
            return (viewDecodeReserved(excContext, excNum));/* reserved */
            break;
        default:
            return (viewDecodeNoIsr(excContext, excNum));   /* no ISR */
            break;
    }
    return (null);
}

/*
 *  ======== viewCallStack ========
 */
function viewCallStack()
{
    try {
        var CallStack = xdc.useModule('xdc.rov.CallStack');
        CallStack.clearRegisters();
    }
    catch (e) {
        return (null);
    }

    var excContext = getExcContext();

    if (excContext == null) {
        var instView = new CallStackFrame();
        instView.frame = "No Interpretable Exception Pending";
        return (instView);
    }

    CallStack.setRegister("R0", Number(excContext.R0));
    CallStack.setRegister("R1", Number(excContext.R1));
    CallStack.setRegister("R2", Number(excContext.R2));
    CallStack.setRegister("R3", Number(excContext.R3));
    CallStack.setRegister("R4", Number(excContext.R4));
    CallStack.setRegister("R5", Number(excContext.R5));
    CallStack.setRegister("R6", Number(excContext.R6));
    CallStack.setRegister("R7", Number(excContext.R7));
    CallStack.setRegister("R8", Number(excContext.R8));
    CallStack.setRegister("R9", Number(excContext.R9));
    CallStack.setRegister("R10", Number(excContext.R10));
    CallStack.setRegister("R11", Number(excContext.R11));
    CallStack.setRegister("R12", Number(excContext.R12));
    CallStack.setRegister("SP", Number(excContext.SP));
    CallStack.setRegister("R13", Number(excContext.SP));
    CallStack.setRegister("R14", Number(excContext.LR));
    CallStack.setRegister("PC", Number(excContext.PC));
    CallStack.setRegister("xPSR", Number(excContext.PSR));

    /* fetch call stack string */
    var frames = CallStack.toText();
    /* break up into separate lines */
    frames = frames.split("\n");

    /* Try using the LR as the PC if there is no call stack using the PC */
    if (frames.length == 1) {
        CallStack.setRegister("PC", excContext.LR);
        frames = CallStack.toText();
        frames = frames.split("\n");
        if (frames.length > 1) {
            frames[0] += " (Using LR as PC)";
        }
    }

    if (frames.length > 1) {
        /*
         * Strip off "Unwind halted ... " from last frame
         */
        frames.length -= 1;
    }
    else {
        /* No call stack */
        var instView = new CallStackFrame();
        instView.frame = "No call stack";
        return (instView);
    }

    for (var i = 0; i < frames.length; i++) {
        var line = frames[i];
        /* separate frame # from frame text a little */
        line = line.replace(" ", "    ");
        var file = line.substr(line.indexOf(" at ") + 4);
        file = file.replace(/\\/g, "/");
        file = file.substr(file.lastIndexOf("/")+1);
        if (file != "") {
            frames[i] = line.substring(0,
                                   line.indexOf(" at ") + 4);
            /* tack on file info */
            frames[i] += file;
        }
    }

    /*
     * Invert the frames[] array so that the strings become the index of a
     * new associative array.
     *
     * This is done because the TREE view renders the array index (field)
     * on the left and the array value on the right.
     *
     * At the same time, extract the "PC = ..." substrings and make them
     * be the value of the array who's index is the beginning of the
     * frame text.
     */
    var invframes = new Array();

    for (var i = 0; i < frames.length; i++) {
        invframes[frames[i].substring(0,frames[i].indexOf("PC")-1)] =
            frames[i].substr(frames[i].indexOf("PC"));
    }

    var view = new Array();

    for (var x in invframes) {
        var instView = new CallStackFrame();
        instView.frame = " " + x + invframes[x];
        view.push(instView);
    }

    return (view);
}

/*
 *  ======== viewCheckVector ========
 *  Depending on ROV/ROV2 and the toolchain used to build the
 * .out file, the vale for a function symbol address may or may
 *  not have its LSB set. This function attempts to normalize
 *  the ROV view behavior for ALL permutations of toolchains
 *  and ROV/ROV2.
 */
function viewCheckVector(vectorObj, expected, errorPrefix)
{
    var val = Program.lookupSymbolValue(expected);
    var vector = vectorObj.vector;
    var match = Number(vector&~1) == Number(val&~1);
    if (match) {
        vectorObj.vectorLabel = expected;
    }
    else {
        vectorObj.vectorLabel = "error";
        if (errorPrefix) {
            vectorObj.$status["vector"] =
            vectorObj.$status["vectorLabel"] =
                errorPrefix + " vector is not as configured!\n" +
                "Should be: " + expected;
        }
    }
    return (match);
}

/*
 *  ======== viewInitVectorTable ========
 */
function viewInitVectorTable(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.v6m.Hwi');
    var halHwi = xdc.useModule('ti.sysbios.hal.Hwi');
    var hwiModCfg = Program.getModuleConfig(Hwi.$name);
    var correctLabel;

    var numInts = hwiModCfg.NUM_INTERRUPTS;

    viewNvicFetch(this);

    var vtor = Number(this.VTOR);

    var vectorTable = Program.fetchfromAddr(vtor, "uint32_t", numInts);

    try {
        var rawView = Program.scanRawView('ti.sysbios.family.arm.v6m.Hwi');
    }
    catch (e) {
        return null;
    }

    var dispatchTableAddr = rawView.modState.dispatchTable;

    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');

    /* Retrieve the dispatchTable array of handles */
    var hwiHandles = Program.fetchArray(ScalarStructs.S_Ptr$fetchDesc,
                                         dispatchTableAddr, numInts);
    var vectors = new Array();

    for (var i = 0; i < numInts; i++) {
        var vector = Program.newViewStruct('ti.sysbios.family.arm.v6m.Hwi', 'Vector Table');
        vector.vectorNum = i;
        vector.vector = "0x" + Number(vectorTable[i]).toString(16);

        /* Tag priority info */
        if (i >= 4) {
            var pri = viewGetPriority(view, this, i);
            var mask = numPriTable[hwiModCfg.NUM_PRIORITIES].mask;
            var shift = numPriTable[hwiModCfg.NUM_PRIORITIES].shift;
            vector.priority = "0x" + Number(pri).toString(16);
            if (hwiModCfg.priGroup + 1 > shift) {
                vector.preemptPriority = pri >> (hwiModCfg.priGroup + 1);
            }
            else {
                vector.preemptPriority = pri >> shift;
            }

            vector.subPriority = (pri & subPriMasks[hwiModCfg.priGroup]) >> shift;
        }

        /* Hwi handles only exist for interrupts 15 thru NUM_INTERRUPTS */
        if (i > 14) {
            var hwiHandle = hwiHandles[i];

            /* If a Hwi object exists for this vector */
            if (Number(hwiHandle.elem) != 0) {
                vector.type = "Dispatched";
                vector.hwiHandle = '0x' + Number(hwiHandle.elem).toString(16);
                /* fetch the Hwi object */
                var hwi = Program.fetchStruct(Hwi.Instance_State$fetchDesc, hwiHandle.elem, false);

                vector.hwiFxn = Program.lookupFuncName(Number(hwi.fxn))[0];
                vector.hwiArg = hwi.arg;
                vector.hwiIrp = hwi.irp;

                if (!viewCheckVector(vector, "ti_sysbios_family_arm_v6m_Hwi_dispatch__I")) {
                    vector.$status["vector"] = vector.$status["vectorLabel"] =
                        "The vector for this dispatched interrupt is not correct!\n" +
                        "Should be: \"ti_sysbios_family_arm_v6m_Hwi_dispatch__I\"";
                }

                if (vector.priority < hwiModCfg.disablePriority) {
                    vector.$status["type"] = vector.$status["priority"] =
                        "Unsafe! This dispatched interrupt " +
                        "has a zero latency interrupt priority and is " +
                        "not disabled by Hwi_disable()!";
                }
            }
            else {
                if (viewCheckVector(vector, "ti_sysbios_family_arm_v6m_Hwi_excHandlerAsm__I")) {
                    vector.type = "Unused";
                }
                else {
                    vector.type = "Unmanaged";
                    /*
                     * Attempt to find a matching symbol for the vector
                     * try with the LSB set to 1 first (ROV2 likes this)
                     */
                    var vectorLabel = Program.lookupFuncName(Number(vectorTable[i]));
                    if (vectorLabel.length == 0) {
                        /* clear LSB if label not found with LSB set (legacy ROV likes this) */
                        vectorLabel = Program.lookupFuncName(Number(vectorTable[i]&0xfffffffe));
                    }
                    if (vectorLabel.length == 0) {
                        vector.vectorLabel = "";
                    }
                    else {
                        vector.vectorLabel = vectorLabel[0];
                    }
                    /* check for non-dispatched interrupts created with Hwi.create */
                    for (var j in rawView.instStates) {
                        /* non-dispatched interrupts are encoded with 2's complemented intNums */
                        if (-(rawView.instStates[j].intNum) == i) {
                            if (vector.priority < hwiModCfg.disablePriority) {
                                vector.type = "Zero Latency";
                            }
                            else {
                                vector.type = "Non Dispatched";
                            }
                            vector.hwiHandle = "0x" + Number(rawView.instStates[j].$addr).toString(16);
                            vector.hwiFxn = Program.lookupFuncName(Number(rawView.instStates[j].fxn))[0];
                            vector.hwiArg = "N/A";
                            vector.hwiIrp = "N/A";
                            if (rawView.instStates[j].fxn != vector.vector) {
                                vector.$status["vector"] =
                                vector.$status["vectorLabel"] =
                                vector.$status["hwiFxn"] = "Vector does not match Hwi function!";
                            }
                        }
                    }
                }
            }

            var enabled = false;
            var active = false;
            var pending = false;

            if (i > 15) {
                var index = (i-16) >> 5;
                var mask = 1 << ((i-16) & 0x1f);
                enabled = this.ISER[index] & mask;
                active = this.IABR[index] & mask;
                pending = this.ISPR[index] & mask;
            }
            else {
                switch(i) {
                    case 15: /* SysTick */
                        pending = this.ICSR & 0x100000000;
                        enabled = this.STCSR & 0x00000002;
                        active = this.SHCSR & 0x00000800;
                        break;
                    default:
                        view.status = "unknown";
                        return;
                        break;
                }
            }

            if (enabled) {
                vector.status = "Enabled";
            }
            else {
                vector.status = "Disabled";
            }

            if (active) {
                vector.status += ", Active";
            }

            if (pending) {
                vector.status += ", Pending";
            }
        }
        vectors[vectors.length] = vector;
    }

    vectors[0].type = "MSP";
    vectors[0].vectorLabel = "";

    /* check exception handlers */

    vectors[1].type = "Reset";
    vectors[1].preemptPriority = -3;
    correctLabel = String(hwiModCfg.resetFunc).substring(1);
    viewCheckVector(vectors[1], correctLabel, "Reset");

    vectors[2].type = "NMI";
    vectors[2].preemptPriority = -2;
    correctLabel = String(hwiModCfg.nmiFunc).substring(1);
    viewCheckVector(vectors[2], correctLabel, "NMI");

    vectors[3].type = "HardFault";
    vectors[3].preemptPriority = -1;
    correctLabel = String(hwiModCfg.hardFaultFunc).substring(1);
    viewCheckVector(vectors[3], correctLabel, "Hard Fault");

    vectors[4].type = "MemFault";
    correctLabel = String(hwiModCfg.memFaultFunc).substring(1);
    viewCheckVector(vectors[4], correctLabel, "Mem Fault");

    vectors[5].type = "BusFault";
    correctLabel = String(hwiModCfg.busFaultFunc).substring(1);
    viewCheckVector(vectors[5], correctLabel, "Bus Fault");

    vectors[6].type = "UsageFault";
    correctLabel = String(hwiModCfg.usageFaultFunc).substring(1);
    viewCheckVector(vectors[6], correctLabel, "Usage Fault");

    vectors[7].type = "Reserved";
    vectors[7].vectorLabel = "";
    vectors[8].type = "Reserved";
    vectors[8].vectorLabel = "";
    vectors[9].type = "Reserved";
    vectors[9].vectorLabel = "";
    vectors[10].type = "Reserved";
    vectors[10].vectorLabel = "";

    vectors[11].type = "SVCall";
    correctLabel = String(hwiModCfg.svCallFunc).substring(1);
    viewCheckVector(vectors[11], correctLabel, "SVCall");

    vectors[12].type = "DebugMon";
    correctLabel = String(hwiModCfg.debugMonFunc).substring(1);
    viewCheckVector(vectors[12], correctLabel, "Debug Mon");

    vectors[13].type = "Reserved";
    vectors[13].vectorLabel = "";

    vectors[14].type = "PendSV";
    correctLabel = "ti_sysbios_family_arm_v6m_Hwi_pendSV__I";
    viewCheckVector(vectors[14], correctLabel, "PendSV");

    view.elements = vectors;
}

/*
 *  ======== viewHwiModuleInfo ========
 */
function viewHwiModuleInfo()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var CallStack = xdc.useModule('xdc.rov.CallStack');

    var modView = new ModuleInfo();

    CallStack.fetchRegisters(["CTRL_FAULT_BASE_PRI"]);
    var ctrlFaultBasePri = CallStack.getRegister("CTRL_FAULT_BASE_PRI");

    viewNvicFetch(this);

    modView.activeInterrupt = String(this.ICSR & 0xff);
    modView.pendingInterrupt = String((this.ICSR & 0xff000) >> 12);
    if (modView.activeInterrupt != "0") {
        modView.processorState = "Handler";
    }
    else if (ctrlFaultBasePri & 0x01000000) {
        modView.processorState = "Unpriv, Thread";
    }
    else {
        modView.processorState = "Priv, Thread";
    }

    if ((modView.activeInterrupt > 0) && (modView.activeInterrupt < 14)) {
        modView.exception = "Yes";
        Program.displayError(modView, "exception", "An exception has occurred!");
    }
    else {
        modView.exception = "none";
    }

//    view.options[0] = "Hwi.autoNestingSupport = ";
//    view.options[1] = "Hwi.swiSupport = ";
//    view.options[2] = "Hwi.taskSupport = ";
//    view.options[3] = "Hwi.irpSupport = ";

//    view.options[0] += hwiModCfg.dispatcherAutoNestingSupport ? "true" : "false";
//    view.options[1] += hwiModCfg.dispatcherSwiSupport ? "true" : "false";
//    view.options[2] += hwiModCfg.dispatcherTaskSupport ? "true" : "false";
//    view.options[3] += hwiModCfg.dispatcherIrpTrackingSupport ? "true" : "false";

    var stackInfo = viewGetHwiStackInfo();

    modView.hwiStackPeak = stackInfo.hwiStackPeak;
    modView.hwiStackSize = stackInfo.hwiStackSize;
    modView.hwiStackBase = stackInfo.hwiStackBase;

    if (stackInfo.hwiStackPeak == stackInfo.hwiStackSize) {
        Program.displayError(modView, "hwiStackPeak", "Hwi Stack Overrun!");
    }

    return (modView);
}
