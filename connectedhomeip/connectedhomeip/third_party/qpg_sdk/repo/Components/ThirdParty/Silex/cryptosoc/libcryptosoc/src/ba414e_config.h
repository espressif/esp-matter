/**
 * @file
 * @brief This file contains useful functions to configure registers of the BA4414E
 *          PointerReg, CommandReg, ControlReg, StatusReg
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef BA414E_CONFIG_H_
#define BA414E_CONFIG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "ba414e_config_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "compiler_extentions.h"
#include "cryptolib_def.h"
#include "cryptolib_types.h"

/** @struct BA414ERegs_t
 *  @brief This structure represents the accessible registers of a BA414EP entity.
 *  See BA414EP's datasheet for details.
 */
typedef struct BA414ERegs_t
{
   volatile uint32_t   PointerReg;              /* BASE_ADDR + 0x0000 */
   volatile uint32_t   CommandReg;              /* BASE_ADDR + 0x0004 */
   volatile uint32_t   ControlReg;              /* BASE_ADDR + 0x0008 */
   volatile uint32_t   StatusReg;               /* BASE_ADDR + 0x000C */
   volatile uint32_t   VersionReg;              /* BASE_ADDR + 0x0010 */
   volatile uint32_t   TimerReg;                /* BASE_ADDR + 0x0014 */
} BA414ERegs_t;

/** @struct BA414E_ucode_t
 *  @brief This structure represents the accessible microcode of a BA414EP entity.
 *  See BA414EP's datasheet for details.
 */
typedef union BA414E_ucode_t
{
   volatile uint32_t   content[2048];
   struct ucode_info {
      volatile uint32_t   pointers[128];
      volatile uint32_t   ucodesize;
   } info;
} BA414E_ucode_t;

#define BA414E_UCODE_MAX_LENGTH 2046  // ucode content: memory size (2048) - redundancy value size (2)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_ba414e_config.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_ba414e_config.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/** @brief This function sets PtrA, PtrB and PtrC to PointerReg
 *  @param PtrA is the memory location of operand A (between 0x0 and 0xF)
 *  @param PtrB is the memory location of operand B (between 0x0 and 0xF)
 *  @param PtrC is the memory location of operand C (between 0x0 and 0xF)
 *  @param PtrN is the memory location of operand N (between 0x0 and 0xF)
 *  @return The PointerReg of the BA4414ERegs struct has been written with
 *  the appropriate pointer values.
 */
void ba414e_set_config(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN);

/**
 * @brief This function sets the BA414 CommandReg with corresponding parameters
 * @param op          the bits [6:0] of the CommandReg register (operation field)
 * @param bitlength   the operands size in bits
 * @param swap        swap the bytes on AHB/AXI4 interface
 * @param curve_flags enable accelerator for specific curve modulus
 */
void ba414e_set_command(uint32_t op, uint32_t bitlength, uint32_t swap, uint32_t curve_flags);

/**
 * @brief Function transfering data from memory to CryptoRAM, writing at the end address (when BA414 byte_swap is used). Data from src are padded from src.len to size with zeros.
 * @param src is a data block of the source that has to be copied (byte-addressing)
 * @param size is a the size of the data to copy
 * @param offset is the offset of the memory location that has to be copied
 * @return 0 if successful
 */
void mem2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset);

/**
 * @brief Function transfering data from CryptoRAM to memory, writing at the end address (when BA414 byte_swap is used)
 * @param dst is a block_t where the source has to be copied to
 * @param size is a the size of the data to copy
 * @param offset is the offset of the memory location that has to be copied
 * @return 0 if successful
 */
void CryptoRAM2mem_rev(block_t dst, uint32_t size, uint32_t offset);

/**
 * @brief Function transfering data from memory to CryptoRAM, writing at the start address (when BA414 byte_swap is not used)
 * @param src is a data block of the source that has to be copied (byte-addressing)
 * @param size is a the size of the data to copy
 * @param offset is the offset of the memory location that has to be copied
 * @return 0 if successful
 */
