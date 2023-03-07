/***************************************************************************//**
 * @file
 * @brief EFP01 (Energy Friendly PMIC) register definitions.
 * @version 5.7.0
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_EFP01_H
#define SL_EFP01_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup sl_efp
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** EFP01 OTP Register definitions. */
#define EFP01_OTP_I2C_CTRL                             0x00
#define EFP01_OTP_CC_CTRL                              0x01
#define EFP01_OTP_EM_CRSREG_CTRL                       0x03
#define EFP01_OTP_VOA_V                                0x09
#define EFP01_OTP_VOC_V                                0x0B
#define EFP01_OTP_VOB_EM0_V                            0x0C
#define EFP01_OTP_VOB_EM2_V                            0x0D
#define EFP01_OTP_BIAS_SW                              0x0E
#define EFP01_OTP_BB_IPK                               0x0F
#define EFP01_OTP_BB_CTRL3                             0x10
#define EFP01_OTP_BB_CTRL5                             0x11
#define EFP01_OTP_BB_CTRL6                             0x12
#define EFP01_OTP_LDOB_CTRL                            0x13
#define EFP01_OTP_LDOC_CTRL                            0x14
#define EFP01_OTP_LDOC_BB_CTRL                         0x15
#define EFP01_OTP_BK_CTRL0                             0x16
#define EFP01_OTP_BK_CTRL1                             0x17
#define EFP01_OTP_BK_IPK                               0x18
#define EFP01_OTP_BK_CTRL2                             0x19
#define EFP01_OTP_ADC_CC_CTRL                          0x1C
#define EFP01_OTP_ADC_LIMITS                           0x1D
#define EFP01_OTP_UID_LOW                              0x20
#define EFP01_OTP_UID_HIGH                             0x21
#define EFP01_OTP_TEMP_CODE                            0x2F
#define EFP01_OTP_OPN_NUM                              0x30
#define EFP01_OTP_OPN_REV                              0x31
#define EFP01_OTP_CCCAL_LOW                            0x33
#define EFP01_OTP_CCCAL_HIGH                           0x34
#define EFP01_OTP_OSC10MHZ_CAL                         0x35

/** EFP01 Register definitions. */
#define EFP01_STATUS_G                                 0x00
#define EFP01_STATUS_V                                 0x01
#define EFP01_STATUS_GM                                0x02
#define EFP01_STATUS_VM                                0x03
#define EFP01_STATUS_LIVE                              0x04
#define EFP01_DEVREV_ID                                0x05
#define EFP01_CC_CAL                                   0x0A
#define EFP01_OTP_ADDR                                 0x0B
#define EFP01_OTP_RDATA                                0x0C
#define EFP01_OTP_STATCTRL                             0x0E
#define EFP01_CMD                                      0x10
#define EFP01_CC_MODE                                  0x12
#define EFP01_CCA_MSBY                                 0x13
#define EFP01_CCA_LSBY                                 0x14
#define EFP01_CCB0_MSBY                                0x15
#define EFP01_CCB0_LSBY                                0x16
#define EFP01_CCB2_MSBY                                0x17
#define EFP01_CCB2_LSBY                                0x18
#define EFP01_CCC_MSBY                                 0x19
#define EFP01_CCC_LSBY                                 0x1A
#define EFP01_VDD_AVG_MSN                              0x1B
#define EFP01_VDD_AVG_LSBY                             0x1C
#define EFP01_VDD_MIN_MSN                              0x1D
#define EFP01_VDD_MIN_LSBY                             0x1E
#define EFP01_VDD_MAX_MSN                              0x1F
#define EFP01_VDD_MAX_LSBY                             0x20
#define EFP01_TEMP_MSN                                 0x21
#define EFP01_TEMP_LSBY                                0x22
#define EFP01_VOA_SW_STAT                              0x23
#define EFP01_I2C_CTRL                                 0x40
#define EFP01_CC_CTRL                                  0x41
#define EFP01_EM_CRSREG_CTRL                           0x43
#define EFP01_VOA_V                                    0x49
#define EFP01_VOC_V                                    0x4B
#define EFP01_VOB_EM0_V                                0x4C
#define EFP01_VOB_EM2_V                                0x4D
#define EFP01_BIAS_SW                                  0x4E
#define EFP01_BB_IPK                                   0x4F
#define EFP01_BB_CTRL3                                 0x50
#define EFP01_BB_CTRL5                                 0x51
#define EFP01_BB_CTRL6                                 0x52
#define EFP01_LDOB_CTRL                                0x53
#define EFP01_LDOC_CTRL                                0x54
#define EFP01_LDOC_BB_CTRL                             0x55
#define EFP01_BK_CTRL0                                 0x56
#define EFP01_BK_CTRL1                                 0x57
#define EFP01_BK_IPK                                   0x58
#define EFP01_BK_CTRL2                                 0x59
#define EFP01_ADC_CC_CTRL                              0x5C
#define EFP01_ADC_LIMITS                               0x5D

/** Bit fields for EFP01_STATUS_G */
#define _EFP01_STATUS_G_ADDRESS                        0x00
#define _EFP01_STATUS_G_RESETVALUE                     0x00
#define _EFP01_STATUS_G_OTP_READ_SHIFT                 0
#define _EFP01_STATUS_G_OTP_READ_MASK                  0x01
#define _EFP01_STATUS_G_OTP_CRC_FAULT_SHIFT            1
#define _EFP01_STATUS_G_OTP_CRC_FAULT_MASK             0x02
#define _EFP01_STATUS_G_I2C_FAULT_SHIFT                2
#define _EFP01_STATUS_G_I2C_FAULT_MASK                 0x04
#define _EFP01_STATUS_G_TEMP_FAULT_SHIFT               3
#define _EFP01_STATUS_G_TEMP_FAULT_MASK                0x08
#define _EFP01_STATUS_G_VOLT_FAULT_SHIFT               4
#define _EFP01_STATUS_G_VOLT_FAULT_MASK                0x10
#define _EFP01_STATUS_G_CC_FULL_SHIFT                  5
#define _EFP01_STATUS_G_CC_FULL_MASK                   0x20
#define _EFP01_STATUS_G_CCC_ISDONE_SHIFT               6
#define _EFP01_STATUS_G_CCC_ISDONE_MASK                0x40
#define _EFP01_STATUS_G_VDD_LOW_SHIFT                  7
#define _EFP01_STATUS_G_VDD_LOW_MASK                   0x80

