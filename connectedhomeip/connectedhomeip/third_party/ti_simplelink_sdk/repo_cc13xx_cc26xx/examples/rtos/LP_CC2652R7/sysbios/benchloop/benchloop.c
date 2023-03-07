/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>

/* "useHwiMacros" allows for faster inline disable/enable */
#define ti_sysbios_Build_useHwiMacros

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>

/* POSIX module Headers */
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define TASK1STACKSIZE           512
#define TASK2STACKSIZE           768
#define THREADSTACKSIZE          512

#if  defined(ti_targets_arm_elf_A8F) \
  || defined(ti_targets_arm_elf_A8Fnv) \
  || defined(gnu_targets_arm_A8F)
    /*
     *  "nolocalnames" removes the conflict of having both family specific and
     *  hal HWI modules included. The family specific module is needed to clear
     *  A8 interrupts posted from software.
     */
    #define ti_sysbios_family_arm_a8_intcps_Hwi__nolocalnames
    #include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#endif

#if defined(xdc_target__isaCompatible_430X) /* MSP430 */
#define Timestamp_get   TimestampProvider_get16
#define getTimestampFreq TimestampProvider_getFreq
#include <ti/sysbios/family/msp430/TimestampProvider.h>

/* Global Variables */
volatile Bits16 t1;    /* temp var for holding first Timestamp */
volatile Bits16 t2;    /* temp var for holding second Timestamp */
Bits16 delta;          /* var for output of t2-t1-offset in TIME() */

#elif (USEROM == 1) /* CC13xx/CC26xx ROM builds */
#include <ti/sysbios/family/arm/m3/TimestampProvider.h>

#define Timestamp_get   TimestampProvider_get32
#define getTimestampFreq TimestampProvider_getFreq

/* Global Variables */
volatile UInt32 t1;    /* temp var for holding first Timestamp */
volatile UInt32 t2;    /* temp var for holding second Timestamp */
UInt32 delta;          /* var for output of t2-t1-offset in TIME() */

#else
#define Timestamp_get   Timestamp_get32
#define getTimestampFreq Timestamp_getFreq

/* Global Variables */
volatile UInt32 t1;    /* temp var for holding first Timestamp */
volatile UInt32 t2;    /* temp var for holding second Timestamp */
UInt32 delta;          /* var for output of t2-t1-offset in TIME() */
#endif

typedef void *(*ThreadFxn)(void *arg);

/* Function Prototypes */
int main(Void);
Void benchmarkTask(UArg a0, UArg a1);
Void task1Func(UArg a0, UArg a1);
Void task2Func(UArg a0, UArg a1);
Void swi1Func(UArg a0, UArg a1);
Void swi2Func(UArg a0, UArg a1);
Void hwi1Func(UArg a0);
Void hwi2Func(UArg a0);
Void hwi3Func(UArg a0);
Void hwi4Func(UArg a0);
Void genericHwiFunc(UArg a0);
Void (*doHwi)(UArg);

static void *thread1Func(void *arg);
static void *thread2Func(void *arg);

/* Set this to 1 for extra diagnostic print statements */
#define USERDEBUG 0

/*
 *  Macro used to LOOP functions and total averages
 *  Inputs: FXN, delta
 *  Outputs: min, max, avg
 *  Assumes delta is externally updated by FXN such as TIME()
 */
#define NUM_LOOPS 10000
#define LOOP(FXN) {              \
    min = ~(0);                  \
    max = 0;                     \
    total = 0;                   \
    for (i = 0; i < NUM_LOOPS; i++) {\
        FXN;                     \
        total += delta;          \
        if(delta < min)          \
            min = delta;         \
        if (delta > max)         \
            max = delta;         \
    }                            \
    avg = total / NUM_LOOPS;     \
}

/*
 *  Macro used to TIME function executions
 *  Inputs: FXN, offset
 *  Output: delta
 */
#define TIME(FXN) {            \
    t1 = Timestamp_get();      \
    FXN;                       \
    t2 = Timestamp_get();      \
    delta = (t2);              \
    delta = delta - (t1);      \
}

