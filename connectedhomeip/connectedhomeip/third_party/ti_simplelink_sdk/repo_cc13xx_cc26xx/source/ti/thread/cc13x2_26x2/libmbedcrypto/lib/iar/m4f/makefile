SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

OBJECTS = third_party_mbedtls_repo_library_aes.obj third_party_mbedtls_repo_library_aesni.obj third_party_mbedtls_repo_library_arc4.obj third_party_mbedtls_repo_library_aria.obj third_party_mbedtls_repo_library_asn1parse.obj third_party_mbedtls_repo_library_asn1write.obj third_party_mbedtls_repo_library_base64.obj third_party_mbedtls_repo_library_bignum.obj third_party_mbedtls_repo_library_blowfish.obj third_party_mbedtls_repo_library_camellia.obj third_party_mbedtls_repo_library_ccm.obj third_party_mbedtls_repo_library_certs.obj third_party_mbedtls_repo_library_chacha20.obj third_party_mbedtls_repo_library_chachapoly.obj third_party_mbedtls_repo_library_cipher.obj third_party_mbedtls_repo_library_cipher_wrap.obj third_party_mbedtls_repo_library_cmac.obj third_party_mbedtls_repo_library_ctr_drbg.obj third_party_mbedtls_repo_library_debug.obj third_party_mbedtls_repo_library_des.obj third_party_mbedtls_repo_library_dhm.obj third_party_mbedtls_repo_library_ecdh.obj third_party_mbedtls_repo_library_ecdsa.obj third_party_mbedtls_repo_library_ecjpake.obj third_party_mbedtls_repo_library_ecp.obj third_party_mbedtls_repo_library_ecp_curves.obj third_party_mbedtls_repo_library_entropy.obj third_party_mbedtls_repo_library_entropy_poll.obj third_party_mbedtls_repo_library_error.obj third_party_mbedtls_repo_library_gcm.obj third_party_mbedtls_repo_library_havege.obj third_party_mbedtls_repo_library_hkdf.obj third_party_mbedtls_repo_library_hmac_drbg.obj third_party_mbedtls_repo_library_md.obj third_party_mbedtls_repo_library_md2.obj third_party_mbedtls_repo_library_md4.obj third_party_mbedtls_repo_library_md5.obj third_party_mbedtls_repo_library_md_wrap.obj third_party_mbedtls_repo_library_memory_buffer_alloc.obj third_party_mbedtls_repo_library_net_sockets.obj third_party_mbedtls_repo_library_nist_kw.obj third_party_mbedtls_repo_library_oid.obj third_party_mbedtls_repo_library_padlock.obj third_party_mbedtls_repo_library_pem.obj third_party_mbedtls_repo_library_pk.obj third_party_mbedtls_repo_library_pk_wrap.obj third_party_mbedtls_repo_library_pkcs11.obj third_party_mbedtls_repo_library_pkcs12.obj third_party_mbedtls_repo_library_pkcs5.obj third_party_mbedtls_repo_library_pkparse.obj third_party_mbedtls_repo_library_pkwrite.obj third_party_mbedtls_repo_library_platform.obj third_party_mbedtls_repo_library_platform_util.obj third_party_mbedtls_repo_library_poly1305.obj third_party_mbedtls_repo_library_ripemd160.obj third_party_mbedtls_repo_library_rsa.obj third_party_mbedtls_repo_library_rsa_internal.obj third_party_mbedtls_repo_library_sha1.obj third_party_mbedtls_repo_library_sha256.obj third_party_mbedtls_repo_library_sha512.obj third_party_mbedtls_repo_library_ssl_cache.obj third_party_mbedtls_repo_library_ssl_ciphersuites.obj third_party_mbedtls_repo_library_ssl_cli.obj third_party_mbedtls_repo_library_ssl_cookie.obj third_party_mbedtls_repo_library_ssl_srv.obj third_party_mbedtls_repo_library_ssl_ticket.obj third_party_mbedtls_repo_library_ssl_tls.obj third_party_mbedtls_repo_library_threading.obj third_party_mbedtls_repo_library_timing.obj third_party_mbedtls_repo_library_version.obj third_party_mbedtls_repo_library_version_features.obj third_party_mbedtls_repo_library_x509.obj third_party_mbedtls_repo_library_x509_create.obj third_party_mbedtls_repo_library_x509_crl.obj third_party_mbedtls_repo_library_x509_crt.obj third_party_mbedtls_repo_library_x509_csr.obj third_party_mbedtls_repo_library_x509write_crt.obj third_party_mbedtls_repo_library_x509write_csr.obj third_party_mbedtls_repo_library_xtea.obj
NAME = libmbedcrypto

CFLAGS = -I../../.. \
    -Ohz \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/examples/platforms \
    -I$SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050 \
    -DMBEDTLS_CONFIG_FILE="mbedtls-config-cc13x2_26x2.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libmbedcrypto/config \
    --cpu=Cortex-M4F \
    --vla

all: $(NAME).a

third_party_mbedtls_repo_library_aes.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/aes.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_aesni.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/aesni.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_arc4.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/arc4.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_aria.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/aria.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_asn1parse.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/asn1parse.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_asn1write.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/asn1write.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_base64.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/base64.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_bignum.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/bignum.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_blowfish.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/blowfish.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_camellia.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/camellia.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ccm.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ccm.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_certs.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/certs.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_chacha20.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/chacha20.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_chachapoly.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/chachapoly.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_cipher.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/cipher.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_cipher_wrap.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/cipher_wrap.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_cmac.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/cmac.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ctr_drbg.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ctr_drbg.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_debug.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/debug.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_des.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/des.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_dhm.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/dhm.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ecdh.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ecdh.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ecdsa.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ecdsa.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ecjpake.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ecjpake.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ecp.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ecp.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ecp_curves.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ecp_curves.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_entropy.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/entropy.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_entropy_poll.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/entropy_poll.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_error.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/error.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_gcm.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/gcm.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_havege.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/havege.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_hkdf.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/hkdf.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_hmac_drbg.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/hmac_drbg.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_md.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/md.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_md2.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/md2.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_md4.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/md4.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_md5.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/md5.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_md_wrap.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/md_wrap.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_memory_buffer_alloc.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/memory_buffer_alloc.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_net_sockets.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/net_sockets.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_nist_kw.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/nist_kw.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_oid.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/oid.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_padlock.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/padlock.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pem.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pem.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pk.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pk.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pk_wrap.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pk_wrap.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pkcs11.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pkcs11.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pkcs12.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pkcs12.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pkcs5.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pkcs5.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pkparse.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pkparse.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_pkwrite.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/pkwrite.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_platform.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/platform.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_platform_util.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/platform_util.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_poly1305.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/poly1305.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ripemd160.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ripemd160.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_rsa.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/rsa.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_rsa_internal.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/rsa_internal.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_sha1.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/sha1.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_sha256.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/sha256.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_sha512.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/sha512.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_cache.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_cache.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_ciphersuites.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_ciphersuites.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_cli.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_cli.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_cookie.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_cookie.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_srv.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_srv.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_ticket.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_ticket.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_ssl_tls.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/ssl_tls.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_threading.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/threading.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_timing.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/timing.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_version.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/version.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_version_features.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/version_features.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509_create.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509_create.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509_crl.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509_crl.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509_crt.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509_crt.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509_csr.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509_csr.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509write_crt.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509write_crt.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_x509write_csr.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/x509write_csr.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

third_party_mbedtls_repo_library_xtea.obj: OPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/library/xtea.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).a: $(OBJECTS)
	@ echo creating library $@
	@ $(AR) --create $@ $^

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
