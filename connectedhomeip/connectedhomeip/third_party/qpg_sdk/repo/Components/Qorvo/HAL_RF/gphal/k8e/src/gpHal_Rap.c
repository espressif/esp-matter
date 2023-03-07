/*
 * Copyright (c) 2018, Qorvo Inc
 *
 * !!! This is a generated file. !!!
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

//wrappers/stubs for RAP code
#define abs(a)              (((a) < 0) ? -(a) : (a))

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/*
 * The actual RAP methods.
 */

void rap_bbpll_pup(void)
{
    GP_WB_WRITE_BBPLL_BBPLL_LDO_PUP(1);
    GP_WB_WRITE_BBPLL_BBPLL_BIAS_FILT_BYPASS(1);
    GP_WB_WRITE_BBPLL_BBPLL_RING_BIAS_PUP(1);
    rap_h_wait_us(10);
    GP_WB_WRITE_BBPLL_BBPLL_RING_OSC_PUP(1);
    rap_h_wait_us(1);
}

void rap_bbpll_calibrate(void)
{
    Int32 coarse = 0;
    Int32 fe_err = 0;
    Int32 fe_result;
    Int32 step = 1;
    GP_WB_WRITE_BBPLL_CLK_192M_VALID(0);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_FOOR_ENA(0);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_POOL_ENA(0);
    GP_WB_WRITE_BBPLL_LOOP_IS_CLOSED(0);
    GP_WB_WRITE_BBPLL_FREQ_OUT_OF_RANGE_FE_MAX((768+(4*4)));
    GP_WB_WRITE_BBPLL_FREQ_OUT_OF_RANGE_FE_MIN((768-(4*4)));
    GP_WB_WRITE_BBPLL_F_ESTIMATION_DURATION(4);
    coarse = GP_WB_READ_BBPLL_DCO_COARSE();
    coarse = coarse & 31;
    rap_h_wait_us(2);
    GP_WB_BBPLL_START_FE();
    rap_h_wait_us(4);
    fe_result = GP_WB_READ_BBPLL_FE_RESULT();
    fe_err = fe_result - 768;
    if (fe_result >= 768)
    {
        step = -1;
    }
    if (coarse == 0)
    {
        step = +1;
    }
    if (coarse == 31)
    {
        step = -1;
    }
    while (true)
    {
        coarse = coarse + step;
        GP_WB_WRITE_BBPLL_DCO_COARSE_NEW(coarse);
        rap_h_wait_us(2);
        GP_WB_BBPLL_START_FE();
        rap_h_wait_us(4);
        fe_result = GP_WB_READ_BBPLL_FE_RESULT();
        if (step == +1)
        {
            if ((fe_result >= 768) || (coarse == 31))
            {
                break;
            }
        }
        else
        {
            if ((fe_result <= 768) || (coarse == 0))
            {
                break;
            }
        }
        fe_err = fe_result - 768;
    }
    if (abs(fe_result - 768) > abs(fe_err))
    {
        coarse = coarse - step;
        GP_WB_WRITE_BBPLL_DCO_COARSE_NEW(coarse);
        rap_h_wait_us(2);
    }
}

void rap_bbpll_close_loop(void)
{
    GP_WB_WRITE_BBPLL_BBPLL_BIAS_FILT_BYPASS(0);
    rap_h_wait_us(10);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_FOOR_ENA(0);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_POOL_ENA(0);
    GP_WB_WRITE_BBPLL_LOOP_ALLOW_FOR_FREQ_OUT_OF_RANGE_COARSE_UPDATE(1);
    GP_WB_WRITE_BBPLL_LOOP_ALLOW_FOR_TUNE_OUT_OF_RANGE_COARSE_UPDATE(1);
    GP_WB_WRITE_BBPLL_LOOP_IS_CLOSED(1);
    GP_WB_BBPLL_CLR_IN_LOCK();
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_BBPLL_UNMASKED_IN_LOCK_INTERRUPT() != 1), 10000);
    GP_WB_BBPLL_CLR_IN_LOCK();

    rap_h_wait_us(2);

    GP_WB_BBPLL_CLR_FREQ_OUT_OF_RANGE_LO();
    GP_WB_BBPLL_CLR_FREQ_OUT_OF_RANGE_HI();
    GP_WB_BBPLL_CLR_FREQ_OUT_OF_RANGE_TIMEOUT();
    GP_WB_BBPLL_CLR_OUT_OF_LOCK();

    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_FOOR_ENA(1);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_POOL_ENA(0);
    GP_WB_BBPLL_CLR_COARSE_OUT_OF_RANGE_HI();
    GP_WB_BBPLL_CLR_COARSE_OUT_OF_RANGE_LO();
    GP_WB_BBPLL_CLR_TUNE_OUT_OF_RANGE_HI();
    GP_WB_BBPLL_CLR_TUNE_OUT_OF_RANGE_LO();
    GP_WB_BBPLL_CLR_FREQ_OUT_OF_RANGE_CNT();
    GP_WB_BBPLL_CLR_TUNE_OUT_OF_RANGE_CNT();
    GP_WB_WRITE_BBPLL_CLK_192M_VALID(1);
    GP_WB_STANDBY_RESET_BLOCK_SYNC();
}

