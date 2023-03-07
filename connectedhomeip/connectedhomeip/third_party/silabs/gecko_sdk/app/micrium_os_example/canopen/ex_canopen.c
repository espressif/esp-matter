/***************************************************************************//**
 * @file
 * @brief CANopen Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_CANOPEN_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_dict.h>
#include  <canopen/include/canopen_obj.h>
#include  <canopen/include/canopen_nmt.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef  EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CANOPEN_NODE_HANDLE Ex_CANopen_Node1Handle;
CANOPEN_NODE_HANDLE Ex_CANopen_Node2Handle;

/********************************************************************************************************
 *                                CANOPEN OBJECT DICTIONARY CONFIGURATION
 *******************************************************************************************************/

// ------------------------------------------------------------------------------------------------
// PARAMETER
// ------------------------------------------------------------------------------------------------

typedef  struct  ex_pg_1 {
  CPU_INT32U Obj_1016_1;                                        // Consumer Heartbeat time
  CPU_INT16U Obj_1017_0;                                        // Producer Heartbeat time
} EX_PG_1;

EX_PG_1 Ex_ParamGrp1;

EX_PG_1 Ex_ParamDef1 = {
  0x1,                                                          // [1016:1] Consumer Heartbeat time
  0x0                                                           // [1017:0] Producer Heartbeat time
};

const CANOPEN_PARAM AppParamGrp1 = {
  .MemBlkSize = sizeof(Ex_ParamGrp1),
  .StartMemBlkPtr = (CPU_INT08U *)&Ex_ParamGrp1,
  .DfltMemBlkPtr = (CPU_INT08U *)&Ex_ParamDef1,
  .ResetType = CANOPEN_RESET_COMM,
  .IdPtr = (void *)"AppParam",
  .Val = CANOPEN_PARAM___E
};

// ------------------------------------------------------------------------------------------------
// VARIABLES
// ------------------------------------------------------------------------------------------------

CPU_INT32U Ex_Var_1000_0 = 0x191;                               // [1000:0] Device Type

CPU_INT32U Ex_Var_1001_0 = 0x1234;                              // [1001:0] Error Register

#if  (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
CPU_INT32U Ex_Var_1003_0 = 0;                                   // [1003:0] Pre-defined Error Field
CPU_INT32U Ex_Var_1003_1 = 0;                                   // [1003:1] Pre-defined Error Field
#endif

CPU_INT32U Ex_Var_1005_0 = 0x80;                                // [1005:0] SYNC

CPU_INT08U Ex_Var_1010_0 = 1;                                   // [1010:0] Store Parameters
CPU_INT32U Ex_Var_1010_1 = 3;

CPU_INT32U Ex_Var_1016_0 = 1u;
CPU_INT32U Ex_Var_1016_1 = 0u;                                  // [1016:0] Consumer Heartbeat time

CPU_INT08U Ex_Var_1018_0 = 4;                                   // [1018:0] Identity Object
CPU_INT32U Ex_Var_1018_1 = 0x01020304;                          // [1018:2] Serial Number
CPU_INT32U Ex_Var_1018_2 = 0x3F;                                // [1018:3] Vendor-ID
CPU_INT32U Ex_Var_1018_3 = 0x0215;                              // [1018:4] Product Code
CPU_INT32U Ex_Var_1018_4 = 0x01070417;                          // [1018:5] Revision Number

CPU_INT08U Ex_Var_1200_0 = 2;                                   // [1200:0] SDO #0
CPU_INT32U Ex_Var_1200_1 = 0x600;                               // [1200:1] Client->Server (rx) COB-ID
CPU_INT32U Ex_Var_1200_2 = 0x580;                               // [1200:2] Server->Client (tx) COB-ID

CPU_INT08U Ex_Var_1201_0 = 3;                                   // [1200:0] SDO #1
CPU_INT32U Ex_Var_1201_1 = 0x1;                                 // [1200:1] Client->Server (rx) COB-ID
CPU_INT32U Ex_Var_1201_2 = 0x1;                                 // [1200:2] Server->Client (tx) COB-ID
CPU_INT32U Ex_Var_1201_3 = 0x1;                                 // [1200:3] Node-ID of the SDO client

