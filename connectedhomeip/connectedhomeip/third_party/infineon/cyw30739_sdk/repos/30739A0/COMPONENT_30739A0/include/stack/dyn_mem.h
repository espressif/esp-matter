/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/****************************************************************************
**
**  Name:       dyn_mem.h
**
**  Function    this file contains definitions used to determine if a component
**              uses static or dynamic memory for its control blocks.
**
*****************************************************************************/
#ifndef DYN_MEM_H
#define DYN_MEM_H

/****************************************************************************
** Define memory usage for GKI (if not defined in buildcfg.h)
**  The default for GKI is to use static memory allocation for its control
**  block.
*/
#ifndef GKI_DYNAMIC_MEMORY
#define GKI_DYNAMIC_MEMORY  FALSE
#endif

/****************************************************************************
** Define memory usage for each CORE component (if not defined in buildcfg.h)
**  The default for each component is to use static memory allocations.
*/
#ifndef BTU_DYNAMIC_MEMORY
#define BTU_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BTM_DYNAMIC_MEMORY
#define BTM_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SDP_DYNAMIC_MEMORY
#define SDP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef L2C_DYNAMIC_MEMORY
#define L2C_DYNAMIC_MEMORY  FALSE
#endif

#ifndef RFC_DYNAMIC_MEMORY
#define RFC_DYNAMIC_MEMORY  FALSE
#endif

#ifndef OBX_DYNAMIC_MEMORY
#define OBX_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BNEP_DYNAMIC_MEMORY
#define BNEP_DYNAMIC_MEMORY FALSE
#endif

#ifndef AVDT_DYNAMIC_MEMORY
#define AVDT_DYNAMIC_MEMORY FALSE
#endif

#ifndef AVCT_DYNAMIC_MEMORY
#define AVCT_DYNAMIC_MEMORY FALSE
#endif

#ifndef MCA_DYNAMIC_MEMORY
#define MCA_DYNAMIC_MEMORY FALSE
#endif

#ifndef GATT_DYNAMIC_MEMORY
#define GATT_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SMP_DYNAMIC_MEMORY
#define SMP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BRCM_DYNAMIC_MEMORY
#define BRCM_DYNAMIC_MEMORY  FALSE
#endif

/****************************************************************************
** Define memory usage for each PROFILE component (if not defined in buildcfg.h)
**  The default for each component is to use static memory allocations.
*/
#ifndef A2D_DYNAMIC_MEMORY
#define A2D_DYNAMIC_MEMORY  FALSE
#endif

#ifndef VDP_DYNAMIC_MEMORY
#define VDP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef AVRC_DYNAMIC_MEMORY
#define AVRC_DYNAMIC_MEMORY FALSE
#endif

#ifndef BIP_DYNAMIC_MEMORY
#define BIP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef BPP_DYNAMIC_MEMORY
#define BPP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef DUN_DYNAMIC_MEMORY
#define DUN_DYNAMIC_MEMORY  FALSE
#endif

#ifndef FTP_DYNAMIC_MEMORY
#define FTP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef GAP_DYNAMIC_MEMORY
#define GAP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef GOEP_DYNAMIC_MEMORY
#define GOEP_DYNAMIC_MEMORY FALSE
#endif

#ifndef HFP_DYNAMIC_MEMORY
#define HFP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef HID_DYNAMIC_MEMORY
#define HID_DYNAMIC_MEMORY  FALSE
#endif

#ifndef HSP2_DYNAMIC_MEMORY
#define HSP2_DYNAMIC_MEMORY FALSE
#endif

#ifndef OPP_DYNAMIC_MEMORY
#define OPP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef PAN_DYNAMIC_MEMORY
#define PAN_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SPP_DYNAMIC_MEMORY
#define SPP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef SLIP_DYNAMIC_MEMORY
#define SLIP_DYNAMIC_MEMORY  FALSE
#endif

#ifndef LLCP_DYNAMIC_MEMORY
#define LLCP_DYNAMIC_MEMORY  FALSE
#endif

/****************************************************************************
** Define memory usage for BTA (if not defined in buildcfg.h)
**  The default for each component is to use static memory allocations.
*/
#ifndef BTA_DYNAMIC_MEMORY
#define BTA_DYNAMIC_MEMORY FALSE
#endif

/****************************************************************************
** Define memory usage for BT Trace (if not defined in buildcfg.h)
**  The default is to use static memory allocations.
*/
#ifndef BTTRC_DYNAMIC_MEMORY
#define BTTRC_DYNAMIC_MEMORY FALSE
#endif

#endif  /* #ifdef DYN_MEM_H */
