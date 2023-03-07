/***************************************************************************//**
 * @file
 * @brief File System - Hamming Code Calculation
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note     (1) Hamming module is endianness-agnostic. When calling API functions, it is possible
 *               to pass ECCs or buffer data with any endianness, as long as it is consistent.
 *
 * @note     (2) This module uses the type CPU_DATA for variables that need to be at least
 *               16-bit. To avoid overflow of those variables, this module should not be used
 *               with 8-bit CPUs.
 *               - (a) If using a 16-bit CPU, buffer data length should be limited to  65536 octets
 *                     (see HAMMING_LEN_OCTET_BUF_MAX) and ECC length should be limited to 4 octets
 *                     (see HAMMING_LEN_OCTET_ECC) to avoid overflow.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <fs_storage_cfg.h>

#include  <fs/source/shared/crc/ecc_hamming.h>
#include  <fs/source/shared/crc/crc_utils.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                                    RTOS_CFG_MODULE_FS

#define  HAMMING_NBR_CORRECTABLE_BITS                      1u

#define  HAMMING_LEN_OCTET_ECC                             4u
#define  HAMMING_LEN_OCTET_BUF_MAX                      8192u   // Max buf len must limit the size of ECC to 4 octets.
#define  HAMMING_LEN_OCTET_BUF_MIN                         1u
#define  HAMMING_LEN_OCTET_PER_LOOP_ITER                  32u

#define  HAMMING_DIFF_CNT_EXPECTED                ((DEF_OCTET_NBR_BITS * HAMMING_LEN_OCTET_ECC) / 2u)

#define  HAMMING_MASK_EVEN_COL_1                  0x55555555u   // Binary: 01010101010101010101010101010101.
#define  HAMMING_MASK_ODD_COL_1                   0xAAAAAAAAu   // Binary: 10101010101010101010101010101010.
#define  HAMMING_MASK_ODD_COL_2                   0xCCCCCCCCu   // Binary: 11001100110011001100110011001100.
#define  HAMMING_MASK_ODD_COL_4                   0xF0F0F0F0u   // Binary: 11110000111100001111000011110000.

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  HAMMING_MASK_ODD_LINE_1                  0x00FF00FFu   // Binary: 00000000111111110000000011111111.
#define  HAMMING_MASK_ODD_LINE_2                  0x0000FFFFu   // Binary: 00000000000000001111111111111111.
#else
#define  HAMMING_MASK_ODD_LINE_1                  0xFF00FF00u   // Binary: 11111111000000001111111100000000.
#define  HAMMING_MASK_ODD_LINE_2                  0xFFFF0000u   // Binary: 11111111111111110000000000000000.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const CPU_INT08U Hamming_BitCnt[256] = {
  0u, 1u, 1u, 2u, 1u, 2u, 2u, 3u, 1u, 2u, 2u, 3u, 2u, 3u, 3u, 4u,
  1u, 2u, 2u, 3u, 2u, 3u, 3u, 4u, 2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u,
  1u, 2u, 2u, 3u, 2u, 3u, 3u, 4u, 2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u,
  2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u, 3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u,
  1u, 2u, 2u, 3u, 2u, 3u, 3u, 4u, 2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u,
  2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u, 3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u,
  2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u, 3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u,
  3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u, 4u, 5u, 5u, 6u, 5u, 6u, 6u, 7u,
  1u, 2u, 2u, 3u, 2u, 3u, 3u, 4u, 2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u,
  2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u, 3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u,
  2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u, 3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u,
  3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u, 4u, 5u, 5u, 6u, 5u, 6u, 6u, 7u,
  2u, 3u, 3u, 4u, 3u, 4u, 4u, 5u, 3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u,
  3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u, 4u, 5u, 5u, 6u, 5u, 6u, 6u, 7u,
  3u, 4u, 4u, 5u, 4u, 5u, 5u, 6u, 4u, 5u, 5u, 6u, 5u, 6u, 6u, 7u,
  4u, 5u, 5u, 6u, 5u, 6u, 6u, 7u, 5u, 6u, 6u, 7u, 6u, 7u, 7u, 8u
};

const ECC_CALC Hamming_ECC = {
  HAMMING_LEN_OCTET_BUF_MIN,
  HAMMING_LEN_OCTET_BUF_MAX,
  HAMMING_LEN_OCTET_ECC,
  HAMMING_NBR_CORRECTABLE_BITS,
  &Hamming_Calc,
  &Hamming_Chk,
  &Hamming_Correct
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Calc err loc.
static void Hamming_CalcErrLoc(CPU_INT32U  ecc_xor,
                               ECC_ERR_LOC *p_err_loc);

//                                                                 Calc par on 32-bit val.
static CPU_INT32U Hamming_ParCalc_32(CPU_INT32U data_32);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Hamming_Calc()
 *
 * @brief    Calculate Hamming code.
 *
 * @param    p_buf       Pointer to buffer that contains the data (see Note #1).
 *
 * @param    len         Length of buffer, in octets (see Note #2).
 *
 * @param    p_buf_ext   Pointer to extension buffer that contains the additional data (see Note #1).
 *
 * @param    len_ext     Length of extension buffer, in octets (see Note #2).
 *
 * @param    p_ecc       Pointer to 4-octet   buffer that will receive Hamming code (see Note #3).
 *
 * @note     (1) The calculation on 'p_buf' is optimized for 'CPU_INT32U'-aligned buffers, though
 *               any buffer alignment is acceptable.
 *
 *               The calculation on 'p_buf_ext' is significantly slower, but is allowed for
 *               convenience when the Hamming code must cover data in 2 buffers. If not needed,
 *               a NULL pointer can be passed as the 'p_buf_ext'.
 *
 * @note     (2) The parameter 'len' must be a multiple of HAMMING_LEN_OCTET_PER_LOOP_ITER.
 *
 *               The sum of 'len' & 'len_ext' must be between HAMMING_LEN_OCTET_BUF_MIN and
 *               HAMMING_LEN_OCTET_BUF_MAX octets.
 *
 *               To take advantage of the fast calculation performed on 'p_buf', a single buffer
 *               with a 'len' that is not a multiple of HAMMING_LEN_OCTET_PER_LOOP_ITER should
 *               be split in 2 buffers. The following example shows how to call Hamming_Calc()
 *               for a single buffer with a 'len' that is not a multiple of
 *               HAMMING_LEN_OCTET_PER_LOOP_ITER. Split the single buffer into Hamming_Calc()'s
 *               primary & extension buffers, 'p_buf' & 'p_buf_ext' respectively:
 *
 *                       len         = 100;
 *                       len_buf     = len / HAMMING_LEN_OCTET_PER_LOOP_ITER;
 *                       len_buf_ext = len % HAMMING_LEN_OCTET_PER_LOOP_ITER;
 *                       p_buf_ext   = (CPU_INT08U *)p_buf + len_buf;
 *
 *                       Hamming_Calc(p_buf,
 *                                   p_buf_ext,
 *                                   len_buf,
 *                                   len_buf_ext,
 *                                   p_ecc,
 *                                   p_err);
 *
 * @note     (3) The return parameter 'p_ecc' must point to a valid 4-octet buffer; this buffer
 *               need not be 'CPU_INT32U'-aligned.
 *
 *               Data buffers larger than HAMMING_LEN_OCTET_BUF_MAX should be divided into smaller
 *               segments, with the ECCs calculated for the individual segments concatenated to
 *               form the ECC for the entire buffer.
 *
 *               The number of relevant bits in the error correction code (*p_ecc) depends on
 *               the sum of 'len' & 'len_ext', according to following table. The application
 *               need not to store irrelevant data.
 *
 *                    ---------------------------+---------------------------
 *                   |  Total length of buffers  |  Number of relevant bits  |
 *                   |       (in octets)         |       (lsb) in ECC        |
 *                    ---------------------------+---------------------------
 *                   |                  1        |             6             |
 *                   |                  2        |             8             |
 *                   |          3  -    4        |            10             |
 *                   |          5  -    8        |            12             |
 *                   |          9  -   16        |            14             |
 *                   |         17  -   32        |            16             |
 *                   |         33  -   64        |            18             |
 *                   |         65  -  128        |            20             |
 *                   |        129  -  256        |            22             |
 *                   |        257  -  512        |            24             |
 *                   |        513  - 1024        |            26             |
 *                   |       1023  - 2048        |            28             |
 *                   |       2049  - 4096        |            30             |
 *                   |       4097  - 8192        |            32             |
 *                    ---------------------------+---------------------------
 *
 * @note     (4) A Hamming code is an error-correcting code (ECC) that can correct single-bit errors &
 *               detect multiple-bit errors.  This implementation detects double-bit errors.
 *
 *           - (a) In a Hamming code calculation, parities are calculated for overlapping bit
 *                   groups in a data buffer.
 *
 *           - (b) Intermediate parities are calculated for each word (the line or word parities) &
 *                   for each bit position in all words (the column or bit parities).
 *
 *           - (c) The intermediate parity values are used to calculate the final parity bits.
 *
 *               - (1) Thirteen odd parity bits are calculated from the line/octet parities :
 *
 *                   - (a) L0001o is the par bit of lines   1,   3, ...       8189,      8191 (odd                 lines)
 *                   - (b) L0002o is the par bit of lines 2-3, 6-7, ...  8186-8187, 8190-8191 (odd  groups of    2 lines)
 *                       .
 *                       .
 *                   - (m) L4096o is the par bit of lines 4096-8191                           (odd  groups of 4096 lines)
 *
 *               - (2) Thirteen even parity bits are calculated from the line/octet parities :
 *
 *                   - (a) L0001e is the par bit of lines   0,   2, ...       8188,      8190 (even                lines)
 *                   - (b) L0002e is the par bit of lines 0-1, 4-5, ...  8184-8185, 8188-8189 (even groups of    2 lines)
 *                       .
 *                       .
 *                   - (m) L4096e is the par bit of lines   0-4095                            (even groups of 4096 lines)
 *
 *               - (3) Three odd parity bits are calculated from the column/bit parities :
 *
 *                   - (a) C1o  is the par bit of columns   1,   3,   5,   7                 (odd              columns)
 *                   - (b) C2o  is the par bit of columns 2-3, 6-7                           (odd  groups of 2 columns)
 *                   - (c) C4o  is the par bit of columns 4-7                                (odd  groups of 4 columns)
 *
 *               - (4) Three even parity bits are calculated from the column/bit parities :
 *
 *                   - (a) C1e  is the par bit of columns   2,   4,   6,   8                 (even             columns)
 *                   - (b) C2e  is the par bit of columns 0-1, 4-5                           (even groups of 2 columns)
 *                   - (c) C4e  is the par bit of columns 0-3                                (even groups of 4 columns)
 *
 *           - (d) 4 8-bit values are formed from these parity bits :
 *
 *                       hamming[0]  BITS  7- 0:    L0001o  L0001e     C4o     C4e     C2o     C2e     C1o     C1e
 *                       hamming[1]  BITS 15- 8:    L0016o  L0016e  L0008o  L0008e  L0004o  L0004e  L0002o  L0002e
 *                       hamming[2]  BITS 23-16:    L0256o  L0256e  L0128o  L0128e  L0064o  L0064e  L0032o  L0032e
 *                       hamming[3]  BITS 31-24:    L4096o  L4096e  L2048o  L2048e  L1024o  L1024e  L0512o  L0512e
 *           ----------------------------------------------------------------------------------------------------------+
 *                                                                                                                   |
 *                                                               LINE / OCTET      CONTRIBUTING TO THE FOLLOWING      |
 *                               BIT ADDRESS                      PARITIES    |--------- LINE PARITY BITS ----------| |
 *                   7    6    5    4    3    2    1    0                                                             |
 *               0  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002e  L004e  L008e ...  L4096e |
 *               1  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002e  L004e  L008e ...  L4096e |
 *               2  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002o  L004e  L008e ...  L4096e |
 *               3  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002o  L004e  L008e ...  L4096e |
 *               4  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002e  L004o  L008e ...  L4096e |
 *               5  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002e  L004o  L008e ...  L4096e |
 *               6  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002o  L004o  L008e ...  L4096e |
 *               7  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002o  L004o  L008e ...  L4096e |
 *               .                                                                                                    |
 *               .                                                                                                    |
 *               .                                                                                                    |
 *           8184  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002e  L004e  L008o ...  L4096o |
 *           8185  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002e  L004e  L008o ...  L4096o |
 *           8186  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002o  L004e  L008o ...  L4096o |
 *           8187  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002o  L004e  L008o ...  L4096o |
 *           8188  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002e  L004o  L008o ...  L4096o |
 *           8189  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002e  L004o  L008o ...  L4096o |
 *           8190  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001e  L002o  L004o  L008o ...  L4096o |
 *           8191  ---  ---  ---  ---  ---  ---  ---  ---    ==>    0/1       L001o  L002o  L004o  L008o ...  L4096o |
 *                                                                                                                   |
 *                   |    |    |    |    |    |    |    |                                                             |
 *                   V    V    V    V    V    V    V    V                                                             |
 *           T                                                                                                         |
 *                   0/1  0/1  0/1  0/1  0/1  0/1  0/1  0/1                                                            |
 *                                                                                                                   |
 *           G TO                                                                                                      |
 *           ING                                                                                                       |
 *                                                                                                                   |
 *                   C1o  C1e  C1o  C1e  C1o  C1e  C1o  C1e                                                            |
 *                                                                                                                   |
 *                   C2o  C2o  C2e  C2e  C2o  C2o  C2e  C2e                                                            |
 *                                                                                                                   |
 *                   C4o  C4o  C4o  C4o  C4e  C4e  C4e  C4e                                                            |
 *                                                                                                                   |
 *           ----------------------------------------------------------------------------------------------------------+
 *
 * @note     (5) Modifications are made to the algorithm to make it faster.
 *
 *           - (a) The calculation is made on 32 bits data.
 *
 *           - (b) Loop unrolling is used.
 *
 *           - (c) Only odd parity bits are calculated in the loop to improve algorithm speed.
 *                   Even parity bits are calculated from odd parity bits and from the parity of
 *                   the whole data block, using the following binary equation :
 *
 *                       Par of whole data = Par of an even group + Par of corresponding odd group
 *
 *                   Examples of the binary equation :
 *
 *                               par_data_whole =   C1e +   C1o
 *                               par_data_whole =   C2e +   C2o
 *                               par_data_whole = L512e + L512o
 *
 * @note     (6) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
void Hamming_Calc(void       *p_buf,
                  CPU_SIZE_T len,
                  void       *p_buf_ext,
                  CPU_SIZE_T len_ext,
                  CPU_INT08U *p_ecc)
{
  CPU_INT08U *p_line_data_08;
  CPU_INT32U *p_line_data_32;
  CPU_INT32U line_data_32;
  CPU_INT32U L0001o;
  CPU_INT32U L0002o;
  CPU_INT32U L0004o;
  CPU_INT32U L0008o;
  CPU_INT32U L0016o;
  CPU_INT32U L0032o;
  CPU_INT32U L0064o;
  CPU_INT32U L0128o;
  CPU_INT32U L0256o;
  CPU_INT32U L0512o;
  CPU_INT32U L1024o;
  CPU_INT32U L2048o;
  CPU_INT32U L4096o;
  CPU_INT32U C1o;
  CPU_INT32U C2o;
  CPU_INT32U C4o;
  CPU_INT32U hamming;
  CPU_INT32U hamming_even;
  CPU_INT32U hamming_odd;
  CPU_INT32U par_odd_shifted;
  CPU_INT32U par_col;
  CPU_INT32U par_tot;
  CPU_INT32U par_tot_even_mask;
  CPU_INT32U par_big_blk_ix;
  CPU_INT32U par_big_blk;
  CPU_DATA   align_mod_32;
  CPU_DATA   ix;
  CPU_DATA   loop_ext_start;
  CPU_DATA   loop_ext_end;
  CPU_DATA   loop_ext_nbr_iter;
  CPU_DATA   loop_ext_len_rem;
  CPU_DATA   loop_main_nbr_iter;

  RTOS_ASSERT_DBG(p_ecc != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_buf != DEF_NULL, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)                        // ------------------ VALIDATE ARGS -------------------
  {
    CPU_DATA    loop_main_mod;
    CPU_DATA    len_tot;
    CPU_BOOLEAN len_valid;

    if (len_ext > 0) {                                          // Validate ext buf ptr.
      RTOS_ASSERT_DBG(p_buf_ext != DEF_NULL, RTOS_ERR_NULL_PTR,; );
    }

    Mem_Clr((void *)p_ecc, HAMMING_LEN_OCTET_ECC);              // Init ECC buf for err(s) (see Note #6).

    len_tot = len + len_ext;
    len_valid = (len_tot <= HAMMING_LEN_OCTET_BUF_MAX) && (len_tot >= HAMMING_LEN_OCTET_BUF_MIN);
    loop_main_mod = len % HAMMING_LEN_OCTET_PER_LOOP_ITER;

    RTOS_ASSERT_DBG(!len_valid, RTOS_ERR_INVALID_ARG,; );
    RTOS_ASSERT_DBG(loop_main_mod == 0u, RTOS_ERR_INVALID_ARG,; );
  }
#endif

  //                                                               ----------------- INIT VARIABLES -------------------
  loop_main_nbr_iter = len / HAMMING_LEN_OCTET_PER_LOOP_ITER;
  L0004o = 0u;
  L0008o = 0u;
  L0016o = 0u;
  L0032o = 0u;
  L0064o = 0u;
  L0128o = 0u;
  L0256o = 0u;
  L0512o = 0u;
  L1024o = 0u;
  L2048o = 0u;
  L4096o = 0u;
  hamming_odd = 0u;
  par_tot = 0u;
  par_big_blk = 0u;

  align_mod_32 = (CPU_ADDR)p_buf % sizeof(CPU_INT32U);

  if (align_mod_32 == 0u) {                                     // --------- ACCESS MEM ON 4-OCTET BOUNDARIES ---------
    p_line_data_32 = (CPU_INT32U *)p_buf;
    //                                                             -------------------- CALC PARITY -------------------
    //                                                             Calc odd par's (see Note #5c).
    for (ix = 0u; ix < loop_main_nbr_iter; ix++) {
      par_col = *p_line_data_32;

      p_line_data_32++;

      L0004o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      L0008o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      L0004o ^= *p_line_data_32;
      L0008o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      L0016o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      L0004o ^= *p_line_data_32;
      L0016o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      L0008o ^= *p_line_data_32;
      L0016o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      L0004o ^= *p_line_data_32;
      L0008o ^= *p_line_data_32;
      L0016o ^= *p_line_data_32;
      par_col ^= *p_line_data_32;

      p_line_data_32++;

      par_tot ^= par_col;

      par_col = Hamming_ParCalc_32(par_col);
      par_big_blk_ix = ix * par_col;
      par_big_blk ^= par_big_blk_ix;
    }
  } else {                                                      // --------- ACCESS MEM ON 1-OCTET BOUNDARIES ---------
    p_line_data_08 = (CPU_INT08U *)p_buf;

    //                                                             -------------------- CALC PARITY -------------------
    //                                                             Calc odd par's (see Note #5c).
    for (ix = 0u; ix < loop_main_nbr_iter; ix++) {
      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      par_col = line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0004o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0008o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0004o ^= line_data_32;
      L0008o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0016o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0004o ^= line_data_32;
      L0016o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0008o ^= line_data_32;
      L0016o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      L0004o ^= line_data_32;
      L0008o ^= line_data_32;
      L0016o ^= line_data_32;
      par_col ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);

      par_tot ^= par_col;

      par_col = Hamming_ParCalc_32(par_col);
      par_big_blk_ix = ix * par_col;
      par_big_blk ^= par_big_blk_ix;
    }
  }

  //                                                               ---------- CALC PARITY OF EXTENDED BUFFER ----------
  if (p_buf_ext != (void *)0) {
    loop_ext_nbr_iter = len_ext / CPU_WORD_SIZE_32;
    loop_ext_len_rem = len_ext % CPU_WORD_SIZE_32;

    p_line_data_08 = (CPU_INT08U *)p_buf_ext;

    loop_ext_start = (HAMMING_LEN_OCTET_PER_LOOP_ITER / CPU_WORD_SIZE_32) * loop_main_nbr_iter;
    loop_ext_end = loop_ext_start + loop_ext_nbr_iter;

    for (ix = loop_ext_start; ix <= loop_ext_end; ix++) {
      if (ix == loop_ext_end) {
        line_data_32 = 0;                                       // For last iteration, get rem'ing valid octets.
        MEM_VAL_COPY_GET_INTU(&line_data_32, p_line_data_08, loop_ext_len_rem);
      } else {
        MEM_VAL_COPY_GET_INT32U(&line_data_32, p_line_data_08);
      }

      if ((ix & DEF_BIT_00) != 0) {
        L0004o ^= line_data_32;
      }
      if ((ix & DEF_BIT_01) != 0) {
        L0008o ^= line_data_32;
      }
      if ((ix & DEF_BIT_02) != 0) {
        L0016o ^= line_data_32;
      }
      if ((ix & DEF_BIT_03) != 0) {
        L0032o ^= line_data_32;
      }
      if ((ix & DEF_BIT_04) != 0) {
        L0064o ^= line_data_32;
      }
      if ((ix & DEF_BIT_05) != 0) {
        L0128o ^= line_data_32;
      }
      if ((ix & DEF_BIT_06) != 0) {
        L0256o ^= line_data_32;
      }
      if ((ix & DEF_BIT_07) != 0) {
        L0512o ^= line_data_32;
      }
      if ((ix & DEF_BIT_08) != 0) {
        L1024o ^= line_data_32;
      }
      if ((ix & DEF_BIT_09) != 0) {
        L2048o ^= line_data_32;
      }
      if ((ix & DEF_BIT_10) != 0) {
        L4096o ^= line_data_32;
      }
      par_tot ^= line_data_32;

      p_line_data_08 += sizeof(CPU_INT32U);
    }
  }

  //                                                               ------------ CALC ODD LINES PARITY BITS ------------
  L4096o = Hamming_ParCalc_32(L4096o);
  L2048o = Hamming_ParCalc_32(L2048o);
  L1024o = Hamming_ParCalc_32(L1024o);
  L0512o = Hamming_ParCalc_32(L0512o);
  L0256o = Hamming_ParCalc_32(L0256o);
  L0128o = Hamming_ParCalc_32(L0128o);
  L0064o = Hamming_ParCalc_32(L0064o);
  L0032o = Hamming_ParCalc_32(L0032o);
  L0016o = Hamming_ParCalc_32(L0016o);
  L0008o = Hamming_ParCalc_32(L0008o);
  L0004o = Hamming_ParCalc_32(L0004o);
  L0002o = Hamming_ParCalc_32(par_tot & HAMMING_MASK_ODD_LINE_2);
  L0001o = Hamming_ParCalc_32(par_tot & HAMMING_MASK_ODD_LINE_1);

  //                                                               Add accumulated parity during first section.
  L4096o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_07);
  L2048o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_06);
  L1024o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_05);
  L0512o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_04);
  L0256o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_03);
  L0128o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_02);
  L0064o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_01);
  L0032o ^= DEF_BIT_IS_SET(par_big_blk, DEF_BIT_00);

  //                                                               ----------- CALC ODD COLUMNS PARITY BITS -----------
  C4o = Hamming_ParCalc_32(par_tot & HAMMING_MASK_ODD_COL_4);
  C2o = Hamming_ParCalc_32(par_tot & HAMMING_MASK_ODD_COL_2);
  C1o = Hamming_ParCalc_32(par_tot & HAMMING_MASK_ODD_COL_1);

  //                                                               --------- SET ODD PAR BITS IN HAMMING CODE ---------
  //                                                               ----------- SET PAR BITS IN FIRST OCTET ------------
  if (C1o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_01);
  }
  if (C2o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_03);
  }
  if (C4o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_05);
  }
  if (L0001o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_07);
  }

  //                                                               ----------- SET PAR BITS IN SECOND OCTET -----------
  if (L0002o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_09);
  }
  if (L0004o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_11);
  }
  if (L0008o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_13);
  }
  if (L0016o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_15);
  }

  //                                                               ----------- SET PAR BITS IN THIRD OCTET ------------
  if (L0032o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_17);
  }
  if (L0064o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_19);
  }
  if (L0128o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_21);
  }
  if (L0256o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_23);
  }

  //                                                               ----------- SET PAR BITS IN FOURTH OCTET -----------
  if (L0512o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_25);
  }
  if (L1024o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_27);
  }
  if (L2048o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_29);
  }
  if (L4096o) {
    DEF_BIT_SET(hamming_odd, DEF_BIT_31);
  }

  //                                                               ---------------- SET EVEN PAR BITS -----------------
  //                                                               See Note #5c.
  par_odd_shifted = hamming_odd >> 1u;                          // Shift odd bits to loc of corresponding even bits.
  par_tot = Hamming_ParCalc_32(par_tot);                        // Calc tot par.
  par_tot_even_mask = HAMMING_MASK_EVEN_COL_1 * par_tot;        // Set each even bit to tot par in mask.
  hamming_even = par_odd_shifted ^ par_tot_even_mask;           // Invert even bits or not, depending on tot par.
  hamming = hamming_even | hamming_odd;                         // OR even and odd bits to obtain full Hamming code.

  //                                                               ----------------- SET HAMMING CODE -----------------
  MEM_VAL_COPY_SET_INT32U(p_ecc, &hamming);
}

/****************************************************************************************************//**
 *                                               Hamming_Chk()
 *
 * @brief    Check previously computed Hamming code against current data.
 *
 * @param    p_buf               Pointer to buffer that contains the data (see Note #1).
 *
 * @param    -----             Argument validated by Hamming_Calc().
 *
 * @param    len                 Length of buffer, in octets (see Note #2).
 *
 * @param    ---               --               Argument validated by Hamming_Calc().
 *
 * @param    p_buf_ext           Pointer to extension buffer that contains the additional data (see Note #1).
 *
 * @param    ---------         Argument validated by Hamming_Calc().
 *
 * @param    len_ext             Length of extension buffer, in octets (see Note #2b).
 *
 * @param    -------           --           Argument validated by Hamming_Calc().
 *
 * @param    p_ecc               Pointer to 4-octet   buffer that contains the Hamming code.
 *
 * @param    -----             Argument validated by Hamming_Calc().
 *
 * @param    err_loc_tbl         Table that will receive the location of any errors.
 *
 * @param    err_loc_tbl_size    Size of 'err_loc_tbl'; the maximum number of error locations that can
 *                               be returned.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                               ECC_ERR_NONE                Hamming code verified.
 *                               ECC_ERR_CORRECTABLE         Correctable error detected in data.
 *                               ECC_ERR_INVALID_LEN         Argument 'len' passed an invalid length.
 *                               ECC_ERR_NULL_PTR            Argument 'p_buf', 'p_ecc' or
 *                               'err_loc_tbl' passed a NULL pointer.
 *                               ECC_ERR_UNCORRECTABLE       Uncorrectable error detected.
 *
 * @return   0,          if there is no               error  in data,
 *           1,          if there is 1    correctable error  in data,
 *           2,          if there is 2+ uncorrectable errors in data,
 *           ECC_FAULT,  if an error occurred.
 *
 * @note     (1) The calculation on 'p_buf' is optimized for 'CPU_INT32U'-aligned buffers, though
 *               any buffer alignment is acceptable.
 *
 *               The calculation on 'p_buf_ext' is significantly slower, but is allowed for
 *               convenience when the Hamming code must cover data in 2 buffers. If not needed,
 *               a NULL pointer can be passed as the 'p_buf_ext'.
 *
 * @note     (2) The parameter 'len' must be a multiple of HAMMING_LEN_OCTET_PER_LOOP_ITER.
 *
 *               The sum of 'len' & 'len_ext' must be between HAMMING_LEN_OCTET_BUF_MIN and
 *               HAMMING_LEN_OCTET_BUF_MAX.
 *
 *               The number of least-significant bits that are relevant in the error correction code
 *               (*p_ecc) depends on the sum of 'len' & 'len_ext', according to following table.
 *               The application need not to store irrelevant data.
 *
 *                    ---------------------------+---------------------------
 *                   |  Total length of buffers  |  Number of relevant bits  |
 *                   |       (in octets)         |       (lsb) in ECC        |
 *                    ---------------------------+---------------------------
 *                   |                  1        |             6             |
 *                   |                  2        |             8             |
 *                   |          3  -    4        |            10             |
 *                   |          5  -    8        |            12             |
 *                   |          9  -   16        |            14             |
 *                   |         17  -   32        |            16             |
 *                   |         33  -   64        |            18             |
 *                   |         65  -  128        |            20             |
 *                   |        129  -  256        |            22             |
 *                   |        257  -  512        |            24             |
 *                   |        513  - 1024        |            26             |
 *                   |       1023  - 2048        |            28             |
 *                   |       2049  - 4096        |            30             |
 *                   |       4097  - 8192        |            32             |
 *                    ---------------------------+---------------------------
 *
 * @note     (3) Hamming_Chk() checks the location of the error, but does not correct it.
 *               Hamming_Correct() should be called instead of Hamming_Chk() for the error to be
 *               corrected.
 *
 * @note     (4) The maximum number of errors that can be corrected is 1, so 'max_errs' should be 1.
 *
 *               An uncorrectable error is one in which two or more bits of the data have changed
 *               & the error is detectable.
 *
 *               If more than two bits of the data have changed, the error may be misdiagnosed as
 *               a single-bit error.
 *
 * @note     (5) If a single-bit error occurs in the data, either the odd OR even parity bit for
 *               each line & column parity (but NOT both) will be flipped in the check ECC. Since
 *               the number of valid parity bits depends of the sum of 'len' and 'len_ext', the
 *               number of flipped bits expected must be computed each time the function is called.
 *
 *               Since the number of parity bits may vary according to the sum of 'len' and 'len_ext',
 *               the unused bits must be masked to avoid detecting false positive errors.
 *******************************************************************************************************/
