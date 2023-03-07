---
# One-way key exchange and message encryption with AESCCM

---

## Example Summary

Simple example of key generation, public key exchange encrypted message transfer.
UART loopback mode is used to operate both 'devices' on one LaunchPad. The public
and private keys of the RX side must be hard-coded for one-way communication to
be possible.

### Disclaimer

This demo is not designed for use in production and is only a demonstration of
how to manipulate the APIs of the various crypto drivers. Please evaluate the
specific security requirements of your product and domain and consult with security
experts before deploying critical code in the field. This example is not appropriate
for a final product and does not provide any guarantee of security if utilised.

In particular, the use of ECDSA in this context serves no practical purpose.
ECDSA is mostly used to verify an unseen public key through a trusted third
party, or authenticate asymmetrically without encryption. It is used here
to document the usage of the API.

### Crytography Drivers

* `CONFIG_SHA2_0` - For generating the hash required for shared secret generation
* `CONFIG_AESECB_0` - One-step encryption and decryption for encrypted packets
* `CONFIG_ECDSA_0` - Signing and verifying packets
* `CONFIG_ECDH_0` - Generating private-public key pairs for secure communication
* `CONFIG_TRNG_0` - Random numbers for private key generation

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_GPIO_LED_0` - Toggled when an encrypted packet is successfully decrypted
* `CONFIG_GPIO_LED_1` - Toggled when the public key has been successfully exchanged
* `CONFIG_GPIO_BUTTON_0` - Sends the public key/shared secret packet to the RX task
* `CONFIG_GPIO_BUTTON_1` - Sends an encrypted packet to the RX task
* `CONFIG_UART_0` - Sends packets to the RX task

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

## Example Usage

Run the example.

1. Push `CONFIG_GPIO_BUTTON_0` to generate a private-public key pair for the TX
task and communicate the new public key to the RX task.

2. Push `CONFIG_GPIO_BUTTON_1` to encrypt a packet with the content
'this is plaintext' on the TX task and decrypt the same content on the RX task.

## Application Design Details

* The main applications run in `txTask.c` and `rxTask.c`. The TX task blocks on two button presses, one for key generation and one for packet transmission. The RX side blocks on `UART_receive` whenever applicable.

* When the key generation button is pressed, the TX task unblocks and:
 * Uses the TRNG to generate a new, random 32-byte private key
 * Generates a corresponding public key using the ECDH driver
 * Loads that public key into a UART data packet
 * Hashes the identifier, length and public key with SHA-256
 * Signs the hash and the previously generated private key
 * Loads the signature (R, S) onto the data packet
 * Transmits that data packet
 * Uses the private key and the hard-coded RX public key to create a shared secret, then hashes and truncates it
 * Blocks until the next button press
* When the RX task receives this packet, it unblocks and:
 * Hashes the identifier, length and public key of the incoming packet with SHA-256 (as above)
 * Uses the hash, R/S and the provided TX public key to verify the packet signature
 * Uses the transferred TX public key and the hard-coded RX private key to create a shared secret, then hashes and truncates it
 * Blocks until the next received packet
* When the encrypted packet button is pressed, the TX task unblocks and:
 * Increments the per-message nonce (to avoid identical messages having identical ciphertext)
 * Runs the header, the nonce and the message through `AESCCM_oneStepEncrypt` and transmits it to the RX task
* When the RX task receives this packet, it unblocks and:
 * Runs the packet through `AESCCM_oneStepDecrypt`

TI-RTOS:

* When building in Code Composer Studio, the kernel configuration project will
be imported along with the example. The kernel configuration project is
referenced by the example, so it will be built first. The "release" kernel
configuration is the default project used. It has many debug features disabled.
These feature include assert checking, logging and runtime stack checks. For a
detailed difference between the "release" and "debug" kernel configurations and
how to switch between them, please refer to the SimpleLink MCU SDK User's
Guide. The "release" and "debug" kernel configuration projects can be found
under &lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/(ccs|gcc).

FreeRTOS:

* Please view the `FreeRTOSConfig.h` header file for example configuration
information.