void rap_bbpll_start(void)
{
    GP_WB_WRITE_PMUD_CLK_32M_PUP(1);
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_PMUD_CLK_32M_RDY() != 1), 10000);
    rap_bbpll_pup();
    rap_bbpll_calibrate();
    rap_bbpll_close_loop();
}

void rap_bbpll_stop(void)
{
    GP_WB_WRITE_PMUD_CLK_29M_PUP(1);
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_PMUD_CLK_29M_RDY() != 1), 10000);
    GP_WB_WRITE_BBPLL_CLK_192M_VALID(0);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_FOOR_ENA(0);
    GP_WB_WRITE_STANDBY_SOFT_POR_BY_BBPLL_POOL_ENA(0);
    GP_WB_WRITE_BBPLL_LOOP_IS_CLOSED(0);
    GP_WB_WRITE_BBPLL_BBPLL_RING_OSC_PUP(0);
    GP_WB_WRITE_BBPLL_BBPLL_RING_BIAS_PUP(0);
    GP_WB_WRITE_BBPLL_BBPLL_LDO_PUP(0);
}

void rap_bbpll_resume(void)
{
    GP_WB_WRITE_PMUD_CLK_32M_PUP(1);
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_PMUD_CLK_32M_RDY() != 1), 10000);
    rap_bbpll_pup();
    rap_bbpll_close_loop();
}

void rap_bbpll_stop_and_use_xtal(void)
{
    GP_WB_WRITE_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE(0);
    rap_bbpll_stop();
    GP_WB_WRITE_PMUD_USE_XTAL_OSCILLATOR(1);
}

void rap_rt_ipc_init(void)
{
    GP_WB_WRITE_INT_CTRL_MASK_GPM_IPCX2GPM_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_IPCX2GPM_CMD_INTERRUPT(1);
}

void rap_rt_ipc_cmd_launch(UInt16 cmd, UInt16 args)
{
    GP_WB_IPC_CLR_GPM2X_CMD_PROCESSED_INTERRUPT();
    GP_WB_WRITE_RT_CMD_TYPE(cmd);
    GP_WB_WRITE_RT_CMD_ARG_0(args);
    GP_WB_WRITE_RT_CMD_RESULT(0);
    GP_WB_IPC_SET_X2GPM_CMD_INTERRUPT();
}

UInt16 rap_rt_ipc_cmd_join(void)
{
    UInt16 res;
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_IPC_UNMASKED_GPM2X_CMD_PROCESSED_INTERRUPT() != 1), 1000000);
    GP_WB_IPC_CLR_GPM2X_CMD_PROCESSED_INTERRUPT();
    res = GP_WB_READ_RT_CMD_RESULT();
    return res;
}

void rap_rt_ipc_cmd(UInt16 cmd, UInt16 args)
{
    UInt16 res;
    rap_rt_ipc_cmd_launch(cmd, 0);
    res = rap_rt_ipc_cmd_join();
    rap_h_assert_sys(res == cmd);
}


void rap_fll_load_fll_table_entry(UInt8 chidx)
{
    UInt16 preamble_thresh_bt_bup;
    GP_WB_WRITE_PLME_CHANNEL_PREAMBLE_DETECT_DISABLE((1<<(chidx)));
    preamble_thresh_bt_bup = GP_WB_READ_RX_PREAMBLE_THRESH_BT();
    GP_WB_WRITE_RX_PREAMBLE_THRESH_BT(65535);
    GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH((1<<(chidx)));
    GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH(0);
    GP_WB_WRITE_RX_PREAMBLE_THRESH_BT(preamble_thresh_bt_bup);
    GP_WB_WRITE_PLME_CHANNEL_PREAMBLE_DETECT_DISABLE(0);
}

