/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "dx_reg_base_host.h"
#include "test_log.h"
#include "tests_phys_map.h"



/////////////////////////////////////
//         macro defines           //
/////////////////////////////////////

#define MAX_MAPPED_ALLOWED  5
/*************************/
/*   Global variables    */
/*************************/
int32_t       g_reeHwFd = 0;
int32_t       processHwFd = 0;
int32_t       userSpaceFd = 0;
uint32_t       map_used = 0;

unsigned long    g_testHwEnvBaseAddr = 0;
unsigned long    g_testHwRegBaseAddr = 0;
unsigned long    g_testHwReeRegBaseAddr = 0;
unsigned long    g_testHwRndCtxBaseAddr = 0;
unsigned long    g_testHwRndWorkBuffBaseAddr = 0;
unsigned long    g_testHwUserStackBaseAddr = 0;
unsigned long    g_testHwUserBaseAddr = 0;


#define VALID_MAPPED_ADDR(addr) ((addr != 0) && (addr != 0xFFFFFFFF))

ProcessMappingArea_t processMap;

/******************************/
/*   function definitions     */
/*****************************/
/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * caution: the user will get unexpected results using that API  with MEM_MAP_USR_DATA flag while using dx_cclib or sbrom testser
 *      since all use the same user physical continues address space!!!
 *
 * @return void -
 */
unsigned int TestMapCCRegs(void)
{
    unsigned int rc;

    rc = TestMapProcessAddrs(0, &processMap);
    if (rc != 0){
        TEST_PRINTF_ERROR("TestMapProcessAddrs Failed\n");
        return 1;
    }
    g_testHwRndCtxBaseAddr = processMap.processHwRndCtxBaseAddr;
    g_testHwRndWorkBuffBaseAddr = g_testHwRndCtxBaseAddr + PROCESS_RND_WORKBUFF_OFFSET;
    g_testHwUserStackBaseAddr = g_testHwRndCtxBaseAddr + PROCESS_RND_CTX_SIZE;  // the address and size of teh stack must be page aligned
    g_testHwUserBaseAddr = g_testHwUserStackBaseAddr + THREAD_STACK_SIZE;
    TEST_PRINTF("g_testHwRndCtxBaseAddr = 0x%lx\n", (unsigned long)g_testHwRndCtxBaseAddr);
    TEST_PRINTF("g_testHwRndWorkBuffBaseAddr = 0x%lx\n", (unsigned long)g_testHwRndWorkBuffBaseAddr);
    TEST_PRINTF("g_testHwUserStackBaseAddr = 0x%lx\n", (unsigned long)g_testHwUserStackBaseAddr);
    TEST_PRINTF("g_testHwUserBaseAddr = 0x%lx\n", (unsigned long)g_testHwUserBaseAddr);

    return 0;

}


/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
void TestUnmapCCRegs(void)
{
    TestMunMapProcessAddrs(&processMap);
    g_testHwRndCtxBaseAddr = 0;
    g_testHwRndWorkBuffBaseAddr = 0;
    g_testHwUserStackBaseAddr = 0;
    g_testHwUserBaseAddr = 0;
}


