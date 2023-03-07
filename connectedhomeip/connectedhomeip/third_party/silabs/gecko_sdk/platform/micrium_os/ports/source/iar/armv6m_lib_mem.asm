;
;/***************************************************************************//**
; * @file
; * @brief CPU - Common - OS CPU Port File
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
;* @note       (1) This port targets the following:
;*               Core      : ARMv6M Cortex-M
;*               Mode      : Thumb
;*               Toolchain : IAR EWARM
;*******************************************************************************************************/

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

        PUBLIC  Mem_Copy


;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

        RSEG CODE:CODE:NOROOT(2)


;
;********************************************************************************************************
;                                             Mem_Copy()
;
; Description : Copy data octets from one buffer to another buffer.
;
; Argument(s) : p_dest      Pointer to destination memory buffer.
;
;               p_src       Pointer to source      memory buffer.
;
;               size        Number of data buffer octets to copy.
;
; Return(s)   : none.
;
; Note(s)     : (1) Null copies allowed (i.e. 0-octet size).
;
;               (2) Memory buffers NOT checked for overlapping.
;
;               (3) Modulo arithmetic is used to determine whether a memory buffer starts on a 'CPU_ALIGN'
;                   address boundary.
;
;               (4) ARM Cortex-M3 processors use a subset of the ARM Thumb-2 instruction set which does
;                   NOT support 16-bit conditional branch instructions but ONLY supports 8-bit conditional
;                   branch instructions.
;
;                   Therefore, branches exceeding 8-bit, signed, relative offsets :
;
;                   (a) CANNOT be implemented with     conditional branches; but ...
;                   (b) MUST   be implemented with non-conditional branches.
;********************************************************************************************************

; void  Mem_Copy (void        *p_dest,      ;  ==>  R0
;                 void        *p_src,       ;  ==>  R1
;                 CPU_SIZE_T   size)        ;  ==>  R2

Mem_Copy:
        CMP         R0, #0
        BNE         Mem_Copy_1
        BX          LR                      ; return if pdest == NULL

Mem_Copy_1:
        CMP         R1, #0
        BNE         Mem_Copy_2
        BX          LR                      ; return if psrc  == NULL

Mem_Copy_2:
        CMP         R2, #0
        BNE         Mem_Copy_3
        BX          LR                      ; return if size  == 0

Mem_Copy_3:
        STMFD       SP!, {R3-R7}            ; save registers R3 to R7 on stack

Chk_Align_32:                               ; check if both dest & src 32-bit aligned
        MOVS        R3, #0x03
        ANDS        R3, R3, R0
        MOVS        R4, #0x03
        ANDS        R4, R4, R1
        CMP         R3, R4
        BNE         Chk_Align_16            ; not 32-bit aligned, check for 16-bit alignment

        MOVS        R4, #0x04
        SUBS        R3, R4, R3
        MOVS        R4, #0x03
        ANDS        R3, R3, R4

Pre_Copy_1:
        CMP         R3, #1                  ; copy 1-2-3 bytes (to align to the next 32-bit boundary)
        BCC         Copy_32_1               ; start real 32-bit copy
        CMP         R2, #1                  ; check if any more data to copy
        BCS         Pre_Copy_1_Cont
        B           Mem_Copy_END            ;           no more data to copy (see Note #4b)

Pre_Copy_1_Cont:

        LDRB        R4, [R1]
        ADDS        R1, R1, #1
        STRB        R4, [R0]
        ADDS        R0, R0, #1
        SUBS        R3, R3, #1
        SUBS        R2, R2, #1
        B           Pre_Copy_1


Chk_Align_16:                               ; check if both dest & src 16-bit aligned
        MOVS        R3, #0x01
        ANDS        R3, R0, R3
        MOVS        R4, #0x01
        ANDS        R4, R1, R4
        CMP         R3, R4
        BEQ         Pre_Copy_2
        B           Copy_08_1               ; not 16-bit aligned, start 8-bit copy (see Note #4b)

Pre_Copy_2:
        CMP         R3, #1                  ; copy 1 byte (to align to the next 16-bit boundary)
        BCC         Copy_16_1               ; start real 16-bit copy
        LDRB        R4, [R1]
        ADDS        R1, #1
        STRB        R4, [R0]
        ADDS        R0, #1
        SUBS        R3, R3, #1
        SUBS        R2, R2, #1
        B           Pre_Copy_2


Copy_32_1:
        CMP         R2, #(04*05*12)         ; Copy 12 chunks of 5 32-bit words (240 octets per loop)
        BCC         Copy_32_2
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        SUBS        R2, R2, #(04*05*12)
        B           Copy_32_1

Copy_32_2:
        CMP         R2, #(04*05*01)         ; Copy chunks of 5 32-bit words (20 octets per loop)
        BCC         Copy_32_3
        LDMIA       R1!, {R3-R7}
        STMIA       R0!, {R3-R7}
        SUBS        R2, R2, #(04*05*01)
        B           Copy_32_2

Copy_32_3:
        CMP         R2, #(04*01*01)         ; Copy remaining 32-bit words
        BCC         Copy_16_1
        LDR         R3, [R1]
        ADDS        R1, R1, #4
        STR         R3, [R0]
        ADDS        R0 , R0, #4
        SUBS        R2, R2, #(04*01*01)
        B           Copy_32_3

Copy_16_1:
        CMP         R2, #(02*01*16)         ; Copy chunks of 16 16-bit words (32 bytes per loop)
        BCC         Copy_16_2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        SUBS         R2, R2, #(02*01*16)
        B           Copy_16_1

Copy_16_2:
        CMP         R2, #(02*01*01)         ; Copy remaining 16-bit words
        BCC         Copy_08_1
        LDRH        R3, [R1]
        ADDS        R1, #2
        STRH        R3, [R0]
        ADDS        R0, #2
        SUBS        R2, R2, #(02*01*01)
        B           Copy_16_2

Copy_08_1:
        CMP         R2, #(01*01*16)         ; Copy chunks of 16 8-bit words (16 bytes per loop)
        BCC         Copy_08_2
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        SUBS        R2, R2, #(01*01*16)
        B           Copy_08_1

Copy_08_2:
        CMP         R2, #(01*01*01)         ; Copy remaining 8-bit words
        BCC         Mem_Copy_END
        LDRB        R3, [R1]
        ADDS        R1, #1
        STRB        R3, [R0]
        ADDS        R0, #1
        SUBS        R2, R2, #(01*01*01)
        B           Copy_08_2


Mem_Copy_END:
        LDMFD       SP!, {R3-R7}            ; restore registers from stack
        BX          LR                      ; return


        END
