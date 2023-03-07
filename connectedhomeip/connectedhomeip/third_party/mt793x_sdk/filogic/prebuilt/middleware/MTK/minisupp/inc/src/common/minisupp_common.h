#ifndef MINISUPP_COMMON_H
#define MINISUPP_COMMON_H

/* rtos porting */
#define WPA_NONCE_LEN 32
#define WPA_KEK_MAX_LEN 64

struct wpa_ie_data {
	int proto;
	int pairwise_cipher;
	int has_pairwise;
	int group_cipher;
	int has_group;
	int key_mgmt;
	int capabilities;
	size_t num_pmkid;
	const u8 *pmkid;
	int mgmt_group_cipher;
};

#endif /* MINISUPP_COMMON_H */
