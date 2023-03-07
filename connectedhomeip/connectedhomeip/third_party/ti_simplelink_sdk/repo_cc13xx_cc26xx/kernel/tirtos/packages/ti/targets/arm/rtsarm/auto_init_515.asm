;******************************************************************************
;* AUTO_INIT  v5.1.5                                                          *
;*                                                                            *
;* Copyright (c) 2007-2014 Texas Instruments Incorporated                     *
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
;* AUTO_INIT.ASM
;*
;* THIS ROUTINE PERFORMS C/C++ AUTO INITIALIZATION. IT IS CALLED
;* FROM THE INITIAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
;*
;* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
;*   1) PROCESSES BINIT TABLE IF PRESENT
;*   2) PERFORMS C AUTO-INITIALIZATION IF NEEDED
;*   3) CALLS INITALIZATION ROUTINES FOR FILE SCOPE CONSTRUCTION
;*
;* THIS MODULE DEFINES THE FOLLOWING LOCAL FUNCTION:
;*   1) perform_cinit     routine to perform C initialization
;*
;****************************************************************************

;****************************************************************************
; Accomodate different lowerd names in different ABIs
;****************************************************************************
   .if   __TI_EABI_ASSEMBLER
        .asg	copy_in,  COPY_IN_RTN
   .elseif __TI_ARM9ABI_ASSEMBLER | .TMS470_32BIS
        .asg    _copy_in, COPY_IN_RTN
   .else
        .asg    $copy_in, COPY_IN_RTN
   .endif

	.global	__TI_auto_init

;****************************************************************************
; 16-BIS ROUTINE
;****************************************************************************
   .if .TMS470_16BIS

        .state16

        .thumbfunc __TI_auto_init

__TI_auto_init:    .asmfunc stack_usage(20)
        PUSH    {r4-r7, lr}
        ;*------------------------------------------------------
        ;* PROCESS BINIT LINKER COPY TABLE.  IF BINIT IS -1, THEN
	;* THERE IS NONE.
        ;*------------------------------------------------------
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	MOVW	r0, binit
	MOVT	r0, binit
	.state16
	.else
	LDR	r0, c_binit
	.endif
	MOV	r7, #1
	CMN	r0, r7
	BEQ	_b1_
        BL      COPY_IN_RTN

	.if __TI_EABI_ASSEMBLER
        ;*------------------------------------------------------
        ;* If eabi, process the compressed cinit table. The format
        ;* is as follows:
	;* |4-byte load addr|4-byte run addr|
	;* |4-byte load addr|4-byte run addr|
        ;*
	;* Processing steps:
        ;*   1. Read load and run address.
        ;*   2. Read one byte at load address, say idx.
        ;*   3. Get pointer to handler at handler_start[idx]
        ;*   4. call handler(load_addr + 1, run_addr)
        ;*------------------------------------------------------
_b1_:
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	MOVW	r5, __TI_CINIT_Base
	MOVT	r5, __TI_CINIT_Base
	MOVW	r7, __TI_CINIT_Limit
	MOVT	r7, __TI_CINIT_Limit
	MOVW	r6, __TI_Handler_Table_Base
	MOVT	r6, __TI_Handler_Table_Base
	.state16
	.else
        LDR     r5, c_cinit_start
        LDR     r7, c_cinit_end
        LDR     r6, handler_start
	.endif
_b1_loop_:
        CMP     r5,r7
        BCS     _b1_loop_end_
        LDMIA   r5!, {r0,r1}
        LDRB    r4, [r0]
        LSL     r4, r4, #2
        LDR     r4, [r6,r4]
        ADD     r0, r0, #1
        .if !__TI_TMS470_V4__
	BLX	r4
	.else
	BL      IND$CALL
        .endif
        B       _b1_loop_
_b1_loop_end_:
        .else
        ;*------------------------------------------------------
        ;* PERFORM AUTO-INITIALIZATION.  IF CINIT IS -1, THEN
	;* THERE IS NONE.
        ;*------------------------------------------------------
_b1_:
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	MOVW	r0, cinit
	MOVT	r0, cinit
	.state16
	.else
	LDR	r0, c_cinit
	.endif
	MOV	r7, #1
	CMN	r0, r7
	BEQ	_c1_
        BL      perform_cinit
	.endif

