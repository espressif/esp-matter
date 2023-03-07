;/*
; * Copyright (c) 2009-2019 ARM Limited. All rights reserved.
; * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */
; adapted from ../armclang/startup_psoc64_ns.s

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  ARM_LIB_STACK_MSP:DATA:NOROOT(3)

                SECTION  .intvec:CODE:NOROOT(2)

                EXTERN   __iar_program_start
                EXTERN   SystemInit
                PUBLIC   __vector_table
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                EXTERN  Cy_SysIpcPipeIsrCm4
                EXTERN  Cy_Flash_ResumeIrqHandler


                DATA

__vector_table
                DCD     sfe(ARM_LIB_STACK_MSP)    ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External interrupts                           Description
                DCD     ioss_interrupts_gpio_0_IRQHandler     ; GPIO Port Interrupt #0
                DCD     ioss_interrupts_gpio_1_IRQHandler     ; GPIO Port Interrupt #1
                DCD     ioss_interrupts_gpio_2_IRQHandler     ; GPIO Port Interrupt #2
                DCD     ioss_interrupts_gpio_3_IRQHandler     ; GPIO Port Interrupt #3
                DCD     ioss_interrupts_gpio_4_IRQHandler     ; GPIO Port Interrupt #4
                DCD     ioss_interrupts_gpio_5_IRQHandler     ; GPIO Port Interrupt #5
                DCD     ioss_interrupts_gpio_6_IRQHandler     ; GPIO Port Interrupt #6
                DCD     ioss_interrupts_gpio_7_IRQHandler     ; GPIO Port Interrupt #7
                DCD     ioss_interrupts_gpio_8_IRQHandler     ; GPIO Port Interrupt #8
                DCD     ioss_interrupts_gpio_9_IRQHandler     ; GPIO Port Interrupt #9
                DCD     ioss_interrupts_gpio_10_IRQHandler    ; GPIO Port Interrupt #10
                DCD     ioss_interrupts_gpio_11_IRQHandler    ; GPIO Port Interrupt #11
                DCD     ioss_interrupts_gpio_12_IRQHandler    ; GPIO Port Interrupt #12
                DCD     ioss_interrupts_gpio_13_IRQHandler    ; GPIO Port Interrupt #13
                DCD     ioss_interrupts_gpio_14_IRQHandler    ; GPIO Port Interrupt #14
                DCD     ioss_interrupt_gpio_IRQHandler        ; GPIO All Ports
                DCD     ioss_interrupt_vdd_IRQHandler         ; GPIO Supply Detect Interrupt
                DCD     lpcomp_interrupt_IRQHandler           ; Low Power Comparator Interrupt
                DCD     scb_8_interrupt_IRQHandler            ; Serial Communication Block #8 (DeepSleep capable)
                DCD     srss_interrupt_mcwdt_0_IRQHandler     ; Multi Counter Watchdog Timer interrupt
                DCD     srss_interrupt_mcwdt_1_IRQHandler     ; Multi Counter Watchdog Timer interrupt
                DCD     srss_interrupt_backup_IRQHandler      ; Backup domain interrupt
                DCD     srss_interrupt_IRQHandler             ; Other combined Interrupts for SRSS (LVD, WDT, CLKCAL)
                DCD     cpuss_interrupts_ipc_0_IRQHandler     ; CPUSS Inter Process Communication Interrupt #0
                DCD     cpuss_interrupts_ipc_1_IRQHandler     ; CPUSS Inter Process Communication Interrupt #1
                DCD     cpuss_interrupts_ipc_2_IRQHandler     ; CPUSS Inter Process Communication Interrupt #2
                DCD     cpuss_interrupts_ipc_3_IRQHandler     ; CPUSS Inter Process Communication Interrupt #3
                DCD     Cy_SysIpcPipeIsrCm4
                DCD     cpuss_interrupts_ipc_5_IRQHandler     ; CPUSS Inter Process Communication Interrupt #5
                DCD     cpuss_interrupts_ipc_6_IRQHandler     ; CPUSS Inter Process Communication Interrupt #6
                DCD     cpuss_interrupts_ipc_7_IRQHandler     ; CPUSS Inter Process Communication Interrupt #7
                DCD     cpuss_interrupts_ipc_8_IRQHandler     ; CPUSS Inter Process Communication Interrupt #8
                DCD     cpuss_interrupts_ipc_9_IRQHandler     ; CPUSS Inter Process Communication Interrupt #9
                DCD     cpuss_interrupts_ipc_10_IRQHandler    ; CPUSS Inter Process Communication Interrupt #10
                DCD     cpuss_interrupts_ipc_11_IRQHandler    ; CPUSS Inter Process Communication Interrupt #11
                DCD     cpuss_interrupts_ipc_12_IRQHandler    ; CPUSS Inter Process Communication Interrupt #12
                DCD     cpuss_interrupts_ipc_13_IRQHandler    ; CPUSS Inter Process Communication Interrupt #13
                DCD     cpuss_interrupts_ipc_14_IRQHandler    ; CPUSS Inter Process Communication Interrupt #14
                DCD     cpuss_interrupts_ipc_15_IRQHandler    ; CPUSS Inter Process Communication Interrupt #15
                DCD     scb_0_interrupt_IRQHandler            ; Serial Communication Block #0
                DCD     scb_1_interrupt_IRQHandler            ; Serial Communication Block #1
                DCD     scb_2_interrupt_IRQHandler            ; Serial Communication Block #2
                DCD     scb_3_interrupt_IRQHandler            ; Serial Communication Block #3
                DCD     scb_4_interrupt_IRQHandler            ; Serial Communication Block #4
                DCD     scb_5_interrupt_IRQHandler            ; Serial Communication Block #5
                DCD     scb_6_interrupt_IRQHandler            ; Serial Communication Block #6
                DCD     scb_7_interrupt_IRQHandler            ; Serial Communication Block #7
                DCD     scb_9_interrupt_IRQHandler            ; Serial Communication Block #9
                DCD     scb_10_interrupt_IRQHandler           ; Serial Communication Block #10
                DCD     scb_11_interrupt_IRQHandler           ; Serial Communication Block #11
                DCD     scb_12_interrupt_IRQHandler           ; Serial Communication Block #1
                DCD     csd_interrupt_IRQHandler              ; CSD (Capsense) interrupt
                DCD     cpuss_interrupts_dmac_0_IRQHandler    ; CPUSS DMAC, Channel #0
                DCD     cpuss_interrupts_dmac_1_IRQHandler    ; CPUSS DMAC, Channel #1
                DCD     cpuss_interrupts_dmac_2_IRQHandler    ; CPUSS DMAC, Channel #2
                DCD     cpuss_interrupts_dmac_3_IRQHandler    ; CPUSS DMAC, Channel #3
                DCD     cpuss_interrupts_dw0_0_IRQHandler     ; CPUSS DataWire #0, Channel #0
                DCD     cpuss_interrupts_dw0_1_IRQHandler     ; CPUSS DataWire #0, Channel #1
                DCD     cpuss_interrupts_dw0_2_IRQHandler     ; CPUSS DataWire #0, Channel #2
                DCD     cpuss_interrupts_dw0_3_IRQHandler     ; CPUSS DataWire #0, Channel #3
                DCD     cpuss_interrupts_dw0_4_IRQHandler     ; CPUSS DataWire #0, Channel #4
                DCD     cpuss_interrupts_dw0_5_IRQHandler     ; CPUSS DataWire #0, Channel #5
                DCD     cpuss_interrupts_dw0_6_IRQHandler     ; CPUSS DataWire #0, Channel #6
                DCD     cpuss_interrupts_dw0_7_IRQHandler     ; CPUSS DataWire #0, Channel #7
                DCD     cpuss_interrupts_dw0_8_IRQHandler     ; CPUSS DataWire #0, Channel #8
                DCD     cpuss_interrupts_dw0_9_IRQHandler     ; CPUSS DataWire #0, Channel #9
                DCD     cpuss_interrupts_dw0_10_IRQHandler    ; CPUSS DataWire #0, Channel #10
                DCD     cpuss_interrupts_dw0_11_IRQHandler    ; CPUSS DataWire #0, Channel #11
                DCD     cpuss_interrupts_dw0_12_IRQHandler    ; CPUSS DataWire #0, Channel #12
                DCD     cpuss_interrupts_dw0_13_IRQHandler    ; CPUSS DataWire #0, Channel #13
                DCD     cpuss_interrupts_dw0_14_IRQHandler    ; CPUSS DataWire #0, Channel #14
                DCD     cpuss_interrupts_dw0_15_IRQHandler    ; CPUSS DataWire #0, Channel #15
                DCD     cpuss_interrupts_dw0_16_IRQHandler    ; CPUSS DataWire #0, Channel #16
                DCD     cpuss_interrupts_dw0_17_IRQHandler    ; CPUSS DataWire #0, Channel #17
                DCD     cpuss_interrupts_dw0_18_IRQHandler    ; CPUSS DataWire #0, Channel #18
                DCD     cpuss_interrupts_dw0_19_IRQHandler    ; CPUSS DataWire #0, Channel #19
                DCD     cpuss_interrupts_dw0_20_IRQHandler    ; CPUSS DataWire #0, Channel #20
                DCD     cpuss_interrupts_dw0_21_IRQHandler    ; CPUSS DataWire #0, Channel #21
                DCD     cpuss_interrupts_dw0_22_IRQHandler    ; CPUSS DataWire #0, Channel #22
                DCD     cpuss_interrupts_dw0_23_IRQHandler    ; CPUSS DataWire #0, Channel #23
                DCD     cpuss_interrupts_dw0_24_IRQHandler    ; CPUSS DataWire #0, Channel #24
                DCD     cpuss_interrupts_dw0_25_IRQHandler    ; CPUSS DataWire #0, Channel #25
                DCD     cpuss_interrupts_dw0_26_IRQHandler    ; CPUSS DataWire #0, Channel #26
                DCD     cpuss_interrupts_dw0_27_IRQHandler    ; CPUSS DataWire #0, Channel #27
                DCD     cpuss_interrupts_dw0_28_IRQHandler    ; CPUSS DataWire #0, Channel #28
                DCD     cpuss_interrupts_dw1_0_IRQHandler     ; CPUSS DataWire #1, Channel #0
                DCD     cpuss_interrupts_dw1_1_IRQHandler     ; CPUSS DataWire #1, Channel #1
                DCD     cpuss_interrupts_dw1_2_IRQHandler     ; CPUSS DataWire #1, Channel #2
                DCD     cpuss_interrupts_dw1_3_IRQHandler     ; CPUSS DataWire #1, Channel #3
                DCD     cpuss_interrupts_dw1_4_IRQHandler     ; CPUSS DataWire #1, Channel #4
                DCD     cpuss_interrupts_dw1_5_IRQHandler     ; CPUSS DataWire #1, Channel #5
                DCD     cpuss_interrupts_dw1_6_IRQHandler     ; CPUSS DataWire #1, Channel #6
                DCD     cpuss_interrupts_dw1_7_IRQHandler     ; CPUSS DataWire #1, Channel #7
                DCD     cpuss_interrupts_dw1_8_IRQHandler     ; CPUSS DataWire #1, Channel #8
                DCD     cpuss_interrupts_dw1_9_IRQHandler     ; CPUSS DataWire #1, Channel #9
                DCD     cpuss_interrupts_dw1_10_IRQHandler    ; CPUSS DataWire #1, Channel #10
                DCD     cpuss_interrupts_dw1_11_IRQHandler    ; CPUSS DataWire #1, Channel #11
                DCD     cpuss_interrupts_dw1_12_IRQHandler    ; CPUSS DataWire #1, Channel #12
                DCD     cpuss_interrupts_dw1_13_IRQHandler    ; CPUSS DataWire #1, Channel #13
                DCD     cpuss_interrupts_dw1_14_IRQHandler    ; CPUSS DataWire #1, Channel #14
                DCD     cpuss_interrupts_dw1_15_IRQHandler    ; CPUSS DataWire #1, Channel #15
                DCD     cpuss_interrupts_dw1_16_IRQHandler    ; CPUSS DataWire #1, Channel #16
                DCD     cpuss_interrupts_dw1_17_IRQHandler    ; CPUSS DataWire #1, Channel #17
                DCD     cpuss_interrupts_dw1_18_IRQHandler    ; CPUSS DataWire #1, Channel #18
                DCD     cpuss_interrupts_dw1_19_IRQHandler    ; CPUSS DataWire #1, Channel #19
                DCD     cpuss_interrupts_dw1_20_IRQHandler    ; CPUSS DataWire #1, Channel #20
                DCD     cpuss_interrupts_dw1_21_IRQHandler    ; CPUSS DataWire #1, Channel #21
                DCD     cpuss_interrupts_dw1_22_IRQHandler    ; CPUSS DataWire #1, Channel #22
                DCD     cpuss_interrupts_dw1_23_IRQHandler    ; CPUSS DataWire #1, Channel #23
                DCD     cpuss_interrupts_dw1_24_IRQHandler    ; CPUSS DataWire #1, Channel #24
                DCD     cpuss_interrupts_dw1_25_IRQHandler    ; CPUSS DataWire #1, Channel #25
                DCD     cpuss_interrupts_dw1_26_IRQHandler    ; CPUSS DataWire #1, Channel #26
                DCD     cpuss_interrupts_dw1_27_IRQHandler    ; CPUSS DataWire #1, Channel #27
                DCD     cpuss_interrupts_dw1_28_IRQHandler    ; CPUSS DataWire #1, Channel #28
                DCD     cpuss_interrupts_fault_0_IRQHandler   ; CPUSS Fault Structure Interrupt #0
                DCD     cpuss_interrupts_fault_1_IRQHandler   ; CPUSS Fault Structure Interrupt #1
                DCD     cpuss_interrupt_crypto_IRQHandler     ; CRYPTO Accelerator Interrupt
                DCD     Cy_Flash_ResumeIrqHandler
                DCD     cpuss_interrupts_cm4_fp_IRQHandler    ; Floating Point operation fault
                DCD     cpuss_interrupts_cm0_cti_0_IRQHandler ; CM0+ CTI #0
                DCD     cpuss_interrupts_cm0_cti_1_IRQHandler ; CM0+ CTI #1
                DCD     cpuss_interrupts_cm4_cti_0_IRQHandler ; CM4 CTI #0
                DCD     cpuss_interrupts_cm4_cti_1_IRQHandler ; CM4 CTI #1
                DCD     tcpwm_0_interrupts_0_IRQHandler       ; TCPWM #0, Counter #0
                DCD     TIMER1_Handler                        ; TCPWM #0, Counter #1
                DCD     tcpwm_0_interrupts_2_IRQHandler       ; TCPWM #0, Counter #2
                DCD     tcpwm_0_interrupts_3_IRQHandler       ; TCPWM #0, Counter #3
                DCD     tcpwm_0_interrupts_4_IRQHandler       ; TCPWM #0, Counter #4
                DCD     tcpwm_0_interrupts_5_IRQHandler       ; TCPWM #0, Counter #5
                DCD     tcpwm_0_interrupts_6_IRQHandler       ; TCPWM #0, Counter #6
                DCD     tcpwm_0_interrupts_7_IRQHandler       ; TCPWM #0, Counter #7
                DCD     tcpwm_1_interrupts_0_IRQHandler       ; TCPWM #1, Counter #0
                DCD     tcpwm_1_interrupts_1_IRQHandler       ; TCPWM #1, Counter #1
                DCD     tcpwm_1_interrupts_2_IRQHandler       ; TCPWM #1, Counter #2
                DCD     tcpwm_1_interrupts_3_IRQHandler       ; TCPWM #1, Counter #3
                DCD     tcpwm_1_interrupts_4_IRQHandler       ; TCPWM #1, Counter #4
                DCD     tcpwm_1_interrupts_5_IRQHandler       ; TCPWM #1, Counter #5
                DCD     tcpwm_1_interrupts_6_IRQHandler       ; TCPWM #1, Counter #6
                DCD     tcpwm_1_interrupts_7_IRQHandler       ; TCPWM #1, Counter #7
                DCD     tcpwm_1_interrupts_8_IRQHandler       ; TCPWM #1, Counter #8
                DCD     tcpwm_1_interrupts_9_IRQHandler       ; TCPWM #1, Counter #9
                DCD     tcpwm_1_interrupts_10_IRQHandler      ; TCPWM #1, Counter #10
                DCD     tcpwm_1_interrupts_11_IRQHandler      ; TCPWM #1, Counter #11
                DCD     tcpwm_1_interrupts_12_IRQHandler      ; TCPWM #1, Counter #12
                DCD     tcpwm_1_interrupts_13_IRQHandler      ; TCPWM #1, Counter #13
                DCD     tcpwm_1_interrupts_14_IRQHandler      ; TCPWM #1, Counter #14
                DCD     tcpwm_1_interrupts_15_IRQHandler      ; TCPWM #1, Counter #15
                DCD     tcpwm_1_interrupts_16_IRQHandler      ; TCPWM #1, Counter #16
                DCD     tcpwm_1_interrupts_17_IRQHandler      ; TCPWM #1, Counter #17
                DCD     tcpwm_1_interrupts_18_IRQHandler      ; TCPWM #1, Counter #18
                DCD     tcpwm_1_interrupts_19_IRQHandler      ; TCPWM #1, Counter #19
                DCD     tcpwm_1_interrupts_20_IRQHandler      ; TCPWM #1, Counter #20
                DCD     tcpwm_1_interrupts_21_IRQHandler      ; TCPWM #1, Counter #21
                DCD     tcpwm_1_interrupts_22_IRQHandler      ; TCPWM #1, Counter #22
                DCD     tcpwm_1_interrupts_23_IRQHandler      ; TCPWM #1, Counter #23
                DCD     pass_interrupt_sar_IRQHandler         ; SAR ADC interrupt
                DCD     audioss_0_interrupt_i2s_IRQHandler    ; I2S0 Audio interrupt
                DCD     audioss_0_interrupt_pdm_IRQHandler    ; PDM0/PCM0 Audio interrupt
                DCD     audioss_1_interrupt_i2s_IRQHandler    ; I2S1 Audio interrupt
                DCD     profile_interrupt_IRQHandler          ; Energy Profiler interrupt
                DCD     smif_interrupt_IRQHandler             ; Serial Memory Interface interrupt
                DCD     usb_interrupt_hi_IRQHandler           ; USB Interrupt
                DCD     usb_interrupt_med_IRQHandler          ; USB Interrupt
                DCD     usb_interrupt_lo_IRQHandler           ; USB Interrupt
                DCD     sdhc_0_interrupt_wakeup_IRQHandler    ; SDIO wakeup interrupt for mxsdhc
                DCD     sdhc_0_interrupt_general_IRQHandler   ; Consolidated interrupt for mxsdhc for everything else
                DCD     sdhc_1_interrupt_wakeup_IRQHandler    ; EEMC wakeup interrupt for mxsdhc, not used
                DCD     sdhc_1_interrupt_general_IRQHandler   ; Consolidated interrupt for mxsdhc for everything else

