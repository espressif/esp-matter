/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_phy.h"
/* TODO: insert other include files here. */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* TODO: insert definitions here. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* TODO: insert prototypes here. */
 
/*******************************************************************************
 * Variables
 ******************************************************************************/

/* TODO: insert variables here. */

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t PHY_Init(ENET_Type *base, uint32_t phyAddr, uint32_t srcClock_Hz)
{
    /* TODO: insert implementation here. */
    return kStatus_Fail;
}

status_t PHY_Write(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t data)
{
    /* TODO: insert implementation here. */
    return kStatus_Fail;
}

status_t PHY_Read(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t *dataPtr)
{
    /* TODO: insert implementation here. */
    return kStatus_Fail;
}

status_t PHY_EnableLoopback(ENET_Type *base, uint32_t phyAddr, phy_loop_t mode, phy_speed_t speed, bool enable)
{
    /* TODO: insert implementation here. */
    return kStatus_Fail;
}

status_t PHY_GetLinkStatus(ENET_Type *base, uint32_t phyAddr, bool *status)
{
    /* TODO: insert implementation here. */
    return kStatus_Fail;
}

status_t PHY_GetLinkSpeedDuplex(ENET_Type *base, uint32_t phyAddr, phy_speed_t *speed, phy_duplex_t *duplex)
{
    /* TODO: insert implementation here. */
    return kStatus_Fail;
}