CPU_INT08U Ex_Var_1400_0 = 5;                                   // [1400:0] RPDO1
CPU_INT32U Ex_Var_1400_1 = 0x40000202;                          // [1400:1] PDO COB ID
CPU_INT08U Ex_Var_1400_2 = 254;                                 // [1400:2] Transmission type
CPU_INT16U Ex_Var_1400_3 = 1000;                                // [1400:3] Inhibit Time
CPU_INT08U Ex_Var_1400_4 = 0;                                   // [1400:4] Unused
CPU_INT16U Ex_Var_1400_5 = 0;                                   // [1400:5] Event Timer

CPU_INT08U Ex_Var_1600_0 = 1;                                   // [1600:0] RPDO1 mapping parameter
CPU_INT32U Ex_Var_1600_1 = 0x20080020;                          // [1600:1] Application object 1

CPU_INT08U Ex_Var_1800_0 = 5;                                   // [1800:0] TPDO1 Comm Param
CPU_INT32U Ex_Var_1800_1 = 0x40000201;                          // [1800:1] PDO COB ID
CPU_INT08U Ex_Var_1800_2 = 254;                                 // [1800:2] Transmission Type
CPU_INT16U Ex_Var_1800_3 = 0;                                   // [1800:3] Inhibit Time
CPU_INT08U Ex_Var_1800_4 = 0;                                   // [1800:4] Unused
CPU_INT16U Ex_Var_1800_5 = 0;                                   // [1800:5] Event Timer

CPU_INT08U Ex_Var_1A00_0 = 1;                                   // [1A00:0] TPDO1 mapping parameter
CPU_INT32U Ex_Var_1A00_1 = 0x20070020;                          // [1A00:1] Application object 1

CPU_INT32U Ex_Var_2007_0 = 0xFFFFFFFF;                          // [2007:1] PDO_TEST

CPU_INT32U Ex_Var_2008_0 = 0;

CPU_INT08U Ex_DomainArray[30u] = "adfifidmmtu6756453jgurhdbegd64";

CANOPEN_DOMAIN_STR Ex_Var_2009_0 = {
  .DataMemStartPtr = Ex_DomainArray,
  .DataMemSize = 30u,
  .DataMemOffset = 0u
};

CPU_INT32U Ex_Var2_1000_0 = 0x191;                              // [1000:0] Device Type

CPU_INT32U Ex_Var2_1001_0 = 0x1234;                             // [1001:0] Error Register

#if  (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
CPU_INT32U Ex_Var2_1003_0 = 0;                                  // [1003:0] Pre-defined Error Field
CPU_INT32U Ex_Var2_1003_1 = 0;                                  // [1003:1] Pre-defined Error Field
#endif

CPU_INT32U Ex_Var2_1005_0 = 0x80;                               // [1005:0] SYNC

CPU_INT08U Ex_Var2_1010_0 = 1;                                  // [1010:0] Store Parameters
CPU_INT32U Ex_Var2_1010_1 = 3;

CPU_INT32U Ex_Var2_1016_0 = 1u;
CPU_INT32U Ex_Var2_1016_1 = 0u;                                 // [1016:0] Consumer Heartbeat time

CPU_INT08U Ex_Var2_1018_0 = 4;                                  // [1018:0] Identity Object
CPU_INT32U Ex_Var2_1018_1 = 0x01020304;                         // [1018:2] Serial Number
CPU_INT32U Ex_Var2_1018_2 = 0x3F;                               // [1018:3] Vendor-ID
CPU_INT32U Ex_Var2_1018_3 = 0x0215;                             // [1018:4] Product Code
CPU_INT32U Ex_Var2_1018_4 = 0x01070417;                         // [1018:5] Revision Number

