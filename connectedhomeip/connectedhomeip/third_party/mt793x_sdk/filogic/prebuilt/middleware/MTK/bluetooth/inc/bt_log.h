/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
/* Airoha restricted information */


#ifndef __BT_LOG_H__
#define __BT_LOG_H__

#include "bt_platform.h"

BT_EXTERN_C_BEGIN

/*  "[BTIF] atci mode: evt len: %d, total size:%d, copysize:%d"  */
extern const char BTIF_001[];
/*  "[BTIF] atci mode: acl len: %d, total size:%d, copysize:%d"  */
extern const char BTIF_002[];
/*  "[BTIF] unknown rx type:%d"  */
extern const char BTIF_003[];
/*  "[BTIF] hci len: %d, total size:%d, copysize:%d"  */
extern const char BTIF_004[];
/*  "[BTIF] ptr:%x"  */
extern const char BTIF_005[];
/*  "[BTIF] RX Buffer Full"  */
extern const char BTIF_006[];
/*  "[BTIF] payload length:%d"  */
extern const char BTIF_007[];
/*  "[BTIF] OOM"  */
extern const char BTIF_008[];
/*  "[BTIF] unknown packet type: %d"  */
extern const char BTIF_009[];
/*  "[BTIF] unknown rx state:%d"  */
extern const char BTIF_010[];
/*  "[BTIF] isr OOM"  */
extern const char BTIF_011[];
/*  "[BTIF] Power on:enable BT clock fail!: %d \r\n"  */
extern const char BTIF_012[];
/*  "[BTIF] Power on:Check BT clock has been enabled\r\n"  */
extern const char BTIF_013[];
/*  "[BTIF] Power off:Disable BT clock fail!: %d "  */
extern const char BTIF_014[];
/*  "[BTIF] Power off:Check BT clock has been disabled! "  */
extern const char BTIF_015[];
/*  "[BTIF] bt_driver_patch_download_combine_command hci_download_addr=%x"  */
extern const char BTIF_016[];
/*  "[BTIF] chip_is_mt2523E1"  */
extern const char BTIF_017[];
/*  "[BTIF] chip_is_mt2523E2"  */
extern const char BTIF_018[];
/*  "[BTIF] chip is not support"  */
extern const char BTIF_019[];
/*  "[BTIF] download_len: %d"  */
extern const char BTIF_020[];
/*  "[BTIF] patch len: %d"  */
extern const char BTIF_021[];
/*  "[BTIF] Waiting for patch evt timeout!"  */
extern const char BTIF_022[];
/*  "[BTIF] hci_download_addr=%x, end_addr=%x"  */
extern const char BTIF_023[];
/*  "[BTIF] download patch end"  */
extern const char BTIF_024[];
/*  "[BTIF] *** btif port is opened before ***"  */
extern const char BTIF_025[];
/*  "[BTIF] eint fail \n"  */
extern const char BTIF_026[];
/*  "[BTIF] eint succeed.\n"  */
extern const char BTIF_027[];
/*  "[BTIF] N9 actived\r\n"  */
extern const char BTIF_028[];
/*  "[BTIF] N9 not active, Active it\r\n"  */
extern const char BTIF_029[];
/*  "[BTIF] Patch download FAIL!!"  */
extern const char BTIF_030[];
/*  "[BTIF] bt_driver_set_wakeup_cmd_sent_flag: (%d)"  */
extern const char BTIF_031[];
/*  "[BTIF] bt_driver_get_wakeup_cmd_sent_flag"  */
extern const char BTIF_032[];
/*  "[BTIF] bt_driver_set_controller_sleep_flag: (%d)\n"  */
extern const char BTIF_033[];
/*  "[BTIF] bt_driver_get_controller_sleep_flag"  */
extern const char BTIF_034[];
/*  "[BTIF] bt_driver_wakeup_host_status"  */
extern const char BTIF_035[];
/*  "[BTIF] wake up controller:N9_SLEEP(%d)"  */
extern const char BTIF_036[];
/*  "[BTIF] delay 5 ms"  */
extern const char BTIF_037[];
/*  "[BTIF] [wakeup] send 0xFF!N9_SLEEP(%d), CIRQ_MASK(%08x)"  */
extern const char BTIF_038[];
/*  "[BTIF] [wakeup] send 0xFF end! N9_SLEEP(%d), CIRQ_MASK(%08x)"  */
extern const char BTIF_039[];
/*  "[BTIF] wakeup_retry_cancel fail\n"  */
extern const char BTIF_040[];
/*  "[BTIF] wakeup_retry_finish\n"  */
extern const char BTIF_041[];
/*  "[BTIF] wakeup_retry_cancel fail\n"  */
extern const char BTIF_042[];
/*  "[BTIF] wakeup_retry_cancel fail\n"  */
extern const char BTIF_043[];
/*  "[BTIF] wakeup_retry\n"  */
extern const char BTIF_044[];
/*  "[BTIF] bt_driver_wakeup_controller\n"  */
extern const char BTIF_045[];
/*  "[BTIF] N9_SLEEP(%d)\n"  */
extern const char BTIF_046[];
/*  "[BTIF] bt_driver_wakeup_controller_complete"  */
extern const char BTIF_047[];
/*  "[BTIF] bt_driver_wakeup_host_complete"  */
extern const char BTIF_048[];
/*  "[BTIF] unmask eint failed, status:%d"  */
extern const char BTIF_049[];
/*  "[BTIF] bt_driver_eint_handler: wake up eint\n"  */
extern const char BTIF_050[];
/*  "[BTIF] unmask eint failed, status:%d\n"  */
extern const char BTIF_051[];
/*  "[BTIF] hal_eint_init fail"  */
extern const char BTIF_052[];
/*  "[BTIF] hal_eint_register_callback fail"  */
extern const char BTIF_053[];
/*  "[BTIF] hal_eint_unmask fail"  */
extern const char BTIF_054[];
/*  "[BTIF] bt_driver_eint_deinit success"  */
extern const char BTIF_055[];
/*  "[BTIF] Register Sleep Handler"  */
extern const char BTIF_056[];
/*  "[BTIF] Deregister Sleep Handler Failed\n"  */
extern const char BTIF_057[];
/*  "[BTIF] Deregister Sleep Handler"  */
extern const char BTIF_058[];
/*  "[BTIF] Lock Sleep ........ "  */
extern const char BTIF_059[];
/*  "[BTIF] ..........Unlock Sleep"  */
extern const char BTIF_060[];
/*  "[BTIF] uart data ready to read, lenth(%d)"  */
extern const char BTIF_061[];
/*  "[BTIF] INIT UART port, port:%d"  */
extern const char BTIF_062[];
/*  "[BTIF] INIT UART port success"  */
extern const char BTIF_063[];
/*  "[BTIF] set dma, ret:%d"  */
extern const char BTIF_064[];
/*  "[BTIF] uart reg cb, ret:%d"  */
extern const char BTIF_065[];
/*  "[BTIF] bt_driver_relay_serial_port_write_data:size(%d)"  */
extern const char BTIF_066[];
/*  "[BTIF] 0x%02x"  */
extern const char BTIF_067[];
/*  "[BTIF] bt_driver_relay_recv() write data fail\r\n"  */
extern const char BTIF_068[];
/*  "[BTIF] bt_enable_relay_mode:port(%d)"  */
extern const char BTIF_069[];
/*  "[BTIF] hal_uart_deinit success"  */
extern const char BTIF_070[];
/*  "[BTIF] hal_uart_deinit failed: %d"  */
extern const char BTIF_071[];
/*  "[BTIF] bt driver power on failed"  */
extern const char BTIF_072[];
/*  "[BTIF] port service feature is not enable"  */
extern const char BTIF_073[];
/*  "[BTIF] serial port read data: port=0x%08x"  */
extern const char BTIF_074[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, g_bt_driver_port_handle == 0\r\n"  */
extern const char BTIF_075[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, event = %d\r\n"  */
extern const char BTIF_076[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, read event\r\n"  */
extern const char BTIF_077[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback() serial_port_control read fail:(%d) \r\n"  */
extern const char BTIF_078[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback() read data size:(%d)\r\n"  */
extern const char BTIF_079[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, write event\r\n"  */
extern const char BTIF_080[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, g_bt_driver_port_handle == 0\r\n"  */
extern const char BTIF_081[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback() write data fail\r\n"  */
extern const char BTIF_082[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback() write data size:(%d)\r\n"  */
extern const char BTIF_083[];
/*  "[BTIF] bt_driver_relay_serial_port_init() enter, select port:%d \r\n"  */
extern const char BTIF_084[];
/*  "[BTIF] bt_driver_relay_serial_port_init() serial_port_open ret=%d, port_handle=0x%08x \r\n"  */
extern const char BTIF_085[];
/*  "[BTIF] bt_enable_relay_mode:port(%d)"  */
extern const char BTIF_086[];
/*  "[BTIF] bt_enable_relay_mode_with_port_service() fail:%d \r\n"  */
extern const char BTIF_087[];
/*  "[BTIF] bt_enable_relay_mode_with_port_service() success \r\n"  */
extern const char BTIF_088[];
/*  "[BTIF] bt driver relay serial port init failed"  */
extern const char BTIF_089[];
/*  "[BTIF] bt driver power on failed"  */
extern const char BTIF_090[];
/*  "[BTIF] register atci rx callback (0x%08x)"  */
extern const char BTIF_091[];
/*  "[BTIF] bt atci rx cb is already exist: (0x%08x)"  */
extern const char BTIF_092[];
/*  "[BTIF] bt atci rx cb is register success"  */
extern const char BTIF_093[];
/*  "[BTIF] register atci rx callback failed, callback is NULL"  */
extern const char BTIF_094[];
/*  "[BTIF] lock voltage: %d\n"  */
extern const char BTIF_095[];
/*  "[BTIF] unlock voltage: %d\n"  */
extern const char BTIF_096[];
/*  "[BTIF] bt driver power on failed"  */
extern const char BTIF_097[];
/*  "[BTIF] Waiting for evt timeout!"  */
extern const char BTIF_098[];
/*  "[AVM] avm_event_cb(), expired timer: 0x%08x"  */
extern const char BTAVM_001[];
/*  "[AVM] avm_event_cb(), timer: 0x%08x, Err: 0x%08x"  */
extern const char BTAVM_002[];
/*  "[AVM] set_N9_lock(), lock: %d, delay_cnt(us):%d"  */
extern const char BTAVM_003[];
/*  "[AVM] set_N9_lock(), comfirm. lock: %d, delay(times):%d"  */
extern const char BTAVM_004[];
/*  "[AVM] init share buffer(clock offset) status: 0x%08x"  */
extern const char BTAVM_005[];
/*  "[AVM] Get clock offset addr(), NULL address."  */
extern const char BTAVM_006[];
/*  "[AVM] Get clock offset addr(), handle: 0x%08x, offset_addr: 0x%08x"  */
extern const char BTAVM_007[];
/*  "[AVM] init share buffer(DSP info) status: 0x%08x"  */
extern const char BTAVM_008[];
/*  "[AVM] [set_clk] Cancel tracking!"  */
extern const char BTAVM_009[];
/*  "[AVM] [set_clk] carry:%d, nclk:0x%08x, intra:0x%04x, off_n:0x%08x, off_intra:0x%04x,"  */
extern const char BTAVM_010[];
/*  "[AVM] set streaming channel(), acl: 0x%04x, stream: 0x%04x, status: 0x%08x"  */
extern const char BTAVM_011[];
/*  "[AVM] [BT_AVM]config N9 64M, enalbe:%d, result:0x%08x"  */
extern const char BTAVM_012[];
/*  "[AVM] init LE audio buffer status: 0x%08x"  */
extern const char BTAVM_013[];

/*  "[BTIF] ccni_resp, ccni mask event =%x, status = %x \r\n"  */
extern const char BTIF_099[];
/*  "[BTIF] ccni_resp, ccni receive event = %x , src = %x, des = %x\r\n"  */
extern const char BTIF_100[];
/*  "[BTIF] ccni_resp, send sync pointer: 0x%08x\n"  */
extern const char BTIF_101[];
/*  "[BTIF] ccni_resp, ccni clear event = %x , status = %x\r\n"  */
extern const char BTIF_102[];
/*  "[BTIF] ccni_resp, ccni unmask event = %x, status = %x \r\n"  */
extern const char BTIF_103[];
/*  "[BTIF] controller_to_host, ccni mask event =%x, status = %x \r\n"  */
extern const char BTIF_104[];
/*  "[BTIF] controller_to_host, ccni receive event = %x , src = %x, des = %x\r\n"  */
extern const char BTIF_105[];
/*  "[BTIF] driver_setup_done"  */
extern const char BTIF_106[];
/*  "[BTIF] send cmd complete\r\n"  */
extern const char BTIF_107[];
/*  "[BTIF] controller_to_host, ccni clear event = %x , status = %x\r\n"  */
extern const char BTIF_108[];
/*  "[BTIF] controller_to_host, ccni unmask event = %x, status = %x \r\n"  */
extern const char BTIF_109[];
/*  "[BTIF] controller_to_host, ccni clear event = %x , status = %x\r\n"  */
extern const char BTIF_110[];
/*  "[BTIF] controller_to_host, ccni unmask event = %x, status = %x \r\n"  */
extern const char BTIF_111[];
/*  "[BTIF] controller init tx power"  */
extern const char BTIF_112[];
/*  "[BTIF] driver_setup_done"  */
extern const char BTIF_113[];
/*  "[BTIF] controller_to_host, ccni clear event = %x , status = %x\r\n"  */
extern const char BTIF_114[];
/*  "[BTIF] controller_to_host, ccni unmask event = %x, status = %x \r\n"  */
extern const char BTIF_115[];
/*  "[BTIF] [CM4]: host gen irq to controller! \r\n"  */
extern const char BTIF_116[];
/*  "[BTIF] to lock N9"  */
extern const char BTIF_117[];
/*  "[BTIF] to release N9"  */
extern const char BTIF_118[];
/*  "[BTIF] N9 status error: released before."  */
extern const char BTIF_119[];
/*  "[BTIF] bt_driver_send_with_pending_rsp\r\n"  */
extern const char BTIF_120[];
/*  "[BTIF] cur size is not enough !!!!!! \r\n"  */
extern const char BTIF_121[];
/*  "[BTIF] Waiting for evt timeout!"  */
extern const char BTIF_122[];
/*  "[BTIF] to send reset cmd\r\n"  */
extern const char BTIF_123[];
/*  "[BTIF] enable dut success"  */
extern const char BTIF_124[];
/*  "[BTIF] hal_flash_read fail !!!"  */
extern const char BTIF_125[];
/*  "[BTIF] FIND ADDRESS FOR N9 PATCH, 0x%x !!!"  */
extern const char BTIF_126[];
/*  "[BTIF] chip hw version: 0x%08x\r\n"  */
extern const char BTIF_127[];
/*  "[BTIF] patch_data is %x\r\n"  */
extern const char BTIF_128[];
/*  "[BTIF] download_len: %d"  */
extern const char BTIF_129[];
/*  "[BTIF] to release N9\r\n"  */
extern const char BTIF_130[];
/*  "[BTIF] RBIST MODE SUPPORTED\r\n"  */
extern const char BTIF_131[];
/*  "[BTIF] check fail 1\r\n"  */
extern const char BTIF_132[];
/*  "[BTIF] check fail 2\r\n"  */
extern const char BTIF_133[];
/*  "[BTIF] check fail 3\r\n"  */
extern const char BTIF_134[];
/*  "[BTIF] check fail 4\r\n"  */
extern const char BTIF_135[];
/*  "[BTIF] check fail 5\r\n"  */
extern const char BTIF_136[];
/*  "[BTIF] check fail 6\r\n"  */
extern const char BTIF_137[];
/*  "[BTIF] power check success!!!!!!\r\n"  */
extern const char BTIF_138[];
/*  "[BTIF] power_level:%d, power_level_offset:%d,tx_power_enable:%d,tx_power_level:%d,le_tx_power_level:%d,bt_max_power_level:%d\n"  */
extern const char BTIF_139[];
/*  "[BTIF] bt driver power on start\n"  */
extern const char BTIF_140[];
/*  "[BTIF] bt driver setup fail\n"  */
extern const char BTIF_141[];
/*  "[BTIF] bt driver power on end\n"  */
extern const char BTIF_142[];
/*  "[BTIF] bt_driver_power_off"  */
extern const char BTIF_143[];
/*  "[BTIF] serial port read data: port=0x%08x"  */
extern const char BTIF_144[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, g_bt_driver_port_handle == 0\r\n"  */
extern const char BTIF_145[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, event = %d\r\n"  */
extern const char BTIF_146[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, read event\r\n"  */
extern const char BTIF_147[];
/*  "[BTIF] ******read_size is %d, read size is %d\r\n"  */
extern const char BTIF_148[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback() serial_port_control read fail:%d, ret : %d\r\n"  */
extern const char BTIF_149[];
/*  "[BTIF] remain size < cmd header len\r\n"  */
extern const char BTIF_150[];
/*  "[BTIF] remain size < acl header len\r\n"  */
extern const char BTIF_151[];
/*  "[BTIF] pkt indicator: %d\r\n"  */
extern const char BTIF_152[];
/*  "[BTIF] total_payload_size is %d\r\n"  */
extern const char BTIF_153[];
/*  "[BTIF] remain_size < total_size, remain: %d, total: %d \r\n"  */
extern const char BTIF_154[];
/*  "[BTIF] read_size %d !!!!!! \r\n"  */
extern const char BTIF_155[];
/*  "[BTIF] cur size is not enough !!!!!! \r\n"  */
extern const char BTIF_156[];
/*  "[BTIF] bt_driver_relay_serial_port_data_callback, write event\r\n"  */
extern const char BTIF_157[];
/*  "[BTIF] bt_driver_relay_serial_port_init() enter, select port:%d \r\n"  */
extern const char BTIF_158[];
/*  "[BTIF] bt_driver_relay_serial_port_init() serial_port_open ret=%d, port_handle=0x%08x \r\n"  */
extern const char BTIF_159[];
/*  "[BTIF] uart data ready to read, lenth(%d)"  */
extern const char BTIF_160[];
/*  "[BTIF] cur size is not enough !!!!!! \r\n"  */
extern const char BTIF_161[];
/*  "[BTIF] bt_enable_relay_mode:port(%d)"  */
extern const char BTIF_162[];
/*  "[BTIF] bt_enable_relay_mode_with_port_service() fail:%d \r\n"  */
extern const char BTIF_163[];
/*  "[BTIF] bt_enable_relay_mode_with_port_service() success \r\n"  */
extern const char BTIF_164[];
/*  "[BTIF] bt driver relay serial port init failed"  */
extern const char BTIF_165[];
/*  "[BTIF] bt driver power on failed"  */
extern const char BTIF_166[];
/*  "[BTIF] bt driver power on failed"  */
extern const char BTIF_167[];
/*  "[BTIF] register atci rx callback (0x%08x)"  */
extern const char BTIF_168[];
/*  "[BTIF] bt atci rx cb is already exist: (0x%08x)"  */
extern const char BTIF_169[];
/*  "[BTIF] bt atci rx cb is register success"  */
extern const char BTIF_170[];
/*  "[BTIF] register atci rx callback failed, callback is NULL"  */
extern const char BTIF_171[];
/*  "[BTIF] validate_public_key result: %d "  */
extern const char BTIF_172[];
/*  "[GAP] default get_init_public_address"  */
extern const char BTGAP_001[];
/*  "[BTIF] trigger n9 dump: pb flag (%d)"  */
extern const char BTIF_173[];
/*  "[BTIF] size3:%d"  */
extern const char BTIF_174[];
/*  "[BTIF] trigger n9 dump: l2cap len (%d) < hci len (%d)"  */
extern const char BTIF_175[];
/*  "[BTIF] size4:%d"  */
extern const char BTIF_176[];
/*  "[BTIF] receive l2cap header = 1"  */
extern const char BTIF_177[];
/*  "[BTIF] ptr:%x"  */
extern const char BTIF_178[];
/*  "[BTIF] RX Buffer Full"  */
extern const char BTIF_179[];
/*  "[BTIF] OOM away"  */
extern const char BTIF_180[];
/*  "[BTIF] OOM"  */
extern const char BTIF_181[];
/*  "[BTIF] ACL Data rx OOM"  */
extern const char BTIF_182[];
/*  "[BTIF] unknown rx state:%d"  */
extern const char BTIF_183[];
/*  "[BTIF] isr OOM"  */
extern const char BTIF_184[];
/*  "[BTIF] *** btif port is opened before ***"  */
extern const char BTIF_185[];
/*  "[BTIF] eint fail \n"  */
extern const char BTIF_186[];
/*  "[BTIF] eint succeed.\n"  */
extern const char BTIF_187[];
/*  "[BTIF] *** btif wmt routine end ***"  */
extern const char BTIF_188[];
/*  "[BTIF] *** btif wmt routine end ***"  */
extern const char BTIF_189[];
/*  "[BTIF] [addr]:0x%x, [val]:0x%x\n"  */
extern const char BTIF_190[];
/*  "[BTIF] bt_driver_sleep_command_timeout-data:(%d)"  */
extern const char BTIF_191[];
/*  "[BTIF] bt_driver_uart_sleep, is_enable:%d\n"  */
extern const char BTIF_192[];
/*  "[BTIF] bt_timer_start fail\n"  */
extern const char BTIF_193[];
/*  "[BTIF] sleep uart command fail\n"  */
extern const char BTIF_194[];
/*  "[BTIF] bt_timer_start fail\n"  */
extern const char BTIF_195[];
/*  "[BTIF] wake up uart command fail\n"  */
extern const char BTIF_196[];
/*  "[BTIF] bt_driver_wakeup_controller\n"  */
extern const char BTIF_197[];
/*  "[BTIF] bt_driver_set_wakeup_cmd_sent_flag: (%d)"  */
extern const char BTIF_198[];
/*  "[BTIF] bt_driver_get_wakeup_cmd_sent_flag"  */
extern const char BTIF_199[];
/*  "[BTIF] bt_driver_wakeup_host_complete"  */
extern const char BTIF_200[];
/*  "[BTIF] unmask eint failed, status:%d"  */
extern const char BTIF_201[];
/*  "[BTIF] bt_driver_get_controller_sleep_flag"  */
extern const char BTIF_202[];
/*  "[BTIF] bt_driver_wakeup_host_status"  */
extern const char BTIF_203[];
/*  "[BTIF] bt_driver_eint_handler: wake up eint\n"  */
extern const char BTIF_204[];
/*  "[BTIF] hal_nvic_init fail"  */
extern const char BTIF_205[];
/*  "[BTIF] hal_nvic_register_isr_handler fail"  */
extern const char BTIF_206[];
/*  "[BTIF] bt_driver_eint_init success"  */
extern const char BTIF_207[];
/*  "[BTIF] bt_driver_eint_deinit success"  */
extern const char BTIF_208[];
/*  "[BTIF] Register Sleep Handler"  */
extern const char BTIF_209[];
/*  "[BTIF] Deregister Sleep Handler Failed\n"  */
extern const char BTIF_210[];
/*  "[BTIF] Deregister Sleep Handler"  */
extern const char BTIF_211[];
/*  "[BTIF] Lock Sleep ........ "  */
extern const char BTIF_212[];
/*  "[BTIF] bt_driver_wakeup_controller_complete"  */
extern const char BTIF_213[];
/*  "[BTIF] ..........Unlock Sleep"  */
extern const char BTIF_214[];
/*  "[BTIF] bt_driver_set_controller_sleep_flag: (%d)\n"  */
extern const char BTIF_215[];
/*  "[GAP] default get_init_public_address"  */
extern const char BTGAP_002[];
/*  "[BTIF] acl2queue, oom"  */
extern const char BTIF_216[];
/*  "[BTIF] event2queue, oom"  */
extern const char BTIF_217[];
/*  "[BTIF] [CM4]: host tx notify controller! \r"*/
extern const char BTIF_218[];
/*.."[BTIF] [CM4]receive controller reset complete event \r"*/
extern const char BTIF_219[];
/*  "[BTIF] [CM4]: bt driver share buffer lock \r"*/
extern const char BTIF_220[];
/*.."[BTIF] [CM4]bt driver share buffer unlock \r"*/
extern const char BTIF_221[];
/*.."[BTIF] [CM4] bt driver not powered on \n*/
extern const char BTIF_222[];
/*.."[BTIF] [CM4] set tx power level success \n*/
extern const char BTIF_223[];
/*.."[BTIF] [CM4] bt driver enter dut mdoe fail \n*/
extern const char BTIF_224[];
/*.."[BTIF] [CM4] bt driver can not find the specific queue header (%d)\n*/
extern const char BTIF_225[];
/*.."[BTIF] [CM4] bt_driver_SLT_test_start result_cb is null \n*/
extern const char BTIF_226[];
/*.."[BTIF] [CM4] bt_driver_SLT_test_start enable relay failed \n*/
extern const char BTIF_227[];
/*.."[BTIF] [CM4] the specific queue is empty (type:%d)\n*/
extern const char BTIF_228[];
/*.."[BTIF] [CM4] bt driver relay mode has entered before\n*/
extern const char BTIF_229[];
/*.."[BTIF] [CM4] bt driver dut mode has entered before \n*/
extern const char BTIF_230[];
/*.."[BTIF] [CM4] bt driver relay deinit \n*/
extern const char BTIF_231[];
/*  "[BTIF] %c"  */
extern const char BTIF_232[];
/*  "[BTIF] (len:%lu):"  */
extern const char BTIF_233[];
/*  "[BTIF] %.2X "  */
extern const char BTIF_234[];
/*  "[BTIF] \n"  */
extern const char BTIF_235[];
/*  "[BTIF] %c"  */
extern const char BTIF_236[];
/*  "[BTIF] %c"  */
extern const char BTIF_237[];
/*  "[BTIF] [CM4]: [Fail] hal_ccni_set_event error status = %d, mask = 0x%08x, status = 0x%08x \r\n"  */
extern const char BTIF_238[];
/*  "[BTIF] [CM4]: [Fail] hal_ccni_set_event error status = %x\r\n"  */
extern const char BTIF_239[];
/*  "[BTIF] [CM4]: [Fail] hal_ccni_set_event error status = %x\r\n"  */
extern const char BTIF_240[];
/*  "[BTIF] start*************\r\n"  */
extern const char BTIF_241[];
/*  "[BTIF] 0x%02x "  */
extern const char BTIF_242[];
/*  "[BTIF] end***************\r\n"  */
extern const char BTIF_243[];
/*  "[BTIF] [CM4]: [Fail] hal_ccni_set_event error status = %d, mask = 0x%08x, status = 0x%08x \r\n"  */
extern const char BTIF_244[];
/*  "[BTIF] N9 target addr:0x%08x, sub bin start addr:0x%08x, len:0x%08x, num:0x%08x \r\n"  */
extern const char BTIF_245[];
/*  "[BTIF] N9 target addr:0x%08x, common bin start addr:0x%08x, len:0x%08x \r\n"  */
extern const char BTIF_246[];
/*  "[BTIF] Copy HW patch to common bin, start addr:0x%08x \r\n"  */
extern const char BTIF_247[];
/*  "[BTIF] N9 target addr:0x%08x, sub bin start addr:0x%08x, len:0x%08x, num:0x%08x \r\n"  */
extern const char BTIF_248[];
/*  "[BTIF] N9 target addr:0x%08x, common bin start addr:0x%08x, len:0x%08x \r\n"  */
extern const char BTIF_249[];
/*  "[BTIF] The %d count of N9 sub patch download complete!\r\n"  */
extern const char BTIF_250[];
/*  "[BTIF] set DL status as complete, mm_info->reserved = %x"  */
extern const char BTIF_251[];
/*  "[BTIF] enter dynamic DL CCNI isr, selected sub bin number:%d \r\n"  */
extern const char BTIF_252[];
/*  "[BTIF] common bin start addr:0x%08x, common bin len:0x%08x, sub bin start addr:0x%08x, sub bin len:0x%x, sub bin number:0x%08x \r\n"  */
extern const char BTIF_253[];
/*  "[BTIF] N9 DSP start address = 0x%08x \r\n"  */
extern const char BTIF_254[];
/*  "[BTIF] set DL status as complete, mm_info->reserved = %x"  */
extern const char BTIF_255[];
/*  "[BTIF] waiting set up done\r\n"  */
extern const char BTIF_256[];
/*  "[BTIF] Received wrong setup done event!\r\n"  */
extern const char BTIF_257[];
/*  "[BTIF] BDR DPC_Value[%d] = 0x%04x, RFCR_Value[%d] = 0x%04x \r\n"  */
extern const char BTIF_258[];
/*  "[BTIF] EDR DPC_Value[%d] = 0x%04x, RFCR_Value[%d] = 0x%04x \r\n"  */
extern const char BTIF_259[];
/*  "[BTIF] to lock 1.1v, send reset cmd\r\n"  */
extern const char BTIF_260[];
/*  "[BTIF] set tx power level success\R\N"  */
extern const char BTIF_261[];
/*  "[BTIF] to unlock 1.1v\r\n "  */
extern const char BTIF_262[];
/*  "[BTIF] Power off N9 duration1 = %d us, duration2 = %d us,  0xA21203E0 = 0x%x, 0xA21203B0 = 0x%x\r\n "  */
extern const char BTIF_263[];
/*  "[BTIF] driver_clear_rx_tx_queue packet addr = 0x%x \r\n  packet)"  */
extern const char BTIF_264[];
/*  "[BTIF] unlock sleep handle when [BTIF]IF driver power off\r\n "  */
extern const char BTIF_265[];
/*  "[BTIF] 0x%02x   [BTIF]_driver_relay_rx_buf[read_size + i])"  */
extern const char BTIF_266[];
/*  "[BTIF] %02x"  */
extern const char BTIF_267[];
/*  "[BTIF] [BTIF]_driver to lock 1.1v\r\n "  */
extern const char BTIF_268[];
/*  "[BTIF] [BTIF]_driver to unlock 1.1v\r\n "  */
extern const char BTIF_269[];
/*  "[BTIF] %02x"  */
extern const char BTIF_270[];
/*  "[BTIF] [BTIF]IF driver usb init success\r\n "  */
extern const char BTIF_271[];
/*  "[BTIF] Waiting for evt timeout!\r\n "  */
extern const char BTIF_272[];
/*  "[BTIF] open port fail, status:%d  status"  */
extern const char BTIF_273[];
/*  "[BTIF] \n "  */
extern const char BTIF_274[];
/*  "[BTIF] %02x   p_buf[i]"  */
extern const char BTIF_275[];
/*  "[BTIF] \n "  */
extern const char BTIF_276[];
/*  "[BTIF] Host take hw semaphore take time : %d us  count_end - count_start"  */
extern const char BTIF_277[];
/*  "[BTIF] cannot get lock, current lock is %d\r\n  cur_lock"  */
extern const char BTIF_278[];
/*  "[BTIF] [BTIF]_driver_SLT_test_start result_cb is null\n "  */
extern const char BTIF_279[];
/*  "[BTIF] [BTIF]_driver_SLT_test_start enable relay failed\n "  */
extern const char BTIF_280[];
/*  "[BTIF] [BTIF]_driver_controller_init_support [Start]... "  */
extern const char BTIF_281[];
/*  "[BTIF] [BTIF]_driver_controller_init_support [Done]... "  */
extern const char BTIF_282[];
/*  "[BTIF] firmware type is invalid! "  */
extern const char BTIF_283[];
/*  "[BTIF] Set firmware type = %d \r\n type"  */
extern const char BTIF_284[];
/*  "[BTIF] selected sub_bin_number:%d, n9_sub_bin_count:%d\r\n  patch_number, n9_sub_bin_count"  */
extern const char BTIF_285[];
/*  "[BTIF] n9_bin_count (%d)\r\n  i"  */
extern const char BTIF_286[];
/*  "[BTIF] chip id:0x%02x  chip_id"  */
extern const char BTIF_287[];
/*  "[BTIF] N9 common bin count:%d\r\n  n9_bin_count"  */
extern const char BTIF_288[];
/*  "[BTIF] n9_bin_count (%d)\r\n  n9_bin_count"  */
extern const char BTIF_289[];
/*  "[BTIF] found mode (%c,%c,%c,%c) in N9 multi bin, patch data start addr 0x(%08x)\r\n"  */
extern const char BTIF_290[];
/*  "[BTIF] can not find mode (index:%d) in n9 multi bin!\r\n  [BTIF]_n9_bin_type_index"  */
extern const char BTIF_291[];

/*  "[GAP] default get_init_public_address"  */
extern const char BTGAP_003[];
/*  "[BT] rx oom\n"  */
extern const char BT_001[];
/*  "[BT] hci_log_util_info(): out of memory"  */
extern const char BT_002[];
/*  "[BT] driver_version"  */
extern const char BT_003[];
/*  "[BT] driver last commit"  */
extern const char BT_004[];
/*  "[BT] bt_lib_version_hci"  */
extern const char BT_005[];
/*  "[BT] STACK_version"  */
extern const char BT_006[];
/*  "[BT] [GAP]driver power off st:%d"  */
extern const char BT_007[];
/*  "[BT] [CTP]bt_ctp_connect_response handle=0x%x, accept=0x%x"  */
extern const char BT_008[];
/*  "[BT] [CTP]bt_ctp_send handle=0x%x, data=0x%x, data_length=0x%x"  */
extern const char BT_009[];
/*  "[BT] [CTP]bt_ctp_connect_response_oom_callback node=0x%x"  */
extern const char BT_010[];
/*  "[BT] [CTP]bt_ctp_l2cap_callback channel=0x%x, info=0x%x"  */
extern const char BT_011[];
/*  "[BT] [CTP]bt_ctp_l2cap_context_op_callback event=0x%x, param=0x%x"  */
extern const char BT_012[];
/*  "[BT] [CTP]bt_ctp_send PDU allocated in 0x%x."  */
extern const char BT_013[];
/*  "[BT] [CTP]bt_ctp_send Send PDU FAILED."  */
extern const char BT_014[];
/*  "[BT] [CTP]bt_ctp_l2cap_context_op_callback Channel allocated in 0x%x."  */
extern const char BT_015[];
/*  "[BT] [CTP]bt_ctp_rho_get_data length NULL address!"  */
extern const char BT_016[];
/*  "[BT] [CTP]bt_ctp_rho_get_data_length data_length: 0x%x."  */
extern const char BT_017[];
/*  "[BT] [CTP]bt_ctp_rho_get_data NULL data or address!"  */
extern const char BT_018[];
/*  "[BT] [CTP]bt_ctp_rho_get_data Connection NOT found!"  */
extern const char BT_019[];
/*  "[BT] [CTP]bt_ctp_rho_get_data Status: 0x%x."  */
extern const char BT_020[];
/*  "[BT] [CTP]bt_ctp_rho_update_connection role=0x%x, address=0x%x, data=0x%x"  */
extern const char BT_021[];
/*  "[BT] [CTP]bt_ctp_rho_update_connection Connection NOT found!"  */
extern const char BT_022[];
/*  "[BT] [CTP]bt_ctp_rho_update_connection Connection allocated in 0x%x"  */
extern const char BT_023[];
/*  "[BT] [CTP]bt_ctp_rho_update_connection Connection allocated FAILED!"  */
extern const char BT_024[];
/*  "[SM] Private Key = "  */
extern const char BTSM_001[];
/*  "[SM] Public Key X = "  */
extern const char BTSM_002[];
/*  "[SM] Public Key Y = "  */
extern const char BTSM_003[];
/*  "[SM] DHKey = "  */
extern const char BTSM_004[];
/*  "[GATT] Waiting handle value confirmation.\n"  */
extern const char BTGATT_001[];
/*  "[GATT] Waiting handle value confirmation.\n"  */
extern const char BTGATT_002[];
/*  "[GATT] Waiting handle value confirmation.\n"  */
extern const char BTGATT_003[];
/*  "[SM] Private Key = "  */
extern const char BTSM_005[];
/*  "[SM] Public Key X = "  */
extern const char BTSM_006[];
/*  "[SM] Public Key Y = "  */
extern const char BTSM_007[];
/*  "[SM] DHKey = "  */
extern const char BTSM_008[];
/*  "[GATT] Waiting handle value confirmation.\n"  */
extern const char BTGATT_004[];
/*  "[GATT] Waiting handle value confirmation.\n"  */
extern const char BTGATT_005[];
/*  "[GATT] Waiting handle value confirmation.\n"  */
extern const char BTGATT_006[];
/*  "[ATT] bt_timer_cancel status: %d"  */
extern const char BTATT_001[];
/*  "[ATT] Send Error response ErrorOPCode[0x%02x] ErrorHandle[0x%02x] ErrorCode[0x%02x]"  */
extern const char BTATT_002[];
/*  "[ATT] bt_att_get_packet(), out of timer sending command: 0x%04x"  */
extern const char BTATT_003[];
/*  "[ATT] connection not found, handle = 0x%08x"  */
extern const char BTATT_004[];
/*  "[ATT] connection is in use.\n"  */
extern const char BTATT_005[];
/*  "[ATT] out of timer sending command: 0x%04x"  */
extern const char BTATT_006[];
/*  "[ATT] connection not found, handle = 0x%08x"  */
extern const char BTATT_007[];
/*  "[ATT] connection is in use.\n"  */
extern const char BTATT_008[];
/*  "[ATT] out of memory sending command: 0x%04x\n"  */
extern const char BTATT_009[];
/*  "[ATT] bt_att_send_packet"  */
extern const char BTATT_010[];
/*  "[ATT] connection not found, handle = 0x%08x"  */
extern const char BTATT_011[];
/*  "[ATT] connection is in use.\n"  */
extern const char BTATT_012[];
/*  "[ATT] [Big ERROR]out of memory creating timer. That may have Tx packet memory leak, because Tx packet is allocated in this stage, but not send out!!!\n"  */
extern const char BTATT_013[];
/*  "[GAP] [AUTH]enter bt_gap_auth_request() BOND conn 0x%x, conn 0x%x, handle 0x%x"  */
extern const char BTGAP_004[];
/*  "[GAP] [AUTH]bt_gap_auth_request() return status %x"  */
extern const char BTGAP_005[];
/*  "[GAP] [AUTH]bt_gap_reply_oob_data_request(%x)"  */
extern const char BTGAP_006[];
/*  "[GAP] [AUTH]bt_gap_reply_oob_data_request fails"  */
extern const char BTGAP_007[];
/*  "[GAP] [AUTH]bt_gap_reply_user_confirm_request(%d)"  */
extern const char BTGAP_008[];
/*  "[GAP] [AUTH]bt_gap_reply_user_confirm_request fails"  */
extern const char BTGAP_009[];
/*  "[GAP] [AUTH]bt_gap_reply_passkey_request fails"  */
extern const char BTGAP_010[];
/*  "[GAP] [AUTH]bt_gap_reply_passkey_request(%d)"  */
extern const char BTGAP_011[];
/*  "[GAP] [AUTH]bt_gap_auth_param() cmd_code %x"  */
extern const char BTGAP_012[];
/*  "[GAP] [AUTH] connection:%x, ask for link key"  */
extern const char BTGAP_013[];
/*  "[GAP] [AUTH] connection:%x, waiting IO replay"  */
extern const char BTGAP_014[];
/*  "[GAP] [AUTH] connection:%x, waiting user confirm %d"  */
extern const char BTGAP_015[];
/*  "[GAP] [AUTH] connection:%x, waiting user passkey"  */
extern const char BTGAP_016[];
/*  "[GAP] [AUTH] connection:%x, waiting OOB data"  */
extern const char BTGAP_017[];
/*  "[GAP] [AUTH]bt_gap_cancel_auth(), conn 0x%x, BOND conn 0x%x"  */
extern const char BTGAP_018[];
/*  "[GAP] [AUTH]bt_gap_cancel_auth(), conn status 0x%x, bonding_start %d"  */
extern const char BTGAP_019[];
/*  "[GAP] [AUTH]bt_gap_auth_complete(), conn 0x%x, BOND conn 0x%x"  */
extern const char BTGAP_020[];
/*  "[GAP] [AUTH]bt_gap_auth_complete(), conn status 0x%x, bonding_start %d"  */
extern const char BTGAP_021[];
/*  "[GAP] [AUTH]bt_gap_reply_io_capability_request() oob %x, auth_req %x, io %x"  */
extern const char BTGAP_022[];
/*  "[GAP] [AUTH]bt_gap_reply_io_capability_request fails"  */
extern const char BTGAP_023[];
/*  "[GAP] [AUTH]bt_gap_reject_io_capability_request()"  */
extern const char BTGAP_024[];
/*  "[GAP] [AUTH]bt_gap_reply_key_request fails"  */
extern const char BTGAP_025[];
/*  "[GAP] [AUTH]bt_gap_send_auth_operation()cmd %x"  */
extern const char BTGAP_026[];
/*  "[GAP] [AUTH]ACL(%x) pairing timeout, op = %x"  */
extern const char BTGAP_027[];
/*  "[GAP] [AUTH] passkey display end %x"  */
extern const char BTGAP_028[];
/*  "[GAP] [AUTH]ACL(%x) start timer fails"  */
extern const char BTGAP_029[];
/*  "[GAP] [AUTH] bt_gap_send_public_key_validation_result() on handle 0x%x, valid 0x%x"  */
extern const char BTGAP_030[];
/*  "[GAP] timer 0x%08x expired"  */
extern const char BTGAP_031[];
/*  "[GAP] [AUTH]bt_gap_auth_evt_callback()timer_id %x"  */
extern const char BTGAP_032[];
/*  "[GAP] [AUTH] connection:%x, invalid link key notify"  */
extern const char BTGAP_033[];
/*  "[GAP] [AUTH] reject the io capability request, line: %d"  */
extern const char BTGAP_034[];
/*  "[GAP] [AUTH] pincode req, line: %d, connection 0x%x"  */
extern const char BTGAP_035[];
/*  "[GAP] [AUTH] -------------> passkey %d <-------------"  */
extern const char BTGAP_036[];
/*  "[GAP] [AUTH] passkey display end %x"  */
extern const char BTGAP_037[];
/*  "[GAP] [AUTH] connection:%x, link key phase out"  */
extern const char BTGAP_038[];
/*  "[GAP] [AUTH] connection:%x, link key update"  */
extern const char BTGAP_039[];
/*  "[GAP] [AUTH] handle:0x%x, key type %d"  */
extern const char BTGAP_040[];
/*  "[GAP] bt conn_handle %x, hci_handle %x, status %x, sniff_status %d, role %d, pending_op %d, sniff_lock %d, [%02x-%02x-%02x-%02x-%02x-%02x]"  */
extern const char BTGAP_041[];
/*  "[GAP] bonding packet:%x, confirm_value:%x, oob:%x, auth_req:%x, io:%x, timer_op:%x"  */
extern const char BTGAP_042[];
/*  "[GAP] SCO handle:%x, link_type:%x, disconn_acl:%x"  */
extern const char BTGAP_043[];
/*  "[GAP] channels: %x, state %x, local_id %x, remote_id %x, flag %x, callback %x"  */
extern const char BTGAP_044[];
/*  "[GAP] trigger timer: mask %d, bt conn_handle %x, hci_handle %x, status %x, sniff_status %d, role %d, pending_op %d, sniff_lock %d, [%02x-%02x-%02x-%02x-%02x-%02x]"  */
extern const char BTGAP_045[];
/*  "[GAP] [CONN] bt_gap_cancel_connection()"  */
extern const char BTGAP_046[];
/*  "[GAP] [CONN] bt_gap_connect()"  */
extern const char BTGAP_047[];
/*  "[GAP] bt_gap_disconnect_with_reason"  */
extern const char BTGAP_048[];
/*  "[GAP] bt_gap_disconnect"  */
extern const char BTGAP_049[];
/*  "[GAP] bt_gap_disconnect: connection handle %x is not existed"  */
extern const char BTGAP_050[];
/*  "[GAP] bt_gap_disconnect: connection handle %x had disconnected or is disconnecting 0x%x"  */
extern const char BTGAP_051[];
/*  "[GAP] [CONN] bt_gap_disconnect() handle %x"  */
extern const char BTGAP_052[];
/*  "[GAP] ERROR: connection(%x) operation %d -> %d is overwrite"  */
extern const char BTGAP_053[];
/*  "[GAP] [CONN] bt_gap_cancel_pending_operation() handle %x"  */
extern const char BTGAP_054[];
/*  "[GAP] [CONN] sending pending_op: %d, handle: %x, buffer %x, cmd %x"  */
extern const char BTGAP_055[];
/*  "[GAP] [CONN]ERROR sending fails %x pending_op: %d, handle: %x, buffer %x, cmd %x"  */
extern const char BTGAP_056[];
/*  "[GAP] [CONN] bt_gap_check_connection_status() start op_timer %x"  */
extern const char BTGAP_057[];
/*  "[GAP] [CONN] bt_gap_create_sco() ACL not found"  */
extern const char BTGAP_058[];
/*  "[GAP] [CONN] bt_gap_create_sco() on handle %x"  */
extern const char BTGAP_059[];
/*  "[GAP] [CONN] bt_gap_create_sco() fails"  */
extern const char BTGAP_060[];
/*  "[GAP] ERROR: connection(%x) operation %d -> %d is overwrite"  */
extern const char BTGAP_061[];
/*  "[GAP] [CONN] bt_gap_disconnect_sco(%x) %x, "  */
extern const char BTGAP_062[];
/*  "[GAP] ERROR: connection(%x) operation %d -> %d is overwrite"  */
extern const char BTGAP_063[];
/*  "[GAP] [CONN] bt_gap_set_sco_status(%x) %x, %d"  */
extern const char BTGAP_064[];
/*  "[GAP] enter bt_gap_connection_sniff_timeout()"  */
extern const char BTGAP_065[];
/*  "[GAP] connection fails: error: %x"  */
extern const char BTGAP_066[];
/*  "[GAP] bt_gap_write_link_policy: sniff feature is disabled by bt_get_feature_mask_configuration()"  */
extern const char BTGAP_067[];
/*  "[GAP] [CONN]bt_gap_notify_sco_status(%x, %d)"  */
extern const char BTGAP_068[];
/*  "[GAP] bt_gap_exit_sniff_mode: connection handle %x is not existed"  */
extern const char BTGAP_069[];
/*  "[GAP] bt_gap_exit_sniff_mode: connection handle %x is in %d staus"  */
extern const char BTGAP_070[];
/*  "[GAP] [CONN] bt_gap_exit_sniff_mode()"  */
extern const char BTGAP_071[];
/*  "[GAP] ERROR: connection(%x) operation %d -> %d is overwrite"  */
extern const char BTGAP_072[];
/*  "[GAP] [CONN]bt_gap_connection_lost() can't find the handle(%x) reason(%x)"  */
extern const char BTGAP_073[];
/*  "[GAP] connection lost: error: %x"  */
extern const char BTGAP_074[];
/*  "[GAP] [CONN]bt_gap_notify_connection_status() handle %x, status %x, error %x"  */
extern const char BTGAP_075[];
/*  "[GAP] [CONN]bt_gap_handle_connection() handle %x, alloc 0x%x for op: %d"  */
extern const char BTGAP_076[];
/*  "[GAP] [CONN] sending command OOM"  */
extern const char BTGAP_077[];
/*  "[GAP] [CONN]ERROR: bt_gap_handle_connection() handle %x, buffer busy 0x%x for op: %d"  */
extern const char BTGAP_078[];
/*  "[GAP] [CONN] bt_gap_handle_connection() handle %x, option %x"  */
extern const char BTGAP_079[];
/*  "[GAP] [CONN] conn[%x] sniff interval[0X%x, 0X%x], attempt: %d, timeout %d"  */
extern const char BTGAP_080[];
/*  "[GAP] timer 0x%08x expired"  */
extern const char BTGAP_081[];
/*  "[GAP] [CONN] bt_gap_connected_evt_callback() status %x, timer_id %x"  */
extern const char BTGAP_082[];
/*  "[GAP] BT_HCI_CMD_AUTH_REQ fail 0x%x"  */
extern const char BTGAP_083[];
/*  "[GAP] [CONN]BT_HCI_CMD_WRITE_LINK_POLICY(0x%x) returns 0x%x disable sniff count %d"  */
extern const char BTGAP_084[];
/*  "[GAP] [CONN]Disconnect cmd disallowed handle %x, status %x"  */
extern const char BTGAP_085[];
/*  "[GAP] [CONN] conn[%x] sniff mode changed interval[0X%x]"  */
extern const char BTGAP_086[];
/*  "[GAP] [CONN]connection complete: handle %x"  */
extern const char BTGAP_087[];
/*  "[GAP] ERROR: connection(%x) operation %d -> %d is overwrite"  */
extern const char BTGAP_088[];
/*  "[GAP] reject sco req, sco: %x, conn: %x"  */
extern const char BTGAP_089[];
/*  "[GAP] ERROR: connection(%x) operation %d -> %d is overwrite"  */
extern const char BTGAP_090[];
/*  "[GAP] Evt vendor esco connetion pending can't find connection by handle %x"  */
extern const char BTGAP_091[];
/*  "[GAP] bt_gap_connection_idle_check:%x, channel_list: %x, sco: %x"  */
extern const char BTGAP_092[];
/*  "[GAP] timer_id 0x%08x"  */
extern const char BTGAP_093[];
/*  "[GAP] controller not support commands byte%d [read:0x%02x, expected:0x%02x]"  */
extern const char BTGAP_094[];
/*  "[GAP] controller not support features byte%d [read:0x%02x, expected:0x%02x]"  */
extern const char BTGAP_095[];
/*  "[GAP] acl_le_credit(%d), acl_le_packet_length(%d)"  */
extern const char BTGAP_096[];
/*  "[GAP] acl_credit(%d), acl_packet_length(%d)"  */
extern const char BTGAP_097[];
/*  "[GAP] controller not support le features byte%d [read:0x%02x, expected:0x%02x]"  */
extern const char BTGAP_098[];
/*  "[GAP] bd addr(%02X:%02X:%02X:%02X:%02X:%02X)\n"  */
extern const char BTGAP_099[];
/*  "[GAP] white list size(%d)"  */
extern const char BTGAP_100[];
/*  "[GAP] resolving list size(%d)"  */
extern const char BTGAP_101[];
/*  "[GAP] maximum advtertising data length (%d)"  */
extern const char BTGAP_102[];
/*  "[GAP] Secure Connection Only Mode set to %d"  */
extern const char BTGAP_103[];
/*  "[GAP] power bt_gap_le_init_proc: timer_id(0x%08x), wait(%d)"  */
extern const char BTGAP_104[];
/*  "[GAP] power on event timer 0x%08x expired"  */
extern const char BTGAP_105[];
/*  "[GAP] send"  */
extern const char BTGAP_106[];
/*  "[GAP] command %d"  */
extern const char BTGAP_107[];
/*  "[GAP] disable mask 0x%x"  */
extern const char BTGAP_108[];
/*  "[GAP] callback %x"  */
extern const char BTGAP_109[];
/*  "[GAP] init cmd failed: cmd_id(0x%08x), status %d"  */
extern const char BTGAP_110[];
/*  "[GAP] get_init_public_address"  */
extern const char BTGAP_111[];
/*  "[GAP] get_init_public_address NULL"  */
extern const char BTGAP_112[];
/*  "[GAP] interval_min=0x%04x"  */
extern const char BTGAP_113[];
/*  "[GAP] interval_max=0x%04x"  */
extern const char BTGAP_114[];
/*  "[GAP] slave_latency=0x%04x"  */
extern const char BTGAP_115[];
/*  "[GAP] timeout_multiplier=0x%04x"  */
extern const char BTGAP_116[];
/*  "[GAP] default_bt_driver_vcore_lock_1P1"  */
extern const char BTGAP_117[];
/*  "[GAP] default_bt_driver_vcore_unlock_1P1"  */
extern const char BTGAP_118[];
/*  "[GAP] bt_vcore_unlock_1P1 0x%x, power off %d, scan type 0x%x, queue empty 0x%x, adv type 0x%x, recon_buf 0x%x, wb_scan 0x%x, ssc 0x%x"  */
extern const char BTGAP_119[];
/*  "[GAP] bt_vcore_lock_1P1 0x%x"  */
extern const char BTGAP_120[];
/*  "[GAP] bt_power_on curr %x"  */
extern const char BTGAP_121[];
/*  "[GAP] Public addr: %02x-%02x-%02x-%02x-%02x-%02x"  */
extern const char BTGAP_122[];
/*  "[GAP] Random addr: %02x-%02x-%02x-%02x-%02x-%02x"  */
extern const char BTGAP_123[];
/*  "[GAP] LE conn: context %x, handle %x, role %d, [%02x-%02x-%02x-%02x-%02x-%02x]<->%d[%02x-%02x-%02x-%02x-%02x-%02x]"  */
extern const char BTGAP_124[];
/*  "[GAP] [MISC] bt_gap_cmd_sending()"  */
extern const char BTGAP_125[];
/*  "[GAP] bt_gap_le_power_off_proc: timer_id(0x%08x)"  */
extern const char BTGAP_126[];
/*  "[GAP] bt_power_off curr %x"  */
extern const char BTGAP_127[];
/*  "[GAP] bt_gap_le_set_white_list_proc: timer_id(0x%08x)"  */
extern const char BTGAP_128[];
/*  "[GAP] set white list timer 0x%08x expired"  */
extern const char BTGAP_129[];
/*  "[GAP] bt_gap_le_set_white_list"  */
extern const char BTGAP_130[];
/*  "[GAP] bt_gap_le_set_resolving_config_proc: timer_id(0x%08x)"  */
extern const char BTGAP_131[];
/*  "[GAP] set resolving config timer 0x%08x expired"  */
extern const char BTGAP_132[];
/*  "[GAP] bt_gap_le_set_resolving_list"  */
extern const char BTGAP_133[];
/*  "[GAP] bt_gap_le_set_address_resolution_enable"  */
extern const char BTGAP_134[];
/*  "[GAP] bt_gap_le_set_resolvable_private_address_timeout"  */
extern const char BTGAP_135[];
/*  "[GAP] bt_gap_le_set_scan_proc: timer_id(0x%08x)"  */
extern const char BTGAP_136[];
/*  "[GAP] set scanning event timer 0x%08x expired"  */
extern const char BTGAP_137[];
/*  "[GAP] bt_gap_le_set_scan"  */
extern const char BTGAP_138[];
/*  "[GAP] bt_gap_le_bond_reply_proc"  */
extern const char BTGAP_139[];
/*  "[GAP] bt_gap_le_bonding_reply"  */
extern const char BTGAP_140[];
/*  "[GAP] bt_gap_le_bond: handle(0x%04x)"  */
extern const char BTGAP_141[];
/*  "[GAP] bt_gap_le_bond: BT_SMP_CODE_SECURITY_REQ: Busy"  */
extern const char BTGAP_142[];
/*  "[GAP] bt_gap_le_bond: sending BT_SMP_CODE_SECURITY_REQ"  */
extern const char BTGAP_143[];
/*  "[GAP] bt_gap_le_connection_proc: timer_id(0x%08x)"  */
extern const char BTGAP_144[];
/*  "[GAP] connection proc timer 0x%08x expired"  */
extern const char BTGAP_145[];
/*  "[GAP] bt_gap_le_connect"  */
extern const char BTGAP_146[];
/*  "[GAP] connection is already existed"  */
extern const char BTGAP_147[];
/*  "[GAP] reach maxmium connection"  */
extern const char BTGAP_148[];
/*  "[GAP] connection indication buffer or reconnect buffer already used"  */
extern const char BTGAP_149[];
/*  "[GAP] bt_gap_le_connect: BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_150[];
/*  "[GAP] bt_gap_le_cancel_connection"  */
extern const char BTGAP_151[];
/*  "[GAP] bt_gap_le_disconnect"  */
extern const char BTGAP_152[];
/*  "[GAP] bt_gap_le_disconnect: connection handle is not existed"  */
extern const char BTGAP_153[];
/*  "[GAP] bt_gap_le_connection_update_proc: timer_id(0x%08x)"  */
extern const char BTGAP_154[];
/*  "[GAP] [L2CAP]connection update timer 0x%08x expired"  */
extern const char BTGAP_155[];
/*  "[GAP] connection update timer 0x%08x expired"  */
extern const char BTGAP_156[];
/*  "[GAP] BT_L2CAP_CONN_UPDATE_REQ_IND Success"  */
extern const char BTGAP_157[];
/*  "[GAP] BT_L2CAP_CONN_UPDATE_REQ_IND Failed"  */
extern const char BTGAP_158[];
/*  "[GAP] APP accept update connection parameters"  */
extern const char BTGAP_159[];
/*  "[GAP] bt_gap_le_update_connection_parameter: handle(0x%04x)"  */
extern const char BTGAP_160[];
/*  "[GAP] Can't find the connection by the connection handle:0x%04x"  */
extern const char BTGAP_161[];
/*  "[GAP] I am Master"  */
extern const char BTGAP_162[];
/*  "[GAP] I am Slave"  */
extern const char BTGAP_163[];
/*  "[GAP] bt_gap_le_read_rssi_proc: timer_id(0x%08x)"  */
extern const char BTGAP_164[];
/*  "[GAP] read rssi timer 0x%08x expired"  */
extern const char BTGAP_165[];
/*  "[GAP] bt_gap_le_read_rssi"  */
extern const char BTGAP_166[];
/*  "[GAP] bt_gap_le_update_data_length_proc: timer_id(0x%08x)"  */
extern const char BTGAP_167[];
/*  "[GAP] update data length timer 0x%08x expired"  */
extern const char BTGAP_168[];
/*  "[GAP] bt_gap_le_update_data_length"  */
extern const char BTGAP_169[];
/*  "[GAP] bt_gap_le_set_tx_power_proc: timer_id(0x%08x)"  */
extern const char BTGAP_170[];
/*  "[GAP] set tx power timer 0x%08x expired"  */
extern const char BTGAP_171[];
/*  "[GAP] bt_gap_le_set_tx_power"  */
extern const char BTGAP_172[];
/*  "[GAP] bt_gap_le_read_remote_used_features_proc: timer_id(0x%08x)"  */
extern const char BTGAP_173[];
/*  "[GAP] read remote used features timer 0x%08x expired"  */
extern const char BTGAP_174[];
/*  "[GAP] bt_gap_le_read_remote_used_features_proc error: disconn handle(0x%04x), status %x"  */
extern const char BTGAP_175[];
/*  "[GAP] bt_gap_le_read_remote_used_features"  */
extern const char BTGAP_176[];
/*  "[GAP] bt_gap_le_phy_proc: timer_id(0x%08x)"  */
extern const char BTGAP_177[];
/*  "[GAP] phy timer 0x%08x expired"  */
extern const char BTGAP_178[];
/*  "[GAP] bt_gap_le_get_phy: handle(0x%04x)"  */
extern const char BTGAP_179[];
/*  "[GAP] Can't find conn by handle(0x%04x)"  */
extern const char BTGAP_180[];
/*  "[GAP] bt_gap_le_set_phy: handle(0x%04x)"  */
extern const char BTGAP_181[];
/*  "[GAP] Can't find conn by handle(0x%04x)"  */
extern const char BTGAP_182[];
/*  "[GAP] BT_GAP_LE_CONNECT_IND: Out of Memory"  */
extern const char BTGAP_183[];
/*  "[GAP] bt_gap_le_event_callback: timer_id(0x%08x)"  */
extern const char BTGAP_184[];
/*  "[GAP] timer 0x%08x expired"  */
extern const char BTGAP_185[];
/*  "[GAP] BT_HCI_SUBEVT_LE_ADVERTISING_REPORT size error"  */
extern const char BTGAP_186[];
/*  "[GAP] Invalid advertising event type (0x%04x)"  */
extern const char BTGAP_187[];
/*  "[GAP] Invalid compatible scan type (0x%02x)"  */
extern const char BTGAP_188[];
/*  "[GAP] BT_HCI_SUBEVT_LE_CONNECTION_COMPLETE status(0x%04x)"  */
extern const char BTGAP_189[];
/*  "[GAP] BT_HCI_SUBEVT_LE_CONNECTION_COMPLETE: Out of Memory"  */
extern const char BTGAP_190[];
/*  "[GAP] security_mod(0x%08x)"  */
extern const char BTGAP_191[];
/*  "[GAP] LE Connection Established: handle(0x%04x)"  */
extern const char BTGAP_192[];
/*  "[GAP] BT_HCI_EVT_DISCONNECTION_COMPLETE: handle(0x%04x)"  */
extern const char BTGAP_193[];
/*  "[GAP] bt_timer_cancel_and_callback status: 0x%04x"  */
extern const char BTGAP_194[];
/*  "[GAP] LE 0x3E Connection Failed: handle(0x%04x), reconnect"  */
extern const char BTGAP_195[];
/*  "[GAP] LE Connection Disconnected: handle(0x%04x)"  */
extern const char BTGAP_196[];
/*  "[GAP] BT_HCI_SUBEVT_LE_LONG_TERM_KEY_REQUEST: handle(0x%04x)"  */
extern const char BTGAP_197[];
/*  "[GAP] do not have LTK, reply NAK"  */
extern const char BTGAP_198[];
/*  "[GAP] BT_HCI_EVT_ENCRYPTION_CHANGE: handle(0x%04x)"  */
extern const char BTGAP_199[];
/*  "[GAP] bt_gap_le_set_periodic_advertising_proc: timer_id(0x%08x)"  */
extern const char BTGAP_200[];
/*  "[GAP] set periodic advertising config timer 0x%08x expired"  */
extern const char BTGAP_201[];
/*  "[GAP] bt_gap_le_set_periodic_advertiser_list"  */
extern const char BTGAP_202[];
/*  "[GAP] bt_gap_le_periodic_advertising_create_sync"  */
extern const char BTGAP_203[];
/*  "[GAP] bt_gap_le_periodic_advertising_create_sync_cancel"  */
extern const char BTGAP_204[];
/*  "[GAP] bt_gap_le_periodic_advertising_terminate_sync sync handle : %04X"  */
extern const char BTGAP_205[];
/*  "[GAP] bt_gap_le_set_extended_scan_proc: timer_id(0x%08x), data(0x%08x), status(0x%08x) next_node(%p)"  */
extern const char BTGAP_206[];
/*  "[GAP] set extended scan timer 0x%08x expired"  */
extern const char BTGAP_207[];
/*  "[GAP] bt_gap_le_set_extended_scan_proc error data(%d)"  */
extern const char BTGAP_208[];
/*  "[GAP] bt_gap_le_set_extended_scan_internal compatible type(%d) params(%p), enable(%p)"  */
extern const char BTGAP_209[];
/*  "[GAP] bt_gap_le_set_extended_scan_internal [params] and [enable] can't be all null"  */
extern const char BTGAP_210[];
/*  "[GAP] bt_gap_le_set_extended_scan_internal params_phy_1M can't be null"  */
extern const char BTGAP_211[];
/*  "[GAP] bt_gap_le_set_extended_scan_internal params_phy_coded can't be null"  */
extern const char BTGAP_212[];
/*  "[GAP] bt_gap_le_set_extended_scan_internal : BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_213[];
/*  "[GAP] bt_gap_le_set_extended_scan_internal fail status 0x%x"  */
extern const char BTGAP_214[];
/*  "[GAP] connection is already existed"  */
extern const char BTGAP_215[];
/*  "[GAP] reach maxmium connection"  */
extern const char BTGAP_216[];
/*  "[GAP] connection indication buffer or reconnect buffer already used"  */
extern const char BTGAP_217[];
/*  "[GAP] bt_gap_le_extended_connect: BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_218[];
/*  "[GAP] bt_gap_le_set_extended_scan"  */
extern const char BTGAP_219[];
/*  "[GAP] bt_gap_le_extended_connect"  */
extern const char BTGAP_220[];
/*  "[GAP] bt_gap_le_set_random_address_proc: timer_id(0x%08x)"  */
extern const char BTGAP_221[];
/*  "[GAP] set random address timer 0x%08x expired"  */
extern const char BTGAP_222[];
/*  "[GAP] bt_gap_le_set_random_address"  */
extern const char BTGAP_223[];
/*  "[GAP] bt_gap_le_set_advertising_single_proc: timer_id(0x%08x), enable(0x%04x), status(0x%02x)"  */
extern const char BTGAP_224[];
/*  "[GAP] set advertising timer 0x%08x expired"  */
extern const char BTGAP_225[];
/*  "[GAP] bt_gap_le_set_advertising_single"  */
extern const char BTGAP_226[];
/*  "[GAP] bt_gap_le_set_advertising_single: BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_227[];
/*  "[GAP] bt_gap_le_set_advertising"  */
extern const char BTGAP_228[];
/*  "[GAP] stop multi advertising timer 0x%08x expired"  */
extern const char BTGAP_229[];
/*  "[GAP] start multi advertising timer 0x%08x expired"  */
extern const char BTGAP_230[];
/*  "[GAP] start multi advertising(%d) finished %x"  */
extern const char BTGAP_231[];
/*  "[GAP] bt_gap_le_start_multi_advertising_proc: timer_id(0x%08x), next_cmd(0x%04x)"  */
extern const char BTGAP_232[];
/*  "[GAP] start multi advertising(%d) stoped %x"  */
extern const char BTGAP_233[];
/*  "[GAP] bt_gap_le_stop_multiple_advertising_int(%d)"  */
extern const char BTGAP_234[];
/*  "[GAP] bt_gap_le_start_multiple_advertising_int(%d)"  */
extern const char BTGAP_235[];
/*  "[GAP] bt_gap_le_start_multiple_advertising_int: BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_236[];
/*  "[GAP] bt_gap_le_allocate_adv_set reserve %d reserve handle %d"  */
extern const char BTGAP_237[];
/*  "[GAP] bt_gap_le_allocate_adv_set error reserve handle(%d) used"  */
extern const char BTGAP_238[];
/*  "[GAP] bt_gap_le_allocate_adv_set allocate handle 0x%02x"  */
extern const char BTGAP_239[];
/*  "[GAP] bt_gap_le_allocate_adv_set OOM"  */
extern const char BTGAP_240[];
/*  "[GAP] bt_gap_le_free_adv_set free handle 0x%02x"  */
extern const char BTGAP_241[];
/*  "[GAP] bt_gap_le_clear_adv_set"  */
extern const char BTGAP_242[];
/*  "[GAP] bt_gap_le_clear_adv_set clear handle %d"  */
extern const char BTGAP_243[];
/*  "[GAP] bt_gap_le_format_paramters_to_list fail unknown op code : %04X to format"  */
extern const char BTGAP_244[];
/*  "[GAP] bt_gap_le_format_paramters_to_list OOM, op code : %04X"  */
extern const char BTGAP_245[];
/*  "[GAP] bt_gap_le_extended_advtersing_event_proc: timer_id(0x%08x), data(0x%08x)"  */
extern const char BTGAP_246[];
/*  "[GAP] Extended adv event timer 0x%08x expired"  */
extern const char BTGAP_247[];
/*  "[GAP] Remove adv set cnf status 0x%x"  */
extern const char BTGAP_248[];
/*  "[GAP] bt_gap_le_enable_extended_advertising_proc error data(0x%08x)"  */
extern const char BTGAP_249[];
/*  "[GAP] Clear adv set cnf status 0x%x"  */
extern const char BTGAP_250[];
/*  "[GAP] Enable periodic adv cnf status 0x%x"  */
extern const char BTGAP_251[];
/*  "[GAP] Enable extended adv cnf status 0x%x"  */
extern const char BTGAP_252[];
/*  "[GAP] bt_gap_le_enable_extended_advertising_proc error data(0x%08x)"  */
extern const char BTGAP_253[];
/*  "[GAP] adv set terminate status:%02X, handle:0x%02X, conn_handle:0x%04X, num:%d"  */
extern const char BTGAP_254[];
/*  "[GAP] local ll addr type %d, local ll addr: %02X:%02X:%02X:%02X:%02X:%02X"  */
extern const char BTGAP_255[];
/*  "[GAP] scan request adv handle 0x%02X"  */
extern const char BTGAP_256[];
/*  "[GAP] bt_gap_le_enable_extended_advertising_internal handle(%d), enable_p(%p)"  */
extern const char BTGAP_257[];
/*  "[GAP] bt_gap_le_enable_extended_advertising_internal param [enable] can't be null"  */
extern const char BTGAP_258[];
/*  "[GAP] bt_gap_le_enable_extended_advertising_internal can't find by handle(%d)"  */
extern const char BTGAP_259[];
/*  "[GAP] bt_gap_le_enable_extended_advertising_internal busy"  */
extern const char BTGAP_260[];
/*  "[GAP] bt_gap_le_config_extended_advertising_proc: timer_id(0x%08x), data(0x%08x), status(0x%08x) next_node(%p)"  */
extern const char BTGAP_261[];
/*  "[GAP] Config extended advertising timer 0x%08x expired"  */
extern const char BTGAP_262[];
/*  "[GAP] bt_gap_le_config_extended_advertising_proc: data(0x%08x)"  */
extern const char BTGAP_263[];
/*  "[GAP] maximum advtertising data length (%d)"  */
extern const char BTGAP_264[];
/*  "[GAP] bt_gap_le_config_extended_advertising_internal handle(%d), random_addr(%p), param(%p), data(%p), scan_rsp(%p)"  */
extern const char BTGAP_265[];
/*  "[GAP] bt_gap_le_config_extended_advertising_internal : BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_266[];
/*  "[GAP] bt_gap_le_config_extended_advertising_internal allocate fail"  */
extern const char BTGAP_267[];
/*  "[GAP] bt_gap_le_config_extended_advertising_internal param can't be null"  */
extern const char BTGAP_268[];
/*  "[GAP] bt_gap_le_config_extended_advertising_internal reach maxmium adv set"  */
extern const char BTGAP_269[];
/*  "[GAP] bt_gap_le_config_extended_advertising_internal fail status 0x%x"  */
extern const char BTGAP_270[];
/*  "[GAP] bt_gap_le_config_periodic_advertising_proc: timer_id(0x%08x), data(0x%08x), status(0x%08x) next_node(%p)"  */
extern const char BTGAP_271[];
/*  "[GAP] Config periodic advertising timer 0x%08x expired"  */
extern const char BTGAP_272[];
/*  "[GAP] bt_gap_le_get_max_adv_length"  */
extern const char BTGAP_273[];
/*  "[GAP] bt_gap_le_enable_extended_advertising"  */
extern const char BTGAP_274[];
/*  "[GAP] bt_gap_le_enable_extended_advertising handle need in [0x%08X~0x%08X]"  */
extern const char BTGAP_275[];
/*  "[GAP] bt_gap_le_config_extended_advertising"  */
extern const char BTGAP_276[];
/*  "[GAP] bt_gap_le_config_extended_advertising handle need in [0x%08X~0x%08X]"  */
extern const char BTGAP_277[];
/*  "[GAP] bt_gap_le_remove_extended_advertising clear all(%d) handle(%d)"  */
extern const char BTGAP_278[];
/*  "[GAP] bt_gap_le_remove_extended_advertising handle need in [0x%08X~0x%08X]"  */
extern const char BTGAP_279[];
/*  "[GAP] bt_gap_le_enable_periodic_advertising handle(%d), enable(%d)"  */
extern const char BTGAP_280[];
/*  "[GAP] bt_gap_le_enable_periodic_advertising handle need in [0x%08X~0x%08X]"  */
extern const char BTGAP_281[];
/*  "[GAP] bt_gap_le_enable_periodic_advertising can't find by handle(%d)"  */
extern const char BTGAP_282[];
/*  "[GAP] bt_gap_le_enable_periodic_advertising busy"  */
extern const char BTGAP_283[];
/*  "[GAP] bt_gap_le_config_periodic_advertising handle(%d)"  */
extern const char BTGAP_284[];
/*  "[GAP] bt_gap_le_config_periodic_advertising handle need in [0x%08X~0x%08X]"  */
extern const char BTGAP_285[];
/*  "[GAP] bt_gap_le_config_periodic_advertising params and data can't be all null"  */
extern const char BTGAP_286[];
/*  "[GAP] bt_gap_le_config_periodic_advertising data length exceed"  */
extern const char BTGAP_287[];
/*  "[GAP] bt_gap_le_config_periodic_advertising : BT_STATUS_BUFFER_USED"  */
extern const char BTGAP_288[];
/*  "[GAP] bt_gap_le_config_periodic_advertising : Unknown adv set"  */
extern const char BTGAP_289[];
/*  "[GAP] bt_gap_le_config_periodic_advertising fail status 0x%x"  */
extern const char BTGAP_290[];
/*  "[GAP] ERROR: multi-adv params instance is wrong: instance %d, max_instance %d"  */
extern const char BTGAP_291[];
/*  "[GAP] ERROR: multi-adv params instance is wrong: instance %d, max_instance %d, adv_type %x"  */
extern const char BTGAP_292[];
/*  "[GAP] ERROR: multi-adv do not support the connectable adv_type %x"  */
extern const char BTGAP_293[];
/*  "[GAP] [MISC] bt_gap_inquiry(during: %d, count: %d)"  */
extern const char BTGAP_294[];
/*  "[GAP] [MISC] bt_gap_cancel_inquiry()"  */
extern const char BTGAP_295[];
/*  "[GAP] [MISC] bt_gap_write_stored_link_key(%d)"  */
extern const char BTGAP_296[];
/*  "[GAP] [MISC] bt_gap_write_stored_link_key OOM"  */
extern const char BTGAP_297[];
/*  "[GAP] [MISC] bt_gap_delete_stored_link_key(%d)"  */
extern const char BTGAP_298[];
/*  "[GAP] [MISC] bt_gap_set_scan_mode(%d)"  */
extern const char BTGAP_299[];
/*  "[GAP] [MISC] bt_gap_set_role() on handle %x"  */
extern const char BTGAP_300[];
/*  "[GAP] [MISC] bt_gap_read_remote_name()"  */
extern const char BTGAP_301[];
/*  "[GAP] [MISC] bt_gap_enter_test_mode()"  */
extern const char BTGAP_302[];
/*  "[GAP] [MISC] bt_gap_set_extended_inquiry_response()"  */
extern const char BTGAP_303[];
/*  "[GAP] [MISC] bt_gap_set_extended_inquiry_response OOM"  */
extern const char BTGAP_304[];
/*  "[GAP] [MISC] bt_gap_read_rssi() fail not find the connection by handle %x"  */
extern const char BTGAP_305[];
/*  "[GAP] [MISC] bt_gap_read_rssi() on handle %x"  */
extern const char BTGAP_306[];
/*  "[GAP] [MISC] bt_gap_read_raw_rssi() fail not find the connection by handle %x"  */
extern const char BTGAP_307[];
/*  "[GAP] [MISC] bt_gap_read_raw_rssi() on handle %x"  */
extern const char BTGAP_308[];
/*  "[GAP] [MISC] bt_gap_read_inquiry_response_tx()"  */
extern const char BTGAP_309[];
/*  "[GAP] [MISC] bt_gap_write_inquiry_tx(%d)"  */
extern const char BTGAP_310[];
/*  "[GAP] [MISC] bt_gap_write_page_scan_activity(%x, %x)"  */
extern const char BTGAP_311[];
/*  "[GAP] [MISC] bt_gap_write_inquiry_scan_activity(%x, %x)"  */
extern const char BTGAP_312[];
/*  "[GAP] [MISC] bt_gap_write_page_timeout(%x)"  */
extern const char BTGAP_313[];
/*  "[GAP] [MISC] bt_gap_write_supervision_timeout() on handle %x, role %x, role check %x"  */
extern const char BTGAP_314[];
/*  "[GAP] [MISC] bt_gap_cancel_name_request()"  */
extern const char BTGAP_315[];
/*  "[GAP] bt_gap_set_public_address"  */
extern const char BTGAP_316[];
/*  "[GAP] bt_gap_enable_sniff_subrating(%d)"  */
extern const char BTGAP_317[];
/*  "[GAP] bt_gap_enable_sniff_subrating: sniff feature is disabled by bt_get_feature_mask_configuration()"  */
extern const char BTGAP_318[];
/*  "[GAP] [MISC] bt_gap_write_inquiry_access_code_num(%x)"  */
extern const char BTGAP_319[];
/*  "[GAP] [MISC] bt_gap_inquiry_extend(duration: %d, count: %d)"  */
extern const char BTGAP_320[];
/*  "[GAP] [MISC] bt_gap_read_remote_version_information() fail, cant find the handle 0x%x"  */
extern const char BTGAP_321[];
/*  "[GAP] [MISC] bt_gap_read_remote_version_information() on handle %x"  */
extern const char BTGAP_322[];
/*  "[GAP] timer 0x%08x expired"  */
extern const char BTGAP_323[];
/*  "[GAP] [MISC] bt_gap_misc_callback() timer_id %x, error %x"  */
extern const char BTGAP_324[];
/*  "[GAP] dut flag = %d"  */
extern const char BTGAP_325[];
/*  "[GAP] BT_HCI_EVT_ROLE_CHANGE connection 0x%x, conn_status 0x%x, new_role = 0x%x, conn_role 0x%x"  */
extern const char BTGAP_326[];
/*  "[GAP] bt_gap_compose_rho_data(), no sp connection!!"  */
extern const char BTGAP_327[];
/*  "[GAP] bt_gap_compose_rho_data(), key_type 0x%x, l2cap_channel_identifier 0x%x, l2cap_signal_identifier 0x%x, status 0x%x, sniff_lock_count 0x%x, disable sniff count %d"  */
extern const char BTGAP_328[];
/*  "[GAP] bt_gap_decompose_rho_data(), key_type 0x%x, l2cap_channel_identifier 0x%x, l2cap_signal_identifier 0x%x, status 0x%x, sniff_lock_count 0x%x, disable sniff count %d"  */
extern const char BTGAP_329[];
/*  "[GAP] [RHO] bt_gap_is_rho_allowed() rx_pending 0x%x, pending_op 0x%x, sdpc_channel 0x%x"  */
extern const char BTGAP_330[];
/*  "[GAP] [RHO] bt_gap_is_rho_allowed() %x"  */
extern const char BTGAP_331[];
/*  "[GAP] [RHO] bt_gap_is_rho_allowed() bonding_conn 0x%x, pending_op.conn 0x%x, buffer 0x%x"  */
extern const char BTGAP_332[];
/*  "[GAP] [RHO] bt_gap_rho_get_data_length() len %x"  */
extern const char BTGAP_333[];
/*  "[GAP] [RHO] bt_gap_rho_update_context() role %x, addr: %02x-%02x-%02x-%02x-%02x-%02x"  */
extern const char BTGAP_334[];
/*  "[GAP] [RHO]bt_gap_rho_update_context: update timer_id/data(0x%08x/0x%08x), new time_id/data(0x%08x/0x%08x)"  */
extern const char BTGAP_335[];
/*  "[GAP] no sp connection!!"  */
extern const char BTGAP_336[];
/*  "[GAP] release before backup, wrong state!!"  */
extern const char BTGAP_337[];
/*  "[GAP] Can't find conn by terminate conn handle" */
extern const char BTGAP_338[];
/*  "[GAP] Power reset type %d, current power reset state %d" */
extern const char BTGAP_339[];
/*  "[GAP] Set local public address, current power state %d" */
extern const char BTGAP_340[];
/*  "[GAP] conection doesn't exist, cmd/event/timer id 0x%x" */
extern const char BTGAP_341[];
/*  "[GAP] wrtie link policy disable sniff mode count %d" */
extern const char BTGAP_342[];
/*  "[GAP] bt_gap_write_link_policy: parameter is error" */
extern const char BTGAP_343[];
/*  "[GAP] bt_gap_write_link_policy: connection handle %x is not existed" */
extern const char BTGAP_344[];
/*  "[GAP] bt_gap_get_tx_power_config: get user tx power config status = %d" */
extern const char BTGAP_345[];
/*  "[GAP] bt_config_tx_power_level_by_version: config tx power by version status = %d" */
extern const char BTGAP_346[];
/*  "[GAP] bt_gap_read_encryption_key_size handle:%x" */
extern const char BTGAP_347[];
/*  "[GAP] bt_gap_le_rho_update_context() role %x, addr: %02x-%02x-%02x-%02x-%02x-%02x"*/
extern const char BTGAP_349[];
/*  "[GAP] bt_gap_le_rho_get_data_length() len: %d"*/
extern const char BTGAP_350[];
/*  "[GAP] bt_gap_le_rho_get_data() le count: %d"*/
extern const char BTGAP_351[];
/*  "bt_gap_le_rho_compose_rho_data() addr: %02x-%02x-%02x-%02x-%02x-%02x"*/
extern const char BTGAP_352[];
/*  "bt_gap_le_rho_decompose_rho_data() addr: %02x-%02x-%02x-%02x-%02x-%02x"*/
extern const char BTGAP_353[];
/*  "bt_gap_le_get_context_handles() num: %d"*/
extern const char BTGAP_354[];
/*  "bt_gap_le_get_context_handles() num: %d, count: %d"*/
extern const char BTGAP_355[];
/*  "[GAP] bt_gap_read_remote_ext_feature() can't find handle:%x" */
extern const char BTGAP_356[];
/*  "[GAP] bt_gap_read_remote_ext_feature handle:%x, page_num: %d" */
extern const char BTGAP_357[];
/*  "[GAP] [GAP] BT_HCI_EVT_READ_REMOTE_EXT_FEAT_COMPLETE handle:%x, page_num: %d,max_page: %d,feature: %x %x %x %x %x %x %x %x" */
extern const char BTGAP_358[];
/*  "[GAP] remote device support security conn" */
extern const char BTGAP_359[];
/*  "[GAP] remote device ever support security conn, but now don't supprot, disconnect" */
extern const char BTGAP_360[];
/*  "[GAP] [CTKD] bt_gap_ctkd_notify,type: %d, ct2:%d " */
extern const char BTGAP_361[];
/*  "[GAP] [CTKD] bt_gap_ctkd_timeout_callback" */
extern const char BTGAP_362[];
/*  "[GAP] [CTKD] bt_gap_ctkd_link_key_to_ltk ct2: %d" */
extern const char BTGAP_363[];
/*  "[GAP] [CTKD] bt_gap_ctkd_ltk_to_link_key ct2: %d" */
extern const char BTGAP_364[];
/*  "[GAP] [CTKD] bt_gap_ctkd_aes_cmac" */
extern const char BTGAP_365[];
/*  "[GAP] [CTKD] print Key %2d - %2d: 0x%2x %2x %2x %2x " */
extern const char BTGAP_366[];
/*  "[GAP] [GAP] tx_power_info = %x, bt_power_status = %d, bt_init_tx_power_level %d > bt_max_tx_power_level %d\n" */
extern const char BTGAP_367[];
/*  "[GAP] bt_init_tx_power_level %d, bt_max_tx_power_level %d, le_init_tx_power_level %d, fixed_tx_power_level %d" */
extern const char BTGAP_368[];
/*  "[GAP] bt_tx_power_level_offset = %x should be between 0 and 3" */
extern const char BTGAP_369[];
/*  "[GAP] bdr_ble_tx_power_level_offset = %x, bdr_ble_fine_tx_power_level_offset = %x, edr_fine_tx_power_level_offset = %x, edr_tx_power_level_offset = %x should be between 0 and 3" */
extern const char BTGAP_370[];
/*  "[GAP] bdr_init_tx_power_level %d, bt_max_tx_power_level %d, le_init_tx_power_level %d, fixed_tx_power_level %d, edr_init_tx_power_level %d" */
extern const char BTGAP_371[];
/*  "[GAP] bdr_tx_power_level_offset = %x, bdr_fine_tx_power_level_offset = %x, edr_fine_tx_power_level_offset = %x, edr_tx_power_level_offset = %x, ble_fine_tx_power_level_offset = %x, ble_tx_power_level_offset = %x should be between 0 and 3" */
extern const char BTGAP_372[];
/*  "[GAP] bt_power_status = %d, bt_init_tx_power_level %d > bt_max_tx_power_level %d" */
extern const char BTGAP_373[];
/*  "[GAP] bt_gap_le_request_peer_sca: handle: %d" */
extern const char BTGAP_374[];
/*  "[GAP] bt_gap_qos_setup() failed,cant find the handle 0x%x" */
extern const char BTGAP_375[];
/*  "[GAP] bt_gap_qos_setup() handle 0x%x" */
extern const char BTGAP_376[];
/*  "[GAP] start %d, drv_diff %d(a%d,b:%d, c:%d), before_reset %d, after reset %d, before_profile %d, before notify_user %d" */
extern const char BTGAP_377[];
/*  "[GAP] bt_gap_le_iso_connection_proc: timer_id(0x%08x)" */
extern const char BTGAP_400[];
/*  "[GAP] bt_gap_le_iso_connection_proc timer 0x%08x expired" */
extern const char BTGAP_401[];
/*  "[GAP] bt_gap_le_iso_data_path_proc: timer_id(0x%08x)" */
extern const char BTGAP_402[];
/*  "[GAP] bt_gap_le_iso_data_path_proc timer 0x%08x expired" */
extern const char BTGAP_403[];
/*  "[GAP] bt_gap_le_reply_cis_request: %d" */
extern const char BTGAP_404[];
/*  "[GAP] bt_gap_le_setup_iso_data_path: handle 0x%x" */
extern const char BTGAP_405[];
/*  "[GAP] bt_gap_le_remove_iso_data_path: handle 0x%x" */
extern const char BTGAP_406[];
/*  "[GAP] bt_gap_le_set_cig_parameters" */
extern const char BTGAP_407[];
/*  "[GAP] bt_gap_le_create_cis" */
extern const char BTGAP_408[];
/*  "[GAP] bt_gap_le_set_cig_params_proc: timer_id(0x%08x)" */
extern const char BTGAP_409[];
/*  "[GAP] bt_gap_le_set_cig_params_proc timer 0x%08x expired" */
extern const char BTGAP_410[];
/*  "[GAP] BT_HCI_SUBEVT_LE_CIS_ESTABLISHED status(0x%04x)" */
extern const char BTGAP_411[];
/*  "[GAP] BT_HCI_SUBEVT_LE_CIS_ESTABLISHED: Out of Memory" */
extern const char BTGAP_412[];
/*  "[GAP] bt_gap_le_create_big" */
extern const char BTGAP_413[];
/*  "[GAP] bt_gap_le_terminate_big" */
extern const char BTGAP_414[];
/*  "[GAP] bt_gap_le_big_create_sync" */
extern const char BTGAP_415[];
/*  "[GAP] bt_gap_le_big_terminate_sync" */
extern const char BTGAP_416[];
/*  "[GAP] BT_HCI_SUBEVT_LE_CREATE_BIG_COMPLETE status(0x%04x)" */
extern const char BTGAP_417[];
/*  "[GAP] BT_HCI_SUBEVT_LE_TERMINATE_BIG_COMPLETE status(0x%04x)" */
extern const char BTGAP_418[];
/*  "[GAP] BT_HCI_SUBEVT_LE_BIG_SYNC_ESTABLISHED status(0x%04x)" */
extern const char BTGAP_419[];
/*  "[GAP] BT_HCI_SUBEVT_LE_BIG_SYNC_LOST status(0x%04x)" */
extern const char BTGAP_420[];
/*  "[GAP] bt_gap_le_big_proc: timer_id(0x%08x)" */
extern const char BTGAP_421[];
/*  "[GAP] bt_gap_le_big_proc timer 0x%08x expired" */
extern const char BTGAP_422[];
/*  "[GAP] bt_gap_le_set_cig_parameters_test" */
extern const char BTGAP_423[];
/*  "[GAP] bt_gap_le_periodic_advertising_set_receive_enable" */
extern const char BTGAP_424[];
/*  "[GAP] bt_gap_le_periodic_advertising_sync_transfer" */
extern const char BTGAP_425[];
/*  "[GAP] bt_gap_le_periodic_advertising_set_info_transfer" */
extern const char BTGAP_426[];
/*  "[GAP] bt_gap_le_set_periodic_advertising_sync_transfer_parameters" */
extern const char BTGAP_427[];
/*  "[GAP] bt_gap_le_set_past_proc: timer_id(0x%08x)" */
extern const char BTGAP_428[];
/*  "[GAP] Set periodic advertising sync transfer timer 0x%08x expired" */
extern const char BTGAP_429[];
/*  "[GAP] bt_gap_sniff_lock   lock_count: %d, conn: 0x%x" */
extern const char BTGAP_432[];
/*  "[GAP] bt_gap_sniff_unlock lock_count: %d, conn: 0x%x" */
extern const char BTGAP_433[];
/*  "[GAP] le cmd parse opcode = %02x, handle = %02x" */
extern const char BTGAP_434[];
/*  "[GAP] set privacy mode = %04x ,peer identity address = %12x ,address type = %04x" */
extern const char BTGAP_435[];


/*  "[GATT] is gatt over edr: %d"  */
extern const char BTGATT_007[];
/*  "[GATT] SDP attr OOM"  */
extern const char BTGATT_008[];
/*  "[GATT] SDP query error: 0x%04x"  */
extern const char BTGATT_009[];
/*  "[GATT] (bt_gatt_free_channel)channel:0x%08x"  */
extern const char BTGATT_010[];
/*  "[GATT] bt_gatt_l2cap_context_op_callback, event %d"  */
extern const char BTGATT_011[];
/*  "[GATT] bt_gatt_l2cap_context_op_callback, allocate gatt channel %x"  */
extern const char BTGATT_012[];
/*  "[GATT] bt_gatt_l2cap_context_op_callback, allocate gatt channel OOM"  */
extern const char BTGATT_013[];
/*  "[GATT] bt_gatt_l2cap_context_op_callback, free GATT channel %x"  */
extern const char BTGATT_014[];
/*  "[GATT] (bt_gatt_l2cap_callback)l2cap channel:0x%08x,event:%d"  */
extern const char BTGATT_015[];
/*  "[GATT] bt_gatt_l2cap_callback, allocate L2CAP conn rsp OOM"  */
extern const char BTGATT_016[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_017[];
/*  "[GATT] Unhandle opcode [%x] in bt_gatt_timeout_callback.\n"  */
extern const char BTGATT_018[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_019[];
/*  "[GATT] Exchange MTU [%d] fail. the value should 23<=max mtu<=512."  */
extern const char BTGATT_020[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_021[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_022[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_023[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_024[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_025[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_026[];
/*  "[GATT] bt_gattc_read_charc: bt-connection:0x%08x"  */
extern const char BTGATT_027[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_028[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_029[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_030[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_031[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_032[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_033[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_034[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_035[];
/*  "[GATT] connection not found, handle = 0x%08x"  */
extern const char BTGATT_036[];
/*  "[GATT] Set GATT MAX MTU [%d] success"  */
extern const char BTGATT_037[];
/*  "[GATT] Set GATT MAX MTU [%d] fail. the value should 23<=max mtu<=512."  */
extern const char BTGATT_038[];
/*  "[GATT] GATTS DB Definition error: Can not support write without callback help.\n"  */
extern const char BTGATT_039[];
/*  "[GATT] bonding key size=%d service required key size=%d"  */
extern const char BTGATT_040[];
/*  "[GATT] [Read] handle[0x%04x] is not allowed, reason[Insufficient authorization]"  */
extern const char BTGATT_041[];
/*  "[GATT] [Read] handle[0x%04x] is not allowed, reason[Insufficient authentication]"  */
extern const char BTGATT_042[];
/*  "[GATT] [Read] handle[0x%04x] is not allowed, reason[Insufficient encryption]"  */
extern const char BTGATT_043[];
/*  "[GATT] [Read] handle[0x%04x] is not allowed, reason[Write not permitted]"  */
extern const char BTGATT_044[];
/*  "[GATT] [Read] handle[0x%04x] is not allowed, reason[Read not permitted]"  */
extern const char BTGATT_045[];
/*  "[GATT] [Read] handle[0x%04x] is not allowed, reason[unknow]"  */
extern const char BTGATT_046[];
/*  "[GATT] [Write] handle[0x%04x] is not allowed, reason[Insufficient authorization]"  */
extern const char BTGATT_047[];
/*  "[GATT] [Write] handle[0x%04x] is not allowed, reason[Insufficient authentication]"  */
extern const char BTGATT_048[];
/*  "[GATT] [Write] handle[0x%04x] is not allowed, reason[Insufficient encryption]"  */
extern const char BTGATT_049[];
/*  "[GATT] [Write] handle[0x%04x] is not allowed, reason[Write not permitted]"  */
extern const char BTGATT_050[];
/*  "[GATT] [Write] handle[0x%04x] is not allowed, reason[Read not permitted]"  */
extern const char BTGATT_051[];
/*  "[GATT] [Write] handle[0x%04x] is not allowed, reason[unknow]"  */
extern const char BTGATT_052[];
/*  "[GATT] Read by group Req opcode[%x] Start[%x] End[%x]\n"  */
extern const char BTGATT_053[];
/*  "[GATT] [READ_BY_TYPE] invalid handle[0x%04x]"  */
extern const char BTGATT_054[];
/*  "[GATT] [READ_BY_GROUP_TYPE] invalid handle[0x%04x]"  */
extern const char BTGATT_055[];
/*  "[GATT] Read by group Rsp Handle[%x]\n"  */
extern const char BTGATT_056[];
/*  "[GATT] Read by group Rsp Ending Handle[%x]\n"  */
extern const char BTGATT_057[];
/*  "[GATT] [READ_BY_TYPE] Attribute not found"  */
extern const char BTGATT_058[];
/*  "[GATT] [READ_BY_GROUP_TYPE] Attribute not found"  */
extern const char BTGATT_059[];
/*  "[GATT] [READ_BY_TYPE] Application error(MTU size is too small."  */
extern const char BTGATT_060[];
/*  "[GATT] [READ_BY_GROUP_TYPE] Application error(MTU size is too small."  */
extern const char BTGATT_061[];
/*  "[GATT] Read by group Rsp len[%d]:\n"  */
extern const char BTGATT_062[];
/*  "[GATT] Find info Req opcode[%x] Start[%x] End[%x]\n"  */
extern const char BTGATT_063[];
/*  "[GATT] [FIND_INFO] invalid handle[0x%04x]"  */
extern const char BTGATT_064[];
/*  "[GATT] Find info Rsp Handle[%x]\n"  */
extern const char BTGATT_065[];
/*  "[GATT] [FIND_INFO] Attribute not found"  */
extern const char BTGATT_066[];
/*  "[GATT] Find info Rsp len[%d]:\n"  */
extern const char BTGATT_067[];
/*  "[GATT] [READ] invalid handle[0x%04x]"  */
extern const char BTGATT_068[];
/*  "[GATT] [READ_BLOB] invalid handle[0x%04x]"  */
extern const char BTGATT_069[];
/*  "[GATT] [READ] handle[0x%04x] Invalid offset"  */
extern const char BTGATT_070[];
/*  "[GATT] [READ_BLOB] handle[0x%04x] Invalid offset"  */
extern const char BTGATT_071[];
/*  "[GATT] [READ_MULTIPLE] invalid handle[0x%04x]"  */
extern const char BTGATT_072[];
/*  "[GATT] [READ_MULTIPLE] handle[0x%04x] GATTS NG."  */
extern const char BTGATT_073[];
/*  "[GATT] Find by type value opcode[%x] Start[%x] End[%x]\n"  */
extern const char BTGATT_074[];
/*  "[GATT] [FIND_BY_TYPE] Can not support UUID[0x%04x]"  */
extern const char BTGATT_075[];
/*  "[GATT] [FIND_BY_TYPE] invalid handle[0x%04x]"  */
extern const char BTGATT_076[];
/*  "[GATT] Find by type value Rsp Handle[%x]\n"  */
extern const char BTGATT_077[];
/*  "[GATT] Find by type value Rsp Ending Handle[%x]\n"  */
extern const char BTGATT_078[];
/*  "[GATT] [FIND_BY_TYPE] Attribute not found."  */
extern const char BTGATT_079[];
/*  "[GATT] Find by type value Rsp len[%d]:\n"  */
extern const char BTGATT_080[];
/*  "[GATT] [WRITE_REQUEST] invalid handle[0x%04x]"  */
extern const char BTGATT_081[];
/*  "[GATT] [WRITE] invalid handle[0x%04x]"  */
extern const char BTGATT_082[];
/*  "[GATT] [SIGNED_WRITE] invalid handle[0x%04x]"  */
extern const char BTGATT_083[];
/*  "[GATT] [SUCCESS]signed write OK."  */
extern const char BTGATT_084[];
/*  "[GATT] [ERROR]signed write failed."  */
extern const char BTGATT_085[];
/*  "[GATT] LOCAL MAC:"  */
extern const char BTGATT_086[];
/*  "[GATT] CMD MAC:"  */
extern const char BTGATT_087[];
/*  "[GATT] LOCAL Signed counter:%d CMD Signed counter:%d"  */
extern const char BTGATT_088[];
/*  "[GATT] Async Response is selected."  */
extern const char BTGATT_089[];
/*  "[GATT] [PREPARE_WRITE] invalid handle[0x%04x]"  */
extern const char BTGATT_090[];
/*  "[GATT] [PREPARE_WRITE] handle[0x%04x] Queue full"  */
extern const char BTGATT_091[];
/*  "[GATT] [PREPARE_WRITE] FAILED with error code[0x%02x]"  */
extern const char BTGATT_092[];
/*  "[GATT] Async rsp, connection not found, connection handle = 0x%08x"  */
extern const char BTGATT_093[];
/*  "[GATT] GATTS Error Response, err_code 0x%x, handle 0x%x, status 0x%08x"  */
extern const char BTGATT_094[];
/*  "[GATT] GATTS Write Response, gatt_op 0x%x, status 0x%08x"  */
extern const char BTGATT_095[];
/*  "[GATT] Gatt rho allow:%d"  */
extern const char BTGATT_096[];
/*  "[GATT] [EDR] channel don't exist"  */
extern const char BTGATT_097[];
/*  "[GATT] Gatt rho get data length = %d"  */
extern const char BTGATT_098[];
/*  "[GATT] Gatt channel don't exist"  */
extern const char BTGATT_099[];
/*  "[GATT] Gatt rho agent update channel don't exist"  */
extern const char BTGATT_100[];
/*  "[GATT] Gatt rho partner set context is null  */
extern const char BTGATT_101[];
/*  "[GATT] Gatt rho update"  */
extern const char BTGATT_102[];
/*  "[GATT] [BREDR] channel is invalid"  */
extern const char BTGATT_103[];
/*  "[GATT] [BREDR] SDP search service fail"  */
extern const char BTGATT_104[];
/*  "[GATT] [BREDR] SDP search attribute fail"  */
extern const char BTGATT_105[];
/*  "[GATT] [BREDR] channel is invalid"  */
extern const char BTGATT_106[];
/*  "[GATT] [EDR] get connection handle fail after RHO"  */
extern const char BTGATT_107[];
/*  "[HCI] controller buffer is shared"  */
extern const char BTHCI_001[];
/*  "[HCI] bt_hci_check_connection_type: connection not found: handle(0x%02x)"  */
extern const char BTHCI_002[];
/*  "[HCI] bt_hci_check_connection_type: connection not found: handle(0x%02x)"  */
extern const char BTHCI_003[];
/*  "[HCI] nocp:acl_flow_control"  */
extern const char BTHCI_004[];
/*  "[HCI] bt_hci_nocp_callback: connection not found: handle(0x%02x)"  */
extern const char BTHCI_005[];
/*  "[HCI] bt_hci_nocp_callback: connection not found: handle(0x%02x)"  */
extern const char BTHCI_006[];
/*  "[HCI] connection destroy, acl_credit_counter:%d"  */
extern const char BTHCI_007[];
/*  "[HCI] conn destroy rx_to_tx\n"  */
extern const char BTHCI_008[];
/*  "[HCI] tx send: sleep(%d), cmd send(%d), eint mask(%d)"  */
extern const char BTHCI_009[];
/*  "[HCI] bt_hci_tx_send: disassemble (first) handle(0x%04x)"  */
extern const char BTHCI_010[];
/*  "[HCI] cmd to be tx when acl tx flow control"  */
extern const char BTHCI_011[];
/*  "[HCI] Flow-controlled: indicator(%d)"  */
extern const char BTHCI_012[];
/*  "[HCI] bt_hci_tx_send: tx done callback:(0x%8x)"  */
extern const char BTHCI_013[];
/*  "[HCI] bt_hci_temp_lock_acl_block"  */
extern const char BTHCI_014[];
/*  "[HCI] bt_hci_temp_unlock_acl_block"  */
extern const char BTHCI_015[];
/*  "[HCI] bt_hci_acl_send"  */
extern const char BTHCI_016[];
/*  "[HCI] bt_hci_tx_send: acl link is disconnecting when tx."  */
extern const char BTHCI_017[];
/*  "[HCI] bt_hci_tx_send: tx done callback:(0x%8x)"  */
extern const char BTHCI_018[];
/*  "[HCI] bt_hci_acl_send_with_auto_flush"  */
extern const char BTHCI_019[];
/*  "[HCI] bt_hci_tx_send: acl link is disconnecting when tx."  */
extern const char BTHCI_020[];
/*  "[HCI] bt_hci_tx_send: tx done callback:(0x%8x)"  */
extern const char BTHCI_021[];
/*  "[HCI] bt_hci_release_rx_packet: rx oom notify"  */
extern const char BTHCI_022[];
/*  "[HCI] clear rx buffer in tx queue\n"  */
extern const char BTHCI_023[];
/*  "[HCI] bt_hci_acl_le_send"  */
extern const char BTHCI_024[];
/*  "[HCI] bt_hci_cmd_send_debug"  */
extern const char BTHCI_025[];
/*  "[HCI] Abandon Tx cmd (%x), power status %d"  */
extern const char BTHCI_026[];
/*  "[HCI] out of memory sending command: 0x%04x"  */
extern const char BTHCI_027[];
/*  "[HCI] bt_hci_cmd_send"  */
extern const char BTHCI_028[];
/*  "[HCI] Abandon Tx cmd (%x), power status %d"  */
extern const char BTHCI_029[];
/*  "[HCI] out of memory sending command: 0x%04x"  */
extern const char BTHCI_030[];
/*  "[HCI] bt_hci_send_vendor_cmd curr 0x%x"  */
extern const char BTHCI_031[];
/*  "[HCI] Abandon Tx cmd (%x), power status %d"  */
extern const char BTHCI_032[];
/*  "[HCI] Abandon Tx cmd (%x), power status %d"  */
extern const char BTHCI_033[];
/*  "[HCI] out of memory sending command: 0x%04x"  */
extern const char BTHCI_034[];
/*  "[HCI] bt_hci_cmd_raw_data_send"  */
extern const char BTHCI_035[];
/*  "[HCI] Abandon Tx cmd (%x), power status %d"  */
extern const char BTHCI_036[];
/*  "[HCI] bt_hci_acl_proc"  */
extern const char BTHCI_037[];
/*  "[HCI] bt_hci_acl_proc: assembling (cont.)"  */
extern const char BTHCI_038[];
/*  "[HCI] bt_hci_acl_proc: edr conn is NULL, pacet(0x%08x)"  */
extern const char BTHCI_039[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_040[];
/*  "[HCI] bt_hci_acl_proc: le conn is NULL, pacet(0x%08x)"  */
extern const char BTHCI_041[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_042[];
/*  "[HCI] bt_hci_acl_proc: assembling (cont) error, xxx->Cont->First, rx_pending (0x%08x), pacet(0x%08x)"  */
extern const char BTHCI_043[];
/*  "[HCI] bt_hci_acl_proc: EDR assembling (cont) error, Fisrt->xxx->xxx->Cont, rx_pending (0x%08x), pacet(0x%08x)"  */
extern const char BTHCI_044[];
/*  "[HCI] ACL length > L2CAP length for the case assembling (cont) error, Fisrt->...->Cont"  */
extern const char BTHCI_045[];
/*  "[HCI] bt_hci_acl_proc: assembling (last)"  */
extern const char BTHCI_046[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_047[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_048[];
/*  "[HCI] bt_hci_acl_proc: assembling (first)"  */
extern const char BTHCI_049[];
/*  "[HCI] bt_hci_acl_proc: edr conn is NULL, pacet(0x%08x)"  */
extern const char BTHCI_050[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_051[];
/*  "[HCI] bt_hci_acl_proc: le conn is NULL, pacet(0x%08x)"  */
extern const char BTHCI_052[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_053[];
/*  "[HCI] bt_hci_acl_proc: assembling (first) error, First->xxx->First, rx_pending (0x%08x), pacet(0x%08x)"  */
extern const char BTHCI_054[];
/*  "[HCI] bt_hci_acl_proc: EDR assembling error, First->xxx->First(Complete), rx_pending (0x%08x), pacet(0x%08x)"  */
extern const char BTHCI_055[];
/*  "[HCI] bt_hci_tx_send: acl link is disconnecting when rx."  */
extern const char BTHCI_056[];
/*  "[HCI] acl Rx proc finished\n"  */
extern const char BTHCI_057[];
/*  "[HCI] bt_hci_acl_proc: LE assembling error, First->xxx->First(Complete), rx_pending (0x%08x), pacet(0x%08x)"  */
extern const char BTHCI_058[];
/*  "[HCI] LE acl Rx proc finished\n"  */
extern const char BTHCI_059[];
/*  "[HCI] bt_hci_acl_proc: rx oom notify"  */
extern const char BTHCI_060[];
/*  "[HCI] Connection not found!"  */
extern const char BTHCI_061[];
/*  "[HCI] bt_hci_evt_proc"  */
extern const char BTHCI_062[];
/*  "[HCI] [RHO] bt_hci_rho evt proc evt_code %x, cmd_code %x"  */
extern const char BTHCI_063[];
/*  "[HCI] Abandon Rx packet(%d), power status %d"  */
extern const char BTHCI_064[];
/*  "[HCI] cmp:cmd_flow_control"  */
extern const char BTHCI_065[];
/*  "[HCI] receive 0xFCC0: sleep(%d), cmd send(%d), eint mask(%d)"  */
extern const char BTHCI_066[];
/*  "[HCI] wakeup_retry_cancel fail\n"  */
extern const char BTHCI_067[];
/*  "[HCI] wakeup_retry_finish\n"  */
extern const char BTHCI_068[];
/*  "[HCI] bt_hci_evt_proc: not send wake up command"  */
extern const char BTHCI_069[];
/*  "[HCI] bt_hci_evt_proc: receive 0xFCC1 command complete\n"  */
extern const char BTHCI_070[];
/*  "[HCI] cms:cmd_flow_control"  */
extern const char BTHCI_071[];
/*  "[HCI] Controller Assert(info length:%d):\nAssert param:"  */
extern const char BTHCI_072[];
/*  "[HCI] param1:%d"  */
extern const char BTHCI_073[];
/*  "[HCI] param2:%d"  */
extern const char BTHCI_074[];
/*  "[HCI] param3:%d"  */
extern const char BTHCI_075[];
/*  "[HCI] Assert Line: %d"  */
extern const char BTHCI_076[];
/*  "[HCI] Assert File:%c"  */
extern const char BTHCI_077[];
/*  "[HCI] bt_hci_evt_proc:receive uart wakeup event\n"  */
extern const char BTHCI_078[];
/*  "[HCI] bt_timer_cancel fail\n"  */
extern const char BTHCI_079[];
/*  "[HCI] bt_hci_evt_proc: not send wake up command"  */
extern const char BTHCI_080[];
/*  "[HCI] bt_hci_evt_proc:receive uart sleep event\n"  */
extern const char BTHCI_081[];
/*  "[HCI] bt_timer_cancel fail\n"  */
extern const char BTHCI_082[];
/*  "[HCI] bt_timer_cancel fail\n"  */
extern const char BTHCI_083[];
/*  "[HCI] bt_hci_evt_proc:receive WMT event\n"  */
extern const char BTHCI_084[];
/*  "[HCI] bt_timer_cancel_and_callback status: 0x%08x"  */
extern const char BTHCI_085[];
/*  "[HCI] no callback event: 0x%02x"  */
extern const char BTHCI_086[];
/*  "[HCI] cmd tx out of memory"  */
extern const char BTHCI_087[];
/*  "[HCI] no acl or event"  */
extern const char BTHCI_088[];
/*  "[HCI] [RHO] bt_hci_rho process packet node 0x%x"  */
extern const char BTHCI_089[];
/*  "[HCI] bt_hci_process_packet: rx oom notify"  */
extern const char BTHCI_090[];
/*  "[HCI] rx out of memory"  */
extern const char BTHCI_091[];
/*  "[HCI] rx pending"  */
extern const char BTHCI_092[];
/*  "[HCI] Rx proc: status(0x%08x)"  */
extern const char BTHCI_093[];
/*  "[HCI] rx_to_tx link:0x%08x\n"  */
extern const char BTHCI_094[];
/*  "[HCI] rx_to_tx oom process:0x%08x\n"  */
extern const char BTHCI_095[];
/*  "[HCI] bt_hci_process_rx_oom: rx oom notify"  */
extern const char BTHCI_096[];
/*  "[HCI] bt_hci_is_sleep_time_out: duration time(%d), time out timer(%d)"  */
extern const char BTHCI_097[];
/*  "[HCI] bt_hci_trigger_controller: controller sleeped!"  */
extern const char BTHCI_098[];
/*  "[HCI] bt_hci_sleep_mode_timeout, [evt_flag]:%d!"  */
extern const char BTHCI_099[];
/*  "[HCI] controller wakeuping host"  */
extern const char BTHCI_100[];
/*  "[HCI] sleep cmd sending"  */
extern const char BTHCI_101[];
/*  "[HCI] sleep time out"  */
extern const char BTHCI_102[];
/*  "[HCI] bt_hci_restart_sleep_mode_timer:bt_hci_sleep_mode_timer config (%d)"  */
extern const char BTHCI_103[];
/*  "[HCI] bt_timer_cancel fail\n"  */
extern const char BTHCI_104[];
/*  "[HCI] bt_timer_start fail\n"  */
extern const char BTHCI_105[];
/*  "[HCI] bt_hci_init_sleep_mode_timer:bt_hci_sleep_mode_timer config (%d)"  */
extern const char BTHCI_106[];
/*  "[HCI] bt_timer_start fail: (0x%08x)\n"  */
extern const char BTHCI_107[];
/*  "[HCI] [RHO] bt_hci_rho_release_rx_queue() node 0x%x"  */
extern const char BTHCI_108[];
/*  "[HCI] [RHO] bt_hci_rho_rollback_rx_queue() node 0x%x"  */
extern const char BTHCI_109[];
/*  "[HCI] rho get flag %d"  */
extern const char BTHCI_110[];
/*  "[HCI] bt_hci_is_rho_allowed(), queue status(1:empty) rx %d tx %d"  */
extern const char BTHCI_111[];
/*  "[HCI] [RHO] bt_hci_rho_get_data_length() len 0x%x"  */
extern const char BTHCI_112[];
/*  "[HCI] [RHO] bt_hci_rho_get_data() node 0x%x, len 0x%x"  */
extern const char BTHCI_113[];
/*  "[HCI] [RHO] dump_rx_queue() node 0x%x, len 0x%x, indicator 0x%x"  */
extern const char BTHCI_114[];
/*  "[HCI] dump_rx_queue(), code %d"  */
extern const char BTHCI_115[];
/*  "[HCI] [RHO] data[%d] = 0x%x"  */
extern const char BTHCI_116[];
/*  "[HCI] [RHO] bt_hci_rho_update_context() role 0x%x, length 0x%x, data 0x%x, status 0x%x"  */
extern const char BTHCI_117[];
/*  "[HCI] [RHO] bt_hci_rho_update_context() rx_packet 0x%x, node 0x%x, indicator(0x%x, 0x%x), handle 0x%x, cpy_len 0x%x"  */
extern const char BTHCI_118[];
/*  "[HCI] [RHO] bt_hci_rho_update_context() node 0x%x, node_len 0x%x, offset 0x%x"  */
extern const char BTHCI_119[];
/*  "[HCI] [RHO] bt_hci_rho_update_context() rx OOM"  */
extern const char BTHCI_120[];
/*  "[HCI] CMD W4 twice send cmd_code: %d"*/
extern const char BTHCI_121[];
/*"send twice CMD begin! cmd_code: %d"*/
extern const char BTHCI_122[];

/*  "[L2CAP] bt_l2cap_get_channel: device_info (%02x-%02x-%02x-%02x-%02x-%02x),psm (%d)"  */
extern const char BTL2CAP_001[];
/*  "[L2CAP] bt_l2cap_get_channel: psm not support"  */
extern const char BTL2CAP_002[];
/*  "[L2CAP] bt_l2cap_get_channel: channel not exist"  */
extern const char BTL2CAP_003[];
/*  "[L2CAP] bt_l2cap_init_channel: device_info (%02x-%02x-%02x-%02x-%02x-%02x),psm (%d), channel(0x%08x)"  */
extern const char BTL2CAP_004[];
/*  "[L2CAP] acl link is disconnecting:(0x%08x)"  */
extern const char BTL2CAP_005[];
/*  "[L2CAP] acl link not exist"  */
extern const char BTL2CAP_006[];
/*  "[L2CAP] bt gap connect return NULL"  */
extern const char BTL2CAP_007[];
/*  "[L2CAP] bt_l2cap_connect_ext: conn status (0x%04x)"  */
extern const char BTL2CAP_008[];
/*  "[L2CAP] channel--0x%08x"  */
extern const char BTL2CAP_009[];
/*  "[L2CAP] bt_l2cap_connect_rsp_ext: TX OOM1"  */
extern const char BTL2CAP_010[];
/*  "[L2CAP] bt_l2cap_disconnect_ext: channel (0x%08x)"  */
extern const char BTL2CAP_011[];
/*  "[L2CAP] ERTM exceed max pdu size :%d"  */
extern const char BTL2CAP_012[];
/*  "[L2CAP] bt_l2cap_send_ext: tx_mtu(%d), tx_len(%d), channel psm(0x%04x)"  */
extern const char BTL2CAP_013[];
/*  "[L2CAP] bt_l2cap_send_with_auto_flush: tx_mtu(%d), tx_len(%d), channel psm(0x%04x)"  */
extern const char BTL2CAP_014[];
/*  "[L2CAP] bt_l2cap_send_with_tx_done_callback: channel(0x%08x), packet(0x%08x), callback(0x%08x)"  */
extern const char BTL2CAP_015[];
/*  "[L2CAP] bt_l2cap_check_channel_is_valid: valid channel (0x%08x)"  */
extern const char BTL2CAP_016[];
/*  "[L2CAP] bt_l2cap_get_psm_info_by_channel: channel callback (0x%08x), psm callback(0x%08x)"  */
extern const char BTL2CAP_017[];
/*  "[L2CAP] bt_l2cap_get_callback_by_psm: psm (0x%04x)"  */
extern const char BTL2CAP_018[];
/*  "[L2CAP] bt_l2cap_get_callback_by_psm: not found"  */
extern const char BTL2CAP_019[];
/*  "[L2CAP] bt_l2cap_get_context_op_callback_by_psm: psm (0x%04x)"  */
extern const char BTL2CAP_020[];
/*  "[L2CAP] bt_l2cap_get_context_op_callback_by_psm: not found"  */
extern const char BTL2CAP_021[];
/*  "[L2CAP] bt_l2cap_get_channel_mode_by_psm: psm (0x%04x)"  */
extern const char BTL2CAP_022[];
/*  "[L2CAP] bt_l2cap_get_channel_mode_by_psm: not found"  */
extern const char BTL2CAP_023[];
/*  "[L2CAP] bt_l2cap_init_channel_info: psm(0x%04x), channel(0x%08x)\n"  */
extern const char BTL2CAP_024[];
/*  "[L2CAP] bt_l2cap_init_channel_info: psm callback (0x%08x)"  */
extern const char BTL2CAP_025[];
/*  "[L2CAP] psm:%d not support"  */
extern const char BTL2CAP_026[];
/*  "[L2CAP] bt_l2cap_send_connect_req"  */
extern const char BTL2CAP_027[];
/*  "[L2CAP] bt_l2cap_send_config_req"  */
extern const char BTL2CAP_028[];
/*  "[L2CAP] bt_l2cap_send_disconnect_req"  */
extern const char BTL2CAP_029[];
/*  "[L2CAP] bt_l2cap_send_connect_rsp"  */
extern const char BTL2CAP_030[];
/*  "[L2CAP] bt_l2cap_send_config_rsp"  */
extern const char BTL2CAP_031[];
/*  "[L2CAP] out of memory sending config rsp"  */
extern const char BTL2CAP_032[];
/*  "[L2CAP] bt_l2cap_send_disconnect_rsp"  */
extern const char BTL2CAP_033[];
/*  "[L2CAP] bt_l2cap_send_information_req"  */
extern const char BTL2CAP_034[];
/*  "[L2CAP] bt_l2cap_send_infomation_rsp"  */
extern const char BTL2CAP_035[];
/*  "[L2CAP] out of memory sending config rsp"  */
extern const char BTL2CAP_036[];
/*  "[L2CAP] bt_l2cap_send_echo_req"  */
extern const char BTL2CAP_037[];
/*  "[L2CAP] bt_l2cap_send_echo_rsp"  */
extern const char BTL2CAP_038[];
/*  "[L2CAP] bt_l2cap_negotiate_second_config_req: find no psm info"  */
extern const char BTL2CAP_039[];
/*  "[L2CAP] negotiate MTU success"  */
extern const char BTL2CAP_040[];
/*  "[L2CAP] negotiate flow control success"  */
extern const char BTL2CAP_041[];
/*  "[L2CAP] bt_l2cap_snegotiate_second_config_req: alloc tx buffer fail"  */
extern const char BTL2CAP_042[];
/*  "[L2CAP] bt_l2cap_send_auto_config_req: find no psm info"  */
extern const char BTL2CAP_043[];
/*  "[L2CAP] bt_l2cap_send_auto_config_req: alloc tx buffer fail"  */
extern const char BTL2CAP_044[];
/*  "[L2CAP] check to send information req"  */
extern const char BTL2CAP_045[];
/*  "[L2CAP] if_w4_info_rsp remote_ex_feature 0x%04x"  */
extern const char BTL2CAP_046[];
/*  "[L2CAP] bt_l2cap_link_status_change_notify: conn (0x%08x), staus (0x%04x), error (0x%08x)"  */
extern const char BTL2CAP_047[];
/*  "[L2CAP] bt_l2cap_link_status_change_notify: find channel--0x%08x"  */
extern const char BTL2CAP_048[];
/*  "[L2CAP] bt_l2cap_link_status_change_notify: find channel--0x%08x"  */
extern const char BTL2CAP_049[];
/*  "[L2CAP] bt_l2cap_link_status_change_notify: find channel--0x%08x"  */
extern const char BTL2CAP_050[];
/*  "[L2CAP] bt_l2cap_link_status_change_notify: TX OOM2"  */
extern const char BTL2CAP_051[];
/*  "[L2CAP] bt_l2cap_handle_config_req:channel(0x%08x), local_id(0x%04x), rsp length(%d) "  */
extern const char BTL2CAP_052[];
/*  "[L2CAP] bt_l2cap_handle_config_req:local min_tx_mtu(%d), remote rx_mtu(%d) "  */
extern const char BTL2CAP_053[];
/*  "[L2CAP] bt_l2cap_handle_config_req:receive flush timeout option:(%d) "  */
extern const char BTL2CAP_054[];
/*  "[L2CAP] bt_l2cap_handle_config_req:receive flow control option:(%d) "  */
extern const char BTL2CAP_055[];
/*  "[L2CAP] ERTM second rsp"  */
extern const char BTL2CAP_056[];
/*  "[L2CAP] ERTM option OK"  */
extern const char BTL2CAP_057[];
/*  "[L2CAP] local tx window size:0x%02x"  */
extern const char BTL2CAP_058[];
/*  "[L2CAP] other mode"  */
extern const char BTL2CAP_059[];
/*  "[L2CAP] bt_l2cap_handle_config_req:receive fcs option"  */
extern const char BTL2CAP_060[];
/*  "[L2CAP] bt_l2cap_handle_config_req:unkown option(%d) "  */
extern const char BTL2CAP_061[];
/*  "[L2CAP] bt_l2cap_handle_config_req: TX OOM4"  */
extern const char BTL2CAP_062[];
/*  "[L2CAP] bt_l2cap_handle_config_req: TX OOM4"  */
extern const char BTL2CAP_063[];
/*  "[L2CAP] flow control option check fail"  */
extern const char BTL2CAP_064[];
/*  "[L2CAP] bt_l2cap_handle_config_req: TX OOM5"  */
extern const char BTL2CAP_065[];
/*  "[L2CAP] bt_l2cap_handle_config_req: TX OOM6"  */
extern const char BTL2CAP_066[];
/*  "[L2CAP] bt_l2cap_handle_config_rsp:channel(0x%08x), local_id(0x%04x), rsp length(%d) "  */
extern const char BTL2CAP_067[];
/*  "[L2CAP] channel already process config rsp"  */
extern const char BTL2CAP_068[];
/*  "[L2CAP] Enter ERTM mode check2 success"  */
extern const char BTL2CAP_069[];
/*  "[L2CAP] bt_l2cap_handle_config_rsp: TX OOM7"  */
extern const char BTL2CAP_070[];
/*  "[L2CAP] bt_l2cap_handle_config_rsp: TX OOM8"  */
extern const char BTL2CAP_071[];
/*  "[L2CAP] bt_l2cap_handle_config_rsp: TX OOM7"  */
extern const char BTL2CAP_072[];
/*  "[L2CAP] bt_l2cap_handle_config_rsp: TX OOM8"  */
extern const char BTL2CAP_073[];
/*  "[L2CAP] bt_l2cap_cmd_send opcode(0x%02x)"  */
extern const char BTL2CAP_074[];
/*  "[L2CAP] timer out of memory sending 0x%02x"  */
extern const char BTL2CAP_075[];
/*  "[L2CAP] out of memory sending 0x%02x"  */
extern const char BTL2CAP_076[];
/*  "[L2CAP] bt_l2cap_signal_handle: cmd code(0x%02x)"  */
extern const char BTL2CAP_077[];
/*  "[L2CAP] unknown timer_id: 0x%08x"  */
extern const char BTL2CAP_078[];
/*  "[L2CAP] bt_timer_cancel status: 0x%04x"  */
extern const char BTL2CAP_079[];
/*  "[L2CAP] psm not support:(0x%04x)"  */
extern const char BTL2CAP_080[];
/*  "[L2CAP] op_cb status 0x%08x\n"  */
extern const char BTL2CAP_081[];
/*  "[L2CAP] bt_l2cap_signal_handle: psm no resource:(0x%04x)"  */
extern const char BTL2CAP_082[];
/*  "[L2CAP] bt_l2cap_signal_handle: psm not support:(0x%04x)"  */
extern const char BTL2CAP_083[];
/*  "[L2CAP] bt_l2cap_signal_handle: other reason (0x%x), psm(0x%04x)"  */
extern const char BTL2CAP_084[];
/*  "[L2CAP] bt_l2cap_signal_handle: channel is null, psm(0x%04x), status(0x%x), op_cb(0x%x)"  */
extern const char BTL2CAP_085[];
/*  "[L2CAP] bt_l2cap_signal_handle: l2cap conn req, psm (0x%04x), channel(0x%08x)"  */
extern const char BTL2CAP_086[];
/*  "[L2CAP] unknown local_channel id: 0x%04x"  */
extern const char BTL2CAP_087[];
/*  "[L2CAP] unknown local_channel id: 0x%04x"  */
extern const char BTL2CAP_088[];
/*  "[L2CAP] bt_l2cap_signal_handle: alloc tx buffer fail"  */
extern const char BTL2CAP_089[];
/*  "[L2CAP] bt_l2cap_signal_handle: TX OOM9"  */
extern const char BTL2CAP_090[];
/*  "[L2CAP] bt_l2cap_signal_handle: support extend features:0x%08x"  */
extern const char BTL2CAP_091[];
/*  "[L2CAP] bt_l2cap_signal_handle: support fix channels:0x%08x%08x"  */
extern const char BTL2CAP_092[];
/*  "[L2CAP] bt_l2cap_signal_handle: fail info type"  */
extern const char BTL2CAP_093[];
/*  "[L2CAP] get info rsp"  */
extern const char BTL2CAP_094[];
/*  "[L2CAP] extended feature:0x%08x"  */
extern const char BTL2CAP_095[];
/*  "[L2CAP] unknown cmd: 0x%02x"  */
extern const char BTL2CAP_096[];
/*  "[L2CAP] bt_l2cap_rx_data_handle: channel is invalid"  */
extern const char BTL2CAP_097[];
/*  "[L2CAP] bt_l2cap_rx_proc: signal cmd rx error, pdu_len:(%d), cmd_len:(%d)"  */
extern const char BTL2CAP_098[];
/*  "[L2CAP] bt_l2cap_rx_proc: receive connectionless data"  */
extern const char BTL2CAP_099[];
/*  "[L2CAP] bt_l2cap_rx_proc: wrong channel id: 0x%04x"  */
extern const char BTL2CAP_100[];
/*  "[L2CAP] bt_l2cap_notify_flush_timeout: conn (0x%08x), staus (0x%04x)"  */
extern const char BTL2CAP_101[];
/*  "[L2CAP] bt_l2cap_notify_flush_timeout: find channel--0x%08x"  */
extern const char BTL2CAP_102[];
/*  "[L2CAP] out of memory sending data"  */
extern const char BTL2CAP_103[];
/*  "[L2CAP] bt_l2cap_ertm start monitor timer success id:0x%08x"  */
extern const char BTL2CAP_104[];
/*  "[L2CAP] bt_l2cap_ertm start monitor timer oom id:0x%08x"  */
extern const char BTL2CAP_105[];
/*  "[L2CAP] bt_l2cap_ertm start monitor timer Fail id:0x%08x"  */
extern const char BTL2CAP_106[];
/*  "[L2CAP] bt_l2cap_ertm stop monitor timer success id:0x%08x"  */
extern const char BTL2CAP_107[];
/*  "[L2CAP] bt_l2cap_ertm stop monitor timer Fail id:0x%08x"  */
extern const char BTL2CAP_108[];
/*  "[L2CAP] bt_l2cap_ertm start retransmit timer success id:0x%08x"  */
extern const char BTL2CAP_109[];
/*  "[L2CAP] bt_l2cap_ertm start retransmit timer oom id:0x%08x"  */
extern const char BTL2CAP_110[];
/*  "[L2CAP] bt_l2cap_ertm start retransmit timer Fail id:0x%08x"  */
extern const char BTL2CAP_111[];
/*  "[L2CAP] bt_l2cap_ertm stop retransmit timer success id:0x%08x"  */
extern const char BTL2CAP_112[];
/*  "[L2CAP] bt_l2cap_ertm stop retransmit timer Fail id:0x%08x"  */
extern const char BTL2CAP_113[];
/*  "[L2CAP] bt_l2cap_ertm_data_send"  */
extern const char BTL2CAP_114[];
/*  "[L2CAP] ERTM out of memory sending data"  */
extern const char BTL2CAP_115[];
/*  "[L2CAP] ERTM payload 0x%02x 0x%02x 0x%02x"  */
extern const char BTL2CAP_116[];
/*  "[L2CAP] ERTM payload 0x%02x 0x%02x 0x%02x"  */
extern const char BTL2CAP_117[];
/*  "[L2CAP] ertm tx next seq:0x%02x"  */
extern const char BTL2CAP_118[];
/*  "[L2CAP] ertm re tx seq:0x%02x"  */
extern const char BTL2CAP_119[];
/*  "[L2CAP] ertm send data fail, free data"  */
extern const char BTL2CAP_120[];
/*  "[L2CAP] send s frame  type: 0x%02x"  */
extern const char BTL2CAP_121[];
/*  "[L2CAP] ERTM oom sending s frame"  */
extern const char BTL2CAP_122[];
/*  "[L2CAP] ertm send s frame fail, free data"  */
extern const char BTL2CAP_123[];
/*  "[L2CAP] ERTM Ack send RNR"  */
extern const char BTL2CAP_124[];
/*  "[L2CAP] ERTM Ack send pending I frame"  */
extern const char BTL2CAP_125[];
/*  "[L2CAP] ERTM Ack send RR"  */
extern const char BTL2CAP_126[];
/*  "[L2CAP] ertm retransmit req seq"  */
extern const char BTL2CAP_127[];
/*  "[L2CAP] ertm retransmit hit tx seq 0x%02x"  */
extern const char BTL2CAP_128[];
/*  "[L2CAP] ertm exceed max transmit, disconnect"  */
extern const char BTL2CAP_129[];
/*  "[L2CAP] ertm retransmit tx seq 0x%02x success"  */
extern const char BTL2CAP_130[];
/*  "[L2CAP] ertm retransmit tx seq 0x%02x fail !!!"  */
extern const char BTL2CAP_131[];
/*  "[L2CAP] ERTM Send pending send stop"  */
extern const char BTL2CAP_132[];
/*  "[L2CAP] ERTM Send 1 pending data success"  */
extern const char BTL2CAP_133[];
/*  "[L2CAP] ERTM Send pending data OOM"  */
extern const char BTL2CAP_134[];
/*  "[L2CAP] ERTM Unexpect happend"  */
extern const char BTL2CAP_135[];
/*  "[L2CAP] ertm send IorRRor_RNR"  */
extern const char BTL2CAP_136[];
/*  "[L2CAP] ertm send fail oom"  */
extern const char BTL2CAP_137[];
/*  "[L2CAP] residual_length %d"  */
extern const char BTL2CAP_138[];
/*  "[L2CAP] ERTM gen end packet"  */
extern const char BTL2CAP_139[];
/*  "[L2CAP] ERTM gen continue packet"  */
extern const char BTL2CAP_140[];
/*  "[L2CAP] ERTM oom segment_SDU_data end/continue"  */
extern const char BTL2CAP_141[];
/*  "[L2CAP] free segment_head"  */
extern const char BTL2CAP_142[];
/*  "[L2CAP] ERTM segment_head/fail_pkt != NULL %d %d"  */
extern const char BTL2CAP_143[];
/*  "[L2CAP] ERTM Send SDU data to state machine"  */
extern const char BTL2CAP_144[];
/*  "[L2CAP] ERTM Segmentation Start"  */
extern const char BTL2CAP_145[];
/*  "[L2CAP] ERTM oom segment_SDU_data start"  */
extern const char BTL2CAP_146[];
/*  "[L2CAP] ERTM rev segment sar:%x"  */
extern const char BTL2CAP_147[];
/*  "[L2CAP] ERTM rev unsegmented data"  */
extern const char BTL2CAP_148[];
/*  "[L2CAP] ERTM app rev data oom"  */
extern const char BTL2CAP_149[];
/*  "[L2CAP] ERTM head != NULL or exceed pdu size"  */
extern const char BTL2CAP_150[];
/*  "[L2CAP] ERTM oom reassembly_SDU_data start"  */
extern const char BTL2CAP_151[];
/*  "[L2CAP] ertm reassemble end length not meet"  */
extern const char BTL2CAP_152[];
/*  "[L2CAP] ertm reassemble continue length unexpected"  */
extern const char BTL2CAP_153[];
/*  "[L2CAP] ertm reassemble success %x %x %x"  */
extern const char BTL2CAP_154[];
/*  "[L2CAP] ertm process req seq"  */
extern const char BTL2CAP_155[];
/*  "[L2CAP] unack list hit tx seq 0x%02x"  */
extern const char BTL2CAP_156[];
/*  "[L2CAP] ertm recv frame fcs FAIL!!! %x %x"  */
extern const char BTL2CAP_157[];
/*  "[L2CAP] Disconnect ERTM req_seq invalid"  */
extern const char BTL2CAP_158[];
/*  "[L2CAP] ertm_save_i_frame %x "  */
extern const char BTL2CAP_159[];
/*  "[L2CAP] ertm_save_i_frame hit1 "  */
extern const char BTL2CAP_160[];
/*  "[L2CAP] ertm_save_i_frame hit2 "  */
extern const char BTL2CAP_161[];
/*  "[L2CAP] ertm_save_i_frame append to NULL"  */
extern const char BTL2CAP_162[];
/*  "[L2CAP] ertm_select_reject_data_indication found seq %x"  */
extern const char BTL2CAP_163[];
/*  "[L2CAP] ertm_select_reject_data_indication not found"  */
extern const char BTL2CAP_164[];
/*  "[L2CAP] ertm send_select_rejects_by_list error!!!"  */
extern const char BTL2CAP_165[];
/*  "[L2CAP] free node success"  */
extern const char BTL2CAP_166[];
/*  "[L2CAP] ertm channel close!!!"  */
extern const char BTL2CAP_167[];
/*  "[L2CAP] bt_l2cap_ertm_oom_handler: channel(0x%08x)"  */
extern const char BTL2CAP_168[];
/*  "[L2CAP] ertm unack:0x%02x tx win:0x%02x:"  */
extern const char BTL2CAP_169[];
/*  "[L2CAP] ertm pending data"  */
extern const char BTL2CAP_170[];
/*  "[L2CAP] bt_l2cap_ertm retransmit timer expire!!"  */
extern const char BTL2CAP_171[];
/*  "[L2CAP] ertm pending data (tx wait f)"  */
extern const char BTL2CAP_172[];
/*  "[L2CAP] bt_l2cap_ertm monitor timer expire!! "  */
extern const char BTL2CAP_173[];
/*  "[L2CAP] ertm state_rx_recv control %x %x"  */
extern const char BTL2CAP_174[];
/*  "[L2CAP] recv I frame"  */
extern const char BTL2CAP_175[];
/*  "[L2CAP] valid tx seq"  */
extern const char BTL2CAP_176[];
/*  "[L2CAP] unexpected_tx_seq_flag == true, missing count:%x"  */
extern const char BTL2CAP_177[];
/*  "[L2CAP] change rx state to rej"  */
extern const char BTL2CAP_178[];
/*  "[L2CAP] change rx state to srej"  */
extern const char BTL2CAP_179[];
/*  "[L2CAP] recv S frame"  */
extern const char BTL2CAP_180[];
/*  "[L2CAP] ertm state_rx_rej_sent control %x %x"  */
extern const char BTL2CAP_181[];
/*  "[L2CAP] recv I frame"  */
extern const char BTL2CAP_182[];
/*  "[L2CAP] recv S frame"  */
extern const char BTL2CAP_183[];
/*  "[L2CAP] ertm state_rx_srej_sent control %x %x"  */
extern const char BTL2CAP_184[];
/*  "[L2CAP] recv I frame"  */
extern const char BTL2CAP_185[];
/*  "[L2CAP] recv S frame"  */
extern const char BTL2CAP_186[];
/*  "[L2CAP] ertm rx_state_machine"  */
extern const char BTL2CAP_187[];
/*  "[L2CAP] bt_l2cap_rx_loopback: channel(0x%02x)"  */
extern const char BTL2CAP_188[];
/*  "[L2CAP] bt_l2cap_bqb_conn_param_update_proc"  */
extern const char BTL2CAP_189[];
/*  "[L2CAP] bt_l2cap_le_signal_handle: cmd code(0x%02x)"  */
extern const char BTL2CAP_190[];
/*  "[L2CAP] BT_L2CAP_CMD_CONN_PARAM_UPDATE_REQ"  */
extern const char BTL2CAP_191[];
/*  "[L2CAP] slave received BT_L2CAP_CMD_CONN_PARAM_UPDATE_REQ"  */
extern const char BTL2CAP_192[];
/*  "[L2CAP] send bt_l2cap_conn_param_update_rsp failed status: 0x%04x"  */
extern const char BTL2CAP_193[];
/*  "[L2CAP] out of memory getting 0x%02x"  */
extern const char BTL2CAP_194[];
/*  "[L2CAP] Received BT_L2CAP_CMD_CONN_PARAM_UPDATE_RSP"  */
extern const char BTL2CAP_195[];
/*  "[L2CAP] bt_timer_cancel_and_callback status: 0x%04x"  */
extern const char BTL2CAP_196[];
/*  "[L2CAP] Received BT_L2CAP_CMD_COMMAND_REJECT"  */
extern const char BTL2CAP_197[];
/*  "[L2CAP] bt_timer_cancel_and_callback status: 0x%04x"  */
extern const char BTL2CAP_198[];
/*  "[L2CAP] unknown cmd: 0x%02x"  */
extern const char BTL2CAP_199[];
/*  "[L2CAP] unknown channel: 0x%02x"  */
extern const char BTL2CAP_200[];
/*  "[L2CAP] bt_l2cap_le_cmd_send opcode(0x%02x)"  */
extern const char BTL2CAP_201[];
/*  "[L2CAP] timer out of memory sending 0x%02x"  */
extern const char BTL2CAP_202[];
/*  "[L2CAP] out of memory sending 0x%02x"  */
extern const char BTL2CAP_203[];
/*  "[L2CAP] bt_l2cap_conn_param_update_req"  */
extern const char BTL2CAP_204[];
/*  "[L2CAP] bt_l2cap_conn_param_update_rsp"  */
extern const char BTL2CAP_205[];
/*  "[L2CAP] bt_l2cap_le_link_disconnect_notify: cancel timer_id(0x%08x)"  */
extern const char BTL2CAP_206[];
/*  "[L2CAP] no connection on channel"  */
extern const char BTL2CAP_207[];
/*  "[L2CAP] aws role: (%d)"  */
extern const char BTL2CAP_208[];
/*  "[L2CAP] user need security (0x%04x)\n"  */
extern const char BTL2CAP_209[];
/*  "[L2CAP] security is not enough\n"  */
extern const char BTL2CAP_210[];
/*  "[L2CAP] change state BT_L2CAP_STATE_W4_INFO_RSP_SEND_CONN_REQ"  */
extern const char BTL2CAP_211[];
/*  "[L2CAP] bt_l2cap_state_close: TX OOM10"  */
extern const char BTL2CAP_212[];
/*  "[L2CAP] change state BT_L2CAP_STATE_W4_INFO_RSP_SEND_CONN_REQ"  */
extern const char BTL2CAP_213[];
/*  "[L2CAP] user need security (%d)\n"  */
extern const char BTL2CAP_214[];
/*  "[L2CAP] bt_l2cap_state_close: TX OOM11"  */
extern const char BTL2CAP_215[];
/*  "[L2CAP] bt_l2cap_state_close: TX OOM12"  */
extern const char BTL2CAP_216[];
/*  "[L2CAP] bt_l2cap_state_w4_conn_rsp: received conn rsp, result(%d)"  */
extern const char BTL2CAP_217[];
/*  "[L2CAP] bt_l2cap_state_w4_conn_rsp: TX OOM13"  */
extern const char BTL2CAP_218[];
/*  "[L2CAP] bt_l2cap_state_w4_conn_rsp: TX OOM14"  */
extern const char BTL2CAP_219[];
/*  "[L2CAP] bt_l2cap_state_w4_upper_conn_rsp: TX OOM15"  */
extern const char BTL2CAP_220[];
/*  "[L2CAP] change state BT_L2CAP_STATE_W4_INFO_RSP_SEND_AUTO_CONFIG"  */
extern const char BTL2CAP_221[];
/*  "[L2CAP] bt_l2cap_state_w4_upper_conn_rsp: TX OOM16"  */
extern const char BTL2CAP_222[];
/*  "[L2CAP] security level update fail.\n"  */
extern const char BTL2CAP_223[];
/*  "[L2CAP] bt_l2cap_state_w4_upper_conn_rsp: TX OOM17"  */
extern const char BTL2CAP_224[];
/*  "[L2CAP] open psm %d\n"  */
extern const char BTL2CAP_225[];
/*  "[L2CAP] bt_l2cap_switch_to_open_state: success to open state\n "  */
extern const char BTL2CAP_226[];
/*  "[L2CAP] bt_l2cap_state_config:event(0x%04x), req->destination_cid(0x%04x), channel->local_channel(0x%04x)\n "  */
extern const char BTL2CAP_227[];
/*  "[L2CAP] bt_l2cap_state_config: TX OOM18"  */
extern const char BTL2CAP_228[];
/*  "[L2CAP] bt_l2cap_state_config:event(0x%04x), rsp->source_cid(0x%04x), channel->local_channel(0x%04x)\n "  */
extern const char BTL2CAP_229[];
/*  "[L2CAP] bt_l2cap_state_config: TX OOM19"  */
extern const char BTL2CAP_230[];
/*  "[L2CAP] bt_l2cap_state_w4_disc_rsp: channel(0x%08x), event(0x%04x)\n "  */
extern const char BTL2CAP_231[];
/*  "[L2CAP] bt_l2cap_state_open: TX OOM20"  */
extern const char BTL2CAP_232[];
/*  "[L2CAP] remote_ex_feature 0x%04x mode %x"  */
extern const char BTL2CAP_233[];
/*  "[L2CAP] bt_l2cap_state_close: TX OOM10"  */
extern const char BTL2CAP_234[];
/*  "[L2CAP] unexpected event %x in BT_L2CAP_STATE_W4_INFO_RSP state"  */
extern const char BTL2CAP_235[];
/*  "[L2CAP] bt_l2cap_state_w4_upper_conn_rsp: TX OOM16"  */
extern const char BTL2CAP_236[];
/*  "[L2CAP] bt_l2cap_state_config:event(0x%04x), req->destination_cid(0x%04x), channel->local_channel(0x%04x)\n "  */
extern const char BTL2CAP_237[];
/*  "[L2CAP] bt_l2cap_state_config: TX OOM18"  */
extern const char BTL2CAP_238[];
/*  "[L2CAP] bt_l2cap_state_config: TX OOM19"  */
extern const char BTL2CAP_239[];
/*  "[L2CAP] unexpected event %x in BT_L2CAP_STATE_W4_INFO_RSP_SEND_AUTO_CONFIG state"  */
extern const char BTL2CAP_240[];
/*  "[L2CAP] bt_l2cap_state_machine: channel(0x%08x), channel->state(%d), event(0x%04x)\n "  */
extern const char BTL2CAP_241[];
/*  "[L2CAP] l2cap handle:0x%08x\n"  */
extern const char BTL2CAP_242[];
/*  "[L2CAP] tx mtu:%d\n"  */
extern const char BTL2CAP_243[];
/*  "[L2CAP] out of memory sending data\n"  */
extern const char BTL2CAP_244[];
/*  "[L2CAP] conn(0x%08x), local identifier--%d\n"  */
extern const char BTL2CAP_245[];
/*  "[L2CAP] find channel--0x%08x\n"  */
extern const char BTL2CAP_246[];
/*  "[L2CAP] bt_l2cap_free_channel: channel(0x%08x)"  */
extern const char BTL2CAP_247[];
/*  "[L2CAP] free context fail!"  */
extern const char BTL2CAP_248[];
/*  "[L2CAP] bt_l2cap_close_channel: channel(0x%08x)"  */
extern const char BTL2CAP_249[];
/*  "[L2CAP] bt_l2cap_notify_user_acl_link_exception: channel(0x%08x), status(0x%08x)\n"  */
extern const char BTL2CAP_250[];
/*  "[L2CAP] bt_l2cap_notify_user_connect_fail_and_close_channel: channel(0x%08x), status(0x%08x)\n"  */
extern const char BTL2CAP_251[];
/*  "[L2CAP] bt_l2cap_notify_user_and_close_channel: channel(0x%08x),status(0x%08x)\n"  */
extern const char BTL2CAP_252[];
/*  "[L2CAP] bt_l2cap_rtx_timer_proc: timer_id(0x%08x)"  */
extern const char BTL2CAP_253[];
/*  "[L2CAP] find no l2cap channel for this timer\n"  */
extern const char BTL2CAP_254[];
/*  "[L2CAP] signal request timer 0x%08x expired"  */
extern const char BTL2CAP_255[];
/*  "[L2CAP] bt_l2cap_oom_handler: channel(0x%08x)"  */
extern const char BTL2CAP_256[];
/*  "[L2CAP] bt_l2cap_is_fixed_channel_packet: %d"  */
extern const char BTL2CAP_257[];
/*  "[L2CAP] bt_l2cap_handle_config_req:receive qos option, unsupport type (%d)"  */
extern const char BTL2CAP_258[];
/*  "[L2CAP] connection is not exist."  */
extern const char BTL2CAP_259[];
/*  "[L2CAP] The channel state of PSM (0x%04x) is %d."  */
extern const char BTL2CAP_260[];
/*  "[L2CAP] l2cap rho is allowed."  */
extern const char BTL2CAP_261[];
/*  "[L2CAP] l2cap rho get data length:%d."  */
extern const char BTL2CAP_262[];
/*  "[L2CAP] l2cap rho get data:psm(0x%04x), flags(%d),local cid(0x%04x),remote cid(0x%04x), remote rx mtu(%d), state(%d), identifier(%d)."  */
extern const char BTL2CAP_263[];
/*  "[L2CAP] l2cap rho update: role (0x%02x), data(0x%08x)."  */
extern const char BTL2CAP_264[];
/*  "[L2CAP] l2cap rho set data:psm(0x%04x),flags(%d),local cid(0x%04x),remote cid(0x%04x), remote rx mtu(%d), state(%d), identifier(%d)."  */
extern const char BTL2CAP_265[];
/*  "[L2CAP] bt_l2cap_fix_channel_register type = %d,cid = %d"  */
extern const char BTL2CAP_266[];
/*  "[L2CAP] bt_l2cap_fix_channel_register cid exist"  */
extern const char BTL2CAP_267[];
/*  "[L2CAP] bt_l2cap_fix_channel_register allocate fail"  */
extern const char BTL2CAP_268[];
/*  "[L2CAP] bt_l2cap_fix_channel_unregister type = %d,cid = %d"  */
extern const char BTL2CAP_269[];
/*  "[L2CAP] bt_l2cap_fix_channel_connect:acl link is exist"  */
extern const char BTL2CAP_270[];
/*  "[L2CAP] bt_l2cap_fix_channel_send context is null"  */
extern const char BTL2CAP_271[];
/*  "[L2CAP] bt_l2cap_fix_channel_send type error type = %d"  */
extern const char BTL2CAP_272[];
/*  "[L2CAP] bt_l2cap_fix_channel need link exist"  */
extern const char BTL2CAP_273[];
/*  "[L2CAP_LE_COC] LE coc increased. Control Block = 0x%x"  */
extern const char BTL2CAP_274[];
/*  "[L2CAP_LE_COC] bt_l2cap_le_locally_release_coc = 0x%x"  */
extern const char BTL2CAP_275[];
/*  "[L2CAP_LE_COC] Destroy LE COC channels on conn handle = 0x%x"  */
extern const char BTL2CAP_276[];
/*  "[L2CAP_LE_COC] bt_l2cap_le_search_next_coc coc = 0x%x"  */
extern const char BTL2CAP_277[];
/*  "[L2CAP_LE_COC] bt_l2cap_le_get_available_coc coc = 0x%X, mode = 0x%X, psm = 0x%X, state = 0x%X"  */
extern const char BTL2CAP_278[];
/*  "[L2CAP_LE_COC] bt_l2cap_le_coc_rx_proc (coc = 0x%x)  (local_cid = 0x%x)"  */
extern const char BTL2CAP_279[];
/*  "[RFCOMM] get channel, device_info (%02x-%02x-%02x-%02x-%02x-%02x),callback (0x%08x)"  */
extern const char BTRFCOMM_001[];
/*  "[RFCOMM] get channel:0x%08x"  */
extern const char BTRFCOMM_002[];
/*  "[RFCOMM] The channel is not exist!"  */
extern const char BTRFCOMM_003[];
/*  "[RFCOMM] init channel, device_info:%02x.%02x.%02x.%02x.%02x.%02x, channel:0x%x"  */
extern const char BTRFCOMM_004[];
/*  "[RFCOMM] init channel result:0x%x"  */
extern const char BTRFCOMM_005[];
/*  "[RFCOMM] rfcomm connect, session:0x%x,channel:0x%x"  */
extern const char BTRFCOMM_006[];
/*  "[RFCOMM] rfcomm connect,session->state:%d"  */
extern const char BTRFCOMM_007[];
/*  "[RFCOMM] rfcomm connect, session->flags:0x%x"  */
extern const char BTRFCOMM_008[];
/*  "[RFCOMM] rfcomm connect, session result:0x%x"  */
extern const char BTRFCOMM_009[];
/*  "[RFCOMM] rfcomm connect, dlci:%d"  */
extern const char BTRFCOMM_010[];
/*  "[RFCOMM] rfcomm connect result:%d"  */
extern const char BTRFCOMM_011[];
/*  "[RFCOMM] connect response, channel:0x%x, session:0x%x, response:%d"  */
extern const char BTRFCOMM_012[];
/*  "[RFCOMM] dlc_channel->tx_credit:%d"  */
extern const char BTRFCOMM_013[];
/*  "[RFCOMM] send_credit, channel:0x%x, rx credit:%d"  */
extern const char BTRFCOMM_014[];
/*  "[RFCOMM] get_max_frame_size :%d"  */
extern const char BTRFCOMM_015[];
/*  "[RFCOMM] get_credit:%d"  */
extern const char BTRFCOMM_016[];
/*  "[RFCOMM] get_server_id :%d"  */
extern const char BTRFCOMM_017[];
/*  "[RFCOMM] state machine, channel:0x%08x, state:%d"  */
extern const char BTRFCOMM_018[];
/*  "[RFCOMM] state machine, frame:%d"  */
extern const char BTRFCOMM_019[];
/*  "[RFCOMM] received tx credit:%d"  */
extern const char BTRFCOMM_020[];
/*  "[RFCOMM] send rx credit:%d"  */
extern const char BTRFCOMM_021[];
/*  "[RFCOMM] received unexcepted frame type:0x%x, disconnect l2cap."  */
extern const char BTRFCOMM_022[];
/*  "[RFCOMM] discard data in this state."  */
extern const char BTRFCOMM_023[];
/*  "[RFCOMM] frame.control:0x%x"  */
extern const char BTRFCOMM_024[];
/*  "[RFCOMM] channel->state:0x%x"  */
extern const char BTRFCOMM_025[];
/*  "[RFCOMM] oom_send_credit, channel:0x%08x)"  */
extern const char BTRFCOMM_026[];
/*  "[RFCOMM] send rx credit:%d"  */
extern const char BTRFCOMM_027[];
/*  "[RFCOMM] oom_connect, channel:0x%08x)"  */
extern const char BTRFCOMM_028[];
/*  "[RFCOMM] oom_connect, session->flags:0x%x"  */
extern const char BTRFCOMM_029[];
/*  "[RFCOMM] oom_disconnect, channel:0x%08x)"  */
extern const char BTRFCOMM_030[];
/*  "[RFCOMM] hold credit:%d"  */
extern const char BTRFCOMM_031[];
/*  "[RFCOMM] release credit:%d"  */
extern const char BTRFCOMM_032[];
/*  "[RFCOMM] send credit:%d"  */
extern const char BTRFCOMM_033[];
/*  "[RFCOMM] out of memory sending data\n"  */
extern const char BTRFCOMM_034[];
/*  "[RFCOMM] send_NUIH, dlci:%d, session:0x%x, type:0x%x"  */
extern const char BTRFCOMM_035[];
/*  "[RFCOMM] send_NUIH, result:0x%x"  */
extern const char BTRFCOMM_036[];
/*  "[RFCOMM] Update max frame size according to l2cap tx mtu, ori:%d, new:%d"  */
extern const char BTRFCOMM_037[];
/*  "[RFCOMM] send PN result:0x%x"  */
extern const char BTRFCOMM_038[];
/*  "[RFCOMM] receive PN, channel:0x%08x, accept:%d"  */
extern const char BTRFCOMM_039[];
/*  "[RFCOMM] receive PN, result:0x%x"  */
extern const char BTRFCOMM_040[];
/*  "[RFCOMM] send TEST result:0x%x"  */
extern const char BTRFCOMM_041[];
/*  "[RFCOMM] send MSC, channel:0x%08x, CR:0x%x"  */
extern const char BTRFCOMM_042[];
/*  "[RFCOMM] PN request handler, dlci:%d, priority:%d, max_frame_size:%d"  */
extern const char BTRFCOMM_043[];
/*  "[RFCOMM] connecting to local server id:%d"  */
extern const char BTRFCOMM_044[];
/*  "[RFCOMM] PN request handler result:0x%x"  */
extern const char BTRFCOMM_045[];
/*  "[RFCOMM] PN response handler, dlci:%d, session:0x%x"  */
extern const char BTRFCOMM_046[];
/*  "[RFCOMM] setup max frame size:%d."  */
extern const char BTRFCOMM_047[];
/*  "[RFCOMM] setup default frame size:%d."  */
extern const char BTRFCOMM_048[];
/*  "[RFCOMM] continue to connect wait conn channle"  */
extern const char BTRFCOMM_049[];
/*  "[RFCOMM] error, PN response not matched request!"  */
extern const char BTRFCOMM_050[];
/*  "[RFCOMM] PN response handler result:0x%x"  */
extern const char BTRFCOMM_051[];
/*  "[RHO][RHO]session:0x%x, state:0x%x, flag:0x%x, is allowed:0x%x"  */
extern const char BTRFCOMM_052[];
/*  "[RFCOMM] [RHO]session rho len: %d"  */
extern const char BTRFCOMM_053[];
/*  "[RFCOMM] [RHO]session don't exist"  */
extern const char BTRFCOMM_054[];
/*  "[RFCOMM] [RHO]Update session, role:0x%x"  */
extern const char BTRFCOMM_055[];
/*  "[RFCOMM] [RHO]Update fail, result:0x%x"  */
extern const char BTRFCOMM_056[];
/*  "[RFCOMM] [RHO]Wrong role:0x%x"  */
extern const char BTRFCOMM_057[];
/*  "[RFCOMM] [RHO]max size:%d, rx credit:%d, tx credit:%d, dlci:%d, flags:%d, state:%d"  */
extern const char BTRFCOMM_058[];
/*  "[RFCOMM] [RHO]update channel, role:0x%x"  */
extern const char BTRFCOMM_059[];
/*  "[RFCOMM] [RHO]channel:0x%x, session:0x%x"  */
extern const char BTRFCOMM_060[];
/*  "[RFCOMM] [RHO]max size:%d, rx credit:%d, tx credit:%d, dlci:%d, flags:%d, state:%d"  */
extern const char BTRFCOMM_061[];
/*  "[RFCOMM] [RHO]Wrong role:0x%x"  */
extern const char BTRFCOMM_062[];
/*  "[RFCOMM] get session, l2cap_channel:0x%08x"  */
extern const char BTRFCOMM_063[];
/*  "[RFCOMM] l2cap callback, channel:0x%08x,event:%d"  */
extern const char BTRFCOMM_064[];
/*  "[RFCOMM] No fixed memory, need enarged if want to support more links!."  */
extern const char BTRFCOMM_065[];
/*  "[RFCOMM] Has sent rfcomm connect request, so reject the new one."  */
extern const char BTRFCOMM_066[];
/*  "[RFCOMM] dlci on the MSC:%d"  */
extern const char BTRFCOMM_067[];
/*  "[RFCOMM] can't find the channel:0x%x"  */
extern const char BTRFCOMM_068[];
/*  "[RFCOMM] received MSC response."  */
extern const char BTRFCOMM_069[];
/*  "[RFCOMM] received frame type on session open:0x%x"  */
extern const char BTRFCOMM_070[];
/*  "[RFCOMM] recevied frame type on conn pending:0x%x"  */
extern const char BTRFCOMM_071[];
/*  "[RFCOMM] continue to connect wait conn channle"  */
extern const char BTRFCOMM_072[];
/*  "[RFCOMM] received unexcepted  l2cap status :0x%0x"  */
extern const char BTRFCOMM_073[];
/*  "[RFCOMM] received unexcepted  l2cap event: %d"  */
extern const char BTRFCOMM_074[];
/*  "[RFCOMM] session state machine, session:0x%08x, state:%d"  */
extern const char BTRFCOMM_075[];
/*  "[RFCOMM] notify, session:0x%08x, result:0x%08x"  */
extern const char BTRFCOMM_076[];
/*  "[RFCOMM] external default_bt_rfcomm_hfp_callback is not defined."  */
extern const char BTRFCOMM_077[];
/*  "[RFCOMM] external default_bt_rfcomm_hsp_callback is not defined."  */
extern const char BTRFCOMM_078[];
/*  "[RFCOMM] external default_bt_rfcomm_obex_callback is not defined."  */
extern const char BTRFCOMM_079[];
/*  "[RFCOMM] external default_bt_rfcomm_spp_callback is not defined."  */
extern const char BTRFCOMM_080[];
/*  "[RFCOMM] get callback, server_id:%d"  */
extern const char BTRFCOMM_081[];
/*  "[RFCOMM] can't find callback."  */
extern const char BTRFCOMM_082[];
/*  "[RFCOMM] find channel, dlci:%d, session:0x%08x"  */
extern const char BTRFCOMM_083[];
/*  "[RFCOMM] find channel, channel:0x%08x"  */
extern const char BTRFCOMM_084[];
/*  "[RFCOMM] find channel, can't found channel."  */
extern const char BTRFCOMM_085[];
/*  "[RFCOMM] find wait conn channel, session:0x%08x"  */
extern const char BTRFCOMM_086[];
/*  "[RFCOMM] find wait conn channel, channel:0x%08x"  */
extern const char BTRFCOMM_087[];
/*  "[RFCOMM] insert channel:0x%x, session:0x%x"  */
extern const char BTRFCOMM_088[];
/*  "[RFCOMM] remove channel:0x%x, session:0x%x"  */
extern const char BTRFCOMM_089[];
/*  "[RFCOMM] allocate session."  */
extern const char BTRFCOMM_090[];
/*  "[RFCOMM] allocate session, Out of Memory"  */
extern const char BTRFCOMM_091[];
/*  "[RFCOMM] allocate session:0x%08x"  */
extern const char BTRFCOMM_092[];
/*  "[RFCOMM] free session:0x%08x"  */
extern const char BTRFCOMM_093[];
/*  "[RFCOMM] check channel exist:0x%08x"  */
extern const char BTRFCOMM_094[];
/*  "[RFCOMM] check result:%d"  */
extern const char BTRFCOMM_095[];
/*  "[RFCOMM] start timer, handle:0x%08x, type:%d, time_ms:%d, timer id:0x%08x"  */
extern const char BTRFCOMM_096[];
/*  "[RFCOMM] stop timer, handle:0x%08x, type:%d"  */
extern const char BTRFCOMM_097[];
/*  "[RFCOMM] stop all timer, handle:0x%08x"  */
extern const char BTRFCOMM_098[];
/*  "[RFCOMM] timeout handler, timer_id(0x%08x)"  */
extern const char BTRFCOMM_099[];
/*  "[RFCOMM] oom trigger, channel(0x%08x)"  */
extern const char BTRFCOMM_100[];
/*  "[RFCOMM] oom trigger, session(0x%08x)"  */
extern const char BTRFCOMM_101[];
/*  "[RFCOMM] oom stop, channel(0x%08x), callback(0x%08x)"  */
extern const char BTRFCOMM_102[];
/*  "[RFCOMM] oom stop, session(0x%08x), callback(0x%08x)"  */
extern const char BTRFCOMM_103[];
/*  "[RFCOMM] deinit session:0x%08x"  */
extern const char BTRFCOMM_104[];
/*  "[RFCOMM] session:0x%08x, flag:0x%08x, state:0x%08x" */
extern const char BTRFCOMM_105[];
/*  "[RFCOMM] channel:0x%08x, flag:0x%08x, state:0x%08x"  */
extern const char BTRFCOMM_106[];
/*  "[RFCOMM] l2cap OP CB, event:0x%x."  */
extern const char BTRFCOMM_107[];
/*  "[RFCOMM] get all channels, found_channels: 0x%x address:0x%x:%x:%x:%x:%x:%x callback:0x%x"  */
extern const char BTRFCOMM_108[];
/*  "[RFCOMM] get all channels, found channl %d: 0x%x"  */
extern const char BTRFCOMM_109[];
/*  "[RFCOMM] get all channels, total %d channel(s) found"  */
extern const char BTRFCOMM_110[];
/*  "[RFCOMM] [RFCOMM] oom trigger, callback(0x%08x)"  */
extern const char BTRFCOMM_111[];
/*  "[SDP] bt_sdp_l2cap_callback, channel %x, info->event %d"  */
extern const char BTSDP_001[];
/*  "[SDP] bt_sdp_l2cap_callback, allocate L2CAP conn rsp OOM"  */
extern const char BTSDP_002[];
/*  "[SDP] bt_sdp_l2cap_context_op_callback, event %d"  */
extern const char BTSDP_003[];
/*  "[SDP] bt_sdp_l2cap_context_op_callback, allocate sdp server channel %x"  */
extern const char BTSDP_004[];
/*  "[SDP] bt_sdp_l2cap_context_op_callback, allocate sdp server channel OOM"  */
extern const char BTSDP_005[];
/*  "[SDP] bt_sdp_l2cap_context_op_callback, free sdp client channel %x"  */
extern const char BTSDP_006[];
/*  "[SDP] bt_sdpc_oom_handler"  */
extern const char BTSDP_007[];
/*  "[SDP] bt_sdpc_delete_request"  */
extern const char BTSDP_008[];
/*  "[SDP] bt_sdpc_timeout_callback, timer_id %x, data %x"  */
extern const char BTSDP_009[];
/*  "[SDP] bt_sdpc_start_query, mode %d"  */
extern const char BTSDP_010[];
/*  "[SDP] bt_sdpc_start_query, pdu_size %d, l2cap_mtu_size %d"  */
extern const char BTSDP_011[];
/*  "[SDP] bt_sdpc_handle_current_request, disconnect l2cap channel"  */
extern const char BTSDP_012[];
/*  "[SDP] bt_sdpc_handle_current_request, req state %d, channel state %d"  */
extern const char BTSDP_013[];
/*  "[SDP] bt_sdpc_handle_current_request, state error, none"  */
extern const char BTSDP_014[];
/*  "[SDP] bt_sdpc_handle_current_request, state error, disconnecting"  */
extern const char BTSDP_015[];
/*  "[SDP] bt_sdpc_handle_current_request, new sdp request, but channel is still connecting"  */
extern const char BTSDP_016[];
/*  "[SDP] bt_sdpc_handle_current_request, start query OOM, state 0x%x, node 0x%x"  */
extern const char BTSDP_017[];
/*  "[SDP] bt_sdpc_allocate_channel, channel %x"  */
extern const char BTSDP_018[];
/*  "[SDP] bt_sdpc_free_channel, channel %x"  */
extern const char BTSDP_019[];
/*  "[SDP] bt_sdpc_free_channel, state 0x%x, node 0x%x"  */
extern const char BTSDP_020[];
/*  "[SDP] bt_sdpc_free_channel, pre_packet 0x%x"  */
extern const char BTSDP_021[];
/*  "[SDP] bt_sdpc_add_request, connection %x"  */
extern const char BTSDP_022[];
/*  "[SDP] bt_sdpc_add_request, allocate sdpc channel OOM"  */
extern const char BTSDP_023[];
/*  "[SDP] bt_sdpc_add_request, connection->sdpc_channel %x"  */
extern const char BTSDP_024[];
/*  "[SDP] bt_sdpc_data_handle, req state %d, trans_id %d"  */
extern const char BTSDP_025[];
/*  "[SDP] bt_sdpc_data_handle, sdp_data[0] %d"  */
extern const char BTSDP_026[];
/*  "[SDP] bt_sdpc_data_handle, cont_state %d"  */
extern const char BTSDP_027[];
/*  "[SDP] bt_sdpc_service_search, request_node %x"  */
extern const char BTSDP_028[];
/*  "[SDP] bt_sdpc_service_search, allocate handle request buffer OOM"  */
extern const char BTSDP_029[];
/*  "[SDP] bt_sdpc_service_attribute, request_node %x"  */
extern const char BTSDP_030[];
/*  "[SDP] bt_sdpc_service_attribute, allocate attr request buffer OOM"  */
extern const char BTSDP_031[];
/*  "[SDP] bt_sdpc_event_callback: handle_number: %d"  */
extern const char BTSDP_032[];
/*  "[SDP] bt_sdpc_event_callback: attr query len: %d"  */
extern const char BTSDP_033[];
/*  "[SDP] bt_sdpc_event_callback: status: 0x%x"  */
extern const char BTSDP_034[];
/*  "[SDP] bt_sdps_find_channel, channel %x"  */
extern const char BTSDP_035[];
/*  "[SDP] bt_sdps_add_channel, channel %x"  */
extern const char BTSDP_036[];
/*  "[SDP] bt_sdps_remove_channel, free sdp server channel %x"  */
extern const char BTSDP_037[];
/*  "[SDP] bt_sdps_remove_channel, channel %x is not existed"  */
extern const char BTSDP_038[];
/*  "[SDP] bt_sdps_db_build_record_handle_rsp_packet, allocate L2CAP buffer OOM"  */
extern const char BTSDP_039[];
/*  "[SDP] bt_sdps_build_error_rsp_packet, allocate L2CAP buffer OOM"  */
extern const char BTSDP_040[];
/*  "[SDP] bt_sdps_db_build_attr_rsp_packet, allocate L2CAP buffer OOM"  */
extern const char BTSDP_041[];
/*  "[SDP] bt_sdps_db_search_attr_match_packet, record_handle %x, customized_record_number %x"  */
extern const char BTSDP_042[];
/*  "[SM] bt_sm_gen_key_distribute_state"  */
extern const char BTSM_009[];
/*  "[SM] bt_sm_find_next_dist"  */
extern const char BTSM_010[];
/*  "[SM] bt_sm_key_dist_send"  */
extern const char BTSM_011[];
/*  "[SM] bt_sm_key_dist_receive"  */
extern const char BTSM_012[];
/*  "[SM] bt_sm_calc_c1"  */
extern const char BTSM_013[];
/*  "[SM] p2 = "  */
extern const char BTSM_014[];
/*  "[SM] bt_sm_calc_f4"  */
extern const char BTSM_015[];
/*  "[SM] Key = "  */
extern const char BTSM_016[];
/*  "[SM] M = "  */
extern const char BTSM_017[];
/*  "[SM] bt_sm_calc_f5: counter(%d)"  */
extern const char BTSM_018[];
/*  "[SM] Key = "  */
extern const char BTSM_019[];
/*  "[SM] M = "  */
extern const char BTSM_020[];
/*  "[SM] bt_sm_calc_f6"  */
extern const char BTSM_021[];
/*  "[SM] iocap = "  */
extern const char BTSM_022[];
/*  "[SM] ra= "  */
extern const char BTSM_023[];
/*  "[SM] Key = "  */
extern const char BTSM_024[];
/*  "[SM] M = "  */
extern const char BTSM_025[];
/*  "[SM] bt_sm_calc_g2"  */
extern const char BTSM_026[];
/*  "[SM] Key = "  */
extern const char BTSM_027[];
/*  "[SM] M = "  */
extern const char BTSM_028[];
/*  "[SM] G2 = "  */
extern const char BTSM_029[];
/*  "[SM] Using BT_GAP_LE_SMP_JUST_WORKS"  */
extern const char BTSM_030[];
/*  "[SM] Using BT_GAP_LE_SMP_PASSKEY_INPUT"  */
extern const char BTSM_031[];
/*  "[SM] Using BT_GAP_LE_SMP_PASSKEY_DISPLAY"  */
extern const char BTSM_032[];
/*  "[SM] Using BT_GAP_LE_SMP_OOB"  */
extern const char BTSM_033[];
/*  "[SM] Using BT_GAP_LE_SMP_LESC_JUST_WORKS"  */
extern const char BTSM_034[];
/*  "[SM] Using BT_GAP_LE_SMP_LESC_PASSKEY_INPUT"  */
extern const char BTSM_035[];
/*  "[SM] Using BT_GAP_LE_SMP_LESC_PASSKEY_DISPLAY"  */
extern const char BTSM_036[];
/*  "[SM] Using BT_GAP_LE_SMP_LESC_OOB"  */
extern const char BTSM_037[];
/*  "[SM] Using BT_GAP_LE_SMP_LESC_NUMERIC_COMPARISON"  */
extern const char BTSM_038[];
/*  "[SM] bt_sm_cal_key_gen_method"  */
extern const char BTSM_039[];
/*  "[SM] Do not support LESC OOB, disable LESC"  */
extern const char BTSM_040[];
/*  "[SM] bt_sm_pairing_user_input_ind"  */
extern const char BTSM_041[];
/*  "[SM] bt_sm_pairing_complete_ind: key_security_mode(0x%08x)"  */
extern const char BTSM_042[];
/*  "[SM] LTK = "  */
extern const char BTSM_043[];
/*  "[SM] IRK = "  */
extern const char BTSM_044[];
/*  "[SM] CSRK = "  */
extern const char BTSM_045[];
/*  "[SM] bt_sm_cal_key_dist_method"  */
extern const char BTSM_046[];
/*  "[SM] bt_sm_master_pairing_proc: timer_id(0x%08x), substate(0x%08x)"  */
extern const char BTSM_047[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_COMPLETE"  */
extern const char BTSM_048[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_FAILED"  */
extern const char BTSM_049[];
/*  "[SM] bt_sm_master_pairing_proc: Timeout"  */
extern const char BTSM_050[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_SECURITY_REQ"  */
extern const char BTSM_051[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_START_PAIRING"  */
extern const char BTSM_052[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_PAIRING_RSP"  */
extern const char BTSM_053[];
/*  "[SM] p1 = "  */
extern const char BTSM_054[];
/*  "[SM] bt_sm_master_pairing_proc: BT_HCI_SUBEVT_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE"  */
extern const char BTSM_055[];
/*  "[SM] validate local public key fail, it's normal"  */
extern const char BTSM_056[];
/*  "[SM] Local Public Key X = "  */
extern const char BTSM_057[];
/*  "[SM] Local Public Key Y = "  */
extern const char BTSM_058[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_PAIRING_PUBLIC_KEY"  */
extern const char BTSM_059[];
/*  "[SM] Peer Public Key X = "  */
extern const char BTSM_060[];
/*  "[SM] Peer Public Key Y = "  */
extern const char BTSM_061[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_LESC_SEND_CONFIRM"  */
extern const char BTSM_062[];
/*  "[SM] Na = "  */
extern const char BTSM_063[];
/*  "[SM] Local Confrim Ca = "  */
extern const char BTSM_064[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_APP_INPUT"  */
extern const char BTSM_065[];
/*  "[SM] TK = "  */
extern const char BTSM_066[];
/*  "[SM] Mrand = "  */
extern const char BTSM_067[];
/*  "[SM] Local Mconfirm c1 = "  */
extern const char BTSM_068[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_LESC_SEND_DHKEY_CHECK"  */
extern const char BTSM_069[];
/*  "[SM] validate public key fail"  */
extern const char BTSM_070[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_PAIRING_CONFIRM"  */
extern const char BTSM_071[];
/*  "[SM] Peer Sconfirm c1 = "  */
extern const char BTSM_072[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_PAIRING_RANDOM"  */
extern const char BTSM_073[];
/*  "[SM] Peer Confrim Cb = "  */
extern const char BTSM_074[];
/*  "[SM] Local Confrim Cb = "  */
extern const char BTSM_075[];
/*  "[SM] Local Sconfirm c1 = "  */
extern const char BTSM_076[];
/*  "[SM] Local Vb = %06d"  */
extern const char BTSM_077[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_PAIRING_RANDOM: count(%d->%d)"  */
extern const char BTSM_078[];
/*  "[SM] r' = "  */
extern const char BTSM_079[];
/*  "[SM] s1' = "  */
extern const char BTSM_080[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_LESC_WAIT_APP_INPUT"  */
extern const char BTSM_081[];
/*  "[SM] Local Vb = %06d"  */
extern const char BTSM_082[];
/*  "[SM] bt_sm_master_pairing_proc: BT_HCI_SUBEVT_LE_GENERATE_DHKEY_COMPLETE"  */
extern const char BTSM_083[];
/*  "[SM] DHKey = "  */
extern const char BTSM_084[];
/*  "[SM] T = "  */
extern const char BTSM_085[];
/*  "[SM] MacKey = "  */
extern const char BTSM_086[];
/*  "[SM] LTK = "  */
extern const char BTSM_087[];
/*  "[SM] Local Ea = "  */
extern const char BTSM_088[];
/*  "[SM] Local Eb = "  */
extern const char BTSM_089[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SMP_CODE_PAIRING_DHKEY_CHECK"  */
extern const char BTSM_090[];
/*  "[SM] Peer Eb = "  */
extern const char BTSM_091[];
/*  "[SM] bt_sm_master_pairing_proc: BT_HCI_EVT_ENCRYPTION_CHANGE"  */
extern const char BTSM_092[];
/*  "[SM] bt_sm_master_pairing_proc: BT_SM_EVENT_KEY_DIST"  */
extern const char BTSM_093[];
/*  "[SM] bt_sm_master_pairing_proc: Distributed Keys"  */
extern const char BTSM_094[];
/*  "[SM] bt_sm_slave_pairing_proc: timer_id(0x%08x), substate(0x%08x)"  */
extern const char BTSM_095[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_COMPLETE"  */
extern const char BTSM_096[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_FAILED"  */
extern const char BTSM_097[];
/*  "[SM] bt_sm_slave_pairing_proc: Timeout"  */
extern const char BTSM_098[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SMP_CODE_PAIRING_REQ"  */
extern const char BTSM_099[];
/*  "[SM] p1 = "  */
extern const char BTSM_100[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SMP_CODE_PAIRING_PUBLIC_KEY"  */
extern const char BTSM_101[];
/*  "[SM] Peer Public Key X = "  */
extern const char BTSM_102[];
/*  "[SM] Peer Public Key Y = "  */
extern const char BTSM_103[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_HCI_SUBEVT_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE"  */
extern const char BTSM_104[];
/*  "[SM] validate local public key fail, it's normal"  */
extern const char BTSM_105[];
/*  "[SM] Local Public Key X = "  */
extern const char BTSM_106[];
/*  "[SM] Local Public Key Y = "  */
extern const char BTSM_107[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_LESC_WAIT_APP_INPUT"  */
extern const char BTSM_108[];
/*  "[SM] Local Vb = %06d"  */
extern const char BTSM_109[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_LESC_SEND_CONFIRM"  */
extern const char BTSM_110[];
/*  "[SM] Nb = "  */
extern const char BTSM_111[];
/*  "[SM] Local Confrim Cb = "  */
extern const char BTSM_112[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SMP_CODE_PAIRING_CONFIRM"  */
extern const char BTSM_113[];
/*  "[SM] Peer Mconfirm c1 = "  */
extern const char BTSM_114[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_APP_INPUT"  */
extern const char BTSM_115[];
/*  "[SM] TK = "  */
extern const char BTSM_116[];
/*  "[SM] Srand = "  */
extern const char BTSM_117[];
/*  "[SM] Local Sconfirm c1 = "  */
extern const char BTSM_118[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SMP_CODE_PAIRING_RANDOM"  */
extern const char BTSM_119[];
/*  "[SM] Peer Confrim Ca = "  */
extern const char BTSM_120[];
/*  "[SM] Local Confrim Ca = "  */
extern const char BTSM_121[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SMP_CODE_PAIRING_RANDOM: count(%d->%d)"  */
extern const char BTSM_122[];
/*  "[SM] Local Mconfirm c1 = "  */
extern const char BTSM_123[];
/*  "[SM] r' = "  */
extern const char BTSM_124[];
/*  "[SM] s1' = "  */
extern const char BTSM_125[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SMP_CODE_PAIRING_DHKEY_CHECK"  */
extern const char BTSM_126[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_LESC_SEND_DHKEY_CHECK"  */
extern const char BTSM_127[];
/*  "[SM] Remote Ea = "  */
extern const char BTSM_128[];
/*  "[SM] validate public key fail"  */
extern const char BTSM_129[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_HCI_SUBEVT_LE_GENERATE_DHKEY_COMPLETE"  */
extern const char BTSM_130[];
/*  "[SM] DHkey check fail, status error"  */
extern const char BTSM_131[];
/*  "[SM] DHkey check fail, value error"  */
extern const char BTSM_132[];
/*  "[SM] DHKey = "  */
extern const char BTSM_133[];
/*  "[SM] T = "  */
extern const char BTSM_134[];
/*  "[SM] MacKey = "  */
extern const char BTSM_135[];
/*  "[SM] LTK = "  */
extern const char BTSM_136[];
/*  "[SM] Local Ea = "  */
extern const char BTSM_137[];
/*  "[SM] Local Eb = "  */
extern const char BTSM_138[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_HCI_SUBEVT_LE_LONG_TERM_KEY_REQUEST"  */
extern const char BTSM_139[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_HCI_EVT_ENCRYPTION_CHANGE"  */
extern const char BTSM_140[];
/*  "[SM] bt_sm_slave_pairing_proc: BT_SM_EVENT_KEY_DIST"  */
extern const char BTSM_141[];
/*  "[SM] bt_sm_slave_pairing_proc: Distributed Keys"  */
extern const char BTSM_142[];
/*  "[SM] bt_smp_send"  */
extern const char BTSM_143[];
/*  "[SM] out of memory sending pdu: 0x%02x"  */
extern const char BTSM_144[];
/*  "[SM] bt_smp_proc: handle(0x%04x), role(%d), timer_id(0x%08x)"  */
extern const char BTSM_145[];
/*  "[SM] sm_session failed"  */
extern const char BTSM_146[];
/*  "[SM] sm_session is not null, because security request not first"  */
extern const char BTSM_147[];
/*  "[SM] sm_session busy"  */
extern const char BTSM_148[];
/*  "[SM] wrong state"  */
extern const char BTSM_149[];
/*  "[SM] bt_smp_proc: OOM recover, handle(0x%04x), role(%d)"  */
extern const char BTSM_150[];
/*  "[SM] sm_session cleaned, handle(0x%04x), running_op(0x%08x)"  */
extern const char BTSM_151[];
/*  "[SM] key size error, LESC key size must be 16bytes"  */
extern const char BTSM_152[];
/*  "[SM] wait for BT_SM_EVENT_KEY_DIST,send status = %02x"  */
extern const char BTSM_153[];
/*  "[SM] add connection handle = %02x to wl"  */
extern const char BTSM_154[];
/*  "[SM] not have wl need handle"  */
extern const char BTSM_155[];
/*  "[SM] connection handle = %02x wl security req"  */
extern const char BTSM_156[];
/*  "[SM] connection handle = %02x wl pair req"  */
extern const char BTSM_157[];
/*  "[SM] wl role error = %02x"  */
extern const char BTSM_158[];

/*  "[A2DP] Alloc memory, basic info. size_sig:%d, size_med:%d, size_link_node:%d, size_node:%d"  */
extern const char BTA2DP_001[];
/*  "[A2DP] Alloc memory. type: 0x%02x, size: %d, addr: 0x%08x"  */
extern const char BTA2DP_002[];
/*  "[A2DP] Free memory, node:0x%08x"  */
extern const char BTA2DP_003[];
/*  "[A2DP] bt_a2dp_init()"  */
extern const char BTA2DP_004[];
/*  "[A2DP] A2dp init failed"  */
extern const char BTA2DP_005[];
/*  "[A2DP] update sep()"  */
extern const char BTA2DP_006[];
/*  "[A2DP] SEP update fail. Err: SDP not enabled."  */
extern const char BTA2DP_007[];
/*  "[A2DP] BQB a2dp_connect_stream_REQ, version: 0x%04x, role: 0x%02x"  */
extern const char BTA2DP_008[];
/*  "[A2DP] a2dp_connect_REQ(), state: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_009[];
/*  "[A2DP] a2dp_connect_REQ(), OOM: 0x%08x"  */
extern const char BTA2DP_010[];
/*  "[A2DP] a2dp_connect_REQ(), Init channel Fail, Reason: 0x%08x"  */
extern const char BTA2DP_011[];
/*  "[A2DP] a2dp_connect_REQ(), SDP Begin, status: 0x%08x"  */
extern const char BTA2DP_012[];
/*  "[A2DP] a2dp_disconnect_REQ(), handle: 0x%08x, state: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_013[];
/*  "[A2DP] a2dp_start_REQ(), handle: 0x%08x, state: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_014[];
/*  "[A2DP] a2dp_send_security_REQ(), State Err(IDLE)"  */
extern const char BTA2DP_015[];
/*  "[A2DP] a2dp_suspend_REQ(). State Error, State: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_016[];
/*  "[A2DP] a2dp_set_delay_report_REQ(), State Error. State: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_017[];
/*  "[A2DP] a2dp_send_media_data(), State Err. State: 0x%02x, sub_state: 0x%02x, media_chnl_existed: %d"  */
extern const char BTA2DP_018[];
/*  "[A2DP] a2dp_start_RSP(), State Error. State: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_019[];
/*  "[A2DP] a2dp_suspend_RSP(), State Error. State: 0x%02x, sub_state: 0x%02x"  */
extern const char BTA2DP_020[];
/*  "[A2DP] a2dp_avdtp_cb(), evt:0x%08x, status: 0x%08x"  */
extern const char BTA2DP_021[];
/*  "[A2DP] a2dp_avdtp_cb(), OPEN CNF, cb: 0x%08x, status:0x%08x"  */
extern const char BTA2DP_022[];
/*  "[A2DP] a2dp_avdtp_cb(), STRM IND, handle: 0x%08x, codec: 0x%08x"  */
extern const char BTA2DP_023[];
/*  "[A2DP] a2dp_avdtp_cb(), OOM, Memory available."  */
extern const char BTA2DP_024[];
/*  "[A2DP] a2dp_sdp_cb(), evt_id: %d, user_data: 0x%08x"  */
extern const char BTA2DP_025[];
/*  "[A2DP] a2dp_sdp_cb(), SDP handle query END, handle_num: %d"  */
extern const char BTA2DP_026[];
/*  "[A2DP] a2dp_sdp_cb(), SDP attr query BEGIN, status: 0x%08x"  */
extern const char BTA2DP_027[];
/*  "[A2DP] a2dp_sdp_cb(), SDP attr query END: len: %d"  */
extern const char BTA2DP_028[];
/*  "[A2DP] a2dp_sdp_cb(), VERSION a2dp: 0x%04x, avdtp: 0x%04x"  */
extern const char BTA2DP_029[];
/*  "[A2DP] a2dp_oom_cb(), node:0x%08x"  */
extern const char BTA2DP_030[];
/*  "[A2DP] a2dp_oom_cb(), SDP service Begin, status: 0x%08x"  */
extern const char BTA2DP_031[];
/*  "[A2DP] a2dp_set_aws_cb(), cb: 0x%08x, data:0x%08x"  */
extern const char BTA2DP_032[];
/*  "[A2DP] [rho allow check]channel not exist, OK"  */
extern const char BTA2DP_033[];
/*  "[A2DP] [get rho data len]parameter NULL"  */
extern const char BTA2DP_034[];
/*  "[A2DP] [get rho data len]channel empty"  */
extern const char BTA2DP_035[];
/*  "[A2DP] [get rho data]paramter is NULL "  */
extern const char BTA2DP_036[];
/*  "[A2DP] [get rho data]channel don't exist"  */
extern const char BTA2DP_037[];
/*  "[A2DP] [update rho context]parameter is NULL "  */
extern const char BTA2DP_038[];
/*  "[A2DP] [update rho context]channel empty"  */
extern const char BTA2DP_039[];
/*  "[A2DP] [set rho data]Parameter is NULL"  */
extern const char BTA2DP_040[];
/*  "[A2DP] [A2DP_SDP] sink:%d,source:%d"  */
extern const char BTA2DP_041[];
/*  "[A2DP] [SDP feature] sink:%d,source:%d"  */
extern const char BTA2DP_042[];
/*  "[A2DP] [A2DP_SDP] get a2dp sink record, sink_enable:%d, record: 0x%08x"  */
extern const char BTA2DP_043[];
/*  "[A2DP] [A2DP_SDP] get a2dp src record, source_enable:%d, record: 0x%08x"  */
extern const char BTA2DP_044[];
/*  "[AVDTP] validate codec, type: 0x%02x, valid: %d"  */
extern const char BTA2DP_045[];
/*  "[AVDTP] codec match decide, type:0x%02x, match: %d"  */
extern const char BTA2DP_046[];
/*  "[AVDTP] set audio channel default, link_handle: 0x%04x, stream_handle: 0x%04x"  */
extern const char BTA2DP_047[];
/*  "[AVDTP] New chnl IND, channel:0x%08x."  */
extern const char BTA2DP_048[];
/*  "[AVDTP] handle special event(), EVT: 0x%02x, sig: 0x%08x, req_chnl: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_049[];
/*  "[AVDTP] handle special event(), fatal Err: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_050[];
/*  "[AVDTP] handle special event(), GET CAP IND: sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_051[];
/*  "[AVDTP] build sig cmd(), OOM"  */
extern const char BTA2DP_052[];
/*  "[AVDTP] AVDTP: error, sig_id:%d"  */
extern const char BTA2DP_053[];
/*  "[AVDTP] build sig rsp(), OOM!"  */
extern const char BTA2DP_054[];
/*  "[AVDTP] sig_timeout_cb(), Timer: 0x%08x, channel: 0x%08x."  */
extern const char BTA2DP_055[];
/*  "[AVDTP] media_conn_timeout_cb(), Timer: 0x%08x, channel: 0x%08x,state: %d."  */
extern const char BTA2DP_056[];
/*  "[AVDTP] check_status_for_command(), cur_state: 0x%02x, sub_state: 0x%02x, sig_cmd: 0x%02x, result:0x%02x."  */
extern const char BTA2DP_057[];
/*  "[AVDTP] deregister_sep(), sep_list: 0x%08x"  */
extern const char BTA2DP_058[];
/*  "[AVDTP] avdtp_connnect_REQ(), status: 0x%08x"  */
extern const char BTA2DP_059[];
/*  "[AVDTP] state_origin(), EVT: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_060[];
/*  "[AVDTP] state_origin(), sub_state err, sub_state: 0x%02x, strm: 0x%08x"  */
extern const char BTA2DP_061[];
/*  "[AVDTP] Disc Signal channel: 0x%08x"  */
extern const char BTA2DP_062[];
/*  "[AVDTP] Timer check: sig: 0x%08x, med:0x%08x"  */
extern const char BTA2DP_063[];
/*  "[AVDTP] Med CHNL check: sig_state: 0x%02x, med_state:0x%02x"  */
extern const char BTA2DP_064[];
/*  "[AVDTP] set_delay_report(), Error, role: %d, delay:%d, cur_state: 0x%02x"  */
extern const char BTA2DP_065[];
/*  "[AVDTP] set_delay_report(), cur_state: 0x%02x, origin delay: %d, cur_delay: %d"  */
extern const char BTA2DP_066[];
/*  "[AVDTP] get rmt caps(), sep_id Error, sep_idx: %d"  */
extern const char BTA2DP_067[];
/*  "[AVDTP] Set Config REQ, int_sep: %02d, acp_sep: %02d"  */
extern const char BTA2DP_068[];
/*  "[AVDTP] Stream Start CNF, Err: 0x%02d"  */
extern const char BTA2DP_069[];
/*  "[AVDTP] Close CNF, state: %d, Err: 0x%02d"  */
extern const char BTA2DP_070[];
/*  "[AVDTP] Close IND, state: %d, Err: 0x%02d"  */
extern const char BTA2DP_071[];
/*  "[AVDTP] state_idle(), EVT: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_072[];
/*  "[AVDTP] state_idle(), Discover CNF, sep_num: %d"  */
extern const char BTA2DP_073[];
/*  "[AVDTP] state_idle(), Rmt SPE Err, disconnect signal channel"  */
extern const char BTA2DP_074[];
/*  "[AVDTP] state_idle(), Discover CNF, Rject by remote, Err: 0x%02d"  */
extern const char BTA2DP_075[];
/*  "[AVDTP] state_idle(), Get Cap CNF(Fail), evt: 0x%02x, Err: 0x%02d"  */
extern const char BTA2DP_076[];
/*  "[AVDTP] state_idle(), Get Cap CNF(OK), sep_id: %d, inUse: %d, sep_type: %d, codec_type: %d, sec:%d"  */
extern const char BTA2DP_077[];
/*  "[AVDTP] state_idle(), Get Cap CNF(OK), cur_pri:0x%02x, pri_ret:0x%02x, next_id:0x%02x"  */
extern const char BTA2DP_078[];
/*  "[AVDTP] state_idle(), Get Capability CNF, no supported codec"  */
extern const char BTA2DP_079[];
/*  "[AVDTP] state_idle(), Set Config IND, sep_id: %02d, error_code:0x%02x, sec:%d, delay_report:%d"  */
extern const char BTA2DP_080[];
/*  "[AVDTP] state_idle(), Set Config CNF, Err: %02d"  */
extern const char BTA2DP_081[];
/*  "[AVDTP] state_configured(), EVT: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_082[];
/*  "[AVDTP] state_configured(), Set Config IND, sep_id: %02d, Err:0x%02x"  */
extern const char BTA2DP_083[];
/*  "[AVDTP] state_configured(), conn strm_chnl, strm_chnl: 0x%08x, status: 0x%08x."  */
extern const char BTA2DP_084[];
/*  "[AVDTP] state_configured(), Stream Open CNF, Err: 0x%02d"  */
extern const char BTA2DP_085[];
/*  "[AVDTP] state_open(), EVT: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_086[];
/*  "[AVDTP] state_open(), sub_state err, sub_state: 0x%02x, strm: 0x%08x"  */
extern const char BTA2DP_087[];
/*  "[AVDTP] state_open(), Reconfigure IND, codec match ret: 0x%02x"  */
extern const char BTA2DP_088[];
/*  "[AVDTP] state_open(), Reconfig CNF, Err: 0x%02d"  */
extern const char BTA2DP_089[];
/*  "[AVDTP] state_streaming(), EVT: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_090[];
/*  "[AVDTP] state_streaming(), Stream Suspend IND, Err: 0x%02x"  */
extern const char BTA2DP_091[];
/*  "[AVDTP] state_streaming(), Stream Suspend CNF, Err: 0x%02x"  */
extern const char BTA2DP_092[];
/*  "[AVDTP] send_rsp(), OOM."  */
extern const char BTA2DP_093[];
/*  "[AVDTP] state_closing(), EVT: 0x%02x, sig: 0x%08x, sub_state: 0x%02x"  */
extern const char BTA2DP_094[];
/*  "[AVDTP] channel_validatoin(), strm_chnl invalid, sig_id: 0x%02x"  */
extern const char BTA2DP_095[];
/*  "[AVDTP] send_command(), avdtp_node:0x%08x, sig_id:0x%02x."  */
extern const char BTA2DP_096[];
/*  "[AVDTP] send_command(), chnl invalid, avdtp_node:0x%08x, sig_id:0x%02x"  */
extern const char BTA2DP_097[];
/*  "[AVDTP] send_command(), State Err: 0x%02x."  */
extern const char BTA2DP_098[];
/*  "[AVDTP] send_command(), OOM."  */
extern const char BTA2DP_099[];
/*  "[AVDTP] send_response(), avdtp_node:0x%08x, sig_id:0x%02x, error_code:0x%02x."  */
extern const char BTA2DP_100[];
/*  "[AVDTP] send_response(), chnl invalid, avdtp_node:0x%08x, sig_id:0x%02x"  */
extern const char BTA2DP_101[];
/*  "[AVDTP] send_response(), OOM!"  */
extern const char BTA2DP_102[];
/*  "[AVDTP] send_media_data(), chnl invalid, avdtp_node:0x%08x, med_chnl:0x%08x"  */
extern const char BTA2DP_103[];
/*  "[AVDTP] send_media_data(), avdtp_node:0x%08x, payload_size:0x%02x, sn:0x%02x, ts:0x%02x "  */
extern const char BTA2DP_104[];
/*  "[AVDTP] send_media_data(), OOM!"  */
extern const char BTA2DP_105[];
/*  "[AVDTP] oom_handle_cb(), sig_channel:0x%08x"  */
extern const char BTA2DP_106[];
/*  "[AIRUPDAT] Alloc channel, chnl: 0x%08x\r\n"  */
extern const char BTAIRUPDAT_001[];
/*  "[AIRUPDAT] special link check(), conn: 0x%08x, special: %d\r\n"  */
extern const char BTAIRUPDAT_002[];
/*  "[AIRUPDAT] Invalid handle!\r\n"  */
extern const char BTAIRUPDAT_003[];
/*  "[AIRUPDAT] send data length(%d) > max size(%d)!\r\n"  */
extern const char BTAIRUPDAT_004[];
/*  "[AIRUPDAT] PDU allocated fail!\r\n"  */
extern const char BTAIRUPDAT_005[];
/*  "[AIRUPDAT] Airupdate data cb, Err conn: 0x%08x!"  */
extern const char BTAIRUPDAT_006[];
/*  "[AIRUPDAT] connect status cb, conn:0x%08x, air_ch:0x%08x, result:0x%08x, status:0x%08x\r\n"  */
extern const char BTAIRUPDAT_007[];
/*  "[AIRUPDAT] AIR status cb attached, chnl error(NULL)"  */
extern const char BTAIRUPDAT_008[];
/*  "[AIRUPDAT] bt_airupdate_is_transfer_data, Err conn: 0x%08x!"  */
extern const char BTAIRUPDAT_009[];
/*  "[AVRCP] Wrongly HCI packet format"  */
extern const char BTAVRCP_001[];
/*  "[AVRCP] Receive incompleted packet"  */
extern const char BTAVRCP_002[];
/*  "[AVRCP] Receive invalid AVCTP packet"  */
extern const char BTAVRCP_003[];
/*  "[AVRCP] Receive error transaction label"  */
extern const char BTAVRCP_004[];
/*  "[AVRCP] avrcp init: role:%d browse %d"  */
extern const char BTAVRCP_005[];
/*  "[AVRCP] Receive vendor response w/o comand"  */
extern const char BTAVRCP_006[];
/*  "[AVRCP] Receive vendor response w/o comand"  */
extern const char BTAVRCP_007[];
/*  "[AVRCP] avrcp send abort pdu:%d"  */
extern const char BTAVRCP_008[];
/*  "[AVRCP] avrcp send abort pdu: status:%d"  */
extern const char BTAVRCP_009[];
/*  "[AVRCP] PASS THROUGH result %d, timer_id %d"  */
extern const char BTAVRCP_010[];
/*  "[AVRCP] Pass through command handler: op_id(%x)"  */
extern const char BTAVRCP_011[];
/*  "[AVRCP] Integrity check packet size(%d), op_code(%d), c_or_r(%d)"  */
extern const char BTAVRCP_012[];
/*  "[AVRCP] should init BT_MEMORY_CONTROL_BLOCK_AVRCP_EX or not support browse!"  */
extern const char BTAVRCP_013[];
/*  "[AVRCP] not support browse, should init BT_MEMORY_CONTROL_BLOCK_AVRCP or waste memory!"  */
extern const char BTAVRCP_014[];
/*  "[AVRCP] command timeout(%x)"  */
extern const char BTAVRCP_015[];
/*  "[AVRCP] cancel sdp failed(%d)"  */
extern const char BTAVRCP_016[];
/*  "[AVRCP] RSP DATA integrity check failed"  */
extern const char BTAVRCP_017[];
/*  "[AVRCP] CMD DATA integrity check failed"  */
extern const char BTAVRCP_018[];
/*  "[AVRCP] should init BT_MEMORY_CONTROL_BLOCK_AVRCP_EX or not support browse!"  */
extern const char BTAVRCP_019[];
/*  "[AVRCP] not support browse, should init BT_MEMORY_CONTROL_BLOCK_AVRCP or waste memory!"  */
extern const char BTAVRCP_020[];
/*  "[AVRCP] out of memory"  */
extern const char BTAVRCP_021[];
/*  "[AVRCP] bt_avrcp_browse_connect"  */
extern const char BTAVRCP_022[];
/*  "[AVRCP] remote feature: 0x%04x"  */
extern const char BTAVRCP_023[];
/*  "[AVRCP] init status: %x"  */
extern const char BTAVRCP_024[];
/*  "[AVRCP] bt_avrcp_browse_get_folder_items_response"  */
extern const char BTAVRCP_025[];
/*  "[AVRCP] avrcp browse cb event: %x status: 0x%08x"  */
extern const char BTAVRCP_026[];
/*  "[AVRCP] cancel sdp failed(%d)"  */
extern const char BTAVRCP_027[];
/*  "[AVRCP] did not send command but receive response"  */
extern const char BTAVRCP_028[];
/*  "[AVRCP] rec browse rsp 0x%02x 0x%04x"  */
extern const char BTAVRCP_029[];
/*  "[AVRCP] unexpected response"  */
extern const char BTAVRCP_030[];
/*  "[AVRCP] rec browse cmd 0x%02x"  */
extern const char BTAVRCP_031[];
/*  "[AVRCP] free context for AVRCP browsing"  */
extern const char BTAVRCP_032[];
/*  "[AVRCP] data ind %x %x %x"  */
extern const char BTAVRCP_033[];
/*  "[AVRCP] l2cap cb evt:%x"  */
extern const char BTAVRCP_034[];
/*  "[AVRCP] BROWSE DATA STATUS FAIL!"  */
extern const char BTAVRCP_035[];
/*  "[AVRCP] AVCTP BROWSE FRAGMENT!"  */
extern const char BTAVRCP_036[];
/*  "[AVRCP] get capability  type error(%d)!"  */
extern const char BTAVRCP_037[];
/*  "[AVRCP] register notification type error(%d)!"  */
extern const char BTAVRCP_038[];
/*  "[AVRCP] get app setting handle(0x%08x),attribute_size(%d)"  */
extern const char BTAVRCP_039[];
/*  "[AVRCP] set app setting handle(0x%08x),attribute_size(%d)"  */
extern const char BTAVRCP_040[];
/*  "[AVRCP] get element handle(0x%08x),attribute_size(%d)"  */
extern const char BTAVRCP_041[];
/*  "[AVRCP] register handle(0x%08x),id(%d),interval(%d)"  */
extern const char BTAVRCP_042[];
/*  "[AVRCP] requeset continue handle(0x%08x),pdu_id(%d)"  */
extern const char BTAVRCP_043[];
/*  "[AVRCP] requeset continue handle(0x%08x),pdu_id(%d)"  */
extern const char BTAVRCP_044[];
/*  "[AVRCP] Set Absolute Volume handle(0x%08x), volume(0x%x)"  */
extern const char BTAVRCP_045[];
/*  "[AVRCP] Play Item handle(0x%08x), scope(0x%x)"  */
extern const char BTAVRCP_046[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:%d"  */
extern const char BTAVRCP_047[];
/*  "[AVRCP] vendor dep cmd hdlr: type:BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED: evt id: %d, para_len"  */
extern const char BTAVRCP_048[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_GET_PLAY_STATUS"  */
extern const char BTAVRCP_049[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME: type: %d"  */
extern const char BTAVRCP_050[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_GET_CAPABILITIES: type: %d"  */
extern const char BTAVRCP_051[];
/*  "[AVRCP] VENDOR DEP GET CAPABILITY INVALID PARAM %d, %d"  */
extern const char BTAVRCP_052[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_LIST_APP_SETTING_ATTRIBUTES"  */
extern const char BTAVRCP_053[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES: %d"  */
extern const char BTAVRCP_054[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RSP"  */
extern const char BTAVRCP_055[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:BT_AVRCP_PDU_ID_ABORT_CONTINUING_RSP"  */
extern const char BTAVRCP_056[];
/*  "[AVRCP] vendor dep cmd hdlr: pdu:not supported"  */
extern const char BTAVRCP_057[];
/*  "[AVRCP] avrcp send cap res: len:%d, %d"  */
extern const char BTAVRCP_058[];
/*  "[AVRCP] avrcp send cap res: status:%d"  */
extern const char BTAVRCP_059[];
/*  "[AVRCP] avrcp reg notif res: rsp par len:%d, len:%d, evt id:%d"  */
extern const char BTAVRCP_060[];
/*  "[AVRCP] avrcp reg notif res status: %d"  */
extern const char BTAVRCP_061[];
/*  "[AVRCP] avrcp metadata res: len:%d, limit_exceeds"  */
extern const char BTAVRCP_062[];
/*  "[AVRCP] avrcp metadata res: data len: %d"  */
extern const char BTAVRCP_063[];
/*  "[AVRCP] avrcp metadata res:%d"  */
extern const char BTAVRCP_064[];
/*  "[AVRCP] [get rho data len]parameter NULL "  */
extern const char BTAVRCP_065[];
/*  "[AVRCP] [get rho data len]channel don't exist"  */
extern const char BTAVRCP_066[];
/*  "[AVRCP] [get rho data]paramter is NULL "  */
extern const char BTAVRCP_067[];
/*  "[AVRCP] [get rho data]channel don't exist"  */
extern const char BTAVRCP_068[];
/*  "[AVRCP] [update rho context]parameter is NULL "  */
extern const char BTAVRCP_069[];
/*  "[AVRCP] [iupdate rho context]channel don't exist"  */
extern const char BTAVRCP_070[];
/*  "[AVRCP] [get rho rollback]paramter is NULL "  */
extern const char BTAVRCP_071[];
/*  "[AVRCP] [get rho rollback]channel already exist"  */
extern const char BTAVRCP_072[];
/*  "[AVRCP] [set rho data]Parameter is NULL"  */
extern const char BTAVRCP_073[];
/*  "[AVRCP] [get rho data len]parameter NULL "  */
extern const char BTAVRCP_074[];
/*  "[AVRCP] [rollback partner]channel don't exist"  */
extern const char BTAVRCP_075[];
/*  "[AVRCP] SDP attr OOM"  */
extern const char BTAVRCP_076[];
/*  "[AVRCP] SDP feature 0x%04x 0x%04x"  */
extern const char BTAVRCP_077[];
/*  "[AVRCP] SDP CB 0x%04x"  */
extern const char BTAVRCP_078[];
/*  "[AVRCP] SDP CB 0x%04x"  */
extern const char BTAVRCP_079[];
/*  "[AVRCP] SDP AVRTP 0x%04x 0x%04x"  */
extern const char BTAVRCP_080[];
extern const char BTAVRCP_081[];
extern const char BTAVRCP_082[];
extern const char BTAVRCP_083[];
extern const char BTAVRCP_084[];
extern const char BTAVRCP_085[];
extern const char BTAVRCP_086[];
extern const char BTAVRCP_087[];
extern const char BTAVRCP_088[];
extern const char BTAVRCP_089[];
extern const char BTAVRCP_090[];
extern const char BTAVRCP_091[];
extern const char BTAVRCP_092[];
extern const char BTAVRCP_093[];
extern const char BTAVRCP_094[];
extern const char BTAVRCP_095[];
extern const char BTAVRCP_096[];
/*  "[AWS] fetch clock offset, conn_handle:%02x"  */
extern const char BTAWS_001[];
/*  "[AWS] Alloc Memory, channel %x"  */
extern const char BTAWS_002[];
/*  "[AWS] bt_aws_connect conflict, already connted with curr address"  */
extern const char BTAWS_003[];
/*  "[AWS] bt_aws_connect out of memeory"  */
extern const char BTAWS_004[];
/*  "[AWS] aws send RSP, l2cap mtu not enough"  */
extern const char BTAWS_005[];
/*  "[AWS] aws send CMD, channel:%x, state:%d, moudule_id:%d"  */
extern const char BTAWS_006[];
/*  "[AWS] aws send CMD, l2cap mtu not enough"  */
extern const char BTAWS_007[];
/*  "[AWS] aws notify play(), channel:%x, state:%d, play_time:%d, gap_role:%d"  */
extern const char BTAWS_008[];
/*  "[AWS] aws notify play(), l2cap mtu not enough"  */
extern const char BTAWS_009[];
/*  "[AWS] aws dummy link, conn_handle:%02x"  */
extern const char BTAWS_010[];
/*  "[AWS] aws_l2cap_op_cb, event %x, "  */
extern const char BTAWS_011[];
/*  "[AWS] connection conflict"  */
extern const char BTAWS_012[];
/*  "[AWS] aws_tx_done_cb, sent fail,droped"  */
extern const char BTAWS_013[];
/*  "[AWS] timer 0x%08x expired"  */
extern const char BTAWS_014[];
/*  "[AWS] aws_evt_cb(), timer_id %x, error %x"  */
extern const char BTAWS_015[];
/*  "[AWS] get_offset, status:%x, offset:%x, offset_intra:%x"  */
extern const char BTAWS_016[];
/*  "[AWS] play_time_notify:%d"  */
extern const char BTAWS_017[];
/*  "[AWS] wrong trans_id ignore. trans_id:%d, rmt_trans_id;%d"  */
extern const char BTAWS_018[];
/*  "[AWS] parse cmd, sig_id:%d, trans_id:%d, rmt_trans_id:%d"  */
extern const char BTAWS_019[];
/*  "[AWS] Protocol len error, pkt_len: %d, data_len:%d"  */
extern const char BTAWS_020[];
/*  "[AWS] alloc memory fail"  */
extern const char BTAWS_021[];
/*  "[AWS] alloc memory fail"  */
extern const char BTAWS_022[];
/*  "[AWS] alloc memory fail"  */
extern const char BTAWS_023[];
/*  "[AWS] alloc memory fail"  */
extern const char BTAWS_024[];
/*  "[AWS] AWS_SET_STATE, aws_channel:%x, or_state:%d, curr_state:%d"  */
extern const char BTAWS_025[];
/*  "[AWS] AWS_SET_SUB_STATE, aws_channel:%x, or_state:%d, or_sub_state:%d, curr_state:%d"  */
extern const char BTAWS_026[];
/*  "[AWS] AWS_SET_CURR_OP, aws_channel:%x, curr_op:%d"  */
extern const char BTAWS_027[];
/*  "[AWS] AWS_CLEAR_CURR_OP, aws_channel:%x, curr_op:%d"  */
extern const char BTAWS_028[];
/*  "[AWS] loc_nclk:%x, loc_nclk_intra:%x"  */
extern const char BTAWS_029[];
/*  "[AWS] play_time_notify:%d"  */
extern const char BTAWS_030[];
/*  "[AWS] paser fail,status:%d"  */
extern const char BTAWS_031[];
/*  "[AWS] state_macheine, aws_channel:%x, curr_state:%d, curr_sub_state:%d, event:%d"  */
extern const char BTAWS_032[];
/*  "[AWS_MCE] Alloc channel, chnl: 0x%08x"  */
extern const char BTAWS_033[];
/*  "[AWS_MCE] special link check(), conn: 0x%08x, special: %d"  */
extern const char BTAWS_034[];
/*  "[AWS_MCE] Role Init, role: 0x%02x"  */
extern const char BTAWS_035[];
/*  "[AWS_MCE] AWS MCE set state. Err role:0x%02x"  */
extern const char BTAWS_036[];
/*  "[AWS_MCE] AWS MCE set state. Err aws:0x%08x"  */
extern const char BTAWS_037[];
/*  "[AWS_MCE] AWS MCE set state. Err gap role,aws:0x%08x"  */
extern const char BTAWS_038[];
/*  "[AWS_MCE] AWS MCE set state. aws:0x%08x, state:0x%02x, status:0x%08x"  */
extern const char BTAWS_039[];
/*  "[AWS_MCE] AWS_attach REQ. Err: None role"  */
extern const char BTAWS_040[];
/*  "[AWS_MCE] AWS_attach REQ. conn existed ,gap:0x%08x, channel:0x%08x"  */
extern const char BTAWS_041[];
/*  "[AWS_MCE] AWS attch REQ. Chnl error(NULL)"  */
extern const char BTAWS_042[];
/*  "[AWS_MCE] AWS attch REQ. gap_conn error(NULL)"  */
extern const char BTAWS_043[];
/*  "[AWS_MCE] AWS_attach REQ. gap_conn: 0x%08x, channel:0x%08x, ret:0x%08x"  */
extern const char BTAWS_044[];
/*  "[AWS_MCE] AWS detach REQ. Err, channel:0x%08x, handle:0x%08x"  */
extern const char BTAWS_045[];
/*  "[AWS_MCE] AWS detach REQ. channel: 0x%08x, gap_conn: 0x%08x, status: 0x%08x"  */
extern const char BTAWS_046[];
/*  "[AWS_MCE] Check a2dp playing, ret: %d"  */
extern const char BTAWS_047[];
/*  "[AWS_MCE] Add stream channel, chnl: 0x%04x"  */
extern const char BTAWS_048[];
/*  "[AWS_MCE] Send info. Err, role: 0x%02x, state: 0x%02x"  */
extern const char BTAWS_049[];
/*  "[AWS_MCE] Send info. AWS: 0x%08x, Type: 0x%02x, len: %03d, l2cap_len: %03d."  */
extern const char BTAWS_050[];
/*  "[AWS_MCE] Send info. Err, length exceeded."  */
extern const char BTAWS_051[];
/*  "[AWS_MCE] Send info. OOM!"  */
extern const char BTAWS_052[];
/*  "[AWS_MCE] AWS data cb, Err conn: 0x%08x, role:%d"  */
extern const char BTAWS_053[];
/*  "[AWS_MCE] sco callback, aws:0x%08x, sco: 0x%04x, evt_id:%d"  */
extern const char BTAWS_054[];
/*  "[AWS_MCE] AWS status cb, conn:0x%08x, aws:0x%08x, result:0x%08x, status:0x%08x"  */
extern const char BTAWS_055[];
/*  "[AWS_MCE] AWS status cb attached, chnl error(NULL)"  */
extern const char BTAWS_056[];
/*  "[AWS_MCE] AWS role change cb, conn: 0x%08x, status:0x%08x, org_state:0x%04x, new_state:0x%04x"  */
extern const char BTAWS_057[];
/*  "[AWS_MCE] aws_mce_prepare_role_handover(), Err, role: 0x%02x, state: 0x%02x"  */
extern const char BTAWS_058[];
/*  "[AWS_MCE] aws_mce_event_cb(), expired timer: 0x%08x"  */
extern const char BTAWS_059[];
/*  "[AWS_MCE] mce_event_cb(), timer: 0x%08x, Err: 0x%08x"  */
extern const char BTAWS_060[];
/*  "[AWS_MCE] mce_event_cb(), PREPARE RHO READY, Err: HCI not empty, flag:%d"  */
extern const char BTAWS_061[];
/*  "[AWS_MCE] Get address by handle. Err(NULL) handle: 0x%08x"  */
extern const char BTAWS_062[];
/*  "[AWS_MCE] AWS prepare RHO fail, status 0x%08x, conn: 0x%08x, idx:%02d"  */
extern const char BTAWS_063[];
/*  "[AWS_MCE] time elapsed for rho(ms):%03d" */
extern const char BTAWS_064[];
/*  "[AWS_MCE] RHO get length, module: 0x%02x, len: %d"  */
extern const char BTAWS_065[];
/*  "[HFP] bt_hfp_connect fail, already connted with curr address"  */
extern const char BTHFP_001[];
/*  "[HFP] init_params fail, status:%x"  */
extern const char BTHFP_002[];
/*  "[HFP] hfp_connect, hfp_channel:%x"  */
extern const char BTHFP_003[];
/*  "[HFP] bt_hfp_connect, init_send OOM, channel:%x"  */
extern const char BTHFP_004[];
/*  "[HFP] hfp_disconnect, hfp_channel:%x, state:%d"  */
extern const char BTHFP_005[];
/*  "[HFP] hfp_connect_rsp, hfp_channel:%x"  */
extern const char BTHFP_006[];
/*  "[HFP] hfp_audi_tran, hfp_channel:%x, dir:%d"  */
extern const char BTHFP_007[];
/*  "[HFP] hfp_set_audio_status, hfp_channel:%x, sco_handle:%x, status:%d"  */
extern const char BTHFP_008[];
/*  "[HFP] get_addr, hfp_channel:%x"  */
extern const char BTHFP_009[];
/*  "[HFP] send cmd fail due to oom"  */
extern const char BTHFP_011[];
/*  "[HFP] Can't find the ending char, waiting the next packet."  */
extern const char BTHFP_013[];
/*  "[HFP] Find the 2nd end char in the first 2nd chars."  */
extern const char BTHFP_014[];
/*  "[HFP] Customized CMD TYPE."  */
extern const char BTHFP_015[];
/*  "[HFP] bt_hfp_oom_callback, hfp channel not existed"  */
extern const char BTHFP_016[];
/*  "[HFP] bt_hfp_oom_callback, hfp_channel:%x, oom_state:%d"  */
extern const char BTHFP_017[];
/*  "[HFP] bt_hfp_oom_callback, hfp state:%d"  */
extern const char BTHFP_018[];
/*  "[HFP] bt_hfp_oom_callback, OOM again, hfp_channel:%x"  */
extern const char BTHFP_019[];
/*  "[HFP] SDP callback,hfp_channel:%x, event_id:%d"  */
extern const char BTHFP_020[];
/*  "[HFP] bt_hfp_sdp_callback,attr_query OOM, channel:%x"  */
extern const char BTHFP_021[];
/*  "[HFP] rf_conect,status: %x, hfp_channel:%x"  */
extern const char BTHFP_022[];
/*  "[HFP] parse ag version, channel:%x, parsed_num:%x"  */
extern const char BTHFP_023[];
/*  "[HFP] sdp_callback,init_send OOM, channel:%x"  */
extern const char BTHFP_024[];
/*  "[HFP] sdp_callback,state error:%x"  */
extern const char BTHFP_025[];
/*  "[HFP] RFCOMM callback,event_id:%d"  */
extern const char BTHFP_026[];
/*  "[HFP] RFCOMM callback alloc fail, already connted with curr address"  */
extern const char BTHFP_027[];
/*  "[HFP] RFCOMM callback alloc fail, OOM"  */
extern const char BTHFP_028[];
/*  "[HFP] init_params fail, status:%x"  */
extern const char BTHFP_029[];
/*  "[HFP] Delete node, oom state:0x%x"  */
extern const char BTHFP_030[];
/*  "[HFP] RFCOMM Connected,channel:%x"  */
extern const char BTHFP_031[];
/*  "[HFP] rfcomm connected,SDP query OOM, channel:%x"  */
extern const char BTHFP_032[];
/*  "[HFP] hold rx packet, packet:%x"  */
extern const char BTHFP_033[];
/*  "[HFP] data_ind, rx oom, return"  */
extern const char BTHFP_034[];
/*  "[HFP] sdp_callback,init_send OOM, channel:%x"  */
extern const char BTHFP_035[];
/*  "[HFP] sco callback evt_id:%d"  */
extern const char BTHFP_036[];
/*  "[HFP] sco connected, hfp_channel:%x, sco_handle:%x"  */
extern const char BTHFP_037[];
/*  "[HFP] sco get param, sco_type:%d, codec:%d"  */
extern const char BTHFP_038[];
/*  "[HFP] [RHO]hfp channel is not existed."  */
extern const char BTHFP_039[];
/*  "[HFP] [RHO]hfp RHO len:%d"  */
extern const char BTHFP_040[];
/*  "[HFP] [RHO]hfp get data."  */
extern const char BTHFP_041[];
/*  "[HFP] [RHO]hfp channel don't exist"  */
extern const char BTHFP_042[];
/*  "[HFP] [RHO]ag_version:%d, ag_version:%d, action_state:%d, codec:%d, hfp_state:%d, ind_num:%d"  */
extern const char BTHFP_043[];
/*  "[HFP] [RHO]update hfp context, role:0x%x"  */
extern const char BTHFP_044[];
/*  "[HFP] [RHO]HFP channel:0x%x"  */
extern const char BTHFP_045[];
/*  "[HFP] [RHO]ag_version:%d, ag_version:%d, action_state:%d, codec:%d, hfp_state:%d, ind_num:%d"  */
extern const char BTHFP_046[];
/*  "[HFP] [RHO]sco handle:0x%x"  */
extern const char BTHFP_047[];
/*  "[HFP] [RHO]Update fail due to init hfp channel fail."  */
extern const char BTHFP_048[];
/*  "[HFP] [RHO]Wrong role:0x%x"  */
extern const char BTHFP_049[];
/*  "[HFP] Change service record enable:%d"  */
extern const char BTHFP_050[];
/*  "[HFP] get hfp sdp log"  */
extern const char BTHFP_051[];
/*  "[HFP] bt_hfp_sdp_fail_handle, hfp_channel:0x%x, hfp_state:%d"  */
extern const char BTHFP_052[];
/*  "[HFP] HFP SET STATE, hfp_channel:%x, or_state:%d, curr_state:%d"  */
extern const char BTHFP_053[];
/*  "[HFP] HFP SET STATE, hfp_channel:%x, or_state:%d, curr_state:%d, add:%d"  */
extern const char BTHFP_054[];
/*  "[HFP] bt_hfp_state_handle, OOM, channel:%x"  */
extern const char BTHFP_055[];
/*  "[HFP] init_at_cmd, channel:%x, at_cmd:%x"  */
extern const char BTHFP_056[];
/*  "[HFP] sdp_callback, init_send OOM, channel:0x%x"  */
extern const char BTHFP_058[];
/*  "[HFP] lock acl, channel:0x%x, state:0x%x"  */
extern const char BTHFP_059[];
/*  "[HFP] at handle OOM."  */
extern const char BTHFP_060[];
/*  "[HFP] alloc channel:%x"  */
extern const char BTHFP_061[];
/*  "[HFP] release rx packet, packet:%x"  */
extern const char BTHFP_062[];
/*  "[HFP] sdp_callback, query error: 0x%x."  */
extern const char BTHFP_063[];
/*  "[HFP] [RHO]hfp is allow,hfp_channel:0x%x,hfp_state:%d."  */
extern const char BTHFP_064[];
/*  "[HID] bt_hid_send_hid_control : %x"  */
extern const char BTHID_001[];
/*  "[HID] bt_hid_send : %x"  */
extern const char BTHID_002[];
/*  "[HID] bt_hid_send : %x"  */
extern const char BTHID_003[];
/*  "[HID] bt_hid_send : %x"  */
extern const char BTHID_004[];
/*  "[HID] bt_hid_send : %x"  */
extern const char BTHID_005[];
/*  "[HID] bt_hid_connect_rsp :cntx :%x, accept : %d"  */
extern const char BTHID_006[];
/*  "[HID] bt_hid_connect_rsp : state :%d, sub_state : %d"  */
extern const char BTHID_007[];
/*  "[HID] bt_hid_connect_response: status = %d"  */
extern const char BTHID_008[];
/*  "[HID] internal_connect: status = %d"  */
extern const char BTHID_009[];
/*  "[HID] bt_hid_find_context_by_channel: l2cap_channel = %x, p = %x"  */
extern const char BTHID_010[];
/*  "[HID] bt_hid_find_context_by_channel, p = %x"  */
extern const char BTHID_011[];
/*  "[HID] bt_hid_check_para_is_valid, temp = %x"  */
extern const char BTHID_012[];
/*  "[HID] (bt_hid_paser_data_and_notify)packet[0] = %d, state = %d,length = %d"  */
extern const char BTHID_013[];
/*  "[HID] (bt_hid_paser_data_and_notify)SET_COMMAND: is_valid = %d"  */
extern const char BTHID_014[];
/*  "[HID] (bt_hid_paser_data_and_notify)GET_PROTOCOL: g_protocol = %d"  */
extern const char BTHID_015[];
/*  "[HID] (bt_hid_paser_data_and_notify)GET_REPORT: report_id = %d"  */
extern const char BTHID_016[];
/*  "[HID] (bt_hid_paser_data_and_notify)GET_REPORT: report_type = %d"  */
extern const char BTHID_017[];
/*  "[HID] (bt_hid_state_machine)status = %d, state = %d, event = %d"  */
extern const char BTHID_018[];
/*  "[HID] (bt_hid_state_machine):connect_rsp : status = %d, "  */
extern const char BTHID_019[];
/*  "[HID] (bt_hid_state_machine)CONN_INCPMMING:  state = %d"  */
extern const char BTHID_020[];
/*  "[HID] (bt_hid_state_machine)STATE_DISCONNECTING:  state = %d"  */
extern const char BTHID_021[];
/*  "[HID] (bt_hid_state_machine)STATE_DISCONNECTING:  status = %d"  */
extern const char BTHID_022[];
/*  "[HID] (bt_hid_state_machine)CONN_PENGDING:  state = %d, control_sub_state = %d"  */
extern const char BTHID_023[];
/*  "[HID] (bt_hid_state_machine)CONN_PENGDING:  state = %d, inter_sub_state = %d"  */
extern const char BTHID_024[];
/*  "[HID] (bt_hid_l2cap_callback)l2cap channel:0x%08x,event:%d"  */
extern const char BTHID_025[];
/*  "[HID] (bt_hid_init):init ctrl : status = %d"  */
extern const char BTHID_026[];
/*  "[HID] (bt_hid_init):init inter : status = %d"  */
extern const char BTHID_027[];
/*  "[HID] (bt_hid_init): status = %d"  */
extern const char BTHID_028[];
/*  "[HID] (bt_hid_l2cap_context_op_callback) event = %d"  */
extern const char BTHID_029[];
/*  "[HID] (bt_hid_l2cap_context_op_callback) alloc = %x"  */
extern const char BTHID_030[];
/*  "[HID] (bt_hid_l2cap_context_op_callback) alloc interrupt channel : cntx = %x"  */
extern const char BTHID_031[];
/*  "[HID] (bt_hid_l2cap_context_op_callback) alloc interrupt channel: inter_channel = %x"  */
extern const char BTHID_032[];
/*  "[HID] (bt_hid_l2cap_context_op_callback) free: cntx = %x"  */
extern const char BTHID_033[];
/*  "[HID] (bt_hid_internal_connect) status= %d"  */
extern const char BTHID_034[];
/*  "[HID] RHO"  */
extern const char BTHID_035[];
extern const char BTHID_036[];
extern const char BTHID_037[];
extern const char BTHID_038[];
extern const char BTHID_039[];
extern const char BTHID_040[];
extern const char BTHID_041[];
extern const char BTHID_042[];
extern const char BTHID_043[];
extern const char BTHID_044[];
extern const char BTHID_045[];
extern const char BTHID_046[];
extern const char BTHID_047[];
extern const char BTHID_048[];

/*  "[HSP] bt_hsp_connect already connected"  */
extern const char BTHSP_001[];
/*  "[HSP] bt_hsp_connect allocate channel fail"  */
extern const char BTHSP_002[];
/*  "[HSP] bt_hsp_connect sdp query OOM handle: %x"  */
extern const char BTHSP_003[];
/*  "[HSP] bt_hsp_disconnect, hsp_channel:%x"  */
extern const char BTHSP_004[];
/*  "[HSP] bt_hsp_disconnect, hsp_channel:%x, state:%d"  */
extern const char BTHSP_005[];
/*  "[HSP] bt_hsp_disconnect return %x"  */
extern const char BTHSP_006[];
/*  "[HSP] hsp_set_audio_status, hsp_channel:%x, sco_handle:%x, status:%d"  */
extern const char BTHSP_007[];
/*  "[HSP] bt_hsp_audio_disconnect(%x)"  */
extern const char BTHSP_008[];
/*  "[HSP] hsp_connect_rsp, hsp_channel:%x"  */
extern const char BTHSP_009[];
/*  "[HSP] bt_rfcomm_hsp_callback evt:0x%x."  */
extern const char BTHSP_012[];
/*  "[HSP] send press key command OOM, channel:%x"  */
extern const char BTHSP_013[];
/*  "[HSP] Not credit."  */
extern const char BTHSP_014[];
/*  "[HSP] cancle sdp callback failed. status:0x%x"  */
extern const char BTHSP_015[];
/*  "[HSP] cancle sdp callback success."  */
extern const char BTHSP_016[];
/*  "[HSP] rx packet, packet:%x, handle %x"  */
extern const char BTHSP_017[];
/*  "[HSP] send press key command OOM, channel:%x"  */
extern const char BTHSP_018[];
/*  "[HSP] already connted with curr address"  */
extern const char BTHSP_019[];
/*  "[HSP] RFCOMM callback alloc fail, OOM"  */
extern const char BTHSP_020[];
/*  "[HSP] Delete node, oom state:0x%x"  */
extern const char BTHSP_021[];
/*  "[HSP] sco callback evt_id:%d"  */
extern const char BTHSP_022[];
/*  "[HSP] Not connected."  */
extern const char BTHSP_023[];
/*  "[HSP] sco connected, hsp_channel:%x, sco_handle:%x"  */
extern const char BTHSP_024[];
/*  "[HSP] sco get param"  */
extern const char BTHSP_025[];
/*  "[HSP] sco get param, sco_type:%d"  */
extern const char BTHSP_026[];
/*  "[HSP] bt_hsp_oom_callback, hsp_channel:%x, oom_state:%d"  */
extern const char BTHSP_027[];
/*  "[HSP] bt_hsp_oom_callback, OOM again, hsp_channel:%x"  */
extern const char BTHSP_028[];
/*  "[HSP] [RHO]hsp channel is not existed."  */
extern const char BTHSP_029[];
/*  "[HSP] [RHO]hsp RHO len:%d"  */
extern const char BTHSP_030[];
/*  "[HSP] [RHO]hsp get data."  */
extern const char BTHSP_031[];
/*  "[HSP] [RHO]hsp channel don't exist"  */
extern const char BTHSP_032[];
/*  "[HSP] [RHO]hsp_state:0x%x,"  */
extern const char BTHSP_033[];
/*  "[HSP] [RHO]update hsp context, role:0x%x"  */
extern const char BTHSP_034[];
/*  "[HSP] [RHO]HSP channel:0x%x"  */
extern const char BTHSP_035[];
/*  "[HSP] [RHO]hsp_state:0x%x"  */
extern const char BTHSP_036[];
/*  "[HSP] [RHO]sco handle:0x%x"  */
extern const char BTHSP_037[];
/*  "[HSP] [RHO]Update fail due to init hsp channel fail."  */
extern const char BTHSP_038[];
/*  "[HSP] [RHO]Wrong role:0x%x"  */
extern const char BTHSP_039[];
/*  "[HSP] Change HSP service record enable:%d"  */
extern const char BTHSP_040[];
/*  "[HSP] get hsp sdp log"  */
extern const char BTHSP_041[];
/*  "[HSP] bt_hsp_sdp_callback(user_data %x, event %x"  */
extern const char BTHSP_042[];
/*  "[HSP] execute bt_hsp_sdp_attr_query OOM: %x"  */
extern const char BTHSP_043[];
/*  "[HSP] execute bt_hsp_sdp_attr_query error %x"  */
extern const char BTHSP_044[];
/*  "[HSP] bt_hsp_sdp_handle_query count=0"  */
extern const char BTHSP_045[];
/*  "[HSP] exec bt_rfcomm_connect error:%x"  */
extern const char BTHSP_046[];
/*  "[HSP] bt_hsp_sdp_handle_query error"  */
extern const char BTHSP_047[];
/*  "[HSP] bt_hsp_sdp_callback event_id == BT_SDPC_EVENT_QUERY_ERROR"  */
extern const char BTHSP_048[];
/*  "[HSP] bt_hsp_connect_fail_handler, hsp_channel:0x%x, hsp_state:%d"  */
extern const char BTHSP_049[];
/*  "[GOEP] bt_goepc_parse_auth_challenge, nonce len = %d\n"  */
extern const char BTGOEP_001[];
/*  "[GOEP] bt_goepc_parse_auth_challenge, options = %x\n"  */
extern const char BTGOEP_002[];
/*  "[GOEP] bt_goepc_parse_packet, offset = %d\n"  */
extern const char BTGOEP_003[];
/*  "[GOEP] bt_goepc_parse_packet, packet_len = %d, data_len = %d, rsp = %x\n"  */
extern const char BTGOEP_004[];
/*  "[GOEP] bt_goepc_parse_packet, mru = %d\n"  */
extern const char BTGOEP_005[];
/*  "[GOEP] bt_goepc_parse_packet, parse packet fail\n"  */
extern const char BTGOEP_006[];
/*  "[GOEP] bt_goepc_parse_packet, parse packet fail\n"  */
extern const char BTGOEP_007[];
/*  "[GOEP] bt_goepc_timeout_callback, timeout = %d\n"  */
extern const char BTGOEP_008[];
/*  "[GOEP] bt_goepc_timeout_callback, wrong sub state = %d\n"  */
extern const char BTGOEP_009[];
/*  "[GOEP] bt_goepc_timeout_callback, state = %d\n"  */
extern const char BTGOEP_010[];
/*  "[GOEP] bt_goepc_rfcomm_callback: event = %x"  */
extern const char BTGOEP_011[];
/*  "[GOEP] BT_RFCOMM_EVENT_CONNECTED"  */
extern const char BTGOEP_012[];
/*  "[GOEP] BT_RFCOMM_EVENT_DATA_IND"  */
extern const char BTGOEP_013[];
/*  "[GOEP] Cancel timer fail!!!"  */
extern const char BTGOEP_014[];
/*  "[GOEP] BT_RFCOMM_EVENT_SEND_IND: state = %d, sub_state = %d"  */
extern const char BTGOEP_015[];
/*  "[GOEP] BT_RFCOMM_EVENT_DATA_IND"  */
extern const char BTGOEP_016[];
/*  "[GOEP] Cancel timer fail!!!"  */
extern const char BTGOEP_017[];
/*  "[GOEP] BT_RFCOMM_EVENT_DISCONNECTED,  sub state = %d\n"  */
extern const char BTGOEP_018[];
/*  "[GOEP] BT_RFCOMM_EVENT_DISCONNECTED, wrong state = %d\n"  */
extern const char BTGOEP_019[];
/*  "[GOEP] bt_goepc_init = %x, state = %d\n"  */
extern const char BTGOEP_020[];
/*  "[GOEP] bt_goepc_connect: status = %x\n"  */
extern const char BTGOEP_021[];
/*  "[GOEP] bt_goepc_pull: OOM\n"  */
extern const char BTGOEP_022[];
/*  "[GOEP] bt_goepc_rsp_disconnect,  client = %x\n"  */
extern const char BTGOEP_023[];
/*  "[GOEP] bt_goepc_rsp_disconnect,  sub state = %d\n"  */
extern const char BTGOEP_024[];
/*  "[GOEP] bt_goepc_rsp_disconnect,  packet_len = %d\n"  */
extern const char BTGOEP_025[];
/*  "[GOEP] bt_goepc_rsp_disconnect,  status = %d\n"  */
extern const char BTGOEP_026[];
/*  "[GOEP] bt_goepc_oom_callback, client = 0x%08x, ptr = %x\n"  */
extern const char BTGOEP_027[];
/*  "[GOEP] bt_goepc_rsp_disconnect,  status = %d\n"  */
extern const char BTGOEP_028[];
/* "[GOEP] bearer send failed: 0x%08x" */
extern const char BTGOEP_029[];
/* "[GOEP] goep connect with auth: handle(0x%08x), auth(0x%08x), status(0x%08x)." */
extern const char BTGOEP_030[];
/* "[GOEP] goep connect rsp: handle(0x%08x), state(%d), sub_state(%d), rsp(%d), auth(0x%08x)." */
extern const char BTGOEP_031[];
/* "[GOEP] goep disconnect: handle(0x%08x), state(%d), sub_state(%d)." */
extern const char BTGOEP_032[];
/* "[GOEP] send pull request: handle(0x%08x), state(%d), sub_state(%d), srm_state(%d), srmp_state(%d)." */
extern const char BTGOEP_033[];
/* "[GOEP] send put request: handle(0x%08x), state(%d), sub_state(%d), srm_state(%d), srmp_state(%d)." */
extern const char BTGOEP_034[];
/* "[GOEP] send put response: handle(0x%08x), state(%d), sub_state(%d), srm_state(%d), srmp_state(%d)." */
extern const char BTGOEP_035[];
/* "[GOEP] send setpath: handle(0x%08x), state(%d), sub_state(%d)." */
extern const char BTGOEP_036[];
/* "[GOEP] send abort: handle(0x%08x), state(%d), sub_state(%d)." */
extern const char BTGOEP_037[];
/* "[GOEP] send abort response: handle(0x%08x), state(%d), sub_state(%d)." */
extern const char BTGOEP_038[];
/* "[GOEP] goep over l2cap init: handle:0x%08x, psm:0x%04x, addr:%02x-%02x-%02x-%02x-%02x-%02x" */
extern const char BTGOEP_039[];
/* "[GOEP] goep already exist." */
extern const char BTGOEP_040[];
/* "[GOEP] recv l2cap conn ind: handle:0x%08x, status:0x%08x" */
extern const char BTGOEP_041[];
/* "[GOEP] l2cap connected: handle:0x%08x, status:0x%08x, goep state:%d" */
extern const char BTGOEP_042[];
/* "[GOEP] goep connect fail: status:0x%08x" */
extern const char BTGOEP_043[];
/* "[GOEP] l2cap disconnected: handle:0x%08x, status:0x%08x, goep state:%d, sub state:%d" */
extern const char BTGOEP_044[];
/* "[GOEP] l2cap disconnected: goep wrong state:%d" */
extern const char BTGOEP_045[];
/* "[GOEP] l2cap op callback allocate: event:0x%02x, psm:0x%04x, callback:0x%08x" */
extern const char BTGOEP_046[];
/* "[GOEP] allocate success: handle:0x%08x" */
extern const char BTGOEP_047[];
/* "[GOEP] no resource, please config the buffer." */
extern const char BTGOEP_048[];
/* "[GOEP] l2cap op callback free: handle:0x%08x" */
extern const char BTGOEP_049[];
/* "[GOEP] notify srm: op:%d, srm state:%d, srmp state:%d, rsp:%d" */
extern const char BTGOEP_050[];
/*  "[PBAPC] bt_pbapc_init: status = %d"  */
extern const char BTPBAPC_001[];
/*  "[PBAPC] bt_pbapc_sdp: status = %d"  */
extern const char BTPBAPC_002[];
/*  "[PBAPC] GET_MISSED_CALL_NUMBER: status = %d"  */
extern const char BTPBAPC_003[];
/*  "[PBAPC] GET_MISSED_CALL: status = %d"  */
extern const char BTPBAPC_004[];
/*  "[PBAPC] caller_name_by_number: len = %d, num = %c"  */
extern const char BTPBAPC_005[];
/*  "[PBAPC] caller_name_by_number: status = %d"  */
extern const char BTPBAPC_006[];
/*  "[PBAPC] bt_pbapc_disconnect: state = %d"  */
extern const char BTPBAPC_007[];
/*  "[PBAPC] GET_MISSED_CALL: status = %d"  */
extern const char BTPBAPC_008[];
/*  "[PBAPC] GET_MISSED_CALL_NUMBER: status = %d"  */
extern const char BTPBAPC_009[];
/*  "[PBAPC] GET_MISSED_CALL: status = %d"  */
extern const char BTPBAPC_010[];
/*  "[PBAPC] pullphonebook: status = %d\n"  */
extern const char BTPBAPC_011[];
/*  "[PBAPC] pullvcardlisting: status = %d\n"  */
extern const char BTPBAPC_012[];
/*  "[PBAPC] pullphonebook: status = %d\n"  */
extern const char BTPBAPC_013[];
/*  "[PBAPC] goepc_callback: event = %d\n"  */
extern const char BTPBAPC_014[];
/*  "[PBAPC] goepc_callback: connect ,handle = 0x%x, status = %d\n"  */
extern const char BTPBAPC_015[];
/*  "[PBAPC] goepc_callback: status = %d, disconn, handle = 0x%x\n"  */
extern const char BTPBAPC_016[];
/*  "[PBAPC] goepc_callback:disconn,ind_handle = 0x%x\n"  */
extern const char BTPBAPC_017[];
/*  "[PBAPC] original pbap data len: %d, node count: %d"  */
extern const char BTPBAPC_018[];
/*  "[PBAPC] all data length: %d"  */
extern const char BTPBAPC_019[];
/*  "[PBAPC] oom, try again."  */
extern const char BTPBAPC_020[];
/*  "[PBAPC] copy data done. g_get_vcard: %d, state: %d"  */
extern const char BTPBAPC_021[];
/*  "[PBAPC] paser_headersk-get_msh:len = %d, num = %c\n"  */
extern const char BTPBAPC_022[];
/*  "[PBAPC] paser_headersk-get_msh:len = %d, num = %c\n"  */
extern const char BTPBAPC_023[];
/*  "[PBAPC] paser_headersk-get_pb:len = %d, num = %c\n"  */
extern const char BTPBAPC_024[];
/*  "[PBAPC] paser_headersk:length = %d, app_param = %c\n"  */
extern const char BTPBAPC_025[];
/*  "[PBAPC] paser_headersk:result = %d, length = %d\n"  */
extern const char BTPBAPC_026[];
/*  "[PBAPC] paser_headersk-get_msh:len = %d, num = %c\n"  */
extern const char BTPBAPC_027[];
/*  "[PBAPC] paser_headersk-get_msh:len = %d, num = %c\n"  */
extern const char BTPBAPC_028[];
/*  "[PBAPC] paser_headersk-get_pb:len = %d, num = %c\n"  */
extern const char BTPBAPC_029[];
/*  "[PBAPC] get_tel: len = %d"  */
extern const char BTPBAPC_030[];
/*  "[PBAPC] data is full:result = %d"  */
extern const char BTPBAPC_031[];
/*  "[PBAPC] bt_pbapc_sdp_oom_callback: handle = 0x%08x\n"  */
extern const char BTPBAPC_032[];
/*  "[PBAPC] bt_pbapc_sdp_oom_callback: addr[0] = 0x%02x, addr[1]= 0x%02x, addr[1]= 0x%02x, addr[2] =0x%02x\n"  */
extern const char BTPBAPC_033[];
/*  "[PBAPC] sdp_callback : state = %d"  */
extern const char BTPBAPC_034[];
/*  "[PBAPC] sdp_callback : result = %d"  */
extern const char BTPBAPC_035[];
/*  "[PBAPC] sdp_attribute : result = %d"  */
extern const char BTPBAPC_036[];
/*  "[PBAPC] cancel sdp: ret = %d"  */
extern const char BTPBAPC_037[];
/*  "[PBAPC] cancel sdp fail"  */
extern const char BTPBAPC_038[];
/*  "[MAPC] Out of memory! ";  */
extern const char BTMAPC_001[];
/*  "[MAPC] _goepc_callback: event = 0x%02x ";  */
extern const char BTMAPC_002[];
/*  "[MAPC] sdp_callback: event_id = %d ";  */
extern const char BTMAPC_003[];
/*  "[MAPC] Cannot find sdp record for instance:%d ";  */
extern const char BTMAPC_004[];
/*  "[MAPC] bt_mapc_paser_headers header_id[0x%02x] header_len[%d] sub_state[%d] state[%d] ";  */
extern const char BTMAPC_005[];
/*  "[MAPC] BT_GOEP_HI_APP_PARAM ";  */
extern const char BTMAPC_006[];
/*  "[MAPC] BT_GOEP_HI_BODY length[%d] ";  */
extern const char BTMAPC_007[];
/*  "[MAPC] BT_GOEP_HI_END_OF_BODY length[%d] ";  */
extern const char BTMAPC_008[];
/*  "[MAPC] OOM length[%d] ";  */
extern const char BTMAPC_009[];
/*  "[MAPC] bt_mapc_get_parser_buffer_length total_len[%d] hci_pkt_num[%d] hci_pkt_offset[%d] real_parser_len[%d] ";  */
extern const char BTMAPC_010[];
/*  "[MAPC] Cannot allocate"*/
extern const char BTMAPC_011[];
/*  "[MAPC] Invalid instance"*/
extern const char BTMAPC_012[];
/*  "[MAPC] PSM[0x%04x] instance_id[%d]"*/
extern const char BTMAPC_013[];
/*  "[MAPC] channel_number[%d] instance_id[%d]"*/
extern const char BTMAPC_014[];
/*  "[MAPC] Incorrect state [%d]"*/
extern const char BTMAPC_015[];
/*  "[MAPC] SetNotificationRegistration: status[0x%08x]"*/
extern const char BTMAPC_016[];
/*  "[MAPC] SetFolder: back[%d] status[0x%08x] name_len[%d]"*/
extern const char BTMAPC_017[];
/*  "[MAPC] GetFolderListing: status[0x%08x]"*/
extern const char BTMAPC_018[];
/*  "[MAPC] GetMessageListing: status[0x%08x], type[0x%x]"*/
extern const char BTMAPC_019[];
/*  "[MAPC] GetMessage: status[0x%08x]"*/
extern const char BTMAPC_020[];
/*  "[MAPC] Invalid message status[%d]"*/
extern const char BTMAPC_021[];
/*  "[MAPC] SetMessageStatus: status[0x%08x]"*/
extern const char BTMAPC_022[];
/*  "[MAPC] UpdateInbox: status[0x%08x]"*/
extern const char BTMAPC_023[];
/*  "[MAPC] bt_mapc_write_buffer: content length = %d"*/
extern const char BTMAPC_024[];
/*  "[MAPC] SetMessageStatus: status[0x%08x] charset[%d] buffer_remain[%d]"*/
extern const char BTMAPC_025[];
/*  "[MAPC] SetNotificationFilter: status[0x%08x]"*/
extern const char BTMAPC_026[];
/*  "[MAPC] GetMasInstanceInformation: status[0x%08x]"*/
extern const char BTMAPC_027[];
/*  "[MAPC] bt_mapc_set_hci_pkt_offset hci_pkt_offset[%d]"*/
extern const char BTMAPC_028[];
/*  "[MAPC] bt_mapc_set_hci_pkt_offset hci_pkt_offset[%d]"*/
extern const char BTMAPC_029[];
/*  "[MAPC] evt->op[0x%x]"*/
extern const char BTMAPC_030[];
/*  "[MAPC] op[0x%x] SRM[%d] SRMP[%d] isGOEPv2[%d] srm_enabled[%d]"*/
extern const char BTMAPC_031[];
/*  "[MAPC] body_len[%d]"*/
extern const char BTMAPC_032[];
/*  "[MAPC] rsp[0x%x] operation.type[%d]"*/
extern const char BTMAPC_033[];
/*  "[MAPC] BT_GOEP_EVENT_START SRMP[%d] srmp_reponse_state[%d]"*/
extern const char BTMAPC_034[];
/*  "[MAPC] SRM_ENABLE msg_offset[%d] body_len[%d]"*/
extern const char BTMAPC_035[];
/*  "[MAPC] bt_mapc_get_parser_buffer_length l2cap new_packet bearer[%d] len[%d]"*/
extern const char BTMAPC_036[];
/*  "[MAPC] MAP get cur_len[%d] cur_offset[%d] real_len[%d] hci_pkt_idx[%d]"*/
extern const char BTMAPC_037[];
/*  "[MAPC] bt_mapc_release_all_hci_data"*/
extern const char BTMAPC_038[];
/*  "[MAPC] bt_mapc_parser [%d] does not exist, end parse"*/
extern const char BTMAPC_039[];
/*  "[MAPC] bt_mapc_parser NOT find [%d], wait for the next pkt"*/
extern const char BTMAPC_040[];
/*  "[MAPC] bt_mapc_parser start_value[0x%x] max_count[%d] current_cnt[%d]bt_mapc_parser start_value[0x%x] max_count[%d] current_cnt[%d]"*/
extern const char BTMAPC_041[];
/*  "[MAPC] bt_mapc_parser folder_list[0x%X] list_count[%d]"*/
extern const char BTMAPC_042[];
/*  "[MAPC] bt_mapc_parser folder/file name[0x%x][0x%x] type[%d] length[%d]"*/
extern const char BTMAPC_043[];
/*  "[MAPC] bt_mapc_parser count +[%d] stage[%d] token_pos[0x%x]"*/
extern const char BTMAPC_044[];
/*  "[MAPC] bt_mapc_parser value[0x%x] max_count[%d] current_cnt[%d]"*/
extern const char BTMAPC_045[];
/*  "[MAPC] bt_mapc_parser message_list[0x%X] list_count[%d]"*/
extern const char BTMAPC_046[];
/* "[MAPC] bt_mapc_parser stage[%d] data[0x%x][0x%x] length[%d]"*/
extern const char BTMAPC_047[];
/*  "[MAPC] bt_mapc_parser NOT find required data[%d]"*/
extern const char BTMAPC_048[];
/*  "[MAPC] bt_mapc_parser NOT exist sender name"*/
extern const char BTMAPC_049[];
/*  "[MAPC] bt_mapc_parser sender addr[0x%x][0x%x] length[%d] token_pos_end[0x%x]"*/
extern const char BTMAPC_050[];
/*  "[MAPC] bt_mapc_parser NOT exist sender addr"*/
extern const char BTMAPC_051[];
/*  "[MAPC] bt_mapc_parser stage[%d] data[%d]"*/
extern const char BTMAPC_052[];
/*  "[MAPC] bt_mapc_parser NOT find required msg type, wait for the next pkt token_pos[0x%X][0x%x] buffer[0x%X][0x%x]"*/
extern const char BTMAPC_053[];
/*  "[MAPC] bt_mapc_parser count +[%d] stage[%d] token_pos[0x%x]"*/
extern const char BTMAPC_054[];
/*  "[MAPC] MAPC benv_ptr[0x%X]value[0x%x]"*/
extern const char BTMAPC_055[];
/*  "[MAPC] bt_mapc_parser token_pos[0x%X] stage[%x] ptr[0x%X] name[0x%x][0x%x] length[%d]"*/
extern const char BTMAPC_056[];
/*  "[MAPC] MAPC ORIGINATOR wait complete vcard 0x%X 0x%X"*/
extern const char BTMAPC_057[];
/*  "[MAPC] MAPC bt_mapc_parser recipient number[0x%x][0x%x] length[%d]"*/
extern const char BTMAPC_058[];
/*  "[MAPC] MAPC RECIPIENT wait complete vcard 0x%X 0x%X"*/
extern const char BTMAPC_059[];
/*  "[MAPC] bt_mapc_parser NO language, go to the next stage"*/
extern const char BTMAPC_060[];
/*  "[MAPC] bt_mapc_parser NO language, wait for the next pkt"*/
extern const char BTMAPC_061[];
/*  "[MAPC] bt_mapc_parser content[0x%x][0x%x] length[%d]"*/
extern const char BTMAPC_062[];
/*  "[MAPC] bt_mapc_parser stage[%d] token_pos[0x%x]"*/
extern const char BTMAPC_063[];
/*  "[MAPC] bt_mapc_parser event_report_version[%d]"*/
extern const char BTMAPC_064[];
/*  "[MAPC] bt_mapc_parser event report version does NOT exist, goto the next stage"*/
extern const char BTMAPC_065[];
/*  "[MAPC] bt_mapc_parser event report version, wait for the next pkt"*/
extern const char BTMAPC_066[];
/*  "[MAPC] bt_mapc_parser stage[%d] data[0x%x]"*/
extern const char BTMAPC_067[];
/*  "[MAPC] bt_mapc_parser stage[%d] data[0x%x][0x%x] length[%d]"*/
extern const char BTMAPC_068[];
/*  "[MAPC] bt_mapc_copy_all_header_data"*/
extern const char BTMAPC_069[];
/*  "[MAPC] MAPC copy start_value[0x%x] cur_len[%d] offset[%d] hci_packet[0x%X]"*/
extern const char BTMAPC_070[];
/*  "[MAPC] MAPC last pkt start_value[0x%x] last_pkt_len[%d] offset[%d]"*/
extern const char BTMAPC_071[];
/*  "[MAPC] MAPC parser buffer: 0[0x%x] 1[0x%x] 2[0x%x] 3[0x%x] [0x%x] [0x%x] [%d]"*/
extern const char BTMAPC_072[];
/*  "[MAPC] bt_mapc_release_hci_pkt_by_offset"*/
extern const char BTMAPC_073[];
/*  "[MAPC] bt_mapc_release_hci_pkt_by_offset offset[%d] cur_len[%d] pkt_offset[%d]"*/
extern const char BTMAPC_074[];
/*  "[MAPC] bt_mapc_release_hci_pkt_by_offset new pkt len[%d] hci_pkt_offse[%d]"*/
extern const char BTMAPC_075[];
/*  "[MAPC] MAPC bt_mapc_callback_evt"*/
extern const char BTMAPC_076[];
/*  "[MAPC] MAPC bt_mapc_allocate_evt_memory stage[%d]"*/
extern const char BTMAPC_077[];
/*  "[MAPC] bt_mapc_get_parser_buffer_length[%d]"*/
extern const char BTMAPC_078[];
/*  "[MAPC] MAPC stage[%d] new_stage[%d]"*/
extern const char BTMAPC_079[];
/*  "[MAPC] bt_mapc_get_sub_state[%d]"*/
extern const char BTMAPC_080[];
/*  "[MAPC] MAPC header_len[0]"*/
extern const char BTMAPC_081[];
/*  "[MAPC] SRM[%d] srm_reponse_state[%d] srmp_reponse_state[%d] param[%d]"*/
extern const char BTMAPC_082[];
/*  "[MAPC] MAPC bt_mapc_parser_find_token type[%d] fail"*/
extern const char BTMAPC_083[];
/*  "[MAPC] MAPC bt_mapc_parser_find_token type[%d] type_len[%d]"*/
extern const char BTMAPC_084[];
/*  "[MAPC] MAPC bt_mapc_parser_find_token_bmsg fail type[%d]"*/
extern const char BTMAPC_085[];
/*  "[MAPC] MAPC bt_mapc_parser_find_token_bmsg fail not complete"*/
extern const char BTMAPC_086[];
/*  "[MAPC] MAPC bt_mapc_parser_get_vcard already allocate mem stage[%d]"*/
extern const char BTMAPC_087[];
/*  "[MAPC] MAPC bt_mapc_parser_get_vcard found name start_ptr[0x%X][0x%x] end_ptr[0x%X][0x%x]"*/
extern const char BTMAPC_088[];
/*  "[MAPC] MASINSTANCEID: ret[%x] attr_value[%d] attr_value_len[%d]"*/
extern const char BTMAPC_089[];
/*  "[MAPC] SEARCH other service handle[0x%08x] next_index[%d]"*/
extern const char BTMAPC_090[];
/*  "[MAPC] cancel sdp: result[0x%08x]"*/
extern const char BTMAPC_091[];
/*  "[SPP] bt_rfcomm_spp_callback, event:0x%08x"  */
extern const char BTSPP_001[];
/*  "[SPP] disconnect handle:0x%08x, result::0x%x "  */
extern const char BTSPP_002[];
/*  "[SPP] connect response handle:0x%08x, accept:%d, result:0x%x"  */
extern const char BTSPP_003[];
/*  "[SPP] hold data offset:%d"  */
extern const char BTSPP_004[];
/*  "[SPP] release data offset:%d"  */
extern const char BTSPP_005[];
/*  "[SPP] hold data ext offset:%d"  */
extern const char BTSPP_006[];
/*  "[SPP] release data ext offset:%d"  */
extern const char BTSPP_007[];
/*  "[SPP] connection(0x%08x), state in---->(0x%x), event(0x%x)"  */
extern const char BTSPP_008[];
/*  "[SPP] connection(0x%08x),state out---->(0x%x), event(0x%x)"  */
extern const char BTSPP_009[];
/*  "[SPP] Received sdp query cnf event in disconnected state."  */
extern const char BTSPP_010[];
/*  "[SPP] Unexception event!!!connection:0x%08x, event:0x%x"  */
extern const char BTSPP_011[];
/*  "[SPP] rfcomm disconnected result:0x%x"  */
extern const char BTSPP_012[];
/*  "[SPP] cancle sdp callback failed. status:0x%x"  */
extern const char BTSPP_013[];
/*  "[SPP] cancle sdp callback success."  */
extern const char BTSPP_014[];
/*  "[SPP] Unexception event!!!connection:0x%08x, event:0x%x"  */
extern const char BTSPP_015[];
/*  "[SPP] Unexception event!!!connection:0x%08x, event:0x%x"  */
extern const char BTSPP_016[];
/*  "[SPP] No TX credit!"  */
extern const char BTSPP_017[];
/*  "[SPP] send data length(%d) > max size(%d)"  */
extern const char BTSPP_018[];
/*  "[SPP] TX Buffer not enough!"  */
extern const char BTSPP_019[];
/*  "[SPP] notify reason:%d"  */
extern const char BTSPP_020[];
/*  "[SPP] Unexception event!!!connection:0x%08x, event:0x%x"  */
extern const char BTSPP_021[];
/*  "[SPP] Unexception event!!!connection:0x%08x, event:0x%x"  */
extern const char BTSPP_022[];
/*  "[SPP] connection:0x%08x"  */
extern const char BTSPP_023[];
/*  "[SPP] oom notify send status:0x%x"  */
extern const char BTSPP_024[];
/*  "[SPP] connection:0x%08x"  */
extern const char BTSPP_025[];
/*  "[SPP] oom connect status:0x%x"  */
extern const char BTSPP_026[];
/*  "[SPP] connection:0x%08x, result:%d"  */
extern const char BTSPP_027[];
/*  "[SPP] sdp callback, event_id:0x%08x"  */
extern const char BTSPP_028[];
/*  "[SPP] sdp server search, uuid128:0x%x"  */
extern const char BTSPP_029[];
/*  "[SPP] spp allocate connection."  */
extern const char BTSPP_030[];
/*  "[SPP] allocate, Out of Memory"  */
extern const char BTSPP_031[];
/*  "[SPP] allocate, channel:0x%08x"  */
extern const char BTSPP_032[];
/*  "[SPP] free connection(0x%08x)"  */
extern const char BTSPP_033[];
/*  "[SPP] oom trigger, connection(0x%08x), callback:(0x%08x)"  */
extern const char BTSPP_034[];
/*  "[SPP] oom handle callback(0x%08x)"  */
extern const char BTSPP_035[];
/*  "[SPP] oom stop, connection(0x%08x), callback(0x%08x)"  */
extern const char BTSPP_036[];
/*  "[SPP][RHO] is allowed, connection:0x%x state:0x%x"  */
extern const char BTSPP_037[];
/*  "[SPP][RHO] is allowed:0x%x"  */
extern const char BTSPP_038[];
/*  "[SPP][RHO] get data length:0x%x"  */
extern const char BTSPP_039[];
/*  "[SPP][RHO] get data, no SPP connection exist!"  */
extern const char BTSPP_040[];
/*  "[SPP][RHO] get data, header:0x%x(connections_num:%d)"  */
extern const char BTSPP_041[];
/*  "[SPP][RHO] get data, rho_connection:0x%x(state:0x%x flags:0x%x)"  */
extern const char BTSPP_042[];
/*  "[SPP][RHO] update context, Agent->Partner success"  */
extern const char BTSPP_043[];
/*  "[SPP][RHO] update context, connection:0x%x"  */
extern const char BTSPP_044[];
/*  "[SPP][RHO] update context, Partner->Agent success"  */
extern const char BTSPP_045[];
/*  "[SPP][RHO] update context, connection:0x%x(state:0x%x flags:0x%x)"  */
extern const char BTSPP_046[];
/*  "[SPP][RHO] update context, allocate connection FAILED!"  */
extern const char BTSPP_047[];
/*  "[SPP][RHO] update context, unknown AWS role!"  */
extern const char BTSPP_048[];
/*  "[SPP][RHO] update context, SPP status:0x%x"  */
extern const char BTSPP_049[];
/*  "[SPP] get handle by local server id:0x%x, handle:0x%x"  */
extern const char BTSPP_050[];
/*  "[SPP] get tx buffer total size = %02x, length = %02x"  */
extern const char BTSPP_051[];
/*  "[COMMON] [A2DP] not loaded."  */
extern const char BTCOMMON_001[];
/*  "[COMMON] alloc rx buf from external(), BT_STATUS_BUFFER_USED"  */
extern const char BTCOMMON_002[];
/*  "[COMMON] [get_clk]handle:0x%04x, nclk:0x%08x, intra:0x%04x, off_n:0x%08x, off_intra:0x%04x"  */
extern const char BTCOMMON_003[];
/*  "[MM] [MMSTATE]ADDR[0x%x]STATE[%d]SIZE[%d]"  */
extern const char BTMM_001[];
/*  "[MM] Buffer type Tx OOM TEST!"  */
extern const char BTMM_002[];
/*  "[MM] Buffer type Rx OOM TEST!"  */
extern const char BTMM_003[];
/*  "[MM] [OOM]%p(%d) alloc Tx memory %d fails"  */
extern const char BTMM_004[];
/*  "[MM] [OOM]%p(%d) alloc Rx memory %d fails"  */
extern const char BTMM_005[];
/*  "[MM] Buffer type[Tx] request size[%d] out of memory!"  */
extern const char BTMM_006[];
/*  "[MM] Buffer type[Rx] request size[%d] out of memory!"  */
extern const char BTMM_007[];
/*  "[MM] bt_mm_free_packet_internal(ptr %x, info %x) %p(%d), *%x = %x"  */
extern const char BTMM_008[];
/*  "[MM] bt_mm_append_packet(des %x, info %x, des_offset %x, src_length %x)"  */
extern const char BTMM_009[];
/*  "[MM] Buffer type[%x] allocate[%x] %p@%d"  */
extern const char BTMM_010[];
/*  "[MM] *****************************************************************"  */
extern const char BTMM_011[];
/*  "[MM] Tx packet:buffer size[%d] start addr[0x%08x] search addr[0x%08x] number of allocated packet[0x%x]"  */
extern const char BTMM_012[];
/*  "[MM] Rx packet:buffer size[%d] start addr[0x%08x] search addr[0x%08x] number of allocated packet[0x%x]"  */
extern const char BTMM_013[];
/*  "[MM] ================================================================"  */
extern const char BTMM_014[];
/*  "[MM] CB Type [%d] number[%d] allocated out[%d]"  */
extern const char BTMM_015[];
/*  "[MM] ================================================================"  */
extern const char BTMM_016[];
/*  "[MM] CB Type [%d]: free num [%d]"  */
extern const char BTMM_017[];
/*  "[MM] *****************************************************************"  */
extern const char BTMM_018[];
/*  "[MM] Buffer type Tx OOM TEST [%d]"  */
extern const char BTMM_019[];
/*  "[MM] Buffer type Rx OOM TEST [%d]"  */
extern const char BTMM_020[];
/*  "[MM] Buffer type Timer OOM TEST [%d]"  */
extern const char BTMM_021[];
/*  "[MM] Buffer type Connection OOM TEST [%d]"  */
extern const char BTMM_022[];
/*  "[TIMER] timer expired!!!!, timer id == 0x%08x"  */
extern const char BTTIMER_001[];
/*  "[TIMER] bt_timer_rho_get_list_size, size is %d\r\n"  */
extern const char BTTIMER_002[];
/*  "[TIMER] bt_timer_rho_get_list_data, timer_id 0x%4x, timer_ms 0x%4x\r\n"  */
extern const char BTTIMER_003[];
/*  "[TIMER] bt_timer_rho_set_data done, timer_id is 0x%4x\r\n"  */
extern const char BTTIMER_004[];
/*  "[TIMER] bt_timer_rho_get_data_length, length is %d\r\n"  */
extern const char BTTIMER_005[];
/*  "[TIMER] bt_timer_rho_update_agent_context, status is 0x%4x\r\n"  */
extern const char BTTIMER_006[];
/*  "[TIMER] bt_timer_rho_update_agent_context: re-start the first timer\r\n"  */
extern const char BTTIMER_007[];
/*  "[TIMER] bt_timer_rho_update_partner_context, timer_id 0x%4x, timer_ms 0x%4x\r\n"  */
extern const char BTTIMER_008[];
/*  "[TIMER] bt_timer_rho_update_partner_context: no data!\r\n"  */
extern const char BTTIMER_009[];
/*  "[TIMER] !!!!!bt_timer_test_insert"  */
extern const char BTTIMER_010[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_011[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_012[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_013[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_014[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_015[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_016[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_017[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_018[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_019[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_020[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_021[];
/*  "[TIMER] timer %d, %d ms, inserted = %d"  */
extern const char BTTIMER_022[];
/*  "[TIMER] timer %d = %d ms"  */
extern const char BTTIMER_023[];
/*  "[TIMER] !!!!!bt_timer_test_cancel"  */
extern const char BTTIMER_024[];
/*  "[TIMER] timer %d, canceled = %d"  */
extern const char BTTIMER_025[];
/*  "[TIMER] timer %d, canceled = %d"  */
extern const char BTTIMER_026[];
/*  "[TIMER] timer %d, canceled = %d"  */
extern const char BTTIMER_027[];
/*  "[TIMER] timer %d, canceled = %d"  */
extern const char BTTIMER_028[];
/*  "[TIMER] timer %d = %d ms"  */
extern const char BTTIMER_029[];
/*  "[TIMER] !!!!!bt_timer_test_timer_handler"  */
extern const char BTTIMER_030[];
/*  "[OTP_COC] *****************************************************************"  */
/*  "[OTP_COC] bt_otp_coc_rx_proc connHdle:0x%02x, buff:0x%02x, length:%d, channel:0x%02x"  */
extern const char BTOTP_001[];
/*  "[OTP_COC] bt_otp_coc_l2cap_evt_cb event 0x%02x"  */
extern const char BTOTP_002[];
/*  "[OTP_COC] BT_L2CAP_LE_CBFCM_CHANNEL_CONNECTED: channel:0x%02x, active:%d, cid:0x%02x"  */
extern const char BTOTP_003[];
/*  "[OTP_COC] BT_L2CAP_LE_CBFCM_CONNECTION_REQUEST_FAILED: reason:0x%02x"  */
extern const char BTOTP_004[];
/*  "[OTP_COC] BT_L2CAP_LE_CBFCM_CHANNEL_DISCONNECTED:channel:0x%02x, reason:0x%02x, cid:0x%02x, handle:0x%02x"  */
extern const char BTOTP_005[];
/*  "[OTP_COC] bt_otp_coc_init malloc fail"  */
extern const char BTOTP_006[];
/*  "[OTP_COC] bt_otp_coc_init success"  */
extern const char BTOTP_007[];
/*  "[OTP] bt_otp_read_state_machine state[%d] evt[0x%02x] p_data[0x%02x] status[0x%02x]"  */
extern const char BTOTP_010[];
/*  "[OTP] otp char type:%d val_hdl:0x%x desc_hdl:0x%x"  */
extern const char BTOTP_011[];
/*  "[OTP] BT_OTP_COC_CONNECTED_IND active:%d cid:0x%04x"  */
extern const char BTOTP_012[];
/*  "[OTP] BT_OTP_COC_CONNECTION_REQUEST_FAILED_IND reason:%d"  */
extern const char BTOTP_013[];
/*  "[OTP] BT_OTP_COC_DISCONNECTED_IND reason:%d evt_cid:0x%04x cid:0x%04x"  */
extern const char BTOTP_014[];
/*  "[OTP] BT_OTP_COC_DATA_IN_IND cid:0x%04x len:%d data:0x%02x 0x%02x 0x%02x 0x%02x"  */
extern const char BTOTP_015[];
/*  "[OTP] ble_otp_discovery_parse_charc charcNum: %d, maxCharcCount: %d, charcRealCount: %d"  */
extern const char BTOTP_016[];
/*  "[OTP] DiscoveryCharcParse charcIdx:%d, UUID:0x%04x charc_type: %d"  */
extern const char BTOTP_017[];
/*  "[OTP] DiscoveryCharcParse charcCountFound: %d"  */
extern const char BTOTP_018[];
/*  "[OTP] ble_otp_event_handler evt:0x%04x status:0x%x"  */
extern const char BTOTP_019[];
/*  "[OTP] ble_otp_discovery_complete"  */
extern const char BTOTP_01A[];
/*  "[OTP] ble_otp_read_object_complete state[%d] status[0x%02x]"  */
extern const char BTOTP_01B[];

/*  "[ULL] *****************************************************************"  */
/*  "[ULL] bt_ull_le_event_callback, timer 0x%08x expired"  */
extern const char BTULL_001[];
/*  "[ULL] bt_ull_le_event_callback, event 0x%08x complete"  */
extern const char BTULL_002[];
/*  "[ULL] bt_ull_le_event_callback, disconnect air cis because of low resource, handle: 0x%x" */
extern const char BTULL_003[];
/*  "[ULL] bt_ull_le_event_callback, not find air cis info, handle : 0x%x" */
extern const char BTULL_004[];
/*  "[ULL] bt_ull_le_air_cis_conn_proc, timer 0x%08x expired" */
extern const char BTULL_005[];
/*  "[ULL] bt_ull_le_air_cis_conn_proc, event 0x%08x complete" */
extern const char BTULL_006[];
/*  "[ULL] bt_ull_le_disconnect_air_cis, not find air cis connction info, handle: 0x%x" */
extern const char BTULL_007[];
/*  "[ULL] bt_ull_le_remove_air_iso_data_path, not find air cis info, handle: 0x%x" */
extern const char BTULL_008[];
/*  "[ULL] bt_ull_le_setup_air_iso_data_path, not find air cis info, handle: 0x%x" */
extern const char BTULL_009[];
/*  "[ULL] bt_ull_le_set_air_cig_parameters, out of memory" */
extern const char BTULL_010[];
/*  "[ULL] bt_ull_le_create_air_cis, out of memory" */
extern const char BTULL_011[];
/*  "[ULL] bt_ull_le_unmute_air_cis, not find air cis info, handle: 0x%x" */
extern const char BTULL_012[];
/*  "[ULL] bt_ull_le_activiate_uplink, not find air cis info, handle: 0x%x" */
extern const char BTULL_013[];
/*  "[ULL] bt_ull_le_set_air_params_table, out of memory" */
extern const char BTULL_014[];
/*  "[ULL] bt_ull_le_send_data, cid is invalid" */
extern const char BTULL_015[];
BT_EXTERN_C_END

#endif /* __BT_LOG_H__ */
