/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_WFX_REGISTERS_H
#define SL_WFX_REGISTERS_H

#include <stdint.h>

#define SYS_BASE_ADDR_SILICON           (0)
#define PAC_BASE_ADDRESS_SILICON        (SYS_BASE_ADDR_SILICON + 0x09000000)
#define PAC_SHARED_MEMORY_SILICON       (PAC_BASE_ADDRESS_SILICON)

#define SL_WFX_APB(addr)                (PAC_SHARED_MEMORY_SILICON + (addr))

/* Download control area */
#define DOWNLOAD_BOOT_LOADER_OFFSET     (0x00000000)
#define DOWNLOAD_FIFO_SIZE              (0x00008000)

#define DOWNLOAD_CTRL_DATA_DWORDS       (32 - 6)

#define DOWNLOAD_CTRL_OFFSET            (0x0900C000)
#define DOWNLOAD_IMAGE_SIZE_REG         (DOWNLOAD_CTRL_OFFSET + 0)
#define DOWNLOAD_PUT_REG                (DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, put))
#define DOWNLOAD_TRACE_PC_REG           (DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, trace_pc))
#define DOWNLOAD_GET_REG                (DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, get))
#define DOWNLOAD_STATUS_REG             (DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, status))
#define DOWNLOAD_DEBUG_DATA_REG         (DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, debug_data))
#define DOWNLOAD_DEBUG_DATA_LEN         (108)

#define DOWNLOAD_BLOCK_SIZE             (1024)

#define ADDR_DWL_CTRL_AREA              0x0900C000
#define FW_KEYSET_SIZE                  8
#define FW_SIGNATURE_SIZE               64
#define FW_HASH_SIZE                    8
#define ADDR_DWL_CTRL_AREA_IMAGE_SIZE   (ADDR_DWL_CTRL_AREA + 0)
#define ADDR_DWL_CTRL_AREA_PUT          (ADDR_DWL_CTRL_AREA + 4)
#define ADDR_DWL_CTRL_AREA_GET          (ADDR_DWL_CTRL_AREA + 8)
#define ADDR_DWL_CTRL_AREA_HOST_STATUS  (ADDR_DWL_CTRL_AREA + 12)
#define ADDR_DWL_CTRL_AREA_NCP_STATUS   (ADDR_DWL_CTRL_AREA + 16)
#define ADDR_DWL_CTRL_AREA_SIGNATURE    (ADDR_DWL_CTRL_AREA + 20)
#define ADDR_DWL_CTRL_AREA_FW_HASH      (ADDR_DWL_CTRL_AREA_SIGNATURE + FW_SIGNATURE_SIZE)
#define ADDR_DWL_CTRL_AREA_FW_VERSION   (ADDR_DWL_CTRL_AREA_FW_HASH + FW_HASH_SIZE)

#define HOST_STATE_UNDEF                0xFFFFFFFF
#define HOST_STATE_NOT_READY            0x12345678
#define HOST_STATE_READY                0x87654321
#define HOST_STATE_HOST_INFO_READ       0xA753BD99
#define HOST_STATE_UPLOAD_PENDING       0xABCDDCBA
#define HOST_STATE_UPLOAD_COMPLETE      0xD4C64A99
#define HOST_STATE_OK_TO_JUMP           0x174FC882

#define NCP_STATE_UNDEF                 0xFFFFFFFF
#define NCP_STATE_NOT_READY             0x12345678
#define NCP_STATE_INFO_READY            0xBD53EF99
#define NCP_STATE_READY                 0x87654321
#define NCP_STATE_DOWNLOAD_PENDING      0xABCDDCBA
#define NCP_STATE_DOWNLOAD_COMPLETE     0xCAFEFECA
#define NCP_STATE_AUTH_OK               0xD4C64A99
#define NCP_STATE_AUTH_FAIL             0x174FC882
#define NCP_STATE_PUB_KEY_RDY           0x7AB41D19

#define ADDR_DOWNLOAD_FIFO_BASE         0x09004000
#define ADDR_DOWNLOAD_FIFO_END          0x0900C000
#define ADDR_SHARED_RAM_DEBUG_AREA      0x09002000

