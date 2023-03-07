#ifndef __CMSIS_COMPATIBLE_GCC_H
#define __CMSIS_COMPATIBLE_GCC_H

/* CMSIS compiler specific defines */
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __ALWAYS_INLINE
  #define __ALWAYS_INLINE                        inline __attribute__((always_inline))
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((noreturn))
#endif
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
#endif
#ifndef   __IRQ
  #define __IRQ                                  __attribute__((interrupt))
#endif
#ifndef   __IRQ_ALIGN64
  #define __IRQ_ALIGN64                          __attribute__((interrupt,aligned(64)))
#endif
#ifndef ALIGN4
#define ALIGN4									 __attribute((aligned (4)))
#endif
/**
  \brief   No Operation
  \details No Operation does nothing. This instruction can be used for code alignment purposes.
 */
//__attribute__((always_inline)) __STATIC_INLINE void __NOP(void)
//{
//  __ASM volatile ("nop");
//}
#define __NOP()                             __ASM volatile ("nop")       /* This implementation generates debug information */

/**
  \brief   Wait For Interrupt
  \details Wait For Interrupt is a hint instruction that suspends execution until one of a number of events occurs.
 */
//__attribute__((always_inline)) __STATIC_INLINE void __WFI(void)
//{
//  __ASM volatile ("wfi");
//}
#define __WFI()                             __ASM volatile ("wfi")       /* This implementation generates debug information */


/**
  \brief   Wait For Event
  \details Wait For Event is a hint instruction that permits the processor to enter
           a low-power state until one of a number of events occurs.
 */
//__attribute__((always_inline)) __STATIC_INLINE void __WFE(void)
//{
//  __ASM volatile ("wfe");
//}
#define __WFE()                             __ASM volatile ("wfe")       /* This implementation generates debug information */


/**
  \brief   Send Event
  \details Send Event is a hint instruction. It causes an event to be signaled to the CPU.
 */
//__attribute__((always_inline)) __STATIC_INLINE void __SEV(void)
//{
//  __ASM volatile ("sev");
//}
#define __SEV()                             __ASM volatile ("sev")       /* This implementation generates debug information */

__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq(void)
{
  __ASM volatile ("csrsi mstatus, 8");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_irq(void)
{
  __ASM volatile ("csrci mstatus, 8");
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value)
{
    //return __builtin_bswap32(value);
    uint32_t res = 0;

    res = (value << 24) | (value >> 24);
    res &= 0xFF0000FF; /* only for sure */
    res |= ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000);

    return res;
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value)
{
  return __builtin_bswap16(value);
}

extern void clic_enable_interrupt (uint32_t source);
extern void clic_disable_interrupt ( uint32_t source);
extern void clic_set_pending(uint32_t source);
extern void clic_clear_pending(uint32_t source);

#define NVIC_EnableIRQ                      clic_enable_interrupt

#define NVIC_DisableIRQ                     clic_disable_interrupt

#define NVIC_ClearPendingIRQ                clic_clear_pending

//#define __set_MSP(val)						__ASM volatile ("lw sp,0(%0)":: "r"(val))
#define __set_MSP(msp)                      __ASM volatile ("add sp, x0, %0":: "r"(msp))

#endif /* __CMSIS_COMPATIBLE_GCC_H */
