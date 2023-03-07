/*
 * BSS load update
 * Copyright (c) 2014, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef BSS_LOAD_UPDATE_H
#define BSS_LOAD_UPDATE_H

#ifdef CONFIG_BSS_LOAD
int bss_load_update_init(struct hostapd_data *hapd);
void bss_load_update_deinit(struct hostapd_data *hapd);
#endif /* CONFIG_BSS_LOAD */


#endif /* BSS_LOAD_UPDATE_H */
