#include "bl602_aon.h"
#include "bl602_hbn.h"
#include "bl602_glb.h"
#include "bl602_pds.h"
#include "bl602_sf_cfg.h"
#include "bl602_sf_ctrl.h"
#include "bl602_sflash.h"
#include "bl602_sflash_ext.h"
#include "bl602_spi.h"
#include "bl602_xip_sflash.h"
#include "bl602_common.h"
#include "bflb_platform.h"
#include <bl_irq.h>
#include <bl_gpio.h>
#include <bl602_glb.h>
#include "pds_level.h"
#include <hal_sys.h> 

#define HBN_RAM_FLAG_PATTEN                 (0x12344321)
#define HBN_RAM_FLAG_ADDR                   (HBN_RAM_BASE)
#define HBN_RAM_CNT_ADDR                    (HBN_RAM_BASE+4)
#define HBN_RAM_DATA_ADDR                   (HBN_RAM_BASE+8)
#define HBN_RAM_DATA_LEN                    (4*1024-8)
#define OCRAM_FLAG_PATTEN                   (0x56788765)
#define OCRAM_FLAG_ADDR                     (BL602_WRAM_BASE+40*1024)
#define OCRAM_CNT_ADDR                      (OCRAM_FLAG_ADDR+4)
#define OCRAM_DATA_ADDR                     (OCRAM_FLAG_ADDR+8)
#define OCRAM_DATA_LEN                      (8*1024-8)
#define BFLB_BOOTROM_NPMAGIC                "BFNP"
#define KEEP_CCI_GPIO_ACTIVE                (0)

static SPI_Flash_Cfg_Type flashCfg_Gd_Q80E_Q16E={
        .resetCreadCmd=0xff,
        .resetCreadCmdSize=3,
        .mid=0xc8,

        .deBurstWrapCmd=0x77,
        .deBurstWrapCmdDmyClk=0x3,
        .deBurstWrapDataMode=SF_CTRL_DATA_4_LINES,
        .deBurstWrapData=0xF0,

        /*reg*/
        .writeEnableCmd=0x06,
        .wrEnableIndex=0x00,
        .wrEnableBit=0x01,
        .wrEnableReadRegLen=0x01,

        .qeIndex=1,
        .qeBit=0x01,
        .qeWriteRegLen=0x02,
        .qeReadRegLen=0x1,

        .busyIndex=0,
        .busyBit=0x00,
        .busyReadRegLen=0x1,
        .releasePowerDown=0xab,

        .readRegCmd[0]=0x05,
        .readRegCmd[1]=0x35,
        .writeRegCmd[0]=0x01,
        .writeRegCmd[1]=0x01,

        .fastReadQioCmd=0xeb,
        .frQioDmyClk=16/8,
        .cReadSupport=1,
        .cReadMode=0xA0,

        .burstWrapCmd=0x77,
        .burstWrapCmdDmyClk=0x3,
        .burstWrapDataMode=SF_CTRL_DATA_4_LINES,
        .burstWrapData=0x40,
         /*erase*/
        .chipEraseCmd=0xc7,
        .sectorEraseCmd=0x20,
        .blk32EraseCmd=0x52,
        .blk64EraseCmd=0xd8,
        /*write*/
        .pageProgramCmd=0x02,
        .qpageProgramCmd=0x32,
        .qppAddrMode=SF_CTRL_ADDR_1_LINE,

        .ioMode=SF_CTRL_QIO_MODE,
        .clkDelay=1,
        .clkInvert=0x3f,

        .resetEnCmd=0x66,
        .resetCmd=0x99,
        .cRExit=0xff,
        .wrEnableWriteRegLen=0x00,

        /*id*/
        .jedecIdCmd=0x9f,
        .jedecIdCmdDmyClk=0,
        .qpiJedecIdCmd=0x9f,
        .qpiJedecIdCmdDmyClk=0x00,
        .sectorSize=4,
        .pageSize=256,

        /*read*/
        .fastReadCmd=0x0b,
        .frDmyClk=8/8,
        .qpiFastReadCmd =0x0b,
        .qpiFrDmyClk=8/8,
        .fastReadDoCmd=0x3b,
        .frDoDmyClk=8/8,
        .fastReadDioCmd=0xbb,
        .frDioDmyClk=0,
        .fastReadQoCmd=0x6b,
        .frQoDmyClk=8/8,

        .qpiFastReadQioCmd=0xeb,
        .qpiFrQioDmyClk=16/8,
        .qpiPageProgramCmd=0x02,
        .writeVregEnableCmd=0x50,

        /* qpi mode */
        .enterQpi=0x38,
        .exitQpi=0xff,

         /*AC*/
        .timeEsector=300,
        .timeE32k=1200,
        .timeE64k=1200,
        .timePagePgm=5,
        .timeCe=20*1000,
        .pdDelay=20,
        .qeData=0,
};

