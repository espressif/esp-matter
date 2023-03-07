/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== TaskSupport.xs ========
 */

/*
 * ======== getAsmFiles ========
 * return the array of assembly language files associated
 * with targetName (ie Program.build.target.$name) 
 */
function getAsmFiles(targetName)
{
    switch(targetName) {
        case "ti.targets.arm.elf.M3":
        case "ti.targets.arm.elf.M4":
        case "ti.targets.arm.elf.M4F":
            return (["TaskSupport_asm.sv7M"]);
            break;

        case "gnu.targets.arm.M3":
        case "gnu.targets.arm.M4":
        case "gnu.targets.arm.M4F":
        case "ti.targets.arm.clang.M3":
        case "ti.targets.arm.clang.M4":
        case "ti.targets.arm.clang.M4F":
            return (["TaskSupport_asm_gnu.sv7M"]);
            break;

        case "iar.targets.arm.M3":
        case "iar.targets.arm.M4":
        case "iar.targets.arm.M4F":
            return (["TaskSupport_asm_iar.sv7M"]);
            break;

	default:
	    return (null);
	    break;
    }
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }
    this.common$.fxntab = false;

    /* provide getAsmFiles() for Build.getAsmFiles() */
    this.$private.getAsmFiles = getAsmFiles;
}

/*
 *  ======== stackUsedMeta ========
 */
function stackUsed$view(stackData)
{
    var size = stackData.length;
    var index = 0;
    /*
     * The stack is filled with 0xBE. Because the type is Char, not UChar,
     * ROV reads this value as signed: -66.
     */
    while(stackData[index] == -66) {
        index++;
    }

    return (size - index);
}

/*
 *  ======== getCallStack$view ========
 */
function getCallStack$view(taskRawView, taskState, threadType)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    var CallStack = xdc.useModule('xdc.rov.CallStack');

    var stackData = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, taskRawView.context, 32, false);

    var sp = 0;
    var pc = 0;
    var lr = 0;

    if (Program.build.target.name == "M4F") {
        var contextStackOffset = 16;
    }
    else {
        var contextStackOffset = 0;
    }

    /*
     * PC should be within TaskSupport_swap() but the debugger can't do
     * call stack for assembly coded functions. So, we set the PC to the
     * saved LR.
     */

    pc = stackData[contextStackOffset + 8];
    sp = Number(taskRawView.context) + 4 * (contextStackOffset + 9);

//    pc = Program.getSymbolValue("ti_sysbios_knl_Task_swapReturn");
//    sp = Number(taskRawView.context);
//    lr = stackData[contextStackOffset + 8];

    if (taskState == Task.Mode_RUNNING) {
        switch (threadType) {
            /* This is the current thread, use live registers */
            case "Task":
                /* use current debugger register values */
                CallStack.fetchRegisters(["R13", "R14", "PC", "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12"]);
                sp = CallStack.getRegister("R13");
                lr = CallStack.getRegister("R14");
                pc = CallStack.getRegister("PC");
                break;

            /*
             * The running task has been pre-empted by a Hwi and/or Swi.
             * Assume its a Hwi.
             */
            case "Hwi":
            case "Swi":
                /* fetch PC, LR, SP from Hwi stack */
                try {
                    /* fetch current PSP from debugger */
                    CallStack.fetchRegisters(["PSP"]);
                    var taskSP = CallStack.getRegister("PSP");
                    /*
                     * Compensate for push of old basepri and swiTaskKey
                     * to get to real task SP at time of interrupt.
                     * (See family/arm/m3/Hwi_asm.sv7M for details)
                     */
                    if (threadType == "Swi") {
                        taskSP += 8;
                    }
                    stackData = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, taskSP, 32);
                }
                catch (e) {
                    return (e.toString);
                }

                if (Program.build.target.name == "M4F") {
                    var hwiStackOffset = 18;
                }
                else {
                    var hwiStackOffset = 0;
                }

                lr = stackData[hwiStackOffset + 5];
                pc = stackData[hwiStackOffset + 6];
                sp = taskSP + 4 * (hwiStackOffset + 8);

                CallStack.setRegister("R12", stackData[hwiStackOffset + 4]);
                CallStack.setRegister("R3", stackData[hwiStackOffset + 3]);
                CallStack.setRegister("R2", stackData[hwiStackOffset + 2]);
                CallStack.setRegister("R1", stackData[hwiStackOffset + 1]);
                CallStack.setRegister("R0", stackData[hwiStackOffset + 0]);
                break;

            /* The program has called BIOS_exit(), use live registers */
            case "Main":
                /* use current debugger register values */
                CallStack.fetchRegisters(["R13","R14","PC"]);
                sp = CallStack.getRegister("R13");
                lr = CallStack.getRegister("R14");
                pc = CallStack.getRegister("PC");
                break;
        }
    }
    else {
        CallStack.setRegister("R11", stackData[contextStackOffset + 7]);
        CallStack.setRegister("R10", stackData[contextStackOffset + 6]);
        CallStack.setRegister("R9", stackData[contextStackOffset + 5]);
        CallStack.setRegister("R8", stackData[contextStackOffset + 4]);
        CallStack.setRegister("R7", stackData[contextStackOffset + 3]);
        CallStack.setRegister("R6", stackData[contextStackOffset + 2]);
        CallStack.setRegister("R5", stackData[contextStackOffset + 1]);
        CallStack.setRegister("R4", stackData[contextStackOffset + 0]);
    }

    CallStack.setRegister("PC", pc);
    CallStack.setRegister("R13", sp);
    CallStack.setRegister("SP", sp);
    CallStack.setRegister("FP", sp);
    CallStack.setRegister("R14", lr);

    var bts = "";

    var enumStrArray = String(taskState).split(".");
    bts += "Task_" + enumStrArray[enumStrArray.length - 1];

    bts += ", PC = 0x" + pc.toString(16);
    bts += ", SP = 0x" + sp.toString(16);
    bts += ", LR = 0x" + lr.toString(16);

    bts += "\n";

    bts += CallStack.toText();

    return (bts);
}