void rap_fll_force_closed_loop(Bool tx)
{
    UInt8 dummy = 0;
    NOT_USED(dummy);

    GP_WB_WRITE_PLME_SIG_GEN_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_TX);
    GP_WB_WRITE_PLME_FLL_TX_NOT_RX_OVERRULE(tx);
    GP_WB_WRITE_PLME_FLL_TX_NOT_RX_OVERRULE_ENA(1);
    GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(1);
    GP_WB_PLME_SIG_GEN_START_SEQUENCE();
    dummy = GP_WB_READ_PLME_SIG_GEN_BUSY();
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_PLME_SIG_GEN_BUSY() != 0), 100);
    GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(0);
}

void rap_fll_force_off(void)
{
    UInt8 dummy = 0;
    NOT_USED(dummy);

    GP_WB_WRITE_PLME_FLL_TX_NOT_RX_OVERRULE_ENA(0);
    GP_WB_WRITE_PLME_SIG_GEN_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_TO_OFF);
    GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(1);
    GP_WB_PLME_SIG_GEN_START_SEQUENCE();
    dummy = GP_WB_READ_PLME_SIG_GEN_BUSY();
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_PLME_SIG_GEN_BUSY() != 0), 100);
    GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(0);
}

void rap_fll_set_channel_launch(UInt8 idx, UInt8 fll_channel)
{
    UInt8 is_ble_ch;
    UInt8 mgmt_granted;
    mgmt_granted = GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
    rap_h_assert_sys(mgmt_granted == 1);
    GP_WB_WRITE_RIB_CHANNEL_IDX(idx);
    GP_WB_WRITE_RIB_CHANNEL_NR(fll_channel);
    is_ble_ch = GP_WB_READ_RIB_IS_BLE_CH();
    is_ble_ch = is_ble_ch & ~ (1<<(idx));
    if (fll_channel > 15)
    {
        is_ble_ch = is_ble_ch | (1<<(idx));
    }
    GP_WB_WRITE_RIB_IS_BLE_CH(is_ble_ch);
    GP_WB_WRITE_RIB_START_TIMESTAMP_VALID(0);
    GP_WB_RIB_CHANNEL_CHANGE_REQUEST();
}

void rap_fll_set_channel_join(void)
{
    rap_h_wait_us(1);
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_RIB_TRC_STATE() == GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_RX), 200);
}

void rap_fll_set_channel(UInt8 idx, UInt8 fll_channel)
{
    rap_fll_set_channel_launch(idx, fll_channel);
    rap_fll_set_channel_join();
}



void rap_fll_cal_nrt(void)
{
    UInt8 mgmt_granted;
    mgmt_granted = GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
    rap_h_assert_sys(mgmt_granted == 1);
    rap_rt_ipc_cmd(GP_WB_ENUM_RT_CMD_TYPE_FLL_CAL_NRT, 0);
}



void rap_rx_zb_set_mode(UInt8 mode)
{

    switch (mode)
    {


        case RAP_RX_MODE_N:
        case RAP_RX_MODE_O:
        {

            GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_MCH);

            GP_WB_WRITE_PLME_FORCE_CHANNEL_TRANSITION_ON_EACH_SLOT(0);
            GP_WB_WRITE_RX_PREAMBLE_THRESH_LP(17);
            GP_WB_WRITE_PLME_ENABLE_CHANNEL_DISABLE_FEATURE(0);


            break;
        }
        case RAP_RX_MODE_N_AD:
        case RAP_RX_MODE_O_AD:
        {

            GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_MCH_AD);

            GP_WB_WRITE_PLME_FORCE_CHANNEL_TRANSITION_ON_EACH_SLOT(0);
            GP_WB_WRITE_RX_PREAMBLE_THRESH_LP(17);
            GP_WB_WRITE_RX_WAIT_SECOND_ANTENNA(1);
            GP_WB_WRITE_RX_USE_LONG_PREAMBLE_EVAL(1);
            GP_WB_WRITE_PLME_ENABLE_CHANNEL_DISABLE_FEATURE(0);



            break;
        }
        case RAP_RX_MODE_L:
        {

            GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_LPL);
            GP_WB_WRITE_PLME_DUTY_CYCLING_START_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_2);

            GP_WB_WRITE_RX_PREAMBLE_THRESH_LP(17);
            GP_WB_WRITE_PLME_ENABLE_CHANNEL_DISABLE_FEATURE(0);


            break;
        }
        case RAP_RX_MODE_L_AD:
        {

            GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_LPL_AD);
            GP_WB_WRITE_PLME_DUTY_CYCLING_START_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_1);

            GP_WB_WRITE_RX_PREAMBLE_THRESH_LP(17);
            GP_WB_WRITE_RX_WAIT_SECOND_ANTENNA(1);
            GP_WB_WRITE_RX_USE_LONG_PREAMBLE_EVAL(0);
            GP_WB_WRITE_PLME_ENABLE_CHANNEL_DISABLE_FEATURE(0);



            break;
        }
        case RAP_RX_MODE_M:
        case RAP_RX_MODE_MB:
        case RAP_RX_MODE_MBXZB:
        {

            GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_MCH);

            GP_WB_WRITE_PLME_FORCE_CHANNEL_TRANSITION_ON_EACH_SLOT(1);
            GP_WB_WRITE_RX_PREAMBLE_THRESH_LP(15);
            GP_WB_WRITE_PLME_ENABLE_CHANNEL_DISABLE_FEATURE(1);


            break;
        }
        case RAP_RX_MODE_M_AD:
        {

            GP_WB_WRITE_PLME_RX_MODE_ZB(GP_WB_ENUM_ZB_RECEIVER_MODE_MCH_AD);

            GP_WB_WRITE_PLME_FORCE_CHANNEL_TRANSITION_ON_EACH_SLOT(1);
            GP_WB_WRITE_RX_PREAMBLE_THRESH_LP(17);
            GP_WB_WRITE_RX_WAIT_SECOND_ANTENNA(0);
            GP_WB_WRITE_RX_USE_LONG_PREAMBLE_EVAL(0);
            GP_WB_WRITE_PLME_ENABLE_CHANNEL_DISABLE_FEATURE(1);



            break;
        }
        default:
        {

            break;
        }
    }

}


