#ifndef __H_ENDIAN_
#define __H_ENDIAN_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void put_cpu_le16(void *buf, uint16_t x)
{
    uint8_t *u8ptr;

    u8ptr = buf;
    u8ptr[0] = (uint8_t)x;
    u8ptr[1] = (uint8_t)(x >> 8);
}

static inline void put_cpu_le32(void *buf, uint32_t x)
{
    uint8_t *u8ptr;

    u8ptr = buf;
    u8ptr[0] = (uint8_t)x;
    u8ptr[1] = (uint8_t)(x >> 8);
    u8ptr[2] = (uint8_t)(x >> 16);
    u8ptr[3] = (uint8_t)(x >> 24);
}

static inline void put_cpu_le64(void *buf, uint64_t x)
{
    uint8_t *u8ptr;

    u8ptr = buf;
    u8ptr[0] = (uint8_t)x;
    u8ptr[1] = (uint8_t)(x >> 8);
    u8ptr[2] = (uint8_t)(x >> 16);
    u8ptr[3] = (uint8_t)(x >> 24);
    u8ptr[4] = (uint8_t)(x >> 32);
    u8ptr[5] = (uint8_t)(x >> 40);
    u8ptr[6] = (uint8_t)(x >> 48);
    u8ptr[7] = (uint8_t)(x >> 56);
}

static inline uint16_t get_cpu_le16(const void *buf)
{
    const uint8_t *u8ptr;
    uint16_t x;

    u8ptr = buf;
    x = u8ptr[0];
    x |= (uint16_t)u8ptr[1] << 8;

    return x;
}

static inline uint32_t get_cpu_le32(const void *buf)
{
    const uint8_t *u8ptr;
    uint32_t x;

    u8ptr = buf;
    x = u8ptr[0];
    x |= (uint32_t)u8ptr[1] << 8;
    x |= (uint32_t)u8ptr[2] << 16;
    x |= (uint32_t)u8ptr[3] << 24;

    return x;
}

static inline uint64_t get_cpu_le64(const void *buf)
{
    const uint8_t *u8ptr;
    uint64_t x;

    u8ptr = buf;
    x = u8ptr[0];
    x |= (uint64_t)u8ptr[1] << 8;
    x |= (uint64_t)u8ptr[2] << 16;
    x |= (uint64_t)u8ptr[3] << 24;
    x |= (uint64_t)u8ptr[4] << 32;
    x |= (uint64_t)u8ptr[5] << 40;
    x |= (uint64_t)u8ptr[6] << 48;
    x |= (uint64_t)u8ptr[7] << 56;

    return x;
}

#ifdef __cplusplus
}
#endif

#endif

