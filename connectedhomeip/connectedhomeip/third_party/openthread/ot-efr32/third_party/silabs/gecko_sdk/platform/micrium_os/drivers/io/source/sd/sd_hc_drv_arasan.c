/***************************************************************************//**
 * @file
 * @brief IO - Sd Ctrlr Driver - Arasan
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_SD_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_math.h>
#include  <common/include/toolchains.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_opt_def.h>

#include  <drivers/io/include/sd_drv.h>
#include  <io/include/sd.h>
#include  <io/include/sd_card.h>
#include  <io/source/sd/sd_card_ctrlr_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (IO)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_IO

/********************************************************************************************************
 *                                       SDHC CLOCK CFG DEFINES
 *******************************************************************************************************/

#define  SDHC_PRESCALER_AMOUNT                      9u
#define  SDHC_DIVIDER_AMOUNT                        16u

/********************************************************************************************************
 *                                           SDHC REG DEFINES
 *******************************************************************************************************/

#define  SDHC_BLKSIZE_BLK_SIZE_MSK                           DEF_BIT_FIELD(12u, 0u)
#define  SDHC_BLKSIZE_BUF_BOUND_MSK                         (DEF_BIT_12 | DEF_BIT_13 | DEF_BIT_14)
#define  SDHC_BLKSIZE_BUF_BOUND_4K                           0u
#define  SDHC_BLKSIZE_BUF_BOUND_8K                           1u
#define  SDHC_BLKSIZE_BUF_BOUND_16K                          2u
#define  SDHC_BLKSIZE_BUF_BOUND_32K                          3u
#define  SDHC_BLKSIZE_BUF_BOUND_64K                          4u
#define  SDHC_BLKSIZE_BUF_BOUND_128K                         5u
#define  SDHC_BLKSIZE_BUF_BOUND_256K                         6u
#define  SDHC_BLKSIZE_BUF_BOUND_512K                         7u

#define  SDHC_XFERMODE_DMA_EN                                DEF_BIT_00
#define  SDHC_XFERMODE_BLK_CNT_EN                            DEF_BIT_01
#define  SDHC_XFERMODE_AUTO_CMD_DIS                          DEF_BIT_NONE
#define  SDHC_XFERMODE_AUTO_CMD_CMD_12_EN                    DEF_BIT_02
#define  SDHC_XFERMODE_AUTO_CMD_CMD_23_EN                    DEF_BIT_03
#define  SDHC_XFERMODE_XFER_DIR_RD                           DEF_BIT_04
#define  SDHC_XFERMODE_MULT_BLK_SEL                          DEF_BIT_05

#define  SDHC_CMD_IX_MSK                                     DEF_BIT_FIELD(6u, 8u)
#define  SDHC_CMD_CMDTYPE_MSK                                DEF_BIT_FIELD(2u, 6u)
#define  SDHC_CMD_CMDTYPE_NORMAL                             0
#define  SDHC_CMD_CMDTYPE_SUSPEND                            1
#define  SDHC_CMD_CMDTYPE_ABORT                              3
#define  SDHC_CMD_CMDTYPE_RESUME                             2
#define  SDHC_CMD_DPSEL                                      DEF_BIT_05
#define  SDHC_CMD_CMDIXCHKEN                                 DEF_BIT_04
#define  SDHC_CMD_CMDCRCCHKEN                                DEF_BIT_03
#define  SDHC_CMD_RSPTYPESEL_MSK                             DEF_BIT_FIELD(2u, 0u)
#define  SDHC_CMD_RSPTYPESEL_NONE                            0
#define  SDHC_CMD_RSPTYPESEL_RESPLEN136                      1
#define  SDHC_CMD_RSPTYPESEL_RESPLEN48                       2
#define  SDHC_CMD_RSPTYPESEL_RESPLEN48_BUSY                  3

#define  SDHC_PRESSTATE_CMD_LINE_SIG_LVL                     DEF_BIT_24
#define  SDHC_PRESSTATE_DAT_LINES_SIG_LVL                    DEF_BIT_FIELD(4u, 20u)
#define  SDHC_PRESSTATE_WRPROT                               DEF_BIT_19
#define  SDHC_PRESSTATE_CARDDETLVL                           DEF_BIT_18
#define  SDHC_PRESSTATE_CARDINS                              DEF_BIT_16
#define  SDHC_PRESSTATE_BUFRDEN                              DEF_BIT_11
#define  SDHC_PRESSTATE_BUFWREN                              DEF_BIT_10
#define  SDHC_PRESSTATE_RDXFERACT                            DEF_BIT_09
#define  SDHC_PRESSTATE_WRXFERACT                            DEF_BIT_08
#define  SDHC_PRESSTATE_RETUNE_REQ                           DEF_BIT_03
#define  SDHC_PRESSTATE_DATALINEACT                          DEF_BIT_02
#define  SDHC_PRESSTATE_DATAINHIBIT                          DEF_BIT_01
#define  SDHC_PRESSTATE_CMDINHIBIT                           DEF_BIT_00

#define  SDHC_HOSTCTRL1_CARDDET_TEST_SIG_SEL                 DEF_BIT_07
#define  SDHC_HOSTCTRL1_CARDDET_TEST_LVL                     DEF_BIT_06
#define  SDHC_HOSTCTRL1_EXT_DATA_XFER_8B                     DEF_BIT_05
#define  SDHC_HOSTCTRL1_DMA_ADMA2_32B                        DEF_BIT_04
#define  SDHC_HOSTCTRL1_DMA_SDMA                             DEF_BIT_NONE
#define  SDHC_HOSTCTRL1_HS_EN                                DEF_BIT_02
#define  SDHC_HOSTCTRL1_DATAXFERWIDTH_4BIT                   DEF_BIT_01
#define  SDHC_HOSTCTRL1_LED_ON                               DEF_BIT_00

#define  SDHC_PWRCTRL_SD_BUS_VOLT_MSK                       (DEF_BIT_01 | DEF_BIT_02 | DEF_BIT_03)
#define  SDHC_PWRCTRL_SD_BUS_3_3V                           (DEF_BIT_01 | DEF_BIT_02 | DEF_BIT_03)
#define  SDHC_PWRCTRL_SD_BUS_3V                             (DEF_BIT_02 | DEF_BIT_03)
#define  SDHC_PWRCTRL_SD_BUS_1_8V                           (DEF_BIT_01 | DEF_BIT_03)
#define  SDHC_PWRCTRL_SD_BUS_PWR_ON                          DEF_BIT_00

#define  SDHC_BLKGAPCTRL_INTBLKGAPEN                         DEF_BIT_03
#define  SDHC_BLKGAPCTRL_RDWAITCTRLEN                        DEF_BIT_02
#define  SDHC_BLKGAPCTRL_CONTINUEREQ                         DEF_BIT_01
#define  SDHC_BLKGAPCTRL_STOPBLKGAPREQ                       DEF_BIT_00

#define  SDHC_WAKEUPCTRL_CARDREMWAKE                         DEF_BIT_02
#define  SDHC_WAKEUPCTRL_CARDINSWAKE                         DEF_BIT_01
#define  SDHC_WAKEUPCTRL_INTWAKE                             DEF_BIT_00

#define  SDHC_CLKCTRL_SDCLK_FREQ_SEL                         DEF_BIT_FIELD(8u, 8u)
#define  SDHC_CLKCTRL_SDCLK_FREQ_SEL_HIGH                   (DEF_BIT_06 | DEF_BIT_07)
#define  SDHC_CLKCTRL_CLK_GEN_SEL_PROG                       DEF_BIT_05
#define  SDHC_CLKCTRL_SD_CLK_EN                              DEF_BIT_02
#define  SDHC_CLKCTRL_INT_CLK_STBL                           DEF_BIT_01
#define  SDHC_CLKCTRL_INT_CLK_EN                             DEF_BIT_00

#define  SDHC_TIMEOUTCTRL_DATATIMEOUTCTR_MSK                 DEF_BIT_FIELD(4u, 0u)

#define  SDHC_SOFTRST_SOFTRSTDATA                            DEF_BIT_02
#define  SDHC_SOFTRST_SOFTRSTCMD                             DEF_BIT_01
#define  SDHC_SOFTRST_SOFTRSTALL                             DEF_BIT_00

