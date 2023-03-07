
#ifndef __BFLB_PLATFORM__H__
#define __BFLB_PLATFORM__H__

#include "stdio.h"
#include "stdint.h"
#include "string.h"

#define bflb_platform_init(...)

#define WRITE_REG(a,v)    *((volatile uint32_t *)(a))=(v)
#define MSG_PRINT_MSG_LEN  (0x200)
#define SV_C_SHARE_LEN     (0x200)
#define DBG_BASE           (0x5201bc00) 


#define MSG_PRINT_MARK_ADR          (DBG_BASE)
#define MSG_PRINT_MSG_ADR           (MSG_PRINT_MARK_ADR+4)
#define MSG_PRINT_MSG_MARK          (0xABCDABCD)
#define MSG_PRINT_ERR_MSG_MARK      (0xCDABCDAB)


#define SIM_END_MARK_ADR            (DBG_BASE+MSG_PRINT_MSG_LEN+4+SV_C_SHARE_LEN)
#define SIM_END_MARK                (0xa5a55a5a)

#define SOC_SV_C_SHARE              ((volatile uint32_t *)(DBG_BASE+MSG_PRINT_MSG_LEN + 4))
#define DV_CFG_BASE                 SOC_SV_C_SHARE

extern void BL602_Delay_US(uint32_t cnt);

#define MSG(a,...)              {sprintf((char*)MSG_PRINT_MSG_ADR, a, ##__VA_ARGS__);\
                                    WRITE_REG(MSG_PRINT_MARK_ADR, MSG_PRINT_MSG_MARK);} 
#define MSG_ERR(a,...)          {sprintf((char*)MSG_PRINT_MSG_ADR, a, ##__VA_ARGS__); \
                                    WRITE_REG(MSG_PRINT_MARK_ADR, MSG_PRINT_ERR_MSG_MARK);}
                           
#define SIM_END                 WRITE_REG(SIM_END_MARK_ADR, SIM_END_MARK)
#define SIM_FAIL                {MSG_ERR("sw sim fail"); SIM_END;}

#define BL_CASE_FAIL            {SIM_FAIL;while(1){BL602_Delay_US(10);};}
#define BL_CASE_SUCCESS         {SIM_END;while(1){BL602_Delay_US(10);};}


#endif
