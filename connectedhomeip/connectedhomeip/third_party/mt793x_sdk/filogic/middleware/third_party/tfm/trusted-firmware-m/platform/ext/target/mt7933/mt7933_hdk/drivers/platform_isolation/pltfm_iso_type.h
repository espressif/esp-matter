#ifndef __PLTFM_ISO_TYPE_H__
#define __PLTFM_ISO_TYPE_H__

#include "hal_devapc.h"
#include "hal_asic_mpu.h"

#define ENABLE 1
#define DISABLE 0
#define TMP_DATA_SECTION_START_ADDR 0xFFFFFFFF

typedef enum {
    BUS_MASTER_CM33_INFRA_AON = 0,
    BUS_MASTER_SDIO_SLAVE,
    BUS_MASTER_SDIO_MASTER,
    BUS_MASTER_SPIM0,
    BUS_MASTER_SPIM1,
    BUS_MASTER_SPIS,
    BUS_MASTER_USB_HOST,
    BUS_MASTER_USB_DEV,
    BUS_MASTER_CONNAC_WFDMA,
    BUS_MASTER_CONNAC_CONN2AP,
    BUS_MASTER_CPUM_M,
    BUS_MASTER_SPI_TEST,
    BUS_MASTER_AP_DMA,
    BUS_MASTER_CQ_DMA,
    BUS_MASTER_GCPU,
    BUS_MASTER_AUDIOSYS_DSP,
    BUS_MASTER_AUDIOSYS_AFE,
    BUS_MASTER_MAX,
} bus_master;

typedef struct {
    unsigned int        addr;
    unsigned int        vio_idx;
    hal_devapc_apc_t    perm[8];
} DEVICE_INFO;

typedef struct {
	hal_asic_mpu_type_t	mem_type;
    unsigned int        addr;
    unsigned int        enable;
    hal_devapc_apc_t    perm[8];
} ASIC_MPU_INFO;

#endif /* __PLTFM_ISO_TYPE_H__ */