__Vectors_End

__Vectors       EQU     __vector_table
__Vectors_Size  EQU     __Vectors_End - __Vectors

                DATA
                EXPORT  __ramVectors
__ramVectors
                DS8     __Vectors_Size

; Reset Handler
                SECTION  .text:CODE:REORDER:NOROOT(2)
                EXTERN   Cy_SystemInitFpuEnable
                PUBWEAK  Reset_Handler

Reset_Handler
                LDR     R0, =Cy_SystemInitFpuEnable
                BLX     R0
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__iar_program_start
                BX      R0


                PUBWEAK Cy_SysLib_FaultHandler
Cy_SysLib_FaultHandler
                B       .

                PUBWEAK  HardFault_Handler
HardFault_Handler
                movs    r0, #4
                mov     r1, LR
                tst     r0, r1
                beq     L_MSP
                mrs     r0, PSP
                b       L_API_call
L_MSP
                mrs     r0, MSP
L_API_call
                ; Storing LR content for Creator call stack trace
                push    {LR}
                bl      Cy_SysLib_FaultHandler

; Dummy Exception Handlers (infinite loops which can be modified)

                PUBWEAK  NMI_Handler
NMI_Handler
                B       .

                PUBWEAK  MemManage_Handler
MemManage_Handler
                B       .

                PUBWEAK  BusFault_Handler