unsigned int mapUserSpace(uint32_t numOfThreads, ProcessMappingArea_t *processMap, uint32_t userSpaceSize)
{
    uint32_t i = 0;
    uint32_t threadSize = 0;
    unsigned long threadOffset = 0;
#ifdef DX_PLAT_JUNO
    char drvName[50] = "/dev/cc_linux_driver";
#endif


    if (processMap == NULL) {
        TEST_PRINTF_ERROR("Invalid processMap, exiting...\n");
        return 1;
    }
    if (userSpaceSize < PROCESS_RND_CTX_SIZE) {
        TEST_PRINTF_ERROR("userSpaceSize Too small, exiting...\n");
        return 1;
    }
    if (userSpaceSize > USER_WORKSPACE_MEM_SIZE) {
        TEST_PRINTF_ERROR("userSpaceSize Too big, exiting...\n");
        return 1;
    }
    if (numOfThreads > TEST_MAX_THREADS) {
        TEST_PRINTF_ERROR("Too many threads, exiting...\n");
        return 1;
    }

    //TEST_PRINTF("about to open\n");
#ifdef DX_PLAT_JUNO
    // need special driver that overwrites the Linux mmap.
    // Linux threats this memory as device memory, therfore each access to that memory must be word ligned.
    // and we need to have byte access.
    userSpaceFd = open(drvName, O_RDWR|O_SYNC);
    if (userSpaceFd < 0) {
        TEST_PRINTF_ERROR("Failed openning %s\n", drvName);
        goto end_error;
    }
#else
    userSpaceFd = processHwFd;
#endif
    //TEST_PRINTF("about to mmap for WORKSPACE for RND_CTX\n");
    processMap->processHwRndCtxBaseAddr = (unsigned long)mmap((unsigned int *)USER_WORKSPACE_MEM_BASE_ADDR,
                           userSpaceSize,
                           PROT_READ|PROT_WRITE|PROT_EXEC,
                           MAP_SHARED|MAP_FIXED,   // must use MAP_FIXED to align with secure boot image table
                           userSpaceFd,
                           USER_WORKSPACE_MEM_BASE_ADDR);
    if (!VALID_MAPPED_ADDR(processMap->processHwRndCtxBaseAddr)) {
        TEST_PRINTF_ERROR("Error: mmap failed for processHwRndCtxBaseAddr\n");
        goto end_error;
    }
    processMap->userSpaceSize = userSpaceSize;
    processMap->processHwRndWorkBuffBaseAddr = processMap->processHwRndCtxBaseAddr + PROCESS_RND_WORKBUFF_OFFSET;
    //TEST_PRINTF("processHwRndCtxBaseAddr = 0x%lx\n", (unsigned long)processMap->processHwRndCtxBaseAddr);
    if (numOfThreads == 0) {
        g_testHwRndCtxBaseAddr = processMap->processHwRndCtxBaseAddr;
        g_testHwRndWorkBuffBaseAddr = g_testHwRndCtxBaseAddr + PROCESS_RND_WORKBUFF_OFFSET;
        g_testHwUserStackBaseAddr = g_testHwRndCtxBaseAddr + PROCESS_RND_CTX_SIZE;
        g_testHwUserBaseAddr = g_testHwUserStackBaseAddr + THREAD_STACK_SIZE;
        return 0;
    }
    processMap->numOfThreads = numOfThreads;
    threadOffset = processMap->processHwRndCtxBaseAddr + PROCESS_RND_CTX_SIZE;
    threadSize = (userSpaceSize-PROCESS_RND_CTX_SIZE)/numOfThreads;
    if (threadSize <= (THREAD_RND_CTX_SIZE+THREAD_STACK_SIZE)) {
        TEST_PRINTF_ERROR("Error: threadSize too small\n");  // should not happen
        goto end_error;
    }
    for (i = 0; i < numOfThreads; i++) {
        processMap->threadMapAddr[i].threadHwRndCtxBaseAddr = threadOffset;
        processMap->threadMapAddr[i].threadHwRndWorkBuffBaseAddr = processMap->threadMapAddr[i].threadHwRndCtxBaseAddr + PROCESS_RND_WORKBUFF_OFFSET;
        processMap->threadMapAddr[i].threadHwUserStackBaseAddr = processMap->threadMapAddr[i].threadHwRndCtxBaseAddr + THREAD_RND_CTX_SIZE;
        processMap->threadMapAddr[i].threadHwUserBaseAddr = processMap->threadMapAddr[i].threadHwUserStackBaseAddr + THREAD_STACK_SIZE;
        threadOffset += threadSize;

        TEST_PRINTF("threadHwRndCtxBaseAddr = 0x%lx\n", (unsigned long)processMap->threadMapAddr[i].threadHwRndCtxBaseAddr);
        TEST_PRINTF("UserStackBaseAddr for thread #%d = 0x%lx\n", i, (unsigned long)processMap->threadMapAddr[i].threadHwUserStackBaseAddr);
        TEST_PRINTF("UserBaseAddr for thread #%d = 0x%lx\n", i, (unsigned long)processMap->threadMapAddr[i].threadHwUserBaseAddr);
    }
    return 0;
end_error:
    if (processMap->processHwRndCtxBaseAddr != 0) {
        munmap((void *)processMap->processHwRndCtxBaseAddr, processMap->userSpaceSize);
    }
#ifdef DX_PLAT_JUNO
    close(userSpaceFd);
#endif
    return 1;
}


