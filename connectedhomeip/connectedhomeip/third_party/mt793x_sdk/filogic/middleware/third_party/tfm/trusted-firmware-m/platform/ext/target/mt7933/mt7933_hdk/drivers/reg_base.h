#ifndef __MT_REG_BASE
#define __MT_REG_BASE

#include <stdint.h>

#define CM33_CFG_BASE                0x20000000
#define CACHE_BASE                   0xB0080000
#define CACHE_MPU_BASE               0xB0090000
#define IRQ_CFG_BASE                 0xB1000000
#define APXGPT_BASE                  0xB1010000
#define WDT_BASE                     0xB1020000
#define CM33_GPT_BASE                0xB1030000
#define CM33_UART_BASE               0xB1040000
#define CM33_REG_BASE                0xB1050000
#define CHIP_MISC_BASE               0xB1060000
#define CHIP_RESOURCE_BASE           0xB1080000
#define SPIS_TEST_MODE_BASE          0xB1090000
#define CM33_BCRM_BASE               0xB10A0000
#define FIREWALL_ROUTING_BASE        0xC0000000
#define TOP_CFG_BASE                 0xC0000000
#define EFUSE_CTRL_0_BASE            0xC0010000
#define EFUSE_CTRL_1_BASE            0xC0020000
#define TOP_CLK_OFF_BASE             0xC0020000
#define TOP_CFG_AON_BASE             0xC0030000
#define PWM0_BASE                    0xC0040000
#define PWM1_BASE                    0xC0050000
#define PWM2_BASE                    0xC0060000
#define RTC_BASE                     0xC0070000
#define SEJ_BASE                     0xC0090000
#define SECURITY_WRAPPER_BASE        0xC00A0000
#define PMU_CTRL_BASE                0xC00B0000
#define PLL_CTRL_BASE                0xC00C0000
#define SPM_TOP_BASE                 0xC00D0000
#define XTAL_CTRL_BASE               0xC00E0000
#define INFRA_DEVAPC_AON_BASE        0xC0300000
#define INFRA_BCRM_AON_BASE          0xC0400000
#define INFRA_DEBUG_CTRL_AON_BASE    0xC0401000
#define PSRAM_BCRM_BASE              0xC0402000
#define CHIP_PINMUX_BASE             0xC0403000
#define GPIO_BASE_BASE               0xC0404000
#define I2C0_BASE                    0xC0480000
#define I2C1_BASE                    0xC0481000
#define AP_UART0_BASE                0xC0482000
#define AP_UART1_BASE                0xC0483000
#define CM33_DSP_SIM_MDM_BASE        0xC0600000
#define INFRA_REG_BASE               0xC4000000
#define SYSRAM_CTRL_BASE             0xC4020000
#define TOP_CFG_OFF_BASE             0xC4030000
#define OTF_AES_BASE                 0xC4040000
#define CPUM_BASE                    0xC4050000
#define GCPU_SEC_BASE                0xC4401000
#define GCPU_NON_SEC_BASE            0xC4402000
#define TRNG_BASE                    0xC4404000
#define INFRA_BCRM_OFF_BASE          0xC4405000
#define INFRA_DEVAPC_BASE            0xC4406000
#define AP_DMA_BASE                  0xC4407000
#define CQ_DMA_BASE                  0xC4408000
#define ADC_BASE                     0xC8000000
#define SPIM0_BASE                   0xC8010000
#define SPIM1_BASE                   0xC8020000
#define SPIS_BASE                    0xC8030000
#define BTIF_HOST_BASE               0xC8040000
#define IRRX_BASE                    0xC8050000
#define KEYPAD_BASE                  0xC8060000
#define NON_UHS_PSRAM_APB_SLAVE_BASE 0xC8070000
#define UHS_PSRAMC_NAO_BASE          0xC8080000
#define UHS_PSRAMC_AO_BASE           0xC8090000
#define UHS_DDRPHY_NAO_BASE          0xC80A0000
#define UHS_DDRPHY_AO_BASE           0xC80B0000
#define UHS_PEMI_BASE                0xC80C0000
#define USB_BASE                     0xC8100000
#define USB_SIF_BASE                 0xC8110000
#define SDIOM_BASE                   0xC8120000
#define SDIOS_BASE                   0xC8130000
#define DSP_SRAM0_BASE               0xD0040000
#define DSP_SRAM1_BASE               0xD0050000
#define DSP_SRAM2_BASE               0xD0060000
#define DSP_SRAM3_BASE               0xD0070000
#define AUD_TOP_BASE                 0xD0080000
#define AUD_SYS_TOP_BASE             0xD0090000
#define DSP_TIMER_BASE               0xD1000000
#define DSP_UART_BASE                0xD1001000
#define DSP_IRQ_CFG_BASE             0xD1002000
#define DSP_REG_BASE                 0xD1003000
#define DSP_WDT_BASE                 0xD1003400
#define AUDIO_BCRM_BASE              0xD1004000
#define AUDIO_DAPC_AO_BASE           0xD1005000
#define AUDIO_PDN_BCRM_BASE          0xD1008000
#define AUDIO_DAPC_PDN_BASE          0xD1009000
#define KBCRM_BASE                   0xD100A000
#define DEBUG_BASE                   0xD100B000
#define SYSCFG_AO_BASE               0xD100C000
#define CONNAC_AP2CONN_BASE          0x60000000
#define SYSRAM_TOP_BASE              0x80000000
#define GRP1_EEF_TOP                 0xC0405000
#define GRP2_EEF_TOP                 0xC0406000
#define EFUSE_TOP                    0xC0407000

