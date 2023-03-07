#include "memory.ld"

/* Additional memory region from project and TFM, this is used in isolation setting */
MEMORY
{
  /* FLASH */

  /* SYSRAM */
  SYSRAM_BL(rwx)    : ORIGIN = 0x80001000, LENGTH = 72K
  SYSRAM_SYS(rwx)   : ORIGIN = 0x8001D000, LENGTH = 908K

  /* PSRAM */
#if defined (MTK_AUDIO_SUPPORT)
  RAM_DSP(rwx)      : ORIGIN = 0xA0058000, LENGTH = 2048K
  RAM_AUDIO(rwx)    : ORIGIN = 0xA0258000, LENGTH = 1024K
#endif

  /* TCM */
  TCM_CACHE(rwx)    : ORIGIN = 0x00100000, LENGTH = 32K
  TCM_BL(rwx)       : ORIGIN = 0x00117F00, LENGTH = 0x100
}

/****************************************************************************
*
* FORWARD DECLARATIONS - SECTION SYMBOLS @ MEMORY BUS ADDRESS
*
****************************************************************************/
/* FLASH */
_xip_bl_addr            = ORIGIN(ROM_BL);
_xip_bl_length          = LENGTH(ROM_BL);
_xip_rbl_addr           = ORIGIN(ROM_RBL);
_xip_rbl_length         = LENGTH(ROM_RBL);
_xip_tfm_int_addr       = ORIGIN(ROM_TFM_INT);
_xip_tfm_int_length     = LENGTH(ROM_TFM_INT);
_xip_tfm_addr           = ORIGIN(ROM_TFM);
_xip_tfm_length         = LENGTH(ROM_TFM);
_xip_rtos_addr          = ORIGIN(ROM_RTOS);
_xip_rtos_length        = LENGTH(ROM_RTOS);
_xip_fota_addr          = ORIGIN(ROM_FOTA);
_xip_fota_length        = LENGTH(ROM_FOTA);
_xip_nvdm_start         = ORIGIN(ROM_NVDM);
_xip_nvdm_length        = LENGTH(ROM_NVDM);
#if defined(TFM_NORMAL_BUILD)
_xip_log_start          = ORIGIN(ROM_LOG);
_xip_log_length         = LENGTH(ROM_LOG);
#endif /* #if defined(TFM_NORMAL_BUILD) */

#if defined (MTK_AUDIO_SUPPORT)
_xip_dsp_start          = ORIGIN(ROM_DSP);
_xip_dsp_length         = LENGTH(ROM_DSP);
#endif

_xip_bt_start           = ORIGIN(ROM_BT);
_xip_bt_length          = LENGTH(ROM_BT);
_xip_wifi_pwrtbl_start  = ORIGIN(ROM_WIFI_PWRTBL);
_xip_wifi_pwrtbl_length = LENGTH(ROM_WIFI_PWRTBL);
_xip_wifi_ext_addr      = ORIGIN(ROM_WIFI_EXT);
_xip_wifi_ext_length    = LENGTH(ROM_WIFI_EXT);
_xip_buffer_bin_addr    = ORIGIN(ROM_BUFFER_BIN);
_xip_buffer_bin_length  = LENGTH(ROM_BUFFER_BIN);

/* SYSRAM */
_sysram_start           = ORIGIN(SYSRAM);
_sysram_length          = LENGTH(SYSRAM);
_sysram_bl_start        = ORIGIN(SYSRAM_BL);
_sysram_bl_length       = LENGTH(SYSRAM_BL);
_sysram_tfm_start       = ORIGIN(SYSRAM_TFM);
_sysram_tfm_length      = LENGTH(SYSRAM_TFM);
_sysram_sys_start       = ORIGIN(SYSRAM_SYS);
_sysram_sys_length      = LENGTH(SYSRAM_SYS);
_vsysram_start          = ORIGIN(VSYSRAM);
_vsysram_length         = LENGTH(VSYSRAM);

/* PSRAM */
#if defined (MTK_AUDIO_SUPPORT)
_ram_bt_fw_start       = ORIGIN(RAM_BTFW);
_ram_bt_fw_length      = LENGTH(RAM_BTFW);
_ram_dsp_start         = ORIGIN(RAM_DSP);
_ram_dsp_length        = LENGTH(RAM_DSP);
_ram_audio_start       = ORIGIN(RAM_AUDIO);
_ram_audio_length      = LENGTH(RAM_AUDIO);
#endif

_ram_start              = ORIGIN(RAM);
_ram_length             = LENGTH(RAM);
_ram_wifi_fw_start      = ORIGIN(RAM_WFFW);
_ram_wifi_fw_length     = LENGTH(RAM_WFFW);
_vram_start             = ORIGIN(VRAM);
_vram_length            = LENGTH(VRAM);


/* TCM */
_tcm_cache_start        = ORIGIN(TCM_CACHE);
_tcm_cache_length       = LENGTH(TCM_CACHE);
_tcm_start              = ORIGIN(TCM);
_tcm_length             = LENGTH(TCM);
_tcm_sec_shm_start      = ORIGIN(TCM_SEC_SHM);
_tcm_sec_shm_length     = LENGTH(TCM_SEC_SHM);
_tcm_tfm_start          = ORIGIN(TCM_TFM);
_tcm_tfm_length         = LENGTH(TCM_TFM);
_tcm_bl_start           = ORIGIN(TCM_BL);
_tcm_bl_length          = LENGTH(TCM_BL);
/****************************************************************************
*
* FORWARD DECLARATIONS - SECTION SYMBOLS @ MEMORY BUS ADDRESS
*
****************************************************************************/
