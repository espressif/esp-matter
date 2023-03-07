;******************************************************************************
;* BOOT  v5.1.5                                                               *
;*                                                                            *
;* Copyright (c) 1996-2014 Texas Instruments Incorporated                     *
;* http://www.ti.com/                                                         *
;*                                                                            *
;*  Redistribution and  use in source  and binary forms, with  or without     *
;*  modification,  are permitted provided  that the  following conditions     *
;*  are met:                                                                  *
;*                                                                            *
;*     Redistributions  of source  code must  retain the  above copyright     *
;*     notice, this list of conditions and the following disclaimer.          *
;*                                                                            *
;*     Redistributions in binary form  must reproduce the above copyright     *
;*     notice, this  list of conditions  and the following  disclaimer in     *
;*     the  documentation  and/or   other  materials  provided  with  the     *
;*     distribution.                                                          *
;*                                                                            *
;*     Neither the  name of Texas Instruments Incorporated  nor the names     *
;*     of its  contributors may  be used to  endorse or  promote products     *
;*     derived  from   this  software  without   specific  prior  written     *
;*     permission.                                                            *
;*                                                                            *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
;*                                                                            *
;******************************************************************************

;****************************************************************************
;* BOOT.ASM
;*
;* THIS IS THE INITAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
;* IT MUST BE LINKED AND LOADED WITH ALL C++ PROGRAMS.
;*
;* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
;*   1) ALLOCATES THE STACK AND INITIALIZES THE STACK POINTER
;*   2) CALLS AUTO-INITIALIZATION ROUTINE
;*   3) CALLS THE FUNCTION MAIN TO START THE C++ PROGRAM
;*   4) CALLS THE STANDARD EXIT ROUTINE
;*
;* THIS MODULE DEFINES THE FOLLOWING GLOBAL SYMBOLS:
;*   1) __stack     STACK MEMORY AREA
;*   2) _c_int00    BOOT ROUTINE
;*
;****************************************************************************
   .if  __TI_ARM_V7M__ | __TI_ARM_V6M0__
	.thumbfunc _c_int00
   .else
	.armfunc _c_int00
   .endif

;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg	_args_main,   ARGS_MAIN_RTN
        .asg	xdc_runtime_System_exit__E,         EXIT_RTN
        .asg    main_func_sp, MAIN_FUNC_SP
	.asg	xdc_runtime_Startup_reset__I,	RESET_FUNC
   .elseif __TI_ARM9ABI_ASSEMBLER | .TMS470_32BIS
        .asg	__args_main,   ARGS_MAIN_RTN
        .asg	_xdc_runtime_System_exit__E,         EXIT_RTN
        .asg    _main_func_sp, MAIN_FUNC_SP
	.asg	_xdc_runtime_Startup_reset__I,	RESET_FUNC
   .else
        .asg	$_args_main,   ARGS_MAIN_RTN
        .asg	xdc_runtime_System_exit__E,         EXIT_RTN
        .asg    _main_func_sp, MAIN_FUNC_SP
	.asg	_xdc_runtime_Startup_reset__I,	RESET_FUNC
   .endif

   .if .TMS470_16BIS

;****************************************************************************
;*  16 BIT STATE BOOT ROUTINE                                               *
;****************************************************************************

   .if __TI_ARM_V7M__ | __TI_ARM_V6M0__
	.state16
   .else
	.state32
   .endif

	.global	__stack
;***************************************************************
;* DEFINE THE USER MODE STACK (DEFAULT SIZE IS 512)
;***************************************************************
__stack:.usect	".stack", 0, 4

	.global	_c_int00

   .if __TI_TMS470_V7M__
;
; Add minimal boot vector table
;
	.global __STACK_SIZE

	.sect	".bootVecs"
	.long	__stack		; This really should be __stack + __STACKSIZE
				; but since c_int00 sets the sp anyway its
				; ok
	.long	_c_int00

	.symdepend ".bootVecs", ".text"

	.text
   .endif

