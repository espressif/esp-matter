/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_SDIO_SLAVE_INTERNAL_H__
#define __HAL_SDIO_SLAVE_INTERNAL_H__

#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED

#define SDIO_SLAVE_TX_QUEUE_START(n)       (0x100 << (n))
#define SDIO_SLAVE_TX_QUEUE_STOP(n)        (0x01 << (n))
#define SDIO_SLAVE_TX_QUEUE_RESUME(n)      (0x10000 << (n))


#define SDIO_SLAVE_TXQ_READY(n)          (0x01 << (n))
#define SDIO_SLAVE_TXQ_OVERFLOW(n)       (0x100 << (n))
#define SDIO_SLAVE_TXQ_CHKSUM_ERR(n)     (0x10000 << (n))
#define SDIO_SLAVE_TXQ_LEN_ERR(n)        (0x1000000 << (n))


#define SDIO_SLAVE_RXQ_DONE(n)           (0x01 << (n))
#define SDIO_SLAVE_RXQ_UNDERFLOW(n)      (0x100 << (n))
#define SDIO_SLAVE_RXQ_CHKSUN_ERR(n)     (0x10000 << (n))
#define SDIO_SLAVE_RXQ_OVERFLOW(n)       (0x1000000 << (n))
#define SDIO_SLAVE_RXQ_OWN_CLEAR(n)      (0x01 << (n))
#define SDIO_SLAVE_RXQ_LEN_ERR(n)        (0x100 << (n))


#define SDIO_SLAVE_RXQ_IOC_DISABLE(n)        (0x100 << (n))
#define SDIO_SLAVE_TXQ_IOC_DISABLE(n)        (0x01 << (n))

#define SDIO_SLAVE_TXQ_START   (1)   /*TXQ0 is removed, TXQ1 is the frist Queue.*/
#define SDIO_SLAVE_MAX_TXQ_NUM (1)
#define SDIO_SLAVE_RXQ_START   (0)
#define SDIO_SLAVE_MAX_RXQ_NUM (2)

#define SDIO_SLAVE_MAX_PACKET_LENGTH  (0x1000 - 4) /*4K-4*/


typedef struct {
    uint32_t    global_interrupt_enable_mask;
    uint16_t    sw_interrupte_enable_mask;
    uint16_t    fw_interrupte_enable_mask;
    uint32_t    tx0_interrupte_enable_mask;
    uint32_t    rx0_interrupte_enable_mask;
    uint32_t    rx1_interrupte_enable_mask;
} sdio_slave_interrupt_enable_t;


typedef struct {
    uint32_t    global_interrupt_status_mask;    //ORG_SDIO_HGFISR    Global  0004
    uint16_t    sw_interrupte_status_mask;        //ORG_SDIO_HWFIER[31:16].H2D_SW_INT    DMA    0100
    uint16_t    fw_interrupte_status_mask;        //ORG_SDIO_HWFIER[13:0]                DMA    0100
    uint32_t    tx0_interrupte_status_mask;    //ORG_SDIO_HWFTE0SR                    DMA    0110
    uint32_t    rx0_interrupte_status_mask;    //ORG_SDIO_HWFRE0SR                    DMA    0130
    uint32_t    rx1_interrupte_status_mask;    //ORG_SDIO_HWFRE1SR                    DMA    0134
} sdio_slave_interrupt_status_t;


typedef enum {
    GLO_INTR_Msk = 0,
    SW_INTR_Msk,
    FW_INTR_Msk,
    UL0_INTR_Msk,
    DL0_INTR_Msk,
    DL1_INTR_Msk,
    SET_ALL_Msk = 0xFF,
} hifsdio_isr_mask_e;


typedef struct {
    uint8_t           txq_number; /*total TX QMU queues requested to configure, this parameter maybe not equal to tx_port_number,because "TX one-to-multiple queue"*/
    uint8_t           rxq_number; /*total RX QMU queues requested to configure*/
    uint8_t           reserve[2];   /*reserve for 4 bytes alignment*/
    bool            checksum_enable; /*set the checksum function of QMU is enable or not*/
    bool            checksum_use_16bytes; /*set the checksum bytes, false = use 12bytes checksum,Notice : the SDIO IP only has 16bytes checksum now, 12bytes ability will be added in future. */
    bool            rx_ioc_disable[SDIO_SLAVE_MAX_RXQ_NUM]; /*set whether IOC function of DL queue is diable or not,(if disable, the interrupt will occurs when each GPD is transfered done no matter the IOC bit of GPD)*/
    hal_sdio_slave_callback_t sdio_slave_callback;            /*sdio_isr_cb_func is the call back function for ISR*/
    void           *sdio_slave_callback_user_data;
} sdio_slave_property_t;