void mem2CryptoRAM(block_t src, uint32_t size, uint32_t offset);

/**
 * @brief Function transfering data from CryptoRAM to memory, writing at the start address (when BA414 byte_swap is not used)
 * @param dst is a block_t where the source has to be copied to
 * @param size is a the size of the data to copy
 * @param offset is the offset of the memory location that has to be copied
 * @return 0 if successful
 */
void CryptoRAM2mem(block_t dst, uint32_t size, uint32_t offset);

/**
 * @brief Function is used to start the PK, wait for an interrupt, and read & return the status register
 * @return the contents of the status register as uint32_t.
 */
uint32_t ba414e_start_wait_status(void) CHECK_RESULT;

/**
 * Copy the BA414 curve parameters to CryptoRAM
 * @param curve      Parameters to copy to the cryptoRAM
 * @param size       Size of the parameter
 * @param byte_swap  1 if byte_swap enabled in BA414, 0 otherwise
 * @param gen        1 if we want to load the generator
 * @return status 0 if successful
 */
uint32_t ba414e_load_curve(block_t curve, uint32_t size, uint32_t byte_swap, uint32_t gen)  CHECK_RESULT;


/**
 * generate load value in and compute \p out = \p in mod N. Precondiction: N  must be already loaded in \p nloc location (\p size bytes).
 * @param outloc  location to place \p out
 * @param nloc   location for modulo N
 * @param size    size of \p in
 * @param in      input value
 * @param out     copy result (if needed)
 * @param flags
  @return CRYPTOLIB_SUCCESS if successful
 */
uint32_t ba414e_load_and_modN(uint8_t outloc, uint8_t nloc, uint32_t size, block_t in, block_t out, uint32_t flags) CHECK_RESULT;

/**
 * Copy ECC point to CryptoRAM
 * @param src block_t to point on the curve
 * @param size size of the operand * @param offset position of the first coordinate in cryptoRAM
 */
void point2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset);

/**
 * Copy ECC point from CryptoRAM
 * @param dst block_t to point container
 * @param size size of the operand
 * @param offset position of the first coordinate in cryptoRAM
 */
void CryptoRAM2point_rev(block_t dst, uint32_t size, uint32_t offset);

#if defined(GP_DIVERSITY_JUMPTABLES)
void ba414e_start(void);
uint32_t ba414e_get_status(void);
uint32_t ba414e_wait_status(void);
#endif // GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */


/* For register CFG */
#define BA414E_PTR_OPPTRN_LSB  (24)
#define BA414E_PTR_OPPTRN_MASK (0x0F000000)
#define BA414E_PTR_OPPTRC_LSB  (16)
#define BA414E_PTR_OPPTRC_MASK (0x000F0000)
#define BA414E_PTR_OPPTRB_LSB  (8)
#define BA414E_PTR_OPPTRB_MASK (0x00000F00)
#define BA414E_PTR_OPPTRA_LSB  (0)
#define BA414E_PTR_OPPTRA_MASK (0x0000000F)

/* For register CMD */
#define BA414E_CMD_CALCR2_LSB      (31)
#define BA414E_CMD_FLAGB_LSB       (30)
#define BA414E_CMD_FLAGA_LSB       (29)
#define BA414E_CMD_SWAP_LSB        (28)
#define BA414E_CMD_RANDPR_LSB      (25)
#define BA414E_CMD_RANDKE_LSB      (24)
#define BA414E_CMD_SELCUR_LSB      (20)
#define BA414E_CMD_OPSIZE_LSB      (8)
#define BA414E_CMD_OPFLD_LSB       (7)
#define BA414E_CMD_OPTYPE_LSB      (0)
#define BA414E_CMD_CALCR2_MASK     (0x80000000)
#define BA414E_CMD_FLAGB_MASK      (0x40000000)
#define BA414E_CMD_FLAGA_MASK      (0x20000000)
#define BA414E_CMD_SWAP_MASK       (0x10000000)
#define BA414E_CMD_RANDPR_MASK     (0x02000000)
#define BA414E_CMD_RANDKE_MASK     (0x01000000)
#define BA414E_CMD_SELCUR_MASK     (0x00F00000)
#define BA414E_CMD_OPSIZE_MASK     (0x0001FF00)
#define BA414E_CMD_OPFLD_MASK      (0x00000080)
#define BA414E_CMD_OPTYPE_MASK     (0x0000007F)

