/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Bluetooth Low Energy
 *   Declarations of the public functions and enumerations of gpBle.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPBLE_EXTADV_H_
#define _GPBLE_EXTADV_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHci_Includes.h"
#include "gpBle.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Adv Extensions properies bits
#define BLE_EA_PROP_CONNADV                         BM(0)   // connectable
#define BLE_EA_PROP_SCANADV                         BM(1)   // scanable
#define BLE_EA_PROP_DIRECTADV                       BM(2)   // directed
#define BLE_EA_PROP_HIGHDUTYADV                     BM(3)
#define BLE_EA_PROP_USELEGACY                       BM(4)
#define BLE_EA_PROP_OMITADVA                        BM(5)
#define BLE_EA_PROP_INCLUDETXPOWER                  BM(6)

// Extended Advertising header defines
#define BLE_EXTADV_HEADER_LENGTH_IDX                0
#define BLE_EXTADV_HEADER_ADVAMODE_IDX              6
#define BLE_EXTADV_HEADER_LENGTH_BM                 0x3F
#define BLE_EXTADV_HEADER_ADVAMODE_BM               0xC0


#define BLE_EXTADV_HEADER_LENGTH_SET(header, length)       BLE_BM_SET(header, BLE_EXTADV_HEADER_LENGTH_BM, BLE_EXTADV_HEADER_LENGTH_IDX,length)
#define BLE_EXTADV_HEADER_LENGTH_GET(header)               BLE_BM_GET(header, BLE_EXTADV_HEADER_LENGTH_BM, BLE_EXTADV_HEADER_LENGTH_IDX)
#define BLE_EXTADV_HEADER_ADVAMODE_SET(header, advMode)    BLE_BM_SET(header, BLE_EXTADV_HEADER_ADVAMODE_BM, BLE_EXTADV_HEADER_ADVAMODE_IDX,advMode)
#define BLE_EXTADV_HEADER_ADVAMODE_GET(header)             BLE_BM_GET(header, BLE_EXTADV_HEADER_ADVAMODE_BM, BLE_EXTADV_HEADER_ADVAMODE_IDX)

// AUX_PTR byte 0 Content manip
#define BLE_AUXPTR_CHANNELINDEX_IDX                0
#define BLE_AUXPTR_CA_IDX                          6
#define BLE_AUXPTR_OFFSETUNITS_IDX                 7
#define BLE_AUXPTR_CHANNELINDEX_BM                 0x3F
#define BLE_AUXPTR_CA_BM                           0x40
#define BLE_AUXPTR_OFFSETUNITS_BM                  0x80
#define BLE_AUXPTR_CHANNELINDEX_SET(auxptr, channelindex)        BLE_BM_SET(auxptr, BLE_AUXPTR_CHANNELINDEX_BM, BLE_AUXPTR_CHANNELINDEX_IDX,channelindex)
#define BLE_AUXPTR_CHANNELINDEX_GET(auxptr)                      BLE_BM_GET(auxptr, BLE_AUXPTR_CHANNELINDEX_BM, BLE_AUXPTR_CHANNELINDEX_IDX)
#define BLE_AUXPTR_CA_SET(auxptr, ca)                            BLE_BM_SET(auxptr, BLE_AUXPTR_CA_BM, BLE_AUXPTR_CA_IDX, ca)
#define BLE_AUXPTR_OFFSETUNITS_SET(auxptr, offsetunits)          BLE_BM_SET(auxptr, BLE_AUXPTR_OFFSETUNITS_BM, BLE_AUXPTR_OFFSETUNITS_IDX,offsetunits)

#define BLE_AUXPTR_AUXOFFSET_IDX                0
#define BLE_AUXPTR_AUXOFFSET_BM                 0x1FFF
#define BLE_AUXPTR_AUXPHY_IDX                   13
#define BLE_AUXPTR_AUXPHY_BM                    0xE000
#define BLE_AUXPTR_AUXOFFSET_SET(auxptr, offset)          BLE_BM_SET(auxptr, BLE_AUXPTR_AUXOFFSET_BM, BLE_AUXPTR_AUXOFFSET_IDX,offset)
#define BLE_AUXPTR_AUXOFFSET_GET(auxptr)                  BLE_BM_GET(auxptr, BLE_AUXPTR_AUXOFFSET_BM, BLE_AUXPTR_AUXOFFSET_IDX)
#define BLE_AUXPTR_AUXPHY_SET(auxptr, phy)                BLE_BM_SET(auxptr, BLE_AUXPTR_AUXPHY_BM, BLE_AUXPTR_AUXPHY_IDX,(phy-1))
#define BLE_AUXPTR_AUXPHY_GET(auxptr)                     BLE_BM_GET(auxptr, BLE_AUXPTR_AUXPHY_BM, BLE_AUXPTR_AUXPHY_IDX)

