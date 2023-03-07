/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "bl_dma.h"
#include "bl_cks.h"
#include <blog.h>
#define USER_UNUSED(a) ((void)(a))

static void _bl_cks_test_case1()
{
    /*case from wiki: https://en.wikipedia.org/wiki/IPv4_header_checksum*/
    static const uint8_t data_src1[] = 
    {
        0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40, 0x00, 0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01,
        0xc0, 0xa8, 0x00, 0xc7
    };
    static const uint8_t data_src1_cks[] =
    {
        0xB8, 0x61 
    };
    int i;
    uint16_t cks;
    USER_UNUSED(cks);
    USER_UNUSED(data_src1_cks);

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    for (i = 0; i < sizeof(data_src1); i++) {
        *(volatile uint8_t*)0x4000A704 = data_src1[i];
    }
    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS result with LE is %04x, should be %02x%02x\r\n", cks, data_src1_cks[1], data_src1_cks[0]);

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    *(volatile uint8_t*)0x4000A700 = 0x2;//BIG Endian
    for (i = 0; i < sizeof(data_src1); i++) {
        *(volatile uint8_t*)0x4000A704 = data_src1[i];
    }
    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS result with BE is %04x, should be %02x%02x\r\n", cks, data_src1_cks[1], data_src1_cks[0]);
}

static void _bl_cks_test_case2()
{
    uint16_t data_segment_one = 0x3F1F;
    int i;
    uint32_t checksum;
    uint16_t cks;

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 0);
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 8);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }

    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS LE result is %04x, %04x\r\n",
            cks,
            (uint16_t)~checksum
    );

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    *(volatile uint8_t*)0x4000A700 = 0x2;//BIG Endian
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 0);
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 8);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }

    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS BE result is %04x, %04x\r\n",
            cks,
            (uint16_t)~checksum
    );
    if (cks == ((uint16_t)~checksum)) {
        blog_info("====== Success %04X Checksum=====\r\n", cks);
    } else {
        blog_info("====== Failed %04X Checksum======\r\n", cks);
    }
}

static void _bl_cks_test_case3()
{
    uint16_t data_segment_one = 0x3F1F;
    int i;
    uint32_t checksum;
    uint16_t cks;

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    checksum = 0;
    for (i = 0; i < 1000; i++) {
        checksum += data_segment_one;
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 0);
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 8);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }

    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS LE result is %04x, %04x\r\n",
            cks,
            (uint16_t)~checksum
    );

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    *(volatile uint8_t*)0x4000A700 = 0x2;//BIG Endian
    checksum = 0;
    for (i = 0; i < 1000; i++) {
        checksum += data_segment_one;
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 0);
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 8);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }

    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS BE result is %04x, %04x\r\n",
            cks,
            (uint16_t)~checksum
    );
    if (cks == ((uint16_t)~checksum)) {
        blog_info("====== Success %04X Checksum=====\r\n", cks);
    } else {
        blog_error("====== Failed %04X Checksum======\r\n", cks);
    }
}

static void _bl_cks_test_case4()
{
    uint16_t data_segment_one = 0x3F1F;
    uint8_t data_segment_two = 0xa1;
    int i;
    uint32_t checksum;
    uint16_t cks;

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 0);
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 8);
    }
    checksum += data_segment_two;
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    *(volatile uint8_t*)0x4000A704 = data_segment_two;

    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS LE result is %04x, %04x\r\n",
            cks,
            (uint16_t)~checksum
    );

    *(volatile uint8_t*)0x4000A700 = 0x1;//REST
    *(volatile uint8_t*)0x4000A700 = 0x2;//BIG Endian
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 0);
        *(volatile uint8_t*)0x4000A704 = (data_segment_one >> 8);
    }
    checksum += data_segment_two;
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    *(volatile uint8_t*)0x4000A704 = data_segment_two;

    cks = *(volatile uint16_t*)0x4000A708;
    blog_info("CKS BE result is %04x, %04x\r\n",
            cks,
            (uint16_t)~checksum
    );
    if (cks == ((uint16_t)~checksum)) {
        blog_info("====== Success %04X Checksum=====\r\n", cks);
    } else {
        blog_error("====== Failed %04X Checksum======\r\n", cks);
    }
}

