/**
 * @file
 * @brief Describes the DMA registers
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_DMA_H
#define SX_DMA_H

#include <stdint.h>

/**
* @brief Structure that represent the register map of the DMA module.
*/
typedef struct {
   volatile uint32_t fetch_addr;       /**< 0x00 - Start address of data block */
   volatile uint32_t reserved_0x04;    /**< 0x04 - Reserved */
   volatile uint32_t fetch_len;        /**< 0x08 - Length of data block */
   volatile uint32_t fetch_tag;        /**< 0x0C - User tag */
   volatile uint32_t push_addr;        /**< 0x10 - Start address of data block */
   volatile uint32_t reserved_0x14;    /**< 0x14 - Reserved */
   volatile uint32_t push_len;         /**< 0x18 - Length of data block */
   volatile uint32_t int_en;           /**< 0x1C - Interrupt enable */
   volatile uint32_t int_en_set;       /**< 0x20 - Interrupt enable set */
   volatile uint32_t int_en_clr;       /**< 0x24 - Interrupt enable clear */
   volatile uint32_t int_stat_raw;     /**< 0x28 - Interrupt raw status */
   volatile uint32_t int_stat;         /**< 0x2C - Interrupt status */
   volatile uint32_t int_stat_clr;     /**< 0x30 - Interrupt status clear */
   volatile uint32_t config;           /**< 0x34 - Configuration */
   volatile uint32_t start;            /**< 0x38 - Start fetch & push */
   volatile uint32_t status;           /**< 0x3C - Status */
} dma_sg_regs32_t;

typedef struct {
   volatile uint64_t fetch_addr;       /**< 0x00 - Start address of data block */
   volatile uint32_t fetch_len;        /**< 0x08 - Length of data block */
   volatile uint32_t fetch_tag;        /**< 0x0C - User tag */
   volatile uint64_t push_addr;        /**< 0x10 - Start address of data block */
   volatile uint32_t push_len;         /**< 0x18 - Length of data block */
   volatile uint32_t int_en;           /**< 0x1C - Interrupt enable */
   volatile uint32_t int_en_set;       /**< 0x20 - Interrupt enable set */
   volatile uint32_t int_en_clr;       /**< 0x24 - Interrupt enable clear */
   volatile uint32_t int_stat_raw;     /**< 0x28 - Interrupt raw status */
   volatile uint32_t int_stat;         /**< 0x2C - Interrupt status */
   volatile uint32_t int_stat_clr;     /**< 0x30 - Interrupt status clear */
   volatile uint32_t config;           /**< 0x34 - Configuration */
   volatile uint32_t start;            /**< 0x38 - Start fetch & push */
   volatile uint32_t status;           /**< 0x3C - Status */
} dma_sg_regs64_t;

/**
* @brief Structure that represent a descriptor for the DMA module (in scatter-gather mode).
*/
struct dma_sg_descr_s {
   volatile void * addr;
   volatile struct dma_sg_descr_s * next_descr;
   volatile uint32_t length_irq;
   volatile uint32_t tag;
};

/** @brief value of ::dma_sg_regs_s.config to disable fecther's scatter/gather mode */
#define DMA_AXI_CONFIGREG_FETCHER_DIRECT           0x00000000

/** @brief value of ::dma_sg_regs_s.config to disable pusher's scatter/gather mode */
#define DMA_AXI_CONFIGREG_PUSHER_DIRECT            0x00000000

/** @brief value of ::dma_sg_regs_s.config to enable fecther's scatter/gather mode */
#define DMA_AXI_CONFIGREG_FETCHER_INDIRECT         0x00000001

/** @brief value of ::dma_sg_regs_s.config to enable pusher's scatter/gather mode */
#define DMA_AXI_CONFIGREG_PUSHER_INDIRECT          0x00000002

/** @brief value of ::dma_sg_regs_s.config to stop pusher at the end of the current block  */
#define DMA_AXI_CONFIGREG_STOP_FETCHER             0x00000004

/** @brief value of ::dma_sg_regs_s.config to stop pusher at the end of the current block  */
#define DMA_AXI_CONFIGREG_STOP_PUSHER              0x00000008

/** @brief value of ::dma_sg_regs_s.config to softreset the processing module  */
#define DMA_AXI_CONFIGREG_SOFTRESET                0x00000010


/** @brief value of ::dma_sg_regs_s.config to enable pusher's scatter/gather mode */
#define DMA_AXI_CONFIGREG_INDIRECT                 0x00000002


