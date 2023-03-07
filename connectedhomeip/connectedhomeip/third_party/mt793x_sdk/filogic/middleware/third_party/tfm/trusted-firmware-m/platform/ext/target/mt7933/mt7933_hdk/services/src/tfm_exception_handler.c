
#include <inttypes.h>
#include "tfm_hal_device_header.h"
#include "region_defs.h"
#include "secure_utilities.h"
#include "tfm_arch.h"
#include "tfm_memory_utils.h"
#include "tfm_core_utils.h"
#include "tfm_secure_api.h"
#include "spm_api.h"
#include "core/tfm_core_svc.h"
#include "memory_map_mt7933.h"

#include "elf.h"

typedef struct
{
  char *region_name;
  unsigned int *start_address;
  unsigned int *end_address;
  unsigned int is_dumped;
} memory_region_type;

static uint32_t exc_delay_ms = 1;

#define REGION(a, b, c, d)   { a, (void *)b, (void *)c, d },

const memory_region_type dump_regions[] =
{
    REGION( "sysram",           S_DATA_START,           S_DATA_START + S_DATA_SIZE,  1 )
    REGION( "tcm",              TCM_TFM_BASE,             TCM_BL_BASE,  1 )
    {0}
};


enum { r0, r1, r2, r3, r12, lr, pc, psr,
       s0, s1, s2, s3, s4, s5, s6, s7,
       s8, s9, s10, s11, s12, s13, s14, s15,
       fpscr
     };

typedef struct TaskContextType {
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int sp;              /* after pop r0-r3, lr, pc, xpsr                   */
    unsigned int lr;              /* lr before exception                             */
    unsigned int pc;              /* pc before exception                             */
    unsigned int psr;             /* xpsr before exeption                            */
    unsigned int control;         /* nPRIV bit & FPCA bit meaningful, SPSEL bit = 0  */
    unsigned int exc_return;      /* current lr                                      */
    unsigned int msp;             /* msp                                             */
    unsigned int msplim;          /* msp limit                                       */
    unsigned int psp;             /* psp                                             */
    unsigned int psplim;          /* psp limit                                       */
    unsigned int fpscr;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int s12;
    unsigned int s13;
    unsigned int s14;
    unsigned int s15;
    unsigned int s16;
    unsigned int s17;
    unsigned int s18;
    unsigned int s19;
    unsigned int s20;
    unsigned int s21;
    unsigned int s22;
    unsigned int s23;
    unsigned int s24;
    unsigned int s25;
    unsigned int s26;
    unsigned int s27;
    unsigned int s28;
    unsigned int s29;
    unsigned int s30;
    unsigned int s31;
} TaskContext;

static TaskContext taskContext = {0};
TaskContext *pTaskContext = &taskContext;

#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

void exception_infinite_loop(void)
{
    while(1)
    {

    }
}

