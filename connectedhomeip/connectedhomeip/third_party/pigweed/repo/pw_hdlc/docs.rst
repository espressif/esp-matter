.. _module-pw_hdlc:

=======
pw_hdlc
=======
`High-Level Data Link Control (HDLC)
<https://en.wikipedia.org/wiki/High-Level_Data_Link_Control>`_ is a data link
layer protocol intended for serial communication between devices. HDLC is
standardized as `ISO/IEC 13239:2002 <https://www.iso.org/standard/37010.html>`_.

The ``pw_hdlc`` module provides a simple, robust frame-oriented transport that
uses a subset of the HDLC protocol. ``pw_hdlc`` supports sending between
embedded devices or the host. It can be used with :ref:`module-pw_rpc` to enable
remote procedure calls (RPCs) on embedded on devices.

**Why use the pw_hdlc module?**

  * Enables the transmission of RPCs and other data between devices over serial.
  * Detects corruption and data loss.
  * Light-weight, simple, and easy to use.
  * Supports streaming to transport without buffering, since the length is not
    encoded.

.. admonition:: Try it out!

  For an example of how to use HDLC with :ref:`module-pw_rpc`, see the
  :ref:`module-pw_hdlc-rpc-example`.

.. toctree::
  :maxdepth: 1
  :hidden:

  rpc_example/docs

--------------------
Protocol Description
--------------------

Frames
======
The HDLC implementation in ``pw_hdlc`` supports only HDLC unnumbered
information frames. These frames are encoded as follows:

.. code-block:: text

    _________________________________________
    | | | |                          |    | |...
    | | | |                          |    | |... [More frames]
    |_|_|_|__________________________|____|_|...
     F A C       Payload              FCS  F

     F = flag byte (0x7e, the ~ character)
     A = address field
     C = control field
     FCS = frame check sequence (CRC-32)


Encoding and sending data
=========================
This module first writes an initial frame delimiter byte (0x7E) to indicate the
beginning of the frame. Before sending any of the payload data through serial,
the special bytes are escaped:

            +-------------------------+-----------------------+
            | Unescaped Special Bytes | Escaped Special Bytes |
            +=========================+=======================+
            |           7E            |        7D 5E          |
            +-------------------------+-----------------------+
            |           7D            |        7D 5D          |
            +-------------------------+-----------------------+

The bytes of the payload are escaped and written in a single pass. The
frame check sequence is calculated, escaped, and written after. After this, a
final frame delimiter byte (0x7E) is written to mark the end of the frame.

Decoding received bytes
=======================
Frames may be received in multiple parts, so we need to store the received data
in a buffer until the ending frame delimiter (0x7E) is read. When the
``pw_hdlc`` decoder receives data, it unescapes it and adds it to a buffer.
When the frame is complete, it calculates and verifies the frame check sequence
and does the following:

* If correctly verified, the decoder returns the decoded frame.
* If the checksum verification fails, the frame is discarded and an error is
  reported.

---------
API Usage
---------
There are two primary functions of the ``pw_hdlc`` module:

  * **Encoding** data by constructing a frame with the escaped payload bytes and
    frame check sequence.
  * **Decoding** data by unescaping the received bytes, verifying the frame
    check sequence, and returning successfully decoded frames.

Encoder
=======
The Encoder API provides a single function that encodes data as an HDLC
unnumbered information frame.

C++
---
.. cpp:namespace:: pw

.. cpp:function:: Status hdlc::WriteUIFrame(uint64_t address, ConstByteSpan data, stream::Writer& writer)

  Writes a span of data to a :ref:`pw::stream::Writer <module-pw_stream>` and
  returns the status. This implementation uses the :ref:`module-pw_checksum`
  module to compute the CRC-32 frame check sequence.

.. code-block:: cpp

  #include "pw_hdlc/encoder.h"
  #include "pw_hdlc/sys_io_stream.h"

  int main() {
    pw::stream::SysIoWriter serial_writer;
    Status status = WriteUIFrame(123 /* address */,
                                 data,
                                 serial_writer);
    if (!status.ok()) {
      PW_LOG_INFO("Writing frame failed! %s", status.str());
    }
  }

Python
------
.. automodule:: pw_hdlc.encode
  :members:

.. code-block:: python

  import serial
  from pw_hdlc import encode

  ser = serial.Serial()
  address = 123
  ser.write(encode.ui_frame(address, b'your data here!'))

Typescript
----------

Encoder
=======
The Encoder class provides a way to build complete, escaped HDLC UI frames.

.. js:method:: Encoder.uiFrame(address, data)

    :param number address: frame address.
    :param Uint8Array data: frame data.
    :returns: Uint8Array containing a complete HDLC frame.

Decoder
=======
The decoder class unescapes received bytes and adds them to a buffer. Complete,
valid HDLC frames are yielded as they are received.

.. js:method:: Decoder.process(bytes)

    :param Uint8Array bytes: bytes received from the medium.
    :yields: Frame complete frames.

C++
---
.. cpp:class:: pw::hdlc::Decoder

  .. cpp:function:: pw::Result<Frame> Process(std::byte b)

    Parses a single byte of an HDLC stream. Returns a Result with the complete
    frame if the byte completes a frame. The status is the following:

      - OK - A frame was successfully decoded. The Result contains the Frame,
        which is invalidated by the next Process call.
      - UNAVAILABLE - No frame is available.
      - RESOURCE_EXHAUSTED - A frame completed, but it was too large to fit in
        the decoder's buffer.
      - DATA_LOSS - A frame completed, but it was invalid. The frame was
        incomplete or the frame check sequence verification failed.

  .. cpp:function:: void Process(pw::ConstByteSpan data, F&& callback, Args&&... args)

    Processes a span of data and calls the provided callback with each frame or
    error.