/** @brief value of ::dma_sg_regs_s.start to start fetcher */
#define DMA_AXI_STARTREG_FETCH                     0x00000001

/** @brief value of ::dma_sg_regs_s.start to start pusher */
#define DMA_AXI_STARTREG_PUSH                      0x00000002


/** @brief ::dma_sg_regs_s.status mask for fetcher busy bit */
#define DMA_AXI_STATUSREG_MASK_FETCHER_BUSY        0x00000001

/** @brief ::dma_sg_regs_s.status mask for pusher busy bit */
#define DMA_AXI_STATUSREG_MASK_PUSHER_BUSY         0x00000002

/** @brief ::dma_sg_regs_s.status mask for input fifo almost full bit */
#define DMA_AXI_STATUSREG_MASK_FIFOIN_AF           0x00000004

/** @brief ::dma_sg_regs_s.status mask for core module busy bit */
#define DMA_AXI_STATUSREG_MASK_CORE_BUSY           0x00000008

/** @brief ::dma_sg_regs_s.status mask for input fifo not empty bit */
#define DMA_AXI_STATUSREG_MASK_FIFOIN_NOT_EMPTY    0x00000010

/** @brief ::dma_sg_regs_s.status mask for pusher waiting FIFO bit */
#define DMA_AXI_STATUSREG_MASK_PUSHER_WAIT         0x00000020

/** @brief ::dma_sg_regs_s.status mask for soft reset bit */
#define DMA_AXI_STATUSREG_MASK_SOFT_RESET          0x00000040

/** @brief ::dma_sg_regs_s.status mask for number of data in output fifo */
#define DMA_AXI_STATUSREG_MASK_FIFOOUT_NDATA       0xFFFF0000

/** @brief ::dma_sg_regs_s.status right shift for number of data in output fifo */
#define DMA_AXI_STATUSREG_LSB_FIFOOUT_NDATA        16


/** @brief ::dma_sg_regs_s.Rawstatus mask for fetcher error bit */
#define DMA_AXI_RAWSTATREG_MASK_FETCHER_ERROR      0x00000004

/** @brief ::dma_sg_regs_s.Rawstatus mask for pusher error bit */
#define DMA_AXI_RAWSTATREG_MASK_PUSHER_ERROR       0x00000020


/** @brief value of ::dma_sg_regs_s.int_en to enable the fetcher triggered at the end of each block (if enabled in the descriptor - scatter-gather only)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_FETCHER_ENDOFBLOCK_EN  0X00000001

/** @brief value of ::dma_sg_regs_s.int_en to enable the fetcher triggered when reaching a block with Stop=1 (or end of direct transfer)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_FETCHER_STOPPED_EN     0X00000002

/** @brief value of ::dma_sg_regs_s.int_en to enable the fetcher triggered when an error response is received from AXI
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_FETCHER_ERROR_EN       0X00000004

/** @brief value of ::dma_sg_regs_s.int_en to enable the pusher triggered at the end of each block (if enabled in the descriptor - scatter-gather only)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_PUSHER_ENDBLOCK_EN     0X00000008

/** @brief value of ::dma_sg_regs_s.int_en to enable the pusher triggered when reaching a block with Stop=1 (or end of direct transfer)
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_PUSHER_STOPPED_EN      0X00000010

/** @brief value of ::dma_sg_regs_s.int_en to enable the pusher triggered when an error response is received from AXI
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_PUSHER_ERROR_EN        0X00000020

/** @brief value of ::dma_sg_regs_s.int_en to enable all the enable lines
    Must be set via ::dma_sg_regs_s.int_en_set and clear via ::dma_sg_regs_s.int_en_clr
*/
#define DMA_AXI_INTENSETREG_ALL_EN        0X0000003F



/** @brief ::dma_axi_descr_t.length_irq mask for data length */
#define DMA_AXI_DESCR_MASK_LENGTH       0x3FFFFFFF

/** @brief ::dma_axi_descr_t.length_irq right shift for data length */
#define DMA_AXI_DESCR_LSB_LENGTH        0


#define DMA_AXI_DESCR_CONST_ADDR       0x10000000
#define DMA_AXI_DESCR_REALIGN          0x20000000
#define DMA_AXI_DESCR_DISCARD          0x40000000
#define DMA_AXI_DESCR_INT_ENABLE       0x80000000
#define DMA_AXI_DESCR_INT_DISABLE      0x00000000

#define DMA_AXI_DESCR_NEXT_STOP        ((struct dma_sg_descr_s*)0x00000001)

#endif /* DMA_SG_CONFIG_H */