#define BA414E_CMD_CALCR2(v)  (((v) << BA414E_CMD_CALCR2_LSB)& BA414E_CMD_CALCR2_MASK)
#define BA414E_CMD_FLAGB(v)   (((v) <<  BA414E_CMD_FLAGB_LSB)&  BA414E_CMD_FLAGB_MASK)
#define BA414E_CMD_FLAGA(v)   (((v) <<  BA414E_CMD_FLAGA_LSB)&  BA414E_CMD_FLAGA_MASK)
#define BA414E_CMD_SWAP(v)    (((v) <<   BA414E_CMD_SWAP_LSB)&   BA414E_CMD_SWAP_MASK)
#define BA414E_CMD_RANDPR(v)  (((v) << BA414E_CMD_RANDPR_LSB)& BA414E_CMD_RANDPR_MASK)
#define BA414E_CMD_RANDKE(v)  (((v) << BA414E_CMD_RANDKE_LSB)& BA414E_CMD_RANDKE_MASK)
#define BA414E_CMD_OPSIZE(v)  (((v) << BA414E_CMD_OPSIZE_LSB)& BA414E_CMD_OPSIZE_MASK)
#define BA414E_CMD_OPFLD(v)   (((v) <<  BA414E_CMD_OPFLD_LSB)&  BA414E_CMD_OPFLD_MASK)
#define BA414E_CMD_OPTYPE(v)  (((v) << BA414E_CMD_OPTYPE_LSB)& BA414E_CMD_OPTYPE_MASK)


/* For operation type */
/*--- Primitive arithmetic operations GF(p) and GF(2^m) ---*/
/** @brief BA414E operation type: modular addition */
#define BA414E_OPTYPE_MOD_ADD                 (0x01)      /**< A+B mod N */
/** @brief BA414E operation type: modular subtraction */
#define BA414E_OPTYPE_MOD_SUB                 (0x02)      /**< A-B mod N */
/** @brief BA414E operation type: modular multiplication (odd N) */
#define BA414E_OPTYPE_MOD_MULT_ODD            (0x03)      /**< A*B mod N */
/** @brief BA414E operation type: modular reduction (odd N) */
#define BA414E_OPTYPE_MOD_RED_ODD             (0x04)      /**< B mod N */
/** @brief BA414E operation type: modular division (odd N) */
#define BA414E_OPTYPE_MOD_DIV_ODD             (0x05)      /**< A/B mod N */
/** @brief BA414E operation type: modular inversion (odd N) */
#define BA414E_OPTYPE_MOD_INV_ODD             (0x06)      /**< 1/B mod N */
/** @brief BA414E operation type: multiplication */
#define BA414E_OPTYPE_MULT                    (0x08)      /**< A*B */
/** @brief BA414E operation type: modular inversion (even N and GF(p) only) */
#define BA414E_OPTYPE_MOD_INV_EVEN            (0x09)      /**< 1/B mod N */
/** @brief BA414E operation type: modular reduction (even N and GF(p) only) */
#define BA414E_OPTYPE_MOD_RED_EVEN            (0x0A)      /**< B mod N */
/** @brief BA414E operation type: clear data memory */
#define BA414E_OPTYPE_CLEAR_MEM               (0x0F)

