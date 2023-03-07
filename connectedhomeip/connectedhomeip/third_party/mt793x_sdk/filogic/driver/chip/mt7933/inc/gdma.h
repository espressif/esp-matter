/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

#ifndef _GDMA_H
#define _GDMA_H

#if (defined(MTK_MINICLI_ENABLE) && defined(MTK_GDMACLI_ENABLE)) || defined(MTK_FULLTEST_ENABLE1)

#define Patch_Print printf

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/


/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

#define MEM_TEST_SIZE           1024
#define RING_BUF_LENGTH         64
#define DMA_CH_TEST_CASE        64
#define DMA_SIZE_1B             0
#define DMA_SIZE_2B             1
#define DMA_SIZE_4B             2
#define DMA_BURST_SINGLE        0
#define DMA_BURST_4BEAT         1
#define DMA_BURST_8BEAT         2
#define DMA_BURST_16BEAT        3
#define DMA_WRAP_NO             0
#define DMA_WRAP_SRC            1
#define DMA_WRAP_DST            2

typedef struct dma_cfg_t {
    uint8_t channel; // 0~29
    uint32_t src;
    uint32_t dst;
    uint32_t count;
    uint32_t wppt;
    uint32_t wpto;
    uint32_t pgmaddr;
    uint32_t limiter;
    uint32_t wrap; // 0 no , 1  wrap on src,  2  wrap on dst
    uint32_t inc_src;
    uint32_t inc_dst;
    uint32_t size;   //DMA_SIZE_1B, DMA_SIZE_2B, DMA_SIZE_4B
    uint32_t b2w;
    uint32_t burst;  //DMA_BURST_SINGLE, DMA_BURST_4BEAT, DMA_BURST_8BEAT, DMA_BURST_16BEAT
    uint32_t iten;
    uint32_t toen;
    uint32_t hiten;
    uint32_t dir;
    uint32_t dreq;
    uint32_t rllen;
    uint32_t master;
    uint32_t start;
    uint32_t altmode; // 0 over altlen, 1 under altlen
    uint32_t altlen;
    uint32_t ffsize;
    uint32_t cascaded_port;
    uint32_t cvff_en;
    uint32_t ff_timeout;
} dma_cfg_t;

typedef struct dma_test_t {
    dma_cfg_t cfg;
    uint32_t rlct;
    uint8_t operation_state;
    uint8_t check_pause_function;
    uint32_t t_start;
    uint32_t t_end;
} dma_test_t;


//BUS_HECXXXXX_001[
#define MEMBITBAND_PERI_REF   0x20000000
#define MEMBITBAND_PERI_BASE  0x22000000
#define MEMBITBAND_PERI(a,b) ((MEMBITBAND_PERI_BASE + (a-MEMBITBAND_PERI_REF)*32 + (b*4)))  // Convert PERI address
#define MEMBITBAND_SET(Field,data)    ((*(volatile uint32_t *)  (MEMBITBAND_PERI((Field##_ADDR),(Field##_SHFT)))) = (uint32_t)(data))
//BUS_HECXXXXX_001]

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
#define _DMA_SW
#ifdef _DMA_SW
#if (PRODUCT_VERSION == 3610)
#define fullsize_dma_test(ch, mode)         (dma_test(ch, mode))
#define halfsize_dma_test(ch, mode)         (dma_test(ch, mode))
#define vff_dma_test(ch, mode)              (dma_test(ch, mode))
#endif /* #if (PRODUCT_VERSION == 3610) */

#if (PRODUCT_VERSION == 3620)
#define fullsize_dma_test(ch, mode)         (dma_loopback_test(ch, mode))
#define halfsize_dma_test(ch, mode)         (dma_loopback_test(ch, mode))
#define vff_dma_test(ch, mode)              (dma_loopback_test(ch, mode))
#endif /* #if (PRODUCT_VERSION == 3620) */

#define fullsize_dma_test_case1(loop)       (dma_test_case1(DMA_FULL_CH_S, DMA_FULL_CH_E, loop))
#define halfsize_dma_test_case1(loop)       (dma_test_case1(DMA_HALF_CH_S, DMA_HALF_CH_E, loop))
#define vff_dma_test_case1(loop)            (dma_test_case1(DMA_VFIFO_CH_S, DMA_VFIFO_CH_E, loop))

#define fullsize_dma_test_case2()       (dma_test_case2(DMA_FULL_CH_S, DMA_FULL_CH_E))
#define halfsize_dma_test_case2()       (dma_test_case2(DMA_HALF_CH_S, DMA_HALF_CH_E))
#define vff_dma_test_case2()            (dma_test_case2(DMA_VFIFO_CH_S, DMA_VFIFO_CH_E))

#define halfsize_dma_test_case4()       (dma_test_case4(DMA_HALF_CH_S, DMA_HALF_CH_E))
#endif /* #ifdef _DMA_SW */

uint32_t trng_generator(void);

void gdma_ini(void);

void DMA_SOFTSTOP(uint8_t channel);

void clear_dma_global_status(uint8_t channel);