CPU_INT08U Hamming_Chk(void        *p_buf,
                       CPU_SIZE_T  len,
                       void        *p_buf_ext,
                       CPU_SIZE_T  len_ext,
                       CPU_INT08U  *p_ecc,
                       ECC_ERR_LOC err_loc_tbl[],
                       CPU_INT08U  err_loc_tbl_size,
                       RTOS_ERR    *p_err)
{
  CPU_INT08U hamming_chk[HAMMING_LEN_OCTET_ECC];
  CPU_INT32U hamming_calcd;
  CPU_INT32U hamming_stored;
  CPU_INT32U hamming_xor;
  CPU_INT32U hamming_pair_calc;
  CPU_INT32U hamming_mask;
  CPU_INT08U diff_cnt;
  CPU_INT08U diff_cnt_expected;
  CPU_INT32U len_tot;
  CPU_INT32U i;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ECC_FAULT);
  RTOS_ASSERT_DBG(err_loc_tbl_size >= HAMMING_NBR_CORRECTABLE_BITS, RTOS_ERR_INVALID_ARG, ECC_FAULT);
  RTOS_ASSERT_DBG(err_loc_tbl != DEF_NULL, RTOS_ERR_INVALID_ARG, ECC_FAULT);

  //                                                               ----------------- CALC HAMMING CODE ----------------
  Hamming_Calc(p_buf,                                           // Calculate Hamming code for current buffer.
               len,
               p_buf_ext,
               len_ext,
               &hamming_chk[0u]);

  MEM_VAL_COPY_GET_INT32U(&hamming_calcd, &hamming_chk[0u]);
  MEM_VAL_COPY_GET_INT32U(&hamming_stored, &p_ecc[0u]);

  hamming_xor = hamming_stored ^ hamming_calcd;
  //                                                               Calc expected nbr of flipped bits in             ...
  //                                                               ... case of a correctable err  (see Note #5a)    ...
  //                                                               ... and mask irrelevant bits   (see Note #5b).
  len_tot = len + len_ext;
  diff_cnt_expected = HAMMING_DIFF_CNT_EXPECTED;
  hamming_mask = 0xFFFFFFFFu;
  //                                                               Determine size of ECC and set hamming_mask and   ...
  //                                                               ... diff_cnt_expected accordingly (see Note #2c).
  for (i = len_tot; i <= (HAMMING_LEN_OCTET_BUF_MAX / 2u); i *= 2u) {
    hamming_mask = hamming_mask >> 2u;
    diff_cnt_expected--;
  }

  hamming_xor &= hamming_mask;
  if (hamming_xor == 0u) {
    return (0u);
  }

  diff_cnt = CRCUtil_PopCnt_32(hamming_xor);
  if (diff_cnt == diff_cnt_expected) {                          // If half of par bits are flipped (see Note #5a)   ...
                                                                // ... make sure exactly 1 bit per pair is set.
    hamming_pair_calc = (hamming_xor >> 1u) & CRC_UTIL_POPCNT_MASK01010101_32;
    hamming_pair_calc += hamming_xor & CRC_UTIL_POPCNT_MASK01010101_32;
    hamming_pair_calc = ~hamming_pair_calc;
    hamming_pair_calc &= hamming_mask;
    hamming_pair_calc &= CRC_UTIL_POPCNT_MASK01010101_32;

    if (hamming_pair_calc == 0u) {                              // If only 1 bit per pair is set ...
      Hamming_CalcErrLoc(hamming_xor,                           // ... calc loc of the flipped data bit (see Note #3).
                         &err_loc_tbl[0]);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ECC_CORR);
      return (1u);
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_ECC_UNCORR);
  return (2u);
}