/** Bit fields for EFP01_STATUS_V */
#define _EFP01_STATUS_V_ADDRESS                        0x01
#define _EFP01_STATUS_V_RESETVALUE                     0x00
#define _EFP01_STATUS_V_VOA_ISGOOD_SHIFT               0
#define _EFP01_STATUS_V_VOA_ISGOOD_MASK                0x01
#define _EFP01_STATUS_V_VOB_ISGOOD_SHIFT               1
#define _EFP01_STATUS_V_VOB_ISGOOD_MASK                0x02
#define _EFP01_STATUS_V_VOC_ISGOOD_SHIFT               2
#define _EFP01_STATUS_V_VOC_ISGOOD_MASK                0x04
#define _EFP01_STATUS_V_VOA_ISLOW_SHIFT                3
#define _EFP01_STATUS_V_VOA_ISLOW_MASK                 0x08
#define _EFP01_STATUS_V_VOB_ISLOW_SHIFT                4
#define _EFP01_STATUS_V_VOB_ISLOW_MASK                 0x10
#define _EFP01_STATUS_V_VOC_ISLOW_SHIFT                5
#define _EFP01_STATUS_V_VOC_ISLOW_MASK                 0x20

/** Bit fields for EFP01_STATUS_GM */
#define _EFP01_STATUS_GM_ADDRESS                       0x02
#define _EFP01_STATUS_GM_RESETVALUE                    0x01
#define _EFP01_STATUS_GM_OTP_READ_UNMASK_SHIFT         0
#define _EFP01_STATUS_GM_OTP_READ_UNMASK_MASK          0x01
#define _EFP01_STATUS_GM_OTP_CRC_FAULT_UNMASK_SHIFT    1
#define _EFP01_STATUS_GM_OTP_CRC_FAULT_UNMASK_MASK     0x02
#define _EFP01_STATUS_GM_I2C_FAULT_UNMASK_SHIFT        2
#define _EFP01_STATUS_GM_I2C_FAULT_UNMASK_MASK         0x04
#define _EFP01_STATUS_GM_TEMP_FAULT_UNMASK_SHIFT       3
#define _EFP01_STATUS_GM_TEMP_FAULT_UNMASK_MASK        0x08
#define _EFP01_STATUS_GM_VOLT_FAULT_UNMASK_SHIFT       4
#define _EFP01_STATUS_GM_VOLT_FAULT_UNMASK_MASK        0x10
#define _EFP01_STATUS_GM_CC_FULL_UNMASK_SHIFT          5
#define _EFP01_STATUS_GM_CC_FULL_UNMASK_MASK           0x20
#define _EFP01_STATUS_GM_CCC_ISDONE_UNMASK_SHIFT       6
#define _EFP01_STATUS_GM_CCC_ISDONE_UNMASK_MASK        0x40
#define _EFP01_STATUS_GM_VDD_LOW_UNMASK_SHIFT          7
#define _EFP01_STATUS_GM_VDD_LOW_UNMASK_MASK           0x80

/** Bit fields for EFP01_STATUS_VM */
#define _EFP01_STATUS_VM_ADDRESS                       0x03
#define _EFP01_STATUS_VM_RESETVALUE                    0x00
#define _EFP01_STATUS_VM_VOA_ISGOOD_UNMASK_SHIFT       0
#define _EFP01_STATUS_VM_VOA_ISGOOD_UNMASK_MASK        0x01
#define _EFP01_STATUS_VM_VOB_ISGOOD_UNMASK_SHIFT       1
#define _EFP01_STATUS_VM_VOB_ISGOOD_UNMASK_MASK        0x02
#define _EFP01_STATUS_VM_VOC_ISGOOD_UNMASK_SHIFT       2
#define _EFP01_STATUS_VM_VOC_ISGOOD_UNMASK_MASK        0x04
#define _EFP01_STATUS_VM_VOA_ISLOW_UNMASK_SHIFT        3
#define _EFP01_STATUS_VM_VOA_ISLOW_UNMASK_MASK         0x08
#define _EFP01_STATUS_VM_VOB_ISLOW_UNMASK_SHIFT        4
#define _EFP01_STATUS_VM_VOB_ISLOW_UNMASK_MASK         0x10
#define _EFP01_STATUS_VM_VOC_ISLOW_UNMASK_SHIFT        5
#define _EFP01_STATUS_VM_VOC_ISLOW_UNMASK_MASK         0x20

/** Bit fields for EFP01_STATUS_LIVE */
#define _EFP01_STATUS_LIVE_ADDRESS                     0x04
#define _EFP01_STATUS_LIVE_RESETVALUE                  0x00
#define _EFP01_STATUS_LIVE_VOA_INREG_LIVE_SHIFT        0
#define _EFP01_STATUS_LIVE_VOA_INREG_LIVE_MASK         0x01
#define _EFP01_STATUS_LIVE_VOB_INREG_LIVE_SHIFT        1
#define _EFP01_STATUS_LIVE_VOB_INREG_LIVE_MASK         0x02
#define _EFP01_STATUS_LIVE_VOC_INREG_LIVE_SHIFT        2
#define _EFP01_STATUS_LIVE_VOC_INREG_LIVE_MASK         0x04
#define _EFP01_STATUS_LIVE_VDD_LOW_LIVE_SHIFT          3
#define _EFP01_STATUS_LIVE_VDD_LOW_LIVE_MASK           0x08
#define _EFP01_STATUS_LIVE_TEMP_HI_LIVE_SHIFT          4
#define _EFP01_STATUS_LIVE_TEMP_HI_LIVE_MASK           0x10
#define _EFP01_STATUS_LIVE_CCC_LIVE_SHIFT              5
#define _EFP01_STATUS_LIVE_CCC_LIVE_MASK               0x20

/** Bit fields for EFP01_DEVREV_ID */
#define _EFP01_DEVREV_ID_ADDRESS                       0x05
#define _EFP01_DEVREV_ID_RESETVALUE                    0x39
#define _EFP01_DEVREV_ID_DEV_ID_SHIFT                  0
#define _EFP01_DEVREV_ID_DEV_ID_MASK                   0x1F
#define _EFP01_DEVREV_ID_REV_ID_SHIFT                  5
#define _EFP01_DEVREV_ID_REV_ID_MASK                   0xE0