_c1_:
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	.if __TI_EABI_ASSEMBLER
	MOVW    r5, SHT$$INIT_ARRAY$$Base
	MOVT    r5, SHT$$INIT_ARRAY$$Base
	.else
	MOVW	r5, pinit
	MOVT	r5, pinit
	.endif
	.state16
	.else
	LDR	r5, c_pinit
	.endif
        .if (!__TI_EABI_ASSEMBLER)
        ;*------------------------------------------------------
        ;* IN NON-EABI MODE, THERE IS NO INITIALIZATION ROUTINE
        ;* IF PINIT IS -1. ALSO, PINT IS NULL TERMINATED. ITERATE
        ;* OVER THE PINIT TABLE AND CALL THE INITIALIZATION ROUTINE
        ;* FOR CONSTRUCTORS.
	;* NOTE THAT r7 IS PRESERVED ACROSS AUTO-INITIALIZATION.
        ;*------------------------------------------------------
	CMN	r5, r7
	BEQ	_c3_
	B	_c2_
_loop_:
        .if !__TI_TMS470_V4__
	BLX	r4
	.else
        BL      IND$CALL
        .endif
_c2_:	LDMIA	r5!, {r4}
	CMP	r4, #0
	BNE	_loop_
_c3_:
        .else
        ;*------------------------------------------------------
        ;* IN EABI MODE, INIT_ARRAY IS NOT NULL TERMINATED. START
        ;* FROM THE INIT_ARRAY START (C_PINIT) AND ITERATE TILL
        ;* INIT_ARRAY END (C_PINT_END)
        ;*------------------------------------------------------
	.if __TI_AVOID_EMBEDDED_CONSTANTS
	.thumb
	MOVW	r7, SHT$$INIT_ARRAY$$Limit
	MOVT	r7, SHT$$INIT_ARRAY$$Limit
	.state16
	.else
        LDR    r7, c_pinit_end
	.endif
_loop_:
        CMP     r5, r7
        BCS     _loop_end_      ; If r5 is GE r7, we have reached the end.
        LDMIA   r5!, {r4}
        .if !__TI_TMS470_V4__
	BLX	r4
	.else
        BL      IND$CALL
        .endif
        B       _loop_
_loop_end_:
        .endif

	.if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
        POP     {r4-r7, pc}             ;
        .else
        POP     {r4-r7}                 ;
	POP	{r0}
	MOV	lr, r0
        BX      lr
        .endif

	.endasmfunc


;***************************************************************************
;*  PROCESS INITIALIZATION TABLE.
;*
;*  THE TABLE CONSISTS OF A SEQUENCE OF RECORDS OF THE FOLLOWING FORMAT:
;*
;*       .word  <length of data (bytes)>
;*       .word  <address of variable to initialize>
;*       .word  <data>
;*
;*  THE INITIALIZATION TABLE IS TERMINATED WITH A ZERO LENGTH RECORD.
;*
;***************************************************************************

tbl_addr  .set    r0
var_addr  .set    r1
tmp	  .set    r2
length:   .set    r3
data:     .set    r4
three:    .set    r5

perform_cinit: .asmfunc
	MOV	three, #3		   ;
	B	rec_chk			   ;

        ;*------------------------------------------------------
	;* PROCESS AN INITIALIZATION RECORD
        ;*------------------------------------------------------