/****************************************************************************************************//**
 *                                               Hamming_Correct()
 *
 * @brief    Check previously computed Hamming code against current data & correct any error(s), if
 *           possible.
 *
 * @param    p_buf       Pointer to buffer that contains the data (see Note #1).
 *
 * @param    -----      Argument validated by Hamming_Chk().
 *
 * @param    len         Length of buffer, in octets (see Note #2).
 *
 * @param    ---        --       Argument validated by Hamming_Chk().
 *
 * @param    p_buf_ext   Pointer to extension buffer that contains the additional data (see Note #1).
 *
 * @param    ---------  Argument validated by Hamming_Chk().
 *
 * @param    len_ext     Length of extension buffer, in octets (see Note #2b).
 *
 * @param    -------    --   Argument validated by Hamming_Chk().
 *
 * @param    p_ecc       Pointer to 4-octet   buffer that contains the Hamming code.
 *
 * @param    -----      Argument validated by Hamming_Chk().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *
 * @param    -----      Argument validated by Hamming_Chk().
 *
 * @note     (1) The calculation on 'p_buf' is optimized for 'CPU_INT32U'-aligned buffers, though
 *               any buffer alignment is acceptable.
 *
 *               The calculation on 'p_buf_ext' is significantly slower, but is allowed for
 *               convenience when the Hamming code must cover data in 2 buffers. If not needed,
 *               a NULL pointer can be passed as the 'p_buf_ext'.
 *
 * @note     (2) The parameter 'len' must be a multiple of HAMMING_LEN_OCTET_PER_LOOP_ITER.
 *
 *               The sum of 'len' & 'len_ext' must be between HAMMING_LEN_OCTET_BUF_MIN and
 *               HAMMING_LEN_OCTET_BUF_MAX.
 *******************************************************************************************************/