/*--- High-level RSA, CRT & DSA Operations - GF(p) only ---*/
/** @brief BA414E operation type: modular exponentiation */
#define BA414E_OPTYPE_MOD_EXP                 (0x10)
/** @brief BA414E operation type: RSA priv key generation */
#define BA414E_OPTYPE_RSA_PK_GEN              (0x11)
/** @brief BA414E operation type: RSA CRT key parameter generation */
#define BA414E_OPTYPE_RSA_CRT_GEN             (0x12)
/** @brief BA414E operation type: RSA CRT decryption */
#define BA414E_OPTYPE_RSA_CRT_DEC             (0x13)
/** @brief BA414E operation type: RSA encryption */
#define BA414E_OPTYPE_RSA_ENC                 (0x14)
/** @brief BA414E operation type: RSA decryption */
#define BA414E_OPTYPE_RSA_DEC                 (0x15)
/** @brief BA414E operation type: RSA signature generation */
#define BA414E_OPTYPE_RSA_SIGN_GEN            (0x16)
/** @brief BA414E operation type: RSA signature verification */
#define BA414E_OPTYPE_RSA_SIGN_VERIF          (0x17)
/** @brief BA414E operation type: DSA key generation */
#define BA414E_OPTYPE_DSA_KEY_GEN             (0x18)
/** @brief BA414E operation type: DSA signature generation */
#define BA414E_OPTYPE_DSA_SIGN_GEN            (0x19)
/** @brief BA414E operation type: DSA signature verification */
#define BA414E_OPTYPE_DSA_SIGN_VERIF          (0x1A)
/** @brief BA414E operation type: SRP server session key */
#define BA414E_OPTYPE_SRP_SERVER_KEY          (0x1B)
/** @brief BA414E operation type: SRP client session key */
#define BA414E_OPTYPE_SRP_CLIENT_KEY          (0x1C)
/** @brief BA414E operation type: RSA half CRT recombination */
#define BA414E_OPTYPE_RSA_HALFCRT_RECOMBI     (0x1D)
/** @brief BA414E operation type: SRP server pub key */
#define BA414E_OPTYPE_SRP_SERVER_PK           (0x1E)
/** @brief BA414E operation type: RSA half CRT decryption */
#define BA414E_OPTYPE_RSA_HALFCRT_DEC         (0x1F)

/*--- Primitive ECC & Check Point Operations GF(p) & GF(2m) ---*/
/** @brief BA414E operation type: ECC point doubling */
#define BA414E_OPTYPE_ECC_POINT_DOUBLE        (0x20)
/** @brief BA414E operation type: ECC point addition */
#define BA414E_OPTYPE_ECC_POINT_ADD           (0x21)
/** @brief BA414E operation type: ECC point multiplication */
#define BA414E_OPTYPE_ECC_POINT_MULT          (0x22)
/** @brief BA414E operation type: ECC check a & b */
#define BA414E_OPTYPE_ECC_CHECK_AB            (0x23)
/** @brief BA414E operation type: ECC check n!=q */
#define BA414E_OPTYPE_ECC_CHECK_NNOTQ         (0x24)
/** @brief BA414E operation type: ECC check x<q, y<q */
#define BA414E_OPTYPE_ECC_CHECK_XY            (0x25)
/** @brief BA414E operation type: ECC check point on curve */
#define BA414E_OPTYPE_ECC_CHECK_POINTONCURVE  (0x26)
/** @brief BA414E operation type: Montgomery curve point multiplication */
#define BA414E_OPTYPE_MONTGOMERY_POINT_MULT   (0x28)