/** Bit fields for EFP01_CC_CAL */
#define _EFP01_CC_CAL_ADDRESS                          0x0A
#define _EFP01_CC_CAL_RESETVALUE                       0x00
#define _EFP01_CC_CAL_CCL_LVL_SHIFT                    0
#define _EFP01_CC_CAL_CCL_LVL_MASK                     0x07
#define _EFP01_CC_CAL_CCL_LVL_440UA                    0x00
#define EFP01_CC_CAL_CCL_LVL_440UA                     (_EFP01_CC_CAL_CCL_LVL_440UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_550UA                    0x01
#define EFP01_CC_CAL_CCL_LVL_550UA                     (_EFP01_CC_CAL_CCL_LVL_550UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_700UA                    0x02
#define EFP01_CC_CAL_CCL_LVL_700UA                     (_EFP01_CC_CAL_CCL_LVL_700UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_875UA                    0x03
#define EFP01_CC_CAL_CCL_LVL_875UA                     (_EFP01_CC_CAL_CCL_LVL_875UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_7060UA                   0x04
#define EFP01_CC_CAL_CCL_LVL_7060UA                    (_EFP01_CC_CAL_CCL_LVL_7060UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_8830UA                   0x05
#define EFP01_CC_CAL_CCL_LVL_8830UA                    (_EFP01_CC_CAL_CCL_LVL_8830UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_11200UA                  0x06
#define EFP01_CC_CAL_CCL_LVL_11200UA                   (_EFP01_CC_CAL_CCL_LVL_11200UA << 0)
#define _EFP01_CC_CAL_CCL_LVL_14000UA                  0x07
#define EFP01_CC_CAL_CCL_LVL_14000UA                   (_EFP01_CC_CAL_CCL_LVL_14000UA << 0)
#define _EFP01_CC_CAL_CCL_SEL_SHIFT                    3
#define _EFP01_CC_CAL_CCL_SEL_MASK                     0x18
#define _EFP01_CC_CAL_CCL_SEL_NONE                     0x00
#define EFP01_CC_CAL_CCL_SEL_NONE                      (_EFP01_CC_CAL_CCL_SEL_NONE << 3)
#define _EFP01_CC_CAL_CCL_SEL_VOA                      0x01
#define EFP01_CC_CAL_CCL_SEL_VOA                       (_EFP01_CC_CAL_CCL_SEL_VOA << 3)
#define _EFP01_CC_CAL_CCL_SEL_VOB                      0x02
#define EFP01_CC_CAL_CCL_SEL_VOB                       (_EFP01_CC_CAL_CCL_SEL_VOB << 3)
#define _EFP01_CC_CAL_CCL_SEL_VOC                      0x03
#define EFP01_CC_CAL_CCL_SEL_VOC                       (_EFP01_CC_CAL_CCL_SEL_VOC << 3)
#define _EFP01_CC_CAL_CC_CAL_NREQ_SHIFT                5
#define _EFP01_CC_CAL_CC_CAL_NREQ_MASK                 0xE0

/** Bit fields for EFP01_OTP_ADDR */
#define _EFP01_OTP_ADDR_ADDRESS                        0x0B
#define _EFP01_OTP_ADDR_RESETVALUE                     0x00
#define _EFP01_OTP_ADDR_OTP_ADDR_SHIFT                 0
#define _EFP01_OTP_ADDR_OTP_ADDR_MASK                  0x7F

/** Bit fields for EFP01_OTP_RDATA */
#define _EFP01_OTP_RDATA_ADDRESS                       0x0C
#define _EFP01_OTP_RDATA_RESETVALUE                    0x00
#define _EFP01_OTP_RDATA_OTP_RDATA_SHIFT               0
#define _EFP01_OTP_RDATA_OTP_RDATA_MASK                0xFF

/** Bit fields for EFP01_OTP_STATCTRL */
#define _EFP01_OTP_STATCTRL_ADDRESS                    0x0E
#define _EFP01_OTP_STATCTRL_RESETVALUE                 0x00
#define _EFP01_OTP_STATCTRL_CMD_READOTP_SHIFT          2
#define _EFP01_OTP_STATCTRL_CMD_READOTP_MASK           0x04
#define _EFP01_OTP_STATCTRL_OTP_BUSY_SHIFT             3
#define _EFP01_OTP_STATCTRL_OTP_BUSY_MASK              0x08

/** Bit fields for EFP01_CMD */
#define _EFP01_CMD_ADDRESS                             0x10
#define _EFP01_CMD_RESETVALUE                          0x00
#define _EFP01_CMD_RESET_SHIFT                         0
#define _EFP01_CMD_RESET_MASK                          0x01
#define _EFP01_CMD_OTP_REREAD_SHIFT                    2
#define _EFP01_CMD_OTP_REREAD_MASK                     0x04
#define _EFP01_CMD_CC_CAL_STRT_SHIFT                   4
#define _EFP01_CMD_CC_CAL_STRT_MASK                    0x10
#define _EFP01_CMD_CC_CLR_SHIFT                        5
#define _EFP01_CMD_CC_CLR_MASK                         0x20
#define _EFP01_CMD_ADC_CLR_SHIFT                       6
#define _EFP01_CMD_ADC_CLR_MASK                        0x40
#define _EFP01_CMD_ADC_START_SHIFT                     7
#define _EFP01_CMD_ADC_START_MASK                      0x80

/** Bit fields for EFP01_CC_MODE */
#define _EFP01_CC_MODE_ADDRESS                         0x12
#define _EFP01_CC_MODE_RESETVALUE                      0x00
#define _EFP01_CC_MODE_CCA_MODE_SHIFT                  0
#define _EFP01_CC_MODE_CCA_MODE_MASK                   0x03
#define _EFP01_CC_MODE_CCA_MODE_BUCK                   0x00
#define EFP01_CC_MODE_CCA_MODE_BUCK                    (_EFP01_CC_MODE_CCA_MODE_BUCK << 0)
#define _EFP01_CC_MODE_CCA_MODE_AUTO                   0x01
#define EFP01_CC_MODE_CCA_MODE_AUTO                    (_EFP01_CC_MODE_CCA_MODE_AUTO << 0)
#define _EFP01_CC_MODE_CCA_MODE_BOOST                  0x02
#define EFP01_CC_MODE_CCA_MODE_BOOST                   (_EFP01_CC_MODE_CCA_MODE_BOOST << 0)
#define _EFP01_CC_MODE_CCA_MODE_LDO                    0x03
#define EFP01_CC_MODE_CCA_MODE_LDO                     (_EFP01_CC_MODE_CCA_MODE_LDO << 0)
#define _EFP01_CC_MODE_CCB_MODE_SHIFT                  2
#define _EFP01_CC_MODE_CCB_MODE_MASK                   0x04
#define _EFP01_CC_MODE_CCB_MODE_BUCK                   0x00
#define EFP01_CC_MODE_CCB_MODE_BUCK                    (_EFP01_CC_MODE_CCB_MODE_BUCK << 2)
#define _EFP01_CC_MODE_CCB_MODE_LDOB                   0x01
#define EFP01_CC_MODE_CCB_MODE_LDOB                    (_EFP01_CC_MODE_CCB_MODE_LDOB << 2)
#define _EFP01_CC_MODE_SC_MODE_SHIFT                   3
#define _EFP01_CC_MODE_SC_MODE_MASK                    0x08

/** Bit fields for EFP01_CCA_MSBY */
#define _EFP01_CCA_MSBY_ADDRESS                        0x13
#define _EFP01_CCA_MSBY_RESETVALUE                     0x00
#define _EFP01_CCA_MSBY_CCA_MSBY_SHIFT                 0
#define _EFP01_CCA_MSBY_CCA_MSBY_MASK                  0xFF

/** Bit fields for EFP01_CCA_LSBY */
#define _EFP01_CCA_LSBY_ADDRESS                        0x14
#define _EFP01_CCA_LSBY_RESETVALUE                     0x00
#define _EFP01_CCA_LSBY_CCA_LSBY_SHIFT                 0
#define _EFP01_CCA_LSBY_CCA_LSBY_MASK                  0xFF

