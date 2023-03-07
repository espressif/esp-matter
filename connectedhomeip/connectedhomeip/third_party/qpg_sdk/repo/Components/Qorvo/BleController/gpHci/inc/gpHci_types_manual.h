/*
 *   Copyright (c) 2015-2016, GreenPeak Technologies
 *   Copyright (c) 2017, Qorvo Inc
 *
 *   Bluetooth Low Energy interface
 *   Declarations of the public functions and enumerations of gpHci_types.
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */

#ifndef _GPHCI_TYPES_MANUAL_H_
#define _GPHCI_TYPES_MANUAL_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHci_Includes.h"

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define HCI_PHYMASK_INIT(maskVal)            ((gpHci_PhyMask_t){.mask = maskVal})
#define HCI_PHYMASK_IS_ZERO(phyMask)         (phyMask.mask == 0)
#define HCI_PHYMASK_IS_EQUAL(mask0, mask1)   ((mask0).mask == (mask1).mask)
#define HCI_PHY_IN_PHYMASK(phy, phyMask)     ((phy.mask & phyMask.mask) == phy.mask)

#define HCI_PHY_TO_PHYMASK(phy)         ((gpHci_PhyMask_t){.mask=(gpHci_Phy_None==phy ? 0 :(1 << (phy - 1)))})

#endif //_GPHCI_TYPES_MANUAL_H_
