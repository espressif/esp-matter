/**************************************************
 *
 * Part two of the system initialization code, contains C-level
 * initialization, thumb-2 only variant.
 *
 * Copyright 2006 IAR Systems. All rights reserved.
 *
 * $Revision: 46842 $
 * 2013-May-23  vikram  Added xdc startup exec hook
 **************************************************/


; --------------------------------------------------
; Module ?cmain, C-level initialization.
;


        SECTION SHT$$PREINIT_ARRAY:CONST:NOROOT(2)
        SECTION SHT$$INIT_ARRAY:CONST:NOROOT(2)

        SECTION .text:CODE:NOROOT(2)

        PUBLIC  __cmain
        ;; Keep ?main for legacy reasons, it is accessed in countless instances of cstartup.s around the world...
        PUBLIC  ?main
        EXTWEAK __iar_data_init3
        EXTERN  __low_level_init
        EXTERN  iar_xdc_startup_exec
        EXTERN  __call_ctors
        EXTERN  main
        EXTERN  xdc_runtime_System_exit__E

        THUMB
__cmain:
?main:

; Initialize segments.
; __segment_init and __low_level_init are assumed to use the same
; instruction set and to be reachable by BL from the ICODE segment
; (it is safest to link them in segment ICODE).

          FUNCALL __cmain, __low_level_init
        bl      __low_level_init
        cmp     r0,#0
        beq     ?l1
          FUNCALL __cmain, __iar_data_init3
        bl      __iar_data_init3
; XDC startup exec hook 
          FUNCALL __cmain, iar_xdc_startup_exec 
        bl      iar_xdc_startup_exec

?l1:
        REQUIRE ?l3

        SECTION .text:CODE:NOROOT(2)

        PUBLIC  _main
        PUBLIC _call_main
        THUMB

__iar_init$$done:               ; Copy initialization is done

?l3:
_call_main:
        MOVS    r0,#0   ;  No parameters
          FUNCALL __cmain, main
        BL      main
_main:
          FUNCALL __cmain, xdc_runtime_System_exit__E
        BL      xdc_runtime_System_exit__E

        END
