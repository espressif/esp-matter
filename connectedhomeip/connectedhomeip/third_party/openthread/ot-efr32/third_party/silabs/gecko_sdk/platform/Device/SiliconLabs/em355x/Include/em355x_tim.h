/**************************************************************************//**
* @file
* @brief em355x_tim Register and Bit Field definitions
* @version 5.8.3
******************************************************************************
* @section License
* <b>(C) Copyright 2014 Silicon Labs, www.silabs.com</b>
*******************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
******************************************************************************/

#ifndef EM355X_TIM_H
#define EM355X_TIM_H

/**************************************************************************//**
 * @defgroup EM355X_TIM
 * @{
 * @brief EM355X_TIM Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t CR1;
  __IOM uint32_t CR2;
  __IOM uint32_t SMCR;
  __IOM uint32_t DIER;
  __IOM uint32_t SR;
  __IOM uint32_t EGR;
  __IOM uint32_t CCMR1;
  __IOM uint32_t CCMR2;
  __IOM uint32_t CCER;
  __IOM uint32_t CNT;
  __IOM uint32_t PSC;
  __IOM uint32_t ARR;
  uint32_t RESERVED0[1];
  __IOM uint32_t CCR1;
  __IOM uint32_t CCR2;
  __IOM uint32_t CCR3;
  __IOM uint32_t CCR4;
  uint32_t RESERVED1[3];
  __IOM uint32_t OR;
} TIM_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM355X_TIM_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for TIM CR1 */
#define _TIM_CR1_RESETVALUE          0x00000000UL
#define _TIM_CR1_MASK                0x000000FFUL
#define TIM_CR1_TIM_ARBE             (0x1UL << 7)
#define _TIM_CR1_TIM_ARBE_SHIFT      7
#define _TIM_CR1_TIM_ARBE_MASK       0x80UL
#define _TIM_CR1_TIM_ARBE_DEFAULT    0x00000000UL
#define TIM_CR1_TIM_ARBE_DEFAULT     (_TIM_CR1_TIM_ARBE_DEFAULT << 7)
#define _TIM_CR1_TIM_CMS_SHIFT       5
#define _TIM_CR1_TIM_CMS_MASK        0x60UL
#define _TIM_CR1_TIM_CMS_DEFAULT     0x00000000UL
#define TIM_CR1_TIM_CMS_DEFAULT      (_TIM_CR1_TIM_CMS_DEFAULT << 5)
#define TIM_CR1_TIM_DIR              (0x1UL << 4)
#define _TIM_CR1_TIM_DIR_SHIFT       4
#define _TIM_CR1_TIM_DIR_MASK        0x10UL
#define _TIM_CR1_TIM_DIR_DEFAULT     0x00000000UL
#define TIM_CR1_TIM_DIR_DEFAULT      (_TIM_CR1_TIM_DIR_DEFAULT << 4)
#define TIM_CR1_TIM_OPM              (0x1UL << 3)
#define _TIM_CR1_TIM_OPM_SHIFT       3
#define _TIM_CR1_TIM_OPM_MASK        0x8UL
#define _TIM_CR1_TIM_OPM_DEFAULT     0x00000000UL
#define TIM_CR1_TIM_OPM_DEFAULT      (_TIM_CR1_TIM_OPM_DEFAULT << 3)
#define TIM_CR1_TIM_URS              (0x1UL << 2)
#define _TIM_CR1_TIM_URS_SHIFT       2
#define _TIM_CR1_TIM_URS_MASK        0x4UL
#define _TIM_CR1_TIM_URS_DEFAULT     0x00000000UL
#define TIM_CR1_TIM_URS_DEFAULT      (_TIM_CR1_TIM_URS_DEFAULT << 2)
#define TIM_CR1_TIM_UDIS             (0x1UL << 1)
#define _TIM_CR1_TIM_UDIS_SHIFT      1
#define _TIM_CR1_TIM_UDIS_MASK       0x2UL
#define _TIM_CR1_TIM_UDIS_DEFAULT    0x00000000UL
#define TIM_CR1_TIM_UDIS_DEFAULT     (_TIM_CR1_TIM_UDIS_DEFAULT << 1)
#define TIM_CR1_TIM_CEN              (0x1UL << 0)
#define _TIM_CR1_TIM_CEN_SHIFT       0
#define _TIM_CR1_TIM_CEN_MASK        0x1UL
#define _TIM_CR1_TIM_CEN_DEFAULT     0x00000000UL
#define TIM_CR1_TIM_CEN_DEFAULT      (_TIM_CR1_TIM_CEN_DEFAULT << 0)

/* Bit fields for TIM CR2 */
#define _TIM_CR2_RESETVALUE          0x00000000UL
#define _TIM_CR2_MASK                0x000000F0UL
#define TIM_CR2_TIM_TI1S             (0x1UL << 7)
#define _TIM_CR2_TIM_TI1S_SHIFT      7
#define _TIM_CR2_TIM_TI1S_MASK       0x80UL
#define _TIM_CR2_TIM_TI1S_DEFAULT    0x00000000UL
#define TIM_CR2_TIM_TI1S_DEFAULT     (_TIM_CR2_TIM_TI1S_DEFAULT << 7)
#define _TIM_CR2_TIM_MMS_SHIFT       4
#define _TIM_CR2_TIM_MMS_MASK        0x70UL
#define _TIM_CR2_TIM_MMS_DEFAULT     0x00000000UL
#define TIM_CR2_TIM_MMS_DEFAULT      (_TIM_CR2_TIM_MMS_DEFAULT << 4)