BusFault_Handler
                B       .

                PUBWEAK  UsageFault_Handler
UsageFault_Handler
                B       .

                PUBWEAK  SVC_Handler
SVC_Handler
                B       .

                PUBWEAK  DebugMon_Handler
DebugMon_Handler
                B       .

                PUBWEAK  PendSV_Handler
PendSV_Handler
                B       .

                PUBWEAK  SysTick_Handler
SysTick_Handler
                B       .


                    PUBWEAK  Default_Handler
Default_Handler
                    PUBWEAK  ioss_interrupts_gpio_0_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_1_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_2_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_3_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_4_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_5_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_6_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_7_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_8_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_9_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_10_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_11_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_12_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_13_IRQHandler
                    PUBWEAK  ioss_interrupts_gpio_14_IRQHandler
                    PUBWEAK  ioss_interrupt_gpio_IRQHandler
                    PUBWEAK  ioss_interrupt_vdd_IRQHandler
                    PUBWEAK  lpcomp_interrupt_IRQHandler
                    PUBWEAK  scb_8_interrupt_IRQHandler
                    PUBWEAK  srss_interrupt_mcwdt_0_IRQHandler
                    PUBWEAK  srss_interrupt_mcwdt_1_IRQHandler
                    PUBWEAK  srss_interrupt_backup_IRQHandler
                    PUBWEAK  srss_interrupt_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_1_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_2_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_3_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_5_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_6_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_7_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_8_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_9_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_10_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_11_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_12_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_13_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_14_IRQHandler
                    PUBWEAK  cpuss_interrupts_ipc_15_IRQHandler
                    PUBWEAK  scb_0_interrupt_IRQHandler
                    PUBWEAK  scb_1_interrupt_IRQHandler
                    PUBWEAK  scb_2_interrupt_IRQHandler
                    PUBWEAK  scb_3_interrupt_IRQHandler
                    PUBWEAK  scb_4_interrupt_IRQHandler
                    PUBWEAK  scb_5_interrupt_IRQHandler
                    PUBWEAK  scb_6_interrupt_IRQHandler
                    PUBWEAK  scb_7_interrupt_IRQHandler
                    PUBWEAK  scb_9_interrupt_IRQHandler
                    PUBWEAK  scb_10_interrupt_IRQHandler
                    PUBWEAK  scb_11_interrupt_IRQHandler
                    PUBWEAK  scb_12_interrupt_IRQHandler
                    PUBWEAK  csd_interrupt_IRQHandler
                    PUBWEAK  cpuss_interrupts_dmac_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_dmac_1_IRQHandler
                    PUBWEAK  cpuss_interrupts_dmac_2_IRQHandler
                    PUBWEAK  cpuss_interrupts_dmac_3_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_1_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_2_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_3_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_4_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_5_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_6_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_7_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_8_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_9_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_10_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_11_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_12_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_13_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_14_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_15_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_16_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_17_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_18_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_19_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_20_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_21_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_22_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_23_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_24_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_25_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_26_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_27_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw0_28_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_1_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_2_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_3_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_4_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_5_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_6_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_7_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_8_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_9_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_10_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_11_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_12_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_13_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_14_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_15_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_16_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_17_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_18_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_19_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_20_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_21_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_22_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_23_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_24_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_25_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_26_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_27_IRQHandler
                    PUBWEAK  cpuss_interrupts_dw1_28_IRQHandler
                    PUBWEAK  cpuss_interrupts_fault_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_fault_1_IRQHandler
                    PUBWEAK  cpuss_interrupt_crypto_IRQHandler
                    PUBWEAK  cpuss_interrupts_cm4_fp_IRQHandler
                    PUBWEAK  cpuss_interrupts_cm0_cti_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_cm0_cti_1_IRQHandler
                    PUBWEAK  cpuss_interrupts_cm4_cti_0_IRQHandler
                    PUBWEAK  cpuss_interrupts_cm4_cti_1_IRQHandler
                    PUBWEAK  tcpwm_0_interrupts_0_IRQHandler
                    PUBWEAK  TIMER1_Handler
                    PUBWEAK  tcpwm_0_interrupts_2_IRQHandler
                    PUBWEAK  tcpwm_0_interrupts_3_IRQHandler
                    PUBWEAK  tcpwm_0_interrupts_4_IRQHandler
                    PUBWEAK  tcpwm_0_interrupts_5_IRQHandler
                    PUBWEAK  tcpwm_0_interrupts_6_IRQHandler
                    PUBWEAK  tcpwm_0_interrupts_7_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_0_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_1_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_2_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_3_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_4_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_5_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_6_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_7_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_8_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_9_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_10_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_11_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_12_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_13_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_14_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_15_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_16_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_17_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_18_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_19_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_20_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_21_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_22_IRQHandler
                    PUBWEAK  tcpwm_1_interrupts_23_IRQHandler
                    PUBWEAK  pass_interrupt_sar_IRQHandler
                    PUBWEAK  audioss_0_interrupt_i2s_IRQHandler
                    PUBWEAK  audioss_0_interrupt_pdm_IRQHandler
                    PUBWEAK  audioss_1_interrupt_i2s_IRQHandler
                    PUBWEAK  profile_interrupt_IRQHandler
                    PUBWEAK  smif_interrupt_IRQHandler
                    PUBWEAK  usb_interrupt_hi_IRQHandler
                    PUBWEAK  usb_interrupt_med_IRQHandler
                    PUBWEAK  usb_interrupt_lo_IRQHandler
                    PUBWEAK  sdhc_0_interrupt_wakeup_IRQHandler
                    PUBWEAK  sdhc_0_interrupt_general_IRQHandler
                    PUBWEAK  sdhc_1_interrupt_wakeup_IRQHandler
                    PUBWEAK  sdhc_1_interrupt_general_IRQHandler

