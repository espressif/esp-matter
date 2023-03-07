.. _module-pw_ring_buffer:

--------------
pw_ring_buffer
--------------
The ``pw_ring_buffer`` module will eventually provide several ring buffer
implementations, each with different tradeoffs.

This documentation is incomplete :)

Compatibility
=============
* C++14

Iterator
========
In crash contexts, it may be useful to scan through a ring buffer that may
have a mix of valid (yet to be read), stale (read), and invalid entries. The
`PrefixedEntryRingBufferMulti::iterator` class can be used to walk through
entries in the provided buffer.

.. code-block:: cpp

  // A test string to push into the buffer.
  constexpr char kExampleEntry[] = "Example!";

  // Setting up buffers and attaching a reader.
  std::byte buffer[1024];
  std::byte read_buffer[256];
  PrefixedEntryRingBuffer ring_buffer(buffer);
  PrefixedEntryRingBuffer::Reader reader;
  ring_buffer.AttachReader(reader);

  // Insert some entries and process some entries.
  ring_buffer.PushBack(kExampleEntry);
  ring_buffer.PushBack(kExampleEntry);
  reader.PopFront();

  // !! A function causes a crash before we've read out all entries.
  FunctionThatCrashes();

  // ... Crash Context ...

  // You can use a range-based for-loop to walk through all entries.
  for (auto entry : ring_buffer) {
    PW_LOG_WARN("Read entry of size: %lu", entry.size());
  }

In cases where a crash has caused the ring buffer to have corrupted data, the
iterator will progress until it sees the corrupted section and instead move to
`iterator::end()`. The `iterator::status()` function returns a `pw::Status`
indicating the reason the iterator reached it's end.

.. code-block:: cpp

   // ... Crash Context ...

   using iterator = PrefixedEntryRingBufferMulti::iterator;

   // Hold the iterator outside any loops to inspect it later.
   iterator it = ring_buffer.begin();
   for (; it != it.end(); ++it) {
     PW_LOG_WARN("Read entry of size: %lu", it->size());
   }

   // Warn if there was a failure during iteration.
   if (!it.status().ok()) {
     PW_LOG_WARN("Iterator failed to read some entries!");
   }

Data corruption
===============
``PrefixedEntryRingBufferMulti`` offers a circular ring buffer for arbitrary
length data entries. Some metadata bytes are added at the beginning of each
entry to delimit the size of the entry. Unlike the iterator, the methods in
``PrefixedEntryRingBufferMulti`` require that data in the buffer is not corrupt.
When these methods encounter data corruption, there is no generic way to
recover, and thus, the application crashes. Data corruption is indicative of
other issues.

Dependencies
============
* ``pw_span``
* ``pw_containers`` - for tests only
