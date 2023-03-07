/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA411e AES
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef BA411E_CONFIG_H
#define BA411E_CONFIG_H



/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_CFG        0
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_KEY        8
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_IV        40
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_IV2       56
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_KEY2      72
/** @brief BA411E offset for Configuration word in DMA Scatter-Gather Tag */
#define AES_OFFSET_MASK      104

/** @brief BA411E Mode Register value for ECB mode of operation */
#define AES_MODEID_ECB        0x00000100
/** @brief BA411E Mode Register value for CBC mode of operation */
#define AES_MODEID_CBC        0x00000200
/** @brief BA411E Mode Register value for CTR mode of operation */
#define AES_MODEID_CTR        0x00000400
/** @brief BA411E Mode Register value for CFB mode of operation */
#define AES_MODEID_CFB        0x00000800
/** @brief BA411E Mode Register value for OFB mode of operation */
#define AES_MODEID_OFB        0x00001000
/** @brief BA411E Mode Register value for CCM mode of operation */
#define AES_MODEID_CCM        0x00002000
/** @brief BA411E Mode Register value for GCM mode of operation */
#define AES_MODEID_GCM        0x00004000
/** @brief BA411E Mode Register value for XTS mode of operation */
#define AES_MODEID_XTS        0x00008000
/** @brief BA411E Mode Register value for CMAC mode of operation */
#define AES_MODEID_CMA        0x00010000
/** @brief BA411E Mode Register value for AES context saving */
#define AES_MODEID_CX_SAVE    0x00000020
/** @brief BA411E Mode Register value for AES context loading */
#define AES_MODEID_CX_LOAD    0x00000010
/** @brief BA411E Mode Register value for AES no context */
#define AES_MODEID_NO_CX      0x00000000
/** @brief BA411E Mode Register value for AES keysize of 128 bits */
#define AES_MODEID_AES128     0x00000000
/** @brief BA411E Mode Register value for AES keysize of 192 bits */
#define AES_MODEID_AES192     0x00000008
/** @brief BA411E Mode Register value for AES keysize of 256 bits */
#define AES_MODEID_AES256     0x00000004
/** @brief BA411E Mode Register value for encryption mode */
#define AES_MODEID_ENCRYPT    0x00000000
/** @brief BA411E Mode Register value for decryption mode */
#define AES_MODEID_DECRYPT    0x00000001
/** @brief BA411E Mode Register value to use Key1 */
#define AES_MODEID_KEY1       0x00000040
/** @brief BA411E Mode Register value to use Key2 */
#define AES_MODEID_KEY2       0x00000080


/** @brief BA411E Mode Register mask for hardware key 1 & 2*/
#define AES_MODEID_KEYX_MASK     0x000000C0




/** @brief BA411E Size for IV in GCM mode */
#define AES_IV_GCM_SIZE       12
/** @brief BA411E Size for IV in all modes except GCM */
#define AES_IV_SIZE           16
/** @brief BA411E Size for Context in GCM and CCM modes */
#define AES_CTX_xCM_SIZE      32
/** @brief BA411E Size for Context in all modes except GCM and CCM */
#define AES_CTX_SIZE          16

#endif