/* Bit fields for TIM SMCR */
#define _TIM_SMCR_RESETVALUE          0x00000000UL
#define _TIM_SMCR_MASK                0x0000FFF7UL
#define TIM_SMCR_TIM_ETP              (0x1UL << 15)
#define _TIM_SMCR_TIM_ETP_SHIFT       15
#define _TIM_SMCR_TIM_ETP_MASK        0x8000UL
#define _TIM_SMCR_TIM_ETP_DEFAULT     0x00000000UL
#define TIM_SMCR_TIM_ETP_DEFAULT      (_TIM_SMCR_TIM_ETP_DEFAULT << 15)
#define TIM_SMCR_TIM_ECE              (0x1UL << 14)
#define _TIM_SMCR_TIM_ECE_SHIFT       14
#define _TIM_SMCR_TIM_ECE_MASK        0x4000UL
#define _TIM_SMCR_TIM_ECE_DEFAULT     0x00000000UL
#define TIM_SMCR_TIM_ECE_DEFAULT      (_TIM_SMCR_TIM_ECE_DEFAULT << 14)
#define _TIM_SMCR_TIM_ETPS_SHIFT      12
#define _TIM_SMCR_TIM_ETPS_MASK       0x3000UL
#define _TIM_SMCR_TIM_ETPS_DEFAULT    0x00000000UL
#define TIM_SMCR_TIM_ETPS_DEFAULT     (_TIM_SMCR_TIM_ETPS_DEFAULT << 12)
#define _TIM_SMCR_TIM_ETF_SHIFT       8
#define _TIM_SMCR_TIM_ETF_MASK        0xF00UL
#define _TIM_SMCR_TIM_ETF_DEFAULT     0x00000000UL
#define TIM_SMCR_TIM_ETF_DEFAULT      (_TIM_SMCR_TIM_ETF_DEFAULT << 8)
#define TIM_SMCR_TIM_MSM              (0x1UL << 7)
#define _TIM_SMCR_TIM_MSM_SHIFT       7
#define _TIM_SMCR_TIM_MSM_MASK        0x80UL
#define _TIM_SMCR_TIM_MSM_DEFAULT     0x00000000UL
#define TIM_SMCR_TIM_MSM_DEFAULT      (_TIM_SMCR_TIM_MSM_DEFAULT << 7)
#define _TIM_SMCR_TIM_TS_SHIFT        4
#define _TIM_SMCR_TIM_TS_MASK         0x70UL
#define _TIM_SMCR_TIM_TS_DEFAULT      0x00000000UL
#define TIM_SMCR_TIM_TS_DEFAULT       (_TIM_SMCR_TIM_TS_DEFAULT << 4)
#define _TIM_SMCR_TIM_SMS_SHIFT       0
#define _TIM_SMCR_TIM_SMS_MASK        0x7UL
#define _TIM_SMCR_TIM_SMS_DEFAULT     0x00000000UL
#define TIM_SMCR_TIM_SMS_DEFAULT      (_TIM_SMCR_TIM_SMS_DEFAULT << 0)

/* Bit fields for TIM DIER */
#define _TIM_DIER_RESETVALUE       0x00000000UL
#define _TIM_DIER_MASK             0x0000005FUL
#define TIM_DIER_TIE               (0x1UL << 6)
#define _TIM_DIER_TIE_SHIFT        6
#define _TIM_DIER_TIE_MASK         0x40UL
#define _TIM_DIER_TIE_DEFAULT      0x00000000UL
#define TIM_DIER_TIE_DEFAULT       (_TIM_DIER_TIE_DEFAULT << 6)
#define TIM_DIER_CC4IE             (0x1UL << 4)
#define _TIM_DIER_CC4IE_SHIFT      4
#define _TIM_DIER_CC4IE_MASK       0x10UL
#define _TIM_DIER_CC4IE_DEFAULT    0x00000000UL
#define TIM_DIER_CC4IE_DEFAULT     (_TIM_DIER_CC4IE_DEFAULT << 4)
#define TIM_DIER_CC3IE             (0x1UL << 3)
#define _TIM_DIER_CC3IE_SHIFT      3
#define _TIM_DIER_CC3IE_MASK       0x8UL
#define _TIM_DIER_CC3IE_DEFAULT    0x00000000UL
#define TIM_DIER_CC3IE_DEFAULT     (_TIM_DIER_CC3IE_DEFAULT << 3)
#define TIM_DIER_CC2IE             (0x1UL << 2)
#define _TIM_DIER_CC2IE_SHIFT      2
#define _TIM_DIER_CC2IE_MASK       0x4UL
#define _TIM_DIER_CC2IE_DEFAULT    0x00000000UL
#define TIM_DIER_CC2IE_DEFAULT     (_TIM_DIER_CC2IE_DEFAULT << 2)
#define TIM_DIER_CC1IE             (0x1UL << 1)
#define _TIM_DIER_CC1IE_SHIFT      1
#define _TIM_DIER_CC1IE_MASK       0x2UL
#define _TIM_DIER_CC1IE_DEFAULT    0x00000000UL
#define TIM_DIER_CC1IE_DEFAULT     (_TIM_DIER_CC1IE_DEFAULT << 1)
#define TIM_DIER_UIE               (0x1UL << 0)
#define _TIM_DIER_UIE_SHIFT        0
#define _TIM_DIER_UIE_MASK         0x1UL
#define _TIM_DIER_UIE_DEFAULT      0x00000000UL
#define TIM_DIER_UIE_DEFAULT       (_TIM_DIER_UIE_DEFAULT << 0)