CPU_INT08U Ex_Var2_1200_0 = 2;                                  // [1200:0] SDO #0
CPU_INT32U Ex_Var2_1200_1 = 0x600;                              // [1200:1] Client->Server (rx) COB-ID
CPU_INT32U Ex_Var2_1200_2 = 0x580;                              // [1200:2] Server->Client (tx) COB-ID

CPU_INT08U Ex_Var2_1201_0 = 3;                                  // [1200:0] SDO #1
CPU_INT32U Ex_Var2_1201_1 = 0x1;                                // [1200:1] Client->Server (rx) COB-ID
CPU_INT32U Ex_Var2_1201_2 = 0x1;                                // [1200:2] Server->Client (tx) COB-ID
CPU_INT32U Ex_Var2_1201_3 = 0x1;                                // [1200:3] Node-ID of the SDO client

CPU_INT08U Ex_Var2_1400_0 = 5;                                  // [1400:0] RPDO1
CPU_INT32U Ex_Var2_1400_1 = 0x40000201;                         // [1400:1] PDO COB ID
CPU_INT08U Ex_Var2_1400_2 = 254;                                // [1400:2] Transmission Type
CPU_INT16U Ex_Var2_1400_3 = 1000;                               // [1400:3] Inhibit Time
CPU_INT08U Ex_Var2_1400_4 = 0;                                  // [1400:4] Unused
CPU_INT16U Ex_Var2_1400_5 = 0;                                  // [1400:5] Event Timer

CPU_INT08U Ex_Var2_1600_0 = 1;                                  // [1600:0] RPDO1 mapping parameter
CPU_INT32U Ex_Var2_1600_1 = 0x20070020;                         // [1600:1] Application object 1

CPU_INT08U Ex_Var2_1800_0 = 5;                                  // [1800:0] TPDO1 Comm Param
CPU_INT32U Ex_Var2_1800_1 = 0x40000202;                         // [1800:1] PDO COB ID
CPU_INT08U Ex_Var2_1800_2 = 254;                                // [1800:2] Transmission Type
CPU_INT16U Ex_Var2_1800_3 = 0;                                  // [1800:3] Inhibit Time
CPU_INT08U Ex_Var2_1800_4 = 0;                                  // [1800:4] Unused
CPU_INT16U Ex_Var2_1800_5 = 0;                                  // [1800:5] Event Timer

CPU_INT08U Ex_Var2_1A00_0 = 1;                                  // [1A00:0] TPDO1 mapping parameter
CPU_INT32U Ex_Var2_1A00_1 = 0x20070020;                         // [1A00:1] Application object 1

CPU_INT32U Ex_Var2_2007_0 = 0x11111111;                         // [2007:1] PDO_TEST

CPU_INT32U Ex_Var2_2008_0 = 0;

CPU_INT08U Ex_DomainArray2[30u] = "adfifidmmtu6756453jgurhdbegd64";

CANOPEN_DOMAIN_STR Ex_Var2_2009_0 = {
  .DataMemStartPtr = Ex_DomainArray2,
  .DataMemSize = 30u,
  .DataMemOffset = 0u
};

// ------------------------------------------------------------------------------------------------
// CANOPEN OBJECT DICTIONARY
// ------------------------------------------------------------------------------------------------

