5. Security Considerations
==========================

{IDF_TARGET_RELEASE:default="v5.2.3"}

5.1 Overview
------------

This guide provides an overview of the overall security features that should be considered while designing the products with Matter framework on ESP32 SoCs.

High level security goals are as follows:

#. Preventing untrustworthy code from being executed
#. Securing device identity (e.g., Matter DAC Private Key)
#. Secure storage for confidential data

5.2 Platform Security
---------------------

5.2.1 Secure Boot
~~~~~~~~~~~~~~~~~

The Secure Boot feature ensures that only authenticated software can execute on the device. The Secure Boot process forms a chain of trust by verifying all **mutable** software entities involved in the boot-up process. Signature verification happens during both boot-up as well as in OTA updates.

Please refer to `Secure Boot V2`_ guide for detailed documentation about this feature in {IDF_TARGET_NAME}.

.. only:: esp32

    For ESP32 before ECO3, please refer to `Secure Boot V1`_ guide.

.. _flash_enc-guide:

5.2.2 Flash Encryption
~~~~~~~~~~~~~~~~~~~~~~

The Flash Encryption feature helps to encrypt the contents on the off-chip flash memory and thus provides the confidentiality aspect to the software or data stored in the flash memory.

Please refer to `Flash Encryption`_ guide for detailed documentation about this feature in {IDF_TARGET_NAME}.

5.3 Product Security
--------------------

.. _secure_storage-guide:

5.3.1 Secure Storage
~~~~~~~~~~~~~~~~~~~~

Secure storage refers to the application-specific data that can be stored in a secure manner on the device, i.e., off-chip flash memory. This is typically a read-write flash partition and holds device specific configuration data, e.g., Wi-Fi credentials.

ESP-IDF provides the **NVS (Non-volatile Storage)** management component which allows encrypted data partitions. This feature is tied with the platform flash encryption feature described earlier.

Please refer to the `NVS Encryption`_ for detailed documentation on the working and instructions to enable this feature in {IDF_TARGET_NAME}.

5.3.2 Device Identity
~~~~~~~~~~~~~~~~~~~~~

Matter specification requires a unique Device Attestation Key (DAC) per device. This is a private ECDSA (secp256r1 curve) key that establishes the device identity to the Matter Ecosystem.
DAC private needs to be protected from remote as well as physical attacks in the best possible way.

Recommended ways for DAC private key protection:

.. only:: esp32h2

  - {IDF_TARGET_NAME} supports ECDSA hardware peripheral with the ECDSA key programmed in the eFuse. This key is software read protected (in default mode). This peripheral can help to protect the identity of the DAC private key on the device.

.. only:: not esp32 and not esp32c2

  - {IDF_TARGET_NAME} supports HMAC peripheral with the HMAC key (software read protected) programmed in the eFuse. This peripheral can be used as a hash function (HMAC-SHA256) for PBKDF2 or similar key derivation function. And thus the DAC private key can be computed at run time using this mechanism.

    .. note::

        Please note that the Secure Boot must be enabled with this scheme to protect against the untrusted software execution.

- DAC private key can be protected using :ref:`flash_enc-guide` or :ref:`secure_storage-guide` schemes.

.. important::

   Support for DAC private key protection mechanisms described above is available in the Matter crypto port layer for ESP32 platform.

.. note::

   Espressif provides pre-provisioning service to build Matter-Compatible devices. This service also ensures the security of the DAC private key and configuration data. Please contact Espressif Sales for more information.

5.4 More Security Considerations
--------------------------------

Please refer to the overall ESP-IDF `Security Guide`_ for more considerations related to the debug interfaces, network, transport and OTA updates related security.

5.5 Security Policy
-------------------

The ESP-Matter GitHub repository has attached `Security Policy Brief`_.

5.5.1 Advisories
~~~~~~~~~~~~~~~~

- Espressif publishes critical `Security Advisories`_, which includes security advisories regarding both hardware and software.
- The specific advisories of the ESP-Matter software components shall be published through the `GitHub repository`_.

5.5.2 Software Updates
~~~~~~~~~~~~~~~~~~~~~~

Critical security issues in the ESP-Matter components, ESP-IDF components and dependant third-party libraries are fixed as and when we find them or when they are reported to us. Gradually, we make the fixes available in all applicable release branches in ESP-Matter.

.. important::

    We recommend periodically updating to the latest bugfix version of the ESP-Matter release to have all critical security fixes available.


.. _`Security Policy Brief`: https://github.com/espressif/esp-matter/blob/main/SECURITY.md
.. _`Security Advisories`: https://www.espressif.com/en/support/documents/advisories
.. _`GitHub repository`: https://github.com/espressif/esp-matter/security/advisories
.. _`NVS Encryption`: https://docs.espressif.com/projects/esp-idf/en/{IDF_TARGET_RELEASE}/{IDF_TARGET_PATH_NAME}/api-reference/storage/nvs_flash.html#nvs-encryption
.. _`Flash Encryption`: https://docs.espressif.com/projects/esp-idf/en/{IDF_TARGET_RELEASE}/{IDF_TARGET_PATH_NAME}/security/flash-encryption.html
.. _`Secure Boot V2`: https://docs.espressif.com/projects/esp-idf/en/{IDF_TARGET_RELEASE}/{IDF_TARGET_PATH_NAME}/security/secure-boot-v2.html
.. _`Secure Boot V1`: https://docs.espressif.com/projects/esp-idf/en/{IDF_TARGET_RELEASE}/{IDF_TARGET_PATH_NAME}/security/secure-boot-v1.html
.. _`Security Guide`: https://docs.espressif.com/projects/esp-idf/en/{IDF_TARGET_RELEASE}/{IDF_TARGET_PATH_NAME}/security/security.html
