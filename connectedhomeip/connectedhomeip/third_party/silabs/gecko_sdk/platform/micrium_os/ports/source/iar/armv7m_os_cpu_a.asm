;
;/***************************************************************************//**
; * @file
; * @brief Kernel - ARM Cortex-M Port
; *******************************************************************************
; * # License
; * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
; *******************************************************************************
; *
; * The licensor of this software is Silicon Laboratories Inc.  Your use of this
; * software is governed by the terms of Silicon Labs Master Software License
; * Agreement (MSLA) available at
; * www.silabs.com/about-us/legal/master-software-license-agreement.  This
; * software is distributed to you in Source Code format and is governed by the
; * sections of the MSLA applicable to Source Code.
; *
; ******************************************************************************/

;****************************************************************************************************//**
;* @note     (1) This port targets the following:
;*                 Core      : ARMv7M Cortex-M
;*                 Mode      : Thumb-2 ISA
;*                 Toolchain : Any 'armcc' based toolchain and/or IDE
;*
;* @note     (2) This port supports the ARM Cortex-M3, Cortex-M4 and Cortex-M7 architectures.
;*
;* @note     (3) It has been tested with the following Hardware Floating Point Unit.
;*                - (a) Single-precision: FPv4-SP-D16-M and FPv5-SP-D16-M
;*                - (b) Double-precision: FPv5-D16-M
;*******************************************************************************************************/

;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

                                                                ; External references.
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCurPtr
    EXTERN  OSTCBHighRdyPtr
    EXTERN  OSIntExit
    EXTERN  OSTaskSwHook
    EXTERN  OS_CPU_ExceptStkBase
    EXTERN  OS_TaskReturn
    EXTERN  OSIdleContext
    EXTERN  OSEnableIRQ
    EXTERN  OSDisableIRQ

    PUBLIC  OSStartHighRdy                                      ; Functions declared in this file
    PUBLIC  OSCtxSw
    PUBLIC  OSIntCtxSw
    PUBLIC  PendSV_Handler

;********************************************************************************************************
;                                               EQUATES
;********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.

FPU_FPCCR       EQU     0xE000EF34                              ; Address of FPU FPCCR register.

;********************************************************************************************************
;                                     CODE GENERATION DIRECTIVES
;********************************************************************************************************

    RSEG CODE:CODE:NOROOT(2)
    THUMB

;********************************************************************************************************
;                                         START MULTITASKING
;                                      void OSStartHighRdy(void)
;
; Note(s) : 1) This function setup the PendSV exception and load the context of the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0;
;              c) Set the main stack to OS_CPU_ExceptStkBase
;              d) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              e) Get current ready thread TCB, OSTCBCurPtr = OSTCBHighRdyPtr;
;              f) Get new process SP from TCB, SP = OSTCBHighRdyPtr->StkPtr;
;              g) Restore R0-R11 and R14 from new process stack;
;              h) Enable interrupts (tasks will run with interrupts enabled).
;********************************************************************************************************

OSStartHighRdy

    CPSID   I                                                   ; Mask all interrupts

    MOV32   R0, NVIC_SYSPRI14                                   ; Set the PendSV exception priority
    MOV32   R1, NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0                                              ; Set the PSP to 0 for initial context switch call
    MSR     PSP, R0

    MOV32   R0, OS_CPU_ExceptStkBase                            ; Initialize the MSP to the OS_CPU_ExceptStkBase
    LDR     R1, [R0]
    MSR     MSP, R1

    BL      OSTaskSwHook                                        ; Call OSTaskSwHook() for FPU Push & Pop

    MOV32   R0, OSPrioCur                                       ; OSPrioCur   = OSPrioHighRdy;
    MOV32   R1, OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    MOV32   R0, OSTCBCurPtr                                     ; OSTCBCurPtr = OSTCBHighRdyPtr;
    MOV32   R1, OSTCBHighRdyPtr
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    MSR     PSP, R0                                             ; Load PSP with new process SP

    MRS     R0, CONTROL
    ORR     R0, R0, #2
    BIC     R0, R0, #4                                          ; Clear the FPU bit in case FPU was used before the first task
    MSR     CONTROL, R0
    ISB                                                         ; Sync instruction stream

    LDMFD   SP!, {R4-R11, LR}                                   ; Restore r4-11, lr from new process stack
    LDMFD   SP!, {R0-R3}                                        ; Restore r0, r3
    LDMFD   SP!, {R12, LR}                                      ; Load R12 and LR
    LDMFD   SP!, {R1, R2}                                       ; Load PC and discard xPSR

    CPSIE   I                                                   ; Unmask all interrupts

    BX      R1


;********************************************************************************************************
;                       PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;********************************************************************************************************

OSCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


;********************************************************************************************************
;                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
;
; Note(s) : 1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;********************************************************************************************************

OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR


