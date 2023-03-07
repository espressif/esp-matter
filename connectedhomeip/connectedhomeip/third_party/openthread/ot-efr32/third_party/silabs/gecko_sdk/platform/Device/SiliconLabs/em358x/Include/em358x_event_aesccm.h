/**************************************************************************//**
* @file
* @brief em358x_event_aesccm Register and Bit Field definitions
* @version 5.8.3
******************************************************************************
* @section License
* <b>(C) Copyright 2014 Silicon Labs, www.silabs.com</b>
*******************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
******************************************************************************/

#ifndef EM358X_EVENT_AESCCM_H
#define EM358X_EVENT_AESCCM_H

/**************************************************************************//**
 * @defgroup EM358X_EVENT_AESCCM
 * @{
 * @brief EM358X_EVENT_AESCCM Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t SRC;
  uint32_t RESERVED0[15];
  __IOM uint32_t MASK;
} EVENT_AESCCM_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM358X_EVENT_AESCCM_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_AESCCM SRC */
#define _EVENT_AESCCM_SRC_RESETVALUE             0x00000000UL
#define _EVENT_AESCCM_SRC_MASK                   0x00000007UL
#define EVENT_AESCCM_SRC_CTWORDVALID             (0x1UL << 2)
#define _EVENT_AESCCM_SRC_CTWORDVALID_SHIFT      2
#define _EVENT_AESCCM_SRC_CTWORDVALID_MASK       0x4UL
#define _EVENT_AESCCM_SRC_CTWORDVALID_DEFAULT    0x00000000UL
#define EVENT_AESCCM_SRC_CTWORDVALID_DEFAULT     (_EVENT_AESCCM_SRC_CTWORDVALID_DEFAULT << 2)
#define EVENT_AESCCM_SRC_PTWORDREQ               (0x1UL << 1)
#define _EVENT_AESCCM_SRC_PTWORDREQ_SHIFT        1
#define _EVENT_AESCCM_SRC_PTWORDREQ_MASK         0x2UL
#define _EVENT_AESCCM_SRC_PTWORDREQ_DEFAULT      0x00000000UL
#define EVENT_AESCCM_SRC_PTWORDREQ_DEFAULT       (_EVENT_AESCCM_SRC_PTWORDREQ_DEFAULT << 1)
#define EVENT_AESCCM_SRC_ENCCOMPLETE             (0x1UL << 0)
#define _EVENT_AESCCM_SRC_ENCCOMPLETE_SHIFT      0
#define _EVENT_AESCCM_SRC_ENCCOMPLETE_MASK       0x1UL
#define _EVENT_AESCCM_SRC_ENCCOMPLETE_DEFAULT    0x00000000UL
#define EVENT_AESCCM_SRC_ENCCOMPLETE_DEFAULT     (_EVENT_AESCCM_SRC_ENCCOMPLETE_DEFAULT << 0)

/* Bit fields for EVENT_AESCCM MASK */
#define _EVENT_AESCCM_MASK_RESETVALUE             0x00000000UL
#define _EVENT_AESCCM_MASK_MASK                   0x00000007UL
#define EVENT_AESCCM_MASK_CTWORDVALID             (0x1UL << 2)
#define _EVENT_AESCCM_MASK_CTWORDVALID_SHIFT      2
#define _EVENT_AESCCM_MASK_CTWORDVALID_MASK       0x4UL
#define _EVENT_AESCCM_MASK_CTWORDVALID_DEFAULT    0x00000000UL
#define EVENT_AESCCM_MASK_CTWORDVALID_DEFAULT     (_EVENT_AESCCM_MASK_CTWORDVALID_DEFAULT << 2)
#define EVENT_AESCCM_MASK_PTWORDREQ               (0x1UL << 1)
#define _EVENT_AESCCM_MASK_PTWORDREQ_SHIFT        1
#define _EVENT_AESCCM_MASK_PTWORDREQ_MASK         0x2UL
#define _EVENT_AESCCM_MASK_PTWORDREQ_DEFAULT      0x00000000UL
#define EVENT_AESCCM_MASK_PTWORDREQ_DEFAULT       (_EVENT_AESCCM_MASK_PTWORDREQ_DEFAULT << 1)
#define EVENT_AESCCM_MASK_ENCCOMPLETE             (0x1UL << 0)
#define _EVENT_AESCCM_MASK_ENCCOMPLETE_SHIFT      0
#define _EVENT_AESCCM_MASK_ENCCOMPLETE_MASK       0x1UL
#define _EVENT_AESCCM_MASK_ENCCOMPLETE_DEFAULT    0x00000000UL
#define EVENT_AESCCM_MASK_ENCCOMPLETE_DEFAULT     (_EVENT_AESCCM_MASK_ENCCOMPLETE_DEFAULT << 0)

/** @} End of group EM358X_EVENT_AESCCM_BitFields */

#endif // EM358X_EVENT_AESCCM_H