#define  SDHC_INTSTAT_ERR_INT                                DEF_BIT_15
#define  SDHC_INTSTAT_RETUNE                                 DEF_BIT_12
#define  SDHC_INTSTAT_INT_C                                  DEF_BIT_11
#define  SDHC_INTSTAT_INT_B                                  DEF_BIT_10
#define  SDHC_INTSTAT_INT_A                                  DEF_BIT_09
#define  SDHC_INTSTAT_CARDINT                                DEF_BIT_08
#define  SDHC_INTSTAT_CARDREM                                DEF_BIT_07
#define  SDHC_INTSTAT_CARDINS                                DEF_BIT_06
#define  SDHC_INTSTAT_BUFRDRDY                               DEF_BIT_05
#define  SDHC_INTSTAT_BUFWRRDY                               DEF_BIT_04
#define  SDHC_INTSTAT_DMA                                    DEF_BIT_03
#define  SDHC_INTSTAT_BLKGAP                                 DEF_BIT_02
#define  SDHC_INTSTAT_XFERCOMPLETE                           DEF_BIT_01
#define  SDHC_INTSTAT_CMDCOMPLETE                            DEF_BIT_00

#define  SDHC_ERRINTSTAT_TUNING                              DEF_BIT_10
#define  SDHC_ERRINTSTAT_ADMA                                DEF_BIT_09
#define  SDHC_ERRINTSTAT_AUTOCMD12                           DEF_BIT_08
#define  SDHC_ERRINTSTAT_CUR_LIMIT                           DEF_BIT_07
#define  SDHC_ERRINTSTAT_DATAENDBIT                          DEF_BIT_06
#define  SDHC_ERRINTSTAT_DATACRC                             DEF_BIT_05
#define  SDHC_ERRINTSTAT_DATATIMEOUT                         DEF_BIT_04
#define  SDHC_ERRINTSTAT_CMDIX                               DEF_BIT_03
#define  SDHC_ERRINTSTAT_CMDENDBIT                           DEF_BIT_02
#define  SDHC_ERRINTSTAT_CMDCRC                              DEF_BIT_01
#define  SDHC_ERRINTSTAT_CMDTIMEOUT                          DEF_BIT_00

#define  SDHC_INTSTATEN_RETUNE                               DEF_BIT_12
#define  SDHC_INTSTATEN_INT_C                                DEF_BIT_11
#define  SDHC_INTSTATEN_INT_B                                DEF_BIT_10
#define  SDHC_INTSTATEN_INT_A                                DEF_BIT_09
#define  SDHC_INTSTATEN_CARDINT                              DEF_BIT_08
#define  SDHC_INTSTATEN_CARDREM                              DEF_BIT_07
#define  SDHC_INTSTATEN_CARDINS                              DEF_BIT_06
#define  SDHC_INTSTATEN_BUFRDRDY                             DEF_BIT_05
#define  SDHC_INTSTATEN_BUFWRRDY                             DEF_BIT_04
#define  SDHC_INTSTATEN_DMA                                  DEF_BIT_03
#define  SDHC_INTSTATEN_BLKGAP                               DEF_BIT_02
#define  SDHC_INTSTATEN_XFERCOMPLETE                         DEF_BIT_01
#define  SDHC_INTSTATEN_CMDCOMPLETE                          DEF_BIT_00

#define  SDHC_ERRINTSTATEN_TUNING                            DEF_BIT_10
#define  SDHC_ERRINTSTATEN_ADMA                              DEF_BIT_09
#define  SDHC_ERRINTSTATEN_AUTOCMD12                         DEF_BIT_08
#define  SDHC_ERRINTSTATEN_CUR_LIMIT                         DEF_BIT_07
#define  SDHC_ERRINTSTATEN_DATAENDBIT                        DEF_BIT_06
#define  SDHC_ERRINTSTATEN_DATACRC                           DEF_BIT_05
#define  SDHC_ERRINTSTATEN_DATATIMEOUT                       DEF_BIT_04
#define  SDHC_ERRINTSTATEN_CMDIX                             DEF_BIT_03
#define  SDHC_ERRINTSTATEN_CMDENDBIT                         DEF_BIT_02
#define  SDHC_ERRINTSTATEN_CMDCRC                            DEF_BIT_01
#define  SDHC_ERRINTSTATEN_CMDTIMEOUT                        DEF_BIT_00

#define  SDHC_INTSIGEN_RETUNE                                DEF_BIT_12
#define  SDHC_INTSIGEN_INT_C                                 DEF_BIT_11
#define  SDHC_INTSIGEN_INT_B                                 DEF_BIT_10
#define  SDHC_INTSIGEN_INT_A                                 DEF_BIT_09
#define  SDHC_INTSIGEN_CARDINT                               DEF_BIT_08
#define  SDHC_INTSIGEN_CARDREM                               DEF_BIT_07
#define  SDHC_INTSIGEN_CARDINS                               DEF_BIT_06
#define  SDHC_INTSIGEN_BUFRDRDY                              DEF_BIT_05
#define  SDHC_INTSIGEN_BUFWRRDY                              DEF_BIT_04
#define  SDHC_INTSIGEN_DMA                                   DEF_BIT_03
#define  SDHC_INTSIGEN_BLKGAP                                DEF_BIT_02
#define  SDHC_INTSIGEN_XFERCOMPLETE                          DEF_BIT_01
#define  SDHC_INTSIGEN_CMDCOMPLETE                           DEF_BIT_00

#define  SDHC_ERRINTSIGEN_TUNING                             DEF_BIT_10
#define  SDHC_ERRINTSIGEN_ADMA                               DEF_BIT_09
#define  SDHC_ERRINTSIGEN_AUTOCMD12                          DEF_BIT_08
#define  SDHC_ERRINTSIGEN_CUR_LIMIT                          DEF_BIT_07
#define  SDHC_ERRINTSIGEN_DATAENDBIT                         DEF_BIT_06
#define  SDHC_ERRINTSIGEN_DATACRC                            DEF_BIT_05
#define  SDHC_ERRINTSIGEN_DATATIMEOUT                        DEF_BIT_04
#define  SDHC_ERRINTSIGEN_CMDIX                              DEF_BIT_03
#define  SDHC_ERRINTSIGEN_CMDENDBIT                          DEF_BIT_02
#define  SDHC_ERRINTSIGEN_CMDCRC                             DEF_BIT_01
#define  SDHC_ERRINTSIGEN_CMDTIMEOUT                         DEF_BIT_00

#define  SDHC_AUTOCMDERR_CMDNOTISSUED                        DEF_BIT_07
#define  SDHC_AUTOCMDERR_AUTOCMDIX                           DEF_BIT_04
#define  SDHC_AUTOCMDERR_AUTOCMDENDBIT                       DEF_BIT_03
#define  SDHC_AUTOCMDERR_AUTOCMDCRC                          DEF_BIT_02
#define  SDHC_AUTOCMDERR_AUTOCMDTIMEOUT                      DEF_BIT_01
#define  SDHC_AUTOCMDERR_AUTOCMD12NOTEXEC                    DEF_BIT_00

#define  SDHC_HOSTCTRL2_UHS_MODE_SEL_SDR12                   DEF_BIT_NONE
#define  SDHC_HOSTCTRL2_UHS_MODE_SEL_SDR25                   DEF_BIT_00
#define  SDHC_HOSTCTRL2_UHS_MODE_SEL_SDR50                   DEF_BIT_01
#define  SDHC_HOSTCTRL2_UHS_MODE_SEL_SDR104                 (DEF_BIT_00 | DEF_BIT_01)
#define  SDHC_HOSTCTRL2_UHS_MODE_SEL_DDR50                   DEF_BIT_02
#define  SDHC_HOSTCTRL2_1_8V_SIG_EN                          DEF_BIT_03
#define  SDHC_HOSTCTRL2_DRV_STRENGTH_SEL_TYPE_B              DEF_BIT_NONE
#define  SDHC_HOSTCTRL2_DRV_STRENGTH_SEL_TYPE_A              DEF_BIT_04
#define  SDHC_HOSTCTRL2_DRV_STRENGTH_SEL_TYPE_C              DEF_BIT_05
#define  SDHC_HOSTCTRL2_DRV_STRENGTH_SEL_TYPE_D             (DEF_BIT_04 | DEF_BIT_05)
#define  SDHC_HOSTCTRL2_EXEC_TUNING                          DEF_BIT_06
#define  SDHC_HOSTCTRL2_SMPL_CLK_SEL                         DEF_BIT_07
#define  SDHC_HOSTCTRL2_ASYNC_INT_EN                         DEF_BIT_14
#define  SDHC_HOSTCTRL2_PRESET_VAL_EN                        DEF_BIT_15

