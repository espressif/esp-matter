#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definations to compile the "lib_mtk_drv" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#   TFM_ROOT_DIR - root directory of the TF-M repository.
#Outputs:
#   Will modify include directories to make the source compile.
#   ALL_SRC_C_S: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   ALL_SRC_CXX_S: C++ source files to be compiled will be added to this list. This shaall be added to your add_executable or add_library command.
#   ALL_SRC_ASM_S: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   Include directories will be modified by using the include_directories() commands as needed.

set (LIB_MTK_DRV_C_DEF
    MTK_TFM_ENABLE
    TRUSTED_ZONE
    TFM_DEEPSLEEP_WAKEUP
    HAL_SLEEP_MANAGER_ENABLED
    HAL_PSRAM_MODULE_ENABLED
    HAL_GPIO_MODULE_ENABLED
    HAL_GPT_MODULE_ENABLED
    HAL_CACHE_MODULE_ENABLED
    HAL_WDT_MODULE_ENABLED
    HAL_TOP_THERMAL_MODULE_ENABLED
    HAL_SECURITY_MODULE_ENABLE
    MTK_HAL_NO_LOG_ENABLE
    HAL_GDMA_MODULE_ENABLED
    HAL_GPT_MODULE_ENABLED
    HAL_SEJ_GPT_MODULE_ENABLED
    HAL_AES_MODULE_ENABLED
    HAL_TRNG_MODULE_ENABLED
    HAL_NVIC_MODULE_ENABLED
    HAL_FLASH_MODULE_ENABLED
    HAL_CLOCK_MODULE_ENABLED
    HAL_NVIC_IRQ_SET_TYPE_FEATURE
    HAL_DEVAPC_MODULE_ENABLED
    )

set (LIB_MTK_DRV_C_SRC
    "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/tfm_hal_cache.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/hal_nvic.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/hal_trng.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/hal_trng_internal.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/hal_psram.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/psramc/non_uhs_psram.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/psramc/psramc_pi_main.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/psramc/Hal_io.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/psramc/pemi.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/psramc/psramc_pi_basic_api.c"
    "${HADRON_ROOT_DIR}/driver/chip/mt7933/src/psramc/psramc_pi_calibration_api.c"
    )

if (EXISTS "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core")
    list(APPEND LIB_MTK_DRV_C_SRC
        "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core/src/hal_spm.c"
        "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core/src/hal_sleep_manager.c"
        "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core/src/hal_sleep_manager_internal.c"
        "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core/src/hal_sleep_manager_platform.c"
        "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core/src/tfm/pltfm_iso.c"
        "${HADRON_ROOT_DIR}/driver/chip/mt7933/src_core/src/hal_security_api.c"
    )
endif()
