/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "common_util_log.h"

/**
 * @brief This function reads bytes from text file into provided buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
int32_t CC_CommonUtilCopyDataFromRawTextFile (uint8_t *fileName, uint8_t *outBuff, uint32_t *outBuffLen)
{
    int32_t status = 0;
    FILE *fd;
    int32_t actualFileLen=0;
    size_t actualRead=0;
    size_t maxBytesToRead = 0;


    if ((NULL == fileName) ||
        (NULL == outBuff) ||
        (NULL == outBuffLen)) {
        UTIL_LOG_ERR( "ilegal parameters for %s\n", __func__);
        return 1;
    }
    if (0 == *outBuffLen) {
        UTIL_LOG_ERR( "ilegal outBuffLen \n");
        return 1;
    }
    fd = fopen(fileName, "rt");
    if (NULL == fd) {
        UTIL_LOG_ERR( "failed to open file %s for reading\n", fileName);
        return 1;
    }
    memset(outBuff, 0, *outBuffLen);

    /* Get file length */
    fseek(fd, 0, SEEK_END);
    actualFileLen = ftell(fd);
    if (actualFileLen == -1) {
        UTIL_LOG_ERR( "ftell error actualFileLen == -1\n");
        status = 1;
        goto EXIT;
    }
    fseek(fd, 0, SEEK_SET);

    /* calculate max bytes to read. should be the min of bytes in file and buffer size*/
    maxBytesToRead = (actualFileLen > (*outBuffLen))?(*outBuffLen):actualFileLen;
    if (0 == maxBytesToRead) {
        UTIL_LOG_ERR( "ilegal maxBytesToRead == 0\n");
        status = 1;
        goto EXIT;
    }

    /* read file content */
    actualRead = fread(outBuff, 1, maxBytesToRead, fd);

    while ((outBuff[actualRead-1] == ' ') ||
           (outBuff[actualRead-1] == '\n') ||
           (outBuff[actualRead-1] == '\0') ||
           (outBuff[actualRead-1] == 0x0A) ||
           (outBuff[actualRead-1] == 0x0D)) {
        actualRead--;
    }
    *outBuffLen = actualRead;

    EXIT:
    if (fd != NULL) {
        fclose(fd);
    }
    return status;
}


/**
 * @brief This function reads bytes from text file into provided buffer
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
int32_t CC_CommonUtilCopyDataFromTextFile (uint8_t *fileName, uint8_t *outBuff, uint32_t *outBuffLen)
{
    #define NUM_OF_CHARS_FOR_BYTE 4
    int32_t status = 0;
    FILE *fd;
    int32_t i = 0, j=0, k=0;
    int32_t actualFileLen=0;
    int32_t tempNum=0;
    size_t actualRead=0;
    size_t maxBytesToRead = 0;
    int8_t *filebufptr = NULL;
    int8_t str[NUM_OF_CHARS_FOR_BYTE+1];


    if ((NULL == fileName) ||
        (NULL == outBuff) ||
        (NULL == outBuffLen)) {
        UTIL_LOG_ERR( "ilegal parameters for %s\n", __func__);
        return 1;
    }
    if (0 == *outBuffLen) {
        UTIL_LOG_ERR( "ilegal outBuffLen \n");
        return 1;
    }
    fd = fopen(fileName, "rt");
    if (NULL == fd) {
        UTIL_LOG_ERR( "failed to open file %s for reading\n", fileName);
        return 1;
    }
    memset(outBuff, 0, *outBuffLen);

    /* Get file length */
    fseek(fd, 0, SEEK_END);
    actualFileLen = ftell(fd);
    if (actualFileLen == -1) {
        UTIL_LOG_ERR( "ftell error actualFileLen == -1\n");
        status = 1;
        goto EXIT;
    }
    fseek(fd, 0, SEEK_SET);

    /* calculate max bytes to read. should be the min of bytes in file and buffer size*/
    maxBytesToRead = (actualFileLen > (*outBuffLen*5))?(*outBuffLen*5):actualFileLen;
    if (0 == maxBytesToRead) {
        UTIL_LOG_ERR( "ilegal maxBytesToRead == 0\n");
        status = 1;
        goto EXIT;
    }


    /* allocate buffer for data from file */
    filebufptr = (int8_t*)malloc(maxBytesToRead+1);
    if (filebufptr == NULL) {
        UTIL_LOG_ERR( "failed to allocate memory\n");
        status = 1;
        goto EXIT;
    }

    /* NULL terminated string to avoid buffer overflow of the sscanf that is used later */
    filebufptr[maxBytesToRead] = '\0';

    /* read file content */
    actualRead = fread(filebufptr, 1, maxBytesToRead, fd);
    j=0;
    k=0;
    for (i=0; i<maxBytesToRead; i++) {
        if (((filebufptr[i] >= '0') && (filebufptr[i] <= '9')) ||
            ((filebufptr[i] >= 'a') && (filebufptr[i] <= 'f')) ||
            ((filebufptr[i] >= 'A') && (filebufptr[i] <= 'F')) ||
            (filebufptr[i] == 'x') || (filebufptr[i] == 'X') &&
            (k<NUM_OF_CHARS_FOR_BYTE)) {
            str[k++] = filebufptr[i];
        } else {
            if ((filebufptr[i] == ' ') ||
                (filebufptr[i] == '\n') ||
                (filebufptr[i] == '\0') ||
                (filebufptr[i] == ',')) {
                if (k>0) {
                    str[k] = '\0';
                    tempNum = strtol(str, NULL, 16);
                    if ((LONG_MIN == tempNum) ||
                        (LONG_MAX == tempNum)) {
                        UTIL_LOG_ERR( "strtol failed. check file name %s\n", fileName);
                        status = 1;
                        goto EXIT_AND_FREE;
                    }
                    outBuff[j++] = tempNum;
                    k = 0;
                }
                continue;
            } else {
                UTIL_LOG_ERR( "ilegal uint8_t in file %c offset %d within file name %s\n", filebufptr[i], i, fileName);
                status = 1;
                goto EXIT_AND_FREE;
            }
        }
    }
    *outBuffLen = j;

    EXIT_AND_FREE:
    if (filebufptr != NULL) {
        free(filebufptr);
    }
    EXIT:
    if (fd != NULL) {
        fclose(fd);
    }
    return status;
}