void printUsageErrorMsg(uint32_t CFSRValue)
{
    printf("Usage fault: ");

    CFSRValue >>= 16; /* right shift to lsb */
    if ((CFSRValue & (1 << 9)) != 0) {
        printf("Divide by zero\n\r");
    }
    if ((CFSRValue & (1 << 8)) != 0) {
        printf("Unaligned access\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("Coprocessor error\n\r");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        printf("Invalid EXC_RETURN\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) {
        printf("Invalid state\n\r");
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("Undefined instruction\n\r");
    }
}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    printf("Memory Management fault: ");

    CFSRValue &= 0x000000FF; /* mask mem faults */
    if ((CFSRValue & (1 << 5)) != 0) {
        printf("A MemManage fault occurred during FP lazy state preservation\n\r");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        printf("A derived MemManage fault occurred on exception entry\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("A derived MemManage fault occurred on exception return\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        printf("Data access violation @0x%08x\n\r", (unsigned int)SCB->MMFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("MPU or Execute Never (XN) default memory map access violation\n\r");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        printf("SCB->MMFAR = 0x%08x\n\r", (unsigned int)SCB->MMFAR );
    }
}

void printBusFaultErrorMsg(uint32_t CFSRValue)
{
    printf("Bus fault: ");

    CFSRValue &= 0x0000FF00; /* mask bus faults */
    CFSRValue >>= 8;
    if ((CFSRValue & (1 << 5)) != 0) {
        printf("A bus fault occurred during FP lazy state preservation\n\r");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        printf("A derived bus fault has occurred on exception entry\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("A derived bus fault has occurred on exception return\n\r");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        printf("Imprecise data access error has occurred\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        printf("A precise data access error has occurred @x%08x\n\r", (unsigned int)SCB->BFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("A bus fault on an instruction prefetch has occurred\n\r");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        printf("SCB->BFAR = 0x%08x\n\r", (unsigned int)SCB->BFAR );
    }
}

void stackDump(uint32_t stack[])
{
    printf("\n===============================================\n");
    printf("\rTFM Exception Log: Copy Start\r");
    printf("\n===============================================\n");

    taskContext.r0   = stack[r0];
    taskContext.r1   = stack[r1];
    taskContext.r2   = stack[r2];
    taskContext.r3   = stack[r3];
    taskContext.r12  = stack[r12];
    taskContext.sp   = ((uint32_t)stack) + 0x20;
    taskContext.lr   = stack[lr];
    taskContext.pc   = stack[pc];
    taskContext.psr  = stack[psr];

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.s0 = stack[s0];
        taskContext.s1 = stack[s1];
        taskContext.s2 = stack[s2];
        taskContext.s3 = stack[s3];
        taskContext.s4 = stack[s4];
        taskContext.s5 = stack[s5];
        taskContext.s6 = stack[s6];
        taskContext.s7 = stack[s7];
        taskContext.s8 = stack[s8];
        taskContext.s9 = stack[s9];
        taskContext.s10 = stack[s10];
        taskContext.s11 = stack[s11];
        taskContext.s12 = stack[s12];
        taskContext.s13 = stack[s13];
        taskContext.s14 = stack[s14];
        taskContext.s15 = stack[s15];
        taskContext.fpscr = stack[fpscr];
        taskContext.sp += 72; /* s0-s15, fpsr, reserved */
    }

#if (__CORTEX_M != (33U) )
    /* if CCR.STKALIGN=1, check PSR[9] to know if there is forced stack alignment */
    if ( (SCB->CCR & SCB_CCR_STKALIGN_Msk) && (taskContext.psr & 0x200)) {
        taskContext.sp += 4;
    }
#endif

    printf("r0  = 0x%08x\n\r", taskContext.r0);
    printf("r1  = 0x%08x\n\r", taskContext.r1);
    printf("r2  = 0x%08x\n\r", taskContext.r2);
    printf("r3  = 0x%08x\n\r", taskContext.r3);
    printf("r4  = 0x%08x\n\r", taskContext.r4);
    printf("r5  = 0x%08x\n\r", taskContext.r5);
    printf("r6  = 0x%08x\n\r", taskContext.r6);
    printf("r7  = 0x%08x\n\r", taskContext.r7);
    printf("r8  = 0x%08x\n\r", taskContext.r8);
    printf("r9  = 0x%08x\n\r", taskContext.r9);
    printf("r10 = 0x%08x\n\r", taskContext.r10);
    printf("r11 = 0x%08x\n\r", taskContext.r11);
    printf("r12 = 0x%08x\n\r", taskContext.r12);
    printf("lr  = 0x%08x\n\r", taskContext.lr);
    printf("pc  = 0x%08x\n\r", taskContext.pc);
    printf("psr = 0x%08x\n\r", taskContext.psr);
    printf("EXC_RET = 0x%08x\n\r", taskContext.exc_return);

    /* update CONTROL.SPSEL and psp if returning to thread mode */
    if (taskContext.exc_return & 0x4) {
        taskContext.control |= 0x2; /* CONTROL.SPSel */
        taskContext.psp = taskContext.sp;
    } else { /* update msp if returning to handler mode */
        taskContext.msp = taskContext.sp;
    }

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.control |= 0x4; /* CONTROL.FPCA */
        printf("s0  = 0x%08x\n\r", taskContext.s0);
        printf("s1  = 0x%08x\n\r", taskContext.s1);
        printf("s2  = 0x%08x\n\r", taskContext.s2);
        printf("s3  = 0x%08x\n\r", taskContext.s3);
        printf("s4  = 0x%08x\n\r", taskContext.s4);
        printf("s5  = 0x%08x\n\r", taskContext.s5);
        printf("s6  = 0x%08x\n\r", taskContext.s6);
        printf("s7  = 0x%08x\n\r", taskContext.s7);
        printf("s8  = 0x%08x\n\r", taskContext.s8);
        printf("s9  = 0x%08x\n\r", taskContext.s9);
        printf("s10 = 0x%08x\n\r", taskContext.s10);
        printf("s11 = 0x%08x\n\r", taskContext.s11);
        printf("s12 = 0x%08x\n\r", taskContext.s12);
        printf("s13 = 0x%08x\n\r", taskContext.s13);
        printf("s14 = 0x%08x\n\r", taskContext.s14);
        printf("s15 = 0x%08x\n\r", taskContext.s15);
        printf("s16 = 0x%08x\n\r", taskContext.s16);
        printf("s17 = 0x%08x\n\r", taskContext.s17);
        printf("s18 = 0x%08x\n\r", taskContext.s18);
        printf("s19 = 0x%08x\n\r", taskContext.s19);
        printf("s20 = 0x%08x\n\r", taskContext.s20);
        printf("s21 = 0x%08x\n\r", taskContext.s21);
        printf("s22 = 0x%08x\n\r", taskContext.s22);
        printf("s23 = 0x%08x\n\r", taskContext.s23);
        printf("s24 = 0x%08x\n\r", taskContext.s24);
        printf("s25 = 0x%08x\n\r", taskContext.s25);
        printf("s26 = 0x%08x\n\r", taskContext.s26);
        printf("s27 = 0x%08x\n\r", taskContext.s27);
        printf("s28 = 0x%08x\n\r", taskContext.s28);
        printf("s29 = 0x%08x\n\r", taskContext.s29);
        printf("s30 = 0x%08x\n\r", taskContext.s30);
        printf("s31 = 0x%08x\n\r", taskContext.s31);
        printf("fpscr = 0x%08x\n\r", taskContext.fpscr);
    }

    printf("CONTROL = 0x%08x\n\r", taskContext.control);
    printf("MSP     = 0x%08x\n\r", taskContext.msp);
    printf("MSPLIM  = 0x%08x\n\r", taskContext.msplim);
    printf("PSP     = 0x%08x\n\r", taskContext.psp);
    printf("PSPLIM  = 0x%08x\n\r", taskContext.psplim);
    printf("sp      = 0x%08x\n\r", taskContext.sp);

}

/* To keep the output format on Teraterm */
static void dumpBusyDelayMs(uint32_t ms)
{
    // TODO: calculate with MPU Hz.
    uint32_t count = ms * 1024;
    do{
        // nop to prevent this function gone after compiler optimization
        __asm volatile("");
        count--;
    } while(count != 0);
}

void memoryDumpAll(void)
{
    unsigned int *current, *end;
    unsigned int i;

    printf("\n\rmemory dump start.\n\r\n\r");

    for (i = 0; ; i++) {
        if ( !dump_regions[i].region_name ) {
           break;
        }
        if ( !dump_regions[i].is_dumped ) {
           continue;
        }

        current = dump_regions[i].start_address;
        end     = dump_regions[i].end_address;

        for (; current < end; current += 4) {

            if (*(current + 0) == 0 && *(current + 1) == 0 && *(current + 2) == 0 && *(current + 3) == 0 ) {
                    continue;
            }

            printf("0x%08x: %08x %08x %08x %08x\n\r", (unsigned int)current, *(current + 0), *(current + 1), *(current + 2), *(current + 3));
            if( exc_delay_ms != 0 ) {
                dumpBusyDelayMs(exc_delay_ms);
            }
         }
    }

    printf("\n\rmemory dump completed.\n\r");
    printf("\n===============================================\n");
    printf("\rTFM Exception Log: Copy End\r");
    printf("\n===============================================\n");

    /* Genie complete message */
    printf("<<<<<<<< LOG END LOG END LOG END LOG END LOG END <<<<<<<<\n");

}

void Hard_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Hard Fault Handler\n\r");
    printf("SCB->HFSR = 0x%08x\n\r", (unsigned int)SCB->HFSR);

    if ((SCB->HFSR & (1 << 30)) != 0) {
        printf("Forced Hard Fault\n\r");
        printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
        if ((SCB->CFSR & 0xFFFF0000) != 0) {
            printUsageErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x0000FF00) != 0 ) {
            printBusFaultErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x000000FF) != 0 ) {
            printMemoryManagementErrorMsg(SCB->CFSR);
        }
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    exception_infinite_loop();
#endif
}