/* Macro and definitions for interrupt compatibility across boards. */
#if defined(ti_targets_arm_elf_A8F) \
    | defined(ti_targets_arm_elf_A8Fnv) \
    | defined(gnu_targets_arm_A8F) /* Cortex A8 */
    #define HWI_INT 13
    #define CONFIGURE_INTERRUPTS()
    #define TRIGGER_INTERRUPT() \
        Hwi_post(HWI_INT);
    #define CLEAR_INTERRUPT() \
        ti_sysbios_family_arm_a8_intcps_Hwi_intc.ISR_CLEAR0 = (1 << HWI_INT);
    #define CHANGE_HWI(hwiHandle, hwiFXN) \
        Hwi_setFunc(hwiHandle, hwiFXN, (UArg)NULL);
#elif defined(xdc_target__isaCompatible_v7M) \
    | defined(xdc_target__isaCompatible_v6M) \
    | defined(xdc_target__isaCompatible_v7M4)/* CortexM0, CortexM4, CortexM3 */
    #define HWI_INT 17
    #define CONFIGURE_INTERRUPTS()
    #define TRIGGER_INTERRUPT() \
        Hwi_post(HWI_INT);
    #define CLEAR_INTERRUPT()
    #define CHANGE_HWI(hwiHandle, hwiFXN) \
        Hwi_setFunc(hwiHandle, hwiFXN, (UArg)NULL);
#elif defined(xdc_target__isaCompatible_430X) /* MSP430 */
    #ifdef __IAR_SYSTEMS_ICC__
        #include <msp430.h>
    #else
        extern volatile unsigned int SFRIFG1;
        extern volatile unsigned int SFRIE1;
    #endif
    #undef NUM_LOOPS
    #define NUM_LOOPS 5
    #define CONFIGURE_INTERRUPTS() {\
        SFRIFG1 &= !0x08; \
        SFRIE1 |= 0x08; \
    }  /* Clear pending interrupt bit, Enable VMA interrupts */
    #ifdef __IAR_SYSTEMS_ICC__
        #define TRIGGER_INTERRUPT() \
            asm("   movx.w #0x1, &0xFFFFF\n    nop");
    #else
        #define TRIGGER_INTERRUPT() { \
            __data20_write_char(0xFFFFF,1); \
        asm(" nop"); \
        }
    #endif
    #define CLEAR_INTERRUPT() \
        SFRIFG1 &= !0x08;  /* Clear pending interrupt bit */
    #define CHANGE_HWI(hwiHandle, hwiFXN) \
        doHwi=hwiFXN;
#elif defined(xdc_target__isaCompatible_674) \
    | defined(xdc_target__isaCompatible_64P) \
    | defined(xdc_target__isaCompatible_64T) \
    | defined(xdc_target__isaCompatible_66) \
    | defined(xdc_target__isaCompatible_28) \
    | defined(xdc_target__isaCompatible_arp32) \
    | defined(ti_targets_arm_elf_A15Fnv) \
    | defined(gnu_targets_arm_A15F) \
    | defined(gnu_targets_arm_A9F) \
    | defined(ti_targets_arm_elf_Arm9) /* C6000, C28, ARP32, A9, A15, ARM9 */
    #define HWI_INT 13
    #define CONFIGURE_INTERRUPTS()
    #define TRIGGER_INTERRUPT() \
        Hwi_post(HWI_INT);
    #define CLEAR_INTERRUPT()
    #define CHANGE_HWI(hwiHandle, hwiFXN) \
        Hwi_setFunc(hwiHandle, hwiFXN, NULL);
#else
    #warn "Target not Supported"
    /* If a target throws this warning it has not been extensively tested */
    #define HWI_INT 13
    #define CONFIGURE_INTERRUPTS()
    #define TRIGGER_INTERRUPT() \
        Hwi_post(HWI_INT);
    #define CLEAR_INTERRUPT()
    #define CHANGE_HWI(hwiHandle, hwiFXN) \
        Hwi_setFunc(hwiHandle, hwiFXN, NULL);
#endif

UInt32 minTimestamp;  /* used to keep min Timestamp cycle count */
UInt32 avgTimestamp;  /* used to keep avg Timestamp cycle count */
Float factor;         /* used for clock ratio cpu/timestamp */
UInt32 min;           /* minimum cycle count for LOOP (output) */
UInt32 max;           /* maximum cycle count for LOOP (output) */
UInt32 avg;           /* average cycle count for LOOP (output) */