// #ifdef GP_DIVERSITY_PERIODIC_ADVERTISING
#define BLE_SYNCINFO_RFU_IDX                       14
#define BLE_SYNCINFO_INTERVAL_IDX                  0
#define BLE_SYNCINFO_INTERVAL_BM                   0xFFFF
#define BLE_SYNCINFO_SCA_IDX                       5
#define BLE_SYNCINFO_SCA_BM                        0xE0

#define BLE_SYNCINFO_INTERVAL_SET(syncinfo, interval)           BLE_BM_SET(syncinfo, BLE_SYNCINFO_INTERVAL_BM, BLE_SYNCINFO_INTERVAL_IDX, interval)
#define BLE_SYNCINFO_SCA_SET(syncinfo, sca)                     BLE_BM_SET(syncinfo, BLE_SYNCINFO_SCA_BM, BLE_SYNCINFO_SCA_IDX, sca)

// #endif //GP_DIVERSITY_PERIODIC_ADVERTISING)

#define BLE_ADI_DID_IDX     0x00
#define BLE_ADI_DID_BM      0x0FFF
#define BLE_ADI_SID_IDX     0x0C
#define BLE_ADI_SID_BM      0xF000
#define BLE_ADI_SID_MAX_VAL 0x0F

#define BLE_ADI_DID_SET(adi, offset)               BLE_BM_SET(adi, BLE_ADI_DID_BM, BLE_ADI_DID_IDX, offset)
#define BLE_ADI_SID_SET(adi, offset)               BLE_BM_SET(adi, BLE_ADI_SID_BM, BLE_ADI_SID_IDX, offset)

// Extended header flag bits (table 2.12)
#define BLE_EXTHDR_FLAGS_ADVA                       (0)
#define BLE_EXTHDR_FLAGS_TARGETA                    (1)
#define BLE_EXTHDR_FLAGS_RESERVED0                  (2)
#define BLE_EXTHDR_FLAGS_ADI                        (3)
#define BLE_EXTHDR_FLAGS_AUXPTR                     (4)
#define BLE_EXTHDR_FLAGS_SYNCINFO                   (5)
#define BLE_EXTHDR_FLAGS_TXPOWER                    (6)
#define BLE_EXTHDR_FLAGS_RESERVED1                  (7)
#define BLE_EXTHDR_FLAGS_ADVA_SET(flags)            BIT_SET(flags,(BLE_EXTHDR_FLAGS_ADVA))
#define BLE_EXTHDR_FLAGS_ADVA_TST(flags)            BIT_TST(flags,(BLE_EXTHDR_FLAGS_ADVA))
#define BLE_EXTHDR_FLAGS_TARGETA_SET(flags)         BIT_SET(flags,(BLE_EXTHDR_FLAGS_TARGETA))
#define BLE_EXTHDR_FLAGS_TARGETA_TST(flags)         BIT_TST(flags,(BLE_EXTHDR_FLAGS_TARGETA))
#define BLE_EXTHDR_FLAGS_RESERVED0_SET(flags)       BIT_SET(flags,(BLE_EXTHDR_FLAGS_RESERVED0))
#define BLE_EXTHDR_FLAGS_RESERVED0_TST(flags)       BIT_TST(flags,(BLE_EXTHDR_FLAGS_RESERVED0))
#define BLE_EXTHDR_FLAGS_ADI_SET(flags)             BIT_SET(flags,(BLE_EXTHDR_FLAGS_ADI))
#define BLE_EXTHDR_FLAGS_ADI_TST(flags)             BIT_TST(flags,(BLE_EXTHDR_FLAGS_ADI))
#define BLE_EXTHDR_FLAGS_AUXPTR_SET(flags)          BIT_SET(flags,(BLE_EXTHDR_FLAGS_AUXPTR))
#define BLE_EXTHDR_FLAGS_AUXPTR_TST(flags)          BIT_TST(flags,(BLE_EXTHDR_FLAGS_AUXPTR))
#define BLE_EXTHDR_FLAGS_SYNCINFO_SET(flags)        BIT_SET(flags,(BLE_EXTHDR_FLAGS_SYNCINFO))
#define BLE_EXTHDR_FLAGS_SYNCINFO_TST(flags)        BIT_TST(flags,(BLE_EXTHDR_FLAGS_SYNCINFO))
#define BLE_EXTHDR_FLAGS_TXPOWER_SET(flags)         BIT_SET(flags,(BLE_EXTHDR_FLAGS_TXPOWER))
#define BLE_EXTHDR_FLAGS_TXPOWER_TST(flags)         BIT_TST(flags,(BLE_EXTHDR_FLAGS_TXPOWER))

// Extended header field sizes
#define BLE_EXTHDR_HEADER_SIZE                      (1)
#define BLE_EXTHDR_FLAGS_SIZE                       (1)
#define BLE_EXTHDR_ADVA_SIZE                        (sizeof(BtDeviceAddress_t))
#define BLE_EXTHDR_TARGETA_SIZE                     (sizeof(BtDeviceAddress_t))
#define BLE_EXTHDR_CTEINFO_SIZE                     (1)
#define BLE_EXTHDR_ADI_SIZE                         (2)
#define BLE_EXTHDR_AUXPTR_SIZE                      (3)
#define BLE_EXTHDR_SYNCINFO_SIZE                    (18)
#define BLE_EXTHDR_TXPOWER_SIZE                     (1)

