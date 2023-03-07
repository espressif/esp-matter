.. _module-pw_stream:

.. cpp:namespace-push:: pw::stream

=========
pw_stream
=========

``pw_stream`` provides a foundational interface for streaming data from one part
of a system to another. In the simplest use cases, this is basically a memcpy
behind a reusable interface that can be passed around the system. On the other
hand, the flexibility of this interface means a ``pw_stream`` could terminate is
something more complex, like a UART stream or flash memory.

--------
Overview
--------
At the most basic level, ``pw_stream``'s interfaces provide very simple handles
to enabling streaming data from one location in a system to an endpoint.

Example:

.. code-block:: cpp

  Status DumpSensorData(pw::stream::Writer& writer) {
    static char temp[64];
    ImuSample imu_sample;
    imu.GetSample(&info);
    size_t bytes_written = imu_sample.AsCsv(temp, sizeof(temp));
    return writer.Write(temp, bytes_written);
  }

In this example, ``DumpSensorData()`` only cares that it has access to a
:cpp:class:`Writer` that it can use to stream data to using ``Writer::Write()``.
The :cpp:class:`Writer` itself can be backed by anything that can act as a data
"sink."

---------------------
pw::stream Interfaces
---------------------
There are three basic capabilities of a stream:

 * Reading -- Bytes can be read from the stream.
 * Writing -- Bytes can be written to the stream.
 * Seeking -- The position in the stream can be changed.

``pw_stream`` provides a family of stream classes with different capabilities.
The most basic class, :cpp:class:`Stream` guarantees no functionality, while the
most capable class, :cpp:class:`SeekableReaderWriter` supports reading, writing,
and seeking.

Usage overview
==============

+-------------------------------------------+-----------------+------------------------------+
| pw::stream Interfaces                     | Accept in APIs? | Extend to create new stream? |
+===========================================+=================+==============================+
| :cpp:class:`Stream`                       | ❌              | ❌                           |
+-------------------------------------------+-----------------+------------------------------+
| :cpp:class:`Reader`                       | ✅              | ❌                           |
|                                           |                 |                              |
| :cpp:class:`Writer`                       |                 |                              |
|                                           |                 |                              |
| :cpp:class:`ReaderWriter`                 |                 |                              |
+-------------------------------------------+-----------------+------------------------------+
| :cpp:class:`SeekableReader`               | ✅              | ✅                           |
|                                           |                 |                              |
| :cpp:class:`SeekableWriter`               |                 |                              |
|                                           |                 |                              |
| :cpp:class:`SeekableReaderWriter`         |                 |                              |
+-------------------------------------------+-----------------+------------------------------+
| :cpp:class:`RelativeSeekableReader`       | ✅ (rarely)     | ✅                           |
|                                           |                 |                              |
| :cpp:class:`RelativeSeekableWriter`       |                 |                              |
|                                           |                 |                              |
| :cpp:class:`RelativeSeekableReaderWriter` |                 |                              |
+-------------------------------------------+-----------------+------------------------------+
| :cpp:class:`NonSeekableReader`            | ❌              | ✅                           |
|                                           |                 |                              |
| :cpp:class:`NonSeekableWriter`            |                 |                              |
|                                           |                 |                              |
| :cpp:class:`NonSeekableReaderWriter`      |                 |                              |
+-------------------------------------------+-----------------+------------------------------+

Interface documentation
=======================
Summary documentation for the ``pw_stream`` interfaces is below. See the API
comments in `pw_stream/public/pw_stream/stream.h
<https://cs.pigweed.dev/pigweed/+/main:pw_stream/public/pw_stream/stream.h>`_
for full details.