/* Bit fields for TIM SR */
#define _TIM_SR_RESETVALUE       0x00000000UL
#define _TIM_SR_MASK             0x00001E5FUL
#define TIM_SR_CC4OF             (0x1UL << 12)
#define _TIM_SR_CC4OF_SHIFT      12
#define _TIM_SR_CC4OF_MASK       0x1000UL
#define _TIM_SR_CC4OF_DEFAULT    0x00000000UL
#define TIM_SR_CC4OF_DEFAULT     (_TIM_SR_CC4OF_DEFAULT << 12)
#define TIM_SR_CC3OF             (0x1UL << 11)
#define _TIM_SR_CC3OF_SHIFT      11
#define _TIM_SR_CC3OF_MASK       0x800UL
#define _TIM_SR_CC3OF_DEFAULT    0x00000000UL
#define TIM_SR_CC3OF_DEFAULT     (_TIM_SR_CC3OF_DEFAULT << 11)
#define TIM_SR_CC2OF             (0x1UL << 10)
#define _TIM_SR_CC2OF_SHIFT      10
#define _TIM_SR_CC2OF_MASK       0x400UL
#define _TIM_SR_CC2OF_DEFAULT    0x00000000UL
#define TIM_SR_CC2OF_DEFAULT     (_TIM_SR_CC2OF_DEFAULT << 10)
#define TIM_SR_CC1OF             (0x1UL << 9)
#define _TIM_SR_CC1OF_SHIFT      9
#define _TIM_SR_CC1OF_MASK       0x200UL
#define _TIM_SR_CC1OF_DEFAULT    0x00000000UL
#define TIM_SR_CC1OF_DEFAULT     (_TIM_SR_CC1OF_DEFAULT << 9)
#define TIM_SR_TIF               (0x1UL << 6)
#define _TIM_SR_TIF_SHIFT        6
#define _TIM_SR_TIF_MASK         0x40UL
#define _TIM_SR_TIF_DEFAULT      0x00000000UL
#define TIM_SR_TIF_DEFAULT       (_TIM_SR_TIF_DEFAULT << 6)
#define TIM_SR_CC4IF             (0x1UL << 4)
#define _TIM_SR_CC4IF_SHIFT      4
#define _TIM_SR_CC4IF_MASK       0x10UL
#define _TIM_SR_CC4IF_DEFAULT    0x00000000UL
#define TIM_SR_CC4IF_DEFAULT     (_TIM_SR_CC4IF_DEFAULT << 4)
#define TIM_SR_CC3IF             (0x1UL << 3)
#define _TIM_SR_CC3IF_SHIFT      3
#define _TIM_SR_CC3IF_MASK       0x8UL
#define _TIM_SR_CC3IF_DEFAULT    0x00000000UL
#define TIM_SR_CC3IF_DEFAULT     (_TIM_SR_CC3IF_DEFAULT << 3)
#define TIM_SR_CC2IF             (0x1UL << 2)
#define _TIM_SR_CC2IF_SHIFT      2
#define _TIM_SR_CC2IF_MASK       0x4UL
#define _TIM_SR_CC2IF_DEFAULT    0x00000000UL
#define TIM_SR_CC2IF_DEFAULT     (_TIM_SR_CC2IF_DEFAULT << 2)
#define TIM_SR_CC1IF             (0x1UL << 1)
#define _TIM_SR_CC1IF_SHIFT      1
#define _TIM_SR_CC1IF_MASK       0x2UL
#define _TIM_SR_CC1IF_DEFAULT    0x00000000UL
#define TIM_SR_CC1IF_DEFAULT     (_TIM_SR_CC1IF_DEFAULT << 1)
#define TIM_SR_UIF               (0x1UL << 0)
#define _TIM_SR_UIF_SHIFT        0
#define _TIM_SR_UIF_MASK         0x1UL
#define _TIM_SR_UIF_DEFAULT      0x00000000UL
#define TIM_SR_UIF_DEFAULT       (_TIM_SR_UIF_DEFAULT << 0)