;********************************************************************************************************
;                                       HANDLE PendSV EXCEPTION
;                                      void PendSV_Handler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M.  This is because the Cortex-M auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 & R14 is required and fixing up the stack pointers. Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Check if current TCB is null (idle).
;              b) If yes, clear LSPACT bit of FPU->FPCCR register to make sure that lazy stacking doesnt
;                 trigger later. Skip context save and got to g).
;              c) Get the process SP.
;              d) If FP instructions was used, indicated by the EXEC_RETURN, save remaining FP regs S16-S31.
;              e) Save remaining regs r4-r11 & r14 on process stack;
;              f) Save the process SP in its TCB, OSTCBCurPtr->OSTCBStkPtr = SP;
;              g) Call OSTaskSwHook();
;              h) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              i) Get current ready thread TCB, OSTCBCurPtr = OSTCBHighRdyPtr;
;              j) Check if going to idle. If yes, skip context restore and go to o)
;              k) Get new process SP from TCB, SP = OSTCBHighRdyPtr->OSTCBStkPtr;
;              l) Restore R4-R11 and R14 from new process stack;
;              m) If FP instructions was used, indicated by the EXEC_RETURN, restore FP regs S16-S31.
;              n) Context restoration is completed, go to end of function.
;              o) Push a fake exception return stack frame on isr stack and use that as MSP for idle context.
;              p) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCurPtr      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdyPtr  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************

PendSV_Handler

    PUSH    {LR}                                                ; Prevent interruption during context switch
    BL      OSDisableIRQ
    POP     {LR}

    MOV32   R2, OSTCBCurPtr                                     ; if current TCB == null, we were in idle ...
    LDR     R1, [R2]
    CMP     R1, #0

#ifdef __ARMVFP__
    ITTTT  EQ
    LDREQ  R2, =FPU_FPCCR                                       ; Load FPU->FPCCR register address
    LDREQ  R3, [R2]
    BICEQ  R3, R3, #1                                           ; Clear LSPACT bit of LPCCR in case a FPU lazy stacking was active.
    STREQ  R3, [R2]
#endif

    BEQ     .end_context_save                                   ; Skip context save

    MRS     R0, PSP                                             ; PSP is process stack pointer

#ifdef __ARMVFP__
    TST       LR, #0x10                                         ; Save high FP registers, only if FPU is used in task context
    IT        EQ
    VSTMDBEQ  R0!, {S16-S31}
#endif

    STMFD   R0!, {R4-R11, R14}                                  ; Save remaining regs r4-11, R14 on process stack

    MOV32   R5, OSTCBCurPtr                                     ; OSTCBCurPtr->StkPtr = SP;
    LDR     R1, [R5]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

.end_context_save:
                                                                ; At this point, entire context of process has been saved
    MOV     R4, LR                                              ; Save LR exc_return value
    BL      OSTaskSwHook                                        ; Call OSTaskSwHook() for FPU Push & Pop

    MOV32   R1, OSTCBCurPtr
    MOV32   R0, OSTCBHighRdyPtr                                 ; OSTCBCurPtr = OSTCBHighRdyPtr;
    LDR     R3, [R0]
    STR     R3, [R1]

    MOV32   R2, OSPrioCur                                       ; OSPrioCur   = OSPrioHighRdy;
    MOV32   R1, OSPrioHighRdy
    LDRB    R0, [R1]
    STRB    R0, [R2]

    ORR     LR,  R4, #0x04                                      ; Ensure exception return uses process stack

    CMP     R3, #0                                              ; if current TCB == idle, we are going to idle...
    BEQ     .update_msp_idle                                    ; Skip normal context restore and set idle context

    LDR     R0,  [R3]                                           ; R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    LDMFD   R0!, {R4-R11, R14}                                  ; Restore r4-11, R14 from new process stack

#ifdef __ARMVFP__
    TST       LR, #0x10                                         ; Restore high FP registers, only if FPU is used in new task context
    IT        EQ
    VLDMIAEQ  R0!, {S16-S31}
#endif

    MSR     PSP, R0                                             ; Load PSP with new process SP

    B       .end_context_restore

.update_msp_idle:
    MOV32   R1, OS_CPU_ExceptStkBase                            ; Reuse exception stack as MSP for idle and create fake
    LDR     R0, [R1]                                            ; exception return stack frame

    SUB     R0, R0, #28                                         ; Stack base = &SP[top - 7]
    MSR     MSP, R0                                             ; Load MSP with new process SP

    MOV32   R1, 0x01000000                                      ; SP[top - 0] = PSR
    STR     R1, [R0, #28]

    MOV32   R1, OSIdleContext                                   ; SP[top - 1] = Return address -> Idle context handler
    STR     R1, [R0, #24]

    MOV32   R1, OS_TaskReturn                                   ; SP[top - 2] = LR -> Task return
    STR     R1, [R0, #20]

    MOV32   LR, #0xFFFFFFF9                                     ; Exception return uses master stack/process mode/basic frame

.end_context_restore:

    PUSH    {LR}                                                ; Enable interrupts again and return from exception
    BL      OSEnableIRQ
    POP     {PC}

    END
