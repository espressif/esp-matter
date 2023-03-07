;
;  Copyright (c) 2018, Texas Instruments Incorporated
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
;
;  *  Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;
;  *  Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;
;  *  Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
;  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;

;
;  ======== pdm2pcm_cc26x2.sem4f ========
;  This file assumes TI Assembly Language Syntax.
;

  ; Determine whether input bitstream has big endian byte order (natural) or
  ; 16b little endian byte order (bytes #1, #0, #3, #2, #5, ... in sequence in memory)
;BITSTREAM_BE .set 1
BITSTREAM_LE16 .set 1

    .sect ".text:pdm2pcm16k"
    .thumb
    .global pdm2pcm16k

; bool pdm2pcm16k(const void* pIn, uint32_t* pState, const int32_t* pBqCoeffs, int16_t* pOut)
;
; Implements a balanced quality/performance PDM to PCM conversion from a 1.024 MHz PDM bitstream from
; a 4th order binary delta-sigma modulator to 16 kHz PCM output. Produces 32 output samples at a time and
; consumes 2048b or 256B of bitstream input.
; Params:
;   [in]    pIn          - Pointer to input bitstream (choose correct endianess for byte ordering!!!)
;   [inOut] pState       - Pointer to state struct (zero-initialize before use)
;   [in]    pBqCoeffs    - Pointer to biquad coefficients (see below for detail)
;   [out]   pOut         - Pointer to where samples are output
;
; Returns:
;   Boolean value: true means at least one output value saturated
;
; The signal processing algorithm performed is:
; - Decimation by 32 with a 4th order CIC decimator (from 1.024 MHz to 32 kHz). This decimator
;   has a fixed gain of +12 dB (i.e. a signal that is -12 dBFs will saturate output). If this
;   gain is not desired it can be adjusted by changing the gain coefficient of the first biquad
;   section in the following filter accordingly.
;     2048 samples PDM input (256B) -> 64 intermediate sample (32b) output [in place]
; - Intermediate signal is run through a N section biquad filter that should implement
;   a DC notch, a halfband filter and, ideally, passband correction for CIC attenuation
;   A fifth order filter structure that fulfills these requirements is given in the example
;   below.
;     64 intermediate sample (32b) input -> 64 intermediate sample (32b) outpur [in place]
; - A 1st order decimate by two CIC (i.e. mean of two and two samples) downsamples from 32 kHz
;   to 16 kHz
;     64 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - An optional N section user-defined biquad filter, that can be used to perform, for example,
;   EQ compensation
;     32 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - Saturation and output of 16b 2s complement PCM samples
;     32 intermediate sample (32b) input -> 32 output samples (16b) output
;
; The pBqCoeffs contains one record per biquad filter coefficient. The first filter is terminated by a
; single zero value and so is the second (two consecutive zero values at the end skips the second filter
; altogether. Each records has the following format:
; struct {
;     uint32_t g;      ; value of 0 here indicates end of IIR filter
;     int32_t  b1;
;     int32_t  b2;
;     int32_t  a1;
;     int32_t  a2;
; }
; Each second order section is assumed to have normalized both numerator and denominator,
; i.e b0 and a0 are 1. The gain g is applied to the input. If unormalized, g can be b0/a0
; and each coefficient in the numerator must then be divided by b0 and each coefficient
; in the denominator divided by a0.
; After the last section a dummy section with gain 0 indicates the end of the IIR filter.
; All coefficients must be in the SI.FFFF_FFFF_FF format, i.e 21 sign bits, one integer bit
; and ten fractional bits. Thus the range of the coefficients are [-2, 2) and the resolution
; is 2^-10
;
; All filter coefficients (including gain) can be changed dynamically if they reside in RAM.
; If, for any stage, the gain coefficient g >= 0x20000000 then it is treated as a pointer to a
; gain coefficent in RAM rather than as an actual gain coefficient. This allows the whole coefficient
; structure to reside in flash except for a few gain coefficients that need to be changed dynamically.
;
; The following filter coefficient structure has an appropriate first filter and shows where
; to insert the optional second filter:
;
; The following filter coefficient structure is the default filter and shows where
; to insert the optional second filter. It has five filter stages and a reasonable tradeoff between
; runtime and frequency response.
;
; int32_t aBqCoeffs[] = {
;    166,     0, -1024, -1356,   342,     // DC-notch, halfband LP filter
;    200,   789,   934,  -994,   508,
;    538,   381,   944,  -519,   722,
;    732,   124,   987,  -386,   886,
;    763,    11,  1014,  -386,   886,
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The following filter coefficients yield a flatter frequency response in the passband
; at the cost of an extra filter stage and thus more computational requirements.
; int32_t aBqCoeffs[] = {
;    // First filter H(z)=H1(z)*H2(z)*H3(z)*H4(z)*H5(z)*H6(z) [any order M supported]
;       345,   121,  1024,  -209,   919,  // H1(z): g, b1, b2, a1, a2
;       407,   765,  1024,  -376,   470,  // H2(z): g, b1, b2, a1, a2
;       392,     0, -1024, -1265,   245,  // H3(z): g, b1, b2, a1, a2
;       450,  1345,  1024,  -495,   269,  // H4(z): g, b1, b2, a1, a2
;       507,   400,  1024,  -296,   690,  // H5(z): g, b1, b2, a1, a2
;       554,   209,  1024,  -240,   870,  // H6(z): g, b1, b2, a1, a2
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The size of the state struct pointed to by pState is 4*(6+2N) bytes, where N is the
; total number of biquads in pNqCoeffs.
pdm2pcm16k:
    .asmfunc
    ; Save some arguments, clobbered registers and return address
    PUSH    {R0-R11, LR}

    ; ### FIRST STAGE (1.021 MHz --> 32 kHz) ###
    ; **** 4th order decimate-by-32 CIC ****
    ; Register usage is:
    ; R0:      pIn
    ; R1:      pState / byte read
    ; R2:      temp0
    ; R3:      temp1
    ; R4:      Acc0 (stored in state)
    ; R5:      Acc1 (stored in state)
    ; R6:      Acc2 (stored in state)
    ; R7:      Acc3
    ; R8:      Diff2 (stored in state)
    ; R9:      Diff1 (stored in state)
    ; R10:     Diff0 (stored in state)
    ; R11:     pOut
    ; R12:     nOutSamples
    ; LR/R14:  pLut

    MOV     R11, R0                     ; R11: pOut is copy of pIn [perform in place]
    MOV     R12, #32*2                  ; R12: nOutSamples
    LDR     R14, cicOrd4Bits8Pad0LutLoc16

    ; Get accumulator/differentiator state
    LDM     R1, {R4-R6, R8-R10}

    ; Preload first data byte
    .if $$defined("BITSTREAM_BE")
        LDRB    R1, [R0], #1            ; Read in 8b of bitstream
    .elseif $$defined("BITSTREAM_LE16")
        LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
    .else
        .emsg "Undefined bitstream input byte ordering"
    .endif

p2p16k_cic_SAMPLELOOP:
    ; To save two state variables and one differentiation we reset the innermost
    ; accumulator at the start of each decimation period
    MOV     R7, #0
    ; Constant used during CIC
    MOV     R3, #36
p2p16k_cic_BYTELOOP:           ; Byte loop is 2*20 cycles
    .loop 32/8/2
        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3      ; Acc3 += 120*Acc0
        MLA     R7, R5, R3, R7          ; Acc3 += 36 * Acc1
        ADD     R7, R7, R6, LSL #3      ; Acc3 += 8 * Acc2

        MLA     R6, R4, R3, R6          ; Acc2 += 36 * Acc0
        ADD     R6, R6, R5, LSL #3      ; Acc2 += 8 * Acc1

        ADD     R5, R5, R4, LSL #3      ; Acc1 += 8 * Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
           LDRB    R1, [R0], #2             ; Read in 8b of bitstream
        .endif

        UXTAB   R4, R4, R2              ; Acc0 += LUT0
        UXTAB   R5, R5, R2, ROR #8      ; Acc1 += LUT1
        UXTAB   R6, R6, R2, ROR #16     ; Acc2 += LUT2
        ADD     R7, R7, R2, LSR #23     ; Acc3 += LUT3
                                        ; LUT3 is treated as a 9 bit value, will have MSB from LUT2 as LSB in LUT3 (trade off for speed)

        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3      ; Acc3 += 120*Acc0
        MLA     R7, R5, R3, R7          ; Acc3 += 36 * Acc1
        ADD     R7, R7, R6, LSL #3      ; Acc3 += 8 * Acc2

        MLA     R6, R4, R3, R6          ; Acc2 += 36 * Acc0
        ADD     R6, R6, R5, LSL #3      ; Acc2 += 8 * Acc1

        ADD     R5, R5, R4, LSL #3      ; Acc1 += 8 * Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
            LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
        .endif

        UXTAB   R4, R4, R2              ; Acc0 += LUT0
        UXTAB   R5, R5, R2, ROR #8      ; Acc1 += LUT1
        UXTAB   R6, R6, R2, ROR #16     ; Acc2 += LUT2
        ADD     R7, R7, R2, LSR #23     ; Acc3 += LUT3
        ; LUT3 is treated as a 9 bit value, will have MSB from LUT2 as LSB in LUT3 (trade off for speed)
    .endloop

    ; End of decimation period, perform differentiation, output sample and see if we do more
    SUB     R3, R7, R8                      ; R3: Diff2out = Diff3out - DIff2 = Acc3 - Diff2
    MOV     R8, R7                          ; Diff2' = Diff3out = Acc3
    SUB     R2, R3, R9                      ; R2: Diff1out = Diff2out - Diff1
    MOV     R9, R3                          ; Diff1' = Diff2out
    SUB     R3, R2, R10                     ; R3: Diff0out = Diff1out - Diff0
    MOV     R10, R2                         ; Diff0' = Diff1out
    SUBS    R12, R12, #1                    ; nOutSamples--
    SUB     R3, R3, #0x80000;               ; Convert to 2s complement and SI3.F17 format (with 12 dB gain)
    STR     R3, [R11], #4
    BNE     p2p16k_cic_SAMPLELOOP

p2p16k_cic_EXIT:
    ; Get pointers to: R0: pIn/intermediateBuf, R1: pState, R2: pBqCoeffs
    POP     {R0-R2}
    ; Store accumulator/differentiator state
    STM     R1!, {R4-R6, R8-R10}
    ; p2pBiQuad expects intermediate buffer pointer on top of stack
    PUSH    {R0}
    B       p2pBiQuad
    .endasmfunc

cicOrd4Bits8Pad0LutLoc16:
    .word cicOrd4Bits8Pad0Lut

    .sect ".text:pdm2pcm8k"
    .thumb
    .global pdm2pcm8k

; bool pdm2pcm8k(const void* pIn, uint32_t* pState, const int32_t* pBqCoeffs, int16_t* pOut)
;
; Implements a balanced quality/performance PDM to PCM conversion from a 1.024 MHz PDM bitstream from
; a 4th order binary delta-sigma modulator to 8 kHz PCM output. Produces 32 output samples at a time and
; consumes 4096b or 512B of bitstream input.
; Params:
;   [in]    pIn          - Pointer to input bitstream (choose correct endianess for byte ordering!!!)
;   [inOut] pState       - Pointer to state struct (zero-initialize before use)
;   [in]    pBqCoeffs    - Pointer to biquad coefficients (see below for detail)
;   [out]   pOut         - Pointer to where samples are output
;
; Returns:
;   Boolean value: true means at least one output value saturated
;
; The signal processing algorithm performed is:
; - Decimation by 64 with a 4th order CIC decimator (from 1.024 MHz to 16 kHz). This decimator
;   has a fixed gain of +12 dB (i.e. a signal that is -12 dBFs will saturate output). If this
;   gain is not desired it can be adjusted by changing the gain coefficient of the first biquad
;   section in the following filter accordingly.
;     4096 samples PDM input (512B) -> 64 intermediate sample (32b) output [in place]
; - Intermediate signal is run through a N section biquad filter that should implement
;   a DC notch, a halfband filter and, ideally, passband correction for CIC attenuation
;   A fifth order filter structure that fulfills these requirements is given in the example
;   below.
;     64 intermediate sample (32b) input -> 64 intermediate sample (32b) outpur [in place]
; - A 1st order decimate by two CIC (i.e. mean of two and two samples) downsamples from 32 kHz
;   to 16 kHz
;     64 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - An optional N section user-defined biquad filter, that can be used to perform, for example,
;   EQ compensation
;     32 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - Saturation and output of 16b 2s complement PCM samples
;     32 intermediate sample (32b) input -> 32 output samples (16b) output
;
; The pBqCoeffs contains one record per biquad filter coefficient. The first filter is terminated by a
; single zero value and so is the second (two consecutive zero values at the end skips the second filter
; altogether. Each records has the following format:
; struct {
;     uint32_t g;      ; value of 0 here indicates end of IIR filter
;     int32_t  b1;
;     int32_t  b2;
;     int32_t  a1;
;     int32_t  a2;
; }
; Each second order section is assumed to have normalized both numerator and denominator,
; i.e b0 and a0 are 1. The gain g is applied to the input. If unormalized, g can be b0/a0
; and each coefficient in the numerator must then be divided by b0 and each coefficient
; in the denominator divided by a0.
; After the last section a dummy section with gain 0 indicates the end of the IIR filter.
; All coefficients must be in the SI.FFFF_FFFF_FF format, i.e 21 sign bits, one integer bit
; and ten fractional bits. Thus the range of the coefficients are [-2, 2) and the resolution
; is 2^-10
;
; All filter coefficients (including gain) can be changed dynamically if they reside in RAM.
; If, for any stage, the gain coefficient g >= 0x20000000 then it is treated as a pointer to a
; gain coefficent in RAM rather than as an actual gain coefficient. This allows the whole coefficient
; structure to reside in flash except for a few gain coefficients that need to be changed dynamically.
;
; The following filter coefficient structure is the default filter and shows where
; to insert the optional second filter. It has five filter stages and a reasonable tradeoff between
; runtime and frequency response.
;
; int32_t aBqCoeffs[] = {
;    166,     0, -1024, -1356,   342,     // DC-notch, halfband LP filter
;    200,   789,   934,  -994,   508,
;    538,   381,   944,  -519,   722,
;    732,   124,   987,  -386,   886,
;    763,    11,  1014,  -386,   886,
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The following filter coefficients yield a flatter frequency response in the passband
; at the cost of an extra filter stage and thus more computational requirements.
; int32_t aBqCoeffs[] = {
;    // First filter H(z)=H1(z)*H2(z)*H3(z)*H4(z)*H5(z)*H6(z) [any order M supported]
;       345,   121,  1024,  -209,   919,  // H1(z): g, b1, b2, a1, a2
;       407,   765,  1024,  -376,   470,  // H2(z): g, b1, b2, a1, a2
;       392,     0, -1024, -1265,   245,  // H3(z): g, b1, b2, a1, a2
;       450,  1345,  1024,  -495,   269,  // H4(z): g, b1, b2, a1, a2
;       507,   400,  1024,  -296,   690,  // H5(z): g, b1, b2, a1, a2
;       554,   209,  1024,  -240,   870,  // H6(z): g, b1, b2, a1, a2
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The size of the state struct pointed to by pState is 4*(6+2N) bytes, where N is the
; total number of biquads in pNqCoeffs.
pdm2pcm8k:
    .asmfunc
    ; Save some arguments, clobbered registers and return address
    PUSH    {R0-R11, LR}

    ; ### FIRST STAGE (1.021 MHz --> 32 kHz) ###
    ; **** 4th order decimate-by-32 CIC ****
    ; Register usage is:
    ; R0:      pIn
    ; R1:      pState / byte read
    ; R2:      temp0
    ; R3:      temp1
    ; R4:      Acc0 (stored in state)
    ; R5:      Acc1 (stored in state)
    ; R6:      Acc2 (stored in state)
    ; R7:      Acc3
    ; R8:      Diff2 (stored in state)
    ; R9:      Diff1 (stored in state)
    ; R10:     Diff0 (stored in state)
    ; R11:     pOut
    ; R12:     nOutSamples
    ; LR/R14:  pLut

    MOV     R11, R0                     ; R11: pOut is copy of pIn [perform in place]
    MOV     R12, #32*2                  ; R12: nOutSamples
    LDR     R14, cicOrd4Bits8Pad0LutLoc8

    ; Get accumulator/differentiator state
    LDM     R1, {R4-R6, R8-R10}

    ; Preload first data byte
    .if $$defined("BITSTREAM_BE")
        LDRB    R1, [R0], #1            ; Read in 8b of bitstream
    .elseif $$defined("BITSTREAM_LE16")
        LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
    .else
        .emsg "Undefined bitstream input byte ordering"
    .endif

p2p8k_cic_SAMPLELOOP:
    ; To save two state variables and one differentiation we reset the innermost
    ; accumulator at the start of each decimation period
    MOV     R7, #0
    ; Constant used during CIC
    MOV     R3, #36
p2p8k_cic_BYTELOOP:           ; Byte loop is 2* 13 cycles
    .loop 64/8/2
        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3      ; Acc3 += 120*Acc0
        MLA     R7, R5, R3, R7          ; Acc3 += 36 * Acc1
        ADD     R7, R7, R6, LSL #3      ; Acc3 += 8 * Acc2

        MLA     R6, R4, R3, R6          ; Acc2 += 36 * Acc0
        ADD     R6, R6, R5, LSL #3      ; Acc2 += 8 * Acc1

        ADD     R5, R5, R4, LSL #3      ; Acc1 += 8 * Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
           LDRB    R1, [R0], #2             ; Read in 8b of bitstream
        .endif

        UXTAB   R4, R4, R2              ; Acc0 += LUT0
        UXTAB   R5, R5, R2, ROR #8      ; Acc1 += LUT1
        UXTAB   R6, R6, R2, ROR #16     ; Acc2 += LUT2
        ADD     R7, R7, R2, LSR #23     ; Acc3 += LUT3

        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3      ; Acc3 += 120*Acc0
        MLA     R7, R5, R3, R7          ; Acc3 += 36 * Acc1
        ADD     R7, R7, R6, LSL #3      ; Acc3 += 8 * Acc2

        MLA     R6, R4, R3, R6          ; Acc2 += 36 * Acc0
        ADD     R6, R6, R5, LSL #3      ; Acc2 += 8 * Acc1

        ADD     R5, R5, R4, LSL #3      ; Acc1 += 8 * Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
            LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
        .endif

        UXTAB   R4, R4, R2              ; Acc0 += LUT0
        UXTAB   R5, R5, R2, ROR #8      ; Acc1 += LUT1
        UXTAB   R6, R6, R2, ROR #16     ; Acc2 += LUT2
        ADD     R7, R7, R2, LSR #23     ; Acc3 += LUT3
    .endloop

    ; End of decimation period, perform differentiation, output sample and see if we do more
    SUB     R3, R7, R8                      ; R3: Diff2out = Diff3out - DIff2 = Acc3 - Diff2
    MOV     R8, R7                          ; Diff2' = Diff3out = Acc3
    SUB     R2, R3, R9                      ; R2: Diff1out = Diff2out - Diff1
    MOV     R9, R3                          ; Diff1' = Diff2out
    SUB     R3, R2, R10                     ; R3: Diff0out = Diff1out - Diff0
    MOV     R10, R2                         ; Diff0' = Diff1out
    SUBS    R12, R12, #1                    ; nOutSamples--
    ASR     R3, #4                          ; Scale to 12 dB gain
    SUB     R3, R3, #0x80000                ; Convert to 2s complement and SI3.F17 format (with 12 dB gain)
    STR     R3, [R11], #4
    BNE     p2p8k_cic_SAMPLELOOP

p2p8k_cic_EXIT:
    ; Get pointers to: R0: pIn/intermediateBuf, R1: pState, R2: pBqCoeffs
    POP     {R0-R2}
    ; Store accumulator/differentiator state
    STM     R1!, {R4-R6, R8-R10}
    ; p2pBiQuad expects intermediate buffer pointer on top of stack
    PUSH    {R0}
    B       p2pBiQuad
    .endasmfunc

cicOrd4Bits8Pad0LutLoc8:
    .word cicOrd4Bits8Pad0Lut

    .sect ".text:pdm2pcm_2nd_stage"
    .thumb
    ; ### SECOND STAGE (@Fsout*2) ###
    ; **** A N-stage (typically 5) biquad IIR filter that is used to ****
    ; **** remove out-of band signals and noise before the final     ****
    ; **** stage of decimation. Performed at 32 kHz.                 ****
    ; Register usage is:
    ; R0:      pInOut                      [clobbered]
    ; R1:      pState                      [in/out]
    ; R2:      pBqCoeffs                   [in/out]
    ; R3:      temp0 / input value         [clobbered]
    ; R4:      temp1                       [clobbered]
    ; R5:      StateNm1 (stored in state)  [clobbered]
    ; R6:      StateNm2 (stored in state)  [clobbered]
    ; R7:      g (read from pBqCoeffs)     [clobbered]
    ; R8:      b1 (read from pBqCoeffs)    [clobbered]
    ; R9:      b2 (read from pBqCoeffs)    [clobbered]
    ; R10:     a1 (read from pBqCoeffs)    [clobbered]
    ; R11:     a2 (read from pBqCoeffs)    [clobbered]
    ; R12:     nOutSamples                 [clobbered]
    ; LR/R14:  Y (output value)            [clobbered]
p2pBiQuad:
    .asmfunc
    ; Expects SP to point to the intermediate buffer (i.e. be on top of stack)
p2pBiQuadCascade1_SECTION_LOOP:
    ; Load state and coefficients for this second-order section
    LDM     R1, {R5,R6}
    LDM     R2!, {R7-R11}
    ; Check for end of IIR filter
    CBNZ    R7, p2pBiQuadCascade1_MORE_SECTIONS
    SUB     R2, R2, #4*4            ; pBqCoeffs adjusted for (possible) next section
    B       p2pBiQuadCascade1_EXIT

p2pBiQuadCascade1_MORE_SECTIONS:
    ; Check whether gain coefficient is a coefficient or pointer to coefficient in RAM
    TST     R7, #0xE1000000
    BEQ     p2pBiQuadCascade1_GAIN_LOADED
    LDR     R7, [R7]

p2pBiQuadCascade1_GAIN_LOADED:
    ; Reset in/out pointer to start of intermediate buffer for each section
    LDR     R0, [SP]                  ; R0: intermediate buffer
    MOV     R12, #64/8                ; R12: loop counter

p2pBiQuadCascade1_SAMPLE_LOOP:
    SUBS    R12, R12, #1
    .loop 8
      LDR     R3, [R0]                ; R3: X

      MUL     R3, R3, R7              ; R3: g*X
      ASR     R3, R3, #10             ; R3: g*X/1024
      ADD     R14, R5, R3             ; R14: S[-1] + g*X/1024 = Y

      MLA     R4, R3, R8, R6          ; R4: g*X*(b1)/1024 + S[-2]
      MLS     R4, R14, R10, R4        ; R4: g*X*(b1 - a1)/1024 + S[-2] - a1*S[-1]
      ASR     R5, R4, #10             ; R4: ( g*X*(b1 - a1)/1024 + S[-2] - a1*S[-1] )/1024

      MUL     R6, R3, R9              ; R6: g*X*(b2)
      MLS     R6, R14, R11, R6        ; R6: g*X*(b2 - a2) - a2*S[-1]

      STR     R14, [R0], #4
    .endloop

    BNE     p2pBiQuadCascade1_SAMPLE_LOOP

    ; Store state for this second-order section
    STM     R1!, {R5,R6}
    B       p2pBiQuadCascade1_SECTION_LOOP

p2pBiQuadCascade1_EXIT:


    ; ### THIRD STAGE (Fsout*2 -> Fsout) ###
    ; **** 1st order decimate-by-2 CIC to go to final sample rate    ****
    ; Register usage is:
    ; R0:      pIn
    ; R3:      pOut
    ; R4:      temp0
    ; R5:      temp1
    ; R6:      nOutSamples
    LDR     R0, [SP]                  ; R0: intermediate buffer
    MOV     R3, R0
    MOVS    R6, #32
p2p16kbq_cic2_SAMPLE_LOOP:
    SUBS    R6, R6, #1
    LDM     R0!, {R4-R5}
    ADD     R4, R4, R5
    ASR     R4, R4, #1
    STR     R4, [R3], #4
    BNE     p2p16kbq_cic2_SAMPLE_LOOP



    ; ### FOURTH STAGE (@Fsout) ###
    ; **** A user-defined N-stage biquad IIR filter that can be      ****
    ; **** skipped (first gain coefficient is 0) or used to implement****
    ; **** for example EQ                                            ****
    ; **** Nth order biquad IIR filter ****
    ; Register usage is:
    ; R0:      pInOut                      [clobbered]
    ; R1:      pState                      [in/out]
    ; R2:      pBqCoeffs                   [in/out]
    ; R3:      temp0 / input value         [clobbered]
    ; R4:      temp1                       [clobbered]
    ; R5:      StateNm1 (stored in state)  [clobbered]
    ; R6:      StateNm2 (stored in state)  [clobbered]
    ; R7:      g (read from pBqCoeffs)     [clobbered]
    ; R8:      b1 (read from pBqCoeffs)    [clobbered]
    ; R9:      b2 (read from pBqCoeffs)    [clobbered]
    ; R10:     a1 (read from pBqCoeffs)    [clobbered]
    ; R11:     a2 (read from pBqCoeffs)    [clobbered]
    ; R12:     nOutSamples                 [clobbered]
    ; LR/R14:  Y (output value)            [clobbered]
p2pBiQuadCascade2_SECTION_LOOP:
    ; Load state and coefficients for this second-order section
    LDM     R1, {R5,R6}
    LDM     R2!, {R7-R11}
    ; Check for end of IIR filter
    CBNZ    R7, p2pBiQuadCascade2_MORE_SECTIONS
    B       p2pBiQuadCascade2_EXIT

p2pBiQuadCascade2_MORE_SECTIONS:
    ; Check whether gain coefficient is a coefficient or pointer to coefficient in RAM
    TST     R7, #0xE1000000
    BEQ     p2pBiQuadCascade2_GAIN_LOADED
    LDR     R7, [R7]

p2pBiQuadCascade2_GAIN_LOADED:    ; Reset in/out pointer to start of intermediate buffer for each section
    LDR     R0, [SP]                  ; R0: intermediate buffer
    MOV     R12, #32/8                ; R12: loop counter

p2pBiQuadCascade2_SAMPLE_LOOP:
    SUBS    R12, R12, #1
    .loop 8
      LDR     R3, [R0]                ; R3: X

      MUL     R3, R3, R7              ; R3: g*X
      ASR     R3, R3, #10             ; R3: g*X/1024
      ADD     R14, R5, R3             ; R14: S[-1] + g*X/1024 = Y

      MLA     R4, R3, R8, R6          ; R4: g*X*(b1)/1024 + S[-2]
      MLS     R4, R14, R10, R4        ; R4: g*X*(b1 - a1)/1024 + S[-2] - a1*S[-1]
      ASR     R5, R4, #10             ; R4: ( g*X*(b1 - a1)/1024 + S[-2] - a1*S[-1] )/1024

      MUL     R6, R3, R9              ; R6: g*X*(b2)
      MLS     R6, R14, R11, R6        ; R6: g*X*(b2 - a2) - a2*S[-1]

      STR     R14, [R0], #4           ; Store Y
    .endloop

    BNE     p2pBiQuadCascade2_SAMPLE_LOOP

    ; Store state for this second-order section
    STM     R1!, {R5,R6}
    B       p2pBiQuadCascade2_SECTION_LOOP

p2pBiQuadCascade2_EXIT:



    ; ### FIFTH STAGE (@Fsout) ###
    ; **** Convert samples to S.F15 format, saturate and output ****
    ; Register usage is:
    ; R0:      pIn
    ; R1:      pOut
    ; R2-R5:   temp
    ; R6:      nOutSamples
    MOV     R0, #0
    MSR     APSR_nzcvq, R0          ; Clear Q flag in APSR.
    POP     {R0,R1}                 ; R0: intermediate buffer, R1: pOut
    MOVS    R6, #32/4
p2pfs_copy_SAMPLE_LOOP:
    LDM     R0!, {R2-R5}
    SSAT    R2, #16, R2, ASR #3      ; Output format S.F15 (saturated)
    SSAT    R3, #16, R3, ASR #3      ; Output format S.F15 (saturated)
    SSAT    R4, #16, R4, ASR #3      ; Output format S.F15 (saturated)
    SSAT    R5, #16, R5, ASR #3      ; Output format S.F15 (saturated)
    SUBS    R6, R6, #1
    STRH    R2, [R1], #2
    STRH    R3, [R1], #2
    STRH    R4, [R1], #2
    STRH    R5, [R1], #2
    BNE     p2pfs_copy_SAMPLE_LOOP

    ; Extract Q flag (saturation) and return to caller
    MRS     R0, APSR
    UBFX    R0, R0, #27, #1

p2pfs_RETURN:
    ; Restore clobbered registers and return (R0-R3 already popped)
    POP     {R4-R11, PC}

;   LTORG compiler directive does not exist for the TI compiler. The TI compiler places the immediates below in the flash addresses following the code by default, since they are in the same section.

    .endasmfunc


    .sect ".text:pdm2pcm_lut"

; LUT for order 4 CIC decimator. The LUT index corresponds to 8 bits of
; input, MSB to LSB (followed by 0 zero bits). Each LUT entry contains a
; packed word that contains the increment value to apply to each accumulator
; for the given input word:
;    [ 7: 0] 4 bit increment value for ACC0 (max val 8)
;    [15: 8] 6 bit increment value for ACC1 (max val 36)
;    [23:16] 7 bit increment value for ACC2 (max val 120)
;    [31:24] 9 bit (LSB lost due to byte packing) increment value for ACC3 (max val 329)
; In addition each accumulator needs to be incremented based on the other
; accumulator values so the whole operation becomes:
;    ACC3 +=   120*ACC0 +    36*ACC1 +     8*ACC2 +       LUT[x][31:24] + LUT[x-1][23] (LUT[x-1] MSB is used as LSB for LUT[x])
;    ACC2 +=    36*ACC0 +     8*ACC1 +       LUT[x][22:16]
;    ACC1 +=     8*ACC0 +       LUT[x][ 13: 8]
;    ACC0 +=       LUT[x][ 3: 0]

cicOrd4Bits8Pad0Lut:
    ;These values are const and go in flash.
    .align 4
    .word 0x00000000, 0x00010101, 0x02030201, 0x02040302, 0x05060301, 0x05070402, 0x07090502, 0x070a0603 ; [0]
    .word 0x0a0a0401, 0x0a0b0502, 0x0c0d0602, 0x0c0e0703, 0x0f100702, 0x0f110803, 0x11130903, 0x11140a04 ; [8]
    .word 0x110f0501, 0x12100602, 0x13120702, 0x14130803, 0x16150802, 0x17160903, 0x18180a03, 0x19190b04 ; [16]
    .word 0x1b190902, 0x1c1a0a03, 0x1d1c0b03, 0x1e1d0c04, 0x201f0c03, 0x21200d04, 0x22220e04, 0x23230f05 ; [24]
    .word 0x1c150601, 0x1c160702, 0x1e180802, 0x1e190903, 0x211b0902, 0x211c0a03, 0x231e0b03, 0x231f0c04 ; [32]
    .word 0x261f0a02, 0x26200b03, 0x28220c03, 0x28230d04, 0x2b250d03, 0x2b260e04, 0x2d280f04, 0x2d291005 ; [40]
    .word 0x2d240b02, 0x2e250c03, 0x2f270d03, 0x30280e04, 0x322a0e03, 0x332b0f04, 0x342d1004, 0x352e1105 ; [48]
    .word 0x372e0f03, 0x382f1004, 0x39311104, 0x3a321205, 0x3c341204, 0x3d351305, 0x3e371405, 0x3f381506 ; [56]
    .word 0x2a1c0701, 0x2a1d0802, 0x2c1f0902, 0x2c200a03, 0x2f220a02, 0x2f230b03, 0x31250c03, 0x31260d04 ; [64]
    .word 0x34260b02, 0x34270c03, 0x36290d03, 0x362a0e04, 0x392c0e03, 0x392d0f04, 0x3b2f1004, 0x3b301105 ; [72]
    .word 0x3b2b0c02, 0x3c2c0d03, 0x3d2e0e03, 0x3e2f0f04, 0x40310f03, 0x41321004, 0x42341104, 0x43351205 ; [80]
    .word 0x45351003, 0x46361104, 0x47381204, 0x48391305, 0x4a3b1304, 0x4b3c1405, 0x4c3e1505, 0x4d3f1606 ; [88]
    .word 0x46310d02, 0x46320e03, 0x48340f03, 0x48351004, 0x4b371003, 0x4b381104, 0x4d3a1204, 0x4d3b1305 ; [96]
    .word 0x503b1103, 0x503c1204, 0x523e1304, 0x523f1405, 0x55411404, 0x55421505, 0x57441605, 0x57451706 ; [104]
    .word 0x57401203, 0x58411304, 0x59431404, 0x5a441505, 0x5c461504, 0x5d471605, 0x5e491705, 0x5f4a1806 ; [112]
    .word 0x614a1604, 0x624b1705, 0x634d1805, 0x644e1906, 0x66501905, 0x67511a06, 0x68531b06, 0x69541c07 ; [120]
    .word 0x3c240801, 0x3c250902, 0x3e270a02, 0x3e280b03, 0x412a0b02, 0x412b0c03, 0x432d0d03, 0x432e0e04 ; [128]
    .word 0x462e0c02, 0x462f0d03, 0x48310e03, 0x48320f04, 0x4b340f03, 0x4b351004, 0x4d371104, 0x4d381205 ; [136]
    .word 0x4d330d02, 0x4e340e03, 0x4f360f03, 0x50371004, 0x52391003, 0x533a1104, 0x543c1204, 0x553d1305 ; [144]
    .word 0x573d1103, 0x583e1204, 0x59401304, 0x5a411405, 0x5c431404, 0x5d441505, 0x5e461605, 0x5f471706 ; [152]
    .word 0x58390e02, 0x583a0f03, 0x5a3c1003, 0x5a3d1104, 0x5d3f1103, 0x5d401204, 0x5f421304, 0x5f431405 ; [160]
    .word 0x62431203, 0x62441304, 0x64461404, 0x64471505, 0x67491504, 0x674a1605, 0x694c1705, 0x694d1806 ; [168]
    .word 0x69481303, 0x6a491404, 0x6b4b1504, 0x6c4c1605, 0x6e4e1604, 0x6f4f1705, 0x70511805, 0x71521906 ; [176]
    .word 0x73521704, 0x74531805, 0x75551905, 0x76561a06, 0x78581a05, 0x79591b06, 0x7a5b1c06, 0x7b5c1d07 ; [184]
    .word 0x66400f02, 0x66411003, 0x68431103, 0x68441204, 0x6b461203, 0x6b471304, 0x6d491404, 0x6d4a1505 ; [192]
    .word 0x704a1303, 0x704b1404, 0x724d1504, 0x724e1605, 0x75501604, 0x75511705, 0x77531805, 0x77541906 ; [200]
    .word 0x774f1403, 0x78501504, 0x79521604, 0x7a531705, 0x7c551704, 0x7d561805, 0x7e581905, 0x7f591a06 ; [208]
    .word 0x81591804, 0x825a1905, 0x835c1a05, 0x845d1b06, 0x865f1b05, 0x87601c06, 0x88621d06, 0x89631e07 ; [216]
    .word 0x82551503, 0x82561604, 0x84581704, 0x84591805, 0x875b1804, 0x875c1905, 0x895e1a05, 0x895f1b06 ; [224]
    .word 0x8c5f1904, 0x8c601a05, 0x8e621b05, 0x8e631c06, 0x91651c05, 0x91661d06, 0x93681e06, 0x93691f07 ; [232]
    .word 0x93641a04, 0x94651b05, 0x95671c05, 0x96681d06, 0x986a1d05, 0x996b1e06, 0x9a6d1f06, 0x9b6e2007 ; [240]
    .word 0x9d6e1e05, 0x9e6f1f06, 0x9f712006, 0xa0722107, 0xa2742106, 0xa3752207, 0xa4772307, 0xa5782408 ; [248]

    .end
