/********************************************************************************************************
 * @file	ext_rf.h
 *
 * @brief	This is the header file for B91
 *
 * @author	BLE Group
 * @date	2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#ifndef DRIVERS_B91_DRIVER_EXT_EXT_RF_H_
#define DRIVERS_B91_DRIVER_EXT_EXT_RF_H_

#include "compiler.h"
#include "types.h"

#define DMA_RFRX_LEN_HW_INFO 0   // 826x: 8
#define DMA_RFRX_OFFSET_HEADER 4 // 826x: 12
#define DMA_RFRX_OFFSET_RFLEN 5  // 826x: 13
#define DMA_RFRX_OFFSET_DATA 6   // 826x: 14

#define RF_TX_PAKET_DMA_LEN(rf_data_len) (((rf_data_len) + 3) / 4) | (((rf_data_len) % 4) << 22)
#define DMA_RFRX_OFFSET_CRC24(p) (p[DMA_RFRX_OFFSET_RFLEN] + 6)        // data len:3
#define DMA_RFRX_OFFSET_TIME_STAMP(p) (p[DMA_RFRX_OFFSET_RFLEN] + 9)   // data len:4
#define DMA_RFRX_OFFSET_FREQ_OFFSET(p) (p[DMA_RFRX_OFFSET_RFLEN] + 13) // data len:2
#define DMA_RFRX_OFFSET_RSSI(p) (p[DMA_RFRX_OFFSET_RFLEN] + 15)        // data len:1, signed

#define RF_BLE_RF_PAYLOAD_LENGTH_OK(p) (p[5] <= reg_rf_rxtmaxlen)
#define RF_BLE_RF_PACKET_CRC_OK(p) ((p[p[5] + 5 + 11] & 0x01) == 0x0)
#define RF_BLE_PACKET_VALIDITY_CHECK(p) (RF_BLE_RF_PAYLOAD_LENGTH_OK(p) && RF_BLE_RF_PACKET_CRC_OK(p))

typedef enum
{
    RF_ACC_CODE_TRIGGER_AUTO = BIT(0), /**< auto trigger */
    RF_ACC_CODE_TRIGGER_MANU = BIT(1), /**< manual trigger */
} rf_acc_trigger_mode;

#ifdef __cplusplus
extern "C" {
#endif

void ble_rf_set_rx_dma(unsigned char * buff, unsigned char fifo_byte_size) __attribute__((section(".ram_code")))
__attribute__((noinline));

void ble_rf_set_tx_dma(unsigned char fifo_dep, unsigned char fifo_byte_size) __attribute__((section(".ram_code")))
__attribute__((noinline));

void ble_tx_dma_config(void) __attribute__((section(".ram_code"))) __attribute__((noinline));

void ble_rx_dma_config(void) __attribute__((section(".ram_code"))) __attribute__((noinline));

void rf_drv_ble_init(void);

/**
 * @brief   This function serves to settle adjust for RF Tx.This function for adjust the differ time
 * 			when rx_dly enable.
 * @param   txstl_us - adjust TX settle time.
 * @return  none
 */
static inline void rf_tx_settle_adjust(unsigned short txstl_us)
{
    REG_ADDR16(0x80140a04) = txstl_us;
}

/**
 *	@brief     This function serves to reset RF BaseBand
 *	@param[in] none.
 *	@return	   none.
 */
static inline void rf_reset_baseband(void)
{
    REG_ADDR8(0x801404e3) = 0;      // rf_reset_baseband,rf reg need re-setting
    REG_ADDR8(0x801404e3) = BIT(0); // release reset signal
}

#define reset_baseband rf_reset_baseband

/**
 * @brief   This function serves to set RF access code value.
 * @param[in]   ac - the address value.
 * @return  none
 */
static inline void rf_set_ble_access_code_value(unsigned int ac)
{
    write_reg32(0x80140808, ac);
}

/**
 * @brief   This function serves to set RF access code.
 * @param[in]   p - the address to access.
 * @return  none
 */
static inline void rf_set_ble_access_code(unsigned char * p)
{
    write_reg32(0x80140808, p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24));
}

/**
 * @brief   This function serves to reset function for RF.
 * @param   none
 * @return  none
 *******************need driver change
 */
static inline void reset_sn_nesn(void)
{
    REG_ADDR8(0x80140a01) = 0x01;
}

/**
 * @brief   This function serves to set RF access code advantage.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_ble_access_code_adv(void)
{
    write_reg32(0x0140808, 0xd6be898e);
}


/**
 * @brief   This function serves to triggle accesscode in coded Phy mode.
 * @param   none.
 * @return  none.
 */
static inline void rf_trigle_codedPhy_accesscode(void)
{
    write_reg8(0x140c25, read_reg8(0x140c25) | 0x01);
}

/**
 * @brief     This function performs to enable RF Tx.
 * @param[in] none.
 * @return    none.
 */
static inline void rf_ble_tx_on()
{
    write_reg8(0x80140a02, 0x45 | BIT(4)); // TX enable
}

/**
 * @brief     This function performs to done RF Tx.
 * @param[in] none.
 * @return    none.
 */
static inline void rf_ble_tx_done()
{
    write_reg8(0x80140a02, 0x45);
}

#define rf_set_ble_channel rf_set_ble_chn

/**
 * @brief     This function performs to switch PHY test mode.
 * @param[in] mode - PHY mode
 * @return    none.
 */
void rf_switchPhyTestMode(rf_mode_e mode);

/**
 * @brief This function reset dma registrs to default (reset values)
 * @param none
 * @return none
 */
void rf_reset_dma(void);

/**
 * @brief This function reset radio registrs to default (reset values)
 * @param none
 * @return none
 */
void rf_radio_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_B91_DRIVER_EXT_EXT_RF_H_ */