typedef struct {
    bool           close_pio_clock; /*set if the clock of pio mode should be closed*/
    bool           close_ehpi_clock; /*set if the clock of ehpi mode should be closed*/
    bool           close_spi_clock; /*set if the clock of spi mode should be closed*/
    bool           close_sdio_clock; /*set if the clock of sdio mode should be closed (Always NOT)*/
    bool           force_high_speed_mode; /*set if we want use high-speed mode forcedly no matter the setting of EHS bit in CCCR (SDIO 2.0)*/
    bool           not_gate_ahb_clk; /*set if we want the AHB bus always alive in IP, not to be gated while not using*/
    bool           int_mask_at_terminal_cycle;
    bool           set_fw_own_back_if_any_interrupt; /*set if we want IP automatically set "FW OWN Back to host interrupt" when any FW interrupt occurs*/
    uint8_t        interface_mode; /*set which interface mode we want to use (0x0=sdio, 0x1=spi, 0x4=ehpi)*/
    uint8_t        tx_port_number; /*the TX PORT number of this SDIO HW*/
    bool           remove_tx_Header; /*set if we want HW to remove Tx Header by itself*/
    bool           remove_rx_redundant_zero; /*set if we want HW to remove Rx redundant 4-byte 0 by itself*/
} sdio_slave_hw_property_t;



typedef struct _hifsdio_priv {
    sdio_slave_hw_property_t         sdio_hw_property; /*hifsdio_hw_property used to store the upper layer driver configured info.*/
    sdio_slave_property_t            sdio_property; /*hifsdio_property used to store the upper layer driver configured info*/
    sdio_slave_interrupt_enable_t    sdio_isr_mask; /*sdio_isr_mask used to store the ISR mask information.*/
} sdio_slave_private_t;




/*GPD header define.*/
typedef struct __sdio_slave_gpd_header_t {
    struct {
        uint32_t hw0: 1;
        uint32_t bdp: 1;
        uint32_t bps: 1;
        uint32_t reserved0: 4;
        uint32_t ioc: 1;
        uint32_t checksum: 8;
        uint32_t allow_buffer_length: 16;
    } word_0;

    struct __sdio_slave_gpd_header_t *next;
    uint32_t buffer_address;

    union {
        struct {
            uint32_t buffer_length: 16;
            uint32_t extension_length: 8;
            unsigned reserved1: 8;
        } length;
        uint32_t reserved2;
    } word_3;
} sdio_slave_gpd_header_t;

typedef enum {
    SDIO_SLAVE_STATUS_RX_LEN_FIFO_OVERFLOW = -12,
    SDIO_SLAVE_STATUS_RX_UNDERFLOW = -11,
    SDIO_SLAVE_STATUS_TX_OVERFLOW = -10,
    SDIO_SLAVE_STATUS_HW_READ_TIMEOUT = -9,
    SDIO_SLAVE_STATUS_HW_WRITE_TIMEOUT = -8,
    SDIO_SLAVE_STATUS_ABNORMAL_HOST_BEHAVIOR = -7,
    SDIO_SLAVE_STATUS_USER_TIMEOUT = -6,
    SDIO_SLAVE_STATUS_ABNORMAL_SETTING = -5,
    SDIO_SLAVE_STATUS_CRC_ERROR = -4,
    SDIO_SLAVE_STATUS_INVALID_ERROR     = -3,
    SDIO_SLAVE_STATUS_INVALID_BUSY      = -2,
    SDIO_SLAVE_STATUS_INVALID_PARAMETER = -1,
    SDIO_SLAVE_STATUS_OK                = 0
} sdio_slave_status_t;

typedef enum {
    SDIO_SLAVE_TX_QUEUE_0 = 0,
    SDIO_SLAVE_TX_QUEUE_1 = 1,
    SDIO_SLAVE_TX_QUEUE_MAX
} sdio_slave_tx_queue_id_t;

typedef enum {
    SDIO_SLAVE_RX_QUEUE_0 = 0,
    SDIO_SLAVE_RX_QUEUE_1 = 1,
    SDIO_SLAVE_RX_QUEUE_MAX
} sdio_slave_rx_queue_id_t;


