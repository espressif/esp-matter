/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "test_pal_map_addrs.h"
#include "test_pal_log.h"

#define FD_MAX_NUM          10
#define FD_FILENAME_MAX_LEN     50

/* Each address has its own file descriptor for simplicity */
struct FdInfo {
    void    *physAddr;
    void    *virtAddr;
    char    filename[FD_FILENAME_MAX_LEN];
    int fd;
};

static struct FdInfo fdList[FD_MAX_NUM] = { {0} };
uint8_t fdNum = 0;

static int Test_PalOpenFd(void *physAddr, const char *filename);
static void Test_PalCloseFd(void *virtAddr);
static void Test_SetMmapFlags(int *prot, int *flags, uint8_t bitMask);

/******************************************************************************/
void *Test_PalIOMap(void *physAddr, size_t size)
{
    return Test_PalMapAddr(physAddr, NULL, "/dev/mem", size,
                     BM_READ|BM_WRITE|BM_SHARED);
}

/******************************************************************************/
void *Test_PalMapAddr(void *physAddr, void *startingAddr, const char *filename,
                size_t size, uint8_t protAndFlagsBitMask)
{
    int fdIndx = Test_PalOpenFd(physAddr, filename);
    int prot = 0;
    int flags = 0;

    if (fdIndx < 0)
        return NULL;

    Test_SetMmapFlags(&prot, &flags, protAndFlagsBitMask);

    fdList[fdIndx].virtAddr = mmap((unsigned long *)startingAddr,
                size,
                prot,
                flags, /* must use MAP_FIXED to align
                with secure boot image table */
                fdList[fdIndx].fd,
                (unsigned long)physAddr);

    if (!VALID_MAPPED_ADDR((unsigned long)fdList[fdIndx].virtAddr)) {
        Test_PalCloseFd(fdList[fdIndx].virtAddr);
        return NULL;
    }

    return fdList[fdIndx].virtAddr;
}

/******************************************************************************/
void Test_PalUnmapAddr(void *virtAddr, size_t size)
{
    Test_PalCloseFd(virtAddr);
    munmap(virtAddr, size);
}

/******************************************************************************/
/*              Internal API                      */
/******************************************************************************/
static int Test_PalOpenFd(void *physAddr, const char *filename)
{
    int i;
    int fdIndx = -1;

    /* Finds fd index in fdList */
    for (i = 0; i < fdNum; i++) {
        if (fdList[i].physAddr == physAddr)
            fdIndx = i;
    }

    /* physAddr was not found. Adds a new fd to fdList */
    if (fdIndx < 0) {
        if (fdNum < FD_MAX_NUM) {
            fdIndx = fdNum;
            fdList[fdIndx].physAddr = physAddr;
            fdList[fdIndx].virtAddr = 0;
            memset(fdList[fdIndx].filename, 0, FD_FILENAME_MAX_LEN);
            memcpy(fdList[fdIndx].filename, filename,
                            FD_FILENAME_MAX_LEN);
            fdList[fdIndx].fd = -1;
            fdNum++;
        } else {
            TEST_PRINTF_ERROR("Error: Too many fd\n");
            return -1;
        }
    }

    /* Opens fd */
    if (fdList[fdIndx].fd < 0) {
        fdList[fdIndx].fd = open(fdList[fdIndx].filename,
                        O_RDWR|O_SYNC);
        if (fdList[fdIndx].fd < 0) {
            TEST_PRINTF_ERROR("Error: Can not open %s\n",
                    fdList[fdIndx].filename);
            return -1;
        }
    }
    return fdIndx;
}

/******************************************************************************/
static void Test_PalCloseFd(void *virtAddr)
{
    int i;
    int fdIndx = -1;

    /* Finds fd index in fdList */
    for (i = 0; i < fdNum; i++) {
        if (fdList[i].virtAddr == virtAddr)
            fdIndx = i;
    }

    /* physAddr was found. Close fd if necessary. */
    if ((fdIndx >= 0) && (fdList[fdIndx].fd >= 0)) {
        close(fdList[fdIndx].fd);
        fdList[fdIndx].fd = -1;
    }
}

/******************************************************************************/
static void Test_SetMmapFlags(int *prot, int *flags, uint8_t bitMask)
{
    if (bitMask & BM_READ)
        *prot |= PROT_READ;
    if (bitMask & BM_WRITE)
        *prot |= PROT_WRITE;
    if (bitMask & BM_EXEC)
        *prot |= PROT_EXEC;
    if (bitMask & BM_NONE)
        *prot |= PROT_NONE;
    if (bitMask & BM_SHARED)
        *flags |= MAP_SHARED;
    if (bitMask & BM_PRIVATE)
        *flags |= MAP_PRIVATE;
    if (bitMask & BM_FIXED)
        *flags |= MAP_FIXED;
}
