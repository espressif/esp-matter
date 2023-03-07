/********************************************************************************************************
 * @file	ll_resolvlist.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
 *
 * @par		Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *			All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#ifndef LL_RESOLVLIST_H_
#define LL_RESOLVLIST_H_


#include <stack/ble/ble_common.h>



/******************************* Macro & Enumeration & Structure Definition for Stack Begin, user can not use!!!!  *****/

#define				MAX_RESOLVING_LIST_SIZE				2
#define 			PRIVACY_NETWORK_MODE             	(0)
#define 			PRIVACY_DEVICE_MODE              	(1)

#define				MAX_WHITE_LIST_SIZE					4
#define 			MAX_WHITE_IRK_LIST_SIZE          	2


typedef struct {
    u8 rlPrivMode;	 			//Privacy network(0) or privacy device(1)
    u8 rlIdAddrType; 			//Public address (0) or random address (1)
    u8 rlIdAddr[BLE_ADDR_LEN]; 	//The Identity Address is stored in little endian format

    u8 rlHasLocalRpa;
    u8 rsvd0;
    u8 rlLocalRpa[BLE_ADDR_LEN];//The local rpa is stored in little endian format

    u8 rlHasPeerRpa;
    u8 rsvd1;
    u8 rlPeerRpa[BLE_ADDR_LEN]; //The local rpa is stored in little endian format.

    u8 rlPeerIrk[16]; 			//The IRKs are stored in big endian format
    u8 rlLocalIrk[16];          //The IRKs are stored in big endian format
}ll_resolv_list_t;

typedef struct {
	u8  rpaTmrEn;   	 //1: rpa tmr start; 0: rpa tmr stop
	u8  rsvd[3];         //used align

	u16 rpaTimeoutXs;    //belong to [0x0001, 0x0E10], unit: 1s
	u16 rpaTmrCnt1sUnit; //design a 1s timer, accumulator unit 1s, 2^32/16us => 268.435456s

	u32 rpaTmr1sChkTick; //accumulate judgment ticks per second
}ll_rpaTmrCtrl_t;

#if (LL_FEATURE_ENABLE_LL_PRIVACY)

typedef struct {
    u8 	rlSize;
    u8 	rlCnt;
    u16 addrRlEn;	//addr res enabled

    u16 rpaTmrEn;   //1: rpa tmr start; 0: rpa tmr stop
    u16 rpaTmo;		//0x0001 to 0x0E10, unit: 1s

    ll_rpaTmrCtrl_t  rpaTmoCtrl;

    ll_resolv_list_t rlList[MAX_RESOLVING_LIST_SIZE];

}ll_ResolvingListTbl_t;

#else

typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
	u8 reserved;
	u8 irk[16];
} rl_addr_t;

typedef struct {
	rl_addr_t	tbl[MAX_WHITE_IRK_LIST_SIZE];
	u8 			idx;
	u8			en;
} ll_ResolvingListTbl_t;

#endif

extern	ll_ResolvingListTbl_t	ll_resolvingList_tbl;

/******************************* Macro & Enumeration & Structure Definition for Stack End ******************************/






/******************************* Macro & Enumeration variables for User Begin ******************************************/


/******************************* Macro & Enumeration variables for User End ********************************************/







/******************************* User Interface  Begin *****************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

ble_sts_t  		ll_resolvingList_add(u8 peerIdAddrType, u8 *peerIdAddr, u8 *peer_irk, u8 *local_irk);
ble_sts_t  		ll_resolvingList_delete(u8 peerIdAddrType, u8 *peerIdAddr);
ble_sts_t  		ll_resolvingList_reset(void);
ble_sts_t  		ll_resolvingList_getSize(u8 *Size);
ble_sts_t  		ll_resolvingList_getPeerResolvableAddr (u8 peerIdAddrType, u8* peerIdAddr, u8* peerResolvableAddr); //not available now
ble_sts_t  		ll_resolvingList_getLocalResolvableAddr(u8 peerIdAddrType, u8* peerIdAddr, u8* LocalResolvableAddr); //not available now
ble_sts_t  		ll_resolvingList_setAddrResolutionEnable (u8 resolutionEn);
ble_sts_t  		ll_resolvingList_setResolvablePrivateAddrTimer (u16 timeout_s);   //not available now
ble_sts_t  		ll_resolvingList_setPrivcyMode(u8 peerIdAddrType, u8* peerIdAddr, u8 privMode);

u16				blc_ll_resolvGetRpaTmo(void);
int				blc_ll_resolvPeerRpaResolvedAny(const u8* rpa);
void			blc_ll_resolvGetRpaByRlEntry(ll_resolv_list_t* rl, u8* addr, u8 local);
void			blc_ll_resolvSetPeerRpaByIdx(u8 idx, u8 *rpa);
void			blc_ll_resolvSetLocalRpaByIdx(u8 idx, u8 *rpa);
bool			blc_ll_resolvGetRpaByAddr(u8* peerIdAddr, u8 peerIdAddrType, u8* rpa, u8 local);
bool			blc_ll_resolvIsAddrResolved(const u8* irk, const u8* rpa);
bool			blc_ll_resolvIsAddrRlEnabled(void);
void			blc_ll_resolvListReset(void);
void			blc_ll_resolvListInit(void);

#ifdef __cplusplus
}
#endif

/******************************* User Interface  End  ******************************************************************/





#endif /* LL_RESOLVLIST_H_ */
