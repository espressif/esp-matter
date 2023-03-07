// lzma-decompression.s : implementation file
//
// Copyright(C) 2022 Qorvo Inc.
// All rights reserved.
//
// Copyright(C) 2020 ubisys technologies GmbH, Duesseldorf, Germany.
// All rights reserved.
//
// www.ubisys.de
// support@ubisys.de
//
// This is an implementation of LZMA decompression written by Igor Pavlov and
// hand-optimized by John F. Reiser, adapted by ubisys to the IAR assembler
// and C/C++ EABI addressing some issues with parameter passing. This version
// is also using a thumb entry point and invokes external functions to read
// and write from the target storage memory. Modifications by Qorvo 
// to be compatible with the GCC assembler were done in 2022.
//
// Hand compiled Copyright (c) 2006-2007 John F. Reiser  (2007-06-18)
// from modified LzmaDecode.c.
// LZMA SDK 4.40 Copyright (c) 1999-2006 Igor Pavlov (2006-05-01)
//
// This file is licensed under either of these two licenses:
//   1) GNU Lesser General Public License (GNU LGPL)
//   2) Common Public License (CPL)
// See files LGPL.txt and CPL.html for the text of the licenses.

    .syntax unified
    .thumb
    .align 1
    .text

    .extern lzma_initialize, read_output, write_output