#define BIT(n) (1 << (n))

/* Control register bit set */
#define SL_WFX_CONT_REGISTER_SIZE      (0x02)
#define SL_WFX_CONT_NEXT_LEN_MASK      (0x0FFF)
#define SL_WFX_CONT_WUP_BIT            (BIT(12))
#define SL_WFX_CONT_RDY_BIT            (BIT(13))
#define SL_WFX_CONT_FRAME_TYPE_INFO    (BIT(14) | BIT(15))
#define SL_WFX_CONT_FRAME_TYPE_OFFSET  (0x0E)

/* Config register bit set */
#define SL_WFX_CONFIG_ERROR_CSN_FRAME     (BIT(0))
#define SL_WFX_CONFIG_ERROR_READ_UNDERRUN (BIT(1))
#define SL_WFX_CONFIG_ERROR_READ_LESS     (BIT(2))
#define SL_WFX_CONFIG_FRAME_READ_ENTRY    (BIT(3))
#define SL_WFX_CONFIG_ERROR_SEND_OVERRUN  (BIT(4))
#define SL_WFX_CONFIG_ERROR_SEND_LARGE    (BIT(5))
#define SL_WFX_CONFIG_ERROR_SEND_ENTRY    (BIT(6))
#define SL_WFX_CONFIG_CSN_FRAME_BIT       (BIT(7))
#define SL_WFX_CONFIG_ERROR_MASK          (0x000F)
/* Word mode config */
#define SL_WFX_CONFIG_WORD_MODE_BITS   (BIT(8) | BIT(9))
#define SL_WFX_CONFIG_WORD_MODE_1      (BIT(8))
#define SL_WFX_CONFIG_WORD_MODE_2      (BIT(9))
/* QueueM */
#define SL_WFX_CONFIG_ACCESS_MODE_BIT  (BIT(10))
/* AHB bus */
#define SL_WFX_CONFIG_AHB_PRFETCH_BIT  (BIT(11))
#define SL_WFX_CONFIG_CPU_CLK_DIS_BIT  (BIT(12))
/* APB bus */
#define SL_WFX_CONFIG_PRFETCH_BIT      (BIT(13))
/* CPU reset */
#define SL_WFX_CONFIG_CPU_RESET_BIT    (BIT(14))
#define SL_WFX_CONFIG_CLEAR_INT_BIT    (BIT(15))
/* For WF200 the IRQ Enable and Ready Bits are in CONFIG register */
#define SL_WFX_CONFIG_DATA_IRQ_ENABLE  (BIT(16))
#define SL_WFX_CONFIG_WUP_IRQ_ENABLE   (BIT(17))
#define SL_WFX_CONFIG_DATA_WUP_ENABLE  (BIT(16) | BIT(17))
/* Revision and type */
#define SL_WFX_CONFIG_REVISION_OFFSET  (0x18)
#define SL_WFX_CONFIG_REVISION_MASK    (0x7)
#define SL_WFX_CONFIG_TYPE_OFFSET      (0x1F)
#define SL_WFX_CONFIG_TYPE_MASK        (0x1)

/* SDIO CCCR register offsets */
#define SL_WFX_SDIO_CCCR_IO_QUEUE_ENABLE        (0x02)
#define SL_WFX_SDIO_CCCR_IRQ_ENABLE             (0x04)
#define SL_WFX_SDIO_CCCR_BUS_INTERFACE_CONTROL  (0x07)
#define SL_WFX_SDIO_CCCR_HIGH_SPEED_ENABLE      (0x13)

/* SDIO FBR1 register offsets */
#define SL_WFX_SDIO_FBR1_BLOCK_SIZE_LSB (0x110)   /* Function 1 16-bit block size LSB */
#define SL_WFX_SDIO_FBR1_BLOCK_SIZE_MSB (0x111)   /* Function 1 16-bit block size MSB */

#define FW_VERSION_VALUE               0x00000001

#endif // SL_WFX_REGISTERS_H
