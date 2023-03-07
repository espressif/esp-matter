.. _module-pw_bluetooth_hci:

================
pw_bluetooth_hci
================
The ``pw_bluetooth_hci`` module contains utilities for using the Host Controller
Interface as defined by the Bluetooth Core Specification version 5.3.

-----------
HCI Packets
-----------
This module has support to parse a the subset of the HCI packets used in the HCI
UART Transport Layer are provided as defined in the Bluetooth Core Specification
version 5.3 "Host Controller Interface Transport Layer" volume 4, part A,
meaning:

* HCI Command Packet: ``pw::bluetooth_hci::CommandPacket``

* HCI ACL Data Packet: ``pw::bluetooth_hci::AsyncDataPacket``

* HCI SCO Data Packet: ``pw::bluetooth_hci::SyncDataPacket``

* HCI Event Data Packet: ``pw::bluetooth_hci::EventPacket``

------------------------
HCI UART Transport Layer
------------------------

Decoding
========
A decoder function is provided to parse HCI packets out of a HCI UART Transport
Layer buffer which may contain multiple packets.

  .. cpp:function:: StatusWithSize DecodeHciUartData(ConstByteSpan data, const DecodedPacketCallback& packet_callback);

    Parses the HCI Packets out of a HCI UART Transport Layer buffer.

    Parses as many complete HCI packets out of the provided buffer based on the
    HCI UART Transport Layer as defined by Bluetooth Core Specification version
    5.3 "Host Controller Interface Transport Layer" volume 4, part A.

    The HciPacketCallback is invoked for each full HCI packet.

    Returns the number of bytes processed and a status based on:

      * OK - No invalid packet indicator found.
      * DATA_LOSS - An invalid packet indicator was detected between packets.
        Synchronization has been lost. The caller is responsible for
        regaining synchronization

    .. note:: The caller is responsible for detecting the lack of progress due
      to an undersized data buffer and/or an invalid length field in case a full
      buffer is passed and no bytes are processed.