void Hamming_Correct(void       *p_buf,
                     CPU_SIZE_T len,
                     void       *p_buf_ext,
                     CPU_SIZE_T len_ext,
                     CPU_INT08U *p_ecc,
                     RTOS_ERR   *p_err)
{
  CPU_INT08U  *p_buf_08;
  ECC_ERR_LOC err_loc = { 0u, 0u };
  CPU_ADDR    octet_loc;
  CPU_INT08U  err_bit;

  //                                                               ----------------- CHK HAMMING CODE -----------------
  (void)Hamming_Chk(p_buf,                                      // Calc Hamming code for cur buf.
                    len,
                    p_buf_ext,
                    len_ext,
                    p_ecc,
                    &err_loc,
                    1u,
                    p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_ECC_CORR) {
    return;
  }

  //                                                               Correct err.
  if (err_loc.LocOctet < len ) {
    p_buf_08 = (CPU_INT08U *)p_buf;
    octet_loc = err_loc.LocOctet;
  } else if (err_loc.LocOctet < (len + len_ext)) {
    p_buf_08 = (CPU_INT08U *)p_buf_ext;
    octet_loc = err_loc.LocOctet - len;
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ECC_UNCORR);                  // Correction loc out of bounds.
    return;
  }
  err_bit = DEF_BIT(err_loc.LocBit);                            // Flip corrupted/correctable bit.
  p_buf_08[octet_loc] ^= err_bit;
  //                                                               Rtn err from Hamming_Chk().
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Hamming_CalcErrLoc()
 *
 * @brief    Calculate error location.
 *
 * @param    ecc_xor     Value of the xor between stored and computed hamming code.
 *
 * @param    p_err_loc   Pointer to the location of the error.
 *
 * @note     (1) See 'Hamming_Calc() Note #4d and corresponding table'.
 *******************************************************************************************************/