/*--- High-level ECC â€“ ECDSA Operations GF(p) & GF(2m) ---*/
/** @brief BA414E operation type: ECDSA signature generation */
#define BA414E_OPTYPE_ECDSA_SIGN_GEN          (0x30)
/** @brief BA414E operation type: ECDSA signature verification */
#define BA414E_OPTYPE_ECDSA_SIGN_VERIF        (0x31)
/** @brief BA414E operation type: ECDSA domain parameters validation */
#define BA414E_OPTYPE_ECDSA_PARAM_EVAL        (0x32)
/** @brief BA414E operation type: ECKCDSA pub key generation */
#define BA414E_OPTYPE_ECKCDSA_PK_GEN          (0x33)
/** @brief BA414E operation type: ECKCDSA signature generation */
#define BA414E_OPTYPE_ECKCDSA_SIGN_GEN        (0x34)
/** @brief BA414E operation type: ECKCDSA signature verification */
#define BA414E_OPTYPE_ECKCDSA_SIGN_VERIF      (0x35)
/** @brief BA414E operation type: JPAKE ZKP generation*/
#define BA414E_OPTYPE_JPAKE_ZKP_GEN           (0x36)
/** @brief BA414E operation type: JPAKE ZKP verification */
#define BA414E_OPTYPE_JPAKE_ZKP_VERIF         (0x37)
/** @brief BA414E operation type: JPAKE 3-point addition */
#define BA414E_OPTYPE_JPAKE_3POINT_ADD        (0x38)
/** @brief BA414E operation type: JPAKE session key */
#define BA414E_OPTYPE_JPAKE_SESSION_KEY       (0x39)
/** @brief BA414E operation type: JPAKE step 2 */
#define BA414E_OPTYPE_JPAKE_STEP2             (0x3A)
/** @brief BA414E operation type: EdDSA point multiplication */
#define BA414E_OPTYPE_EDDSA_POINT_MULT        (0x3B)
/** @brief BA414E operation type: EdDSA signature generation */
#define BA414E_OPTYPE_EDDSA_SIGN_GEN          (0x3C)
/** @brief BA414E operation type: EdDSA signature verification */
#define BA414E_OPTYPE_EDDSA_SIGN_VERIF        (0x3D)
/** @brief BA414E operation type: ECMQV session key */
#define BA414E_OPTYPE_ECMQV_SESSION_KEY       (0x3E)

/*--- Primality Test â€“ Rabin-Miller ---*/
/** @brief BA414E operation type: round Rabin-Miller */
#define BA414E_OPTYPE_RM_ROUND                (0x40)
/** @brief BA414E operation type: init Rabin-Miller */
#define BA414E_OPTYPE_RM_INIT                 (0x41)


#if PK_CM_ENABLED
   /** @brief BA414E modular operations involving a secret exponent */
   #define BA414E_IS_OP_WITH_SECRET_MOD(op)      ((op == BA414E_OPTYPE_MOD_EXP)              || \
                                                  (op == BA414E_OPTYPE_RSA_CRT_DEC)          || \
                                                  (op == BA414E_OPTYPE_RSA_DEC)              || \
                                                  (op == BA414E_OPTYPE_RSA_SIGN_GEN)         || \
                                                  (op == BA414E_OPTYPE_RSA_HALFCRT_RECOMBI)  || \
                                                  (op == BA414E_OPTYPE_RSA_HALFCRT_DEC)      || \
                                                  (op == BA414E_OPTYPE_SRP_SERVER_KEY)       || \
                                                  (op == BA414E_OPTYPE_SRP_CLIENT_KEY)       || \
                                                  (op == BA414E_OPTYPE_SRP_SERVER_PK)        || \
                                                  (op == BA414E_OPTYPE_DSA_KEY_GEN)          || \
                                                  (op == BA414E_OPTYPE_DSA_SIGN_GEN))

   /** @brief BA414E ECC operations involving point mult */
   #define BA414E_IS_OP_WITH_SECRET_ECC(op)      ((op == BA414E_OPTYPE_ECC_POINT_MULT)          || \
                                                  (op == BA414E_OPTYPE_MONTGOMERY_POINT_MULT)   || \
                                                  (op == BA414E_OPTYPE_ECDSA_SIGN_GEN)          || \
                                                  (op == BA414E_OPTYPE_ECKCDSA_PK_GEN)          || \
                                                  (op == BA414E_OPTYPE_ECKCDSA_SIGN_GEN)        || \
                                                  (op == BA414E_OPTYPE_JPAKE_ZKP_GEN)           || \
                                                  (op == BA414E_OPTYPE_JPAKE_ZKP_VERIF)         || \
                                                  (op == BA414E_OPTYPE_JPAKE_3POINT_ADD)        || \
                                                  (op == BA414E_OPTYPE_JPAKE_SESSION_KEY)       || \
                                                  (op == BA414E_OPTYPE_JPAKE_STEP2)             || \
                                                  (op == BA414E_OPTYPE_ECMQV_SESSION_KEY)       || \
                                                  (op == BA414E_OPTYPE_EDDSA_POINT_MULT)        || \
                                                  (op == BA414E_OPTYPE_EDDSA_SIGN_GEN))