/* Bit fields for TIM EGR */
#define _TIM_EGR_RESETVALUE          0x00000000UL
#define _TIM_EGR_MASK                0x0000005FUL
#define TIM_EGR_TIM_TG               (0x1UL << 6)
#define _TIM_EGR_TIM_TG_SHIFT        6
#define _TIM_EGR_TIM_TG_MASK         0x40UL
#define _TIM_EGR_TIM_TG_DEFAULT      0x00000000UL
#define TIM_EGR_TIM_TG_DEFAULT       (_TIM_EGR_TIM_TG_DEFAULT << 6)
#define TIM_EGR_TIM_CC4G             (0x1UL << 4)
#define _TIM_EGR_TIM_CC4G_SHIFT      4
#define _TIM_EGR_TIM_CC4G_MASK       0x10UL
#define _TIM_EGR_TIM_CC4G_DEFAULT    0x00000000UL
#define TIM_EGR_TIM_CC4G_DEFAULT     (_TIM_EGR_TIM_CC4G_DEFAULT << 4)
#define TIM_EGR_TIM_CC3G             (0x1UL << 3)
#define _TIM_EGR_TIM_CC3G_SHIFT      3
#define _TIM_EGR_TIM_CC3G_MASK       0x8UL
#define _TIM_EGR_TIM_CC3G_DEFAULT    0x00000000UL
#define TIM_EGR_TIM_CC3G_DEFAULT     (_TIM_EGR_TIM_CC3G_DEFAULT << 3)
#define TIM_EGR_TIM_CC2G             (0x1UL << 2)
#define _TIM_EGR_TIM_CC2G_SHIFT      2
#define _TIM_EGR_TIM_CC2G_MASK       0x4UL
#define _TIM_EGR_TIM_CC2G_DEFAULT    0x00000000UL
#define TIM_EGR_TIM_CC2G_DEFAULT     (_TIM_EGR_TIM_CC2G_DEFAULT << 2)
#define TIM_EGR_TIM_CC1G             (0x1UL << 1)
#define _TIM_EGR_TIM_CC1G_SHIFT      1
#define _TIM_EGR_TIM_CC1G_MASK       0x2UL
#define _TIM_EGR_TIM_CC1G_DEFAULT    0x00000000UL
#define TIM_EGR_TIM_CC1G_DEFAULT     (_TIM_EGR_TIM_CC1G_DEFAULT << 1)
#define TIM_EGR_TIM_UG               (0x1UL << 0)
#define _TIM_EGR_TIM_UG_SHIFT        0
#define _TIM_EGR_TIM_UG_MASK         0x1UL
#define _TIM_EGR_TIM_UG_DEFAULT      0x00000000UL
#define TIM_EGR_TIM_UG_DEFAULT       (_TIM_EGR_TIM_UG_DEFAULT << 0)

