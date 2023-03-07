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
#include "lmac154.h"
#include "bl702.h"
#include "bl702_glb.h"
#include "bl_irq.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#define LMAC154_PRINT         1  // enable log print in this example

#ifndef LMAC154_CASE
#define LMAC154_CASE          0  // 0: normal tx & rx, 1: throughput test, 2: range test
#endif

#ifndef LMAC154_TXRX
#define LMAC154_TXRX          1  // 1: tx, 0: rx
#endif

#ifndef LMAC154_TX_RATE
#define LMAC154_TX_RATE       0  // 0: 250K, 1: 500K, 2: 1M, 3: 2M
#endif

#if LMAC154_CASE == 1
#define LMAC154_PKT_SIZE      (LMAC154_TX_RATE == 0 ? 127 - 2 : 255 - 2)
#define LMAC154_PKT_CNT       3000
#elif LMAC154_CASE == 2
#define LMAC154_PKT_SIZE      (9 + 3)
#define LMAC154_PKT_CNT       100
#else
#define LMAC154_PKT_SIZE      (9 + 3)
#endif


#if LMAC154_PRINT == 0
#define printf(...)           (void)0
#endif


static uint16_t pan_id = 0xBBAA;

#if LMAC154_TXRX == 1
static uint16_t dst_addr = 0x3412;
static uint16_t src_addr = 0x7856;
#else
static uint16_t dst_addr = 0x7856;
static uint16_t src_addr = 0x3412;
#endif

static uint8_t csma_en = 1;
static uint8_t req_ack = 1;
static uint8_t mac_pkt[LMAC154_PKT_SIZE] = {0x61, 0x88, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x11, 0x22, 0x33};

#if LMAC154_CASE == 1 || LMAC154_CASE == 2
static uint32_t tx_cnt = 0;
static uint32_t ack_cnt = 0;
static uint32_t rx_cnt = 0;
#endif

#if LMAC154_CASE == 2
SemaphoreHandle_t sem = NULL;
static uint8_t ack_ok = 0;
static uint8_t app_retry = 0;
#endif


#define SEQ_NUM               mac_pkt[2]
#define PAN_ID_LO             mac_pkt[3]
#define PAN_ID_HI             mac_pkt[4]
#define DST_ADDR_LO           mac_pkt[5]
#define DST_ADDR_HI           mac_pkt[6]
#define SRC_ADDR_LO           mac_pkt[7]
#define SRC_ADDR_HI           mac_pkt[8]


static void lmac154_send_packet(void)
{
    lmac154_triggerTx(mac_pkt, sizeof(mac_pkt), csma_en);
}


void lmac154_example_task(void *pvParameters)
{
    uint8_t cca_result;
    int cca_rssi;
    
    vTaskDelay(1000);
    
    lmac154_init();
    lmac154_enableCoex();
    lmac154_setTxPower(LMAC154_TX_POWER_14dBm);
    lmac154_setChannel(LMAC154_CHANNEL_12);
    lmac154_setPanId(pan_id);
    lmac154_setShortAddr(src_addr);
    
    bl_irq_register(M154_IRQn, lmac154_getInterruptHandler());
    bl_irq_enable(M154_IRQn);
    
    PAN_ID_LO = pan_id & 0xFF;
    PAN_ID_HI = pan_id >> 8;
    DST_ADDR_LO = dst_addr & 0xFF;
    DST_ADDR_HI = dst_addr >> 8;
    SRC_ADDR_LO = src_addr & 0xFF;
    SRC_ADDR_HI = src_addr >> 8;
    
    if(req_ack){
        mac_pkt[0] |= 0x20;
    }else{
        mac_pkt[0] &= 0xDF;
    }
    
    printf("RF (channel %d) Init\r\n", 11 + lmac154_getChannel());
    
    cca_result = lmac154_runCCA(&cca_rssi);
    printf("CCA: %d, RSSI: %ddBm\r\n\r\n", cca_result, cca_rssi);
    
    lmac154_setTxDataRate(LMAC154_TX_RATE);
    lmac154_enableAutoRxDataRate();
    lmac154_enableFrameTypeFiltering(LMAC154_FRAME_TYPE_DATA | LMAC154_FRAME_TYPE_ACK);
    lmac154_enableRx();
    
#if LMAC154_TXRX == 1
#if LMAC154_CASE == 1
    lmac154_send_packet();
    
    while(1){
        vTaskDelay(1000);
        
        printf("tx_cnt: %lu, ack_cnt: %lu\r\n\r\n", tx_cnt, ack_cnt);
        
        if(ack_cnt == LMAC154_PKT_CNT){
            printf("Throughput test finished!\r\n\r\n");
            
            vTaskDelay(portMAX_DELAY);
        }
    }
#elif LMAC154_CASE == 2
    lmac154_setTxRetry(3);
    sem = xSemaphoreCreateCounting(1, 0);
    
    while(1){
        lmac154_send_packet();
        xSemaphoreTake(sem, portMAX_DELAY);
        
        if(!ack_ok){
            app_retry++;
        }
        
        if(ack_ok || app_retry > 5){
            app_retry = 0;
            SEQ_NUM += 1;
            
            tx_cnt++;
            printf("tx_cnt: %lu\r\n", tx_cnt);
            printf("ack_cnt: %lu\r\n\r\n", ack_cnt);
        }
        
        vTaskDelay(100);
        
        if(tx_cnt == LMAC154_PKT_CNT){
            printf("Range test finished!\r\n\r\n");
            
            vTaskDelay(portMAX_DELAY);
        }
    }
#else
    while(1){
        lmac154_send_packet();
        
        vTaskDelay(1000);
    }
#endif
#else
#if LMAC154_CASE == 1
    uint32_t rx_cnt_last = 0;
    uint32_t rx_cnt_curr;
    uint32_t throughput;
    
    while(1){
        vTaskDelay(1000);
        
        rx_cnt_curr = rx_cnt;
        throughput = (rx_cnt_curr - rx_cnt_last) * (LMAC154_PKT_SIZE - 9) * 8 / 1000;
        rx_cnt_last = rx_cnt_curr;
        
        printf("packet count: %lu, payload size: %dbytes, throughput: %lukbps\r\n\r\n", rx_cnt_curr, LMAC154_PKT_SIZE - 9, throughput);
    }
#elif LMAC154_CASE == 2
    SEQ_NUM = 0xFF;
    
    while(1){
        vTaskDelay(portMAX_DELAY);
    }
#else
    while(1){
        vTaskDelay(portMAX_DELAY);
    }
#endif
#endif
}

