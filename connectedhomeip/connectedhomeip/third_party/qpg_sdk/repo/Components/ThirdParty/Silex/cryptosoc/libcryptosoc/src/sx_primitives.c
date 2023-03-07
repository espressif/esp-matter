#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Implements the procedures to make primitive operations with
 *          the BA414E pub key
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "sx_primitives.h"
#include <stddef.h>
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba414e_config.h"
#include "sx_ecc_curves.h"

/* PRIMITIVE OPERATIONS */


/* Modular Addition: C = A + B mod N */
uint32_t modular_addition(uint8_t *a,
                           uint8_t *b,
                           uint8_t *modulo,
                           uint8_t *result,
                          uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_ADD , a, b, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Subtraction: C = A - B mod N */
uint32_t modular_substraction(uint8_t *a,
                               uint8_t *b,
                               uint8_t *modulo,
                               uint8_t *result,
                              uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_SUB, a, b, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Multiplication: C = A * B mod N */
uint32_t modular_multiplication(uint8_t *a,
                                 uint8_t *b,
                                 uint8_t *modulo,
                                 uint8_t *result,
                                uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_MULT_ODD, a, b, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Reduction: C = A mod N */
uint32_t modular_reduction(uint8_t *a,
                            uint8_t *modulo,
                            uint8_t *result,
                           uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_RED_ODD, NULL, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Division: C = A/B mod N */
uint32_t modular_division(uint8_t *a,
                           uint8_t *b,
                           uint8_t *modulo,
                           uint8_t *result,
                          uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_DIV_ODD, b, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Modular Inversion: C = 1/A mod N */
uint32_t  modular_inversion(uint8_t *a,
                             uint8_t *modulo,
                             uint8_t *result,
                            uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_INV_ODD , NULL, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}

/* Multiplication: C = A * B */
uint32_t multiplication(uint8_t *a,
                         uint8_t *b,
                         uint8_t *result,
                        uint32_t  size)
{
   return primitive_operation((uint8_t) BA414E_OPTYPE_MULT, a, b, NULL, result, size, 0xA, 0x8, 0x0, 0x6);
}


/* Modular Exponentiation    C = B^A mod N */
uint32_t modular_exponentiation(uint8_t *a,
                           uint8_t *b,
                           uint8_t *modulo,
                           uint8_t *result,
                          uint32_t  size)
{

   return primitive_operation((uint8_t) BA414E_OPTYPE_MOD_EXP, b, a, modulo, result, size, 0xA, 0x8, 0x0, 0x6);
}


/* a -> PtrA
 * b -> PtrB
 * modulo -> PtrN
 * result -> PtrC
 * */
uint32_t primitive_operation(uint32_t op,
                              uint8_t *a,
                              uint8_t *b,
                              uint8_t *modulo,
                              uint8_t *result,
                             uint32_t  size,
                              uint32_t PtrA,
                              uint32_t PtrB,
                              uint32_t PtrN,
                              uint32_t PtrC){
   uint32_t status;

   /* Set command register with operation, operand size, swap and field
    * This is done before transferring data into cryptoRAM because
    * endianness needs to be known before transferring data */
    ba414e_set_command(op, size, BA414E_BIGEND, BA414E_SELCUR_NO_ACCELERATOR);

    if(modulo != NULL) {
      mem2CryptoRAM_rev(block_t_convert(modulo, size), size, PtrN);
   }

   if(a != NULL) {
      mem2CryptoRAM_rev(block_t_convert(a, size), size, PtrA);
   }

   if(b != NULL) {
      mem2CryptoRAM_rev(block_t_convert(b, size), size, PtrB);
   }

   ba414e_set_config(PtrA, PtrB, PtrC, PtrN);

   /* Start BA414 */
   status = ba414e_start_wait_status();
   if (status)
      return status;

   CryptoRAM2mem_rev(block_t_convert(result, size), size, PtrC);

   return CRYPTOLIB_SUCCESS;
}



/* Modular Exponentiation    C = B^A mod N */
uint32_t modular_exponentiation_blk(block_t a,
                                 block_t b,
                                 block_t modulo,
                                 block_t result,
                                uint32_t  size)
{
   uint32_t status;
   uint32_t size_adapt = size;

   #if PK_CM_ENABLED
      if (PK_CM_RANDPROJ_MOD) {
         size_adapt += PK_CM_RAND_SIZE;
      }
   #endif

   ba414e_set_command(BA414E_OPTYPE_MOD_EXP, size_adapt, BA414E_BIGEND, BA414E_SELCUR_NO_ACCELERATOR);

   ba414e_set_config(BA414E_MEMLOC_10, BA414E_MEMLOC_11, BA414E_MEMLOC_12, BA414E_MEMLOC_0);

   mem2CryptoRAM_rev(modulo, size_adapt, BA414E_MEMLOC_0);
   mem2CryptoRAM_rev(a, size_adapt, BA414E_MEMLOC_10);
   mem2CryptoRAM_rev(b, size_adapt, BA414E_MEMLOC_11);

   /* Start BA414 */
   status = ba414e_start_wait_status();
   if (status)
      return status;

   CryptoRAM2mem_rev(result, size, BA414E_MEMLOC_12);

   return CRYPTOLIB_SUCCESS;
}



uint32_t ecc_montgomery_mult(block_t curve, uint32_t curve_flags, uint32_t size, block_t scalar, block_t point, block_t result)
{
   uint32_t status;
   uint8_t buffer_sc[ECC_MAX_KEY_SIZE];
   uint8_t buffer_pt[ECC_MAX_KEY_SIZE];

   if(point.addr == sx_getNULL_blk().addr)
      point = block_t_convert(curve.addr + 2*size, size);

   /* copy and process keys */
   memcpy_blkIn(buffer_pt, point, point.len);
   memcpy_blkIn(buffer_sc, scalar, scalar.len);

   sx_ecc_curve_t const *local_sx_ecc_curve_curve25519 = sx_find_ecp_curve(SX_ECP_DP_CURVE25519);
   sx_ecc_curve_t const *local_sx_ecc_curve_curve448   = sx_find_ecp_curve(SX_ECP_DP_CURVE448);

   if(curve.addr == local_sx_ecc_curve_curve25519->params.addr)
   {
      buffer_pt[31] &= 127;     //decodeUCoordinate
      buffer_sc[0] &= 248;      //decodeScalar25519 (clamp)
      buffer_sc[31] &= 127;     //decodeScalar25519 (clamp)
      buffer_sc[31] |= 64;      //decodeScalar25519 (clamp)
   }
   else if(curve.addr == local_sx_ecc_curve_curve448->params.addr)
   {
      buffer_sc[0] &= 252;       //decodeScalar448 (clamp)
      buffer_sc[55] |= 128;      //decodeScalar448 (clamp)
   }
   else
   {
      return CRYPTOLIB_INVALID_PARAM;
   }


   /* Set Command register - size 32 bytes - G(p) field */
   ba414e_set_command(BA414E_OPTYPE_MONTGOMERY_POINT_MULT, size, BA414E_LITTLEEND, curve_flags);
   mem2CryptoRAM(block_t_convert(buffer_pt, ECC_MAX_KEY_SIZE), size, BA414E_MEMLOC_2);
   mem2CryptoRAM(block_t_convert(buffer_sc, ECC_MAX_KEY_SIZE), size, BA414E_MEMLOC_3);

   /* Load parameters */
   mem2CryptoRAM(curve, size, BA414E_MEMLOC_0);
   if(!(curve.flags & BLOCK_S_CONST_ADDR)) curve.addr += size;
   mem2CryptoRAM(curve, size, BA414E_MEMLOC_1);

   /* Set Configuration register */
   ba414e_set_config(BA414E_MEMLOC_2, BA414E_MEMLOC_3, BA414E_MEMLOC_4, 0x0);

   // Start BA414 & get status when finished
   status = ba414e_start_wait_status();
   if (status)
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem(result, size, BA414E_MEMLOC_4);
   return CRYPTOLIB_SUCCESS;
}
