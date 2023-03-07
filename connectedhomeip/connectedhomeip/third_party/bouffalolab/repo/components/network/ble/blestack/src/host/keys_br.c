/* keys_br.c - Bluetooth BR/EDR key handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <string.h>
#include <atomic.h>
#include <util.h>

#include <bluetooth.h>
#include <conn.h>
#include <hci_host.h>
#include <settings.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_KEYS)
#define LOG_MODULE_NAME bt_keys_br
#include "log.h"

#include "hci_core.h"
#include "settings.h"
#include "keys.h"

static struct bt_keys_link_key key_pool[CONFIG_BT_MAX_PAIRED];

#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
static uint32_t aging_counter_val;
static struct bt_keys_link_key *last_keys_updated;
#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */

#if defined(BFLB_BT_LINK_KEYS_STORE)
#define LINK_KEY "link_key"
#define MAX_LINK_KEY_NUMBER 	8
#define LINK_KEY_TAG	0xa5a6

static int bt_key_set(const char *key, void *value, int len, int sync)
{
	int ret = 0;

	#if defined(CONFIG_BT_SETTINGS)
	ret = bt_settings_set_bin(key,value,len);
	#endif

	return ret;
}

static int bt_key_get(const char *key, void *buffer, int *buffer_len)
{
	int ret = 0;

	#if defined(CONFIG_BT_SETTINGS)
	ret = bt_settings_get_bin(key,buffer,*buffer_len,NULL);
	#endif

	return ret;
}

static int bt_key_del(const char *key)
{
	int ret = 0;

	#if defined(CONFIG_BT_SETTINGS)
	ret = settings_delete(key);
	#endif

	return ret;
}

struct linkkey_ops {
	int (*key_get)(const char *key, void *buffer, int *buffer_len);
	int (*key_set)(const char *key, void *value, int len, int sync);
	int (*key_del)(const char *key);
};

struct bt_link_keys {
	struct bt_keys_link_key link_key[MAX_LINK_KEY_NUMBER];
	bool used[MAX_LINK_KEY_NUMBER];
	uint8_t last_key_idx;
	uint8_t key_idx;
	uint16_t tag;
};

typedef struct bt_keys_list_t{
	struct bt_link_keys keys;
	struct linkkey_ops *ops;
}bt_keys_list;

static bt_keys_list key_list;

int bt_keys_init(void)
{
	static struct linkkey_ops ops = {
		.key_get = bt_key_get,
		.key_set = bt_key_set,
		.key_del = bt_key_del,
	};
	struct bt_link_keys keys = {0};
	int len = sizeof(struct bt_link_keys);

	memset(&key_list.keys,0,sizeof(struct bt_link_keys));
	for(int i=0;i<MAX_LINK_KEY_NUMBER;i++){
		memset(&key_list.keys.link_key[i],0,MAX_LINK_KEY_NUMBER);
		memset(&key_list.keys.used[i],0,MAX_LINK_KEY_NUMBER);
	}

	key_list.keys.tag = LINK_KEY_TAG;
	key_list.ops = &ops;

	if(key_list.ops && key_list.ops->key_get){
		int ret = key_list.ops->key_get(LINK_KEY,&keys,&len);
		if(ret){
			BT_ERR("Failed to get key");
			return -1;
		}
	}

	if(keys.tag == LINK_KEY_TAG){
		memcpy(&key_list.keys,&keys,sizeof(struct bt_link_keys));
	}

	return 0;
}

static int bt_keys_link_key_set(const struct bt_keys_link_key *key)
{
	uint8_t exist = 0;
	int ret = 0;

	for(int i=0;i<sizeof(key_list.keys.link_key);i++){
		if(!memcmp(&key_list.keys.link_key[i],key,sizeof(*key))){
			exist = 1;
			break;
		}
	}

	if(!exist){
		BT_ERR("Key not exist\r\n");
		return -1;
	}

	if(key_list.ops && key_list.ops->key_set){
		key_list.keys.tag = LINK_KEY_TAG;
		ret = key_list.ops->key_set(LINK_KEY,&key_list.keys,sizeof(struct bt_link_keys),1);
	}

	return ret;
}

static struct bt_keys_link_key* bt_keys_link_key_get(const bt_addr_t *addr)
{
	struct bt_keys_link_key *link_key;
	bt_addr_t *bd_addr = NULL;
	int len = sizeof(struct bt_link_keys);
	int ret = 0;

	for(int i=0; i<MAX_LINK_KEY_NUMBER;i++){
		bd_addr = &key_list.keys.link_key[i].addr;
		if(!bt_addr_cmp(bd_addr,addr)){
			key_list.keys.used[i] = 1;
			link_key = &key_list.keys.link_key[i];
			goto last;
		}
	}

	for(int i=0; i<MAX_LINK_KEY_NUMBER;i++){
		if(!key_list.keys.used[i]){
			key_list.keys.used[i] = 1;
			link_key = &key_list.keys.link_key[i];
			goto last;
		}
	}

	if(key_list.keys.key_idx >= MAX_LINK_KEY_NUMBER){
		key_list.keys.key_idx = 0;
	}

	link_key = &key_list.keys.link_key[key_list.keys.key_idx];
	key_list.keys.key_idx++;

last:
	bt_addr_copy(&link_key->addr, addr);
	return link_key;
}

