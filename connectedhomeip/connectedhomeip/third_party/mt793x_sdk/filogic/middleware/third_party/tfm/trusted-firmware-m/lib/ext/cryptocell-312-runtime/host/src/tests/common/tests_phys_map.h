/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __TST_PHYS_MEM_H__
#define __TST_PHYS_MEM_H__

#include <stdint.h>

#ifdef DX_PLAT_ZYNQ7000
    #define USER_WORKSPACE_MEM_BASE_ADDR    0x30000000
    #define DX_REE_BASE_CC          0x80000000
#elif defined DX_PLAT_JUNO
    #define USER_WORKSPACE_MEM_BASE_ADDR    0x8B0000000
    #define DX_REE_BASE_CC          0x60010000
#endif

#define USER_WORKSPACE_MEM_SIZE         0x10000000
#define BOOT_FREE_MEM_LEN           0x50000

// workspace mapping supports 1 process and up to 16 threads:
// process RND CTX - 16KB
// for each thread:
//      RND CTX - 16KB
//      stack - 16KB
//      user Data - (((256MB-16KB)/16)-32KB)
#define REG_MAP_AREA_LEN        0x20000
#define TEST_MAX_PROCESSES      1
#define TEST_MAX_THREADS        16
#define PROCESS_RND_CTX_OFFSET      0
   #define PROCESS_RND_WORKBUFF_OFFSET  (10*1024)
   #define PROCESS_RND_WORKBUFF_SIZE    (6*1024) // rnd working buff needs 2KB, we take 6KB within total size of context
#define PROCESS_RND_CTX_SIZE        (16*1024) // rnd ctx needs 7KB, we take 16KB

#define TOTAL_USR_THREAD_MAPPING_SIZE   ((USER_WORKSPACE_MEM_SIZE-PROCESS_RND_CTX_SIZE)/TEST_MAX_THREADS)  // each thread gets workspace size [(256MB-16KB)/16] = [16MB-1KB]
#define THREADS_START_OFFSET        (PROCESS_RND_CTX_OFFSET+PROCESS_RND_CTX_SIZE)
#define THREAD_RND_CTX_OFFSET       0
#define THREAD_RND_CTX_SIZE     PROCESS_RND_CTX_SIZE
#define THREAD_STACK_OFFSET     (THREAD_RND_CTX_OFFSET+THREAD_RND_CTX_SIZE)
#define THREAD_STACK_SIZE       (128*1024) // stack has 128KB for 64bit CPU
#define USER_DATA_OFFSET        (THREAD_STACK_OFFSET+THREAD_STACK_SIZE)
#define USER_DATA_SIZE          (TOTAL_USR_THREAD_MAPPING_SIZE-THREAD_RND_CTX_SIZE-THREAD_STACK_SIZE) // {[16MB-1KB]-32KB} = 16MB-33KB
// devide by 6 for: dataIn, dataOut, expectedData, ccm-aData, hmac-keySize, other-key, iv, tag...
#define USER_DATA_INPUT_MAX_SIZE    (USER_DATA_SIZE/6)  // {16MB-33KB}/6 =

typedef enum memMapTypes_t {
        MEM_MAP_REGS = 1,
        MEM_MAP_ENV = 2,
        MEM_MAP_USR_DATA = 4,
        MEM_MAP_USR_STACK = 8,
        MEM_MAP_RND_CTX = 0x10

}MemMapTypes_t;


typedef struct {
    unsigned long      threadHwRndCtxBaseAddr;
    unsigned long      threadHwRndWorkBuffBaseAddr;
    unsigned long      threadHwUserStackBaseAddr;
    unsigned long      threadHwUserBaseAddr;
}ThreadMappingArea_t;

typedef struct {
    unsigned long       processHwRegBaseAddr;
    unsigned long       processHwEnvBaseAddr;
    unsigned long       processHwRndCtxBaseAddr;
    unsigned long       processHwRndWorkBuffBaseAddr;
    uint32_t            userSpaceSize;
    uint32_t            numOfThreads;
    ThreadMappingArea_t     threadMapAddr[TEST_MAX_THREADS];
}ProcessMappingArea_t;


/******************************/
/*   function declaration     */
/*****************************/

unsigned int TestMapCCRegs(void);
void TestUnmapCCRegs(void);

unsigned int TestMapRee(void);
void TestMunMapRee(void);

unsigned int TestMapProcessAddrs(uint32_t numOfThreads, ProcessMappingArea_t *processMap);
void TestMunMapProcessAddrs(ProcessMappingArea_t *processMap);

extern unsigned long     g_testHwEnvBaseAddr;
extern unsigned long     g_testHwRegBaseAddr;
extern unsigned long     g_testHwReeRegBaseAddr;
extern unsigned long     g_testHwRndCtxBaseAddr;
extern unsigned long     g_testHwRndWorkBuffBaseAddr;
extern unsigned long     g_testHwUserStackBaseAddr;
extern unsigned long     g_testHwUserBaseAddr;

#endif //__TST_PHYS_MEM_H__
