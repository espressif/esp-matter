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

#ifndef __BL_EMAC_H__
#define __BL_EMAC_H__

#include "emac_reg.h"
#include "bl702_common.h"

/**
 *  @brief EMAC half/full-duplex type definition
 */
typedef enum {
    EMAC_MODE_HALFDUPLEX,                 /*!< ETH half-duplex */
    EMAC_MODE_FULLDUPLEX,                 /*!< ETH full-duplex */
}EMAC_Duplex_Type;

/**
 *  @brief EMAC speed type definition
 */
typedef enum {
    EMAC_SPEED_10M,                       /*!< ETH speed 10M */
    EMAC_SPEED_100M,                      /*!< ETH speed 100M */
}EMAC_Speed_Type;

/**
 *  @brief EMAC interrupt type definition
 */
typedef enum {
    EMAC_INT_TX_DONE=0x01,                /*!< Transmit one frame done */
    EMAC_INT_TX_ERROR=0x02,               /*!< Transmit error occur */
    EMAC_INT_RX_DONE=0x04,                /*!< Receive one frame done */
    EMAC_INT_RX_ERROR=0x08,               /*!< Receive error occur */
    EMAC_INT_RX_BUSY=0x10,                /*!< Receive busy due to no free RX buffer Descripter */
    EMAC_INT_TX_CTRL=0x20,                /*!< Transmit control frame */
    EMAC_INT_RX_CTRL=0x40,                /*!< Receive control frame */
    EMAC_INT_ALL = 0x7F,                  /*!<  */
}EMAC_INT_Type;

typedef enum {
    EMAC_INT_TX_DONE_IDX=0x0,                /*!< Transmit one frame done */
    EMAC_INT_TX_ERROR_IDX=0x1,               /*!< Transmit error occur */
    EMAC_INT_RX_DONE_IDX=0x2,                /*!< Receive one frame done */
    EMAC_INT_RX_ERROR_IDX=0x3,               /*!< Receive error occur */
    EMAC_INT_RX_BUSY_IDX=0x4,                /*!< Receive busy due to no free RX buffer Descripter */
    EMAC_INT_TX_CTRL_IDX=0x5,                /*!< Transmit control frame */
    EMAC_INT_RX_CTRL_IDX=0x6,                /*!< Receive control frame */

    EMAC_INT_CNT                             /* EMAC INT source count */
}EMAC_INT_Index;

/**
 *  @brief EMAC configuration type definition
 */
typedef struct {
    BL_Fun_Type recvSmallFrame;             /*!< Receive small frmae or not */
    BL_Fun_Type recvHugeFrame;              /*!< Receive huge frmae(>64K bytes) or not */
    BL_Fun_Type padEnable;                  /*!< Enable padding for frame which is less than MINFL or not */
    BL_Fun_Type crcEnable;                  /*!< Enable hardware CRC or not */
    BL_Fun_Type noPreamble;                 /*!< Enable preamble or not */
    BL_Fun_Type recvBroadCast;              /*!< Receive broadcast frame or not */
    BL_Fun_Type interFrameGapCheck;         /*!< Check inter frame gap or not */
    BL_Fun_Type miiNoPreamble;              /*!< Enable MII interface preamble or not */
    uint8_t miiClkDiv;                      /*!< MII interface clock divider from bus clock */
    uint8_t maxTxRetry;                     /*!< Maximum tx retry count */
    uint16_t interFrameGapValue;            /*!< Inter frame gap vaule in clock cycles(default 24),which equals 9.6 us for 10 Mbps and 0.96 us 
                                                 for 100 Mbps mode */
    uint16_t minFrameLen;                   /*!< Minimum frame length */
    uint16_t maxFrameLen;                   /*!< Maximum frame length */
    uint16_t collisionValid;                /*!< Collision valid value */
    uint8_t macAddr[6];                     /*!< MAC Address */
}EMAC_CFG_Type;

/**
 *  @brief EMAC PHY configuration type definition
 */
typedef struct {
    BL_Fun_Type autoNegotiation;            /*!< Speed and mode auto negotiation */
    EMAC_Duplex_Type duplex;              /*!< Duplex mode */
    EMAC_Speed_Type speed;                /*!< Speed mode */
    uint8_t rsvd;                           /*!< Reserved */
    uint16_t phyAddress;                    /*!< PHY address */
}ETHPHY_CFG_Type;

