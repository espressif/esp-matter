/* Copyright Statement:
*
* (C) 2005-2016  MediaTek Inc. All rights reserved.
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
* Without the prior written permission of MediaTek and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
* You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
* if you have agreed to and been bound by the applicable license agreement with
* MediaTek ("License Agreement") and been granted explicit permission to do so within
* the License Agreement ("Permitted User").  If you are not a Permitted User,
* please cease any access or use of MediaTek Software immediately.
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
* ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/

#ifndef __MISC_H__
#define __MISC_H__


/* Sanity check the configuration. */
#define ALIGN_4BYTE(size) (((size + 3) / 4) * 4)

#ifndef IS_ALIGN_4
#define IS_ALIGN_4(_value) (((_value) & 0x3) ? FALSE : TRUE)
#define IS_NOT_ALIGN_4(_value) (((_value) & 0x3) ? TRUE : FALSE)
#endif /* IS_ALIGN_4 */

#ifndef IS_NOT_ALIGN_4
#define IS_NOT_ALIGN_4(_value) (((_value) & 0x3) ? TRUE : FALSE)
#endif /* IS_NOT_ALIGN_4 */

#define BIT_ARRAY_MIN_BYTES(ARRAY, N_BYTES) os_memrlen((ARRAY), (N_BYTES))
#define BIT_ARRAY_MAX_BYTES(BITS) (((BITS) + 7) / 8)
#define BIT_ARRAY_MAX_BITS(ARRAY_BYTES) ((ARRAY_BYTES) * 8)
#define BIT_ARRAY_CLEAR(ARRAY_PTR, ARRAY_BYTES) os_memset((ARRAY_PTR), 0, (ARRAY_BYTES));
#define BIT_ARRAY_GET_BIT(PTR, LEN, BIT) \
	(((BIT) < BIT_ARRAY_MAX_BITS((LEN))) && ((PTR)[(BIT) / 8] & (1 << (7 - ((BIT) & 7)))))
#define BIT_ARRAY_SET_BIT(ARRAY, BIT) ((ARRAY)[(BIT) / 8] |= (1 << (7 - ((BIT) & 7))))
#define BIT_ARRAY_CLEAR_BIT(ARRAY, BIT) ((ARRAY)[(BIT) / 8] &= ~(1 << (7 - ((BIT) & 7))))



extern char *rstrtok(char *s, const char *ct);
extern void convert_string_to_hex_array(char *string, unsigned char *hex);
uint8_t BtoH(char ch);
void AtoH(char *src, char *dest, int destlen);

#endif

