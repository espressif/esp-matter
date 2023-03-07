#include "hal_devapc.h"
#include "pltfm_iso_type.h"
#include "memory_attribute.h"

/*******************************************************************
 * MT7933 supports up to 8 domains can be configurated
 ******************************************************************/
ATTR_RODATA_IN_TCM const hal_devapc_dom_t master_domain[BUS_MASTER_MAX] =
{
#ifdef TFM_ENABLE_PLATFORM_ISOLATION
    [BUS_MASTER_CM33_INFRA_AON] = HAL_DEVAPC_DOMAIN_1,
    [BUS_MASTER_SDIO_SLAVE]     = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_SDIO_MASTER]    = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_SPIM0]          = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_SPIM1]          = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_SPIS]           = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_USB_HOST]       = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_USB_DEV]        = HAL_DEVAPC_DOMAIN_2,
    [BUS_MASTER_CONNAC_WFDMA]   = HAL_DEVAPC_DOMAIN_4,
    [BUS_MASTER_CONNAC_CONN2AP] = HAL_DEVAPC_DOMAIN_3,
    [BUS_MASTER_CPUM_M]         = HAL_DEVAPC_DOMAIN_0,
    [BUS_MASTER_SPI_TEST]       = HAL_DEVAPC_DOMAIN_0,
    [BUS_MASTER_AP_DMA]         = HAL_DEVAPC_DOMAIN_6,
    [BUS_MASTER_CQ_DMA]         = HAL_DEVAPC_DOMAIN_6,
    [BUS_MASTER_GCPU]           = HAL_DEVAPC_DOMAIN_5,
#ifdef MTK_AUDIO_SUPPORT
    [BUS_MASTER_AUDIOSYS_DSP]   = HAL_DEVAPC_DOMAIN_7,
    [BUS_MASTER_AUDIOSYS_AFE]   = HAL_DEVAPC_DOMAIN_7,
#endif /* ifdef MTK_AUDIO_SUPPORT */
#endif /* ifdef TFM_ENABLE_PLATFORM_ISOLATION */
};