/* BIOS Objects */
Task_Struct task1Struct, task2Struct;
Char task1Stack[TASK1STACKSIZE], task2Stack[TASK2STACKSIZE];
Task_Handle benchmarkHandle, task1Handle;
Swi_Struct swi1Struct, swi2Struct;
Swi_Handle swi1, swi2;
Semaphore_Struct sem1Struct, sem2Struct;
Semaphore_Handle sema1, sema2;

/* POSIX Objects */
sem_t sem1;
sem_t sem2;
pthread_t threadHandle1 = (pthread_t)NULL;
pthread_t threadHandle2 = (pthread_t)NULL;
char thread1Stack[THREADSTACKSIZE], thread2Stack[THREADSTACKSIZE];
int       threadArg = 1;
UInt32    threadRet = 0;
int       threadPri = 5;

/*
 *  ======== printResults ========
 */
Void printResults(String name)
{
    /* Verify values will not underflow */
    if (min >= minTimestamp) {
        min -= minTimestamp;
    }
    if (avg >= avgTimestamp) {
        avg -= avgTimestamp;
    }
    if (max >= avgTimestamp) {
        max -= avgTimestamp;
    }

    /* No computation can take 0 cycles */
    if (min == 0) {
        min = 1;
    }
    if (avg == 0) {
        avg = 1;
    }
    if (max == 0) {
        max = 1;
    }

    System_printf("%lu  %lu  %lu   %s\n",
        (UInt32)(min * factor),
        (UInt32)(max * factor),
        (UInt32)(avg * factor),
    name);
}

/*
 *  ======== benchmarkTask ========
 *  Main bencmark task
 *
 *  This task executes and prints the timing tests outlined in the
 *  "Timing Benchmarks" section of the Bios User Guide.
 */