#define SDIO_SLAVE_GPD_GET_HW0(GPD)                  (GPD->word_0.hw0)
#define SDIO_SLAVE_GPD_SET_HW0(GPD)                  (GPD->word_0.hw0 = 1)
#define SDIO_SLAVE_GPD_CLR_HW0(GPD)                  (GPD->word_0.hw0 = 0)
#define SDIO_SLAVE_GPD_GET_BDP(GPD)                  (GPD->word_0.bdp)
#define SDIO_SLAVE_GPD_SET_BDP(GPD)                  (GPD->word_0.bdp = 1)
#define SDIO_SLAVE_GPD_CLR_BDP(GPD)                  (GPD->word_0.bdp = 0)
#define SDIO_SLAVE_GPD_GET_BPS(GPD)                  (GPD->word_0.bps)
#define SDIO_SLAVE_GPD_SET_BPS(GPD)                  (GPD->word_0.bps = 1)
#define SDIO_SLAVE_GPD_CLR_BPS(GPD)                  (GPD->word_0.bps = 0)
#define SDIO_SLAVE_GPD_GET_IOC(GPD)                  (GPD->word_0.ioc)
#define SDIO_SLAVE_GPD_SET_IOC(GPD)                  (GPD->word_0.ioc = 1)
#define SDIO_SLAVE_GPD_CLR_IOC(GPD)                  (GPD->word_0.ioc = 0)
#define SDIO_SLAVE_GPD_SET_CHECKSUM(GPD, CHECKSUM)   (GPD->word_0.checksum = CHECKSUM)
#define SDIO_SLAVE_GPD_SET_ALLOW_LENGTH(GPD, LEN)    (GPD->word_0.allow_buffer_length = LEN)
#define SDIO_SLAVE_GPD_GET_BUFFER_LENGTH(GPD)        (GPD->word_3.length.buffer_length)
#define SDIO_SLAVE_GPD_SET_BUFFER_LENGTH(GPD, LEN)   (GPD->word_3.length.buffer_length = LEN)
#define SDIO_SLAVE_GPD_GET_EXTENSION_LENGTH(GPD)     (GPD->word_3.length.extension_length)
#define SDIO_SLAVE_GPD_SET_EXTENSION_LENGTH(GPD, LEN) (GPD->word_3.length.extension_length = LEN)
#define SDIO_SLAVE_GPD_GET_NEXT_HEADER(GPD)          (GPD->next)
#define SDIO_SLAVE_GPD_SET_NEXT_HEADER(GPD, NEXT)    (GPD->next = NEXT)
#define SDIO_SLAVE_GPD_GET_BUFFER_ADDRESS(GPD)       (GPD->buffer_address)
#define SDIO_SLAVE_GPD_SET_BUFFER_ADDRESS(GPD, ADDR) (GPD->buffer_address = ADDR)


typedef struct {
    sdio_slave_gpd_header_t pGPD_Enq;
    sdio_slave_gpd_header_t pGPD_Deq;
    uint8_t   Nodes_Limit;
    uint8_t   Nodes_Current;
    bool      is_tx;
    uint8_t  *TX_USER_Buffer;
    uint8_t  *TX_USER_Buffer_Write_Ptr;
} sdio_slave_queue_t;

#define SDIO_QUEUE_GET_GPD_ENQ(PQUEUE)        (PQUEUE->pGPD_Enq)
#define SDIO_QUEUE_SET_GPD_ENQ(PQUEUE, ENQ)   (PQUEUE->pGPD_Enq=ENQ)
#define SDIO_QUEUE_GET_GPD_DEQ(PQUEUE)        (PQUEUE->pGPD_Deq)
#define SDIO_QUEUE_SET_GPD_DEQ(PQUEUE, DEQ)   (PQUEUE->pGPD_Deq=DEQ)
#define SDIO_QUEUE_GET_Q_HW_IDX(PQUEUE)       (PQUEUE->Q_HW_Idx)
#define SDIO_QUEUE_SET_Q_HW_IDX(PQUEUE, IDX)  (PQUEUE->Q_HW_Idx=IDX)
#define SDIO_QUEUE_GET_NODES_LIMIT(PQUEUE)    (PQUEUE->Nodes_Limit)
#define SDIO_QUEUE_SET_NODES_LIMIT(PQUEUE, LIMIT) (PQUEUE->Nodes_Limit=LIMIT)
#define SDIO_QUEUE_GET_NODES_CURRENT(PQUEUE)  (PQUEUE->Nodes_Current)
#define SDIO_QUEUE_SET_NODES_CURRENT(PQUEUE, CUR) (PQUEUE->Nodes_Current=CUR)
#define SDIO_QUEUE_DEC_NODES_CURRENT(PQUEUE, DEC) (PQUEUE->Nodes_Current-=DEC)
#define SDIO_QUEUE_INC_NODES_CURRENT(PQUEUE, INC) (PQUEUE->Nodes_Current+=INC)
#define SDIO_QUEUE_GET_ISTX(PQUEUE)           (PQUEUE->isTX)
#define SDIO_QUEUE_SET_ISTX(PQUEUE, ISTX)     (PQUEUE->isTX=ISTX)
#define SDIO_QUEUE_GET_TX_USER_BUFFER(PQUEUE, BUF)         (PQUEUE->TX_USER_Buffer)
#define SDIO_QUEUE_SET_TX_USER_BUFFER(PQUEUE, BUF)         (PQUEUE->TX_USER_Buffer=BUF)
#define SDIO_QUEUE_GET_TX_USER_BUFFER_WR_PTR(PQUEUE)       (PQUEUE->TX_USER_Buffer_Write_Ptr)
#define SDIO_QUEUE_SET_TX_USER_BUFFER_WR_PTR(PQUEUE, PTR)  (PQUEUE->TX_USER_Buffer_Write_Ptr=PTR)
#define SDIO_QUEUE_INC_TX_USER_BUFFER_WR_PTR(PQUEUE, INC)  (PQUEUE->TX_USER_Buffer_Write_Ptr+=INC)