;***************************************************************
;* FUNCTION DEF: _c_int00                                      
;***************************************************************
_c_int00: .asmfunc

	.if !__TI_ARM_V7M__ & !__TI_ARM_V6M0__
	.if __TI_NEON_SUPPORT__ | __TI_VFP_SUPPORT__
        ;*------------------------------------------------------
	;* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
	;* 10 AND 11, REQUIRED TO ENABLE NEON/VFP      
	;* COPROCESSOR ACCESS CONTROL REG 
	;* BITS [23:22] - CP11, [21:20] - CP10
	;* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
        ;*------------------------------------------------------
	MRC      p15,#0x0,r0,c1,c0,#2
        MOV      r3,#0xf00000
	ORR      r0,r0,r3
        MCR      p15,#0x0,r0,c1,c0,#2

        ;*------------------------------------------------------
	; SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
        ;*------------------------------------------------------
      	MOV      r0,#0x40000000
        FMXR     FPEXC,r0
        .endif ; __TI_NEON_SUPPORT__ | __TI_VFP_SUPPORT__

	;------------------------------------------------------
	;* SET TO SYSTEM MODE
        ;*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0x1F  ; CLEAR MODES
        ORR     r0, r0, #0x1F  ; SET SYSTEM MODE
        MSR     cpsr_cf, r0

        ;*------------------------------------------------------
	;* CHANGE TO 16 BIT STATE
        ;*------------------------------------------------------
	ADD	r0, pc, #1
	BX	r0

	.state16
        .else ; !__TI_ARM_V7M & !__TI_ARM_V6M0
	.if __TI_TMS470_V7M4__ & __TI_VFP_SUPPORT__

	.thumb
	;*------------------------------------------------------
	;* SETUP FULL ACCESS TO COPROCESSORS 10 AND 11,
	;* REQUIRED FOR FP. COPROCESSOR ACCESS CONTROL REG
	;* BITS [23:22] - CP11, [21:20] - CP10
	;* SET TO 0b11 TO ENABLE FULL ACCESS
        ;*------------------------------------------------------
cpacr   .set     0xE000ED88			; CAPCR address
	MOVW     r1, #cpacr & 0xFFFF
	MOVT     r1, #cpacr >> 16
	LDR      r0, [ r1 ]
        MOV      r3, #0xf0
	ORR      r0,r0,r3, LSL #16
	STR      r0, [ r1 ]
	.state16
	.endif ; __TI_TMS470_V7M4__ & __TI_VFP_SUPPORT__
	.endif ; !__TI_ARM_V7M & !__TI_ARM_V6M0__

	;*------------------------------------------------------
        ;* INITIALIZE THE USER/SYSTEM MODE STACK                      
        ;*------------------------------------------------------
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	MOVW	r0, __stack
	MOVT	r0, __stack
	MOV	sp, r0
	MOVW	r0, __STACK_SIZE
	MOVT	r0, __STACK_SIZE
	.state16
	.else ; __TI_AVOID_EMBEDDED_CONSTANTS
	LDR     r0, c_stack
	MOV	sp, r0
        LDR     r0, c_STACK_SIZE
	.endif ; __TI_AVOID_EMBEDDED_CONSTANTS
	ADD	sp, r0

	;*-----------------------------------------------------
	;* ALIGN THE STACK TO 64-BITS IF EABI.
	;*-----------------------------------------------------
	.if __TI_EABI_ASSEMBLER
	MOV	r7, sp
	MOV	r0, #0x07
	BIC     r7, r0         ; Clear upper 3 bits for 64-bit alignment.
	MOV	sp, r7
	.endif

	;*-----------------------------------------------------
	;* SAVE CURRENT STACK POINTER FOR SDP ANALYSIS
	;*-----------------------------------------------------
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	MOVW	r0, MAIN_FUNC_SP
	MOVT	r0, MAIN_FUNC_SP
	.state16
	.else
	LDR	r0, c_mf_sp
	.endif
	MOV	r7, sp
	STR	r7, [r0]

	.if __TI_AVOID_EMBEDDED_CONSTANTS
	MOVW	r0, RESET_FUNC
	MOVT	r0, RESET_FUNC
	.state16
	.else
	LDR	r0, c_reset
	.endif
	CMP	r0, #0
	BEQ	_no_reset_
	MOV	r1, pc
	ADD	r1, r1, #5
	MOV	lr, r1
	BX	r0

_no_reset_:

        ;*------------------------------------------------------
        ;* Perform all the required initilizations:
        ;*   - Process BINIT Table
        ;*   - Perform C auto initialization
        ;*   - Call global constructors 
        ;*------------------------------------------------------
        BL      __TI_auto_init

        ;*------------------------------------------------------
	;* CALL APPLICATION                                     
        ;*------------------------------------------------------
        BL      ARGS_MAIN_RTN

        ;*------------------------------------------------------
	;* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT()
        ;*------------------------------------------------------
        BL      EXIT_RTN

        ;*------------------------------------------------------
	;* DONE, LOOP FOREVER
        ;*------------------------------------------------------
L1:     B	L1
	.endasmfunc

   .else           ; !.TMS470_16BIS

;****************************************************************************
;*  32 BIT STATE BOOT ROUTINE                                               *
;****************************************************************************

	.global	__stack