/* Bit fields for TIM CCMR1 */
#define _TIM_CCMR1_RESETVALUE            0x00000000UL
#define _TIM_CCMR1_MASK                  0x0000FFFFUL
#define _TIM_CCMR1_TIM_IC2F_SHIFT        12
#define _TIM_CCMR1_TIM_IC2F_MASK         0xF000UL
#define _TIM_CCMR1_TIM_IC2F_DEFAULT      0x00000000UL
#define TIM_CCMR1_TIM_IC2F_DEFAULT       (_TIM_CCMR1_TIM_IC2F_DEFAULT << 12)
#define _TIM_CCMR1_TIM_IC2PSC_SHIFT      10
#define _TIM_CCMR1_TIM_IC2PSC_MASK       0xC00UL
#define _TIM_CCMR1_TIM_IC2PSC_DEFAULT    0x00000000UL
#define TIM_CCMR1_TIM_IC2PSC_DEFAULT     (_TIM_CCMR1_TIM_IC2PSC_DEFAULT << 10)
#define _TIM_CCMR1_TIM_IC1F_SHIFT        4
#define _TIM_CCMR1_TIM_IC1F_MASK         0xF0UL
#define _TIM_CCMR1_TIM_IC1F_DEFAULT      0x00000000UL
#define TIM_CCMR1_TIM_IC1F_DEFAULT       (_TIM_CCMR1_TIM_IC1F_DEFAULT << 4)
#define _TIM_CCMR1_TIM_IC1PSC_SHIFT      2
#define _TIM_CCMR1_TIM_IC1PSC_MASK       0xCUL
#define _TIM_CCMR1_TIM_IC1PSC_DEFAULT    0x00000000UL
#define TIM_CCMR1_TIM_IC1PSC_DEFAULT     (_TIM_CCMR1_TIM_IC1PSC_DEFAULT << 2)
#define TIM_CCMR1_TIM_OC2CE              (0x1UL << 15)
#define _TIM_CCMR1_TIM_OC2CE_SHIFT       15
#define _TIM_CCMR1_TIM_OC2CE_MASK        0x8000UL
#define _TIM_CCMR1_TIM_OC2CE_DEFAULT     0x00000000UL
#define TIM_CCMR1_TIM_OC2CE_DEFAULT      (_TIM_CCMR1_TIM_OC2CE_DEFAULT << 15)
#define _TIM_CCMR1_TIM_OC2M_SHIFT        12
#define _TIM_CCMR1_TIM_OC2M_MASK         0x7000UL
#define _TIM_CCMR1_TIM_OC2M_DEFAULT      0x00000000UL
#define TIM_CCMR1_TIM_OC2M_DEFAULT       (_TIM_CCMR1_TIM_OC2M_DEFAULT << 12)
#define TIM_CCMR1_TIM_OC2BE              (0x1UL << 11)
#define _TIM_CCMR1_TIM_OC2BE_SHIFT       11
#define _TIM_CCMR1_TIM_OC2BE_MASK        0x800UL
#define _TIM_CCMR1_TIM_OC2BE_DEFAULT     0x00000000UL
#define TIM_CCMR1_TIM_OC2BE_DEFAULT      (_TIM_CCMR1_TIM_OC2BE_DEFAULT << 11)
#define TIM_CCMR1_TIM_OC2FE              (0x1UL << 10)
#define _TIM_CCMR1_TIM_OC2FE_SHIFT       10
#define _TIM_CCMR1_TIM_OC2FE_MASK        0x400UL
#define _TIM_CCMR1_TIM_OC2FE_DEFAULT     0x00000000UL
#define TIM_CCMR1_TIM_OC2FE_DEFAULT      (_TIM_CCMR1_TIM_OC2FE_DEFAULT << 10)
#define _TIM_CCMR1_TIM_CC2S_SHIFT        8
#define _TIM_CCMR1_TIM_CC2S_MASK         0x300UL
#define _TIM_CCMR1_TIM_CC2S_DEFAULT      0x00000000UL
#define TIM_CCMR1_TIM_CC2S_DEFAULT       (_TIM_CCMR1_TIM_CC2S_DEFAULT << 8)
#define TIM_CCMR1_TIM_OC1CE              (0x1UL << 7)
#define _TIM_CCMR1_TIM_OC1CE_SHIFT       7
#define _TIM_CCMR1_TIM_OC1CE_MASK        0x80UL
#define _TIM_CCMR1_TIM_OC1CE_DEFAULT     0x00000000UL
#define TIM_CCMR1_TIM_OC1CE_DEFAULT      (_TIM_CCMR1_TIM_OC1CE_DEFAULT << 7)
#define _TIM_CCMR1_TIM_OC1M_SHIFT        4
#define _TIM_CCMR1_TIM_OC1M_MASK         0x70UL
#define _TIM_CCMR1_TIM_OC1M_DEFAULT      0x00000000UL
#define TIM_CCMR1_TIM_OC1M_DEFAULT       (_TIM_CCMR1_TIM_OC1M_DEFAULT << 4)
#define TIM_CCMR1_TIM_OC1BE              (0x1UL << 3)
#define _TIM_CCMR1_TIM_OC1BE_SHIFT       3
#define _TIM_CCMR1_TIM_OC1BE_MASK        0x8UL
#define _TIM_CCMR1_TIM_OC1BE_DEFAULT     0x00000000UL
#define TIM_CCMR1_TIM_OC1BE_DEFAULT      (_TIM_CCMR1_TIM_OC1BE_DEFAULT << 3)
#define TIM_CCMR1_TIM_OC1FE              (0x1UL << 2)
#define _TIM_CCMR1_TIM_OC1FE_SHIFT       2
#define _TIM_CCMR1_TIM_OC1FE_MASK        0x4UL
#define _TIM_CCMR1_TIM_OC1FE_DEFAULT     0x00000000UL
#define TIM_CCMR1_TIM_OC1FE_DEFAULT      (_TIM_CCMR1_TIM_OC1FE_DEFAULT << 2)
#define _TIM_CCMR1_TIM_CC1S_SHIFT        0
#define _TIM_CCMR1_TIM_CC1S_MASK         0x3UL
#define _TIM_CCMR1_TIM_CC1S_DEFAULT      0x00000000UL
#define TIM_CCMR1_TIM_CC1S_DEFAULT       (_TIM_CCMR1_TIM_CC1S_DEFAULT << 0)

