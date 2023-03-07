# Plug-And-Trust Mini Package Change Log


## Release v04.02.00

- SE050E applet support added. Default applet in feature file 'fsl_sss_ftr.h' changed to SE050E.

- Extended :cpp:enumerator:`smStatus_t` with new error codes.

- Updated behaviour of :cpp:func:`sss_se05x_key_object_get_handle` to return
  a success and print warning if it is unable to read attributes but the object exists so
  that other operations (like deleting) can proceed if they don't depend
  on object attributes.

- Updated OEF specific SCP keys handling. Added flags to enable OEF specific SCP03 keys in fsl_sss_ftr.h file.

- SE051-H applet support added (Provides PAKE support).

- Bug fix : Memory leak fix on open session with wrong keys.


## Release v04.01.01

- Policy changes for 7.x applet  (Also refer - :ref:`sss_policies`)
    - Below policies removed from :cpp:type:`sss_policy_sym_key_u` for applet version 7.x.
        - Allow key derivation policy (``can_KD``)
        - Allow to write the object policy (``can_Write``)
        - Allow to (re)generate policy (``can_Gen``)
    - Below policies are added for :cpp:type:`sss_policy_sym_key_u` for applet version 7.x.
        - Allow TLS PRF key derivation (``can_TLS_KDF``)
        - Allow TLS PMS key derivation (``can_TLS_PMS_KD``)
        - Allow HKDF (``can_HKDF``)
        - Allow PBKDF (``can_PBKDF``)
        - Allow Desfire key derivation (``can_Desfire_KD``)
        - Forbid External iv (``forbid_external_iv``)
        - Allow usage as hmac pepper (``can_usage_hmac_pepper``)
    - Below policies removed from :cpp:type:`sss_policy_asym_key_u` for applet version 7.x.
        - Allow to read the object policy (``can_Read``)
        - Allow to write the object policy (``can_Write``)
        - Allow key derivation policy (``can_KD``)
        - Allow key wrapping policy (``can_Wrap``)
    - Below policies are added for :cpp:type:`sss_policy_common_u` for applet version 7.x.
        - Allow to read the object policy (``can_Read``)
        - Allow to write the object policy (``can_Write``)
    - Added new policy - ``ALLOW_DESFIRE_CHANGEKEY``, :cpp:type:`sss_policy_desfire_changekey_authId_value_u`
    - Added new policy - ``ALLOW_DERIVED_INPUT``, :cpp:type:`sss_policy_key_drv_master_keyid_value_u`
    - **can_Read** and **can_Write** polices are moved from symmetric and asymmetric object policy to common policy in applet 7.x. **PLEASE UPDATE THE APPLICATIONS ACCORDINGLY**.

- New attestation scheme for applet 7.x
    - Updated API :cpp:func:`Se05x_API_TriggerSelfTest_W_Attst` for applet version 7.x.
    - Updated API :cpp:func:`Se05x_i2c_master_attst_txn` for applet version 7.x.
    - Updated API :cpp:func:`sss_se05x_key_store_get_key_attst` for applet version 7.x.

- New API added for PBKDF2 support: :cpp:func:`Se05x_API_PBKDF2_extended`. Supports optional salt
  object id and optional derived object id.

- New mode :cpp:enumerator:`kMode_SSS_Mac_Validate` added to support MAC validation feature in
  :cpp:func:`sss_mac_one_go` and ``sss_mac_*`` multistep APIs.

- New API added for ECDH calulation with option to select ECDH algorithm:
  :cpp:func:`Se05x_API_ECDHGenerateSharedSecret_InObject_extended`. ECDH algorithms
  supported - ``EC_SVDP_DH`` and ``EC_SVDP_DH_PLAIN``.

- New API added :cpp:func:`sss_cipher_one_go_v2` with different parameters for source
  and destination lengths to support ISO/IEC 9797-M2 padding.

- Internal IV generation supported added for AES CTR, AES CCM, AES GCM modes:
  :cpp:enumerator:`kAlgorithm_SSS_AES_GCM_INT_IV`,
  :cpp:enumerator:`kAlgorithm_SSS_AES_CTR_INT_IV`,
  :cpp:enumerator:`kAlgorithm_SSS_AES_CCM_INT_IV`.

- New MAC algorithm - :cpp:enumerator:`kAlgorithm_SSS_DES_CMAC8` supported.

- New api :cpp:func:`Se05x_API_ECPointMultiply_InputObj` added.

- New api :cpp:func:`Se05x_API_WriteSymmKey_Ver_extended` added to set key with minimun tag length for AEAD operations

- Removed all deprecated defines starting with ``With`` and replaced with ``SSS_HAVE_``

- ECKey authentication is updated to read SE.ECKA public key with attestation using
  :cpp:func:`Se05x_API_ReadObject_W_Attst_V2` or :cpp:func:`Se05x_API_ReadObject_W_Attst` (based on applet version)
  instead of GetData APDU. To authenicate the public key read with attestation, signature verification is performed
  on the data received from SE. See details of :cpp:func:`Se05x_API_ReadObject_W_Attst_V2` / :cpp:func:`Se05x_API_ReadObject_W_Attst`.

- sss_se05x_cipher_update() and sss_se05x_aead_update() APIs modified to use input buffer directly.

- Bugfix: Write of large binary files with policy fails on applet 3.x.


## Release v03.02.01_Spake

- Updated with v03.01.00 of Plug-and-trust

## Release v03.02.00_Spake

- Pake protocol support in APDU layer

## Release v03.00.06

- smCom_Init: return type is now *U16* instead of *void*. Return value indicates success/failure to create mutex/semophore.

- The enumerated type **SE05x_EDSignatureAlgo_t** contained a value **kSE05x_EDSignatureAlgo_ED25519PH_SHA_512**.
  The mnemonic name of the value was misleading as it actually corresponded to the `Pure EDDSA algorithm` not the
  `Prehashed (PH) EDDSA algorithm`. This has now been corrected. **This will require corresponding update in the application code.**

  - EDDSA signature algorithm enumerated value **kSE05x_EDSignatureAlgo_ED25519PH_SHA_512** is changed into **kSE05x_EDSignatureAlgo_ED25519PURE_SHA_512**.

  - EDDSA attestation algorithm enumerated value **kSE05x_AttestationAlgo_ED25519PH_SHA_512** is changed into as **kSE05x_AttestationAlgo_ED25519PURE_SHA_512**.

- Fixed typo in example code API: ex_sss_kestore_and_object_init() is now ex_sss_key_store_and_object_init()

- Added support for SE051 type

- Extended SE051 specific APDU command and response buffer size to match SE051's capabilities.

- SSS API blocks SHA512 attestation, signing and verification for RSA512 key

- Bug Fix : Fix for attestation read of symmetric objects which have no read policy.

- Added Platform SCP03 keys for SE051 (Variant A2 and C2).


## Release v03.00.02

- T1oI2C:

  - Fixed: potential null pointer dereference

  - Fixed: RSYNC _ + CRC error results in saving response to uninitialised buffer.

- ``hostlib/hostLib/platform/linux/i2c_a7.c``: A call to `axI2CTerm()` now closes the I2C file descriptor associated with the
  I2C communication channel.


## Release v03.00.00

- Initial commit

- Plug & Trust middleware to use secure element SE050