ioss_interrupts_gpio_0_IRQHandler
ioss_interrupts_gpio_1_IRQHandler
ioss_interrupts_gpio_2_IRQHandler
ioss_interrupts_gpio_3_IRQHandler
ioss_interrupts_gpio_4_IRQHandler
ioss_interrupts_gpio_5_IRQHandler
ioss_interrupts_gpio_6_IRQHandler
ioss_interrupts_gpio_7_IRQHandler
ioss_interrupts_gpio_8_IRQHandler
ioss_interrupts_gpio_9_IRQHandler
ioss_interrupts_gpio_10_IRQHandler
ioss_interrupts_gpio_11_IRQHandler
ioss_interrupts_gpio_12_IRQHandler
ioss_interrupts_gpio_13_IRQHandler
ioss_interrupts_gpio_14_IRQHandler
ioss_interrupt_gpio_IRQHandler
ioss_interrupt_vdd_IRQHandler
lpcomp_interrupt_IRQHandler
scb_8_interrupt_IRQHandler
srss_interrupt_mcwdt_0_IRQHandler
srss_interrupt_mcwdt_1_IRQHandler
srss_interrupt_backup_IRQHandler
srss_interrupt_IRQHandler
cpuss_interrupts_ipc_0_IRQHandler
cpuss_interrupts_ipc_1_IRQHandler
cpuss_interrupts_ipc_2_IRQHandler
cpuss_interrupts_ipc_3_IRQHandler
cpuss_interrupts_ipc_5_IRQHandler
cpuss_interrupts_ipc_6_IRQHandler
cpuss_interrupts_ipc_7_IRQHandler
cpuss_interrupts_ipc_8_IRQHandler
cpuss_interrupts_ipc_9_IRQHandler
cpuss_interrupts_ipc_10_IRQHandler
cpuss_interrupts_ipc_11_IRQHandler
cpuss_interrupts_ipc_12_IRQHandler
cpuss_interrupts_ipc_13_IRQHandler
cpuss_interrupts_ipc_14_IRQHandler
cpuss_interrupts_ipc_15_IRQHandler
scb_0_interrupt_IRQHandler
scb_1_interrupt_IRQHandler
scb_2_interrupt_IRQHandler
scb_3_interrupt_IRQHandler
scb_4_interrupt_IRQHandler
scb_5_interrupt_IRQHandler
scb_6_interrupt_IRQHandler
scb_7_interrupt_IRQHandler
scb_9_interrupt_IRQHandler
scb_10_interrupt_IRQHandler
scb_11_interrupt_IRQHandler
scb_12_interrupt_IRQHandler
csd_interrupt_IRQHandler
cpuss_interrupts_dmac_0_IRQHandler
cpuss_interrupts_dmac_1_IRQHandler
cpuss_interrupts_dmac_2_IRQHandler
cpuss_interrupts_dmac_3_IRQHandler
cpuss_interrupts_dw0_0_IRQHandler
cpuss_interrupts_dw0_1_IRQHandler
cpuss_interrupts_dw0_2_IRQHandler
cpuss_interrupts_dw0_3_IRQHandler
cpuss_interrupts_dw0_4_IRQHandler
cpuss_interrupts_dw0_5_IRQHandler
cpuss_interrupts_dw0_6_IRQHandler
cpuss_interrupts_dw0_7_IRQHandler
cpuss_interrupts_dw0_8_IRQHandler
cpuss_interrupts_dw0_9_IRQHandler
cpuss_interrupts_dw0_10_IRQHandler
cpuss_interrupts_dw0_11_IRQHandler
cpuss_interrupts_dw0_12_IRQHandler
cpuss_interrupts_dw0_13_IRQHandler
cpuss_interrupts_dw0_14_IRQHandler
cpuss_interrupts_dw0_15_IRQHandler
cpuss_interrupts_dw0_16_IRQHandler
cpuss_interrupts_dw0_17_IRQHandler
cpuss_interrupts_dw0_18_IRQHandler
cpuss_interrupts_dw0_19_IRQHandler
cpuss_interrupts_dw0_20_IRQHandler
cpuss_interrupts_dw0_21_IRQHandler
cpuss_interrupts_dw0_22_IRQHandler
cpuss_interrupts_dw0_23_IRQHandler
cpuss_interrupts_dw0_24_IRQHandler
cpuss_interrupts_dw0_25_IRQHandler
cpuss_interrupts_dw0_26_IRQHandler
cpuss_interrupts_dw0_27_IRQHandler
cpuss_interrupts_dw0_28_IRQHandler
cpuss_interrupts_dw1_0_IRQHandler
cpuss_interrupts_dw1_1_IRQHandler
cpuss_interrupts_dw1_2_IRQHandler
cpuss_interrupts_dw1_3_IRQHandler
cpuss_interrupts_dw1_4_IRQHandler
cpuss_interrupts_dw1_5_IRQHandler
cpuss_interrupts_dw1_6_IRQHandler
cpuss_interrupts_dw1_7_IRQHandler
cpuss_interrupts_dw1_8_IRQHandler
cpuss_interrupts_dw1_9_IRQHandler
cpuss_interrupts_dw1_10_IRQHandler
cpuss_interrupts_dw1_11_IRQHandler
cpuss_interrupts_dw1_12_IRQHandler
cpuss_interrupts_dw1_13_IRQHandler
cpuss_interrupts_dw1_14_IRQHandler
cpuss_interrupts_dw1_15_IRQHandler
cpuss_interrupts_dw1_16_IRQHandler
cpuss_interrupts_dw1_17_IRQHandler
cpuss_interrupts_dw1_18_IRQHandler
cpuss_interrupts_dw1_19_IRQHandler
cpuss_interrupts_dw1_20_IRQHandler
cpuss_interrupts_dw1_21_IRQHandler
cpuss_interrupts_dw1_22_IRQHandler
cpuss_interrupts_dw1_23_IRQHandler
cpuss_interrupts_dw1_24_IRQHandler
cpuss_interrupts_dw1_25_IRQHandler
cpuss_interrupts_dw1_26_IRQHandler
cpuss_interrupts_dw1_27_IRQHandler
cpuss_interrupts_dw1_28_IRQHandler
cpuss_interrupts_fault_0_IRQHandler
cpuss_interrupts_fault_1_IRQHandler
cpuss_interrupt_crypto_IRQHandler
cpuss_interrupts_cm4_fp_IRQHandler
cpuss_interrupts_cm0_cti_0_IRQHandler
cpuss_interrupts_cm0_cti_1_IRQHandler
cpuss_interrupts_cm4_cti_0_IRQHandler
cpuss_interrupts_cm4_cti_1_IRQHandler
tcpwm_0_interrupts_0_IRQHandler
TIMER1_Handler
tcpwm_0_interrupts_2_IRQHandler
tcpwm_0_interrupts_3_IRQHandler
tcpwm_0_interrupts_4_IRQHandler
tcpwm_0_interrupts_5_IRQHandler
tcpwm_0_interrupts_6_IRQHandler
tcpwm_0_interrupts_7_IRQHandler
tcpwm_1_interrupts_0_IRQHandler
tcpwm_1_interrupts_1_IRQHandler
tcpwm_1_interrupts_2_IRQHandler
tcpwm_1_interrupts_3_IRQHandler
tcpwm_1_interrupts_4_IRQHandler
tcpwm_1_interrupts_5_IRQHandler
tcpwm_1_interrupts_6_IRQHandler
tcpwm_1_interrupts_7_IRQHandler
tcpwm_1_interrupts_8_IRQHandler
tcpwm_1_interrupts_9_IRQHandler
tcpwm_1_interrupts_10_IRQHandler
tcpwm_1_interrupts_11_IRQHandler
tcpwm_1_interrupts_12_IRQHandler
tcpwm_1_interrupts_13_IRQHandler
tcpwm_1_interrupts_14_IRQHandler
tcpwm_1_interrupts_15_IRQHandler
tcpwm_1_interrupts_16_IRQHandler
tcpwm_1_interrupts_17_IRQHandler
tcpwm_1_interrupts_18_IRQHandler
tcpwm_1_interrupts_19_IRQHandler
tcpwm_1_interrupts_20_IRQHandler
tcpwm_1_interrupts_21_IRQHandler
tcpwm_1_interrupts_22_IRQHandler
tcpwm_1_interrupts_23_IRQHandler
pass_interrupt_sar_IRQHandler
audioss_0_interrupt_i2s_IRQHandler
audioss_0_interrupt_pdm_IRQHandler
audioss_1_interrupt_i2s_IRQHandler
profile_interrupt_IRQHandler
smif_interrupt_IRQHandler
usb_interrupt_hi_IRQHandler
usb_interrupt_med_IRQHandler
usb_interrupt_lo_IRQHandler
sdhc_0_interrupt_wakeup_IRQHandler
sdhc_0_interrupt_general_IRQHandler
sdhc_1_interrupt_wakeup_IRQHandler
sdhc_1_interrupt_general_IRQHandler

                B       .

                END