.. cpp:class:: Stream

  A generic stream that may support reading, writing, and seeking, but makes no
  guarantees about whether any operations are supported. Stream serves as the
  base for the Reader, Writer, and ReaderWriter interfaces.

  Stream cannot be extended directly. Instead, work with one of the derived
  classes that explicitly supports the required functionality. Stream should
  almost never be used in APIs; accept a derived class with the required
  capabilities instead.

  All Stream methods are blocking. They return when the requested operation
  completes.

  **Public methods**

  .. cpp:function:: bool readable() const

    True if :cpp:func:`Read` is supported.

  .. cpp:function:: bool writable() const

    True if :cpp:func:`Write` is supported.

  .. cpp:function:: bool seekable() const

    True if :cpp:func:`Seek` is supported.

  .. cpp:function:: Result<ByteSpan> Read(ByteSpan buffer)
  .. cpp:function:: Result<ByteSpan> Read(void* buffer, size_t size_bytes)

    Reads data from the stream into the provided buffer, if supported. As many
    bytes as are available up to the buffer size are copied into the buffer.
    Remaining bytes may by read in subsequent calls.

    Returns:

      * OK - Between 1 and dest.size_bytes() were successfully read. Returns
        the span of read bytes.
      * UNIMPLEMENTED - This stream does not support writing.
      * FAILED_PRECONDITION - The Reader  is not in state to read data.
      * RESOURCE_EXHAUSTED - Unable to read any bytes at this time. No bytes
        read. Try again once bytes become available.
      * OUT_OF_RANGE - Reader has been exhausted, similar to EOF. No bytes were
        read, no more will be read.

  .. cpp:function:: Status Write(ConstByteSpan data)

    Writes the provided data to the stream, if supported.

    Returns:

      * OK - Data was successfully accepted by the stream.
      * UNIMPLEMENTED - This stream does not support writing.
      * FAILED_PRECONDITION - The writer is not in a state to accept data.
      * RESOURCE_EXHAUSTED - The writer was unable to write all of requested data
        at this time. No data was written.
      * OUT_OF_RANGE - The Writer has been exhausted, similar to EOF. No data was
        written; no more will be written.


  .. cpp:function:: Status Seek(ptrdiff_t offset, Whence origin = kBeginning)

    Changes the current read & write position in the stream, if supported.

    Returns:

      * OK - Successfully updated the position.
      * UNIMPLEMENTED - Seeking is not supported for this stream.
      * OUT_OF_RANGE - Attempted to seek beyond the bounds of the stream. The
        position is unchanged.

  .. cpp:function:: size_t Tell() const

    Returns the current read & write position in the stream, if supported.
    Returns ``Stream::kUnknownPosition`` (``size_t(-1)``) if unsupported.

  .. cpp:function:: size_t ConservativeReadLimit() const

    Likely minimum bytes available to read. Returns ``kUnlimited``
    (``size_t(-1)``) if there is no limit or it is unknown.

  .. cpp:function:: size_t ConservativeWriteLimit() const

    Likely minimum bytes available to write. Returns ``kUnlimited``
    (``size_t(-1)``) if there is no limit or it is unknown.

  **Private virtual methods**

  Stream's public methods are non-virtual. The public methods call private
  virtual methods that are implemented by derived classes.

  .. cpp:function:: private virtual StatusWithSize DoRead(ByteSpan destination)

    Virtual :cpp:func:`Read` function implemented by derived classes.

  .. cpp:function:: private virtual Status DoWrite(ConstByteSpan data)

    Virtual :cpp:func:`Write` function implemented by derived classes.

  .. cpp:function:: private virtual Status DoSeek(ptrdiff_t offset, Whence origin)

    Virtual :cpp:func:`Seek` function implemented by derived classes.

  .. cpp:function:: private virtual size_t DoTell() const

    Virtual :cpp:func:`Tell` function optionally implemented by derived classes.
    The default implementation always returns ``kUnknownPosition``.

  .. cpp:function:: private virtual size_t ConservativeLimit(LimitType limit_type)

    Virtual function optionally implemented by derived classes that is used for
    :cpp:func:`ConservativeReadLimit` and :cpp:func:`ConservativeWriteLimit`.
    The default implementation returns ``kUnlimited`` or ``0`` depending on
    whether the stream is readable/writable.