void MemManage_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn MemManage Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );

    if ((SCB->CFSR & 0xFF) != 0) {
        printMemoryManagementErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    exception_infinite_loop();
#endif
}

void Bus_Fault_Handler(uint32_t stack[])
{

    printf("\n\rIn Bus Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );

    if ((SCB->CFSR & 0xFF00) != 0) {
        printBusFaultErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    exception_infinite_loop();
#endif
}

void Usage_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Usage Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR);

    if ((SCB->CFSR & 0xFFFF0000) != 0) {
        printUsageErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    exception_infinite_loop();
#endif
}

void exception_init(void)
{
    printf("[Fault]: TFM Exception Init \r\n");
    //TODO: Write exception dump to Flash
}

void fault_switch_ns()
{
    // Exception From Non-Secure
    uint32_t reg_value = SCB->AIRCR;

    /* Switch to Non-Secure Fault Handler */
    printf("[Fault]: Handle Switch (TFM -> RTOS) \r\n");

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Enable system reset request only to the secure world */
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK | SCB_AIRCR_SYSRESETREQS_Msk | SCB_AIRCR_BFHFNMINS_Msk);

    SCB->AIRCR = reg_value;

}

void Non_Banked_Handler(void)
{
    __asm volatile
    (
        "and r12, r12, %0              \n"
        "cmp r12, #0x0                 \n"
        "beq ns_handle                 \n"
        "mov r12, lr                   \n"
        "cpsid i                       \n"     /* disable irq                 */
        "ldr r0, =pTaskContext         \n"
        "ldr r0, [r0]                  \n"     /* r0 := pTaskContext          */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r4, msplim                \n"     /* r4 := MSPLIM                */
        "str r4, [r0], #4              \n"     /* store MSPLIM                */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0], #4              \n"     /* store PSP                   */
        "mrs r1, psplim                \n"     /* move PSPLIM to r1           */
        "str r1, [r0]                  \n"     /* store PSPLIM                */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "mrseq r0, msp                 \n"
        "mrsne r0, psp                 \n"
        "ldr r12, =0xE000ED2C          \n"     /* Load HFSR */
        "ldr r12, [r12]                \n"
        "and r12, r12, #(1<<30)        \n"
        "cmp r12 ,#0x0                 \n"     /* Hard Fault? */
        "bne hardfault_handler         \n"
        "ldr r12, =0xE000ED28          \n"     /* Load CFSR */
        "ldr r12, [r12]                \n"
        "and r12, r12, #0xFF00         \n"     /* Bus Fault? */
        "cmp r12 ,#0x0                 \n"
        "bne busfault_handler          \n"
        "b fault_end                   \n"
        "hardfault_handler:            \n"     /* Hard Fault */
        "bl Hard_Fault_Handler         \n"
        "b fault_end                   \n"
        "busfault_handler:             \n"     /* Bus Fault? */
        "bl Bus_Fault_Handler          \n"
        "b fault_end                   \n"
        "ns_handle:                    \n"
        "beq fault_switch_ns           \n"
        "fault_end:                    \n"
        ::"i" (EXC_RETURN_SECURE_STACK)
    );
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                    \n"
        "b Non_Banked_Handler          \n"
    );
    return;
}

void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                    \n"
        "b Non_Banked_Handler          \n"
    );
    return;
}

void Banked_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "ldr r0, =pTaskContext         \n"
        "ldr r0, [r0]                  \n"     /* r0 := pTaskContext          */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r4, msplim                \n"     /* r4 := MSPLIM                */
        "str r4, [r0], #4              \n"     /* store MSPLIM                */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0], #4              \n"     /* store PSP                   */
        "mrs r1, psplim                \n"     /* move PSPLIM to r1           */
        "str r1, [r0]                  \n"     /* store PSPLIM                */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "mrseq r0, msp                 \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
    );
}

void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl Banked_Handler             \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl Banked_Handler             \n"
        "bl Usage_Fault_Handler        \n"
    );
}