Void benchmarkTask(UArg a0, UArg a1)
{
    UInt32 total;         /* temporary variable used by LOOP() */
    UInt32 i;             /* temporary variable used for LOOP ctrl */
    UInt key;             /* used to store previous IEB when disabling */
    Types_FreqHz freq1;   /* used to keep Timestamp frequency */
    Types_FreqHz freq2;   /* used to keep BIOS frequency */
    Task_Handle task2;
    Task_Params taskParams;
    pthread_attr_t pattrs;
    struct sched_param priParam;
    int detachState;


    /*
     * This will calculate the factor needed top correlate Timestamp delta
     * to the CPU cycles
     */
    getTimestampFreq(&freq1);
    BIOS_getCpuFreq(&freq2);
    factor = (Float)freq2.lo / freq1.lo;
    if(USERDEBUG) {
        System_printf("%lu\t%lu\t%lu\t Timestamp Freq, BIOS Freq, Factor\n",
            freq1.lo, freq2.lo, (UInt32) factor);
    }

    /*
     *  This will calculate the overhead of making a Timestamp_get()
     *  Function call. The ";" is to hold the place of "FXN" as the input param
     *  replacing the ";" with " " is valid but more easily overlooked
     */
    LOOP(TIME(;));
    minTimestamp = min;
    avgTimestamp = avg;
    if(USERDEBUG) {
        System_printf("%lu\t%lu\t Timestamps\n",
            (UInt32)(minTimestamp * factor),
            (UInt32)(avgTimestamp * factor));
    }

    CONFIGURE_INTERRUPTS();

    /* Test and Print Hwi_restore() time */
    LOOP(
        key = Hwi_disable();
        TIME(Hwi_restore(key));
    );
    printResults("Hwi_restore()");

    /* Test and print Hwi_disable() time */
    LOOP(
        TIME(key = Hwi_disable());
        Hwi_restore(key);
    );

    printResults("Hwi_disable()");

    /* Test and print Hwi dispatcher prolog time */
    CHANGE_HWI(hwi1, &hwi1Func);
    LOOP(
        t1 = Timestamp_get();
        TRIGGER_INTERRUPT();
        delta = t2;
        delta = delta - t1;
    );
    printResults("Hwi_dispatcher_prolog");

    /* Test and print Hwi dispatcher epilog time */
    LOOP(
        TRIGGER_INTERRUPT();
        t1 = Timestamp_get();
        delta = t1;
        delta = delta - t2;
    );
    printResults("Hwi_dispatcher_epilog");

    /* Test and print Hwi dispatcher time */
    CHANGE_HWI(hwi1, &hwi2Func);  /* change to empty Hwi */
    LOOP(TIME(TRIGGER_INTERRUPT()));
    printResults("Hwi_dispatcher");

    /* Test and print Hwi to blocked Task time */
    CHANGE_HWI(hwi1, &hwi3Func);  /* change to start task Hwi */
    Task_setPri(task1Handle, 3);  /* Make task1 higher priority than this task */
    LOOP(
        t1 = Timestamp_get();
        TRIGGER_INTERRUPT();
        delta = t2;
        delta = delta - t1;
    );
    printResults("Hardware_Interrupt_to_Blocked_Task");

    /* Test and print time to post SWI from HWI */
    CHANGE_HWI(hwi1, &hwi4Func);  /* change to post swi Hwi */
    LOOP(
        t1 = Timestamp_get();
        TRIGGER_INTERRUPT();
        delta = t2;
        delta = delta - t1;
    );
    printResults("Hardware_Interrupt_to_Software_Interrupt");

    /* Test and print time to enable SWI's */
    LOOP(
        Swi_disable();
        TIME(Swi_enable());
    );
    printResults("Swi_enable()");

    /* Test and print time to disable SWI's */
    LOOP(
        TIME(Swi_disable());
        Swi_enable();
    );
    printResults("Swi_disable()");

    /* Test and print time to post SWI when one is already pending */
    Swi_disable();
    Swi_post(swi1);
    LOOP(TIME(Swi_post(swi1)));
    printResults("Post_Software_Interrupt_Again");

    /* Test and print time to post swi while already in a different swi */
    Swi_enable();
    LOOP(Swi_post(swi2));
    printResults("Post_Software_Interrupt_without_Context_Switch");

    /* Test and print time to post and enter a SWI */
    LOOP(
        t1 = Timestamp_get();
        Swi_post(swi1);
        delta = t2;
        delta = delta - t1;
    );
    printResults("Post_Software_Interrupt_with_Context_Switch");

    /* Test and print time to create a new task */
    Task_Params_init(&taskParams);
    taskParams.priority = 2;  /* same priority as $this */
    taskParams.stackSize = 512;
    LOOP(
        TIME(task2 = Task_create(task2Func,&taskParams, NULL));
        Task_delete(&task2);
    );
    printResults("Create_a_New_Task_without_Context_Switch");

    /* Test and print time to lower a task's priority */
    LOOP(
        TIME(Task_setPri(task1Handle, 1));
        Task_setPri(task1Handle, 2);
        /* raise the priority so that function above runs full */
    );
    printResults("Set_a_Task_Priority_without_a_Context_Switch");

    /* Test and print the time to yield to another task */
    LOOP(
        /* Task1 is priority 2 here */
        Semaphore_post(sema2);
        t1 = Timestamp_get();
        Task_yield();
        delta = t2;
        delta = delta - t1;
    );
    printResults("Task_yield()");

    /* Test and print time to post a semaphore with no waiting tasks */
    LOOP(TIME(Semaphore_post(sema1)));
    printResults("Post_Semaphore_No_Waiting_Task");

    /* Test and print time to post a semaphore with a waiting task of the same priority */
    LOOP(
        Task_yield();
        /*
         *  1. Put task1Func back in blocked state
         *  2. Posting semaphore2 will not cause task switch as task1 is
         *     same priority
         *  3. Delta is calculated in "TIME()" so t2 from task1 is
         *     not of concern
         */
        TIME(Semaphore_post(sema2));
    );
    printResults("Post_Semaphore_No_Task_Switch");

    /* Test and print time to post a semaphore that cuases a task switch */
    Task_setPri(task1Handle, 3);  /* Make task1 a higher priority */
    LOOP(
        t1 = Timestamp_get();
        Semaphore_post(sema2);
        delta = t2;
        delta = delta - t1;
    );
    printResults("Post_Semaphore_with_Task_Switch");

    /*
     *  Test and print time to pend on an already posted semaphore
     *  sema1 = NUM_LOOPS from "post semaphore again" above
     */
    LOOP(TIME(Semaphore_pend(sema1, BIOS_WAIT_FOREVER)));
    printResults("Pend_on_Semaphore_No_Context_Switch");

    /*
     *  Test and print the time it takes to pend on a semaphore with task switch
     *  Task 1 is blocked as it is a higher priority and we're running here
     */
    Task_setPri(task1Handle, 2); /* Make task1 the same priority as this */
    Semaphore_reset(sema1, 0);   /* Clear sema1 so we can re-use */
    LOOP(
            Semaphore_post(sema2); /* Ready task1 */
            t1 = Timestamp_get();
            Semaphore_pend(sema1, BIOS_WAIT_FOREVER);
            delta = t2;
            delta = delta - t1;
    );
    printResults("Pend_on_Semaphore_with_Task_Switch");

    /* Test and print the time it takes to call Clock_getTicks() */
    LOOP(TIME(Clock_getTicks()));
    printResults("Clock_getTicks()");

    /* Prevent this test task from interefering with the POSIX Tests */
    Task_destruct(&task1Struct);

    /* POSIX Timing Benchmarks */
    /* Test and print time to create a new pthread */
    pthread_attr_init(&pattrs);

    /* Test set/get thread priority attr */
    priParam.sched_priority = -1;
    pthread_attr_setschedparam(&pattrs, &priParam);

    detachState = PTHREAD_CREATE_DETACHED;
    pthread_attr_setdetachstate(&pattrs, detachState);

    LOOP(
        TIME(pthread_create(&threadHandle1, &pattrs, (ThreadFxn)thread1Func,
            (void *)&threadArg););
        pthread_cancel(threadHandle1);
    );

    printResults("POSIX_Create_a_New_Task_without_Context_Switch");

    /* Joinable so thread isn't automatically destroyed after running */
    detachState = PTHREAD_CREATE_JOINABLE;
    pthread_attr_setdetachstate(&pattrs, detachState);
    pthread_attr_setstack(&pattrs, (void *)thread1Stack, THREADSTACKSIZE);

    pthread_create(&threadHandle1, &pattrs, (ThreadFxn)thread1Func,
                (void *)&threadArg);

    /* Thread priorities must change with each iteration */
    priParam.sched_priority = 3;
    pthread_setschedparam(threadHandle1, SCHED_RR, &priParam);

    /* Test and print time to lower a task's priority */
    LOOP(
        priParam.sched_priority = 4;
        pthread_setschedparam(threadHandle1, SCHED_RR, &priParam);

        priParam.sched_priority = 2;
        TIME(
            pthread_setschedparam(threadHandle1, SCHED_RR, &priParam);
        );
    );
    printResults("POSIX_Set_a_Task_Priority_without_a_Context_Switch");

    pthread_cancel(threadHandle1);


    sem_init(&sem1, 0 /* PTHREAD_PROCESS_PRIVATE */, 0);

    /* Test and print time to post a semaphore with no waiting tasks */
    LOOP(TIME(sem_post(&sem1)));
    printResults("POSIX_Post_Semaphore_No_Waiting_Task");


    sem_init(&sem2, 0 /* PTHREAD_PROCESS_PRIVATE */, 0);

    /* Set to same priority of the current task */
    priParam.sched_priority = Task_getPri(Task_self());
    pthread_attr_setschedparam(&pattrs, &priParam);
    pthread_attr_setstack(&pattrs, (void *)thread2Stack, THREADSTACKSIZE);

    pthread_create(&threadHandle2, &pattrs, (ThreadFxn)thread2Func,
                (void *)&threadArg);

    LOOP(
        Task_yield();
        TIME(sem_post(&sem2););
    );
    printResults("POSIX_Post_Semaphore_No_Task_Switch");

    /* Test and print time to post a semaphore that causes a task switch */
    priParam.sched_priority = Task_getPri(Task_self()) + 1;
    pthread_setschedparam(threadHandle2, SCHED_RR, &priParam);

    LOOP(
        t1 = Timestamp_get();
        sem_post(&sem2);
        delta = t2;
        delta = delta - t1;
    );

    printResults("POSIX_Post_Semaphore_with_Task_Switch");

    /*
     *  Test and print time to pend on an already posted semaphore
     *  sema1 = NUM_LOOPS from "post semaphore again" above
     */
    LOOP(
        sem_post(&sem1);
        TIME(sem_wait(&sem1))
    );
    printResults("POSIX_Pend_on_Semaphore_No_Context_Switch");

    /*
     *  Test and print the time it takes to pend on a semaphore with task switch
     *  Task 1 is blocked as it is a higher priority and we're running here
     */
    /* Reset Semaphore */
    sem_destroy(&sem1);
    sem_init(&sem1, 0 /* PTHREAD_PROCESS_PRIVATE */, 0);

    priParam.sched_priority = Task_getPri(Task_self());
    pthread_setschedparam(threadHandle2, SCHED_RR, &priParam);

    LOOP(
        sem_post(&sem2); /* Ready thread2 */
        t1 = Timestamp_get();
        sem_wait(&sem1);
        delta = t2;
        delta = delta - t1;
    );
    printResults("POSIX_Pend_on_Semaphore_with_Task_Switch");

    System_printf("Benchloop Complete\n");

    BIOS_exit(1);
}