Reader interfaces
-----------------
.. cpp:class:: Reader : public Stream

   A Stream that supports writing but not reading. The Write() method is hidden.

   Use in APIs when:
     * Must read from, but not write to, a stream.
     * May or may not need seeking. Use a SeekableReader& if seeking is
       required.

   Inherit from when:
     * Reader cannot be extended directly. Instead, extend SeekableReader,
       NonSeekableReader, or (rarely) RelativeSeekableReader, as appropriate.

   A Reader may or may not support seeking. Check seekable() or try calling
   Seek() to determine if the stream is seekable.

.. cpp:class:: SeekableReader : public RelativeSeekableReader

   A Reader that fully supports seeking.

   Use in APIs when:
     * Absolute seeking is required. Use Reader& if seeking is not required or
       seek failures can be handled gracefully.

   Inherit from when:
     * Implementing a reader that supports absolute seeking.

.. cpp:class:: RelativeSeekableReader : public Reader

   A Reader that at least partially supports seeking. Seeking within some range
   of the current position works, but seeking beyond that or from other origins
   may or may not be supported. The extent to which seeking is possible is NOT
   exposed by this API.

   Use in APIs when:
     * Relative seeking is required. Usage in APIs should be rare; generally
       Reader should be used instead.

   Inherit from when:
     * Implementing a Reader that can only support seeking near the current
       position.

   A buffered Reader that only supports seeking within its buffer is a good
   example of a RelativeSeekableReader.

.. cpp:class:: NonSeekableReader : public Reader

   A Reader that does not support seeking. The Seek() method is hidden.

   Use in APIs when:
     * Do NOT use in APIs! If seeking is not required, use Reader& instead.

   Inherit from when:
     * Implementing a Reader that does not support seeking.

Writer interfaces
-----------------
.. cpp:class:: Writer : public Stream

   A Stream that supports writing but not reading. The Read() method is hidden.

   Use in APIs when:
     * Must write to, but not read from, a stream.
     * May or may not need seeking. Use a SeekableWriter& if seeking is
       required.

   Inherit from when:
     * Writer cannot be extended directly. Instead, extend SeekableWriter,
       NonSeekableWriter, or (rarely) RelativeSeekableWriter, as appropriate.

   A Writer may or may not support seeking. Check seekable() or try calling
   Seek() to determine if the stream is seekable.

.. cpp:class:: SeekableWriter : public RelativeSeekableWriter

   A Writer that fully supports seeking.

   Use in APIs when:
     * Absolute seeking is required. Use Writer& if seeking is not required or
       seek failures can be handled gracefully.

   Inherit from when:
     * Implementing a writer that supports absolute seeking.


.. cpp:class:: RelativeSeekableWriter : public Writer

   A Writer that at least partially supports seeking. Seeking within some range
   of the current position works, but seeking beyond that or from other origins
   may or may not be supported. The extent to which seeking is possible is NOT
   exposed by this API.

   Use in APIs when:
     * Relative seeking is required. Usage in APIs should be rare; generally
       Writer should be used instead.

   Inherit from when:
     * Implementing a Writer that can only support seeking near the current
       position.

   A buffered Writer that only supports seeking within its buffer is a good
   example of a RelativeSeekableWriter.

.. cpp:class:: NonSeekableWriter : public Writer

   A Writer that does not support seeking. The Seek() method is hidden.

   Use in APIs when:
     * Do NOT use in APIs! If seeking is not required, use Writer& instead.

   Inherit from when:
     * Implementing a Writer that does not support seeking.