/** Bit fields for EFP01_CCB0_MSBY */
#define _EFP01_CCB0_MSBY_ADDRESS                       0x15
#define _EFP01_CCB0_MSBY_RESETVALUE                    0x00
#define _EFP01_CCB0_MSBY_CCB0_MSBY_SHIFT               0
#define _EFP01_CCB0_MSBY_CCB0_MSBY_MASK                0xFF

/** Bit fields for EFP01_CCB0_LSBY */
#define _EFP01_CCB0_LSBY_ADDRESS                       0x16
#define _EFP01_CCB0_LSBY_RESETVALUE                    0x00
#define _EFP01_CCB0_LSBY_CCB0_LSBY_SHIFT               0
#define _EFP01_CCB0_LSBY_CCB0_LSBY_MASK                0xFF

/** Bit fields for EFP01_CCB2_MSBY */
#define _EFP01_CCB2_MSBY_ADDRESS                       0x17
#define _EFP01_CCB2_MSBY_RESETVALUE                    0x00
#define _EFP01_CCB2_MSBY_CCB2_MSBY_SHIFT               0
#define _EFP01_CCB2_MSBY_CCB2_MSBY_MASK                0xFF

/** Bit fields for EFP01_CCB2_LSBY */
#define _EFP01_CCB2_LSBY_ADDRESS                       0x18
#define _EFP01_CCB2_LSBY_RESETVALUE                    0x00
#define _EFP01_CCB2_LSBY_CCB2_LSBY_SHIFT               0
#define _EFP01_CCB2_LSBY_CCB2_LSBY_MASK                0xFF

/** Bit fields for EFP01_CCC_MSBY */
#define _EFP01_CCC_MSBY_ADDRESS                        0x19
#define _EFP01_CCC_MSBY_RESETVALUE                     0x00
#define _EFP01_CCC_MSBY_CCC_MSBY_SHIFT                 0
#define _EFP01_CCC_MSBY_CCC_MSBY_MASK                  0xFF

/** Bit fields for EFP01_CCC_LSBY */
#define _EFP01_CCC_LSBY_ADDRESS                        0x1A
#define _EFP01_CCC_LSBY_RESETVALUE                     0x00
#define _EFP01_CCC_LSBY_CCC_LSBY_SHIFT                 0
#define _EFP01_CCC_LSBY_CCC_LSBY_MASK                  0xFF

/** Bit fields for EFP01_VDD_AVG_MSN */
#define _EFP01_VDD_AVG_MSN_ADDRESS                     0x1B
#define _EFP01_VDD_AVG_MSN_RESETVALUE                  0x00
#define _EFP01_VDD_AVG_MSN_VDD_AVG_MSN_SHIFT           0
#define _EFP01_VDD_AVG_MSN_VDD_AVG_MSN_MASK            0x0F

/** Bit fields for EFP01_VDD_AVG_LSBY */
#define _EFP01_VDD_AVG_LSBY_ADDRESS                    0x1C
#define _EFP01_VDD_AVG_LSBY_RESETVALUE                 0x00
#define _EFP01_VDD_AVG_LSBY_VDD_AVG_LSBY_SHIFT         0
#define _EFP01_VDD_AVG_LSBY_VDD_AVG_LSBY_MASK          0xFF

/** Bit fields for EFP01_VDD_MIN_MSN */
#define _EFP01_VDD_MIN_MSN_ADDRESS                     0x1D
#define _EFP01_VDD_MIN_MSN_RESETVALUE                  0x00
#define _EFP01_VDD_MIN_MSN_VDD_MIN_MSN_SHIFT           0
#define _EFP01_VDD_MIN_MSN_VDD_MIN_MSN_MASK            0x0F

/** Bit fields for EFP01_VDD_MIN_LSBY */
#define _EFP01_VDD_MIN_LSBY_ADDRESS                    0x1E
#define _EFP01_VDD_MIN_LSBY_RESETVALUE                 0x00
#define _EFP01_VDD_MIN_LSBY_VDD_MIN_LSBY_SHIFT         0
#define _EFP01_VDD_MIN_LSBY_VDD_MIN_LSBY_MASK          0xFF

/** Bit fields for EFP01_VDD_MAX_MSN */
#define _EFP01_VDD_MAX_MSN_ADDRESS                     0x1F
#define _EFP01_VDD_MAX_MSN_RESETVALUE                  0x00
#define _EFP01_VDD_MAX_MSN_VDD_MAX_MSN_SHIFT           0
#define _EFP01_VDD_MAX_MSN_VDD_MAX_MSN_MASK            0x0F

/** Bit fields for EFP01_VDD_MAX_LSBY */
#define _EFP01_VDD_MAX_LSBY_ADDRESS                    0x20
#define _EFP01_VDD_MAX_LSBY_RESETVALUE                 0x00
#define _EFP01_VDD_MAX_LSBY_VDD_MAX_LSBY_SHIFT         0
#define _EFP01_VDD_MAX_LSBY_VDD_MAX_LSBY_MASK          0xFF

/** Bit fields for EFP01_TEMP_MSN */
#define _EFP01_TEMP_MSN_ADDRESS                        0x21
#define _EFP01_TEMP_MSN_RESETVALUE                     0x00
#define _EFP01_TEMP_MSN_TEMP_MSN_SHIFT                 0
#define _EFP01_TEMP_MSN_TEMP_MSN_MASK                  0x0F

/** Bit fields for EFP01_TEMP_LSBY */
#define _EFP01_TEMP_LSBY_ADDRESS                       0x22
#define _EFP01_TEMP_LSBY_RESETVALUE                    0x00
#define _EFP01_TEMP_LSBY_TEMP_LSBY_SHIFT               0
#define _EFP01_TEMP_LSBY_TEMP_LSBY_MASK                0xFF

/** Bit fields for EFP01_VOA_SW_STAT */
#define _EFP01_VOA_SW_STAT_ADDRESS                     0x23
#define _EFP01_VOA_SW_STAT_RESETVALUE                  0x00
#define _EFP01_VOA_SW_STAT_VOA_SW_ISLOW_SHIFT          0
#define _EFP01_VOA_SW_STAT_VOA_SW_ISLOW_MASK           0x01

/** Bit fields for EFP01_I2C_CTRL */
#define _EFP01_I2C_CTRL_ADDRESS                        0x40
#define _EFP01_I2C_CTRL_RESETVALUE                     0x00
#define _EFP01_I2C_CTRL_I2C_PU_SHIFT                   0
#define _EFP01_I2C_CTRL_I2C_PU_MASK                    0x01

