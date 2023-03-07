
#ifndef _GPHAL_KX_MSI_BASIC_H_
#define _GPHAL_KX_MSI_BASIC_H_



#ifdef GP_DIVERSITY_GPHAL_INTERN
#include "gpHal_kx_mm.h"
#endif

#if defined(__GNUC__)
# define ALWAYS_INLINE static inline __attribute__((always_inline))
#elif defined(__ICCARM__)
# define ALWAYS_INLINE   _Pragma("inline=forced")
#else
# define ALWAYS_INLINE static inline
#endif

#define GP_WB_CHECK_OFFSET_PBM(offset) do {                                                         \
    COMPILE_TIME_ASSERT((((offset) - GP_MM_RAM_PBM_OPTS_START)                                      \
                         % GP_MM_RAM_PBM_OPTS_OFFSET) == 0);                                        \
    COMPILE_TIME_ASSERT((offset) >= GP_MM_RAM_PBM_OPTS_START);                                      \
    COMPILE_TIME_ASSERT((offset) <  GP_MM_RAM_PBM_OPTS_START + 8UL * GP_MM_RAM_PBM_OPTS_OFFSET);    \
} while (false)

#define GP_WB_CHECK_OFFSET_PBM_FORMAT_T         GP_WB_CHECK_OFFSET_PBM
#define GP_WB_CHECK_OFFSET_PBM_BLE_FORMAT_T     GP_WB_CHECK_OFFSET_PBM
#define GP_WB_CHECK_OFFSET_PBM_FORMAT_R         GP_WB_CHECK_OFFSET_PBM
#define GP_WB_CHECK_OFFSET_PBM_BLE_FORMAT_R     GP_WB_CHECK_OFFSET_PBM

#define GP_WB_CHECK_OFFSET_UART(offset) do {                        \
    COMPILE_TIME_ASSERT((offset) >= GP_WB_UART_0_BASE_ADDRESS);     \
} while (false)
#define GP_WB_CHECK_OFFSET_DMA(offset) do {                         \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_0_ADDRESS);      \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_1_ADDRESS);      \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_2_ADDRESS);      \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_3_ADDRESS);      \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_4_ADDRESS);      \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_6_ADDRESS);      \
    COMPILE_TIME_ASSERT((offset) == GP_WB_DMAS_SRC_ADDR_7_ADDRESS);      \
} while (false)

#define GP_WB_CHECK_OFFSET_EVENT(offset) do {                       \
    COMPILE_TIME_ASSERT((((offset) - GP_MM_RAM_EVENT_START)         \
        % GP_MM_RAM_EVENT_OFFSET) == 0);                            \
    COMPILE_TIME_ASSERT((offset) >= GP_MM_RAM_EVENT_START);         \
    COMPILE_TIME_ASSERT((offset) <  GP_MM_RAM_EVENT_START           \
        + GP_MM_RAM_EVENT_NR_OF * GP_MM_RAM_EVENT_OFFSET);          \
} while (false)

#ifndef GP_WB_CHECK_OFFSET_PWM
#define GP_WB_CHECK_OFFSET_PWM(offset)
#endif //GP_WB_CHECK_OFFSET_PWM

/*
 * Defines the maximum variable size, used for alignment purposes
 */
#define GP_WB_MAX_MEMBER_SIZE 8