/* Bit fields for TIM CCMR2 */
#define _TIM_CCMR2_RESETVALUE            0x00000000UL
#define _TIM_CCMR2_MASK                  0x0000FFFFUL
#define _TIM_CCMR2_TIM_IC4F_SHIFT        12
#define _TIM_CCMR2_TIM_IC4F_MASK         0xF000UL
#define _TIM_CCMR2_TIM_IC4F_DEFAULT      0x00000000UL
#define TIM_CCMR2_TIM_IC4F_DEFAULT       (_TIM_CCMR2_TIM_IC4F_DEFAULT << 12)
#define _TIM_CCMR2_TIM_IC4PSC_SHIFT      10
#define _TIM_CCMR2_TIM_IC4PSC_MASK       0xC00UL
#define _TIM_CCMR2_TIM_IC4PSC_DEFAULT    0x00000000UL
#define TIM_CCMR2_TIM_IC4PSC_DEFAULT     (_TIM_CCMR2_TIM_IC4PSC_DEFAULT << 10)
#define _TIM_CCMR2_TIM_IC3F_SHIFT        4
#define _TIM_CCMR2_TIM_IC3F_MASK         0xF0UL
#define _TIM_CCMR2_TIM_IC3F_DEFAULT      0x00000000UL
#define TIM_CCMR2_TIM_IC3F_DEFAULT       (_TIM_CCMR2_TIM_IC3F_DEFAULT << 4)
#define _TIM_CCMR2_TIM_IC3PSC_SHIFT      2
#define _TIM_CCMR2_TIM_IC3PSC_MASK       0xCUL
#define _TIM_CCMR2_TIM_IC3PSC_DEFAULT    0x00000000UL
#define TIM_CCMR2_TIM_IC3PSC_DEFAULT     (_TIM_CCMR2_TIM_IC3PSC_DEFAULT << 2)
#define TIM_CCMR2_TIM_OC4CE              (0x1UL << 15)
#define _TIM_CCMR2_TIM_OC4CE_SHIFT       15
#define _TIM_CCMR2_TIM_OC4CE_MASK        0x8000UL
#define _TIM_CCMR2_TIM_OC4CE_DEFAULT     0x00000000UL
#define TIM_CCMR2_TIM_OC4CE_DEFAULT      (_TIM_CCMR2_TIM_OC4CE_DEFAULT << 15)
#define _TIM_CCMR2_TIM_OC4M_SHIFT        12
#define _TIM_CCMR2_TIM_OC4M_MASK         0x7000UL
#define _TIM_CCMR2_TIM_OC4M_DEFAULT      0x00000000UL
#define TIM_CCMR2_TIM_OC4M_DEFAULT       (_TIM_CCMR2_TIM_OC4M_DEFAULT << 12)
#define TIM_CCMR2_TIM_OC4BE              (0x1UL << 11)
#define _TIM_CCMR2_TIM_OC4BE_SHIFT       11
#define _TIM_CCMR2_TIM_OC4BE_MASK        0x800UL
#define _TIM_CCMR2_TIM_OC4BE_DEFAULT     0x00000000UL
#define TIM_CCMR2_TIM_OC4BE_DEFAULT      (_TIM_CCMR2_TIM_OC4BE_DEFAULT << 11)
#define TIM_CCMR2_TIM_OC4FE              (0x1UL << 10)
#define _TIM_CCMR2_TIM_OC4FE_SHIFT       10
#define _TIM_CCMR2_TIM_OC4FE_MASK        0x400UL
#define _TIM_CCMR2_TIM_OC4FE_DEFAULT     0x00000000UL
#define TIM_CCMR2_TIM_OC4FE_DEFAULT      (_TIM_CCMR2_TIM_OC4FE_DEFAULT << 10)
#define _TIM_CCMR2_TIM_CC4S_SHIFT        8
#define _TIM_CCMR2_TIM_CC4S_MASK         0x300UL
#define _TIM_CCMR2_TIM_CC4S_DEFAULT      0x00000000UL
#define TIM_CCMR2_TIM_CC4S_DEFAULT       (_TIM_CCMR2_TIM_CC4S_DEFAULT << 8)
#define TIM_CCMR2_TIM_OC3CE              (0x1UL << 7)
#define _TIM_CCMR2_TIM_OC3CE_SHIFT       7
#define _TIM_CCMR2_TIM_OC3CE_MASK        0x80UL
#define _TIM_CCMR2_TIM_OC3CE_DEFAULT     0x00000000UL
#define TIM_CCMR2_TIM_OC3CE_DEFAULT      (_TIM_CCMR2_TIM_OC3CE_DEFAULT << 7)
#define _TIM_CCMR2_TIM_OC3M_SHIFT        4
#define _TIM_CCMR2_TIM_OC3M_MASK         0x70UL
#define _TIM_CCMR2_TIM_OC3M_DEFAULT      0x00000000UL
#define TIM_CCMR2_TIM_OC3M_DEFAULT       (_TIM_CCMR2_TIM_OC3M_DEFAULT << 4)
#define TIM_CCMR2_TIM_OC3BE              (0x1UL << 3)
#define _TIM_CCMR2_TIM_OC3BE_SHIFT       3
#define _TIM_CCMR2_TIM_OC3BE_MASK        0x8UL
#define _TIM_CCMR2_TIM_OC3BE_DEFAULT     0x00000000UL
#define TIM_CCMR2_TIM_OC3BE_DEFAULT      (_TIM_CCMR2_TIM_OC3BE_DEFAULT << 3)
#define TIM_CCMR2_TIM_OC3FE              (0x1UL << 2)
#define _TIM_CCMR2_TIM_OC3FE_SHIFT       2
#define _TIM_CCMR2_TIM_OC3FE_MASK        0x4UL
#define _TIM_CCMR2_TIM_OC3FE_DEFAULT     0x00000000UL
#define TIM_CCMR2_TIM_OC3FE_DEFAULT      (_TIM_CCMR2_TIM_OC3FE_DEFAULT << 2)
#define _TIM_CCMR2_TIM_CC3S_SHIFT        0
#define _TIM_CCMR2_TIM_CC3S_MASK         0x3UL
#define _TIM_CCMR2_TIM_CC3S_DEFAULT      0x00000000UL
#define TIM_CCMR2_TIM_CC3S_DEFAULT       (_TIM_CCMR2_TIM_CC3S_DEFAULT << 0)