#define  SDHC_CAPABILITIES_SLOT_TYPE_MSK                    (DEF_BIT_30 | DEF_BIT_31)
#define  SDHC_CAPABILITIES_SLOT_TYPE_REMOVABLE               DEF_BIT_NONE
#define  SDHC_CAPABILITIES_SLOT_TYPE_EMBEDDED                DEF_BIT_30
#define  SDHC_CAPABILITIES_SLOT_TYPE_SHARED                  DEF_BIT_31
#define  SDHC_CAPABILITIES_ASYNC_INT_SUPPORT                 DEF_BIT_29
#define  SDHC_CAPABILITIES_64B_SYSBUS_SUPPORT                DEF_BIT_28
#define  SDHC_CAPABILITIES_1_8V_SUPPORT                      DEF_BIT_26
#define  SDHC_CAPABILITIES_3V_SUPPORT                        DEF_BIT_25
#define  SDHC_CAPABILITIES_3_3V_SUPPORT                      DEF_BIT_24
#define  SDHC_CAPABILITIES_SUSP_RESUME_SUPPORT               DEF_BIT_23
#define  SDHC_CAPABILITIES_SDMA_SUPPORT                      DEF_BIT_22
#define  SDHC_CAPABILITIES_HS_SUPPORT                        DEF_BIT_21
#define  SDHC_CAPABILITIES_ADMA2_SUPPORT                     DEF_BIT_19
#define  SDHC_CAPABILITIES_8BIT_SUPPORT                      DEF_BIT_18
#define  SDHC_CAPABILITIES_MAX_BLK_LEN_MSK                  (DEF_BIT_16 | DEF_BIT_17)
#define  SDHC_CAPABILITIES_MAX_BLK_LEN_512                   DEF_BIT_NONE
#define  SDHC_CAPABILITIES_MAX_BLK_LEN_1024                  DEF_BIT_16
#define  SDHC_CAPABILITIES_MAX_BLK_LEN_2048                  DEF_BIT_17
#define  SDHC_CAPABILITIES_BASE_CLK_FREQ_SD_MSK              DEF_BIT_FIELD(8u, 8u)
#define  SDHC_CAPABILITIES_TIMEOUT_CLK_UNIT_MHZ              DEF_BIT_07
#define  SDHC_CAPABILITIES_TIMEOUT_CLK_FREQ_MSK              DEF_BIT_FIELD(5u, 0u)

#define  SDHC_CAPABILITIES2_SDR50_SUPPORT                    DEF_BIT_00
#define  SDHC_CAPABILITIES2_SDR104_SUPPORT                   DEF_BIT_01
#define  SDHC_CAPABILITIES2_DDR50_SUPPORT                    DEF_BIT_02
#define  SDHC_CAPABILITIES2_DRV_TYPEA_SUPPORT                DEF_BIT_04
#define  SDHC_CAPABILITIES2_DRV_TYPEC_SUPPORT                DEF_BIT_05
#define  SDHC_CAPABILITIES2_DRV_TYPED_SUPPORT                DEF_BIT_06
#define  SDHC_CAPABILITIES2_DRV_TIMER_CNT_RETUNING_MSK       DEF_BIT_FIELD(4u, 8u)
#define  SDHC_CAPABILITIES2_TUNING_SDR50_SUPPORT             DEF_BIT_13
#define  SDHC_CAPABILITIES2_RETUNING_MODE_MSK               (DEF_BIT_14 | DEF_BIT_15)
#define  SDHC_CAPABILITIES2_RETUNING_MODE1                   DEF_BIT_NONE
#define  SDHC_CAPABILITIES2_RETUNING_MODE2                   DEF_BIT_14
#define  SDHC_CAPABILITIES2_RETUNING_MODE3                   DEF_BIT_15
#define  SDHC_CAPABILITIES2_CLK_MULT_MSK                     DEF_BIT_FIELD(8u, 16u)

#define  SDHC_MAXCURCAP_3_3V_MSK                             DEF_BIT_FIELD(8u, 0u)
#define  SDHC_MAXCURCAP_3V_MSK                               DEF_BIT_FIELD(8u, 8u)
#define  SDHC_MAXCURCAP_1_8V_MSK                             DEF_BIT_FIELD(8u, 16u)

#define  SDHC_FORCEACMDERR_CMDNOTISSUED                      DEF_BIT_07
#define  SDHC_FORCEACMDERR_AUTOCMDIX                         DEF_BIT_04
#define  SDHC_FORCEACMDERR_AUTOCMDENDBIT                     DEF_BIT_03
#define  SDHC_FORCEACMDERR_AUTOCMDCRC                        DEF_BIT_02
#define  SDHC_FORCEACMDERR_AUTOCMDTIMEOUT                    DEF_BIT_01
#define  SDHC_FORCEACMDERR_AUTOCMD12NOTEXEC                  DEF_BIT_00

#define  SDHC_FORCEERR_ADMA                                  DEF_BIT_09
#define  SDHC_FORCEERR_AUTOCMD                               DEF_BIT_08
#define  SDHC_FORCEERR_CUR_LIMIT                             DEF_BIT_07
#define  SDHC_FORCEERR_DATAENDBIT                            DEF_BIT_06
#define  SDHC_FORCEERR_DATACRC                               DEF_BIT_05
#define  SDHC_FORCEERR_DATATIMEOUT                           DEF_BIT_04
#define  SDHC_FORCEERR_CMDIX                                 DEF_BIT_03
#define  SDHC_FORCEERR_CMDENDBIT                             DEF_BIT_02
#define  SDHC_FORCEERR_CMDCRC                                DEF_BIT_01
#define  SDHC_FORCEERR_CMDTIMEOUT                            DEF_BIT_00

#define  SDHC_ADMAERRSTAT_LENMISMATCHERR                     DEF_BIT_02
#define  SDHC_ADMAERRSTAT_ERRSTATES_MSK                     (DEF_BIT_00 | DEF_BIT_01)
#define  SDHC_ADMAERRSTAT_ERRSTATE_STOP                      DEF_BIT_NONE
#define  SDHC_ADMAERRSTAT_ERRSTATE_FETCHDESC                 DEF_BIT_00
#define  SDHC_ADMAERRSTAT_ERRSTATE_XFERDATA                 (DEF_BIT_00 | DEF_BIT_01)

#define  SDHC_SHAREDBUSCTRL_BACK_END_PWR_CTRL(dev)           DEF_BIT(24u + (dev))
#define  SDHC_SHAREDBUSCTRL_INT_PIN_SEL_MSK                 (DEF_BIT_20 | DEF_BIT_21 | DEF_BIT_22)
#define  SDHC_SHAREDBUSCTRL_INT_PIN_SEL_INT_A                DEF_BIT_20
#define  SDHC_SHAREDBUSCTRL_INT_PIN_SEL_INT_B                DEF_BIT_21
#define  SDHC_SHAREDBUSCTRL_INT_PIN_SEL_INT_C                DEF_BIT_22
#define  SDHC_SHAREDBUSCTRL_CLK_PIN_SEL_MSK                 (DEF_BIT_16 | DEF_BIT_17 | DEF_BIT_18)
#define  SDHC_SHAREDBUSCTRL_BUS_WIDTH_8B(dev)                DEF_BIT(8u + (dev))
#define  SDHC_SHAREDBUSCTRL_INT_INPUT_PINS_MSK              (DEF_BIT_04 | DEF_BIT_05)
#define  SDHC_SHAREDBUSCTRL_INT_INPUT_PINS_NONE              DEF_BIT_NONE
#define  SDHC_SHAREDBUSCTRL_INT_INPUT_PINS_INTA              DEF_BIT_04
#define  SDHC_SHAREDBUSCTRL_INT_INPUT_PINS_INTA_B            DEF_BIT_05
#define  SDHC_SHAREDBUSCTRL_INT_INPUT_PINS_INTA_B_C         (DEF_BIT_04 | DEF_BIT_05)
#define  SDHC_SHAREDBUSCTRL_CLK_PINS_NBR_MSK                 DEF_BIT_FIELD(3u, 0u)

#define  SDHC_SLOTINTSTATUS_SLOT(slot)                       DEF_BIT(slot)

#define  SDHC_HCVERS_SPEC_VER_NBR_MSK                        DEF_BIT_FIELD(8u, 0u)
#define  SDHC_HCVERS_VEND_VER_NBR_MSK                        DEF_BIT_FIELD(8u, 8u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                       SDHC registers structure
 *******************************************************************************************************/