#ifdef __cplusplus
extern "C" {
#endif

#ifdef GP_DIVERSITY_GPHAL_INTERN

ALWAYS_INLINE void GP_WB_WRITE_U8(UIntPtr address, UInt8 val)
{
    *((volatile UInt8 *) address) = val;
}

ALWAYS_INLINE void GP_WB_WRITE_U16(UIntPtr address, UInt16 val)
{
    *((volatile UInt16 *) address) = val;
}

ALWAYS_INLINE void GP_WB_WRITE_U24(UIntPtr address, UInt32 val)
{
    *(volatile UInt16 *) address      = val;
    *(volatile UInt8 *) (address + 2) = val >> 16;
}

ALWAYS_INLINE void GP_WB_WRITE_U32(UIntPtr address, UInt32 val)
{
    *((volatile UInt32 *) address) = val;
}

ALWAYS_INLINE void GP_WB_WRITE_U40(UIntPtr address, UInt64 val)
{
    *(volatile UInt32 *)  address      = val;
    *(volatile UInt8 *) (address + 4) = val >> 32;
}

ALWAYS_INLINE void GP_WB_WRITE_U48(UIntPtr address, UInt64 val)
{
    *(volatile UInt32 *)  address      = val;
    *(volatile UInt16 *) (address + 4) = val >> 32;
}

ALWAYS_INLINE void GP_WB_WRITE_U64(UIntPtr address, UInt64 val)
{
    *((volatile UInt64 *) address) = val;
}

ALWAYS_INLINE UInt8 GP_WB_READ_U8(UIntPtr address)
{
    return *((volatile UInt8 *) address);
}

ALWAYS_INLINE UInt16 GP_WB_READ_U16(UIntPtr address)
{
    return *((volatile UInt16 *) address);
}

ALWAYS_INLINE UInt32 GP_WB_READ_U24(UIntPtr address)
{
    UInt32 val = *(volatile UInt16*) address;

    return val | *(volatile UInt8*) (address + 2) << 16;
}

ALWAYS_INLINE UInt32 GP_WB_READ_U32(UIntPtr address)
{
    return *((volatile UInt32 *) address);
}

ALWAYS_INLINE UInt64 GP_WB_READ_U40(UIntPtr address)
{
    UInt64 val = *(volatile UInt32*) address;

    return val |(UInt64) *(volatile UInt8*) (address + 4) << 32;
}

ALWAYS_INLINE UInt64 GP_WB_READ_U48(UIntPtr address)
{
    UInt64 val = *(volatile UInt32*)  address;

    return val | (UInt64) *(volatile UInt16*) (address + 4) << 32;
}

ALWAYS_INLINE UInt64 GP_WB_READ_U64(UIntPtr address)
{
    return *((volatile UInt64 *) address);
}

ALWAYS_INLINE void GP_WB_MWRITE_U8(UIntPtr address, UInt8 mask, UInt8 val)
{
    UInt16 tmp= GP_WB_READ_U8(address);
    tmp = (tmp & ~mask) | (val & mask);
    GP_WB_WRITE_U8(address, tmp);
}

#define BITBAND_OFFSET      0x02000000
#define BITBAND_REGION_MASK 0xFFF00000
#define BITBAND_ADDR_MASK   0x000FFFFF
#define ADDR_TO_BITBAND_REGION(addr)    (((addr) & BITBAND_REGION_MASK) + BITBAND_OFFSET)
#define ADDR_TO_BITBAND_OFFSET(addr)    (((addr) & BITBAND_ADDR_MASK) * 32)
#define GP_WB_ADDR_TO_BITBAND(addr)     (ADDR_TO_BITBAND_REGION(addr) + ADDR_TO_BITBAND_OFFSET(addr))

#define GP_WB_READ_U1(_address, _bit)           \
    GP_WB_READ_U32(GP_WB_ADDR_TO_BITBAND(_address) + (_bit) * 4)

#define GP_WB_MWRITE_U1(_address, _bit, _val)   \
    GP_WB_WRITE_U32(GP_WB_ADDR_TO_BITBAND(_address) + (_bit) * 4, (_val))

#else   /* GP_DIVERSITY_GPHAL_EXTERN */

ALWAYS_INLINE void GP_WB_WRITE_U8(UIntPtr address, UInt8 value)
{
    GP_HAL_WRITE_REG((gpHal_Address_t)address, value);
}

ALWAYS_INLINE void GP_WB_WRITE_U16(UIntPtr address, UInt16 value)
{
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address, (void*)&value, 2);
}

ALWAYS_INLINE void GP_WB_WRITE_U24(UIntPtr address, UInt32 value)
{
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address, (void*)&value, 3);
}

ALWAYS_INLINE void GP_WB_WRITE_U32(UIntPtr address, UInt32 value)
{
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address, (void*)&value, 4);
}

ALWAYS_INLINE void GP_WB_WRITE_U40(UIntPtr address, UInt64Struct_t value)
{
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address,   (void*)&value.LSB, 4);
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address+4, (void*)&value.MSB, 1);
}

ALWAYS_INLINE void GP_WB_WRITE_U48(UIntPtr address, UInt64Struct_t value)
{
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address,   (void*)&value.LSB, 4);
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address+4, (void*)&value.MSB, 2);
}

ALWAYS_INLINE void GP_WB_WRITE_U64(UIntPtr address, UInt64Struct_t value)
{
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address,   (void*)&value.LSB, 4);
    GP_HAL_WRITE_BYTE_STREAM((gpHal_Address_t)address+4, (void*)&value.MSB, 4);
}

ALWAYS_INLINE UInt16 GP_WB_READ_U16(UIntPtr address)
{
    UInt16 n;
    GP_HAL_READ_BYTE_STREAM((gpHal_Address_t)address, &n, 2);
    return n;
}

ALWAYS_INLINE UInt32 GP_WB_READ_U24(UIntPtr address)
{
    UInt32 n=0;
    GP_HAL_READ_BYTE_STREAM((gpHal_Address_t)address, (void*)&n, 3);
    return n;
}