ReaderWriter interfaces
-----------------------
.. cpp:class:: ReaderWriter : public Stream

   A Stream that supports both reading and writing.

   Use in APIs when:
     * Must both read from and write to a stream.
     * May or may not need seeking. Use a SeekableReaderWriter& if seeking is
       required.

   Inherit from when:
     * Cannot extend ReaderWriter directly. Instead, extend
       SeekableReaderWriter, NonSeekableReaderWriter, or (rarely)
       RelativeSeekableReaderWriter, as appropriate.

   A ReaderWriter may or may not support seeking. Check seekable() or try
   calling Seek() to determine if the stream is seekable.

.. cpp:class:: SeekableReaderWriter : public RelativeSeekableReaderWriter

   A ReaderWriter that fully supports seeking.

   Use in APIs when:
     * Absolute seeking is required. Use ReaderWriter& if seeking is not
       required or seek failures can be handled gracefully.

   Inherit from when:
     * Implementing a writer that supports absolute seeking.

.. cpp:class:: RelativeSeekableReaderWriter : public ReaderWriter

   A ReaderWriter that at least partially supports seeking. Seeking within some
   range of the current position works, but seeking beyond that or from other
   origins may or may not be supported. The extent to which seeking is possible
   is NOT exposed by this API.

   Use in APIs when:
     * Relative seeking is required. Usage in APIs should be rare; generally
       ReaderWriter should be used instead.

   Inherit from when:
     * Implementing a ReaderWriter that can only support seeking near the
       current position.

   A buffered ReaderWriter that only supports seeking within its buffer is a
   good example of a RelativeSeekableReaderWriter.

.. cpp:class:: NonSeekableReaderWriter : public ReaderWriter

   A ReaderWriter that does not support seeking. The Seek() method is hidden.

   Use in APIs when:
     * Do NOT use in APIs! If seeking is not required, use ReaderWriter&
       instead.

   Inherit from when:
     * Implementing a ReaderWriter that does not support seeking.

---------------
Implementations
---------------
``pw_stream`` includes a few stream implementations for general use.

.. cpp:class:: MemoryWriter : public SeekableWriter

  The ``MemoryWriter`` class implements the :cpp:class:`Writer` interface by
  backing the data destination with an **externally-provided** memory buffer.
  ``MemoryWriterBuffer`` extends ``MemoryWriter`` to internally provide a memory
  buffer.

  The ``MemoryWriter`` can be accessed like a standard C++ container. The
  contents grow as data is written.

.. cpp:class:: MemoryReader : public SeekableReader

  The ``MemoryReader`` class implements the :cpp:class:`Reader` interface by
  backing the data source with an **externally-provided** memory buffer.

.. cpp:class:: NullReaderWriter : public SeekableReaderWriter

  ``NullReaderWriter`` is a no-op stream implementation, similar to
  ``/dev/null``. Writes are always dropped. Reads always return
  ``OUT_OF_RANGE``. Seeks have no effect.

.. cpp:class:: StdFileWriter : public SeekableWriter

  ``StdFileWriter`` wraps an ``std::ofstream`` with the :cpp:class:`Writer`
  interface.

.. cpp:class:: StdFileReader : public SeekableReader

  ``StdFileReader`` wraps an ``std::ifstream`` with the :cpp:class:`Reader`
  interface.

.. cpp:class:: SocketStream : public NonSeekableReaderWriter

  ``SocketStream`` wraps posix-style sockets with the :cpp:class:`Reader` and
  :cpp:class:`Writer` interfaces.

------------------
Why use pw_stream?
------------------

Standard API
============
``pw_stream`` provides a standard way for classes to express that they have the
ability to write data. Writing to one sink versus another sink is a matter of
just passing a reference to the appropriate :cpp:class:`Writer`.

As an example, imagine dumping sensor data. If written against a random HAL
or one-off class, there's porting work required to write to a different sink
(imagine writing over UART vs dumping to flash memory). Building a "dumping"
implementation against the :cpp:class:`Writer` interface prevents a dependency
on a bespoke API that would require porting work.