void rap_rx_ble_set_mode(UInt8 mode)
{

    switch (mode)
    {


        case RAP_RX_MODE_Ba:
        case RAP_RX_MODE_B:
        {

            GP_WB_WRITE_PLME_RX_MODE_BLE(GP_WB_ENUM_BLE_RECEIVER_MODE_BLE);


            GP_WB_WRITE_RX_BT_TIMEOUT_QUARD_TIMER(9);
            GP_WB_WRITE_RX_TTRACK_THR(4);

            GP_WB_WRITE_RX_BT_DATA_VALID_ENABLE(1);


            break;
        }
        case RAP_RX_MODE_Ha:
        case RAP_RX_MODE_H:
        {

            GP_WB_WRITE_PLME_RX_MODE_BLE(GP_WB_ENUM_BLE_RECEIVER_MODE_BLE_HDR);


            GP_WB_WRITE_RX_BT_TIMEOUT_QUARD_TIMER(9);
            GP_WB_WRITE_RX_TTRACK_THR(6);

            GP_WB_WRITE_RX_BT_DATA_VALID_ENABLE(0);



            break;
        }
        case RAP_RX_MODE_MB:
        {

            GP_WB_WRITE_PLME_RX_MODE_BLE(GP_WB_ENUM_BLE_RECEIVER_MODE_BLE);

            GP_WB_WRITE_RX_BLE_TIMEOUT_VALUE(57);
            GP_WB_WRITE_RX_BLE_TIMEOUT_VALUE_LONG(83);
            GP_WB_WRITE_RX_BLE_TIMEOUT_VALUE_PIP(65);
            GP_WB_WRITE_RX_HOLD_BLE_TIMEOUT_COUNTER_EN(0);

            GP_WB_WRITE_RX_BT_TIMEOUT_QUARD_TIMER(9);
            GP_WB_WRITE_RX_TTRACK_THR(4);

            GP_WB_WRITE_RX_BT_DATA_VALID_ENABLE(1);


            break;
        }
        default:
        {

            break;
        }
    }

}


void rap_rx_set_mode(UInt8 mode)
{
    rap_rx_zb_set_mode(mode);
    rap_rx_ble_set_mode(mode);

}

#define RAP_RX_MODE_N (0)
#define RAP_RX_MODE_N_AD (1)
#define RAP_RX_MODE_L (2)
#define RAP_RX_MODE_L_AD (3)
#define RAP_RX_MODE_M (4)
#define RAP_RX_MODE_M_AD (5)
#define RAP_RX_MODE_Bc (6)
#define RAP_RX_MODE_Ba (7)
#define RAP_RX_MODE_B (8)
#define RAP_RX_MODE_Be (9)
#define RAP_RX_MODE_Hc (10)
#define RAP_RX_MODE_Ha (11)
#define RAP_RX_MODE_H (12)
#define RAP_RX_MODE_He (13)
#define RAP_RX_MODE_MB (14)
#define RAP_RX_MODE_O (15)
#define RAP_RX_MODE_O_AD (16)
#define RAP_RX_MODE_MBXZB (17)