#define BD_TX_CS_POS                     (0)  /*!< Carrier Sense Lost */
#    define BD_TX_CS_LEN                     (1)
#define BD_TX_DF_POS                     (1)  /*!< Defer Indication */
#    define BD_TX_DF_LEN                     (1)
#define BD_TX_LC_POS                     (2)  /*!< Late Collision */
#    define BD_TX_LC_LEN                     (1)
#define BD_TX_RL_POS                     (3)  /*!< Retransmission Limit */
#    define BD_TX_RL_LEN                     (1)
#define BD_TX_RTRY_POS                   (4)  /*!< Retry Count */
#    define BD_TX_RTRY_LEN                   (4)
#define BD_TX_UR_POS                     (8)  /*!< Underrun */
#    define BD_TX_UR_LEN                     (1)
#define BD_TX_CRC_POS                    (11) /*!< CRC Enable */
#    define BD_TX_CRC_LEN                    (1)
#define BD_TX_PAD_POS                    (12) /*!< PAD enable */
#    define BD_TX_PAD_LEN                    (1)
#define BD_TX_WR_POS                     (13) /*!< Wrap */
#    define BD_TX_WR_LEN                     (1)
#define BD_TX_IRQ_POS                    (14) /*!< Interrupt Request Enable */
#    define BD_TX_IRQ_LEN                    (1)
#define BD_TX_RD_POS                     (15) /*!< The data buffer is ready for transmission or is currently being transmitted. You are not allowed to change it */
#    define BD_TX_RD_LEN                     (1)
#define BD_TX_LEN_POS                    (16) /*!< TX Data buffer length */
#    define BD_TX_LEN_LEN                    (16)

#define BD_RX_LC_POS                     (0)  /*!< Late Collision */
#    define BD_RX_LC_LEN                     (1)
#define BD_RX_CRC_POS                    (1)  /*!< RX CRC Error */
#    define BD_RX_CRC_LEN                    (1)
#define BD_RX_SF_POS                     (2)  /*!< Short Frame */
#    define BD_RX_SF_LEN                     (1)
#define BD_RX_TL_POS                     (3)  /*!< Too Long */
#    define BD_RX_TL_LEN                     (1)
#define BD_RX_DN_POS                     (4)  /*!< Dribble Nibble */
#    define BD_RX_DN_LEN                     (1)
#define BD_RX_RE_POS                     (5)  /*!< Receive Error */
#    define BD_RX_RE_LEN                     (1)
#define BD_RX_OR_POS                     (6)  /*!< Overrun */
#    define BD_RX_OR_LEN                     (1)
#define BD_RX_M_POS                      (7)  /*!< Miss */
#    define BD_RX_M_LEN                      (1)
#define BD_RX_CF_POS                     (8)  /*!< Control Frame Received */
#    define BD_RX_CF_LEN                     (1)
#define BD_RX_WR_POS                     (13) /*!< Wrap */
#    define BD_RX_WR_LEN                     (1)
#define BD_RX_IRQ_POS                    (14) /*!< Interrupt Request Enable */
#    define BD_RX_IRQ_LEN                    (1)
#define BD_RX_E_POS                      (15) /*!< The data buffer is empty (and ready for receiving data) or currently receiving data */
#    define BD_RX_E_LEN                      (1)
#define BD_RX_LEN_POS                    (16) /*!< RX Data buffer length */
#    define BD_RX_LEN_LEN                    (16)

#define EMAC_BD_FIELD_MSK(field)         (((1U << BD_##field##_LEN) - 1) << BD_##field##_POS)
#define EMAC_BD_FIELD_UMSK(field)        (~(((1U << BD_##field##_LEN) - 1) << BD_##field##_POS))

/**
 *  @brief EMAC TX DMA description type definition
 */
/**
 * Note: Always write DWORD1 (buffer addr) first then DWORD0 for racing concern.
 */
typedef struct {
    uint32_t C_S_L;                         /*!< Buffer Descriptors(BD) control,status,length */
    uint32_t Buffer;                        /*!< BD buffer address */
}EMAC_BD_Desc_Type;

/**
 *  @brief EMAC Handle type definition
 */
