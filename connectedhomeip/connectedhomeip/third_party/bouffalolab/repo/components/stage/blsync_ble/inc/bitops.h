#ifndef __BITOPS_H
#define __BITOPS_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief bit移位 */
#ifndef BIT
#define BIT(bit)                  (1u << (bit))
#endif
/** \brief 值移位 */
#define SBF(value, field)         ((value) << (field))

/** \brief bit置位 */
#define BIT_SET(data, bit)        ((data) |= BIT(bit))

/** \brief bit清零 */
#define BIT_CLR(data, bit)        ((data) &= ~BIT(bit))

/** \brief bit置位, 根据 mask 指定的位 */
#define BIT_SET_MASK(data, mask)  ((data) |= (mask))

/** \brief bit清零, 根据 mask 指定的位 */
#define BIT_CLR_MASK(data, mask)  ((data) &= ~(mask))

/** \brief bit翻转 */
#define BIT_TOGGLE(data, bit)     ((data) ^= BIT(bit))

/** \brief bit修改 */
#define BIT_MODIFY(data, bit, value) \
    ((value) ? BIT_SET(data, bit) : BIT_CLR(data, bit))

/** \brief 测试bit是否置位 */
#define BIT_ISSET(data, bit)      ((data) & BIT(bit))

/** \brief 获取bit值 */
#define BIT_GET(data, bit)        (BIT_ISSET(data, bit) ? 1 : 0)

/** \brief 获取 n bits 掩码值 */
#define BITS_MASK(n)              (~((~0u) << (n)))

/** \brief 获取位段值 */
#define BITS_GET(data, start, len)  \
    (((data) >> (start)) & BITS_MASK(len))

/** \brief 设置位段值 */
#define BITS_SET(data, start, len, value) \
    ((data) = (((data) & ~SBF(BITS_MASK(len), (start))) | \
        SBF((value) & (BITS_MASK(len)), (start))))

#ifdef __cplusplus
}
#endif

#endif /* __BITOPS_H */