/** Bit fields for EFP01_CC_CTRL */
#define _EFP01_CC_CTRL_ADDRESS                         0x41
#define _EFP01_CC_CTRL_RESETVALUE                      0x00
#define _EFP01_CC_CTRL_CC_PRESCL_SHIFT                 2
#define _EFP01_CC_CTRL_CC_PRESCL_MASK                  0x0C
#define _EFP01_CC_CTRL_CC_EN_SHIFT                     4
#define _EFP01_CC_CTRL_CC_EN_MASK                      0x10
#define _EFP01_CC_CTRL_CC_THRSH_SHIFT                  5
#define _EFP01_CC_CTRL_CC_THRSH_MASK                   0x60
#define _EFP01_CC_CTRL_CC_THRSH_50PCT                  0x00
#define EFP01_CC_CTRL_CC_THRSH_50PCT                   (_EFP01_CC_CTRL_CC_THRSH_50PCT << 5)
#define _EFP01_CC_CTRL_CC_THRSH_62PCT                  0x01
#define EFP01_CC_CTRL_CC_THRSH_62PCT                   (_EFP01_CC_CTRL_CC_THRSH_62PCT << 5)
#define _EFP01_CC_CTRL_CC_THRSH_75PCT                  0x02
#define EFP01_CC_CTRL_CC_THRSH_75PCT                   (_EFP01_CC_CTRL_CC_THRSH_75PCT << 5)
#define _EFP01_CC_CTRL_CC_THRSH_87PCT                  0x03
#define EFP01_CC_CTRL_CC_THRSH_87PCT                   (_EFP01_CC_CTRL_CC_THRSH_87PCT << 5)

/** Bit fields for EFP01_EM_CRSREG_CTRL */
#define _EFP01_EM_CRSREG_CTRL_ADDRESS                  0x43
#define _EFP01_EM_CRSREG_CTRL_RESETVALUE               0x00
#define _EFP01_EM_CRSREG_CTRL_DIRECT_MODE_EN_SHIFT     0
#define _EFP01_EM_CRSREG_CTRL_DIRECT_MODE_EN_MASK      0x01
#define _EFP01_EM_CRSREG_CTRL_EM_SEL_SHIFT             1
#define _EFP01_EM_CRSREG_CTRL_EM_SEL_MASK              0x06
#define _EFP01_EM_CRSREG_CTRL_EM_SEL_EM0               0x00
#define EFP01_EM_CRSREG_CTRL_EM_SEL_EM0                (_EFP01_EM_CRSREG_CTRL_EM_SEL_EM0 << 1)
#define _EFP01_EM_CRSREG_CTRL_EM_SEL_RESERVED          0x01
#define EFP01_EM_CRSREG_CTRL_EM_SEL_RESERVED           (_EFP01_EM_CRSREG_CTRL_EM_SEL_RESERVED << 1)
#define _EFP01_EM_CRSREG_CTRL_EM_SEL_EM2               0x02
#define EFP01_EM_CRSREG_CTRL_EM_SEL_EM2                (_EFP01_EM_CRSREG_CTRL_EM_SEL_EM2 << 1)
#define _EFP01_EM_CRSREG_CTRL_EM_SEL_EM4               0x03
#define EFP01_EM_CRSREG_CTRL_EM_SEL_EM4                (_EFP01_EM_CRSREG_CTRL_EM_SEL_EM4 << 1)
#define _EFP01_EM_CRSREG_CTRL_FORCE_EM0_SHIFT          3
#define _EFP01_EM_CRSREG_CTRL_FORCE_EM0_MASK           0x08
#define _EFP01_EM_CRSREG_CTRL_CRSREG_BYP_SHIFT         4
#define _EFP01_EM_CRSREG_CTRL_CRSREG_BYP_MASK          0x10
#define _EFP01_EM_CRSREG_CTRL_CRSREG_EN_A_SHIFT        5
#define _EFP01_EM_CRSREG_CTRL_CRSREG_EN_A_MASK         0x20
#define _EFP01_EM_CRSREG_CTRL_CRSREG_EN_B_SHIFT        6
#define _EFP01_EM_CRSREG_CTRL_CRSREG_EN_B_MASK         0x40
#define _EFP01_EM_CRSREG_CTRL_CRSREG_EN_C_SHIFT        7
#define _EFP01_EM_CRSREG_CTRL_CRSREG_EN_C_MASK         0x80

/** Bit fields for EFP01_VOA_V */
#define _EFP01_VOA_V_ADDRESS                           0x49
#define _EFP01_VOA_V_RESETVALUE                        0x00
#define _EFP01_VOA_V_VOA_V_SHIFT                       0
#define _EFP01_VOA_V_VOA_V_MASK                        0x7F

/** Bit fields for EFP01_VOC_V */
#define _EFP01_VOC_V_ADDRESS                           0x4B
#define _EFP01_VOC_V_RESETVALUE                        0x00
#define _EFP01_VOC_V_VOC_V_SHIFT                       0
#define _EFP01_VOC_V_VOC_V_MASK                        0x3F

/** Bit fields for EFP01_VOB_EM0_V */
#define _EFP01_VOB_EM0_V_ADDRESS                       0x4C
#define _EFP01_VOB_EM0_V_RESETVALUE                    0x00
#define _EFP01_VOB_EM0_V_VOB_EM0_V_SHIFT               0
#define _EFP01_VOB_EM0_V_VOB_EM0_V_MASK                0x7F
#define _EFP01_VOB_EM0_V_OOR_DIS_SHIFT                 7
#define _EFP01_VOB_EM0_V_OOR_DIS_MASK                  0x80

/** Bit fields for EFP01_VOB_EM2_V */
#define _EFP01_VOB_EM2_V_ADDRESS                       0x4D
#define _EFP01_VOB_EM2_V_RESETVALUE                    0x00
#define _EFP01_VOB_EM2_V_VOB_EM2_V_SHIFT               0
#define _EFP01_VOB_EM2_V_VOB_EM2_V_MASK                0x7F