const CANOPEN_OBJ Ex_ObjDict1[] = {
  // --------------------------------------------------------------------------------------------
  // [1000:xx] - Device Type
  { CANOPEN_KEY(0x1000, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1000_0 },
  // --------------------------------------------------------------------------------------------
  // [1001:xx] - Error Register
  { CANOPEN_KEY(0x1001, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1001_0 },
#if  (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
  // --------------------------------------------------------------------------------------------
  // [1003:xx] - Pre-defined error history
  { CANOPEN_KEY(0x1003, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), CANOPEN_OBJ_TYPE_EMCY, (CPU_INT32U)&Ex_Var_1003_0 },
  { CANOPEN_KEY(0x1003, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1003_1 },
#endif
  // --------------------------------------------------------------------------------------------
  // [1005:xx] - SYNC
  { CANOPEN_KEY(0x1005, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1005_0 },
  // --------------------------------------------------------------------------------------------
  // [1010:xx] - Store Parameters
  { CANOPEN_KEY(0x1010, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1010_0 },
  { CANOPEN_KEY(0x1010, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&AppParamGrp1 },
  // --------------------------------------------------------------------------------------------
  // [1016:xx] - Heartbeat
  { CANOPEN_KEY(0x1016, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), CANOPEN_OBJ_TYPE_HB_CONS, (CPU_INT32U)&Ex_Var_1016_0 },
  { CANOPEN_KEY(0x1016, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_ParamGrp1.Obj_1016_1 },
  // --------------------------------------------------------------------------------------------
  // [1017:xx] - Heartbeat
  { CANOPEN_KEY(0x1017, 0x0, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), CANOPEN_OBJ_TYPE_HB_PROD, (CPU_INT32U)&Ex_ParamGrp1.Obj_1017_0 },
  // --------------------------------------------------------------------------------------------
  // [1018:xx] - Identity Object
  { CANOPEN_KEY(0x1018, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1018_0 },
  { CANOPEN_KEY(0x1018, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1018_1 },
  { CANOPEN_KEY(0x1018, 0x2, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1018_2 },
  { CANOPEN_KEY(0x1018, 0x3, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1018_3 },
  { CANOPEN_KEY(0x1018, 0x4, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var_1018_4 },
  // --------------------------------------------------------------------------------------------
  // [1200:xx] - SDO #0 Com
  { CANOPEN_KEY(0x1200, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ_D__R_), 0, (CPU_INT32U)&Ex_Var_1200_0 },
  { CANOPEN_KEY(0x1200, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ__N_R_), 0, (CPU_INT32U)&Ex_Var_1200_1 },
  { CANOPEN_KEY(0x1200, 0x2, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ__N_R_), 0, (CPU_INT32U)&Ex_Var_1200_2 },
  // --------------------------------------------------------------------------------------------
  // [1201:xx] - SDO #1 Com
  { CANOPEN_KEY(0x1201, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1201_0 },
  { CANOPEN_KEY(0x1201, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1201_1 },
  { CANOPEN_KEY(0x1201, 0x2, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1201_2 },
  { CANOPEN_KEY(0x1201, 0x3, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1201_3 },
  // --------------------------------------------------------------------------------------------
  // [1400:xx] - RPDO1
  { CANOPEN_KEY(0x1400, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1400_0 },
  { CANOPEN_KEY(0x1400, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1400_1 },
  { CANOPEN_KEY(0x1400, 0x2, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1400_2 },
  { CANOPEN_KEY(0x1400, 0x3, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1400_3 },
  { CANOPEN_KEY(0x1400, 0x4, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1400_4 },
  { CANOPEN_KEY(0x1400, 0x5, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1400_5 },
  // --------------------------------------------------------------------------------------------
  // [1600:xx] - RPDO1 mapping parameter
  { CANOPEN_KEY(0x1600, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1600_0 },
  { CANOPEN_KEY(0x1600, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1600_1 },
  // --------------------------------------------------------------------------------------------
  // [1800:xx] - TPDO1
  { CANOPEN_KEY(0x1800, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1800_0 },
  { CANOPEN_KEY(0x1800, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1800_1 },
  { CANOPEN_KEY(0x1800, 0x2, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1800_2 },
  { CANOPEN_KEY(0x1800, 0x3, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1800_3 },
  { CANOPEN_KEY(0x1800, 0x4, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1800_4 },
  { CANOPEN_KEY(0x1800, 0x5, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1800_5 },
  // --------------------------------------------------------------------------------------------
  // [1A00:xx] - TPDO1 mapping parameter
  { CANOPEN_KEY(0x1A00, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1A00_0 },
  { CANOPEN_KEY(0x1A00, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_1A00_1 },
  // --------------------------------------------------------------------------------------------
  // [2007:xx] - write/read test
  { CANOPEN_KEY(0x2007, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ___PRW), CANOPEN_OBJ_TYPE_TPDO_ASYNC, (CPU_INT32U)&Ex_Var_2007_0 },
  { CANOPEN_KEY(0x2008, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_2008_0 },
  { CANOPEN_KEY(0x2009, 0x0, CANOPEN_OBJ_DOMAIN | CANOPEN_OBJ____RW), CANOPEN_OBJ_TYPE_DOMAIN, (CPU_INT32U)&Ex_Var_2009_0 },
  CANOPEN_OBJ_DICT_ENDMARK
};

const CANOPEN_OBJ Ex_ObjDict2[] = {
  // --------------------------------------------------------------------------------------------
  // [1000:xx] - Device Type
  { CANOPEN_KEY(0x1000, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1000_0 },
  // --------------------------------------------------------------------------------------------
  // [1001:xx] - Error Register
  { CANOPEN_KEY(0x1001, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1001_0 },
#if  (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
  // --------------------------------------------------------------------------------------------
  // [1003:xx] - Pre-defined error history
  { CANOPEN_KEY(0x1003, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), CANOPEN_OBJ_TYPE_EMCY, (CPU_INT32U)&Ex_Var2_1003_0 },
  { CANOPEN_KEY(0x1003, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1003_1 },
#endif
  // --------------------------------------------------------------------------------------------
  // [1005:xx] - SYNC
  { CANOPEN_KEY(0x1005, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1005_0 },
  // [1010:xx] - Store Parameters
  { CANOPEN_KEY(0x1010, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1010_0 },
  { CANOPEN_KEY(0x1010, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&AppParamGrp1 },
  // --------------------------------------------------------------------------------------------
  // [1016:xx] - Heartbeat
  { CANOPEN_KEY(0x1016, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), CANOPEN_OBJ_TYPE_HB_CONS, (CPU_INT32U)&Ex_Var_1016_0 },
  { CANOPEN_KEY(0x1016, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_ParamGrp1.Obj_1016_1 },
  // --------------------------------------------------------------------------------------------
  // [1017:xx] - Heartbeat
  { CANOPEN_KEY(0x1017, 0x0, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), CANOPEN_OBJ_TYPE_HB_PROD, (CPU_INT32U)&Ex_ParamGrp1.Obj_1017_0 },
  // --------------------------------------------------------------------------------------------
  // [1018:xx] - Identity Object
  { CANOPEN_KEY(0x1018, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1018_0 },
  { CANOPEN_KEY(0x1018, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1018_1 },
  { CANOPEN_KEY(0x1018, 0x2, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1018_2 },
  { CANOPEN_KEY(0x1018, 0x3, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1018_3 },
  { CANOPEN_KEY(0x1018, 0x4, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____R_), 0, (CPU_INT32U)&Ex_Var2_1018_4 },
  // --------------------------------------------------------------------------------------------
  // [1200:xx] - SDO #0 Com
  { CANOPEN_KEY(0x1200, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ_D__R_), 0, (CPU_INT32U)&Ex_Var2_1200_0 },
  { CANOPEN_KEY(0x1200, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ__N_R_), 0, (CPU_INT32U)&Ex_Var2_1200_1 },
  { CANOPEN_KEY(0x1200, 0x2, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ__N_R_), 0, (CPU_INT32U)&Ex_Var2_1200_2 },
  // --------------------------------------------------------------------------------------------
  // [1201:xx] - SDO #1 Com
  { CANOPEN_KEY(0x1201, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1201_0 },
  { CANOPEN_KEY(0x1201, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1201_1 },
  { CANOPEN_KEY(0x1201, 0x2, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1201_2 },
  { CANOPEN_KEY(0x1201, 0x3, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1201_3 },
  // --------------------------------------------------------------------------------------------
  // [1400:xx] - RPDO1
  { CANOPEN_KEY(0x1400, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1400_0 },
  { CANOPEN_KEY(0x1400, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1400_1 },
  { CANOPEN_KEY(0x1400, 0x2, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1400_2 },
  { CANOPEN_KEY(0x1400, 0x3, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1400_3 },
  { CANOPEN_KEY(0x1400, 0x4, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1400_4 },
  { CANOPEN_KEY(0x1400, 0x5, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1400_5 },
  // --------------------------------------------------------------------------------------------
  // [1600:xx] - RPDO1 mapping parameter
  { CANOPEN_KEY(0x1600, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1600_0 },
  { CANOPEN_KEY(0x1600, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1600_1 },
  // --------------------------------------------------------------------------------------------
  // [1800:xx] - TPDO1
  { CANOPEN_KEY(0x1800, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1800_0 },
  { CANOPEN_KEY(0x1800, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1800_1 },
  { CANOPEN_KEY(0x1800, 0x2, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1800_2 },
  { CANOPEN_KEY(0x1800, 0x3, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1800_3 },
  { CANOPEN_KEY(0x1800, 0x4, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1800_4 },
  { CANOPEN_KEY(0x1800, 0x5, CANOPEN_OBJ_UNSIGNED16 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1800_5 },
  // --------------------------------------------------------------------------------------------
  // [1A00:xx] - TPDO1 mapping parameter
  { CANOPEN_KEY(0x1A00, 0x0, CANOPEN_OBJ_UNSIGNED8 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1A00_0 },
  { CANOPEN_KEY(0x1A00, 0x1, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var2_1A00_1 },
  // --------------------------------------------------------------------------------------------
  // [2007:xx] - write/read test
  { CANOPEN_KEY(0x2007, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ___PRW), CANOPEN_OBJ_TYPE_TPDO_ASYNC, (CPU_INT32U)&Ex_Var2_2007_0 },
  { CANOPEN_KEY(0x2008, 0x0, CANOPEN_OBJ_UNSIGNED32 | CANOPEN_OBJ____RW), 0, (CPU_INT32U)&Ex_Var_2008_0 },
  { CANOPEN_KEY(0x2009, 0x0, CANOPEN_OBJ_DOMAIN | CANOPEN_OBJ____RW), CANOPEN_OBJ_TYPE_DOMAIN, (CPU_INT32U)&Ex_Var_2009_0 },
  CANOPEN_OBJ_DICT_ENDMARK
};

// ------------------------------------------------------------------------------------------------
// MEMORY ALLOCATION
// ------------------------------------------------------------------------------------------------

CANOPEN_TMR_MEM Ex_TmrMem1[CANOPEN_TMR_MAX_QTY];                // Allocation of Timer memory blocks.
CANOPEN_TMR_MEM Ex_TmrMem2[CANOPEN_TMR_MAX_QTY];                // Allocation of Timer memory blocks.
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)                              // Allocation of Emergency Code table.
CANOPEN_EMCY_TBL Ex_EmcyTbl[CANOPEN_EMCY_MAX_ERR_QTY] = {
  { 0, 0x1000 },
  { 1, 0x1001 },
};
#endif

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
//                                                                 Allocation of SDO transfer memory.
CPU_INT08U Ex_SdoBuf1[CANOPEN_SDO_MAX_SERVER_QTY];
CPU_INT08U Ex_SdoBuf2[CANOPEN_SDO_MAX_SERVER_QTY];
#endif

const CANOPEN_NODE_SPEC Ex_CANopen_NodeSpec = {
  .NodeId = 0x1,
  .Baudrate = 125000,
  .DictPtr = (CANOPEN_OBJ *)&Ex_ObjDict1,
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
  .EmcyCodePtr = (CANOPEN_EMCY_TBL *)&Ex_EmcyTbl,
#else
  .EmcyCodePtr = DEF_NULL,
#endif
  .TmrMemPtr = (CANOPEN_TMR_MEM *)&Ex_TmrMem1,
  .TmrQty = sizeof(Ex_TmrMem1) / sizeof(CANOPEN_TMR_MEM),
#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
  .SdoBufPtr = (CPU_INT08U *)&Ex_SdoBuf1
#else
  .SdoBufPtr = DEF_NULL
#endif
};

const CANOPEN_NODE_SPEC Ex_CANopen_Node2Spec = {
  .NodeId = 0x2,
  .Baudrate = 125000,
  .DictPtr = (CANOPEN_OBJ *)&Ex_ObjDict2,
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
  .EmcyCodePtr = (CANOPEN_EMCY_TBL *)&Ex_EmcyTbl,
#else
  .EmcyCodePtr = DEF_NULL,
#endif
  .TmrMemPtr = (CANOPEN_TMR_MEM *)&Ex_TmrMem2,
  .TmrQty = sizeof(Ex_TmrMem2) / sizeof(CANOPEN_TMR_MEM),
#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
  .SdoBufPtr = (CPU_INT08U *)&Ex_SdoBuf2
#else
  .SdoBufPtr = DEF_NULL
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN Ex_Param_OnLoad(CANOPEN_NODE_HANDLE handle,
                                   CANOPEN_PARAM       *p_pg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const CANOPEN_EVENT_FNCTS Ex_EventsCB = {
  .RpdoOnRx = DEF_NULL,
  .TpdoOnTx = DEF_NULL,
  .StateOnChange = DEF_NULL,
  .HbcOnEvent = DEF_NULL,
  .HbcOnChange = DEF_NULL,
  .ParamOnLoad = Ex_Param_OnLoad,
  .ParamOnSave = DEF_NULL,
  .ParamOnDflt = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Ex_CANopen_Init()
 *
 * @brief    Initialize the CANopen module and adds a node to the node list.
 *
 * @note     The Param_OnLoad event callback is set in this function through CANopen_ConfigureEventFncts().
 *           When the node gets the reset signal as part of its initialization process during
 *           CANopen_NodeAdd(), the callback will load the object dictionary entries from NVM, according
 *           to the configuration in object 0x1010.
 *******************************************************************************************************/
void Ex_CANopen_Init(void)
{
  RTOS_ERR err;

  CANopen_Init(&err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), 0u);

  Ex_CANopen_Node1Handle = CANopen_NodeAdd("can0",
                                           &Ex_CANopen_NodeSpec,
                                           &Ex_EventsCB,
                                           &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), 0u);

  Ex_CANopen_Node2Handle = CANopen_NodeAdd("can1",
                                           &Ex_CANopen_Node2Spec,
                                           &Ex_EventsCB,
                                           &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), 0u);
}

/****************************************************************************************************//**
 *                                        Ex_CANopen_NodeStart()
 *
 * @brief    Start a CANopen node. For the purpose of the examples, the nodes put themselves in the
 *           Operational mode. Normally, a CANopen master would instruct the nodes to go from
 *           Pre-operational to Operational instead.
 *******************************************************************************************************/
void Ex_CANopen_NodeStart(void)
{
  RTOS_ERR err;

  CANopen_NodeStart(Ex_CANopen_Node1Handle, &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), 0u);

  CANopen_NodeStart(Ex_CANopen_Node2Handle, &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE), 0u);

  CANopen_NmtStateSet(Ex_CANopen_Node1Handle, CANOPEN_OPERATIONAL, &err);
  CANopen_NmtStateSet(Ex_CANopen_Node2Handle, CANOPEN_OPERATIONAL, &err);
}

/****************************************************************************************************//**
 *                                      Ex_CANopen_DictRdWrReset()
 *
 * @brief    Example of writing and reading a value to/from the CANopen object dictionary.
 *           Also demonstrates how a node reset can retrieve default values from non-volatile memory.
 *******************************************************************************************************/
void Ex_CANopen_DictRdWrReset(void)
{
  CPU_INT16U producer_hb_time;
  RTOS_ERR   err;

  //                                                               Assess the value before writing a new one.
  CANopen_DictWordRd(Ex_CANopen_Node1Handle,
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME, 0x00),
                     &producer_hb_time,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("Producer heartbeat time initially                      : %d\n", producer_hb_time);

  //                                                               Write a new value.
  CANopen_DictWordWr(Ex_CANopen_Node1Handle,
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME, 0x00),
                     0x1234,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("Setting producer heartbeat time to                     : 4660 ms\n");

  //                                                               Confirm by reading the value from the dictionary.
  CANopen_DictWordRd(Ex_CANopen_Node1Handle,
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME, 0x00),
                     &producer_hb_time,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("Producer heartbeat time after update                   : %d\n", producer_hb_time);

  //                                                               Perform node reset.
  CANopen_NmtReset(Ex_CANopen_Node1Handle,
                   CANOPEN_RESET_NODE,
                   &err);

  //                                                               Confirm the value is reset from NVM.
  CANopen_DictWordRd(Ex_CANopen_Node1Handle,
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME, 0x00),
                     &producer_hb_time,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("Producer heartbeat time after user-generated node reset: %d\n\n", producer_hb_time);
}

/****************************************************************************************************//**
 *                                         Ex_CANopen_DictShow()
 *
 * @brief    Display the value of object dictionary object number 2007:0 of each node.
 *******************************************************************************************************/
void Ex_CANopen_DictShow(void)
{
  CPU_INT32U val;
  RTOS_ERR   err;

  CANopen_DictLongRd(Ex_CANopen_Node1Handle,
                     CANOPEN_DEV(0x2007, 0x00),
                     &val,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("can0: %X\n", val);

  CANopen_DictLongRd(Ex_CANopen_Node2Handle,
                     CANOPEN_DEV(0x2007, 0x00),
                     &val,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("can1: %X\n", val);
}

/****************************************************************************************************//**
 *                                         Ex_CANopen_DictWrVal()
 *
 * @brief    Write a value to Object Dictionary object number 2007:0.
 *
 * @param    val     Value to write to object dictionary.
 *
 * @note     This function was intended to be used with the Studio example.
 *******************************************************************************************************/
void Ex_CANopen_DictWrVal(CPU_INT32U val)
{
  RTOS_ERR err;

  CANopen_DictLongWr(Ex_CANopen_Node1Handle,
                     CANOPEN_DEV(0x2007, 0x00),
                     val,
                     &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
}

/****************************************************************************************************//**
 *                                         Ex_CANopen_DictRd()
 *
 * @brief    Read object dictionary value of object number 2007:0 on specified node.
 *
 * @param    node    Node from which to read the object dictionary.
 *                       - 1: can bus 0
 *                       - 2: can bus 1
 *
 * @return   Value read from object dictionary object 2007:0 for the node specified.
 *
 * @note     This function was intended to be used with the Studio example.
 *******************************************************************************************************/
CPU_INT32U Ex_CANopen_DictRd(CPU_INT08U node)
{
  CPU_INT32U val;
  RTOS_ERR   err;

  if (node == 1) {
    CANopen_DictLongRd(Ex_CANopen_Node1Handle,
                       CANOPEN_DEV(0x2007, 0x00),
                       &val,
                       &err);
  } else {
    CANopen_DictLongRd(Ex_CANopen_Node2Handle,
                       CANOPEN_DEV(0x2007, 0x00),
                       &val,
                       &err);
  }
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);

  return (val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Ex_Param_OnLoad()
 *
 * @brief    Callback function to copy parameters from non-volatile memory to the object dictionary.
 *
 * @param    p_pg    Pointer to the parameter group structure.
 *
 * @return   true
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_Param_OnLoad(CANOPEN_NODE_HANDLE handle,
                                   CANOPEN_PARAM       *p_pg)
{
  CPU_INT08U *p_param;                                          // Local: ptr within parameter memory block.
  CPU_INT08U *p_src;                                            // Local: ptr within default memory block.
  CPU_INT32U ix;                                                // Local: loop counter.

  PP_UNUSED_PARAM(handle);

  p_param = p_pg->StartMemBlkPtr;                               // set ptr to parameter memory block start.
  p_src = p_pg->DfltMemBlkPtr;                                  // set src to default memory block start.

  for (ix = 0; ix < p_pg->MemBlkSize; ix++) {                   // loop through whole memory block.
    *p_param = *p_src;                                          // set data in parameter memory block.
    p_param++;                                                  // switch to next parameter memory cell.
    p_src++;                                                    // switch to next default memory cell.
  }
  return (DEF_TRUE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