Similarly, after building a :cpp:class:`Writer` implementation for a Sink that
data could be dumped to, that same :cpp:class:`Writer` can be reused for other
contexts that already write data to the :cpp:class:`pw::stream::Writer`
interface.

Before:

.. code-block:: cpp

  // Not reusable, depends on `Uart`.
  void DumpSensorData(Uart& uart) {
    static char temp[64];
    ImuSample imu_sample;
    imu.GetSample(&info);
    size_t bytes_written = imu_sample.AsCsv(temp, sizeof(temp));
    uart.Transmit(temp, bytes_written, /*timeout_ms=*/ 200);
  }

After:

.. code-block:: cpp

  // Reusable; no more Uart dependency!
  Status DumpSensorData(Writer& writer) {
    static char temp[64];
    ImuSample imu_sample;
    imu.GetSample(&info);
    size_t bytes_written = imu_sample.AsCsv(temp, sizeof(temp));
    return writer.Write(temp, bytes_written);
  }

Reduce intermediate buffers
===========================
Often functions that write larger blobs of data request a buffer is passed as
the destination that data should be written to. This *requires* a buffer to be
allocated, even if the data only exists in that buffer for a very short period
of time before it's written somewhere else.

In situations where data read from somewhere will immediately be written
somewhere else, a :cpp:class:`Writer` interface can cut out the middleman
buffer.

Before:

.. code-block:: cpp

  // Requires an intermediate buffer to write the data as CSV.
  void DumpSensorData(Uart& uart) {
    char temp[64];
    ImuSample imu_sample;
    imu.GetSample(&info);
    size_t bytes_written = imu_sample.AsCsv(temp, sizeof(temp));
    uart.Transmit(temp, bytes_written, /*timeout_ms=*/ 200);
  }

After:

.. code-block:: cpp

  // Both DumpSensorData() and RawSample::AsCsv() use a Writer, eliminating the
  // need for an intermediate buffer.
  Status DumpSensorData(Writer& writer) {
    RawSample imu_sample;
    imu.GetSample(&info);
    return imu_sample.AsCsv(writer);
  }

Prevent buffer overflow
=======================
When copying data from one buffer to another, there must be checks to ensure the
copy does not overflow the destination buffer. As this sort of logic is
duplicated throughout a codebase, there's more opportunities for bound-checking
bugs to sneak in. ``Writers`` manage this logic internally rather than pushing
the bounds checking to the code that is moving or writing the data.

Similarly, since only the :cpp:class:`Writer` has access to any underlying
buffers, it's harder for functions that share a :cpp:class:`Writer` to
accidentally clobber data written by others using the same buffer.

Before:

.. code-block:: cpp

  Status BuildPacket(Id dest, span<const std::byte> payload,
                     span<std::byte> dest) {
    Header header;
    if (dest.size_bytes() + payload.size_bytes() < sizeof(Header)) {
      return Status::ResourceExhausted();
    }
    header.dest = dest;
    header.src = DeviceId();
    header.payload_size = payload.size_bytes();

    memcpy(dest.data(), &header, sizeof(header));
    // Forgetting this line would clobber buffer contents. Also, using
    // a temporary span instead could leave `dest` to be misused elsewhere in
    // the function.
    dest = dest.subspan(sizeof(header));
    memcpy(dest.data(), payload.data(), payload.size_bytes());
  }

After:

.. code-block:: cpp

  Status BuildPacket(Id dest, span<const std::byte> payload, Writer& writer) {
    Header header;
    header.dest = dest;
    header.src = DeviceId();
    header.payload_size = payload.size_bytes();

    writer.Write(header);
    return writer.Write(payload);
  }

------------
Design notes
------------

Sync & Flush
============
The :cpp:class:`pw::stream::Stream` API does not include ``Sync()`` or
``Flush()`` functions. There no mechanism in the :cpp:class:`Stream` API to
synchronize a :cpp:class:`Reader`'s potentially buffered input with its
underlying data source. This must be handled by the implementation if required.
Similarly, the :cpp:class:`Writer` implementation is responsible for flushing
any buffered data to the sink.