/**
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
int32_t CC_CommonUtilCopyDataFromBinFile(uint8_t *fileName, uint8_t *outBuff, uint32_t *outBuffLen)
{
    int32_t rc = 0;
    FILE *fd;
    size_t actualRead = 0;
    size_t actualFileLen = 0;


    if ((NULL == fileName) ||
        (NULL == outBuff) ||
        (0 == *outBuffLen)) {
        UTIL_LOG_ERR( "ilegal parameters for %s\n", __func__);
        return 1;
    }
    UTIL_LOG_INFO( "opening %s\n", fileName);
    fd = fopen(fileName, "rb");
    if (NULL == fd) {
        UTIL_LOG_ERR( "failed to open file %s for reading\n", fileName);
        return 1;
    }
    /* Get file length */
    fseek(fd, 0, SEEK_END);
    actualFileLen=ftell(fd);
    if (actualFileLen == -1)
    {
        UTIL_LOG_ERR( "failed to ftell file %s\n", fileName);
        goto EXIT_AND_FREE;
    }
    fseek(fd, 0, SEEK_SET);
    if (0 == actualFileLen) {
        UTIL_LOG_ERR( "ilegal actualFileLen == 0\n");
        rc = 3;
        goto EXIT_AND_FREE;
    }

    /* calculate max bytes to read. should be the min of bytes in file and buffer size*/
    if (actualFileLen > *outBuffLen) {
        UTIL_LOG_ERR( "ilegal actualFileLen %d > *outBuffLen %d\n", (int)actualFileLen, *outBuffLen);
        rc = 2;
        goto EXIT_AND_FREE;
    }

    /* read file content */
    actualRead = fread(outBuff, 1, actualFileLen, fd);
    if (actualRead == 0)
    {
        UTIL_LOG_ERR( "failed to open fread %s for reading\n", fileName);
        goto EXIT_AND_FREE;
    }
    if ((uint8_t)EOF == outBuff[actualRead-1]) {
        actualRead--;
    }
    *outBuffLen = actualRead;

    EXIT_AND_FREE:
    if (fd != NULL) {
        fclose(fd);
    }
    return rc;
}


/**
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
int32_t CC_CommonUtilCopyBuffToBinFile(uint8_t *fileName, uint8_t *inBuff, uint32_t inBuffLen)
{
    int32_t rc = 0;
    int32_t actualWriten = 0;
    FILE *fd;


    if ((NULL == fileName) ||
        (NULL == inBuff) ||
        (0 == inBuffLen)) {
        UTIL_LOG_ERR( "ilegal parameters for %s\n", __func__);
        return 1;
    }
    fd = fopen(fileName, "wb");
    if (NULL == fd) {
        UTIL_LOG_ERR( "failed to open file %s for writing\n", fileName);
        return 1;
    }

    actualWriten = fwrite(inBuff, 1, inBuffLen, fd);
    if (actualWriten != inBuffLen) {
        UTIL_LOG_ERR( "failed to write data to file actual written %d, expected %d\n", actualWriten, inBuffLen);
        rc = 1;
    }

    if (fd != NULL) {
        fclose(fd);
    }
    UTIL_LOG_ERR( "%d bytes were written %s\n", actualWriten, fileName);


    return rc;
}


/**
 * @brief The function reads the pwd file name gets the pwd and returns it
 *
 * @param[in] pPwdFileName - file name of the password
 * @param[out] pwd - passphrase data
 *
 */
/*********************************************************/
int32_t CC_CommonGetPassphrase(int8_t *pPwdFileName, uint8_t **pwd)
{

    FILE *fp = NULL;
    int32_t fsize = 0;
    int32_t seek =0, i=0;
    uint8_t *tmpBuf;
    int32_t status = 0;

    if (pPwdFileName == NULL) {
        UTIL_LOG_ERR("illegal file name\n");
        return -1;
    }

    if (pwd == NULL) {
        UTIL_LOG_ERR("illegal pwd\n");
        return -1;
    }

    fp = fopen (pPwdFileName, "r");
    if (fp == NULL) {
        UTIL_LOG_ERR ("Cannot open file %s\n", pPwdFileName);
        return -1;
    }


    /* Get the pwd file size */
    seek = fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fsize == 0) {
        UTIL_LOG_ERR("PWD file is empty!\n");
        status = -1;
        goto END;
    }

    tmpBuf = (int8_t *)malloc(fsize+1);
    if (tmpBuf == NULL) {
        UTIL_LOG_ERR("failed to allocate memory\n");
        status = -1;
        goto END;
    }

    memset(tmpBuf, 0, fsize+1);
    /* get the file data */
    for (i=0; i<fsize; i++) {
        tmpBuf[i] = (uint8_t)fgetc(fp);
        if (tmpBuf[i] == (uint8_t)EOF || tmpBuf[i] == '\n') {
            tmpBuf[i] = '\0';
        }
    }
    *pwd = tmpBuf;
    status = 0;

    END:
    fclose(fp);
    return status;
}