/*
 *  ======== main ========
 */
int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;
    Swi_Params swiParams;
    Semaphore_Params semParams;

    Task_Params_init(&taskParams);
    taskParams.stackSize = TASK1STACKSIZE;
    taskParams.priority = 2;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1Func, &taskParams, NULL);

    /* Obtain instance handle */
    task1Handle = Task_handle(&task1Struct);

    taskParams.stackSize = TASK2STACKSIZE;
    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)benchmarkTask,
                          &taskParams, NULL);

    benchmarkHandle = Task_handle(&task2Struct);

    Swi_Params_init(&swiParams);
    swiParams.priority = 1;

    Swi_construct(&swi1Struct, (Swi_FuncPtr)swi1Func, &swiParams, NULL);
    swi1 = Swi_handle(&swi1Struct);

    swiParams.priority = 2;
    Swi_construct(&swi2Struct, (Swi_FuncPtr)swi2Func, &swiParams, NULL);
    swi2 = Swi_handle(&swi2Struct);

    /* Construct a Semaphore object, inital count 0 */
    Semaphore_Params_init(&semParams);

    Semaphore_construct(&sem1Struct, 0, &semParams);
    Semaphore_construct(&sem2Struct, 0, &semParams);

    /* Obtain instance handle */
    sema1 = Semaphore_handle(&sem1Struct);
    sema2 = Semaphore_handle(&sem2Struct);

    System_printf("Benchloop Started\nComputing the Min, Max, Avg cycles for " \
                   "each [BIOS API]\n");
    System_flush();

    /* Does not return */
    BIOS_start();

    return(0);
}

