#ifndef __PLTFM_ISO_H__
#define __PLTFM_ISO_H__

#include "pltfm_iso_type.h"

/**
 * \brief This function registers platform isolation configuration
 *        table to the global context
 *
 * \param[in]  md_cfg      Master domain configuration table
 * \param[in]  IFD_cfg     Device APC configuration table of infra bus masters
 * \param[in]  AUDD_cfg    Device APC configuration table of audio bus masters
 * \param[in]  SMPUD_cfg   ASIC MPU region and APC configuration table
 * \param[in]  ns_addr     VTOR address of NS load
 *
 */
void pltfm_iso_cfg_table_register(const hal_devapc_dom_t *md_cfg, const DEVICE_INFO *IFD_cfg, const DEVICE_INFO *AUDD_cfg, ASIC_MPU_INFO SMPUD_cfg[][REGION_NUM], uint32_t ns_addr);

/**
 * \brief This function set domain of bus master AFE
 */
void AFE_domain_cfg(void);

/**
 * \brief This function set domain of bus master DSP
 */
void DSP_secure_cfg(void);

/**
 * \brief This function set domain of bus master Audio
 */
void Audio_secure_cfg(void);

/**
 * \brief This function set the bus master domains
 */
void master_domain_cfg(void);

/**
 * \brief This function set the bus slave APC
 */
void dapc_cfg(void);

/**
 * \brief This function set the memory region for each
 *        memory type(FLASH, SYSRAM, PSRAM, TCM) to ASIC_MPU
 */
void asic_mpu_region_cfg(void);

/**
 * \brief This function set the APC to the every region by ASIC_MPU
 */
void asic_mpu_apc_cfg(void);

/**
 * \brief This function init DAPC and ASIC_MPU before isolation configurations
 */
void platform_isolation_init(void);

#endif /* __PLTFM_ISO_H__ */