/** Bit fields for EFP01_BIAS_SW */
#define _EFP01_BIAS_SW_ADDRESS                         0x4E
#define _EFP01_BIAS_SW_RESETVALUE                      0x00
#define _EFP01_BIAS_SW_BIAS0_SHIFT                     0
#define _EFP01_BIAS_SW_BIAS0_MASK                      0x07
#define _EFP01_BIAS_SW_BIAS0_87NA                      0x00
#define EFP01_BIAS_SW_BIAS0_87NA                       (_EFP01_BIAS_SW_BIAS0_87NA << 0)
#define _EFP01_BIAS_SW_BIAS0_100NA                     0x01
#define EFP01_BIAS_SW_BIAS0_100NA                      (_EFP01_BIAS_SW_BIAS0_100NA << 0)
#define _EFP01_BIAS_SW_BIAS0_114NA                     0x02
#define EFP01_BIAS_SW_BIAS0_114NA                      (_EFP01_BIAS_SW_BIAS0_114NA << 0)
#define _EFP01_BIAS_SW_BIAS0_147NA                     0x03
#define EFP01_BIAS_SW_BIAS0_147NA                      (_EFP01_BIAS_SW_BIAS0_147NA << 0)
#define _EFP01_BIAS_SW_BIAS0_325NA                     0x04
#define EFP01_BIAS_SW_BIAS0_325NA                      (_EFP01_BIAS_SW_BIAS0_325NA << 0)
#define _EFP01_BIAS_SW_BIAS0_386NA                     0x05
#define EFP01_BIAS_SW_BIAS0_386NA                      (_EFP01_BIAS_SW_BIAS0_386NA << 0)
#define _EFP01_BIAS_SW_BIAS0_453NA                     0x06
#define EFP01_BIAS_SW_BIAS0_453NA                      (_EFP01_BIAS_SW_BIAS0_453NA << 0)
#define _EFP01_BIAS_SW_BIAS0_600NA                     0x07
#define EFP01_BIAS_SW_BIAS0_600NA                      (_EFP01_BIAS_SW_BIAS0_600NA << 0)
#define _EFP01_BIAS_SW_BIAS2_SHIFT                     3
#define _EFP01_BIAS_SW_BIAS2_MASK                      0x38
#define _EFP01_BIAS_SW_BIAS2_87NA                      0x00
#define EFP01_BIAS_SW_BIAS2_87NA                       (_EFP01_BIAS_SW_BIAS2_87NA << 3)
#define _EFP01_BIAS_SW_BIAS2_100NA                     0x01
#define EFP01_BIAS_SW_BIAS2_100NA                      (_EFP01_BIAS_SW_BIAS2_100NA << 3)
#define _EFP01_BIAS_SW_BIAS2_114NA                     0x02
#define EFP01_BIAS_SW_BIAS2_114NA                      (_EFP01_BIAS_SW_BIAS2_114NA << 3)
#define _EFP01_BIAS_SW_BIAS2_147NA                     0x03
#define EFP01_BIAS_SW_BIAS2_147NA                      (_EFP01_BIAS_SW_BIAS2_147NA << 3)
#define _EFP01_BIAS_SW_BIAS2_325NA                     0x04
#define EFP01_BIAS_SW_BIAS2_325NA                      (_EFP01_BIAS_SW_BIAS2_325NA << 3)
#define _EFP01_BIAS_SW_BIAS2_386NA                     0x05
#define EFP01_BIAS_SW_BIAS2_386NA                      (_EFP01_BIAS_SW_BIAS2_386NA << 3)
#define _EFP01_BIAS_SW_BIAS2_453NA                     0x06
#define EFP01_BIAS_SW_BIAS2_453NA                      (_EFP01_BIAS_SW_BIAS2_453NA << 3)
#define _EFP01_BIAS_SW_BIAS2_600NA                     0x07
#define EFP01_BIAS_SW_BIAS2_600NA                      (_EFP01_BIAS_SW_BIAS2_600NA << 3)
#define _EFP01_BIAS_SW_VOA_SW_CON_SHIFT                6
#define _EFP01_BIAS_SW_VOA_SW_CON_MASK                 0xC0
#define _EFP01_BIAS_SW_VOA_SW_CON_OFF                  0x00
#define EFP01_BIAS_SW_VOA_SW_CON_OFF                   (_EFP01_BIAS_SW_VOA_SW_CON_OFF << 6)
#define _EFP01_BIAS_SW_VOA_SW_CON_ON                   0x01
#define EFP01_BIAS_SW_VOA_SW_CON_ON                    (_EFP01_BIAS_SW_VOA_SW_CON_ON << 6)
#define _EFP01_BIAS_SW_VOA_SW_CON_ONINEM0              0x02
#define EFP01_BIAS_SW_VOA_SW_CON_ONINEM0               (_EFP01_BIAS_SW_VOA_SW_CON_ONINEM0 << 6)
#define _EFP01_BIAS_SW_VOA_SW_CON_ONINEM02             0x03
#define EFP01_BIAS_SW_VOA_SW_CON_ONINEM02              (_EFP01_BIAS_SW_VOA_SW_CON_ONINEM02 << 6)

/** Bit fields for EFP01_BB_IPK */
#define _EFP01_BB_IPK_ADDRESS                          0x4F
#define _EFP01_BB_IPK_RESETVALUE                       0x00
#define _EFP01_BB_IPK_BB_IPK_SHIFT                     0
#define _EFP01_BB_IPK_BB_IPK_MASK                      0x1F
#define _EFP01_BB_IPK_BB_IPK_EM2_SHIFT                 5
#define _EFP01_BB_IPK_BB_IPK_EM2_MASK                  0xE0

/** Bit fields for EFP01_BB_CTRL3 */
#define _EFP01_BB_CTRL3_ADDRESS                        0x50
#define _EFP01_BB_CTRL3_RESETVALUE                     0x00
#define _EFP01_BB_CTRL3_BB_MODE_SHIFT                  0
#define _EFP01_BB_CTRL3_BB_MODE_MASK                   0x07
#define _EFP01_BB_CTRL3_BB_MODE_DISABLED               0x00
#define EFP01_BB_CTRL3_BB_MODE_DISABLED                (_EFP01_BB_CTRL3_BB_MODE_DISABLED << 0)
#define _EFP01_BB_CTRL3_BB_MODE_AUTONOMOUS             0x01
#define EFP01_BB_CTRL3_BB_MODE_AUTONOMOUS              (_EFP01_BB_CTRL3_BB_MODE_AUTONOMOUS << 0)
#define _EFP01_BB_CTRL3_BB_MODE_FORCEBUCK              0x02
#define EFP01_BB_CTRL3_BB_MODE_FORCEBUCK               (_EFP01_BB_CTRL3_BB_MODE_FORCEBUCK << 0)
#define _EFP01_BB_CTRL3_BB_MODE_FORCENTM               0x03
#define EFP01_BB_CTRL3_BB_MODE_FORCENTM                (_EFP01_BB_CTRL3_BB_MODE_FORCENTM << 0)
#define _EFP01_BB_CTRL3_BB_MODE_FORCEBOOST             0x04
#define EFP01_BB_CTRL3_BB_MODE_FORCEBOOST              (_EFP01_BB_CTRL3_BB_MODE_FORCEBOOST << 0)
#define _EFP01_BB_CTRL3_BB_MODE_WIREDBUCKLDO           0x05
#define EFP01_BB_CTRL3_BB_MODE_WIREDBUCKLDO            (_EFP01_BB_CTRL3_BB_MODE_WIREDBUCKLDO << 0)
#define _EFP01_BB_CTRL3_BB_MODE_WIREDBUCK              0x06
#define EFP01_BB_CTRL3_BB_MODE_WIREDBUCK               (_EFP01_BB_CTRL3_BB_MODE_WIREDBUCK << 0)
#define _EFP01_BB_CTRL3_BB_MODE_WIREDBOOST             0x07
#define EFP01_BB_CTRL3_BB_MODE_WIREDBOOST              (_EFP01_BB_CTRL3_BB_MODE_WIREDBOOST << 0)
#define _EFP01_BB_CTRL3_NTM_DUR_SHIFT                  3
#define _EFP01_BB_CTRL3_NTM_DUR_MASK                   0x18
#define _EFP01_BB_CTRL3_NTM_DUR_60PCT                  0x00
#define EFP01_BB_CTRL3_NTM_DUR_60PCT                   (_EFP01_BB_CTRL3_NTM_DUR_60PCT << 3)
#define _EFP01_BB_CTRL3_NTM_DUR_83PCT                  0x01
#define EFP01_BB_CTRL3_NTM_DUR_83PCT                   (_EFP01_BB_CTRL3_NTM_DUR_83PCT << 3)
#define _EFP01_BB_CTRL3_NTM_DUR_107PCT                 0x02
#define EFP01_BB_CTRL3_NTM_DUR_107PCT                  (_EFP01_BB_CTRL3_NTM_DUR_107PCT << 3)
#define _EFP01_BB_CTRL3_NTM_DUR_130PCT                 0x03
#define EFP01_BB_CTRL3_NTM_DUR_130PCT                  (_EFP01_BB_CTRL3_NTM_DUR_130PCT << 3)
#define _EFP01_BB_CTRL3_NTM_LDO_THRSH_SHIFT            5
#define _EFP01_BB_CTRL3_NTM_LDO_THRSH_MASK             0xE0