typedef struct sd_hc_regs {
  CPU_REG32 SDMASA;
  CPU_REG16 BLKSIZE;
  CPU_REG16 BLKCNT;
  CPU_REG32 ARG;
  CPU_REG32 XFERMODE;
  CPU_REG32 RSP10;
  CPU_REG32 RSP32;
  CPU_REG32 RSP54;
  CPU_REG32 RSP76;
  CPU_REG32 DATA;
  CPU_REG32 PRESSTATE;
  CPU_REG08 HOSTCTRL1;
  CPU_REG08 PWRCTRL;
  CPU_REG08 BLKGAPCTRL;
  CPU_REG08 WAKEUPCTRL;
  CPU_REG16 CLKCTRL;
  CPU_REG08 TIMEOUTCTRL;
  CPU_REG08 SOFTRST;
  CPU_REG16 INTSTAT;
  CPU_REG16 ERRINTSTAT;
  CPU_REG16 INTSTATEN;
  CPU_REG16 ERRINTSTATEN;
  CPU_REG16 INTSIGEN;
  CPU_REG16 ERRINTSIGEN;
  CPU_REG16 AUTOCMDERR;
  CPU_REG16 HOSTCTRL2;
  CPU_REG32 CAPABILITIES;
  CPU_REG32 CAPABILITIES2;
  CPU_REG32 MAXCURCAP;
  CPU_REG08 RSVD2[4u];
  CPU_REG16 FORCEACMDERR;
  CPU_REG16 FORCEERR;
  CPU_REG08 ADMAERRSTAT;
  CPU_REG08 RSVD3[3u];
  CPU_REG32 ADMASAL;
  CPU_REG32 ADMASAH;
  CPU_REG32 PRESETVAL[4u];
  CPU_REG08 RSVD4[112u];
  CPU_REG32 SHAREDBUSCTRL;
  CPU_REG08 RSVD5[24u];
  CPU_REG16 SLOTINTSTATUS;
  CPU_REG16 HCVERS;
} SD_HC_REGS;

/*
 ********************************************************************************************************
 *                                       SDHC driver data structure
 *******************************************************************************************************/

typedef struct sd_hc_drv_data {
  SD_CARD_BUS_HANDLE BusHandle;                                 ///< Handle to SD bus.
} SD_HC_DRV_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const CPU_INT32U SDHC_ClkPrescaler[] = { 1u, 2u, 4u, 8u, 16u, 32u, 64u, 128u, 256u };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DRIVER FUNCTIONS
 *******************************************************************************************************/

static void SD_CtrlrDrvInit(SD_CARD_DRV        *p_drv,
                            SD_CARD_BUS_HANDLE handle,
                            MEM_SEG            *p_mem_seg,
                            RTOS_ERR           *p_err);

static void SD_CtrlrDrvInitHW(SD_CARD_DRV *p_drv,
                              RTOS_ERR    *p_err);

static void SD_CtrlrDrvStart(SD_CARD_DRV *p_drv,
                             RTOS_ERR    *p_err);

static void SD_CtrlrDrvStop(SD_CARD_DRV *p_drv,
                            RTOS_ERR    *p_err);

static void SD_CtrlrDrvClkFreqSet(SD_CARD_DRV *p_drv,
                                  CPU_INT32U  freq,
                                  RTOS_ERR    *p_err);

static void SD_CtrlrDrvBusSupplyVoltSet(SD_CARD_DRV      *p_drv,
                                        SD_CARD_BUS_VOLT volt,
                                        RTOS_ERR         *p_err);

static void SD_CtrlrDrvBusSignalVoltSet(SD_CARD_DRV             *p_drv,
                                        SD_CARD_BUS_SIGNAL_VOLT volt,
                                        RTOS_ERR                *p_err);

static SD_CARD_BUS_SIGNAL_VOLT SD_CtrlrDrvBusSignalVoltGet(SD_CARD_DRV *p_drv,
                                                           RTOS_ERR    *p_err);

static void SD_CtrlrDrvBusWidthSet(SD_CARD_DRV *p_drv,
                                   CPU_INT08U  width,
                                   RTOS_ERR    *p_err);

static CPU_INT08U SD_CtrlrDrvDataLinesLvlGet(SD_CARD_DRV *p_drv,
                                             RTOS_ERR    *p_err);

static void SD_CtrlrDrvCmdStart(SD_CARD_DRV *p_drv,
                                SD_CARD_CMD *p_cmd,
                                CPU_INT08U  *p_resp,
                                RTOS_ERR    *p_err);

static void SD_CtrlrDrvCmdRespGet(SD_CARD_DRV *p_drv,
                                  CPU_INT08U  *p_resp,
                                  RTOS_ERR    *p_err);

static void SD_CtrlrDrvDataBufSubmit(SD_CARD_DRV *p_drv,
                                     CPU_INT08U  *p_buf,
                                     CPU_INT32U  blk_size,
                                     CPU_INT32U  blk_cnt,
                                     CPU_BOOLEAN dir_is_rd,
                                     RTOS_ERR    *p_err);

static void SD_CtrlrDrvCardIntEnDis(SD_CARD_DRV *p_drv,
                                    CPU_BOOLEAN enable);

static void SD_CtrlrDrvCtrlrReset(SD_CARD_DRV             *p_drv,
                                  SD_CARD_CTRLR_RESET_LVL lvl,
                                  RTOS_ERR                *p_err);

static SD_HOST_CAPABILITIES SD_CtrlrDrvCapabilitiesGet(SD_CARD_DRV *p_drv);

static CPU_SIZE_T SD_CtrlrDrvReqAlignGet(SD_CARD_DRV *p_drv);

/********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 *******************************************************************************************************/

static void SD_CtrlrISR_Handler(SD_CARD_DRV *p_drv);

/********************************************************************************************************
 *                                               API STRUCTURE
 *******************************************************************************************************/