``Flush()`` and ``Sync()`` were excluded from :cpp:class:`Stream` for a few
reasons:

  * The semantics of when to call ``Flush()``/``Sync()`` on the stream are
    unclear. The presence of these methods complicates using a
    :cpp:class:`Reader` or :cpp:class:`Writer`.
  * Adding one or two additional virtual calls increases the size of all
    :cpp:class:`Stream` vtables.

Class hierarchy
===============
All ``pw_stream`` classes inherit from a single, common base with all possible
functionality: :cpp:class:`pw::stream::Stream`. This structure has
some similarities with Python's `io module
<https://docs.python.org/3/library/io.html>`_ and C#'s `Stream class
<https://docs.microsoft.com/en-us/dotnet/api/system.io.stream>`_.

An alternative approach is to have the reading, writing, and seeking portions of
the interface provided by different entities. This is how Go's `io
<https://pkg.go.dev/io package>`_ and C++'s `input/output library
<https://en.cppreference.com/w/cpp/io>`_ are structured.

We chose to use a single base class for a few reasons:

* The inheritance hierarchy is simple and linear. Despite the linear
  hierarchy, combining capabilities is natural with classes like
  :cpp:class:`ReaderWriter`.

  In C++, separate interfaces for each capability requires either a complex
  virtual inheritance hierarchy or entirely separate hierarchies for each
  capability. Separate hierarchies can become cumbersome when trying to
  combine multiple capabilities. A :cpp:class:`SeekableReaderWriter` would
  have to implement three different interfaces, which means three different
  vtables and three vtable pointers in each instance.
* Stream capabilities are clearly expressed in the type system, while
  naturally supporting optional functionality. A :cpp:class:`Reader` may
  or may not support :cpp:func:`Stream::Seek`. Applications that can handle
  seek failures gracefully way use seek on any :cpp:class:`Reader`. If seeking
  is strictly necessary, an API can accept a :cpp:class:`SeekableReader`
  instead.

  Expressing optional functionality in the type system is cumbersome when
  there are distinct interfaces for each capability. ``Reader``, ``Writer``,
  and ``Seeker`` interfaces would not be sufficient. To match the flexibility
  of the current structure, there would have to be separate optional versions
  of each interface, and classes for various combinations. :cpp:class:`Stream`
  would be an "OptionalReaderOptionalWriterOptionalSeeker" in this model.
* Code reuse is maximized. For example, a single
  :cpp:func:`Stream::ConservativeLimit` implementation supports many stream
  implementations.

Virtual interfaces
==================
``pw_stream`` uses virtual functions. Virtual functions enable runtime
polymorphism. The same code can be used with any stream implementation.

Virtual functions have inherently has more overhead than a regular function
call. However, this is true of any polymorphic API. Using a C-style ``struct``
of function pointers makes different trade-offs but still has more overhead than
a regular function call.

For many use cases, the overhead of virtual calls insignificant. However, in
some extremely performance-sensitive contexts, the flexibility of the virtual
interface may not justify the performance cost.

Asynchronous APIs
=================
At present, ``pw_stream`` is synchronous. All :cpp:class:`Stream` API calls are
expected to block until the operation is complete. This might be undesirable
for slow operations, like writing to NOR flash.

Pigweed has not yet established a pattern for asynchronous C++ APIs. The
:cpp:class:`Stream` class may be extended in the future to add asynchronous
capabilities, or a separate ``AsyncStream`` could be created.

------------
Dependencies
------------
  * :ref:`module-pw_assert`
  * :ref:`module-pw_preprocessor`
  * :ref:`module-pw_status`
  * :ref:`module-pw_span`

.. cpp:namespace-pop::

Zephyr
======
To enable ``pw_stream`` for Zephyr add ``CONFIG_PIGWEED_STREAM=y`` to the
project's configuration.