/** Bit fields for EFP01_BB_CTRL5 */
#define _EFP01_BB_CTRL5_ADDRESS                        0x51
#define _EFP01_BB_CTRL5_RESETVALUE                     0x00
#define _EFP01_BB_CTRL5_BB_IPK_BOOST_ADJ_SHIFT         0
#define _EFP01_BB_CTRL5_BB_IPK_BOOST_ADJ_MASK          0x3F
#define _EFP01_BB_CTRL5_BB_DRVR_SPEED_SHIFT            6
#define _EFP01_BB_CTRL5_BB_DRVR_SPEED_MASK             0xC0

/** Bit fields for EFP01_BB_CTRL6 */
#define _EFP01_BB_CTRL6_ADDRESS                        0x52
#define _EFP01_BB_CTRL6_RESETVALUE                     0x00
#define _EFP01_BB_CTRL6_BB_TOFF_MAX_SHIFT              0
#define _EFP01_BB_CTRL6_BB_TOFF_MAX_MASK               0x03
#define _EFP01_BB_CTRL6_BB_TOFF_MAX_NOLIMIT            0x00
#define EFP01_BB_CTRL6_BB_TOFF_MAX_NOLIMIT             (_EFP01_BB_CTRL6_BB_TOFF_MAX_NOLIMIT << 0)
#define _EFP01_BB_CTRL6_BB_TOFF_MAX_0P9US              0x01
#define EFP01_BB_CTRL6_BB_TOFF_MAX_0P9US               (_EFP01_BB_CTRL6_BB_TOFF_MAX_0P9US << 0)
#define _EFP01_BB_CTRL6_BB_TOFF_MAX_1P5US              0x02
#define EFP01_BB_CTRL6_BB_TOFF_MAX_1P5US               (_EFP01_BB_CTRL6_BB_TOFF_MAX_1P5US << 0)
#define _EFP01_BB_CTRL6_BB_TOFF_MAX_2P1US              0x03
#define EFP01_BB_CTRL6_BB_TOFF_MAX_2P1US               (_EFP01_BB_CTRL6_BB_TOFF_MAX_2P1US << 0)
#define _EFP01_BB_CTRL6_BB_IRI_CON_SHIFT               2
#define _EFP01_BB_CTRL6_BB_IRI_CON_MASK                0x3C
#define _EFP01_BB_CTRL6_SW_FAST_SHIFT                  6
#define _EFP01_BB_CTRL6_SW_FAST_MASK                   0x40
#define _EFP01_BB_CTRL6_BB_IPK_NOADJ_SHIFT             7
#define _EFP01_BB_CTRL6_BB_IPK_NOADJ_MASK              0x80

/** Bit fields for EFP01_LDOB_CTRL */
#define _EFP01_LDOB_CTRL_ADDRESS                       0x53
#define _EFP01_LDOB_CTRL_RESETVALUE                    0x00
#define _EFP01_LDOB_CTRL_LDOB_IGAIN_SHIFT              0
#define _EFP01_LDOB_CTRL_LDOB_IGAIN_MASK               0x0F
#define _EFP01_LDOB_CTRL_LDOB_VMIN_SHIFT               4
#define _EFP01_LDOB_CTRL_LDOB_VMIN_MASK                0x30
#define _EFP01_LDOB_CTRL_LDOB_BYP_SHIFT                6
#define _EFP01_LDOB_CTRL_LDOB_BYP_MASK                 0x40
#define _EFP01_LDOB_CTRL_LDO_NO_AUTO_BYP_SHIFT         7
#define _EFP01_LDOB_CTRL_LDO_NO_AUTO_BYP_MASK          0x80

/** Bit fields for EFP01_LDOC_CTRL */
#define _EFP01_LDOC_CTRL_ADDRESS                       0x54
#define _EFP01_LDOC_CTRL_RESETVALUE                    0x00
#define _EFP01_LDOC_CTRL_LDOC_IGAIN_SHIFT              0
#define _EFP01_LDOC_CTRL_LDOC_IGAIN_MASK               0x0F
#define _EFP01_LDOC_CTRL_LDOC_VMIN_SHIFT               4
#define _EFP01_LDOC_CTRL_LDOC_VMIN_MASK                0x30
#define _EFP01_LDOC_CTRL_LDOC_BYP_SHIFT                6
#define _EFP01_LDOC_CTRL_LDOC_BYP_MASK                 0x40
#define _EFP01_LDOC_CTRL_LDOC_ENA_SA_SHIFT             7
#define _EFP01_LDOC_CTRL_LDOC_ENA_SA_MASK              0x80