typedef struct {
    EMAC_BD_Desc_Type *bd;                /*!< Tx descriptor header pointer */
    uint8_t txIndexEMAC;                  /*!< TX index: EMAC */
    uint8_t txIndexCPU;                   /*!< TX index: CPU/SW */
    uint8_t txBuffLimit;                  /*!< TX index max */
    uint8_t rsv0;
    uint8_t rxIndexEMAC;                  /*!< RX index: EMAC */
    uint8_t rxIndexCPU;                   /*!< RX index: CPU/SW */
    uint8_t rxBuffLimit;                  /*!< RX index max */
    uint8_t rsv1;
}EMAC_Handle_Type;

/*@} end of group EMAC_Public_Types */

/** @defgroup  EMAC_Public_Constants
 *  @{
 */

/** @defgroup  EMAC_DUPLEX_TYPE
 *  @{
 */
#define IS_EMAC_DUPLEX_TYPE(type)                      (((type) == EMAC_MODE_HALFDUPLEX) || \
                                                          ((type) == EMAC_MODE_FULLDUPLEX))

/** @defgroup  EMAC_SPEED_TYPE
 *  @{
 */
#define IS_EMAC_SPEED_TYPE(type)                       (((type) == EMAC_SPEED_10M) || \
                                                          ((type) == EMAC_SPEED_100M))

/** @defgroup  EMAC_INT_TYPE
 *  @{
 */
#define IS_EMAC_INT_TYPE(type)                         (((type) == EMAC_INT_TX_DONE) || \
                                                          ((type) == EMAC_INT_TX_ERROR) || \
                                                          ((type) == EMAC_INT_RX_DONE) || \
                                                          ((type) == EMAC_INT_RX_ERROR) || \
                                                          ((type) == EMAC_INT_RX_BUSY) || \
                                                          ((type) == EMAC_INT_TX_CTRL) || \
                                                          ((type) == EMAC_INT_RX_CTRL) || \
                                                          ((type) == EMAC_INT_ALL))

/*@} end of group EMAC_Public_Constants */

/** @defgroup  EMAC_Public_Macros
 *  @{
 */
/* DMA Descriptor offset */
#define EMAC_DMA_DESC_OFFSET                          0x400
/* ETH packet size */
#define ETH_MAX_PACKET_SIZE                             ((uint32_t)1524U)  /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER_SZIE                                 ((uint32_t)14U)    /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC_SIZE                                    ((uint32_t)4U)     /*!< Ethernet CRC */
#define ETH_EXTRA_SIZE                                  ((uint32_t)2U)     /*!< Extra bytes in some cases */
#define ETH_VLAN_TAG_SIZE                               ((uint32_t)4U)     /*!< optional 802.1q VLAN Tag */
#define ETH_MIN_ETH_PAYLOAD_SIZE                        ((uint32_t)46U)    /*!< Minimum Ethernet payload size */
#define ETH_MAX_ETH_PAYLOAD_SIZE                        ((uint32_t)1500U)  /*!< Maximum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD_SIZE                    ((uint32_t)9000U)  /*!< Jumbo frame payload size */
/* DP83848 PHY Address*/
#define EMAC_PHY_ADDRESS                              0x00U
#define PHY_LINK_TO                                     ((uint32_t)0x00000FFFU)
#define PHY_AUTONEGO_COMPLETED_TO                       ((uint32_t)0x00000FFFU)
/* Section 3: Common PHY Registers */
#define PHY_BCR                                         ((uint16_t)0x00U)    /*!< Transceiver Basic Control Register   */
#define PHY_BSR                                         ((uint16_t)0x01U)    /*!< Transceiver Basic Status Register    */
#    define PHY_BSR_100BASETXFULL                       (1 << 14)
#    define PHY_BSR_100BASETXHALF                       (1 << 13)
#    define PHY_BSR_10BASETXFULL                        (1 << 12)
#    define PHY_BSR_10BASETXHALF                        (1 << 11)
#define PHY_RESET                                       ((uint16_t)0x8000U)  /*!< PHY Reset */
#define PHY_LOOPBACK                                    ((uint16_t)0x4000U)  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M                             ((uint16_t)0x2100U)  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M                             ((uint16_t)0x2000U)  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M                              ((uint16_t)0x0100U)  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M                              ((uint16_t)0x0000U)  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION                             ((uint16_t)0x1000U)  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION                     ((uint16_t)0x0200U)  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                                   ((uint16_t)0x0800U)  /*!< Select the power down mode           */
#define PHY_ISOLATE                                     ((uint16_t)0x0400U)  /*!< Isolate PHY from MII                 */
#define PHY_AUTONEGO_COMPLETE                           ((uint16_t)0x0020U)  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS                               ((uint16_t)0x0004U)  /*!< Valid link established               */
#define PHY_JABBER_DETECTION                            ((uint16_t)0x0002U)  /*!< Jabber condition detected            */