static void Hamming_CalcErrLoc(CPU_INT32U  ecc_xor,
                               ECC_ERR_LOC *p_err_loc)
{
  CPU_INT32U xor_val;
  CPU_SIZE_T loc_octet;
  CPU_INT08U loc_bit;

  xor_val = ecc_xor;

  xor_val = xor_val >> 1u;
  loc_bit = xor_val &  DEF_BIT_00;
  xor_val = xor_val >> 1u;
  loc_bit |= xor_val &  DEF_BIT_01;
  xor_val = xor_val >> 1u;
  loc_bit |= xor_val &  DEF_BIT_02;
  xor_val = xor_val >> 3u;

  xor_val = xor_val >> 1u;
  loc_octet = xor_val &  DEF_BIT_00;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_01;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_02;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_03;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_04;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_05;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_06;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_07;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_08;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_09;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_10;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_11;
  xor_val = xor_val >> 1u;
  loc_octet |= xor_val &  DEF_BIT_12;

  p_err_loc->LocBit = loc_bit;
  p_err_loc->LocOctet = loc_octet;
}

/****************************************************************************************************//**
 *                                           Hamming_ParCalc_32()
 *
 * @brief    Calculate parity for 32-bit data value.
 *
 * @param    data_32     32-bit data value.
 *
 * @return   Parity.
 *******************************************************************************************************/
static CPU_INT32U Hamming_ParCalc_32(CPU_INT32U data_32)
{
  CPU_INT32U par;

  par = data_32;
  par ^= (par >> 16u);
  par ^= (par >>  8u);
  par ^= (par >>  4u);
  par ^= (par >>  2u);
  par ^= (par >>  1u);

  par &= DEF_BIT_00;

  return (par);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
