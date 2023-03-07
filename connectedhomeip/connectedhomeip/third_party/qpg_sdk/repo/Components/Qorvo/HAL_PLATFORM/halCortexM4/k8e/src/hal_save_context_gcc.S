    .syntax unified
    .thumb

    .global hal_save_context
    .global hal_restore_context

/*-----------------------------------------------------------
    Save all scrach registers(R4--R11, LR) and CONTROL register
    CONTROL register need to be saved to find out which stack
    was used (PSP or MSP) and during restore, need to restore
    back to the save stack
*/
.type hal_save_context, %function
hal_save_context:
  mov R12, SP
  STM R0!, {R4 - R11}
  STM R0!, {R12, LR}
  MRS R12, CONTROL
  STR R12, [R0]
  MOV R0, #0
  BX LR

/*-----------------------------------------------------------*/
.type hal_restore_context, %function
hal_restore_context:
   CMP R1, #0
   IT  EQ
   ADDEQ R1, R1, #1
   LDM R0!, {R4 - R11}
   LDM R0!, {R12, LR}
   LDR R2, [R0]
   TEQ R2, #0x2
   ITE  EQ
   MSREQ PSP, R12
   MSRNE MSP, R12
   MSR CONTROL, R2
   MOVS R0, R1
   BX LR

