
// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BL_WPA3_I_H
#define BL_WPA3_I_H

#ifdef CONFIG_WPA3_SAE

u8 *wpa3_build_sae_msg(u8 *bssid, u8 *mac, u8 *passphrase, u32 sae_msg_type, size_t *sae_msg_len);
int wpa3_parse_sae_msg(u8 *buf, size_t len, u32 sae_msg_type, u16 status);

void bl_wpa3_free_sae_data(void);
#endif

#endif /* BL_WPA3_I_H */