#define GPBLE_EXTADV_T_MAFS_US                      (300)

#define GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_IDX     0
#define GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_IDX       13
#define GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_IDX     14
#define GPBLE_SYNCINFO_OFFSET_NR_OF_BITS            (GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_IDX)

#define GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_BM      0x1FFF
#define GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_BM        0x2000
#define GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_BM      0x4000

#define GPBLE_OFFSET_UNIT30                 (30)
#define GPBLE_OFFSET_UNIT300                (300)
// 245700 us
#define GPBLE_OFFSET_TURNING_POINT_US       ((BM(GPBLE_SYNCINFO_OFFSET_NR_OF_BITS) - 2) * GPBLE_OFFSET_UNIT30)
// 2.457600 seconds
#define GPBLE_OFFSET_ADJUST_US              ((BM(GPBLE_SYNCINFO_OFFSET_NR_OF_BITS)) * GPBLE_OFFSET_UNIT300)

#define GPBLE_SYNC_HANDLE_INVALID                   0xFFFF
#define GPBLE_EXTENDED_ADVERTISING_HANDLE_INVALID   0xFF

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_SET(offset, val)     BLE_BM_SET(offset, GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_BM, GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_IDX, val)
#define GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_GET(offset)          BLE_BM_GET(offset, GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_BM, GPBLE_SYNCINFO_OFFSET_PACKET_OFFSET_IDX)
#define GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_SET(offset, val)       BLE_BM_SET(offset, GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_BM,   GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_IDX, val)
#define GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_GET(offset)            BLE_BM_GET(offset, GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_BM,   GPBLE_SYNCINFO_OFFSET_OFFSET_UNIT_IDX)
#define GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_SET(offset, val)     BLE_BM_SET(offset, GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_BM, GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_IDX, val)
#define GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_GET(offset)          BLE_BM_GET(offset, GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_BM, GPBLE_SYNCINFO_OFFSET_OFFSET_ADJUST_IDX)

#define GPBLE_OFFSET_GET_UNIT_BIT(offsetUs)                      (offsetUs < GPBLE_OFFSET_TURNING_POINT_US ? 0:1)
#define GPBLE_OFFSET_GET_VALUE_IN_UNITS(offsetUs)                (offsetUs / GPBLE_OFFSET_GET_UNIT_FROM_BIT(GPBLE_OFFSET_GET_UNIT_BIT(offsetUs)))
#define GPBLE_OFFSET_GET_UNIT_FROM_BIT(offsetUnit)               (offsetUnit ? GPBLE_OFFSET_UNIT300 : GPBLE_OFFSET_UNIT30)
#define GPBLE_OFFSET_GET_ADJUST_US_FROM_BIT(adjust)              (adjust ? GPBLE_OFFSET_ADJUST_US : 0)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    UInt16  syncPacketOffset;
    Bool    offsetUnits;
    Bool    offsetAdjust;
} gpBle_SyncOffsetInfo_t;

typedef struct
{
    gpBle_SyncOffsetInfo_t offset;
    UInt16 interval;
    UInt8  channelMap[5];
    UInt8  sca;
    UInt32  accessAddress;
    UInt32  crcInit;
    UInt16 eventCounter;
} gpBle_SyncInfo_t;

typedef struct {
    UInt16 id;
    gpBle_SyncInfo_t syncInfo;
    UInt16 connEventCount;
    UInt16 lastPaEventCounter;
    UInt8 sid;
    /*  This field has double use:
     *   - provides the content of the LL_PERIODIC_SYNC_IND (aType is in [0, 1])
     *   - provides the content of the HCI_LE_Periodic_Advertising_Sync_Transfer_Received Event (aType is in [0, 3])
     */
    gpHci_AdvertiserAddressType_t aType;
    gpHci_ClockAccuracy_t sca;
    gpHci_PhyMask_t phyMask;
    BtDeviceAddress_t advA;
    UInt16 syncConnEventCounter;
    UInt16 connEventCounterRx;
    UInt32 perAdvEventTs;
} gpBle_PastInfoData_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

void gpBle_SetSyncInfoToBuffer(UInt8* pBuffer, gpBle_SyncInfo_t* pSyncInfo);
void gpBle_GetSyncInfoFromBuffer(UInt8 *pBuffer, gpBle_SyncInfo_t *syncInfoField);
void gpBle_GetSyncInfoFromOffsetUs(UInt32 offsetUs, gpBle_SyncOffsetInfo_t* pInfo);
UInt32 gpBle_GetSyncOffsetUsFromInfo(gpBle_SyncOffsetInfo_t* pInfo);


#endif //_GPBLE_EXTADV_H_