#if LMAC154_CASE == 1
void lmac154_txDoneEvent(lmac154_tx_status_t tx_status)
{
    if(tx_status == 0){
        tx_cnt++;
    }else{
        printf("tx failed, status: %d\r\n\r\n", tx_status);
    }
}

void lmac154_ackEvent(uint8_t ack_received, uint8_t frame_pending, uint8_t seq_num)
{
    if(ack_received){
        ack_cnt++;
    }
    
    if(ack_cnt < LMAC154_PKT_CNT){
        lmac154_send_packet();
    }
}

void lmac154_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t crc_fail)
{
    if(rx_buf[2] == SEQ_NUM && rx_len == LMAC154_PKT_SIZE + 2){
        rx_cnt++;
    }
}
#elif LMAC154_CASE == 2
void lmac154_txDoneEvent(lmac154_tx_status_t tx_status)
{
    if(tx_status == 0){
        //tx_cnt++;
        //printf("tx_cnt: %lu\r\n", tx_cnt);
    }else{
        printf("tx failed, status: %d\r\n\r\n", tx_status);
    }
}

void lmac154_ackEvent(uint8_t ack_received, uint8_t frame_pending, uint8_t seq_num)
{
    if(ack_received){
        ack_cnt++;
    }
    
    //printf("ack_cnt: %lu\r\n\r\n", ack_cnt);
    
    ack_ok = ack_received;
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void lmac154_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t crc_fail)
{
    int rssi;
    int rssi_avg;
    static int rssi_max = 0;
    static int rssi_min = 0;
    static int rssi_sum = 0;
    
    if(rx_buf[2] != SEQ_NUM && rx_len == LMAC154_PKT_SIZE + 2){
        SEQ_NUM = rx_buf[2];
        
        rssi = lmac154_getRSSI();
        rx_cnt++;
        
        if(rx_cnt == 1){
            rssi_max = rssi;
            rssi_min = rssi;
        }else{
            if(rssi > rssi_max){
                rssi_max = rssi;
            }
            if(rssi < rssi_min){
                rssi_min = rssi;
            }
        }
        
        rssi_sum += rssi;
        rssi_avg = rssi_sum / (int)rx_cnt;
        
        printf("rssi: %ddBm, rssi_max: %ddBm, rssi_min: %ddBm, rssi_avg: %ddBm, rx_cnt: %lu\r\n\r\n", rssi, rssi_max, rssi_min, rssi_avg, rx_cnt);
    }
}
#else
void lmac154_txDoneEvent(lmac154_tx_status_t tx_status)
{
    printf("TX Done (STATUS: %d, SN: 0x%02X)\r\n\r\n", tx_status, SEQ_NUM);
}

void lmac154_ackEvent(uint8_t ack_received, uint8_t frame_pending, uint8_t seq_num)
{
    if(ack_received){
        printf("Ack Received (FP: %d, SN: 0x%02X)\r\n\r\n", frame_pending, seq_num);
    }else{
        printf("No Ack\r\n\r\n");
    }
}

void lmac154_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t crc_fail)
{
    int i;
    
    printf("RX:");
    for(i=0; i<rx_len; i++){
        printf(" %02X", rx_buf[i]);
    }
    printf("\r\n");
    printf("RSSI: %ddBm, LQI: %d, FREQ_OFFSET: %dHz\r\n\r\n", lmac154_getRSSI(), lmac154_getLQI(), lmac154_getFrequencyOffset());
    
    SEQ_NUM += 1;
    
#if LMAC154_TXRX == 0
    lmac154_send_packet();
#endif
}
#endif
