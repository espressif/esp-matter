# Connect: Elliptic-Curve Diffie-Hellman (ECDH) Key Agreement

## Introduction

The Connect stack supports sending encrypted messages using a pre-shared
(AES-128) key, which must be common for the whole network (hereinafter referred
to as *network key*). However, sometimes it is not feasible to pre-share the key
(for example burning the key to the device at production) or apply the key
manually (like typing in on some kind of console as the UART of the device).
Fortunately, it is possible to create a shared key (hereinafter referred to as
*shared key*) between two devices using the Diffie-Hellman algorithm while any
third party will not have the information to recover the key from the data sent
over the communication medium. This key however unique between every two
parties, so the current implementation will use this shared key to securely
distribute the network key to other devices.

The application example uses the *ECDH Key Exchange support* component available
under *Connect* -> *Test* in the Universal Configurator. The component relies on
Platform Security Architecture (PSA).

Note, that the method used in this example does not protect against physical
access to the devices.

## Prerequisites

The provided method is applicable for any Connect-based application with any
EFR32 device which supports Connect.

Further readings:

- [Connect Online Documentation](https://docs.silabs.com/connect-stack/latest/)
- [Connect Tutorial
  Series](https://community.silabs.com/s/article/connect-tutorial-series?language=en_US)

## Prepare the Application

Open the *Connect - SoC ECDH Key Exchange* through Simplicity Studio
project wizard.

Compile the code and download it to two (or optionally more) radio boards.

## Major Steps of Key Exchange

The same code runs on both (each) devices, one of the devices will request
(hereinafter referred to as *requestor*) the network key while the other will
provide it (hereinafter referred to as *provider*). These roles are not fixed,
it only depends on which device requests for the network key. In real-world
application the provider role may be tied to one specific device in the network
however, ECDH key exchange method does not force these roles to any device. The
key exchange is built from the following steps:

- the requestor creates its private/public key pair locally on the devices using
  ECC and  exports the public key
- the requestor initiates the request process by transferring the public key to
  the provider device
- the provider creates its private/public key pair locally on the devices using
  ECC and  exports the public key
- the provider receives the requestor's public key and calculates a shared key
  from its private key and the requestor's public key
- the provider encrypts the previously applied network key with the shared key
  and sends it to the requestor together with its public key
- the requestor receives the provider's public key and calculates the shared key
  then decrypts the encrypted network key using the shared key
- the requestor applies the network key

From that point, the two nodes can communicate with each other using standard
Connect API calls in an encrypted way.

## Testing the Application

If everything went well, a CLI interface should be available where the user can
issue the necessary commands.

### Example

Private/public key pair is generated on every network key request. The example
generates a random number each time it starts and this value will be set as the
network key.

```text
PSA init succeed
PSA: generate random network key succeed
PSA: random network key length: 16
PSA: random network key: 50 46 73 D0 F7 6B 12 F1 B6 B1 D2 B0 5E 8A 9F C2
Connect: set the random key as network key succeed
```

 The CLI allows to regenerate a random network key again or to apply an
 arbitrary user-defined key as the network key.

Assume there are two devices a *requestor* (node ID will be `0x0001`) and a
*provider* (node ID will be `0x002`). As mentioned earlier, any of the devices
can act as a *requestor* or as a *provider* in the example implementation.

First, issue the `leave` command on both parties - that's not mandatory
(especially for the first use when none of the addresses are commissioned), but
it makes sure that the network state is reset, no short addresses are assigned.

```text
> leave
```

Choose a node ID for the *requestor*:

```text
> commission 0x0001
```

Then choose a node ID for the *provider*:

```text
> commission 0x0002
```

If the network is set up on both devices they are ready to send and receive
messages - however, received messages will be discarded as the network keys
(generated randomly at startup) are differ.

The next step is the ECDH key exchange. It consists of several sub-steps, but
the example combines these into a single command to issue. Issue the following
command on the node with address `0x0001` (the *requestor*):

```text
> request_network_key 0x0002
```

The *requestor* initiates the key exchange

```test
Connect: initiating key exchange
PSA: generating public/private key pair
PSA: generated key ID: 2147483643
PSA: exporting public key
PSA: public key export succeed
PSA: public key length: 65
PSA: public key: 04 DC 44 6E 96 5A 47 AD 46 89 7F 43 98 AE DA F9 8B 99 96 ED D0 C9 18 78 39 A8 04 E9 CC 17 A9 07 7A 8F 73 B4 33 C7 CC 08 C0 C4 CB D5 06 B2 7E F6 7C CC B2 95 55 D7 EB C0 79 CF BD DF 3E 44 10 45 73
Connect: requestor message length 71
Connect: sending public key
```

When the *provider* receives the request it will generate a public/private key
pair and calculates the shared key, encrypts the network key then sends its
public key and the encrypted network key to the requestor.

```text
Connect: key exchange request received
PSA: generating public/private key pair
PSA: exporting public key
PSA: generating shared key
PSA: destroying public/private key pair
PSA: encrypting network key
PSA: plain text connect network key: 62 B1 38 5D 79 31 BB 48 18 F3 F9 D9 DF 8D B7 C9
PSA: encrypted connect network key:  4E 3C 95 03 E1 66 35 B5 40 E6 C3 66 46 D4 0E F3
PSA: initial vector:                 38 D4 AE 03 DA 5F 35 67 2C 09 05 61 E7 2F BF D7
PSA: destroying shared key
Connect: sending public key, initial vector, network key
```

In the last step, the *requestor* receives the public key and the encrypted
network key, calculates the shared key then decrypts the network key and applies
it as the Connect network key.

```text
Connect: reply to key exchange request received
PSA: generating shared key
PSA: destroying public/private key pair
PSA: decrypting network key
PSA: destroying shared key
PSA: received network key: 62 B1 38 5D 79 31 BB 48 18 F3 F9 D9 DF 8D B7 C9
Connect: applying received network key
Connect: set network key succeed
```

In the current application implementation the public/private key pair - thus the
shared key as well - is re-generated on every request for the network key. This
increases the security, but not strictly necessary, the same key pairs can be
used multiple times. The calculated shared key however will be different between
different device pairs.

Once the network key is successfully transferred it is possible to send secured
messages between devices by the Connect message send API. The application
implements the `send` command to demonstrate this.

```text
> send 2 {00112233}
Message queued (length: 4)
```

On the remote side the message should be received:

```text
RX: Data from 0x0001:00 11 22 33
```

If the network keys do not match on the devices (i.e. the `request_network_key`
command was not issued or a new network key applied otherwise) the receiver side
will discard the incoming message (and nothing will be printed on the console).

### Additional CLI commands

Besides the commands detailed above, there are a few commands to inspect the
operation of ECDH key exchange somewhat deeper.

#### Generating new random network key

```text
> generate_random_network_key
PSA: generate random network key succeed
PSA: random network key length: 16
PSA: random network key: 0B 2D FF 07 C8 DD 3B 80 EE 9A C3 A2 78 8D 4C FA
Connect: set the random key as network key succeed
```

This command generates a random key using the PSA API and immediately applies it
as the Connect network key that will be used for outgoing message encryption and
can be retrieved by a key exchange request.

#### Applying an arbitrary network key

```text
> set_network_key {00112233445566778899aabbccddeeff}
00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF
Connect: set network key succeed
```

The above command applies a user-defined Connect network key that will be used
for outgoing message encryption and can be retrieved by a key exchange request.

#### Generating public/private key pair

```text
> generate_key_pair
PSA: generated key ID: 2147483643
```

This command will create a public/private key pair. On devices with the *Secure
Vault High* feature the key pair is generated in the vault which means the
application cannot access it - so, the key itself cannot be displayed (and only
the public part can be exported), hence the example displays only the key ID as
its reference that can be used in other functions.

#### Exporting the public key

```text
> export_public_key 2147483643
PSA: public key export succeed
PSA: public key length: 65
PSA: public key: 04 5B DC 7B 64 93 16 10 E3 8E BA DB 5D 58 54 27 EE BF 98 C8 F6 B2 F8 46 F8 E1 2C 37 C6 60 5F 5E 1C 86 11 00 DB F4 9C B7 03 C4 39 DB 20 A9 48 1D A4 8E 7C 78 1D 5B DB C5 C6 44 54 DC D1 45 DF BB D4
```

This command exports the public part of a previously generated key pair. The
argument is the output of a previous `generate_key_pair` command.

#### Generating the shared key

The shared key is a temporary key between two devices that can be used to
encrypt data to send securely between the two parties. In the example
application, this key is used to transfer the Connect network key from the
provider to the requestor.

```text
> generate_shared_key 2147483643 {04C787AF90F992C4160EA4E0D8618CE980EB0F2860E11EDA2DFCFE49B930A4502470C66FEBA256C1AF3A0C00A0990E2A87146D7979A1B520444008DB16576EA3E3}
PSA: generate shared key succeed
PSA: generated shared key ID: 2147483644
```

As the public/private key pair the shared key is unaccessible by the application
hence only the ID of the key is printed.

#### Encrypting a message

If the shared key is available the required message can be encrypted with it.

```text
> encrypt_message 2147483644 {00112233445566778899aabbccddeeff}  
PSA: encrypt succeed
PSA: initial vector: C2 E2 A9 43 0C D3 3E C5 4B 92 D9 A9 15 25 D9 58
PSA: encrypted message: D8 E2 62 FB 09 6E 6D E9 7F 8F E7 45 31 04 EC 97
```

The ECDH Key Exchange support component uses the AES CBC algorithm that requires
an initial vector. The encrypt function generates a random initial vector that
is used during the encryption. This initial vector must be provided to the
decrypt function to successfully decrypt the message. The encrypt command
displays not only the encrypted message but the initial vector as well. Note:
AES supports only 128-bit blocks as input so, the input data must be multiple of
16 bytes otherwise the encryption will fail.

The input parameters of the command are the shared key ID and the plain text
data to encrypt.

#### Decrypting a message

The encrypted data can be decrypted as long as the shared key matches and the
initial vector is available from the encryption operation.

```text
> decrypt_message 2147483644 {C2 E2 A9 43 0C D3 3E C5 4B 92 D9 A9 15 25 D9 58} {D8 E2 62 FB 09 6E 6D E9 7F 8F E7 45 31 04 EC 97}
PSA: decrypt succeed
PSA: decrypted message: 00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF
```

The input parameters of the command are the shared key ID, the initial vector
from the encryption operation, and the encrypted data to decrypt.

#### Destroying keys

Keys that are not used anymore can be discarded by the destroy command.

```text
> destroy_key 2147483644
PSA: destroy key succeed
```

#### Retrieving network information

The current status of the network can be displayed by the info command.

```text
> info
Info:
         MCU Id: 0xccccccfffef8cd42
  Network state: 0x02
      Node type: 0x05
          eui64: >ccccccfffef8cd42
        Node id: 0x0002
         Pan id: 0x01ff
        Channel: 0
          Power: -1
     TX options: MAC acks enabled, security enabled, priority disabled
```

### Notes

In normal use cases, the network key is not generated at every restart. The
usual workflow is that the network key is stored in the non-volatile space of
the *provider* - in the code memory for example and applied once (at first power
up or initiated by a CLI command, etc.). Although Connect's
[`emberSetSecurityKey()`](https://docs.silabs.com/connect-stack/latest/group-stack-info#ga90da760f71e7b50c5b0edbda6a97d2bc)
API function stores the network key in a non-volatile space, there is no API
call to read it back - thus the application must store the network key in a
retrievable form to be able to send it to the *requestor*.

The security can be enabled at the beginning of the process above because
enabling security for the messages only makes sense for sent messages, received
messages are passed to the application if the network keys match (the device can
decode the message) or if the message is unencrypted. If the encryption status
is not checked by the application for the incoming messages malicious devices
can trick the devices in the network.

The above method for transferring the network key works with multiple
*requestor* devices, but in that the key sharing process must be repeated
between the *provider* and every *requestor* one by one.

## Conclusion

Although Connect does not support ECDH key exchange at the stack level, the way
to add ECDH key exchange is fairly straightforward by using the *Connect* ->
*ECDH key exchange support* component.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