#ifndef __IO
#define     __IO    volatile             /*!< Defines 'read / write' permissions */
#endif

/* structure type to access the CACHE register
 */
typedef struct {
    __IO uint32_t CACHE_CON;
    __IO uint32_t CACHE_OP;
    __IO uint32_t CACHE_HCNT0L;
    __IO uint32_t CACHE_HCNT0U;
    __IO uint32_t CACHE_CCNT0L;
    __IO uint32_t CACHE_CCNT0U;
    __IO uint32_t CACHE_HCNT1L;
    __IO uint32_t CACHE_HCNT1U;
    __IO uint32_t CACHE_CCNT1L;
    __IO uint32_t CACHE_CCNT1U;
    uint32_t RESERVED0[1];
    __IO uint32_t CACHE_REGION_EN;
    uint32_t RESERVED1[16372];                  /**< (0x10000-12*4)/4 */
    __IO uint32_t CACHE_ENTRY_N[16];
    __IO uint32_t CACHE_END_ENTRY_N[16];
} CACHE_REGISTER_T;

/* CACHE_CON register definitions
 */
#define CACHE_CON_MCEN_OFFSET                   (0)
#define CACHE_CON_MCEN_MASK                     (0x1UL<<CACHE_CON_MCEN_OFFSET)

#define CACHE_CON_CNTEN0_OFFSET                 (2)
#define CACHE_CON_CNTEN0_MASK                   (0x1UL<<CACHE_CON_CNTEN0_OFFSET)

#define CACHE_CON_CNTEN1_OFFSET                 (3)
#define CACHE_CON_CNTEN1_MASK                   (0x1UL<<CACHE_CON_CNTEN1_OFFSET)

#define CACHE_CON_MDRF_OFFSET                   (7)
#define CACHE_CON_MDRF_MASK                     (0x1UL<<CACHE_CON_MDRF_OFFSET)

#define CACHE_CON_CACHESIZE_OFFSET              (8)
#define CACHE_CON_CACHESIZE_MASK                (0x3UL<<CACHE_CON_CACHESIZE_OFFSET)

/* CACHE_OP register definitions
 */
#define CACHE_OP_EN_OFFSET                      (0)
#define CACHE_OP_EN_MASK                        (0x1UL<<CACHE_OP_EN_OFFSET)

#define CACHE_OP_OP_OFFSET                      (1)
#define CACHE_OP_OP_MASK                        (0xFUL<<CACHE_OP_OP_OFFSET)

#define CACHE_OP_TADDR_OFFSET                   (5)
#define CACHE_OP_TADDR_MASK                     (0x7FFFFFFUL<<CACHE_OP_TADDR_OFFSET)

/* CACHE_HCNT0L register definitions
 */
#define CACHE_HCNT0L_CHIT_CNT0_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_HCNT0U register definitions
 */
#define CACHE_HCNT0U_CHIT_CNT0_MASK             (0xFFFFUL<<0)

/* CACHE_CCNT0L register definitions
 */
#define CACHE_CCNT0L_CACC_CNT0_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_CCNT0U register definitions
 */
#define CACHE_CCNT0U_CACC_CNT0_MASK             (0xFFFFUL<<0)

/* CACHE_HCNT1L register definitions
 */
#define CACHE_HCNT1L_CHIT_CNT1_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_HCNT1U register definitions
 */
#define CACHE_HCNT1U_CHIT_CNT1_MASK             (0xFFFFUL<<0)

/* CACHE_CCNT1L register definitions
 */
#define CACHE_CCNT1L_CACC_CNT1_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_CCNT1U register definitions
 */
#define CACHE_CCNT1U_CACC_CNT1_MASK             (0xFFFFUL<<0)

/* CACHE_ENTRY_N register definitions
 */
#define CACHE_ENTRY_N_C_MASK                    (0x1UL<<8)
#define CACHE_ENTRY_BASEADDR_MASK               (0xFFFFFUL<<12)

/* CACHE_END_ENTRY_N register definitions
 */
#define CACHE_END_ENTRY_N_BASEADDR_MASK         (0xFFFFFUL<<12)

/* memory mapping of MT7687
 */
#define CACHE                                   ((CACHE_REGISTER_T *)CACHE_BASE)

/* CACHE register End 
 */
 

#endif

