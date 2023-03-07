#ifndef __BL602_COMMON_H__
#define __BL602_COMMON_H__

#include "bl602.h"
#include "bflb_platform.h"

/** @addtogroup  BL606_Peripheral_Driver
 *  @{
 */

/** @addtogroup  COMMON
 *  @{
 */

/** @defgroup  COMMON_Public_Types
 *  @{
 */

/**
 * @brief Error type definition
 */
typedef enum 
{
  SUCCESS  = 0, 
  ERROR   = 1,
  TIMEOUT = 2,
}BL_Err_Type;

/**
 * @brief Functional type definition
 */
typedef enum 
{
  DISABLE  = 0, 
  ENABLE   = 1,
}BL_Fun_Type;

/**
 * @brief Status type definition
 */
typedef enum 
{
  RESET  = 0, 
  SET   = 1,
}BL_Sts_Type;

/**
 * @brief Mask type definition
 */
typedef enum 
{
  UNMASK = 0, 
  MASK = 1
}BL_Mask_Type;

/*@} end of group COMMON_Public_Types */

/** @defgroup  COMMON_Public_Constants
 *  @{
 */
 
/** @defgroup DRIVER_INT_PERIPH
 *  @{
 */
#define IS_INT_PERIPH(INT_PERIPH)       ((INT_PERIPH) < IRQn_LAST)

/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK     
 *  @{
 */
#define IS_BL_MASK_TYPE(type)          (((type) == MASK) || ((type) == UNMASK))

/*@} end of group COMMON_Public_Constants */

/** @defgroup  COMMON_Public_Macros
 *  @{
 */

#define ARCH_Delay_MS       BL602_Delay_MS
#define ARCH_MemCpy_Fast    BL602_MemCpy_Fast
/**
 * @brief Null Type definition
 */
#ifndef NULL
#define NULL   0
#endif

/**
 * @brief Debug definition
 */
//#define DEBUG   1

#ifdef  DEBUG
void check_failed(uint8_t *file, uint32_t line);
#define CHECK_PARAM(expr) ((expr) ? (void)0 : check_failed((uint8_t *)__FILE__, __LINE__))
#else
#define CHECK_PARAM(expr) ((void)0)
#endif /* DEBUG */



/**
 * @brief Logical status Type definition
 */
typedef enum 
{
  LOGIC_LO = 0, 
  LOGIC_HI = !LOGIC_LO
}LogicalStatus;

/**
 * @brief Active status Type definition
 */
typedef enum 
{
  DEACTIVE = 0, 
  ACTIVE = !DEACTIVE
}ActiveStatus;

/**  
 *  @brief Interrupt callback function type
 */
typedef void (intCallback_Type)(void);
typedef void( *pFunc )( void );

/*@} end of group DRIVER_Public_Macro */

/** @defgroup DRIVER_Public_FunctionDeclaration
 *  @brief DRIVER functions declaration
 *  @{
 */
void Interrupt_Handler_Register(IRQn_Type irq,pFunc interruptFun);
void ASM_Delay_Us(uint32_t core,uint32_t cnt);
void BL602_Delay_US(uint32_t cnt);
void BL602_Delay_MS(uint32_t cnt);
void *BL602_MemCpy(void *dst, const void *src, uint32_t n);
uint32_t *BL602_MemCpy4(uint32_t *dst, const uint32_t *src, uint32_t n);
void *BL602_MemCpy_Fast(void *pdst, const void *psrc, uint32_t n);
void* BL602_MemSet(void *s, uint8_t c, uint32_t n);
uint32_t *BL602_MemSet4(uint32_t *dst, const uint32_t val, uint32_t n);
int BL602_MemCmp(const void *s1, const void *s2, uint32_t n);


/*@} end of group DRIVER_COMMON  */

#endif /* __BL602_COMMON_H__ */