static int bt_keys_link_key_del(const struct bt_keys_link_key *key)
{
	for(int i=0;i<MAX_LINK_KEY_NUMBER;i++){
		if(!memcmp(key,&key_list.keys.link_key[i],sizeof(struct bt_keys_link_key))){
			memset(&key_list.keys.link_key[i],0,sizeof(struct bt_keys_link_key));
			key_list.keys.used[i] = 0;
			if(key_list.ops && key_list.ops->key_set){
				key_list.keys.tag = LINK_KEY_TAG;
				key_list.ops->key_set(LINK_KEY,&key_list.keys,sizeof(struct bt_link_keys),1);
			}
		}
	}

	return 0;
}

#endif
struct bt_keys_link_key *bt_keys_find_link_key(const bt_addr_t *addr)
{
	#if defined(BFLB_BT_LINK_KEYS_STORE)
    struct bt_keys_link_key *link_key;
    bt_addr_t *bd_addr = NULL;
    
    for(int i=0; i<MAX_LINK_KEY_NUMBER;i++){
        bd_addr = &key_list.keys.link_key[i].addr;
        if(key_list.keys.used[i] && !bt_addr_cmp(bd_addr,addr)){
            link_key = &key_list.keys.link_key[i];
            return link_key;
        }
    }
    return NULL;
	#else

	struct bt_keys_link_key *key = NULL;
	int i,len = sizeof(struct bt_keys_link_key);

	BT_DBG("%s", bt_addr_str(addr));

	for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
		key = &key_pool[i];

		if (!bt_addr_cmp(&key->addr, addr)) {
			return key;
		}
	}

	return NULL;
	#endif
}

struct bt_keys_link_key *bt_keys_get_link_key(const bt_addr_t *addr)
{
	struct bt_keys_link_key *key;

	key = bt_keys_link_key_get(addr);
	if (key) {
		return key;
	}

	key = bt_keys_link_key_get(BT_ADDR_ANY);
	if (key) {
		bt_addr_copy(&key->addr, addr);
		BT_DBG("created %p for %s", key, bt_addr_str(addr));
		return key;
	}

	BT_DBG("unable to create keys for %s", bt_addr_str(addr));

	return NULL;
}

void bt_keys_link_key_clear(struct bt_keys_link_key *link_key)
{

	BT_DBG("%s", bt_addr_str(&link_key->addr));
	#if defined(BFLB_BT_LINK_KEYS_STORE)
	bt_keys_link_key_del(link_key);
	#endif
	(void)memset(link_key, 0, sizeof(*link_key));
}

void bt_keys_link_key_clear_addr(const bt_addr_t *addr)
{
	int i;
	struct bt_keys_link_key *key;

	if (!addr) {
		for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
			key = &key_pool[i];
			bt_keys_link_key_clear(key);
		}
		#if defined(BFLB_BT_LINK_KEYS_STORE)
		for (i = 0; i < ARRAY_SIZE(key_list.keys.link_key); i++){
			key = &key_list.keys.link_key[i];
			bt_keys_link_key_clear(key);
		}
		#endif
		return;
	}

	key = bt_keys_find_link_key(addr);
	if (key) {
		bt_keys_link_key_clear(key);
	}
}

void bt_keys_link_key_store(struct bt_keys_link_key *link_key)
{
	#if defined(BFLB_BT_LINK_KEYS_STORE)
	bt_keys_link_key_set(link_key);
	#endif
}
#if !defined(BFLB_BLE)
#if defined(CONFIG_BT_SETTINGS)

static int link_key_set(const char *name, size_t len_rd,
			settings_read_cb read_cb, void *cb_arg)
{
	int err;
	ssize_t len;
	bt_addr_le_t le_addr;
	struct bt_keys_link_key *link_key;
	char val[BT_KEYS_LINK_KEY_STORAGE_LEN];

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -EINVAL;
	}

	len = read_cb(cb_arg, val, sizeof(val));
	if (len < 0) {
		BT_ERR("Failed to read value (err %zu)", len);
		return -EINVAL;
	}

	BT_DBG("name %s val %s", log_strdup(name),
	       len ? bt_hex(val, sizeof(val)) : "(null)");

	err = bt_settings_decode_key(name, &le_addr);
	if (err) {
		BT_ERR("Unable to decode address %s", name);
		return -EINVAL;
	}

	link_key = bt_keys_get_link_key(&le_addr.a);
	if (len != BT_KEYS_LINK_KEY_STORAGE_LEN) {
		if (link_key) {
			bt_keys_link_key_clear(link_key);
			BT_DBG("Clear keys for %s", bt_addr_le_str(&le_addr));
		} else {
			BT_WARN("Unable to find deleted keys for %s",
				bt_addr_le_str(&le_addr));
		}

		return 0;
	}

	memcpy(link_key->storage_start, val, len);
	BT_DBG("Successfully restored link key for %s",
	       bt_addr_le_str(&le_addr));
#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
	if (aging_counter_val < link_key->aging_counter) {
		aging_counter_val = link_key->aging_counter;
	}
#endif  /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */

	return 0;
}

static int link_key_commit(void)
{
	return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(bt_link_key, "bt/link_key", NULL, link_key_set,
			       link_key_commit, NULL);

void bt_keys_link_key_update_usage(const bt_addr_t *addr)
{
	struct bt_keys_link_key *link_key = bt_keys_find_link_key(addr);

	if (!link_key) {
		return;
	}

	if (last_keys_updated == link_key) {
		return;
	}

	link_key->aging_counter = ++aging_counter_val;
	last_keys_updated = link_key;

	BT_DBG("Aging counter for %s is set to %u", bt_addr_str(addr),
	       link_key->aging_counter);

	if (IS_ENABLED(CONFIG_BT_KEYS_SAVE_AGING_COUNTER_ON_PAIRING)) {
		bt_keys_link_key_store(link_key);
	}
}

#endif
#endif
