#include "l1c_reg.h"
#include "bl602_common.h"
#include <string.h>
#include "bflb_platform.h"

/** @addtogroup  BL602_Periph_Driver
 *  @{
 */

/** @defgroup DRIVER_COMMON DRIVER_COMMON
 *  @brief Digger driver common functions
 *  @{
 */

/** @defgroup DRIVER_Private_Type
 *  @{
 */


/*@} end of group DRIVER_Private_Type*/

/** @defgroup DRIVER_Private_Defines
 *  @{
 */

/*@} end of group DRIVER_Private_Defines */

/** @defgroup DRIVER_Private_Variables
 *  @{
 */                               

/*@} end of group DRIVER_Private_Variables */

/** @defgroup DRIVER_Global_Variables
 *  @{
 */

/*@} end of group DRIVER_Global_Variables */

/** @defgroup DRIVER_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DRIVER_Private_FunctionDeclaration */

/** @defgroup DRIVER_Private_Functions
 *  @{
 */

/*@} end of group DRIVER_Private_Functions */

/** @defgroup DRIVER_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      delay us and access reg
 *
 * @param[in]  core:  systemcoreclock
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
#ifndef BL602_USE_ROM_DRIVER
#ifdef ARCH_RISCV
__WEAK
void ATTR_TCM_SECTION ASM_Delay_Us_Divu(uint32_t core,uint32_t cnt)
{
    volatile uint32_t codeAddress = 0;
    volatile uint32_t divVal = 40;
    volatile uint32_t speed = 0;
    volatile uint32_t cycNum = 0;

    codeAddress = (uint32_t)&ASM_Delay_Us_Divu;

    /* 1M=100K*10, so multiple is 10 */
    /* loop function take 4 instructions, so instructionNum is 4 */
    /* if codeAddress locate at IROM space and irom_2t_access is 1, then irom2TAccess=2, else irom2TAccess=1 */
    /* divVal = multiple*instructionNum*irom2TAccess */
    if(((codeAddress&(0xF<<24))>>24)==0x01){
        /* IROM space */
        if(BL_GET_REG_BITS_VAL(BL_RD_REG(L1C_BASE,L1C_CONFIG),L1C_IROM_2T_ACCESS)){
            /* instruction 2T */
            divVal = divVal*2;
        }
    }

    if(core>=1000000){
        /* CPU clock >= 1MHz */
        speed = core/100000;
        cycNum = speed*cnt;
        cycNum = cycNum/10;
        cycNum = cycNum/divVal;
        /* cycNum >= 0 */
    }else{
        /* CPU clock < 1MHz */
        speed = core/1000;
        cycNum = speed*cnt;
        cycNum = cycNum/1000;
        cycNum = cycNum/divVal;
        /* cycNum >= 0 */
    }

    __asm__ __volatile__(
        "li       a4,0x0\n\t"
        "li       a5,3\n\t"
        "li       a6,3\n\t"
        ".align 4\n\t"
        "loop  :\n"
        "beq      a4,%0,end\n\t"
        "addi     a4,a4,0x1\n\t"
        "nop\n\t"
        "divu     a6,a6,a5\n\t"
        "j        loop\n\t"
        "nop\n\t"
        "end   :\n\t"
        "nop\n"
        :                                  /* output */
        :"r"(cycNum)                       /* input */
        :"a4","a5","a6"                    /* destruct description */
    );
}
#endif


/****************************************************************************//**
 * @brief      delay us and access reg
 *
 * @param[in]  core:  systemcoreclock
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
void ATTR_TCM_SECTION BL602_Delay_US_Divu(uint32_t cnt)
{
    ASM_Delay_Us_Divu(SystemCoreClockGet(),cnt);
}

/****************************************************************************//**
 * @brief      delay us and access reg
 *
 * @param[in]  core:  systemcoreclock
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
void ATTR_TCM_SECTION BL602_Delay_MS_Divu(uint32_t cnt)
{
    uint32_t i = 0;
    uint32_t count = 0;

    if(cnt>=1024){
        /* delay (n*1024) ms */
        for(i=0;i<(cnt/1024);i++){
            BL602_Delay_US_Divu(1024*1000);
        }
    }
    count = cnt&0x3FF;
    if(count){
        /* delay (1-1023)ms */
        BL602_Delay_US_Divu(count*1000);
    }
}
#endif


/*@} end of group DRIVER_Public_Functions */

/*@} end of group DRIVER_COMMON */

/*@} end of group BL602_Periph_Driver */