#endif

/* For BA414E accelerator curve */
#define BA414E_SELCUR_NO_ACCELERATOR   (0<<BA414E_CMD_SELCUR_LSB)
#define BA414E_SELCUR_ACCEL_P256       (1<<BA414E_CMD_SELCUR_LSB)
#define BA414E_SELCUR_ACCEL_P384       (2<<BA414E_CMD_SELCUR_LSB)
#define BA414E_SELCUR_ACCEL_P521       (3<<BA414E_CMD_SELCUR_LSB)
#define BA414E_SELCUR_ACCEL_P192       (4<<BA414E_CMD_SELCUR_LSB)
#define BA414E_SELCUR_ACCEL_25519      (5<<BA414E_CMD_SELCUR_LSB)

#define BA414E_OPFLD_PRIME      (0)
#define BA414E_OPFLD_BINARY     (1)

#define BA414E_LITTLEEND        0
#define BA414E_BIGEND           1

/* For register CTL */
#define BA414E_CTL_CLIRQ_LSB  (1)
#define BA414E_CTL_CLIRQ_MASK (0x00000002)
#define BA414E_CTL_START_LSB  (0)
#define BA414E_CTL_START_MASK (0x00000001)

/* For register STS */
#define BA414E_STS_IRQ_LSB        (17)
#define BA414E_STS_IRQ_MASK       (0x00020000)
#define BA414E_STS_BUSY_LSB       (16)
#define BA414E_STS_BUSY_MASK      (0x00010000)
#define BA414E_STS_PRIM_LSB       (12)
#define BA414E_STS_PRIM_MASK      (0x00001000)
#define BA414E_STS_NINV_LSB       (11)
#define BA414E_STS_NINV_MASK      (0x00000800)
#define BA414E_STS_ABNV_LSB       (10)
#define BA414E_STS_ABNV_MASK      (0x00000400)
#define BA414E_STS_SINV_LSB       (9)
#define BA414E_STS_SINV_MASK      (0x00000200)
#define BA414E_STS_NOTIMPL_LSB    (8)
#define BA414E_STS_NOTIMPL_MASK   (0x00000100)
#define BA414E_STS_NNV_LSB        (7)
#define BA414E_STS_NNV_MASK       (0x00000080)
#define BA414E_STS_CPNV_LSB       (6)
#define BA414E_STS_CPNV_MASK      (0x00000040)
#define BA414E_STS_PINF_LSB       (5)
#define BA414E_STS_PINF_MASK      (0x00000020)
#define BA414E_STS_PNCU_LSB       (4)
#define BA414E_STS_PNCU_MASK      (0x00000010)
#define BA414E_STS_FADD_LSB       (0)
#define BA414E_STS_FADD_MASK      (0x0000000F)

#define BA414E_STS_ERROR_MASK     (0x0000FFF0)





// BA414 memory locations
#define BA414E_MEMLOC_0    0x0
#define BA414E_MEMLOC_1    0x1
#define BA414E_MEMLOC_2    0x2
#define BA414E_MEMLOC_3    0x3
#define BA414E_MEMLOC_4    0x4
#define BA414E_MEMLOC_5    0x5
#define BA414E_MEMLOC_6    0x6
#define BA414E_MEMLOC_7    0x7
#define BA414E_MEMLOC_8    0x8
#define BA414E_MEMLOC_9    0x9
#define BA414E_MEMLOC_10   0xA
#define BA414E_MEMLOC_11   0xB
#define BA414E_MEMLOC_12   0xC
#define BA414E_MEMLOC_13   0xD
#define BA414E_MEMLOC_14   0xE
#define BA414E_MEMLOC_15   0xF

#define BA414E_ADDR_MEMLOC(loc, size) ((uint8_t*)(ADDR_BA414E_CRYPTORAM + \
         (((loc) + 1) << 9) - (size) ))

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