#define PHY_PHYID1                                      ((uint16_t)0x02U)    /*!< PHY ID 1    */
#define PHY_PHYID2                                      ((uint16_t)0x03U)    /*!< PHY ID 2    */
#define PHY_ADVERTISE                                   ((uint16_t)0x04U)    /*!< Auto-negotiation advertisement       */
#    define PHY_ADVERTISE_100BASETXFULL                 (1 << 8)
#    define PHY_ADVERTISE_100BASETXHALF                 (1 << 7)
#    define PHY_ADVERTISE_10BASETXFULL                  (1 << 6)
#    define PHY_ADVERTISE_10BASETXHALF                  (1 << 5)
#    define PHY_ADVERTISE_8023                          (1 << 0)
#define PHY_LPA                                         ((uint16_t)0x05U)    /*!< Auto-negotiation link partner base page ability    */
#define PHY_EXPANSION                                   ((uint16_t)0x06U)    /*!< Auto-negotiation expansion           */
/* Section 4: Extended PHY Registers */
#define PHY_SR                                          ((uint16_t)0x1FU)    /*!< PHY special control/ status register Offset     */
#    define PHY_SR_SPEED_OFFSET                         (2)
#    define PHY_SR_SPEED_MASK                           (0x7 << PHY_SR_SPEED_OFFSET)
#    define PHY_SR_SPEED_10BASETXHALF                   (0x1 << PHY_SR_SPEED_OFFSET)
#    define PHY_SR_SPEED_10BASETXFULL                   (0x5 << PHY_SR_SPEED_OFFSET)
#    define PHY_SR_SPEED_100BASETXHALF                  (0x2 << PHY_SR_SPEED_OFFSET)
#    define PHY_SR_SPEED_100BASETXFULL                  (0x6 << PHY_SR_SPEED_OFFSET)
#    define PHY_SR_SPEED_MODE_COMPARE(status, mode)     (!!(mode == (status & PHY_SR_SPEED_MASK)))
#define PHY_SPEED_STATUS                                ((uint16_t)0x0004U)  /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS                               ((uint16_t)0x0010U)  /*!< PHY Duplex mask                                 */
#define PHY_ISFR                                        ((uint16_t)0x1DU)    /*!< PHY Interrupt Source Flag register Offset       */
#define PHY_ISFR_INT4                                   ((uint16_t)0x0010U)  /*!< PHY Link down inturrupt                         */

/*@} end of group EMAC_Public_Macros */

/** @defgroup  EMAC_Public_Functions
 *  @{
 */

int emac_init(EMAC_CFG_Type *cfg);
int emac_phyinit(ETHPHY_CFG_Type *cfg);
int emac_deinit(void);
int emac_dmadesclistinit(EMAC_Handle_Type *handle,uint8_t *txBuff, uint32_t txBuffCount,
                                   uint8_t *rxBuff, uint32_t rxBuffCount);
int emac_dmatxdescget(EMAC_Handle_Type *handle,EMAC_BD_Desc_Type **txDMADesc);
int emac_starttx(EMAC_Handle_Type *handle,EMAC_BD_Desc_Type *txDMADesc, uint32_t len);
int emac_enable(void);
int emac_disable(void);
int emac_intmask(EMAC_INT_Type intType,BL_Mask_Type intMask);
int emac_getintstatus(EMAC_INT_Type intType);
int emac_clrintstatus(EMAC_INT_Type intType);
int EMAC_Int_Callback_Install(EMAC_INT_Index intIdx, intCallback_Type* cbFun);
int emac_txpausereq(uint16_t timeCount);
int emac_sethash(uint32_t hash0,uint32_t hash1);
int emac_phy_read(uint16_t phyReg, uint16_t *regValue);
int emac_phy_write(uint16_t phyReg, uint16_t regValue);

int emac_enable_tx(void);
int emac_disable_tx(void);

#endif /* __BL702_EMAC_H__ */
