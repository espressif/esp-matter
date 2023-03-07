/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2010
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef _MTK_DVC_TEST_DRIVER_API_H
#define _MTK_DVC_TEST_DRIVER_API_H


/// I/O    ////////////////////////////////////////////////////////////////////
#define OUTREG32(a,b)   (*(volatile unsigned int *)(a) = (unsigned int)b)
#define INREG32(a)      (*(volatile unsigned int *)(a))
#define OUTREG16(a,b)   (*(volatile unsigned short *)(a) = (unsigned short)b)
#define INREG16(a)      (*(volatile unsigned short *)(a))

#define DRV_WriteReg(addr,data)     ((*(volatile kal_uint16 *)(addr)) = (kal_uint16)data)
#define DRV_Reg(addr)               (*(volatile kal_uint16 *)(addr))
#ifndef DRV_WriteReg32
#define DRV_WriteReg32(addr,data)   ((*(volatile kal_uint32 *)(addr)) = (kal_uint32)data)
#endif /* #ifndef DRV_WriteReg32 */
#ifndef DRV_Reg32
#define DRV_Reg32(addr)             (*(volatile kal_uint32 *)(addr))
#endif /* #ifndef DRV_Reg32 */
#define DRV_WriteReg8(addr,data)    ((*(volatile kal_uint8 *)(addr)) = (kal_uint8)data)
#define DRV_Reg8(addr)              (*(volatile kal_uint8 *)(addr))
#define DRV_SetReg32(addr, data)    ((*(volatile kal_uint32 *)(addr)) |= (kal_uint32)data)
#define DRV_ClrReg32(addr, data)    ((*(volatile kal_uint32 *)(addr)) &= ~((kal_uint32)data))
#define DRV_SetReg8(addr, data)    ((*(volatile kal_uint8 *)(addr)) |= (kal_uint8)data)
#define DRV_ClrReg8(addr, data)    ((*(volatile kal_uint8 *)(addr)) &= ~((kal_uint8)data))

/*
 * MTK HW register definitions
 */

#define u_intHW             uint32_t
#define ADDR(Field)         ((u_intHW volatile *) (Field##_ADDR  ))
#define ADDR32(Field)       ((uint32_t *) (Field##_ADDR  ))
#define MASK(Field)         (Field##_MASK)
#define SHFT(Field)         (Field##_SHFT)

#define ADDR16(Field)       ((u_intHW volatile *) \
                             (Field##_ADDR + device_addr + ((Field##_SHFT) < 16 ? (0) : (2)) ) )
#define MASK16(Field) \
                            ((Field##_SHFT) < 16 ? (Field##_MASK) : ((Field##_MASK) >> 16) )
#define SHFT16(Field) \
    ((Field##_SHFT) < 16 ? (Field##_SHFT) : ((Field##_SHFT) - 16 ) )

#define mGetHWEntry(Field) \
    ( ( *ADDR(Field) & (MASK(Field)) ) >> SHFT(Field) )

#define mSetHWEntry(Field, Value) \
    { u_intHW volatile *addr = ADDR(Field); \
        *addr = ((((u_intHW)(Value) << SHFT(Field)) \
        & MASK(Field)) | (*addr & ~MASK(Field))) ; }

#define mGetHWEntry32(Field) ( *ADDR(Field) )

#define mSetHWEntry32(Field, Source) ( *ADDR(Field) = Source )

#define mSetSWEntry(p_Data, Field, Value) \
    { /*u_intHW volatile *addr = ADDR(Field)*/; \
        *p_Data = ((((u_intHW)(Value) << SHFT(Field)) \
        & MASK(Field)) | (*p_Data & ~MASK(Field))) ; }


#define writel(value, addr)                     \
    do {                                \
        *(volatile uint32_t *) (addr) = (uint32_t) (value); \
    } while (0);

#define readl(addr) ( *((volatile uint32_t *) (addr)) )

#define DRV_WriteReg16(addr,data)     ((*(volatile uint16_t *)(addr)) = (uint16_t)(data))
#define DRV_Reg16(addr)               (*(volatile uint16_t *)(addr))

#define HAL_REG_32(reg)         (*((volatile unsigned int *)(reg)))
#define REG32(x)                (*(volatile unsigned int *)(x))
#define REG16(x)                (*(volatile unsigned short *)(x))
#define REG8(x)                 (*(volatile unsigned char *)(x))

#ifndef BIT
#define BIT(n)                  ((UINT32) 1 << (n))

/*
 * bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* #ifndef BIT */

/* reg scan macro */
#define REG_FLD(width, shift) \
    ((uint32_t)((((width) & 0xFF) << 16) | ((shift) & 0xFF)))

#define REG_FLD_WIDTH(field) \
    ((uint32_t)(((field) >> 16) & 0xFF))

#define REG_FLD_SHIFT(field) \
    ((uint32_t)((field) & 0xFF))

#define REG_FLD_MASK(field) \
    (((uint32_t)((uint64_t)(1) << REG_FLD_WIDTH(field)) - 1) << REG_FLD_SHIFT(field))

#define REG_FLD_GET(field, reg32) \
    (((reg32) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))

#define REG_FLD_VAL(field, val) \
    (((val) << REG_FLD_SHIFT(field)) & REG_FLD_MASK(field))

#define REG_FLD_SET(field, reg32, val)                  \
    do {                                                \
    (reg32) = (((reg32) & ~REG_FLD_MASK(field)) |   \
                   REG_FLD_VAL((field), (val)));        \
    } while (0)


/**
*  REG_SET macro : set 32 bits register at one time
*  @ reg32: macro which get the register definition
*  usage example: REG_SET(INT_STATUS, regval32);
*/

#define REG_SET(reg32, val)   \
    do {          \
      (reg32) = (val);  \
    } while (0)

enum {
    RO = 0,
    RW,
    W1C,
    WO,
};

#define SET_FLD_VAL(field, val) ((REG_FLD_VAL(field, val)) >> (REG_FLD_SHIFT(field)))


#endif /* #ifndef _MTK_DVC_TEST_DRIVER_API_H */