;***************************************************************
;* DEFINE THE USER MODE STACK (DEFAULT SIZE IS 512)            
;***************************************************************
__stack:.usect	".stack", 0, 4

	.global	_c_int00
;***************************************************************
;* FUNCTION DEF: _c_int00                                      
;***************************************************************
_c_int00: .asmfunc

	.if __TI_NEON_SUPPORT__ | __TI_VFP_SUPPORT__
        ;*------------------------------------------------------
	;* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
	;* 10 AND 11, REQUIRED TO ENABLE NEON/VFP      
	;* COPROCESSOR ACCESS CONTROL REG 
	;* BITS [23:22] - CP11, [21:20] - CP10
	;* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
        ;*------------------------------------------------------
	MRC      p15,#0x0,r0,c1,c0,#2
        MOV      r3,#0xf00000
	ORR      r0,r0,r3
        MCR      p15,#0x0,r0,c1,c0,#2

        ;*------------------------------------------------------
	; SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
        ;*------------------------------------------------------
      	MOV      r0,#0x40000000
        FMXR     FPEXC,r0
	.endif

        ;*------------------------------------------------------
	;* SET TO SYSTEM MODE
        ;*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0x1F  ; CLEAR MODES
        ORR     r0, r0, #0x1F  ; SET SYSTEM MODE
        MSR     cpsr_cf, r0

        ;*------------------------------------------------------
        ;* INITIALIZE THE USER/SYSTEM MODE STACK                      
        ;*------------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    sp, __stack
        MOVT    sp, __stack
        MOVW    r0, __STACK_SIZE
        MOVT    r0, __STACK_SIZE
        .else
        LDR     sp, c_stack
        LDR     r0, c_STACK_SIZE
        .endif
        ADD     sp, sp, r0

	;*-----------------------------------------------------
	;* ALIGN THE STACK TO 64-BITS IF EABI.
	;*-----------------------------------------------------
	.if __TI_EABI_ASSEMBLER
	BIC     sp, sp, #0x07  ; Clear upper 3 bits for 64-bit alignment.
	.endif

        ;*-----------------------------------------------------
        ;* SAVE CURRENT STACK POINTER FOR SDP ANALYSIS
        ;*-----------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    r0, MAIN_FUNC_SP
        MOVT    r0, MAIN_FUNC_SP
        .else
        LDR     r0, c_mf_sp
        .endif
        STR     sp, [r0]
                
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    r0, RESET_FUNC
        MOVT    r0, REST_FUNC
        .else
	LDR	r0, c_reset
        .endif
	CMP	r0, #0
	BEQ	_no_reset_
	MOV	lr, pc
	BX	r0

_no_reset_:

        ;*------------------------------------------------------
        ;* Perform all the required initilizations:
        ;*   - Process BINIT Table
        ;*   - Perform C auto initialization
        ;*   - Call global constructors 
        ;*------------------------------------------------------
        BL      __TI_auto_init

        ;*------------------------------------------------------
	;* CALL APPLICATION                                     
        ;*------------------------------------------------------    
        BL      ARGS_MAIN_RTN

        ;*------------------------------------------------------
	;* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT()
        ;*------------------------------------------------------
        BL      EXIT_RTN

        ;*------------------------------------------------------
	;* DONE, LOOP FOREVER
        ;*------------------------------------------------------
L1:     B	L1
	.endasmfunc

   .endif    ; !.TMS470_16BIS

;***************************************************************
;* CONSTANTS USED BY THIS MODULE
;***************************************************************
	.if !__TI_AVOID_EMBEDDED_CONSTANTS
c_stack		.long    __stack
c_STACK_SIZE  	.long    __STACK_SIZE
c_mf_sp	        .long    MAIN_FUNC_SP
c_reset       	.long    RESET_FUNC
	.endif

	.if __TI_EABI_ASSEMBLER
        .data
        .align 4
_stkchk_called:
        .field          0,32
        .else
        .sect   ".cinit"
        .align  4
        .field          4,32
        .field          _stkchk_called+0,32
        .field          0,32

        .bss    _stkchk_called,4,4
        .symdepend ".cinit", ".bss"
        .symdepend ".cinit", ".text"
        .symdepend ".bss", ".text"
	.endif

;******************************************************
;* UNDEFINED REFERENCES                               *
;******************************************************
	.global _stkchk_called
	.global	__STACK_SIZE
	.global ARGS_MAIN_RTN
	.global MAIN_FUNC_SP
	.global	EXIT_RTN
	.global	RESET_FUNC
        .global __TI_auto_init

	.end