UInt8 rap_sys_get_chip_version(void)
{
    UInt16 version;
    UInt8 chip_version;
    version = GP_WB_READ_STANDBY_VERSION();
    chip_version = version & 0x003f;
    return chip_version;
}

void rap_sys_reset_early(void)
{

    GP_WB_WRITE_PMUD_PMU_BG_LPF(1);

}

void rap_sys_backup_late(void)
{


}

void rap_sys_hal_init(void)
{
    Int8 rssi_offset_a;
    Int8 rssi_offset_b;

    GP_WB_WRITE_RX_RSSI_OFFSET_LOW_ATT(25);
    GP_WB_WRITE_RX_RSSI_OFFSET_ATT(38);
    rssi_offset_a = GP_WB_READ_RX_RSSI_OFFSET_A();
    rssi_offset_b = GP_WB_READ_RX_RSSI_OFFSET_B();
    GP_WB_WRITE_RX_RSSI_OFFSET_FS_MODE_NO_ATT(0-rssi_offset_a+rssi_offset_b);
    GP_WB_WRITE_RX_RSSI_OFFSET_FS_MODE_LOW_ATT(12-rssi_offset_a+rssi_offset_b);
    GP_WB_WRITE_RX_RSSI_OFFSET_FS_MODE_ATT(21-rssi_offset_a+rssi_offset_b);

    GP_WB_WRITE_RX_LNA_AGC_ENABLE_ATT_DURING_TIMEOUT(0);

    GP_WB_WRITE_ADCIF_ADC_OVP_PUP(0);
    GP_WB_WRITE_PMUD_PMU_VDDRAM_TUNE(-1);
    GP_WB_WRITE_PMUD_PMU_VMT_1K(1);


}



void rap_tx_force_pa_on(UInt8 power, Bool pa_low, Bool pa_ultralow)
{
    UInt8 pa_biastrim_mult;
    GP_WB_WRITE_RADIOITF_TX_POWER_OVERRULE(power);
    if (pa_ultralow)
    {
        GP_WB_WRITE_RADIOITF_TX_RF_P_PUP_OVERRULE(0);
    }
    else
    {
        GP_WB_WRITE_RADIOITF_TX_RF_P_PUP_OVERRULE(1);
    }
    GP_WB_WRITE_RADIOITF_TX_RF_N_PUP_OVERRULE(1);
    if (pa_low || pa_ultralow)
    {
        GP_WB_WRITE_RADIOITF_TX_PA_BIASTRIM_MULT_OVERRULE(2);
    }
    else
    {
        pa_biastrim_mult = GP_WB_READ_TX_TX_PA_BIASTRIM_MULT();
        GP_WB_WRITE_RADIOITF_TX_PA_BIASTRIM_MULT_OVERRULE(pa_biastrim_mult);
    }
    GP_WB_WRITE_RADIOITF_TX_POWER_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_TX_RF_P_PUP_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_TX_RF_N_PUP_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_TX_PA_BIASTRIM_MULT_OVERRULE_ENA(1);
    GP_WB_WRITE_TX_EN_TX_PA_ON(1);
}

void rap_tx_unforce_pa_on(void)
{
    GP_WB_WRITE_TX_EN_TX_PA_ON(0);
    GP_WB_WRITE_RADIOITF_TX_POWER_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_TX_RF_P_PUP_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_TX_RF_N_PUP_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_TX_PA_BIASTRIM_MULT_OVERRULE_ENA(0);
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_RADIOITF_TX_RF_N_PUP_STATUS() != 0), 100);
}