// workspace mapping supports 1 process and up to 16 threads:
// process RND CTX - 16KB
// for each thread:
//      RND CTX - 16KB
//      stack - 16KB
//      user Data - (((256MB-16KB)/16)-32KB)
unsigned int TestMapProcessAddrs(uint32_t numOfThreads, ProcessMappingArea_t *processMap)
{
    uint32_t rc = 0;

    if (processMap == NULL) {
        TEST_PRINTF_ERROR("Invalid processMap, exiting...\n");
        return 1;
    }
    if (map_used > 0) {
        if (map_used == MAX_MAPPED_ALLOWED) {
            TEST_PRINTF_ERROR("too many mapping, exiting...\n");
            return 1;
        }
        map_used++;
        TEST_PRINTF_ERROR("process FD already opened, nothing to map\n");
        return 0;
    }

    map_used++;
    memset(processMap, 0, sizeof(ProcessMappingArea_t));

    processHwFd = open("/dev/mem", O_RDWR|O_SYNC);
    if(processHwFd < 0) {
        TEST_PRINTF_ERROR("Error: Can not open /dev/mem\n");
        goto end_with_error;
    }
    processMap->numOfThreads = 0;
    processMap->processHwRegBaseAddr = (unsigned long)mmap(NULL, REG_MAP_AREA_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, processHwFd, DX_BASE_CC);
    if (!VALID_MAPPED_ADDR(processMap->processHwRegBaseAddr)) {
        TEST_PRINTF_ERROR("Error: mmap failed for processHwRegBaseAddr 0x%lx\n", (unsigned long)processMap->processHwRegBaseAddr);
        goto end_with_error;
    }
    TEST_PRINTF("processHwRegBaseAddr = 0x%lx\n", (unsigned long)processMap->processHwRegBaseAddr);

    processMap->processHwEnvBaseAddr = (unsigned long)mmap(NULL, REG_MAP_AREA_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, processHwFd, DX_BASE_ENV_REGS);
    if (!VALID_MAPPED_ADDR(processMap->processHwEnvBaseAddr)) {
        TEST_PRINTF_ERROR("Error: mmap failed for processHwEnvBaseAddr\n");
        goto end_with_error;
    }
    TEST_PRINTF("processHwEnvBaseAddr = 0x%lx\n", (unsigned long)processMap->processHwEnvBaseAddr);

    rc = mapUserSpace(numOfThreads, processMap, USER_WORKSPACE_MEM_SIZE);
    if (rc != 0) {
        TEST_PRINTF_ERROR("Error: failed for mapUserSpace %d\n", rc);
        goto end_with_error;
    }
    g_testHwRegBaseAddr = processMap->processHwRegBaseAddr;
    g_testHwEnvBaseAddr = processMap->processHwEnvBaseAddr;

    return 0;
end_with_error:
    TestMunMapProcessAddrs(processMap);
    return 1;

}


/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
void TestMunMapProcessAddrs(ProcessMappingArea_t *processMap)
{

    if (processMap == NULL) {
        TEST_PRINTF_ERROR("processMap == NULL\n");
        return;
    }
    if (map_used > 0) {
        map_used--;
    } else {
        TEST_PRINTF_ERROR("nothing is mapped. returning...\n");
        return;
    }
    if (map_used > 0) {
        TEST_PRINTF_ERROR("waiting for others to Munmap before closing...\n");
        return;
    }
    if (processMap->processHwRegBaseAddr != 0) {
        munmap((void *)processMap->processHwRegBaseAddr, REG_MAP_AREA_LEN);
    }
    if (processMap->processHwEnvBaseAddr != 0) {
        munmap((void *)processMap->processHwEnvBaseAddr, REG_MAP_AREA_LEN);
    }
    if (processMap->processHwRndCtxBaseAddr != 0) {
        munmap((void *)processMap->processHwRndCtxBaseAddr, processMap->userSpaceSize);
    }
#ifdef DX_PLAT_JUNO
    close(userSpaceFd);
#endif
    close(processHwFd);
    memset((uint8_t *)processMap, 0, sizeof(ProcessMappingArea_t));
    g_testHwRegBaseAddr = 0;
    g_testHwEnvBaseAddr = 0;
    userSpaceFd = 0;
    processHwFd = 0;
}


// map the REE CC HW base, to write to some registers to enable TEE works
unsigned int TestMapRee(void)
{
    //TEST_PRINTF("about to open\n");
    g_reeHwFd = open("/dev/mem", O_RDWR|O_SYNC);
    if(g_reeHwFd < 0) {
        TEST_PRINTF_ERROR("Error: Can not open /dev/mem\n");
        goto end_error;
    }
    TEST_PRINTF("about to mmap for DX_REE_BASE_CC 0x%lx", (unsigned long)DX_REE_BASE_CC);
    g_testHwReeRegBaseAddr = (unsigned long)mmap(NULL, REG_MAP_AREA_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, g_reeHwFd, DX_REE_BASE_CC);
    if (!VALID_MAPPED_ADDR(g_testHwReeRegBaseAddr)) {
        TEST_PRINTF_ERROR("Error: mmap failed for g_testHwReeRegBaseAddr\n");
        goto end_error;
    }
    TEST_PRINTF("g_testHwReeRegBaseAddr = 0x%lx\n", (unsigned long)g_testHwReeRegBaseAddr);
    return 0;

end_error:
    TestMunMapRee();

    return 1;

}


/*
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return void -

 */
void TestMunMapRee(void)
{


    if (g_testHwReeRegBaseAddr != 0) {
        munmap((void *)g_testHwReeRegBaseAddr, REG_MAP_AREA_LEN);
    }
    g_testHwReeRegBaseAddr = 0;

    if (g_reeHwFd > 0) {
        close(g_reeHwFd);
    }
    g_reeHwFd = 0;
}


