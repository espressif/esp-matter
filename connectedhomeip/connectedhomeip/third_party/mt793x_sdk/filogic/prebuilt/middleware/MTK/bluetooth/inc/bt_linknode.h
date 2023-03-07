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
/* MediaTek restricted information */

#ifndef __BT_LINKNODE_H__
#define __BT_LINKNODE_H__

#include "bt_platform_internal.h"
#include "bt_type.h"


BT_EXTERN_C_BEGIN

#define BT_NODE_FRONT   0x0
#define BT_NODE_BACK    0x1
typedef uint32_t bt_linknode_position;

BT_PACKED(
typedef struct _bt_linknode_t{
    struct _bt_linknode_t *front;
}) bt_linknode_t;

typedef bool(*bt_linknode_cmp_t)(const bt_linknode_t *node, const void *data);

void bt_linknode_insert_node(bt_linknode_t *des, bt_linknode_t *src, bt_linknode_position pos);
bt_linknode_t *bt_linknode_remove_node(bt_linknode_t *src, bt_linknode_position pos);

/*
 *   Travel list and find the special node.
 *  - Sample code:
 *      @code
 *          if(bt_linknode_travel_node(head_list, bt_linknode_cmp_current, node) != NULL)
 *          {
 *              //the node is in the head_list.
 *          }
 *          else {
 *              //the node is not in the head_list.
 *          }
 *      @endcode
 */
bool bt_linknode_cmp_current(const bt_linknode_t *node, const void *data);

/*
 *   Travel list and find the backward node.
 *  - Sample code:
 *      @code
 *          bt_linknode_t * backward_node;
 *          backward_node = bt_linknode_travel_node(head_list, bt_linknode_cmp_backward, node);
 *          if(backward_node != NULL)
 *          {
 *              //the backward of node is in backward_node.
 *          }
 *          else {
 *              //no backward node.
 *          }
 *      @endcode
 */
bool bt_linknode_cmp_backward(const bt_linknode_t *node, const void *data);

bt_linknode_t *bt_linknode_travel_node(bt_linknode_t *head, bt_linknode_cmp_t func, const void *data);

bt_linknode_t *bt_linknode_travel_and_remove_node(bt_linknode_t *head, bt_linknode_cmp_t func, const void* data);

BT_EXTERN_C_END
#endif /*__BT_LINKNODE_H__*/