/** Bit fields for EFP01_LDOC_BB_CTRL */
#define _EFP01_LDOC_BB_CTRL_ADDRESS                    0x55
#define _EFP01_LDOC_BB_CTRL_RESETVALUE                 0x00
#define _EFP01_LDOC_BB_CTRL_VOC_IRI_CON_SHIFT          0
#define _EFP01_LDOC_BB_CTRL_VOC_IRI_CON_MASK           0x0F
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_SHIFT           4
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_MASK            0x70
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_NOLIMIT         0x00
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_NOLIMIT          (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_NOLIMIT << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_350NS           0x01
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_350NS            (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_350NS << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_630NS           0x02
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_630NS            (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_630NS << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_910NS           0x03
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_910NS            (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_910NS << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_1190NS          0x04
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_1190NS           (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_1190NS << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_1470NS          0x05
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_1470NS           (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_1470NS << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_1750NS          0x06
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_1750NS           (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_1750NS << 4)
#define _EFP01_LDOC_BB_CTRL_BB_TON_MAX_2030NS          0x07
#define EFP01_LDOC_BB_CTRL_BB_TON_MAX_2030NS           (_EFP01_LDOC_BB_CTRL_BB_TON_MAX_2030NS << 4)
#define _EFP01_LDOC_BB_CTRL_SEQ_BB_FIRST_SHIFT         7
#define _EFP01_LDOC_BB_CTRL_SEQ_BB_FIRST_MASK          0x80

/** Bit fields for EFP01_BK_CTRL0 */
#define _EFP01_BK_CTRL0_ADDRESS                        0x56
#define _EFP01_BK_CTRL0_RESETVALUE                     0x00
#define _EFP01_BK_CTRL0_CLAMPB_SHIFT                   0
#define _EFP01_BK_CTRL0_CLAMPB_MASK                    0x01
#define _EFP01_BK_CTRL0_BK_DIS_OV_PROT_SHIFT           1
#define _EFP01_BK_CTRL0_BK_DIS_OV_PROT_MASK            0x02

/** Bit fields for EFP01_BK_CTRL1 */
#define _EFP01_BK_CTRL1_ADDRESS                        0x57
#define _EFP01_BK_CTRL1_RESETVALUE                     0x00
#define _EFP01_BK_CTRL1_BK_DRVR_SPEED_SHIFT            1
#define _EFP01_BK_CTRL1_BK_DRVR_SPEED_MASK             0x06
#define _EFP01_BK_CTRL1_BK_MODE_SHIFT                  3
#define _EFP01_BK_CTRL1_BK_MODE_MASK                   0x18
#define _EFP01_BK_CTRL1_BK_MODE_DISABLED               0x00
#define EFP01_BK_CTRL1_BK_MODE_DISABLED                (_EFP01_BK_CTRL1_BK_MODE_DISABLED << 3)
#define _EFP01_BK_CTRL1_BK_MODE_BUCKONLY               0x01
#define EFP01_BK_CTRL1_BK_MODE_BUCKONLY                (_EFP01_BK_CTRL1_BK_MODE_BUCKONLY << 3)
#define _EFP01_BK_CTRL1_BK_MODE_LDOONLY                0x02
#define EFP01_BK_CTRL1_BK_MODE_LDOONLY                 (_EFP01_BK_CTRL1_BK_MODE_LDOONLY << 3)
#define _EFP01_BK_CTRL1_BK_MODE_BUCKLDO                0x03
#define EFP01_BK_CTRL1_BK_MODE_BUCKLDO                 (_EFP01_BK_CTRL1_BK_MODE_BUCKLDO << 3)
#define _EFP01_BK_CTRL1_BK_TON_MAX_SHIFT               5
#define _EFP01_BK_CTRL1_BK_TON_MAX_MASK                0xE0
#define _EFP01_BK_CTRL1_BK_TON_MAX_NOLIMIT             0x00
#define EFP01_BK_CTRL1_BK_TON_MAX_NOLIMIT              (_EFP01_BK_CTRL1_BK_TON_MAX_NOLIMIT << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_350NS               0x01
#define EFP01_BK_CTRL1_BK_TON_MAX_350NS                (_EFP01_BK_CTRL1_BK_TON_MAX_350NS << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_630NS               0x02
#define EFP01_BK_CTRL1_BK_TON_MAX_630NS                (_EFP01_BK_CTRL1_BK_TON_MAX_630NS << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_910NS               0x03
#define EFP01_BK_CTRL1_BK_TON_MAX_910NS                (_EFP01_BK_CTRL1_BK_TON_MAX_910NS << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_1190NS              0x04
#define EFP01_BK_CTRL1_BK_TON_MAX_1190NS               (_EFP01_BK_CTRL1_BK_TON_MAX_1190NS << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_1470NS              0x05
#define EFP01_BK_CTRL1_BK_TON_MAX_1470NS               (_EFP01_BK_CTRL1_BK_TON_MAX_1470NS << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_1750NS              0x06
#define EFP01_BK_CTRL1_BK_TON_MAX_1750NS               (_EFP01_BK_CTRL1_BK_TON_MAX_1750NS << 5)
#define _EFP01_BK_CTRL1_BK_TON_MAX_2030NS              0x07
#define EFP01_BK_CTRL1_BK_TON_MAX_2030NS               (_EFP01_BK_CTRL1_BK_TON_MAX_2030NS << 5)

/** Bit fields for EFP01_BK_IPK */
#define _EFP01_BK_IPK_ADDRESS                          0x58
#define _EFP01_BK_IPK_RESETVALUE                       0x00
#define _EFP01_BK_IPK_BK_IPK_SHIFT                     0
#define _EFP01_BK_IPK_BK_IPK_MASK                      0x1F
#define _EFP01_BK_IPK_BK_IPK_EM2_SHIFT                 5
#define _EFP01_BK_IPK_BK_IPK_EM2_MASK                  0xE0

/** Bit fields for EFP01_BK_CTRL2 */
#define _EFP01_BK_CTRL2_ADDRESS                        0x59
#define _EFP01_BK_CTRL2_RESETVALUE                     0x00
#define _EFP01_BK_CTRL2_BK_IRI_CON_SHIFT               0
#define _EFP01_BK_CTRL2_BK_IRI_CON_MASK                0x0F
#define _EFP01_BK_CTRL2_BK_LDO_THRESH_SHIFT            4
#define _EFP01_BK_CTRL2_BK_LDO_THRESH_MASK             0x70

/** Bit fields for EFP01_ADC_CC_CTRL */
#define _EFP01_ADC_CC_CTRL_ADDRESS                     0x5C
#define _EFP01_ADC_CC_CTRL_RESETVALUE                  0x00
#define _EFP01_ADC_CC_CTRL_ADC_INTERVAL_SHIFT          0
#define _EFP01_ADC_CC_CTRL_ADC_INTERVAL_MASK           0x07
#define _EFP01_ADC_CC_CTRL_ADC_IIR_TAU_SHIFT           3
#define _EFP01_ADC_CC_CTRL_ADC_IIR_TAU_MASK            0x38

/** Bit fields for EFP01_ADC_LIMITS */
#define _EFP01_ADC_LIMITS_ADDRESS                      0x5D
#define _EFP01_ADC_LIMITS_RESETVALUE                   0x00
#define _EFP01_ADC_LIMITS_ADC_T_LIM_SHIFT              0
#define _EFP01_ADC_LIMITS_ADC_T_LIM_MASK               0x07
#define _EFP01_ADC_LIMITS_ADC_V_LIM_SHIFT              3
#define _EFP01_ADC_LIMITS_ADC_V_LIM_MASK               0xF8

/** @endcond */

/** @} (end addtogroup sl_efp) */

#ifdef __cplusplus
}
#endif
#endif /* #define SL_EFP01_H */