record:	LDR	var_addr, [tbl_addr, #4]   ;
	ADD	tbl_addr, #8		   ;

        ;*------------------------------------------------------
	;* COPY THE INITIALIZATION DATA
        ;*------------------------------------------------------
	MOV	tmp, var_addr		   ; DETERMINE ALIGNMENT
	AND	tmp, three		   ; AND COPY BYTE BY BYTE
	BNE	_bcopy			   ; IF NOT WORD ALIGNED

	MOV	tmp, length		   ; FOR WORD COPY, STRIP
	AND	tmp, three		   ; OUT THE NONWORD PART
	BIC	length, three		   ; OF THE LENGTH
	BEQ	_wcont			   ;

_wcopy:	LDR	data, [tbl_addr]	   ;
	ADD	tbl_addr, #4		   ;
	STR	data, [var_addr]	   ; COPY A WORD OF DATA
	ADD	var_addr, #4		   ;
	SUB	length, #4		   ;
	BNE	_wcopy                     ;
_wcont:	MOV	length, tmp		   ;
	BEQ	_cont			   ;

_bcopy:	LDRB	data, [tbl_addr]	   ;
	ADD	tbl_addr, #1		   ;
	STRB	data, [var_addr]	   ; COPY A BYTE OF DATA
	ADD	var_addr, #1		   ;
	SUB	length, #1		   ;
	BNE	_bcopy                     ;

_cont:	MOV	tmp, tbl_addr	           ;
	AND	tmp, three	           ; MAKE SURE THE ADDRESS
	BEQ	rec_chk			   ; IS WORD ALIGNED
	BIC	tbl_addr, three		   ;
	ADD	tbl_addr, #0x4             ;

rec_chk:LDR	length, [tbl_addr]         ; PROCESS NEXT
	CMP	length, #0                 ; RECORD IF LENGTH IS
	BNE	record                     ; NONZERO

	BX	lr
	.endasmfunc

   .else    ; !.TMS470_16BIS

	.armfunc __TI_auto_init

        .state32

;****************************************************************************
;*  AUTO INIT ROUTINE                                                       *
;****************************************************************************

	.global	__TI_auto_init
;***************************************************************
;* FUNCTION DEF: _c_int00
;***************************************************************
__TI_auto_init: .asmfunc stack_usage(20)

	STMFD	sp!, {r4-r7, lr}
        ;*------------------------------------------------------
        ;* PROCESS BINIT LINKER COPY TABLE.  IF BINIT IS -1, THEN
	;* THERE IS NONE.
        ;*------------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    r0, binit
        MOVT    r0, binit
        .else
	LDR	r0, c_binit
        .endif
	CMN	r0, #1
        BLNE    COPY_IN_RTN

	.if __TI_EABI_ASSEMBLER
        ;*------------------------------------------------------
        ;* If eabi, process the compressed cinit table. The format
        ;* is as follows:
	;* |4-byte load addr|4-byte run addr|
	;* |4-byte load addr|4-byte run addr|
        ;*
	;* Processing steps:
        ;*   1. Read load and run address.
        ;*   2. Read one byte at load address, say idx.
        ;*   3. Get pointer to handler at handler_start[idx]
        ;*   4. call handler(load_addr + 1, run_addr)
        ;*------------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    r5, __TI_CINIT_Base
        MOVT    r5, __TI_CINIT_Base
        MOVW    r7, __TI_CINIT_Limit
        MOVT    r7, __TI_CINIT_Limit
        MOVW    r6, __TI_Handler_Table_Base
        MOVT    r6, __TI_Handler_Table_Base
        .else
        LDR     r5, c_cinit_start
        LDR     r7, c_cinit_end
        LDR     r6, handler_start
        .endif
_b1_loop_:
        CMP     r5,r7
        BCS     _b1_loop_end_
        LDMIA   r5!, {r0,r1}
        LDRB    r4, [r0]
        MOV     r4, r4, LSL #2
        LDR     r4, [r6,r4]
        ADD     r0, r0, #1
        .if !__TI_TMS470_V4__
	BLX	r4
	.else
        BL      IND_CALL
        .endif
        B       _b1_loop_
_b1_loop_end_:
        .else
        ;*------------------------------------------------------
        ;* PERFORM COFF MODE AUTO-INITIALIZATION.  IF CINIT IS -1, THEN
	;* THERE IS NO CINIT RECORDS TO PROCESS.
        ;*------------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    r0, cinit
        MOVT    r0, cinit
        .else
	LDR	r0, c_cinit
        .endif
	CMN	r0, #1
        BLNE    perform_cinit
        .endif

        ;*------------------------------------------------------
	;* CALL INITIALIZATION ROUTINES FOR CONSTRUCTORS. IF
	;* PINIT IS -1, THEN THERE ARE NONE.
        ;*------------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        .if __TI_EABI_ASSEMBLER
        MOVW    r5, SHT$$INIT_ARRAY$$Base
        MOVT    r5, SHT$$INIT_ARRAY$$Base
        .else
        MOVW    r5, pinit
        MOVT    r5, pinit
        .endif
        .else
	LDR	r5, c_pinit
        .endif
        .if (!__TI_EABI_ASSEMBLER)
        ;*------------------------------------------------------
        ;* IN NON-EABI MODE, THERE IS NO INITIALIZATION ROUTINE
        ;* IF PINIT IS -1. ALSO, PINT IS NULL TERMINATED. ITERATE
        ;* OVER THE PINIT TABLE AND CALL THE INITIALIZATION ROUTINE
        ;* FOR CONSTRUCTORS.
        ;*------------------------------------------------------
	CMN	r5, #1
	BEQ	_c2_
	B	_c1_
_loop_:
        .if !__TI_TMS470_V4__
	BLX	r4
	.else
        BL      IND_CALL
        .endif
_c1_:	LDR	r4, [r5], #4
	CMP	r4, #0
	BNE	_loop_
_c2_:
        .else
        ;*------------------------------------------------------
        ;* IN EABI MODE, INIT_ARRAY IS NOT NULL TERMINATED. START
        ;* FROM THE INIT_ARRAY START (C_PINIT) AND ITERATE TILL
        ;* INIT_ARRAY END (C_PINT_END)
        ;*------------------------------------------------------
        .if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW   r7, SHT$$INIT_ARRAY$$Limit
        MOVT   r7, SHT$$INIT_ARRAY$$Limit
        .else
        LDR    r7, c_pinit_end
        .endif
_loop_:
        CMP     r5, r7
        BCS     _loop_end_      ; If r5 is GE r7, we have reached the end.
        LDR     r4, [r5], #4
        .if !__TI_TMS470_V4__
	BLX	r4
	.else
        BL      IND_CALL
        .endif
        B       _loop_
_loop_end_:
        .endif

	.if __TI_ARM7ABI_ASSEMBLER | __TI_ARM9ABI_ASSEMBLER | !__TI_TMS470_V4__
	LDMFD	sp!, {r4-r7, pc}	;
	.else
	LDMFD	sp!, {r4-r7, lr}
	BX	lr
	.endif

	.endasmfunc

;***************************************************************************
;*  PROCESS INITIALIZATION TABLE.
;*
;*  THE TABLE CONSISTS OF A SEQUENCE OF RECORDS OF THE FOLLOWING FORMAT:
;*
;*       .word  <length of data (bytes)>
;*       .word  <address of variable to initialize>
;*       .word  <data>
;*
;*  THE INITIALIZATION TABLE IS TERMINATED WITH A ZERO LENGTH RECORD.
;*
;***************************************************************************

tbl_addr: .set    R0
var_addr: .set    R1
length:   .set    R2
data:     .set    R3

perform_cinit: .asmfunc
	B	rec_chk

        ;*------------------------------------------------------
	;* PROCESS AN INITIALIZATION RECORD
        ;*------------------------------------------------------
record:	LDR	var_addr, [tbl_addr], #4   ;

        ;*------------------------------------------------------
	;* COPY THE INITIALIZATION DATA
        ;*------------------------------------------------------
	TST	var_addr, #3		   ; SEE IF DEST IS ALIGNED
	BNE     _bcopy			   ; IF NOT, COPY BYTES
	SUBS	length, length, #4	   ; IF length <= 3, ALSO
	BMI     _bcont			   ; COPY BYTES

_wcopy:	LDR	data, [tbl_addr], #4  	   ;
	STR	data, [var_addr], #4	   ; COPY A WORD OF DATA
	SUBS	length, length, #4	   ;
	BPL	_wcopy			   ;
_bcont:	ADDS	length, length, #4	   ;
	BEQ	_cont			   ;

_bcopy:	LDRB	data, [tbl_addr], #1       ;
	STRB	data, [var_addr], #1       ; COPY A BYTE OF DATA
	SUBS	length, length, #1	   ;
	BNE	_bcopy                     ;

_cont:	ANDS	length, tbl_addr, #0x3     ; MAKE SURE THE ADDRESS
	RSBNE	length, length, #0x4       ; IS WORD ALIGNED
	ADDNE	tbl_addr, tbl_addr, length ;

rec_chk:LDR	length, [tbl_addr], #4     ; PROCESS NEXT
	CMP	length, #0                 ; RECORD IF LENGTH IS
	BNE	record                     ; NONZERO

	BX	LR
	.endasmfunc

   .endif    ; !.TMS470_16BIS

;***************************************************************
;* CONSTANTS USED BY THIS MODULE
;***************************************************************
   .if !__TI_AVOID_EMBEDDED_CONSTANTS
c_binit       	.long    binit

   .if __TI_EABI_ASSEMBLER
c_pinit       	.long    SHT$$INIT_ARRAY$$Base
c_pinit_end     .long    SHT$$INIT_ARRAY$$Limit
c_cinit_start   .long    __TI_CINIT_Base
c_cinit_end     .long    __TI_CINIT_Limit
handler_start   .long    __TI_Handler_Table_Base
   .else
c_pinit       	.long    pinit
c_cinit       	.long    cinit
   .endif
   .endif

;******************************************************
;* UNDEFINED REFERENCES                               *
;******************************************************
	.global	binit
	.global	cinit
	.global	COPY_IN_RTN

   .if __TI_TMS470_V4__
   .if .TMS470_16BIS
	.global IND$CALL
   .else
	.global IND_CALL
   .endif
   .endif

   .if __TI_EABI_ASSEMBLER
        .weak   SHT$$INIT_ARRAY$$Base
        .weak   SHT$$INIT_ARRAY$$Limit
        .weak   __TI_CINIT_Base
        .weak   __TI_CINIT_Limit
        .weak   __TI_Handler_Table_Base
   .else
	.global	pinit
   .endif

	.end