ALWAYS_INLINE UInt32 GP_WB_READ_U32(UIntPtr address)
{
    UInt32 n;
    GP_HAL_READ_BYTE_STREAM((gpHal_Address_t)address, (void*)&n, 4);
    return n;
}

ALWAYS_INLINE UInt64 GP_WB_READ_U40(UIntPtr address)
{
    UInt64 n = 0;
    GP_HAL_READ_BYTE_STREAM((gpHal_Address_t)address,   (void*)&n, 5);
    return n;
}

ALWAYS_INLINE UInt64 GP_WB_READ_U48(UIntPtr address)
{
    UInt64 n = 0;
    GP_HAL_READ_BYTE_STREAM((gpHal_Address_t)address,   (void*)&n, 6);
    return n;
}

ALWAYS_INLINE UInt64 GP_WB_READ_U64(UIntPtr address)
{
    UInt64 n;
    GP_HAL_READ_BYTE_STREAM((gpHal_Address_t)address,   (void*)&n, 8);
    return n;
}

ALWAYS_INLINE UInt8 GP_WB_READ_U8(UIntPtr address)
{
    return GP_HAL_READ_REG((gpHal_Address_t)address);
}

ALWAYS_INLINE void GP_WB_MWRITE_U8(UIntPtr address, UInt8 mask, UInt8 value)
{
    GP_HAL_READMODIFYWRITE_REG((gpHal_Address_t)address, mask, value);
}

#define GP_WB_READ_U1(address, bit) ((GP_WB_READ_U8(address) >> bit) & 0x1)
#define GP_WB_MWRITE_U1(address, bit, val) GP_WB_MWRITE_U8((address), (1 << (bit)), (val) << (bit))

#endif  /* GP_DIVERSITY_GPHAL_INTERN */

#define GP_WB_READ_SER_U1(address, bit) GP_WB_READ_U1(address, bit)
#define GP_WB_READ_SER_U8(address)      GP_WB_READ_U8(address)
#define GP_WB_READ_SER_U16(address)     GP_WB_READ_U16(address)
#define GP_WB_READ_SER_U24(address)     GP_WB_READ_U24(address)
#define GP_WB_READ_SER_U32(address)     GP_WB_READ_U32(address)

#define GP_WB_RANGE_CHECK(val, max) do              \
    {                                               \
        COMPILE_TIME_ASSERT((val) <= (max));        \
        GP_ASSERT_DEV_INT((val) <= (max));          \
    } while (false)

#define GP_WB_RANGE_CHECK_SIGNED(val, min, max) do  \
    {                                               \
        COMPILE_TIME_ASSERT((val) >= (min));        \
        COMPILE_TIME_ASSERT((val) <= (max));        \
        GP_ASSERT_DEV_INT((val) >= (min));          \
        GP_ASSERT_DEV_INT((val) <= (max));          \
    } while (false)

#define GP_WB_SIZE_CHECK(type, var) do                                      \
{                                                                           \
    type GP_WB_SIZE_CHECK_dummy;                                            \
    Bool GP_WB_SIZE_CHECK_result = (&GP_WB_SIZE_CHECK_dummy == &(var));     \
    NOT_USED(GP_WB_SIZE_CHECK_result);                                      \
} while (false)
#define GP_WB_SIZE_CHECK_U8(var)  GP_WB_SIZE_CHECK(UInt8, var)
#define GP_WB_SIZE_CHECK_U16(var) GP_WB_SIZE_CHECK(UInt16, var)
#define GP_WB_SIZE_CHECK_U32(var) GP_WB_SIZE_CHECK(UInt32, var)
#define GP_WB_SIZE_CHECK_U64(var) GP_WB_SIZE_CHECK(UInt64, var)

/* sign extension/casting macros: extends the sign bit for len-bit
   numbers and signed casting */
#define GP_WB_S8(val, len)  ((Int8)(((val & (((UInt8)1 << len) - 1)) | (-(val & ((UInt8)1 << (len - 1)))))))
#define GP_WB_S16(val, len) ((Int16)(((val & (((UInt16)1 << len) - 1)) | (-(val & ((UInt16)1 << (len - 1)))))))
#define GP_WB_S24(val, len) ((Int32)(((val & (((UInt32)1 << len) - 1)) | (-(val & ((UInt32)1 << (len - 1)))))))
#define GP_WB_S32(val, len) ((Int32)(((val & (((UInt32)1 << len) - 1)) | (-(val & ((UInt32)1 << (len - 1)))))))

#ifdef __cplusplus
}
#endif
#endif  /* _GPHAL_KX_MSI_BASIC_H_ */