/* Bit fields for TIM CCER */
#define _TIM_CCER_RESETVALUE          0x00000000UL
#define _TIM_CCER_MASK                0x00003333UL
#define TIM_CCER_TIM_CC4P             (0x1UL << 13)
#define _TIM_CCER_TIM_CC4P_SHIFT      13
#define _TIM_CCER_TIM_CC4P_MASK       0x2000UL
#define _TIM_CCER_TIM_CC4P_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC4P_DEFAULT     (_TIM_CCER_TIM_CC4P_DEFAULT << 13)
#define TIM_CCER_TIM_CC4E             (0x1UL << 12)
#define _TIM_CCER_TIM_CC4E_SHIFT      12
#define _TIM_CCER_TIM_CC4E_MASK       0x1000UL
#define _TIM_CCER_TIM_CC4E_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC4E_DEFAULT     (_TIM_CCER_TIM_CC4E_DEFAULT << 12)
#define TIM_CCER_TIM_CC3P             (0x1UL << 9)
#define _TIM_CCER_TIM_CC3P_SHIFT      9
#define _TIM_CCER_TIM_CC3P_MASK       0x200UL
#define _TIM_CCER_TIM_CC3P_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC3P_DEFAULT     (_TIM_CCER_TIM_CC3P_DEFAULT << 9)
#define TIM_CCER_TIM_CC3E             (0x1UL << 8)
#define _TIM_CCER_TIM_CC3E_SHIFT      8
#define _TIM_CCER_TIM_CC3E_MASK       0x100UL
#define _TIM_CCER_TIM_CC3E_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC3E_DEFAULT     (_TIM_CCER_TIM_CC3E_DEFAULT << 8)
#define TIM_CCER_TIM_CC2P             (0x1UL << 5)
#define _TIM_CCER_TIM_CC2P_SHIFT      5
#define _TIM_CCER_TIM_CC2P_MASK       0x20UL
#define _TIM_CCER_TIM_CC2P_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC2P_DEFAULT     (_TIM_CCER_TIM_CC2P_DEFAULT << 5)
#define TIM_CCER_TIM_CC2E             (0x1UL << 4)
#define _TIM_CCER_TIM_CC2E_SHIFT      4
#define _TIM_CCER_TIM_CC2E_MASK       0x10UL
#define _TIM_CCER_TIM_CC2E_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC2E_DEFAULT     (_TIM_CCER_TIM_CC2E_DEFAULT << 4)
#define TIM_CCER_TIM_CC1P             (0x1UL << 1)
#define _TIM_CCER_TIM_CC1P_SHIFT      1
#define _TIM_CCER_TIM_CC1P_MASK       0x2UL
#define _TIM_CCER_TIM_CC1P_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC1P_DEFAULT     (_TIM_CCER_TIM_CC1P_DEFAULT << 1)
#define TIM_CCER_TIM_CC1E             (0x1UL << 0)
#define _TIM_CCER_TIM_CC1E_SHIFT      0
#define _TIM_CCER_TIM_CC1E_MASK       0x1UL
#define _TIM_CCER_TIM_CC1E_DEFAULT    0x00000000UL
#define TIM_CCER_TIM_CC1E_DEFAULT     (_TIM_CCER_TIM_CC1E_DEFAULT << 0)

/* Bit fields for TIM CNT */
#define _TIM_CNT_RESETVALUE         0x00000000UL
#define _TIM_CNT_MASK               0x0000FFFFUL
#define _TIM_CNT_TIM_CNT_SHIFT      0
#define _TIM_CNT_TIM_CNT_MASK       0xFFFFUL
#define _TIM_CNT_TIM_CNT_DEFAULT    0x00000000UL
#define TIM_CNT_TIM_CNT_DEFAULT     (_TIM_CNT_TIM_CNT_DEFAULT << 0)

/* Bit fields for TIM PSC */
#define _TIM_PSC_RESETVALUE         0x00000000UL
#define _TIM_PSC_MASK               0x0000000FUL
#define _TIM_PSC_TIM_PSC_SHIFT      0
#define _TIM_PSC_TIM_PSC_MASK       0xFUL
#define _TIM_PSC_TIM_PSC_DEFAULT    0x00000000UL
#define TIM_PSC_TIM_PSC_DEFAULT     (_TIM_PSC_TIM_PSC_DEFAULT << 0)

/* Bit fields for TIM ARR */
#define _TIM_ARR_RESETVALUE         0x0000FFFFUL
#define _TIM_ARR_MASK               0x0000FFFFUL
#define _TIM_ARR_TIM_ARR_SHIFT      0
#define _TIM_ARR_TIM_ARR_MASK       0xFFFFUL
#define _TIM_ARR_TIM_ARR_DEFAULT    0x0000FFFFUL
#define TIM_ARR_TIM_ARR_DEFAULT     (_TIM_ARR_TIM_ARR_DEFAULT << 0)

/* Bit fields for TIM CCR1 */
#define _TIM_CCR1_RESETVALUE         0x00000000UL
#define _TIM_CCR1_MASK               0x0000FFFFUL
#define _TIM_CCR1_TIM_CCR_SHIFT      0
#define _TIM_CCR1_TIM_CCR_MASK       0xFFFFUL
#define _TIM_CCR1_TIM_CCR_DEFAULT    0x00000000UL
#define TIM_CCR1_TIM_CCR_DEFAULT     (_TIM_CCR1_TIM_CCR_DEFAULT << 0)