const SD_CARD_CTRLR_DRV_API SDHC_Drv_API_Arasan = {
  .Init = SD_CtrlrDrvInit,
  .InitHW = SD_CtrlrDrvInitHW,
  .Start = SD_CtrlrDrvStart,
  .Stop = SD_CtrlrDrvStop,
  .ClkFreqSet = SD_CtrlrDrvClkFreqSet,
  .BusSupplyVoltSet = SD_CtrlrDrvBusSupplyVoltSet,
  .BusSignalVoltSet = SD_CtrlrDrvBusSignalVoltSet,
  .BusSignalVoltGet = SD_CtrlrDrvBusSignalVoltGet,
  .BusWidthSet = SD_CtrlrDrvBusWidthSet,
  .DataLinesLvlGet = SD_CtrlrDrvDataLinesLvlGet,
  .CmdStart = SD_CtrlrDrvCmdStart,
  .CmdRespGet = SD_CtrlrDrvCmdRespGet,
  .DataBufSubmit = SD_CtrlrDrvDataBufSubmit,
  .DataXferCmpl = DEF_NULL,
  .CardIntEnDis = SD_CtrlrDrvCardIntEnDis,
  .CtrlrReset = SD_CtrlrDrvCtrlrReset,
  .CapabilitiesGet = SD_CtrlrDrvCapabilitiesGet,
  .ReqAlignGet = SD_CtrlrDrvReqAlignGet
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION DEFINITIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DRIVER FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvInit()
 *
 * @brief    Initializes SD driver.
 *
 * @param    p_drv       Pointer to SD driver structure.
 *
 * @param    handle      Handle to SD card bus.
 *
 * @param    p_mem_seg   Pointer to memory segment to use for data allocation.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvInit(SD_CARD_DRV        *p_drv,
                            SD_CARD_BUS_HANDLE handle,
                            MEM_SEG            *p_mem_seg,
                            RTOS_ERR           *p_err)
{
  SD_HC_DRV_DATA              *p_data;
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;

  RTOS_ASSERT_CRITICAL(p_bsp_api != DEF_NULL, RTOS_ERR_INVALID_ARG,; );

  p_data = (SD_HC_DRV_DATA *)Mem_SegAlloc("IO - SD HC Drv Data",
                                          p_mem_seg,
                                          sizeof(SD_HC_DRV_DATA),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  Mem_Clr((void *)p_data, sizeof(SD_HC_DRV_DATA));

  p_drv->DataPtr = (void *)p_data;

  p_data->BusHandle = handle;

  if (p_bsp_api->ClkFreqGet == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return;
  }

  if (p_bsp_api->Init != DEF_NULL) {
    p_bsp_api->Init(SD_CtrlrISR_Handler, p_drv);
  }
}

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvInitHW()
 *
 * @brief    Initialize SD controller.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvInitHW(SD_CARD_DRV *p_drv,
                              RTOS_ERR    *p_err)
{
  CPU_BOOLEAN                 ok;
  SD_HC_REGS                  *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;

  //                                                               Initialize BSP.
  if (p_bsp_api->PwrCfg != DEF_NULL) {
    ok = p_bsp_api->PwrCfg();
    if (ok == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (p_bsp_api->ClkEn != DEF_NULL) {
    ok = p_bsp_api->ClkEn();
    if (ok == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (p_bsp_api->IO_Cfg != DEF_NULL) {
    ok = p_bsp_api->IO_Cfg();
    if (ok == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  if (p_bsp_api->IntCfg != DEF_NULL) {
    ok = p_bsp_api->IntCfg();
    if (ok == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  DEF_BIT_FIELD_WR(p_regs->BLKSIZE,
                   SDHC_BLKSIZE_BUF_BOUND_512K,
                   (CPU_REG16)SDHC_BLKSIZE_BUF_BOUND_MSK);

  //                                                               Clear All Normal Interrupt Flags.
  p_regs->INTSTAT = (SDHC_INTSTAT_CARDINS      | SDHC_INTSTAT_BUFRDRDY
                     | SDHC_INTSTAT_BUFWRRDY     | SDHC_INTSTAT_DMA
                     | SDHC_INTSTAT_BLKGAP       | SDHC_INTSTAT_XFERCOMPLETE
                     | SDHC_INTSTAT_CMDCOMPLETE  | SDHC_INTSTAT_CARDINT);

  //                                                               Clear All Error Interrupt Flags.
  p_regs->ERRINTSTAT = (SDHC_ERRINTSTAT_TUNING      | SDHC_ERRINTSTAT_ADMA
                        | SDHC_ERRINTSTAT_AUTOCMD12   | SDHC_ERRINTSTAT_CUR_LIMIT
                        | SDHC_ERRINTSTAT_DATAENDBIT  | SDHC_ERRINTSTAT_DATACRC
                        | SDHC_ERRINTSTAT_DATATIMEOUT | SDHC_ERRINTSTAT_CMDIX
                        | SDHC_ERRINTSTAT_CMDENDBIT   | SDHC_ERRINTSTAT_CMDCRC
                        | SDHC_ERRINTSTAT_CMDTIMEOUT);
}

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvStart()
 *
 * @brief    Starts SD controller.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvStart(SD_CARD_DRV *p_drv,
                             RTOS_ERR    *p_err)
{
  CPU_BOOLEAN                 ok;
  SD_HC_REGS                  *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;
  CPU_INT16U                  reg_val = 0u;

  //                                                               Enable Interrupt Flags.
  reg_val = SDHC_INTSTATEN_RETUNE;
  //                                                               If polling mode, card insertion interrupt not...
  //                                                               ...enabled.
  if (p_drv->HW_InfoPtr->CardDetectMode == SD_CARD_DETECT_MODE_INTERRUPT) {
    DEF_BIT_SET(reg_val, SDHC_INTSTATEN_CARDINS);
  }
  p_regs->INTSTATEN = reg_val;

  p_regs->ERRINTSTATEN = (SDHC_ERRINTSTATEN_CUR_LIMIT | SDHC_ERRINTSTATEN_TUNING);

  p_regs->INTSIGEN = p_regs->INTSTATEN;
  p_regs->ERRINTSIGEN = p_regs->ERRINTSTATEN;

  DEF_BIT_SET(p_regs->PWRCTRL, SDHC_PWRCTRL_SD_BUS_PWR_ON);

  if (p_bsp_api->Start != DEF_NULL) {
    ok = p_bsp_api->Start();
    if (ok == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvStop()
 *
 * @brief    Stops SD controller.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvStop(SD_CARD_DRV *p_drv,
                            RTOS_ERR    *p_err)
{
  CPU_BOOLEAN                 ok;
  SD_HC_REGS                  *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;

  DEF_BIT_CLR(p_regs->PWRCTRL, (CPU_REG08)SDHC_PWRCTRL_SD_BUS_PWR_ON);

  if (p_bsp_api->Stop != DEF_NULL) {
    ok = p_bsp_api->Stop();
    if (ok == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvClkFreqSet()
 *
 * @brief    Sets SD bus clock frequency.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    freq    Frequency to be set, in hertz.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvClkFreqSet(SD_CARD_DRV *p_drv,
                                  CPU_INT32U  freq,
                                  RTOS_ERR    *p_err)
{
  CPU_BOOLEAN                 found_val = DEF_NO;
  CPU_INT08U                  clk_prescaler;
  CPU_INT08U                  cnt_prescaler;
  CPU_INT16U                  clkctrl = 0;
  CPU_INT16U                  timeout;
  CPU_INT32U                  base_clk_freq;
  SD_HC_REGS                  *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;

  //                                                               Disable SD Clock if given frequency is 0.
  if (freq == 0) {
    //                                                             Wait until no more SD activity on the SD Bus.
    timeout = DEF_INT_16U_MAX_VAL;
    while ((DEF_BIT_IS_SET(p_regs->PRESSTATE, (SDHC_PRESSTATE_CMDINHIBIT | SDHC_PRESSTATE_DATAINHIBIT)) == DEF_YES)
           && (timeout > 0u)) {
      timeout--;
    }
    if (timeout == 0u) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }

    DEF_BIT_CLR(p_regs->CLKCTRL, (CPU_REG16)SDHC_CLKCTRL_SD_CLK_EN);
    DEF_BIT_CLR(p_regs->CLKCTRL, (CPU_REG16)SDHC_CLKCTRL_INT_CLK_STBL);
    DEF_BIT_CLR(p_regs->CLKCTRL, (CPU_REG16)SDHC_CLKCTRL_INT_CLK_EN);
    return;
  }

  base_clk_freq = p_bsp_api->ClkFreqGet();

  //                                                               MICRIUM-692 (Later) Support 3.0 mode
  for (cnt_prescaler = 0; cnt_prescaler < SDHC_PRESCALER_AMOUNT; cnt_prescaler++) {
    CPU_INT32U clk_freq;

    clk_freq = (base_clk_freq / SDHC_ClkPrescaler[cnt_prescaler]);

    if (clk_freq <= freq) {
      found_val = DEF_YES;
      break;
    }
  }

  if (found_val != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
    return;
  }

  clk_prescaler = SDHC_ClkPrescaler[cnt_prescaler] / 2u;        // Obtains best Prescaler Value for 'freq'.
  p_regs->CLKCTRL = DEF_BIT_NONE;

  //                                                               Sets Prescale Value.
  DEF_BIT_FIELD_WR(clkctrl, clk_prescaler, (CPU_INT16U)SDHC_CLKCTRL_SDCLK_FREQ_SEL);
  DEF_BIT_SET(clkctrl, SDHC_CLKCTRL_INT_CLK_EN);

  p_regs->CLKCTRL = clkctrl;

  //                                                               Wait till SD Clock is Stable before Enabling Clock.
  timeout = DEF_INT_16U_MAX_VAL;
  while ((DEF_BIT_IS_CLR(p_regs->CLKCTRL, SDHC_CLKCTRL_INT_CLK_STBL) == DEF_YES)
         && (timeout > 0u)) {
    timeout--;
  }
  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               En different clks for proper operation.
  DEF_BIT_SET(p_regs->CLKCTRL, SDHC_CLKCTRL_SD_CLK_EN);

  //                                                               Wait until able to issue next DAT & CMD Command.
  timeout = DEF_INT_16U_MAX_VAL;
  while ((DEF_BIT_IS_SET(p_regs->PRESSTATE, (SDHC_PRESSTATE_CMDINHIBIT | SDHC_PRESSTATE_DATAINHIBIT)) == DEF_YES)
         && (timeout > 0u)) {
    timeout--;
  }
  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvBusSupplyVoltSet()
 *
 * @brief    Sets bus supply voltage.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    volt    Bus voltage supply to set:
 *                       - SD_CARD_BUS_VOLT_3_3
 *                       - SD_CARD_BUS_VOLT_3
 *                       - SD_CARD_BUS_VOLT_1_8
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvBusSupplyVoltSet(SD_CARD_DRV      *p_drv,
                                        SD_CARD_BUS_VOLT volt,
                                        RTOS_ERR         *p_err)
{
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;

  switch (volt) {
    case SD_CARD_BUS_VOLT_3_3:
      DEF_BIT_FIELD_WR(p_regs->PWRCTRL, SDHC_PWRCTRL_SD_BUS_3_3V, (CPU_REG08)SDHC_PWRCTRL_SD_BUS_VOLT_MSK);
      break;

    case SD_CARD_BUS_VOLT_3:
      DEF_BIT_FIELD_WR(p_regs->PWRCTRL, SDHC_PWRCTRL_SD_BUS_3V, (CPU_REG08)SDHC_PWRCTRL_SD_BUS_VOLT_MSK);
      break;

    case SD_CARD_BUS_VOLT_1_8:
      DEF_BIT_FIELD_WR(p_regs->PWRCTRL, SDHC_PWRCTRL_SD_BUS_1_8V, (CPU_REG08)SDHC_PWRCTRL_SD_BUS_VOLT_MSK);
      break;

    case SD_CARD_BUS_VOLT_NONE:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);
      break;
  }
}

/****************************************************************************************************//**
 *                                   SD_CtrlrDrvBusSignalVoltSet()
 *
 * @brief    Sets the bus signaling voltage level.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    volt    voltage level to set:
 *                       - SD_CARD_BUS_SIGNAL_VOLT_1_8
 *                       - SD_CARD_BUS_SIGNAL_VOLT_3_3
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) Additional operations on the voltage level of the SDIO pins could be necessary at the BSP
 *               level.
 *******************************************************************************************************/
static void SD_CtrlrDrvBusSignalVoltSet(SD_CARD_DRV             *p_drv,
                                        SD_CARD_BUS_SIGNAL_VOLT volt,
                                        RTOS_ERR                *p_err)
{
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;
  SD_HC_REGS                  *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;

  PP_UNUSED_PARAM(p_err);

  switch (volt) {
    case SD_CARD_BUS_SIGNAL_VOLT_3_3:
      DEF_BIT_CLR(p_regs->HOSTCTRL2, (CPU_REG16)SDHC_HOSTCTRL2_1_8V_SIG_EN);
      break;

    case SD_CARD_BUS_SIGNAL_VOLT_1_8:
      DEF_BIT_SET(p_regs->HOSTCTRL2, SDHC_HOSTCTRL2_1_8V_SIG_EN);
      break;

    case SD_CARD_BUS_SIGNAL_VOLT_AUTO:
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               See Note #1.
  if (p_bsp_api->SignalVoltSet != DEF_NULL) {
    CPU_BOOLEAN volt_updated;

    volt_updated = p_bsp_api->SignalVoltSet(volt);
    if (volt_updated != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
}

/********************************************************************************************************
 *                                       SD_CtrlrDrvBusSignalVoltGet()
 *
 * @brief      Gets the bus signaling voltage level.
 *
 * @param      p_drv   Pointer to SD driver structure.
 *
 * @param      p_err   Error pointer.
 *
 * @return     SD_CARD_BUS_SIGNAL_VOLT_1_8, if the control bit is still up (enable).
 *             SD_CARD_BUS_SIGNAL_VOLT_3_3, otherwise.
 *******************************************************************************************************/
static SD_CARD_BUS_SIGNAL_VOLT SD_CtrlrDrvBusSignalVoltGet(SD_CARD_DRV *p_drv,
                                                           RTOS_ERR    *p_err)
{
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;

  PP_UNUSED_PARAM(p_err);

  if (DEF_BIT_IS_SET(p_regs->HOSTCTRL2, SDHC_HOSTCTRL2_1_8V_SIG_EN)) {
    return (SD_CARD_BUS_SIGNAL_VOLT_1_8);
  } else {
    return (SD_CARD_BUS_SIGNAL_VOLT_3_3);
  }
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvBusWidthSet()
 *
 * @brief    Sets SD bus width.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    width   Width of the bus to set, in number of bit.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvBusWidthSet(SD_CARD_DRV *p_drv,
                                   CPU_INT08U  width,
                                   RTOS_ERR    *p_err)
{
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;

  PP_UNUSED_PARAM(p_err);

  DEF_BIT_CLR(p_regs->HOSTCTRL1, (CPU_REG08)SDHC_HOSTCTRL1_EXT_DATA_XFER_8B);
  DEF_BIT_CLR(p_regs->HOSTCTRL1, (CPU_REG08)SDHC_HOSTCTRL1_DATAXFERWIDTH_4BIT);

  switch (width) {
    case 8u:                                                    // Set Data Transfer Width to 8-Bit Mode.
      DEF_BIT_SET(p_regs->HOSTCTRL1, SDHC_HOSTCTRL1_EXT_DATA_XFER_8B);
      break;

    case 4u:                                                    // Set Data Transfer Width to 4-Bit Mode.
      DEF_BIT_SET(p_regs->HOSTCTRL1, SDHC_HOSTCTRL1_DATAXFERWIDTH_4BIT);
      break;

    case 1u:
    default:                                                    // Set Default Value to Transfer 1-Bit Mode.
      break;
  }
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvDataLinesLvlGet()
 *
 * @brief    Gets the Data[3:0] lines level.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Bitmap of the data lines level, where DAT0 is bit number 3 and DAT3 the bit number 0.
 *******************************************************************************************************/
static CPU_INT08U SD_CtrlrDrvDataLinesLvlGet(SD_CARD_DRV *p_drv,
                                             RTOS_ERR    *p_err)
{
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  CPU_INT32U lvl_bitmap = DEF_BIT_NONE;

  PP_UNUSED_PARAM(p_err);

  lvl_bitmap = DEF_BIT_FIELD_RD(p_regs->PRESSTATE, SDHC_PRESSTATE_DAT_LINES_SIG_LVL);

  return ((CPU_INT08U)lvl_bitmap);
}

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvCmdStart()
 *
 * @brief    Issue a SD command on the CMD line.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    p_cmd   Pointer to structure describing the command to issue.
 *
 * @param    p_resp  Pointer to buffer that will contain the command's response. Can be useful if a DMA
 *                   request must be prepared for the upcoming response.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvCmdStart(SD_CARD_DRV *p_drv,
                                SD_CARD_CMD *p_cmd,
                                CPU_INT08U  *p_resp,
                                RTOS_ERR    *p_err)
{
  CPU_INT16U xfermode = DEF_BIT_NONE;
  CPU_INT16U cmd = DEF_BIT_NONE;
  CPU_INT16U errintstaten = DEF_BIT_NONE;
  CPU_INT16U intstaten = DEF_BIT_NONE;
  CPU_INT32U timeout;
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_resp);

  //                                                               Wait till appropriate info can be transferred via
  //                                                               the designated comm line.
  timeout = DEF_INT_16U_MAX_VAL;
  while ((DEF_BIT_IS_SET(p_regs->PRESSTATE, SDHC_PRESSTATE_CMDINHIBIT) == DEF_YES)
         && (timeout > 0u)) {
    timeout--;
  }
  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO_TIMEOUT);
    return;
  }

  //                                                               Check Response Type.
  if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_RESP) == DEF_YES) {
    if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_RESP_LONG) == DEF_YES) {
      cmd |= SDHC_CMD_RSPTYPESEL_RESPLEN136;
    } else {
      if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_BUSY) == DEF_YES) {
        cmd |= SDHC_CMD_RSPTYPESEL_RESPLEN48_BUSY;
      } else {
        cmd |= SDHC_CMD_RSPTYPESEL_RESPLEN48;
      }
    }
  }

  if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_DATA_ABORT)) {
    DEF_BIT_FIELD_WR(cmd, SDHC_CMD_CMDTYPE_ABORT, (CPU_INT16U)SDHC_CMD_CMDTYPE_MSK);
  } else if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_DATA_RESUME)) {
    DEF_BIT_FIELD_WR(cmd, SDHC_CMD_CMDTYPE_RESUME, (CPU_INT16U)SDHC_CMD_CMDTYPE_MSK);
  } else if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_DATA_SUSPEND)) {
    DEF_BIT_FIELD_WR(cmd, SDHC_CMD_CMDTYPE_SUSPEND, (CPU_INT16U)SDHC_CMD_CMDTYPE_MSK);
  }

  if (p_cmd->DataDir == SD_CARD_DATA_DIR_HOST_TO_CARD) {
    xfermode |= SDHC_XFERMODE_DMA_EN;
    cmd |= SDHC_CMD_DPSEL;
  }

  if (p_cmd->DataDir == SD_CARD_DATA_DIR_CARD_TO_HOST) {
    xfermode |= SDHC_XFERMODE_XFER_DIR_RD | SDHC_XFERMODE_DMA_EN;
    cmd |= SDHC_CMD_DPSEL;
  }

  if (DEF_BIT_IS_SET(p_cmd->DataType, SD_CARD_DATA_TYPE_MULTI_BLOCK) == DEF_YES) {
    xfermode |= (SDHC_XFERMODE_BLK_CNT_EN                       // Enable Block Count for Multiple Block Transfers.
                 | SDHC_XFERMODE_MULT_BLK_SEL);
  }

  if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_CRC_VALID) == DEF_YES) {
    cmd |= SDHC_CMD_CMDCRCCHKEN;
  }

  if (DEF_BIT_IS_SET(p_cmd->Flags, SD_CARD_CMD_FLAG_IX_VALID) == DEF_YES) {
    cmd |= SDHC_CMD_CMDIXCHKEN;
  }

  DEF_BIT_FIELD_WR(cmd, p_cmd->Cmd, (CPU_INT16U)SDHC_CMD_IX_MSK);

  errintstaten |= (SDHC_ERRINTSTATEN_CMDCRC | SDHC_ERRINTSTATEN_CMDENDBIT
                   | SDHC_ERRINTSTATEN_CMDIX  | SDHC_ERRINTSTATEN_CMDTIMEOUT);
  intstaten |= SDHC_INTSTATEN_CMDCOMPLETE;

  CORE_ENTER_ATOMIC();
  p_regs->INTSTATEN |= intstaten;
  p_regs->ERRINTSTATEN |= errintstaten;

  p_regs->INTSIGEN = p_regs->INTSTATEN;
  p_regs->ERRINTSIGEN = p_regs->ERRINTSTATEN;
  CORE_EXIT_ATOMIC();

  p_regs->ARG = p_cmd->Arg;
  p_regs->XFERMODE = (xfermode | (cmd << 16u));                 // Writing to this reg causes cmd to be issued.
}

/****************************************************************************************************//**
 *                                           SD_CtrlrDrvCmdRespGet()
 *
 * @brief    Retrieves response from previously issued SD command.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    p_resp  Pointer to buffer that will contain the command's response.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvCmdRespGet(SD_CARD_DRV *p_drv,
                                  CPU_INT08U  *p_resp,
                                  RTOS_ERR    *p_err)
{
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  CPU_INT16U *p_resp16 = (CPU_INT16U *)p_resp;

  PP_UNUSED_PARAM(p_err);

  p_resp16[0u] = (p_regs->RSP10 & 0x0000FFFFu);
  p_resp16[1u] = (p_regs->RSP10 & 0xFFFF0000u) >> 16u;
  p_resp16[2u] = (p_regs->RSP32 & 0x0000FFFFu);
  p_resp16[3u] = (p_regs->RSP32 & 0xFFFF0000u) >> 16u;
  p_resp16[4u] = (p_regs->RSP54 & 0x0000FFFFu);
  p_resp16[5u] = (p_regs->RSP54 & 0xFFFF0000u) >> 16u;
  p_resp16[6u] = (p_regs->RSP76 & 0x0000FFFFu);
  p_resp16[7u] = (p_regs->RSP76 & 0xFFFF0000u) >> 16u;
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvDataBufSubmit()
 *
 * @brief    Submits a data buffer for a future data transfer via DAT line.
 *
 * @param    p_drv       Pointer to SD driver structure.
 *
 * @param    p_buf       Pointer to buffer that contains or will receive the data to transmit/received.
 *
 * @param    blk_cnt     Number of blocks to send/receive.
 *
 * @param    blk_size    Size of a block, in octets.
 *
 * @param    dir_is_rd   Flag that indicates if transfer is read or write.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvDataBufSubmit(SD_CARD_DRV *p_drv,
                                     CPU_INT08U  *p_buf,
                                     CPU_INT32U  blk_cnt,
                                     CPU_INT32U  blk_size,
                                     CPU_BOOLEAN dir_is_rd,
                                     RTOS_ERR    *p_err)
{
  CPU_INT32U timeout;
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(dir_is_rd);
  PP_UNUSED_PARAM(p_err);

  RTOS_ASSERT_DBG_ERR_SET((DEF_ADDR_IS_ALIGNED((CPU_INT32U)p_buf, CPU_WORD_SIZE_32) == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Wait till DAT line is ready.
  timeout = DEF_INT_16U_MAX_VAL;
  while ((DEF_BIT_IS_SET(p_regs->PRESSTATE, SDHC_PRESSTATE_DATAINHIBIT) == DEF_YES)
         && (timeout > 0u)) {
    timeout--;
  }
  if (timeout == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO_TIMEOUT);
    return;
  }

  p_regs->SDMASA = (CPU_INT32U)(CPU_ADDR)p_buf;
  p_regs->BLKCNT = blk_cnt;

  DEF_BIT_FIELD_WR(p_regs->BLKSIZE,
                   blk_size,
                   (CPU_REG16)SDHC_BLKSIZE_BLK_SIZE_MSK);

  CORE_ENTER_ATOMIC();
  p_regs->INTSTATEN |= (SDHC_INTSTATEN_XFERCOMPLETE
                        | SDHC_INTSTATEN_DMA);

  p_regs->ERRINTSTATEN |= (SDHC_ERRINTSTATEN_DATACRC
                           | SDHC_ERRINTSTATEN_DATAENDBIT
                           | SDHC_ERRINTSTATEN_DATATIMEOUT);

  p_regs->INTSIGEN = p_regs->INTSTATEN;
  p_regs->ERRINTSIGEN = p_regs->ERRINTSTATEN;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvCardIntEn()
 *
 * @brief    Enables the Card interrupt.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    enable  DEF_YES, to enable Card Interrupt
 *                   DEF_NO,  to disable Card Interrupt
 *******************************************************************************************************/
static void SD_CtrlrDrvCardIntEnDis(SD_CARD_DRV *p_drv,
                                    CPU_BOOLEAN enable)
{
  SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;

  if (enable == DEF_YES) {
    DEF_BIT_SET(p_regs->INTSTATEN, SDHC_INTSTATEN_CARDINT);
    DEF_BIT_SET(p_regs->INTSIGEN, SDHC_INTSTATEN_CARDINT);
  } else {
    DEF_BIT_CLR(p_regs->INTSTATEN, (CPU_REG16)SDHC_INTSIGEN_CARDINT);
    DEF_BIT_CLR(p_regs->INTSIGEN, (CPU_REG16)SDHC_INTSIGEN_CARDINT);
  }
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvCtrlrReset()
 *
 * @brief    Resets part of or entire host controller.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @param    lvl     Level of reset to apply.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void SD_CtrlrDrvCtrlrReset(SD_CARD_DRV             *p_drv,
                                  SD_CARD_CTRLR_RESET_LVL lvl,
                                  RTOS_ERR                *p_err)
{
  CPU_INT08U reset_msk = DEF_BIT_NONE;

  switch (lvl) {
    case SD_CARD_CTRLR_RESET_LVL_CMD:
      reset_msk = SDHC_SOFTRST_SOFTRSTCMD;
      break;

    case SD_CARD_CTRLR_RESET_LVL_DATA:
      reset_msk = SDHC_SOFTRST_SOFTRSTDATA;
      break;

    case SD_CARD_CTRLR_RESET_LVL_ALL:
      reset_msk = SDHC_SOFTRST_SOFTRSTALL;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      break;
  }

  if (reset_msk != DEF_BIT_NONE) {
    CPU_INT16U timeout = DEF_INT_16U_MAX_VAL;
    SD_HC_REGS *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;

    DEF_BIT_SET(p_regs->SOFTRST, reset_msk);                    // Apply reset.

    while ((DEF_BIT_IS_SET(p_regs->SOFTRST, reset_msk))         // Wait until reset completes.
           && (timeout > 0u)) {
      timeout--;
    }

    if (timeout == 0u) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO_TIMEOUT);
    }
  }
}

/****************************************************************************************************//**
 *                                       SD_CtrlrCapabilitiesGet()
 *
 * @brief    Gets SD controller capabilities.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @return   Structure containing the SD host controller capabilities.
 *******************************************************************************************************/
static SD_HOST_CAPABILITIES SD_CtrlrDrvCapabilitiesGet(SD_CARD_DRV *p_drv)
{
  SD_HOST_CAPABILITIES        capab = { 0u };
  SD_HC_REGS                  *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  const SD_CARD_CTRLR_BSP_API *p_bsp_api = p_drv->BSP_API_Ptr;

  capab.Capabilities |= (DEF_BIT_IS_SET(p_regs->CAPABILITIES, SDHC_CAPABILITIES_3_3V_SUPPORT)) ? SD_CAP_BUS_VOLT_3_3    : DEF_BIT_NONE;
  capab.Capabilities |= (DEF_BIT_IS_SET(p_regs->CAPABILITIES, SDHC_CAPABILITIES_3V_SUPPORT))   ? SD_CAP_BUS_VOLT_3      : DEF_BIT_NONE;
  capab.Capabilities |= (DEF_BIT_IS_SET(p_regs->CAPABILITIES, SDHC_CAPABILITIES_1_8V_SUPPORT)) ? SD_CAP_BUS_VOLT_1_8    : DEF_BIT_NONE;

  capab.Capabilities |= (DEF_BIT_IS_SET(p_regs->CAPABILITIES, SDHC_CAPABILITIES_8BIT_SUPPORT)) ? SD_CAP_BUS_WIDTH_8_BIT : DEF_BIT_NONE;
  capab.Capabilities |= SD_CAP_BUS_WIDTH_4_BIT | SD_CAP_BUS_WIDTH_1_BIT;

  capab.Capabilities |= SD_CAP_BUS_SIGNAL_VOLT_3_3 | SD_CAP_BUS_SIGNAL_VOLT_1_8;

  //                                                               MICRIUM-686 Add card detection capab.

  capab.OCR = SD_V2_OCR_VOLTAGE_MASK;

  //                                                               See if BSP wants to override some capabilities...
  if (p_bsp_api->CapabilitiesGet != DEF_NULL) {
    p_bsp_api->CapabilitiesGet(&capab);
  }

  return (capab);
}

/****************************************************************************************************//**
 *                                       SD_CtrlrDrvReqAlignGet()
 *
 * @brief    Gets required buffer alignment.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *
 * @return   Required buffer alignment, in bytes.
 *******************************************************************************************************/
static CPU_SIZE_T SD_CtrlrDrvReqAlignGet(SD_CARD_DRV *p_drv)
{
  PP_UNUSED_PARAM(p_drv);

  return (sizeof(CPU_ALIGN));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SD_CtrlrISR_Handler()
 *
 * @brief    SD controller ISR handler.
 *
 * @param    p_drv   Pointer to SD driver structure.
 *******************************************************************************************************/
static void SD_CtrlrISR_Handler(SD_CARD_DRV *p_drv)
{
  CPU_BOOLEAN    is_cmpl = DEF_NO;
  CPU_INT32U     intstatus;
  CPU_INT32U     errintstatus;
  SD_HC_REGS     *p_regs = (SD_HC_REGS *)p_drv->HW_InfoPtr->BaseAddr;
  SD_HC_DRV_DATA *p_data = (SD_HC_DRV_DATA *)p_drv->DataPtr;
  RTOS_ERR       cmpl_err;

  intstatus = p_regs->INTSTAT;
  p_regs->INTSTAT = intstatus;
  intstatus &= (p_regs->INTSTATEN | SDHC_INTSTAT_ERR_INT);

  errintstatus = p_regs->ERRINTSTAT;
  p_regs->ERRINTSTAT = errintstatus;
  errintstatus &= p_regs->ERRINTSTATEN;

  //                                                               ------------------ CARD INTERRUPT ------------------
  if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTAT_CARDINT)) {
    DEF_BIT_CLR(p_regs->INTSTATEN, (CPU_REG16)SDHC_INTSTATEN_CARDINT);

    SD_CardEventCardInt(p_data->BusHandle);
  }

  //                                                               ------------------ CARD INSERTED -------------------
  if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTATEN_CARDINS)) {
    DEF_BIT_CLR(p_regs->INTSTATEN, (CPU_REG16)SDHC_INTSTATEN_CARDINS);
    DEF_BIT_SET(p_regs->INTSTATEN, SDHC_INTSTATEN_CARDREM);
    DEF_BIT_SET(p_regs->INTSIGEN, SDHC_INTSIGEN_CARDREM);

    SD_CardEventCardDetect(p_data->BusHandle);
  }

  //                                                               ------------------- CARD REMOVED -------------------
  if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTATEN_CARDREM)) {
    DEF_BIT_CLR(p_regs->INTSTATEN, (CPU_REG16)SDHC_INTSTATEN_CARDREM);
    DEF_BIT_SET(p_regs->INTSTATEN, SDHC_INTSTATEN_CARDINS);
    DEF_BIT_SET(p_regs->INTSIGEN, SDHC_INTSIGEN_CARDINS);

    SD_CardEventCardRemove(p_data->BusHandle);
  }

  //                                                               ---------------- COMMAND INTERRUPT -----------------
  if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTAT_ERR_INT)) {
    if (DEF_BIT_IS_SET(errintstatus, SDHC_ERRINTSTAT_CMDTIMEOUT)) {
      RTOS_ERR_SET(cmpl_err, RTOS_ERR_IO_TIMEOUT);
      is_cmpl = DEF_YES;
    } else if (DEF_BIT_IS_SET(errintstatus, SDHC_ERRINTSTAT_CMDCRC)
               || DEF_BIT_IS_SET(errintstatus, SDHC_ERRINTSTAT_CMDIX)
               || DEF_BIT_IS_SET(errintstatus, SDHC_ERRINTSTAT_CMDENDBIT)) {
      RTOS_ERR_SET(cmpl_err, RTOS_ERR_IO);
      is_cmpl = DEF_YES;
    }
  } else if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTAT_CMDCOMPLETE)) {
    RTOS_ERR_SET(cmpl_err, RTOS_ERR_NONE);
    is_cmpl = DEF_YES;
  }

  if (is_cmpl) {
    CORE_DECLARE_IRQ_STATE;

    SD_CardEventCmdCmpl(p_data->BusHandle, cmpl_err);

    CORE_ENTER_ATOMIC();
    p_regs->INTSTATEN &= ~SDHC_INTSTATEN_CMDCOMPLETE;
    p_regs->ERRINTSTATEN &= ~(SDHC_ERRINTSTATEN_CMDCRC | SDHC_ERRINTSTATEN_CMDENDBIT
                              | SDHC_ERRINTSTATEN_CMDIX  | SDHC_ERRINTSTATEN_CMDTIMEOUT);

    p_regs->INTSIGEN = p_regs->INTSTATEN;
    p_regs->ERRINTSIGEN = p_regs->ERRINTSTATEN;
    CORE_EXIT_ATOMIC();
  }

  //                                                               --------------- XFER CMPL INTERRUPT ----------------
  is_cmpl = DEF_NO;
  if (DEF_BIT_IS_SET_ANY(errintstatus, (SDHC_ERRINTSTATEN_DATACRC | SDHC_ERRINTSTATEN_DATAENDBIT))) {
    is_cmpl = DEF_YES;
    RTOS_ERR_SET(cmpl_err, RTOS_ERR_IO);
  } else if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTAT_XFERCOMPLETE)) {
    is_cmpl = DEF_YES;
    RTOS_ERR_SET(cmpl_err, RTOS_ERR_NONE);
  } else if (DEF_BIT_IS_SET(errintstatus, SDHC_ERRINTSTATEN_DATATIMEOUT)) {
    is_cmpl = DEF_YES;
    RTOS_ERR_SET(cmpl_err, RTOS_ERR_IO_TIMEOUT);
  } else if (DEF_BIT_IS_SET(intstatus, SDHC_INTSTAT_DMA)) {
    p_regs->SDMASA = p_regs->SDMASA;                            // Re-load register with start address at next page.
  }

  if (is_cmpl) {
    CORE_DECLARE_IRQ_STATE;

    SD_CardEventDataXferCmpl(p_data->BusHandle, cmpl_err);

    CORE_ENTER_ATOMIC();
    p_regs->INTSTATEN &= ~(SDHC_INTSTATEN_XFERCOMPLETE
                           | SDHC_INTSTATEN_DMA);

    p_regs->ERRINTSTATEN &= ~(SDHC_ERRINTSTATEN_DATACRC
                              | SDHC_ERRINTSTATEN_DATAENDBIT
                              | SDHC_ERRINTSTATEN_DATATIMEOUT);

    p_regs->INTSIGEN = p_regs->INTSTATEN;
    p_regs->ERRINTSIGEN = p_regs->ERRINTSTATEN;
    CORE_EXIT_ATOMIC();
  }

  //                                                               MICRIUM-692 Handle retune int
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SD_AVAIL))