/*
 *  ======== thread1Func ========
 */
static void *thread1Func(void *arg)
{
    static UInt32    threadRet = 0;

    return ((void *)threadRet);
}

/*
 *  ======== thread2Func ========
 */
static void *thread2Func(void *arg)
{
    static UInt32    threadRet = 0;

    while(1) {
        sem_wait(&sem2);
        t2 = Timestamp_get();
        sem_post(&sem1);

        /* Statement added to silence "can't reach statement" warning */
        if (threadRet != 0) {
            break;
        }
    }

    return ((void *)threadRet);
}
/*
 *  ======== swi1Func ========
 */
Void swi1Func(UArg a0, UArg a1)
{
    t2 = Timestamp_get();
}

/*
 *  ======== swi2Func =======
 */
Void swi2Func(UArg a0, UArg a1)
{
    TIME(Swi_post(swi1));
}

/*
 *  ======== hwi1Func ========
 */
Void hwi1Func(UArg a0)
{
    CLEAR_INTERRUPT();
    t2 = Timestamp_get();
}

/*
 *  ======== hwi2Func ========
 */
Void hwi2Func(UArg a0)
{
    CLEAR_INTERRUPT();
}

/*
 *  ======== hwi3Func ========
 */
Void hwi3Func(UArg a0)
{
    Semaphore_post(sema2);
    CLEAR_INTERRUPT();
}

/*
 *  ======== hwi4Func ========
 */
Void hwi4Func(UArg a0)
{
    Swi_post(swi1);
    CLEAR_INTERRUPT();
}

/*
 *  ======== genericHwiFunc ========
 */
Void genericHwiFunc(UArg a0)
{
    doHwi(a0);
}

/*
 *  ======== task1Func ========
 *
 */
Void task1Func(UArg a0, UArg a1)
{
    while (1) {
        Semaphore_pend(sema2, BIOS_WAIT_FOREVER);
        t2 = Timestamp_get();
        Semaphore_post(sema1);
    }
}

/*
 *  ======== task2Func ========
 */
Void task2Func(UArg a0, UArg a1)
{
}
