#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines BA431 Random generator registers and fields
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "ba431_config.h"
#include "sxregs.h"
#include "cryptolib_def.h"

uint32_t ba431_read_fifovalue(void)
{
   return RD_REG32(*CRYPTOLIB_BA431_FIFO);
}

uint32_t ba431_read_fifolevel(void)
{
   return RD_REG32(BA431_REGS->FIFOLevelReg);
}

uint32_t ba431_read_status(void)
{
   return RD_REG32(BA431_REGS->StatusReg);
}

uint32_t ba431_read_controlreg(void)
{
   return RD_REG32(BA431_REGS->ControlReg);
}

void ba431_write_fifo_wakeup_threshold(uint32_t fifo_threshold)
{
   WR_REG32(BA431_REGS->FIFOThresholdReg, fifo_threshold);
}


void ba431_write_switch_off_timer(uint32_t sw_off_timer_val)
{
   CRYPTOLIB_ASSERT_NM(!(sw_off_timer_val & 0xFFFF0000));
   WR_REG32(BA431_REGS->SwOffTmrValReg, sw_off_timer_val);
}


void ba431_write_clk_div(uint8_t clk_div)
{
   WR_REG32(BA431_REGS->ClkDivReg, clk_div);
}


void ba431_write_init_wait(uint32_t init_wait_val)
{
   CRYPTOLIB_ASSERT_NM(!(init_wait_val & 0xFFFF0000));
   WR_REG32(BA431_REGS->InitWaitReg, init_wait_val);
}


void ba431_clear_ctrl(void)
{
   WR_REG32(BA431_REGS->ControlReg, 0);
}


void ba431_enable_ndrng(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_NDRNG_ENABLE);
}


void ba431_disable_ndrng(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() & ~BA431_CTRL_NDRNG_ENABLE);
}


void ba431_enable_test_mode(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_TEST_ENABLE);
}


void ba431_disable_test_mode(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() & ~BA431_CTRL_TEST_ENABLE);
}


void ba431_set_cond_test_mode(void)
{
   WR_REG32(BA431_REGS->ControlReg, BA431_CTRL_TEST_ENABLE | BA431_CTRL_BYPASS_AIS31_TEST | BA431_CTRL_BYPASS_HEALTH_TEST);
}


void ba431_enable_health_test_irq(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_INT_REP_EN | BA431_CTRL_INT_PROP1_EN | BA431_CTRL_INT_PROP2_EN);
}


void ba431_disable_health_test_irq(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() & ~(BA431_CTRL_INT_REP_EN | BA431_CTRL_INT_PROP1_EN | BA431_CTRL_INT_PROP2_EN));
}


void ba431_enable_fifofull_irq(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_INT_FIFOFULL_EN);
}


void ba431_disable_fifofull_irq(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() & ~BA431_CTRL_INT_FIFOFULL_EN);
}


void ba431_softreset(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_SOFTRESET);
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() & ~BA431_CTRL_SOFTRESET);
}


void ba431_enable_AIS31_test_irq(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_INT_ALM_EN);
}


void ba431_force_run(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() | BA431_CTRL_FORCE_RUN);
}


void ba431_disable_AIS31_test_irq(void)
{
   WR_REG32(BA431_REGS->ControlReg, ba431_read_controlreg() & ~(BA431_CTRL_INT_PREALM_EN | BA431_CTRL_INT_ALM_EN));
}


void ba431_set_nb128bitblocks(uint8_t nb128bitblocks)
{
   CRYPTOLIB_ASSERT_NM(!(nb128bitblocks & 0xF0));
   WR_REG32(BA431_REGS->ControlReg, (ba431_read_controlreg() & ~BA431_CTRL_MASK_NB_128BIT_BLOCKS)
                                       | (nb128bitblocks << BA431_CTRL_OFFSET_NB_128BIT_BLOCKS));
}


void ba431_write_conditioning_key(const uint32_t * key)
{
   WR_REG32(BA431_REGS->Key0Reg, key[0]);
   WR_REG32(BA431_REGS->Key1Reg, key[1]);
   WR_REG32(BA431_REGS->Key2Reg, key[2]);
   WR_REG32(BA431_REGS->Key3Reg, key[3]);
}


void ba431_write_test_data(uint32_t test_data)
{
   /*Wait for previous data to be processed*/
   while(ba431_read_status() & BA431_STAT_MASK_TESTDATA_BUSY);
   /*Write data*/
   WR_REG32(BA431_REGS->TestDataReg,test_data);
}


ba431_state_t ba431_get_state(void)
{
   return (ba431_state_t) (ba431_read_status() & BA431_STAT_MASK_STATE);
}