static void _cb_cmd(void *arg)
{
    struct bl_dma_item *first;

    first = (struct bl_dma_item*)arg;
    blog_info("[DMA] [TEST] Callback is working, arg is %p\r\n", arg);
    first->arg = NULL;
}

static void _bl_cks_test_case5()
{
    struct bl_dma_item first;
    struct bl_dma_item second;
    struct bl_dma_item last;
    uint16_t data_segment_one = 0x3F1F;
    uint16_t cks_result = 0;
    uint32_t cks_cmd_list[2] = {
        0x1, //reset command
        0x2, //Big endian
    };

    memset(&first, 0, sizeof(struct bl_dma_item));
    memset(&second, 0, sizeof(struct bl_dma_item));
    memset(&last, 0, sizeof(struct bl_dma_item));

    /*cks config section*/
    first.src = (uint32_t)&cks_cmd_list;
    first.dst = 0x4000A700;
    first.next = (uint32_t)&second.src;
    first.ctrl = (
            //BL_DMA_ITEM_BITS_IRQ_ENABLE |
            BL_DMA_ITEM_BITS_SRC_BURST_COUNT_01 |
            BL_DMA_ITEM_BITS_SRC_WIDTH_4BYTE |
            BL_DMA_ITEM_BITS_SRC_AUTO_INCR_ENABLE |
            BL_DMA_ITEM_BITS_DST_BURST_COUNT_01 |
            BL_DMA_ITEM_BITS_DST_WIDTH_4BYTE |
            (2)
    );
    first.cb = _cb_cmd;
    first.arg = &first;

    /*data copy section*/
    second.src = (uint32_t)&data_segment_one;
    second.dst = 0x4000A704;
    second.next = (uint32_t)&last.src;
    second.ctrl = (
            //BL_DMA_ITEM_BITS_IRQ_ENABLE |
            BL_DMA_ITEM_BITS_SRC_BURST_COUNT_01 |
            BL_DMA_ITEM_BITS_SRC_WIDTH_2BYTE |
            BL_DMA_ITEM_BITS_DST_BURST_COUNT_01 |
            BL_DMA_ITEM_BITS_DST_WIDTH_1BYTE |
            (1000)
    );
    second.cb = NULL;
    second.arg = NULL;

    /*copy result section*/
    last.src = 0x4000A708;
    last.dst = (uint32_t)&cks_result;
    last.next = 0;
    last.ctrl = (
            BL_DMA_ITEM_BITS_IRQ_ENABLE |
            BL_DMA_ITEM_BITS_SRC_BURST_COUNT_01 |
            BL_DMA_ITEM_BITS_SRC_WIDTH_2BYTE |
            BL_DMA_ITEM_BITS_DST_BURST_COUNT_01 |
            BL_DMA_ITEM_BITS_DST_WIDTH_2BYTE |
            (1)
    );
    last.cb = NULL;
    last.arg = NULL;

    bl_dma_copy(&first);

    /*Arg will be set in callback*/
    /*XXX volatile may needed*/
    while (first.arg) {
        vTaskDelay(2);
    }
    if (0x6DF1 == cks_result) {
        blog_info("====== Success %04X Checksum=====\r\n", cks_result);
    } else {
        blog_error("====== Failed %04X Checksum======\r\n", cks_result);
    }
}

int bl_cks_test(void)
{
    blog_info("--->>> case1 test\r\n");
    _bl_cks_test_case1();
    blog_info("--->>> case2 test\r\n");
    _bl_cks_test_case2();
    blog_info("--->>> case3 test\r\n");
    _bl_cks_test_case3();
    blog_info("--->>> case4 test\r\n");
    _bl_cks_test_case4();
    blog_info("--->>> case5 test\r\n");
    _bl_cks_test_case5();
    return 0;
}
