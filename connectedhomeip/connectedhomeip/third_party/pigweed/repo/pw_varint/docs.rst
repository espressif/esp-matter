.. _module-pw_varint:

---------
pw_varint
---------
The ``pw_varint`` module provides functions for encoding and decoding variable
length integers, or varints. For smaller values, varints require less memory
than a fixed-size encoding. For example, a 32-bit (4-byte) integer requires 1--5
bytes when varint-encoded.

`Protocol Buffers <https://developers.google.com/protocol-buffers/docs/encoding#varints>`_
use a variable-length encoding for integers.

Compatibility
=============
* C
* C++14 (with :doc:`../pw_polyfill/docs`)

API
===

.. cpp:function:: size_t EncodedSize(uint64_t integer)

Returns the size of an integer when encoded as a varint. Works on both signed
and unsigned integers.

.. cpp:function:: size_t ZigZagEncodedSize(int64_t integer)

Returns the size of a signed integer when ZigZag encoded as a varint.

.. cpp:function:: uint64_t MaxValueInBytes(size_t bytes)

Returns the maximum integer value that can be encoded as a varint into the
specified number of bytes.


Stream API
----------

.. cpp:function:: StatusWithSize Read(stream::Reader& reader, int64_t* output)
.. cpp:function:: StatusWithSize Read(stream::Reader& reader, uint64_t* output)

Decoders a varint from the current position of a stream. If reading into a
signed integer, the value is ZigZag decoded.

Returns the number of bytes read from the stream, places the value in `output`,
if successful. Returns `OutOfRange` if the varint does not fit in to the type,
or if the input is exhausted before the number terminates.

Reads a maximum of 10 bytes.

Dependencies
============
* ``pw_span``

Zephyr
======
To enable ``pw_varint`` for Zephyr add ``CONFIG_PIGWEED_VARINT=y`` to the
project's configuration.