extern void ATTR_TCM_SECTION PDS_Mode_Enter(PDS_APP_CFG_Type *cfg);

void copy_bin_from_flash(uint32_t ramAddr,uint32_t flashAddr,uint32_t length)
{
    uint32_t *psrc = (uint32_t *)(flashAddr);
    uint32_t *pdst = (uint32_t *)(ramAddr);
    
    for(uint32_t i=0;i<length;i++){
        *pdst++ = *psrc++;
    }
}

void WRAM_Init(uint32_t addr, uint32_t cnt)
{
    uint32_t value = 0;
    uint32_t *p = NULL;

    value = addr;
    p = (uint32_t *)(addr);
    for(uint32_t i=0;i<cnt;i++){
        *p = value;
        p++;
        value += 4;
    }
}

BL_Err_Type WRAM_Check(uint32_t addr, uint32_t cnt)
{
    uint32_t value = 0;
    uint32_t *p = NULL;

    value = addr;
    p = (uint32_t *)(addr);
    for(uint32_t i=0;i<cnt;i++){
        if(*p != value){
            return ERROR;
        }
        p++;
        value += 4;
    }

    return SUCCESS;
}

PDS_APP_CFG_Type pdsCfg= {
        .pdsLevel=1,                       /*!< PDS level */
        .turnOffRF=1,                      /*!< Wheather turn off RF */
        .useXtal32k=0,                     /*!< Wheather use xtal 32K as 32K clock source,otherwise use rc32k */
        .pdsAonGpioWakeupSrc=PDS_AON_WAKEUP_GPIO_NONE,            /*!< PDS level 0/1/2/3 mode always on GPIO Wakeup source(HBN wakeup pin) */
        .pdsAonGpioTrigType=PDS_AON_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE,    /*!< PDS level 0/1/2/3 mode always on GPIO Triger type(HBN wakeup pin) */
        .powerDownFlash=0,                 /*!< Whether power down flash */
        .turnOffFlashPad=0,                /*!< Whether turn off embedded flash pad */
        .ocramRetetion=0,                  /*!< Whether OCRAM Retention */
        .turnoffPLL=1,                     /*!< Whether trun off PLL */
        .xtalType=GLB_PLL_XTAL_40M,        /*!< XTal type, used when user choose turn off PLL, PDS will turn on when exit PDS mode */
        .flashContRead=0,                  /*!< Whether enable flash continue read */
        .sleepTime=2*65536,                   /*!< PDS sleep time */
        .flashCfg=&flashCfg_Gd_Q80E_Q16E,  /*!< Flash config pointer, used when power down flash */
        .ldoLevel=PDS_LDO_LEVEL_1P10V,     /*!< LDO level */
        .preCbFun=NULL,        /*!< Pre callback function */
        .postCbFun=NULL,      /*!< Post callback function */
};

void ATTR_TCM_SECTION enter_pds_mode(uint32_t sleep_cycle)
{
    pdsCfg.sleepTime = sleep_cycle;
    /* normal work LDO level */
    HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_1P10V);
    
    PDS_IntClear();
    /* register pds wakeup IRQHandler */
    
    /* enable PDS interrupt to wakeup CPU (PDS1:CPU not powerdown, CPU __WFI) */
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIE + PDS_WAKEUP_IRQn) = 1;
    
    /* clear and mask PDS int */
    PDS_IntMask(PDS_INT_WAKEUP,UNMASK);
    PDS_IntMask(PDS_INT_HBN_GPIO_IRRX_BLE_WIFI,MASK);
    PDS_IntMask(PDS_INT_RF_DONE,MASK);
    PDS_IntMask(PDS_INT_PLL_DONE,MASK);
    PDS_IntClear();

    PDS_Mode_Enter(&pdsCfg);
}

uint8_t ATTR_TCM_SECTION check_whether_enter_pds(void)
{
    uint8_t val;

    GLB_GPIO_Cfg_Type cfg;
    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = PDS_MODE_PIN; 
    cfg.gpioFun = 11;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    val = GLB_GPIO_Read((GLB_GPIO_Type)PDS_MODE_PIN);
    return val;
}

void ATTR_TCM_SECTION pds_mode_entry(void)
{
    uint8_t status;
    uint32_t cycles;

    while (1) {
        cycles = PDS_WAKEUP_MS * 32768 / 1000;
        enter_pds_mode(cycles);
        status = check_whether_enter_pds();
        if (status != PDS_STATUS) {
            hal_reboot();
            return;
        }
    } 
}