void rap_tx_force_antenna(Bool antenna)
{
    UInt8 match_tx_tune;
    UInt8 match_ant1_select;
    UInt8 match_ant2_select;
    UInt8 match_ant1_tune;
    UInt8 match_ant2_tune;
    UInt8 match_ant1_rload;
    UInt8 match_ant2_rload;
    if (antenna)
    {
        match_tx_tune = GP_WB_READ_TRX_TX_ANT1_MATCH_TX_TUNE();
        match_ant1_select = GP_WB_READ_TRX_TX_ANT1_MATCH_ANT1_SELECT();
        match_ant2_select = GP_WB_READ_TRX_TX_ANT1_MATCH_ANT2_SELECT();
        match_ant1_tune = GP_WB_READ_TRX_TX_ANT1_MATCH_ANT1_TUNE();
        match_ant2_tune = GP_WB_READ_TRX_TX_ANT1_MATCH_ANT2_TUNE();
        match_ant1_rload = GP_WB_READ_TRX_TX_ANT1_MATCH_ANT1_RLOAD();
        match_ant2_rload = GP_WB_READ_TRX_TX_ANT1_MATCH_ANT2_RLOAD();
    }
    else
    {
        match_tx_tune = GP_WB_READ_TRX_TX_ANT0_MATCH_TX_TUNE();
        match_ant1_select = GP_WB_READ_TRX_TX_ANT0_MATCH_ANT1_SELECT();
        match_ant2_select = GP_WB_READ_TRX_TX_ANT0_MATCH_ANT2_SELECT();
        match_ant1_tune = GP_WB_READ_TRX_TX_ANT0_MATCH_ANT1_TUNE();
        match_ant2_tune = GP_WB_READ_TRX_TX_ANT0_MATCH_ANT2_TUNE();
        match_ant1_rload = GP_WB_READ_TRX_TX_ANT0_MATCH_ANT1_RLOAD();
        match_ant2_rload = GP_WB_READ_TRX_TX_ANT0_MATCH_ANT2_RLOAD();
    }
    GP_WB_WRITE_RADIOITF_MATCH_TX_TUNE_OVERRULE(match_tx_tune);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_SELECT_OVERRULE(match_ant1_select);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_SELECT_OVERRULE(match_ant2_select);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_TUNE_OVERRULE(match_ant1_tune);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_TUNE_OVERRULE(match_ant2_tune);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_RLOAD_OVERRULE(match_ant1_rload);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_RLOAD_OVERRULE(match_ant2_rload);
    GP_WB_WRITE_RADIOITF_MATCH_TX_TUNE_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_SELECT_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_SELECT_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_TUNE_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_TUNE_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_RLOAD_OVERRULE_ENA(1);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_RLOAD_OVERRULE_ENA(1);
}

void rap_tx_unforce_antenna(void)
{
    GP_WB_WRITE_RADIOITF_MATCH_TX_TUNE_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_SELECT_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_SELECT_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_TUNE_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_TUNE_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_MATCH_ANT1_RLOAD_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_MATCH_ANT2_RLOAD_OVERRULE_ENA(0);
}

void rap_tx_cw_enable(UInt8 chidx, UInt8 power, Bool pa_low, Bool pa_ultralow, Bool antenna, Bool modulated, Bool tx)
{
    UInt8 is_ble_ch;
    Bool is_ble;
    UInt8 mgmt_granted;
    mgmt_granted = GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
    rap_h_assert_sys(mgmt_granted == 1);
    is_ble_ch = GP_WB_READ_RIB_IS_BLE_CH();
    is_ble = (is_ble_ch & (1<<(chidx))) != 0;
    rap_fll_load_fll_table_entry(chidx);
    rap_fll_force_closed_loop(tx);
    rap_tx_force_antenna(antenna);
    GP_WB_WRITE_RIB_MGMT_PAT_SELECT_BLE(is_ble);
    GP_WB_WRITE_TX_CONT_MOD_BLE_MODE(is_ble);
    GP_WB_WRITE_TX_EN_CONT_MOD(modulated);
    rap_tx_force_pa_on(power, pa_low, pa_ultralow);
}

void rap_tx_cw_disable(void)
{
    UInt8 mgmt_granted;
    mgmt_granted = GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
    rap_h_assert_sys(mgmt_granted == 1);
    GP_WB_WRITE_TX_EN_CONT_MOD(0);
    rap_tx_unforce_pa_on();
    rap_tx_unforce_antenna();
    rap_fll_force_off();
}

void rap_trx_arb_mgmt_claim(void)
{
    UInt8 mgmt_granted;
    mgmt_granted = GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
    rap_h_assert_sys(mgmt_granted != 1);
    GP_WB_WRITE_RADIO_ARB_MGMT_CLAIM(1);
    GP_DO_WHILE_TIMEOUT_ASSERT((GP_WB_READ_RADIO_ARB_MGMT_GRANTED() != 1), 1000000);
}

void rap_trx_arb_mgmt_release(void)
{
    UInt8 mgmt_granted;
    mgmt_granted = GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
    rap_h_assert_sys(mgmt_granted == 1);
    GP_WB_WRITE_RADIO_ARB_MGMT_CLAIM(0);
}