void sdio_slave_read_mailbox(uint32_t mailbox_number, uint32_t *mailbox_value);
void sdio_slave_write_mailbox(uint32_t mailbox_number, uint32_t mailbox_value);
void sdio_slave_set_device_to_host_interrupt(uint32_t interrupt_number);
bool sdio_slave_check_fw_own(void);
void sdio_slave_give_fw_own(void);
void sdio_slave_pdn_set(bool enable);
void sdio_slave_private_init(void);
void sdio_slave_hardware_init(void);
void sdio_slave_hardware_deinit(void);
void sdio_slave_isr(hal_nvic_irq_t irq_number);
sdio_slave_status_t sdio_slave_interrupt_status_polling(void);
void sdio_slave_nvic_set(bool enable);
void sdio_slave_interrupt_init(void);
void sdio_slave_queue_property_config(void);
void sdio_slave_init(void);
void sdio_slave_deinit(void);
void sdio_slave_set_tx_queue_node_add(sdio_slave_tx_queue_id_t queue_id, uint32_t add_queue_number);
void sdio_slave_tx_queue_count_reset(void);
void sdio_slave_set_tx_queue_start(sdio_slave_tx_queue_id_t queue_id);
void sdio_slave_set_tx_queue_stop(sdio_slave_tx_queue_id_t queue_id);
void sdio_slave_set_tx_queue_resume(sdio_slave_tx_queue_id_t queue_id);
void sdio_slave_set_rx_queue_start(sdio_slave_rx_queue_id_t queue_id);
void sdio_slave_set_rx_queue_stop(sdio_slave_rx_queue_id_t queue_id);
void sdio_slave_set_rx_queue_resume_and_length(sdio_slave_rx_queue_id_t queue_id, uint32_t length);
void sdio_slave_set_rx_queue_packet_length(sdio_slave_rx_queue_id_t queue_id, uint32_t length);
void sdio_slave_set_tx_queue_start_address(sdio_slave_tx_queue_id_t queue_id, uint32_t gpd_header);
void sdio_slave_set_rx_queue_start_address(sdio_slave_rx_queue_id_t queue_id, uint32_t gpd_header);
uint8_t sdio_slave_calculate_checksum_by_len(const uint8_t *data, uint32_t check_len);
sdio_slave_status_t sdio_slave_prepare_gpd(sdio_slave_rx_queue_id_t queue_id, uint32_t data_address, uint32_t data_lenght);
sdio_slave_status_t sdio_slave_wait_tx_done(sdio_slave_tx_queue_id_t queue_id);
sdio_slave_status_t sdio_slave_wait_rx_done(sdio_slave_tx_queue_id_t queue_id);
sdio_slave_status_t sdio_slave_send(sdio_slave_rx_queue_id_t queue_id, uint32_t data_address, uint32_t data_length);
sdio_slave_status_t sdio_slave_receive(sdio_slave_tx_queue_id_t queue_id, uint32_t data_address, uint32_t data_length);


#ifdef HAL_SLEEP_MANAGER_ENABLED
void sdio_slave_backup_all(void *data);
void sdio_slave_restore_all(void *data);
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */
#endif /* #ifdef HAL_SDIO_SLAVE_MODULE_ENABLED */
#endif /* #ifndef __HAL_SDIO_SLAVE_INTERNAL_H__ */