This example demonstrates reading individual bytes from ``pw::sys_io`` and
decoding HDLC frames:

.. code-block:: cpp

  #include "pw_hdlc/decoder.h"
  #include "pw_sys_io/sys_io.h"

  int main() {
    std::byte data;
    while (true) {
      if (!pw::sys_io::ReadByte(&data).ok()) {
        // Log serial reading error
      }
      Result<Frame> decoded_frame = decoder.Process(data);

      if (decoded_frame.ok()) {
        // Handle the decoded frame
      }
    }
  }

Python
------
.. autoclass:: pw_hdlc.decode.FrameDecoder
  :members:

Below is an example using the decoder class to decode data read from serial:

.. code-block:: python

  import serial
  from pw_hdlc import decode

  ser = serial.Serial()
  decoder = decode.FrameDecoder()

  while True:
      for frame in decoder.process_valid_frames(ser.read()):
          # Handle the decoded frame

Typescript
----------
Decodes one or more HDLC frames from a stream of data.

.. js:method:: process(data)

    :param Uint8Array data: bytes to be decoded.
    :yields: HDLC frames, including corrupt frames.
      The Frame.ok() method whether the frame is valid.

.. js:method:: processValidFrames(data)

    :param Uint8Array data: bytes to be decoded.
    :yields: Valid HDLC frames, logging any errors.

Allocating buffers
------------------
Since HDLC's encoding overhead changes with payload size and what data is being
encoded, this module provides helper functions that are useful for determining
the size of buffers by providing worst-case sizes of frames given a certain
payload size and vice-versa.

.. code-block:: cpp

  #include "pw_assert/check.h"
  #include "pw_bytes/span.h"
  #include "pw_hdlc/encoder"
  #include "pw_hdlc/encoded_size.h"
  #include "pw_status/status.h"

  // The max on-the-wire size in bytes of a single HDLC frame after encoding.
  constexpr size_t kMtu = 512;
  constexpr size_t kRpcEncodeBufferSize = pw::hdlc::MaxSafePayloadSize(kMtu);
  std::array<std::byte, kRpcEncodeBufferSize> rpc_encode_buffer;

  // Any data encoded to this buffer is guaranteed to fit in the MTU after
  // HDLC encoding.
  pw::ConstByteSpan GetRpcEncodeBuffer() {
    return rpc_encode_buffer;
  }

The HDLC ``Decoder`` has its own helper for allocating a buffer since it doesn't
need the entire escaped frame in-memory to decode, and therefore has slightly
lower overhead.

.. code-block:: cpp

  #include "pw_hdlc/decoder.h"

  // The max on-the-wire size in bytes of a single HDLC frame after encoding.
  constexpr size_t kMtu = 512;

  // Create a decoder given the MTU constraint.
  constexpr size_t kDecoderBufferSize =
      pw::hdlc::Decoder::RequiredBufferSizeForFrameSize(kMtu);
  pw::hdlc::DecoderBuffer<kDecoderBufferSize> decoder;

-------------------
Additional features
-------------------

Interleaving unstructured data with HDLC
========================================
It is possible to decode HDLC frames from a stream using different protocols or
unstructured data. This is not recommended, but may be necessary when
introducing HDLC to an existing system.

The ``FrameAndNonFrameDecoder`` Python class supports working with raw data and
HDLC frames in the same stream.

.. autoclass:: pw_hdlc.decode.FrameAndNonFrameDecoder
  :members:

RpcChannelOutput
================
The ``RpcChannelOutput`` implements pw_rpc's ``pw::rpc::ChannelOutput``
interface, simplifying the process of creating an RPC channel over HDLC. A
``pw::stream::Writer`` must be provided as the underlying transport
implementation.

If your HDLC routing path has a Maximum Transmission Unit (MTU) limitation,
using the ``FixedMtuChannelOutput`` is strongly recommended to verify that the
currently configured max RPC payload size (dictated by pw_rpc's static encode
buffer) will always fit safely within the limits of the fixed HDLC MTU *after*
HDLC encoding.

HdlcRpcClient
=============
.. autoclass:: pw_hdlc.rpc.HdlcRpcClient
  :members:

.. autoclass:: pw_hdlc.rpc.HdlcRpcLocalServerAndClient
  :members:

Example pw::rpc::system_server backend
======================================
This module includes an example implementation of ``pw_rpc``'s ``system_server``
facade. This implementation sends HDLC encoded RPC packets via ``pw_sys_io``,
and has blocking sends/reads, so it is hardly performance-oriented and
unsuitable for performance-sensitive applications. This mostly servers as a
simplistic example for quickly bringing up RPC over HDLC on bare-metal targets.

-----------
Size report
-----------
The HDLC module currently optimizes for robustness and flexibility instead of
binary size or performance.

There are two size reports: the first shows the cost of everything needed to
use HDLC, including the dependencies on common modules like CRC32 from
pw_checksum and variable-length integer handling from pw_varint. The other is
the cost if your application is already linking those functions. pw_varint is
commonly used since it's necessary for protocol buffer handling, so is often
already present.

.. include:: size_report

-------
Roadmap
-------
- **Expanded protocol support** - ``pw_hdlc`` currently only supports
  unnumbered information frames. Support for different frame types and
  extended control fields may be added in the future.

-------------
Compatibility
-------------
C++17

------
Zephyr
------
To enable ``pw_hdlc.pw_rpc`` for Zephyr add ``CONFIG_PIGWEED_HDLC_RPC=y`` to
the project's configuration.

