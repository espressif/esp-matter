//******************************************************************************
//* BOOT  v#####                                                               *
//* Copyright (c) 1996-2020@%%%% Texas Instruments Incorporated                *
//******************************************************************************

//****************************************************************************
//* BOOT.S
//*
//* THIS IS THE INITAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
//* IT MUST BE LINKED AND LOADED WITH ALL C++ PROGRAMS.
//*
//* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
//*   1) ALLOCATES THE STACK AND INITIALIZES THE STACK POINTER
//*   2) CALLS AUTO-INITIALIZATION ROUTINE
//*   3) CALLS THE FUNCTION MAIN TO START THE C++ PROGRAM
//*   4) CALLS THE STANDARD EXIT ROUTINE
//*
//* THIS MODULE DEFINES THE FOLLOWING GLOBAL SYMBOLS:
//*   1) __stack     STACK MEMORY AREA
//*   2) _c_int00    BOOT ROUTINE
//*
//****************************************************************************


//****************************************************************************
//*  32 BIT STATE BOOT ROUTINE                                               *
//****************************************************************************

        .global __stack
//***************************************************************
//* DEFINE THE USER MODE STACK (DEFAULT SIZE IS 512)
//***************************************************************
        .section ".stack","aw",%nobits
__stack: .zero 4

        .text
        .global _c_int00
        .type   _c_int00,%function
        .arm
//***************************************************************
//* FUNCTION DEF: _c_int00
//***************************************************************
_c_int00:

        #ifdef __ARM_FP
        //*------------------------------------------------------
        //* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
        //* 10 AND 11, REQUIRED TO ENABLE NEON/VFP
        //* COPROCESSOR ACCESS CONTROL REG
        //* BITS [23:22] - CP11, [21:20] - CP10
        //* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
        //*------------------------------------------------------
        MRC     p15,#0x0,r0,c1,c0,#2
        MOV     r3,#0xf00000
        ORR     r0,r0,r3
        MCR     p15,#0x0,r0,c1,c0,#2

        //*------------------------------------------------------
        // SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
        //*------------------------------------------------------
        MOV     r0,#0x40000000
        FMXR    FPEXC,r0
        #endif

        //*------------------------------------------------------
        //* SET TO SYSTEM MODE
        //*------------------------------------------------------
        MRS     r0, cpsr
        BIC     r0, r0, #0x1F  // CLEAR MODES
        ORR     r0, r0, #0x1F  // SET SYSTEM MODE
        MSR     cpsr_cf, r0

        //*------------------------------------------------------
        //* INITIALIZE THE STACK
        //*------------------------------------------------------
        #if __TI_AVOID_EMBEDDED_CONSTANTS
        MOVW    r0, __stack
        MOVT    r0, __stack
        MOV     sp, r0
        MOVW    r0, __STACK_SIZE
        MOVT    r0, __STACK_SIZE
        #else
        LDR     sp, c_stack
        LDR     r0, c_STACK_SIZE
        #endif
        ADD     sp, sp, r0


        //*------------------------------------------------------
        //* Call the __mpu_init hook function.
        //*------------------------------------------------------
        BL      __mpu_init

        //*------------------------------------------------------
        //* Perform all the required initializations when
        //* _system_pre_init() returns non-zero:
        //*   - Process BINIT Table
        //*   - Perform C auto initialization
        //*   - Call global constructors
        //*------------------------------------------------------
        BL      _system_pre_init
        CMP     R0, #0
        BEQ     bypass_auto_init
        BL      __TI_auto_init
bypass_auto_init:

        //*------------------------------------------------------
        //* CALL APPLICATION
        //*------------------------------------------------------
        BL      _args_main

        //*------------------------------------------------------
        //* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT
        //*------------------------------------------------------
        BL      xdc_runtime_System_exit__E

        //*------------------------------------------------------
        //* DONE, LOOP FOREVER
        //*------------------------------------------------------
L1:     B       L1


//***************************************************************
//* CONSTANTS USED BY THIS MODULE
//***************************************************************
        #if !__TI_AVOID_EMBEDDED_CONSTANTS
c_stack:        .long    __stack
c_STACK_SIZE:   .long    __STACK_SIZE
        #endif

//******************************************************
//* UNDEFINED REFERENCES                               *
//******************************************************
        .global __STACK_SIZE
        .global _system_pre_init
        .global __TI_auto_init
        .global _args_main
        .global exit
        .global __mpu_init

        .end
