//disable all asic_mpu protection
#ifndef TFM_ENABLE_PLATFORM_ISOLATION
void asic_mpu_disable_all_protect(void)
{
    for (uint32_t region = 0; region < REGION_NUM; region++) {
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_FLASH, region, -1, 0);
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_SYSRAM, region, -1, 0);
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_PSRAM, region, -1, 0);
        hal_asic_mpu_set_region_apc(HAL_ASIC_MPU_TYPE_TCM, region, -1, 0);
    }
}
#endif /* #ifndef TFM_ENABLE_PLATFORM_ISOLATION */