void wmt_dma_transfer_complete(void);

uint32_t WMT_DMA_Done(uint8_t channel, uint8_t int_en, uint8_t mode);




//void fill_testing_pattern2(uint8_t accessing_port, uint32_t addr, uint32_t length, uint8_t fix_pattern);

uint32_t data_comparison(uint32_t src, uint32_t dst, uint32_t lens_in_byte, uint32_t wrap_mode, uint32_t wppt_cnt, uint32_t wpto_addr);

int32_t wmt_config_dma(dma_cfg_t *cfg);

uint8_t fullsize_dma_transfer(uint8_t ch, uint32_t src, uint8_t inc_src, uint32_t dst, uint8_t inc_dst, uint32_t lens_in_byte, uint32_t size_in_0_1_2, uint32_t burst_mode, uint8_t int_en);

void vff_configuration(uint32_t ch, uint32_t dir, uint32_t addr, uint32_t ffsize, uint32_t transfer_size, uint32_t transfer_burst);

uint8_t full_and_half_dma_pause_function_chk(uint32_t ch);



uint8_t fullsize_dma_ch_test(uint32_t src, uint32_t dst, uint32_t len, uint8_t channel, uint8_t mode);

#ifndef _DMA_SW
int8_t fullsize_dma_test(uint8_t ch, uint8_t mode);
#endif /* #ifndef _DMA_SW */

int8_t halfsize_dma_ch_test_process(dma_test_t *testcfg);

uint8_t halfsize_dma_ch_test(uint32_t src, uint32_t dst, uint32_t len, uint8_t channel, uint8_t mode, uint8_t dir);

#ifndef _DMA_SW
int8_t halfsize_dma_test(uint8_t ch, uint8_t mode);
#endif /* #ifndef _DMA_SW */

void vff_dma_write_data(uint8_t dma_no, uint8_t *data, uint32_t length);

//void emulUARTTestCase_A9(uint8_t dma_no);
//int8_t uart_cli_Case_A10_Dma(uint8_t dma_no);

#ifndef _DMA_SW
int8_t fullsize_dma_test_case1(uint32_t loop);

int8_t halfsize_dma_test_case1(uint32_t loop);

int8_t vff_dma_test_case1(uint32_t loop);
#endif /* #ifndef _DMA_SW */

int8_t DMA_Access_Matrix_Check1(uint32_t loop);

//int8_t DMA_Access_Matrix_Check2(uint32_t loop);

#ifdef _DMA_SW
extern int8_t dma_test(uint8_t ch, uint8_t mode);
extern int8_t dma_test_case1(uint8_t start_ch, uint8_t end_ch, uint32_t loop);
extern int8_t dma_test_case2(uint8_t start_ch, uint8_t end_ch);
extern int8_t dma_test_case3(void);
extern int8_t dma_test_case4(uint8_t start_ch, uint8_t end_ch);
extern uint8_t heavy_dma_test(uint32_t loop);
extern int8_t dma_priority_test(void);
extern int8_t dma_sw_reset_test(void);
#ifdef MT3620_DMA_PRIVATE
extern int8_t DMA_Register_Check(void);

extern int8_t dma_loopback_test(uint8_t ch, uint8_t mode);
int8_t dma_lpk_test(uint8_t mode);
int8_t dma_lpk_ag_test(uint32_t loop);
#endif /* #ifdef MT3620_DMA_PRIVATE */

#endif /* #ifdef _DMA_SW */



#endif /* #if (defined(MTK_MINICLI_ENABLE) && defined(MTK_GDMACLI_ENABLE)) || defined(MTK_FULLTEST_ENABLE1) */

#ifdef FULLSIZE_DMA_TASK
void dma_fullsize_task_init(void);
void DMA_fullsize_req(uint32_t src, uint32_t dst, uint32_t length);
uint32_t dma_fullsize_data_comparison(uint32_t src, uint32_t dst, uint32_t lens_in_byte, uint32_t wrap_mode, uint32_t wppt_cnt, uint32_t wpto_addr);
#endif /* #ifdef FULLSIZE_DMA_TASK */

#ifdef MTK_FULLTEST_ENABLE1
uint32_t dma_fullsize_start_interrupt_FULLTEST(uint32_t channel, uint32_t destination_address, uint32_t source_address, uint32_t data_length);
#endif /* #ifdef MTK_FULLTEST_ENABLE1 */

#if (defined(MTK_MINICLI_ENABLE) && defined(MTK_GDMACLI_ENABLE)) || defined(MTK_FULLTEST_ENABLE1)
void fill_testing_pattern(uint8_t accessing_port, uint32_t addr, uint32_t length, uint32_t fix_pattern);
void fill_fixed_pattern(uint8_t accessing_port, uint32_t addr, uint32_t length, uint32_t fill_fix_pattern);
#endif /* #if (defined(MTK_MINICLI_ENABLE) && defined(MTK_GDMACLI_ENABLE)) || defined(MTK_FULLTEST_ENABLE1) */


#endif /* #ifndef _GDMA_H */