kLzmaStreamWasFinishedId= (-1)
kNumTopBits= 24
kTopValue= 1<<kNumTopBits
kNumBitModelTotalBits= 11
kBitModelTotal= (1 << kNumBitModelTotalBits)
kNumMoveBits= 5
kNumPosBitsMax= 4
kNumPosStatesMax= (1 << kNumPosBitsMax)
kLenNumLowBits= 3
kLenNumLowSymbols= (1 << kLenNumLowBits)
kLenNumMidBits= 3
kLenNumMidSymbols= (1 << kLenNumMidBits)
kLenNumHighBits= 8
kLenNumHighSymbols= (1 << kLenNumHighBits)
kLenNumLowSymbols_p_kLenNumMidSymbols= kLenNumLowSymbols + kLenNumMidSymbols
LenChoice= 0
LenChoice2= (LenChoice + 1)
LenLow= (LenChoice2 + 1)
LenMid= (LenLow + (kNumPosStatesMax << kLenNumLowBits))
LenMid__1= LenMid<<1
LenHigh= (LenMid + (kNumPosStatesMax << kLenNumMidBits))
LenHigh__1= LenHigh<<1
kNumLenProbs= (LenHigh + kLenNumHighSymbols)
kNumStates= 12
kNumLitStates= 7
kStartPosModelIndex= 4
kEndPosModelIndex= 14
kNumFullDistances= (1 << (kEndPosModelIndex >> 1))
kNumPosSlotBits= 6
kNumLenToPosStates= 4
kNumAlignBits= 4
kAlignTableSize= (1 << kNumAlignBits)
kMatchMinLen= 2
Init0x400= ((kBitModelTotal>>1)<<16) | (kBitModelTotal>>1)
IsMatch= 0
IsRep= (IsMatch + (kNumStates << kNumPosBitsMax))
IsRepG0= (IsRep + kNumStates)
IsRepG1= (IsRepG0 + kNumStates)
IsRepG2= (IsRepG1 + kNumStates)
IsRep0Long= (IsRepG2 + kNumStates)
PosSlot= (IsRep0Long + (kNumStates << kNumPosBitsMax))
PosSlot__1= PosSlot<<1
SpecPos= (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
SpecPos_m1__1= (SpecPos -1)<<1
Align= (SpecPos + kNumFullDistances - kEndPosModelIndex)
Align__1= Align<<1
LenCoder= (Align + kAlignTableSize)
LenCoder__1= LenCoder<<1
RepLenCoder= (LenCoder + kNumLenProbs)
RepLenCoder__1= RepLenCoder<<1
Literal=  (RepLenCoder + kNumLenProbs)
Literal__1= Literal<<1
LZMA_BASE_SIZE= Literal  // 1846
LZMA_LIT_SIZE =  768

/* lzma_decode(r0=vs, r1=inStream, r2=inSize, r3= &inSizeProcessed,
    [sp,#0]= outStream, [sp,#4]= outSize, [sp,#8]= &outSizeProcessed)
*/

// ==========================================================================
// On Entry
// --------------------------------------------------------------------------
// SP + 4 * 0 | xxxx | outStream pointer, in argument
// --------------------------------------------------------------------------
// SP + 4 * 1 | xxxx | outSize pointer, in argument
// --------------------------------------------------------------------------
// SP + 4 * 2 | xxxx | outSizeProcessed, out argument
// --------------------------------------------------------------------------

// ==========================================================================
// After setup stage
// --------------------------------------------------------------------------
// SP + 4 *  0 | R0** | rep1
// --------------------------------------------------------------------------
// SP + 4 *  1 | R1** | rep2
// --------------------------------------------------------------------------
// SP + 4 *  2 | R2** | rep3
// --------------------------------------------------------------------------
// SP + 4 *  3 | R3** | state
// --------------------------------------------------------------------------
// SP + 4 *  4 | R4** | posStateMask
// --------------------------------------------------------------------------
// SP + 4 *  5 | R5** | litPosMask
// --------------------------------------------------------------------------
// SP + 4 *  6 | R6** | lc
// --------------------------------------------------------------------------
// SP + 4 *  7 | R7** | m_len
// ==========================================================================
// SP + 4 *  8 | R8*  | inBuf
// --------------------------------------------------------------------------
// SP + 4 *  9 | R9*  | outBuf
// --------------------------------------------------------------------------
// SP + 4 * 10 | R10* | outLim
// --------------------------------------------------------------------------
// SP + 4 * 11 | R11* | posState
// --------------------------------------------------------------------------
// SP + 4 * 12 | R12* | inLim
// ==========================================================================
// SP + 4 * 13 | R8   | preserved register R8 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 14 | R9   | preserved register R9 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 15 | R10  | preserved register R10 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 16 | R11  | preserved register R11 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 17 | R3   | argument: inSizeProcessed
// --------------------------------------------------------------------------
// SP + 4 * 18 | R4   | preserved register R4 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 19 | R5   | preserved register R5 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 20 | R6   | preserved register R6 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 21 | R7   | preserved register R7 (caller context)
// --------------------------------------------------------------------------
// SP + 4 * 22 | LR   | return address
// ==========================================================================
// SP + 4 * 23 | xxxx | outStream pointer, in argument
// --------------------------------------------------------------------------
// SP + 4 * 24 | xxxx | outSize pointer, in argument
// --------------------------------------------------------------------------
// SP + 4 * 25 | xxxx | outSizeProcessed, out argument
// --------------------------------------------------------------------------

    .section .text.lzma_decode, "ax", %progbits
    .thumb_func
    .type   lzma_decode, %function
    .global lzma_decode

lzma_decode:
    // save &inSizeProcessed and caller registers
    push { r3 - r7, lr }

    mov r4, r8
    mov r5, r9
    mov r6, r10
    mov r7, r11

    push { r4 - r7 }

    adds r7,r2,r1  // inLim
    mov r12, r7
    ldr r4,[sp, #10*4 + 0*4]  // outStream
    ldr r3,[sp, #10*4 + 1*4]  // outSize
    adds r5,r3,r4  // outLim
    ldr r6,[r0, #4]
    mov r8, r1
    mov r9, r4
    mov r10, r5
    mov r11, r6

    push { r1, r4 - r7 }

    ldrb r6,[r0, #0]        // lc
    ldrb r5,[r0, #1]        // lp
    ldrb r4,[r0, #2]        // ps

    mov lr,r5
    add lr,r6  // lp + lc
    movs r0,#1
    lsls r0,r5
    subs r5,r0,#1  // (1<<lp)-1  litPosMask
    movs r0,#1
    lsls r0,r4
    subs r4,r0,#1  // (1<<pb)-1  posStateMask
    movs r3,#0
    mvns r7,r3
    movs r2,#1
    movs r1,#1
    movs r0,#1
    push {r0-r7}

    movs r4,#0
    movs r6,#1

    ldr r2,=LZMA_LIT_SIZE
    mov r3,lr
    lsls r2,r3  // LZMA_LIT_SIZE << (lp + lc)
    ldr r3,=LZMA_BASE_SIZE
    adds r2,r2,r3
    mov r1,r11
    ldr r0,=Init0x400

L10:
    str r0,[r1]
    adds r1,#4
    subs r2,#2
    bhi L10

    movs r0,#5
    add r0,r8  // sentinel

L20:
    bl rcInit2
    cmp r0,r8
    bne L20

L200:  // main loop; In: prevB
    ldr r3,[sp, #9*4] // outBuf
    ldr r2,[sp, #4*4]   // posStateMask
    mov r5,r9
    ands r2,r5
    subs r5, r5,r3
    str r2,[sp, #11*4] // posState
    movs r0,#IsMatch
    bl rcGetBit_mi_pstateSH0
    bne L270

    ldr r3,[sp, #5*4] // litPosMask
    ldr r2,[sp, #6*4]   // lc
    ands r3,r3,r5

    movs r0,r4  // last use of prevB
    lsls r3,r2  // (nowPos & litPosMask)<<lc
    subs r2,#8
    negs r2,r2
    lsrs r0,r2
    adds r3,r3,r0  // + (prevB >> (8- lc))
    lsls r2,r3,#1
    adds r3,r3,r2  // *3
    ldr r2,=Literal__1
    add r2,r11
    lsls r3, r3, #1+8  // *3 *256 *2
    adds r3,r3,r2
    mov r10,r3
    movs r0,#1

    ldr r3,[sp, #3*4] // state
    cmp r3,#kNumLitStates
    blo L240
L205:
    mov r3,r9
    subs r3,r3,r6
    bl read_output
    movs r5, r4
L210:  // symbol === mi === mo
    movs r4,#0x80  // no 0x100 in thumb mode
    lsls r1,r4,#2  // 2*0x100
    ands r4,r5  // pick top bit before doubling
    lsls r4,r4,#2  // scale for ushort
    lsls r5,r5,#1
    adds r1,r1,r4
    add r1,r10
    bl rcGetBit_mi
    movs r3,#1  // thumb has no and-immediate
    lsrs r4,r4,#1+ 8  // unscale
    ands r3,r0
    cmp r3,r4
    bne L243  // break
    cmp r0,#0xFF
    bls L210
    b L245

L240:  // symbol === mi === mo
    mov r1,r10
    bl rcGetBit_mi
L243:
    cmp r0,#0xFF
    bls L240
L245:
    ldr r2,[sp, #3*4] // state
    movs r3,r2
    cmp r2,#4
    blo _0
    movs r3,#3
    cmp r2,#10
    blo _0
    movs r3,#6
_0:
    subs r2,r2,r3
    str r2,[sp, #3*4] // state
    b L298

L270:
    movs r1,#IsRep
    bl rcGetBit_1pstate0
    bne L290
    ldr r3,[sp, #1*4] // rep2
    ldr r2,[sp, #0*4] // rep1
    str r3,[sp, #2*4] // rep3
    str r2,[sp, #1*4] // rep2
    str r6,[sp, #0*4] // rep1

    ldr r2,[sp, #3*4] // state
    movs r3,#0
    cmp r2,#kNumLitStates
    blo _1
    movs r3,#3
_1:
    str r3,[sp, #3*4] // state

    ldr r3,=LenCoder__1
    add r3,r11
    mov r10,r3
    b L350

L290:
    movs r1,#IsRepG0
    bl rcGetBit_1pstate0
    bne L300

L293:
    movs r0,#IsRep0Long
    bl rcGetBit_mi_pstateSH0
    bne L340

L295:
    ldr r2,[sp, #3*4] // state
    movs r3,#9
    cmp r2,#kNumLitStates
    blo _2
    movs r3,#11
_2:
    str r3,[sp, #3*4] // state

L297:
    ldr r3,[sp, #9*4] //outBuf
    mov r2,r9
    subs r3,r2,r3  // nowPos
    cmp r3,r6
    blo lzmaDataError
    subs r3,r2,r6  // outPtr - rep0
    bl read_output
    movs r0, r4

L298:
    mov r2,r9
    movs r3,#1
    movs r4, r0
    bl write_output // Stores decoded elements at the output location
    add r9,r3
    movs r4,#0xff
    ands r4,r0  // subtracts 0x100 if necessary
    b L520

L300:
    movs r1,#IsRepG1
    bl rcGetBit_1pstate0
    ldr r4,[sp, #0*4] // rep1
    beq L330

L310:
    movs r1,#IsRepG2
    bl rcGetBit_1pstate0
    ldr r4,[sp, #1*4] // rep2
    beq L325

L320:
    ldr r3,[sp, #1*4] // rep2
    ldr r4,[sp, #2*4] // rep3
    str r3,[sp, #2*4] // rep3

L325:
    ldr r3,[sp, #0*4] // rep1
    str r3,[sp, #1*4] // rep2

L330:
    str r6,[sp, #0*4] // rep1
    movs r6,r4

L340:
    ldr r2,[sp, #3*4] // state
    movs r3,#8
    cmp r2,#kNumLitStates
    blo _3
    movs r3,#11
_3:
    str r3,[sp, #3*4] // state

    ldr r3,=RepLenCoder__1
    add r3,r11
    mov r10,r3
L350:
    movs r1,#LenChoice<<1
    add r1,r10
    bl rcGetBit_0
    bne L360
    movs r5,#LenLow<<1
    add r5,r10
    movs r2,#0
    ldr r3,[sp, #11*4] // posState
    lsls r3,r3, #1+ kLenNumLowBits
    adds r5,r5,r3
    movs r4,#1<<kLenNumLowBits
    b L390

L360:
    movs r1,#LenChoice2<<1
    add r1,r10
    bl rcGetBit_0
    bne L370
    ldr r5,=LenMid__1
    add r5,r10
    movs r2,#kLenNumLowSymbols
    ldr r3,[sp, #11*4] // posState
    lsls r3,r3,#1+ kLenNumMidBits
    adds r5,r5,r3
    movs r4,#1<<kLenNumMidBits
    b L390

L370:
    ldr r5,=LenHigh__1
    add r5,r10
    ldr r2,=kLenNumLowSymbols_p_kLenNumMidSymbols
    movs r4,#1
    lsls r4,r4,#kLenNumHighBits

L390:
    str r2,[sp, #7*4] // m_len
    movs r0,#1

L395:  // RangeDecoderBitTreeDecode
    movs r1,r5
    bl rcGetBit_mi
    subs r3,r0,r4
    blo L395
    ldr r5,[sp, #7*4] // m_len
    adds r5,r5,r3
    str r5,[sp, #7*4] // m_len
    ldr r3,[sp, #3*4] // state
    cmp r3,#4
    bhs L500
/*L400:*/
    adds r3,#kNumLitStates
    str r3,[sp, #3*4] // state

    movs r3,r5
    cmp r5,#kNumLenToPosStates
    blo _4
    movs r3,#kNumLenToPosStates -1
_4:
    movs r5,r3

    lsls r5,r5,#1+ kNumPosSlotBits
    add r5,r11
    ldr r3,=PosSlot__1
    adds r5,r5,r3
    movs r0,#1
    movs r4,#1<<kNumPosSlotBits

L403:  // RangeDecoderBitTreeDecode
    movs r1,r5
    bl rcGetBit_mi
    subs r3,r0,r4
    blo L403
    str r3,[sp, #11*4] // posSlot

    cmp r3,#kStartPosModelIndex
    blo L460

L405:
    lsrs r4,r3,#1
    subs r4,#1
    movs r6,#1
    ands r6,r3
    adds r6,#2  // same as OR
    cmp r3,#kEndPosModelIndex
    bhs L410

L407:
    lsls r6,r4
    subs r3,r6,r3  // r_posSlot dies
    ldr r2,=SpecPos_m1__1
    mov r10,r11
    add r10,r2
    lsls r3,r3,#1
    add r10,r3
    b L438

lzmaDataError:
    movs r0,#1  // failure
    b lzmaExit

L410:
    subs r4,#kNumAlignBits

L420:
    bl rcNormalize
    lsrs r7,r7,#1
    lsls r6,r6,#1
    cmp r12,r7
    blo L430
    adds r6,#1  // same as OR
    negs r3,r7
    add r12,r3  // if (Code>=Range) Code-=Range;

L430:
    subs r4,#1
    bne L420
    ldr r3,=Align__1
    add r3,r11
    mov r10,r3
    lsls r6,r6,#kNumAlignBits
    movs r4,#kNumAlignBits

L438:
    movs r5,#1
    movs r0,#1

L440:
    mov r1,r10
    bl rcGetBit_mi
    movs r3,#1
    tst r0,r3
    beq L445
    orrs r6,r5

L445:
    lsls r5, r5,#1
    subs r4,#1
    bne L440
    b L465

L450:
L460:
    ldr r6,[sp, #11*4] // posSlot

L465:
    adds r6,#1

    ldr r5,[sp, #7*4] // m_len
L500:
    ldr r3,[sp, #9*4] //outBuf
    adds r5,#kMatchMinLen
    mov r2,r9
    subs r3,r2,r3  // nowPos
    cmp r6,r3
    bhi lzmaDataError

    ldr r1,[sp, #10*4] // outLim
    mov r2,r9
    subs r3,r2,r6

L510:                       // Append multiple bytes from past output
    bl read_output
    adds r3,#1
    bl write_output
    adds r2,#1
    cmp r2,r1
    bhs L515
    subs r5,#1
    bne L510

L515:
    mov r9,r2

L520:  // bottom of while loop
    ldr r2,[sp, #10*4] // outLim
    cmp r9,r2
    bhs _5
    b L200
_5:

L530:
    bl rcNormalize
    movs r0,#0  // success

lzmaExit:
    ldr r2,[sp, #8*4] // inBuf
    mov r3,r8
    subs r3,r3,r2
    ldr r2,[sp, #17*4] // inSizeProcessed
    str r3,[r2]

    ldr r2,[sp, #9*4] //outBuf
    mov r3,r9
    subs r3,r3,r2
    ldr r2,[sp, #25*4] // outSizeProcessed
    str r3,[r2]

    add sp,#(8+5 +1)*4  // +1: r3

    pop { r4 - r7 }

    pop { r1 - r3 }
    mov r8, r1
    mov r9, r2
    mov r10, r3

    pop { r1 - r2 }
    mov r11, r1
    bx r2

rcNormalize:
    movs r3,#1
    lsls r3,r3,#24
    cmp r7,r3
    blo _6
    mov pc,lr
_6:

rcLoad:
    ldr r3,[sp, #12*4] // inLim
    lsls r7,r7,#8
    cmp r3,r8
    beq lzmaDataError

rcInit2:  // must not use r2
    mov r3,r12
    lsls r3,r3,#8
    mov r12,r3
    mov r3,r8
    ldrb r3,[r3]
    add r12,r3
    movs r3,#1
    add r8,r3
    mov pc,lr

rcGetBit_mi_pstateSH0:
    ldr r3,[sp, #3*4] // state
    ldr r2,[sp, #11*4] // posState
    lsls r3,r3,#kNumPosBitsMax
    adds r0,r0,r2
    adds r0,r0,r3
    mov r1,r11
    b rcGetBit_mi0

rcGetBit_1pstate0:
    lsls r1,r1,#1

rcGetBit_pstate0:  // rcGetBit(0, state + p)
    add r1,r11

rcGetBit_state0:  // rcGetBit(0, state + p_in)
    ldr r0,[sp, #3*4] // state

rcGetBit_mi0:  // rcGetBit(0, mi + p_in)
    lsls r3,r0,#1
    adds r1,r1,r3

rcGetBit_0:  // rcGetBit(0, p_in)
    movs r0,#0

rcGetBit_mi:  // rcGetBit(mi, mi + p_in)
    lsls r3,r0,#1
    adds r1,r1,r3

rcGetBit:  // Out: CC set on mo
    movs r3,#1  // inline the first block of rcNormalize
    lsls r3, r3,#24
    cmp r7,r3
    bhs _7
    mov r2,lr
    bl rcLoad  // may not use r2
    mov lr,r2
_7:

    ldrh r2,[r1]
    lsrs r3,r7,#kNumBitModelTotalBits
    muls r3,r2
    cmp r12,r3
    bhs rcGetBit1

rcGetBit0:
    adcs r0,r0  // mo = (mi<<1) | (Code >= bound)
    movs r7,r3
    movs r3,#1
    lsls r3,r3,#kNumBitModelTotalBits
    subs r3,r3,r2
    lsrs r3,r3,#kNumMoveBits
    adds r2,r2,r3
    cmp r0,#0  // Set CC
    strh r2,[r1]
#ifdef _DEBUG  /*{*/
   b rcTail
#endif  /*}*/
    mov pc,lr

rcGetBit1:
    adcs r0,r0  // mo = (mi<<1) | (Code >= bound)
    negs r3,r3
    add r12, r3
    adds r7,r7,r3
    lsrs r3,r2,#kNumMoveBits
    subs r2,r2,r3
    cmp r0,#0  // Set CC
    strh r2,[r1]
#ifdef _DEBUG  /*{*/
   rcTail:
#endif  /*}*/
    mov pc,lr