/* Bit fields for TIM CCR2 */
#define _TIM_CCR2_RESETVALUE         0x00000000UL
#define _TIM_CCR2_MASK               0x0000FFFFUL
#define _TIM_CCR2_TIM_CCR_SHIFT      0
#define _TIM_CCR2_TIM_CCR_MASK       0xFFFFUL
#define _TIM_CCR2_TIM_CCR_DEFAULT    0x00000000UL
#define TIM_CCR2_TIM_CCR_DEFAULT     (_TIM_CCR2_TIM_CCR_DEFAULT << 0)

/* Bit fields for TIM CCR3 */
#define _TIM_CCR3_RESETVALUE         0x00000000UL
#define _TIM_CCR3_MASK               0x0000FFFFUL
#define _TIM_CCR3_TIM_CCR_SHIFT      0
#define _TIM_CCR3_TIM_CCR_MASK       0xFFFFUL
#define _TIM_CCR3_TIM_CCR_DEFAULT    0x00000000UL
#define TIM_CCR3_TIM_CCR_DEFAULT     (_TIM_CCR3_TIM_CCR_DEFAULT << 0)

/* Bit fields for TIM CCR4 */
#define _TIM_CCR4_RESETVALUE         0x00000000UL
#define _TIM_CCR4_MASK               0x0000FFFFUL
#define _TIM_CCR4_TIM_CCR_SHIFT      0
#define _TIM_CCR4_TIM_CCR_MASK       0xFFFFUL
#define _TIM_CCR4_TIM_CCR_DEFAULT    0x00000000UL
#define TIM_CCR4_TIM_CCR_DEFAULT     (_TIM_CCR4_TIM_CCR_DEFAULT << 0)

/* Bit fields for TIM OR */
#define _TIM_OR_RESETVALUE               0x00000000UL
#define _TIM_OR_MASK                     0x000000FFUL
#define TIM_OR_TIM_REMAPC4               (0x1UL << 7)
#define _TIM_OR_TIM_REMAPC4_SHIFT        7
#define _TIM_OR_TIM_REMAPC4_MASK         0x80UL
#define _TIM_OR_TIM_REMAPC4_DEFAULT      0x00000000UL
#define TIM_OR_TIM_REMAPC4_DEFAULT       (_TIM_OR_TIM_REMAPC4_DEFAULT << 7)
#define TIM_OR_TIM_REMAPC3               (0x1UL << 6)
#define _TIM_OR_TIM_REMAPC3_SHIFT        6
#define _TIM_OR_TIM_REMAPC3_MASK         0x40UL
#define _TIM_OR_TIM_REMAPC3_DEFAULT      0x00000000UL
#define TIM_OR_TIM_REMAPC3_DEFAULT       (_TIM_OR_TIM_REMAPC3_DEFAULT << 6)
#define TIM_OR_TIM_REMAPC2               (0x1UL << 5)
#define _TIM_OR_TIM_REMAPC2_SHIFT        5
#define _TIM_OR_TIM_REMAPC2_MASK         0x20UL
#define _TIM_OR_TIM_REMAPC2_DEFAULT      0x00000000UL
#define TIM_OR_TIM_REMAPC2_DEFAULT       (_TIM_OR_TIM_REMAPC2_DEFAULT << 5)
#define TIM_OR_TIM_REMAPC1               (0x1UL << 4)
#define _TIM_OR_TIM_REMAPC1_SHIFT        4
#define _TIM_OR_TIM_REMAPC1_MASK         0x10UL
#define _TIM_OR_TIM_REMAPC1_DEFAULT      0x00000000UL
#define TIM_OR_TIM_REMAPC1_DEFAULT       (_TIM_OR_TIM_REMAPC1_DEFAULT << 4)
#define TIM_OR_TIM_ORRSVD                (0x1UL << 3)
#define _TIM_OR_TIM_ORRSVD_SHIFT         3
#define _TIM_OR_TIM_ORRSVD_MASK          0x8UL
#define _TIM_OR_TIM_ORRSVD_DEFAULT       0x00000000UL
#define TIM_OR_TIM_ORRSVD_DEFAULT        (_TIM_OR_TIM_ORRSVD_DEFAULT << 3)
#define TIM_OR_TIM_CLKMSKEN              (0x1UL << 2)
#define _TIM_OR_TIM_CLKMSKEN_SHIFT       2
#define _TIM_OR_TIM_CLKMSKEN_MASK        0x4UL
#define _TIM_OR_TIM_CLKMSKEN_DEFAULT     0x00000000UL
#define TIM_OR_TIM_CLKMSKEN_DEFAULT      (_TIM_OR_TIM_CLKMSKEN_DEFAULT << 2)
#define _TIM_OR_TIM_EXTRIGSEL_SHIFT      0
#define _TIM_OR_TIM_EXTRIGSEL_MASK       0x3UL
#define _TIM_OR_TIM_EXTRIGSEL_DEFAULT    0x00000000UL
#define TIM_OR_TIM_EXTRIGSEL_DEFAULT     (_TIM_OR_TIM_EXTRIGSEL_DEFAULT << 0)

/** @} End of group EM355X_TIM_BitFields */

#endif // EM355X_TIM_H
