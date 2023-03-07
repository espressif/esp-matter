/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#ifndef TEST_PAL_MAP_ADDRS_H_
#define TEST_PAL_MAP_ADDRS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VALID_MAPPED_ADDR(addr) ((addr != 0) && (addr != 0xFFFFFFFF))

/* Bit Masks - Used by Linux */
#define BM_READ     0x01 /* Pages may be read */
#define BM_WRITE    0x02 /* Pages may be written */
#define BM_EXEC     0x04 /* Pages may be executed */
#define BM_NONE     0x08 /* Pages may not be accessed */
#define BM_SHARED   0x10 /* Share this mapping */
#define BM_PRIVATE  0x20 /* Create a private copy-on-write mapping */
#define BM_FIXED    0x40 /* Don't interpret addr as a hint:
                place the mapping at exactly that address. */

/******************************************************************************/
/*
 * @brief This function maps IO physical address to OS accessible address.
 * @param[in]
 * physAddr - a physical address.
 * size - size in bytes.
 *
 * @param[out]
 *
 * @return a valid virtual address or null in case of failure.
 */
void *Test_PalIOMap(void *physAddr, size_t size);

/******************************************************************************/
/*
 * @brief This function maps a physical address to a virtual address.
 * @param[in]
 * physAddr - a physical address.
 * startingAddr - preferred static address for mapping.
 * filename - File name.
 * size - size in bytes.
 * protAndFlagsBitMask - prot and flags bit mask.
 *
 * @param[out]
 *
 * @return a valid virtual address or null in case of failure.
 */
void *Test_PalMapAddr(void *physAddr, void *startingAddr, const char *filename,
                size_t size, uint8_t protAndFlagsBitMask);

/******************************************************************************/
/*
 * @brief This function unmaps a virtual address.
 * @param[in] virtual address and size in bytes.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalUnmapAddr(void *virtAddr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_MAP_ADDRS_H_ */
