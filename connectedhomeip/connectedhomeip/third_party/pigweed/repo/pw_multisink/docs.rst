.. _module-pw_multisink:

============
pw_multisink
============
This is an module that forwards messages to multiple attached sinks, which
consume messages asynchronously. It is not ready for use and is under
construction.

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration
of this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_MULTISINK_CONFIG_LOCK_INTERRUPT_SAFE

  Whether an interrupt-safe lock is used to guard multisink read/write
  operations.

  By default, this option is enabled and the multisink uses an interrupt
  spin-lock to guard its transactions. If disabled, a mutex is used instead.

  Disabling this will alter the entry precondition of the multisink,
  requiring that it not be called from an interrupt context.

Late Drain Attach
=================
It is possible to push entries or inform the multisink of drops before any
drains are attached to it, allowing you to defer the creation of the drain
further into an application. The multisink maintains the location and drop
count of the oldest drain and will set drains to match on attachment. This
permits drains that are attached late to still consume any entries that were
pushed into the ring buffer, so long as those entries have not yet been evicted
by newer entries. This may be particularly useful in early-boot scenarios where
drain consumers may need time to initialize their output paths. Listeners are
notified immediately when attached, to allow late drain users to consume
existing entries. If draining in response to the notification, ensure that
the drain is attached prior to registering the listener; attempting to drain
when unattached will crash.

.. code-block:: cpp

  // Create a multisink during global construction.
  std::byte buffer[1024];
  MultiSink multisink(buffer);

  int main() {
    // Do some initialization work for the application that pushes information
    // into the multisink.
    multisink.HandleEntry("Booting up!");
    Initialize();

    multisink.HandleEntry("Prepare I/O!");
    PrepareIO();

    // Start a thread to process logs in multisink.
    StartLoggingThread();
  }

  void StartLoggingThread() {
    MultiSink::Drain drain;
    multisink.AttachDrain(drain);

    std::byte read_buffer[512];
    uint32_t drop_count = 0;
    do {
      Result<ConstByteSpan> entry = drain.PopEntry(read_buffer, drop_count);
      if (drop_count > 0) {
        StringBuilder<32> sb;
        sb.Format("Dropped %d entries.", drop_count);
        // Note: PrintByteArray is not a provided utility function.
        PrintByteArray(sb.as_bytes());
      }

      // Iterate through the entries, this will print out:
      //   "Booting up!"
      //   "Prepare I/O!"
      //
      // Even though the drain was attached after entries were pushed into the
      // multisink, this drain will still be able to consume those entries.
      //
      // Note: PrintByteArray is not a provided utility function.
      if (entry.status().ok()) {
        PrintByteArray(read_buffer);
      }
    } while (true);
  }

Iterator
========
It may be useful to access the entries in the underlying buffer when no drains
are attached or in crash contexts where dumping out all entries is desirable,
even if those entries were previously consumed by a drain. This module provides
an iteration class that is thread-unsafe and like standard iterators, assumes
that the buffer is not being mutated while iterating. A
``MultiSink::UnsafeIterationWrapper`` class that supports range-based for-loop
usage can be acquired via ``MultiSink::UnsafeIteration()``.

The iterator starts from the oldest available entry in the buffer, regardless of
whether all attached drains have already consumed that entry. This allows the
iterator to be used even if no drains have been previously attached.

.. code-block:: cpp

  // Create a multisink and a test string to push into it.
  constexpr char kExampleEntry[] = "Example!";
  std::byte buffer[1024];
  MultiSink multisink(buffer);
  MultiSink::Drain drain;

  // Push an entry before a drain is attached.
  multisink.HandleEntry(kExampleEntry);
  multisink.HandleEntry(kExampleEntry);

  // Iterate through the entries, this will print out:
  //  "Example!"
  //  "Example!"
  // Note: PrintByteArray is not a provided utility function.
  for (ConstByteSpan entry : multisink.UnsafeIteration()) {
    PrintByteArray(entry);
  }

  // Attach a drain and consume only one of the entries.
  std::byte read_buffer[512];
  uint32_t drop_count = 0;

  multisink.AttachDrain(drain);
  drain.PopEntry(read_buffer, drop_count);

  // !! A function causes a crash before we've read out all entries.
  FunctionThatCrashes();

  // ... Crash Context ...

  // You can use a range-based for-loop to walk through all entries,
  // even though the attached drain has consumed one of them.
  // This will also print out:
  //  "Example!"
  //  "Example!"
  for (ConstByteSpan entry : multisink.UnsafeIteration()) {
    PrintByteArray(entry);
  }

As an alternative to using the ``UnsafeIterationWrapper``,
``MultiSink::UnsafeForEachEntry()`` may be used to run a callback for each
entry in the buffer. This helper also provides a way to limit the iteration to
the ``N`` most recent entries.

Peek & Pop
==========
A drain can peek the front multisink entry without removing it using
`PeekEntry`, which is the same as `PopEntry` without removing the entry from the
multisink. Once the drain is done with the peeked entry, `PopEntry` will tell
the drain to remove the peeked entry from the multisink and advance one entry.

.. code-block:: cpp

  constexpr char kExampleEntry[] = "Example!";
  std::byte buffer[1024];
  MultiSink multisink(buffer);
  MultiSink::Drain drain;

  multisink.AttachDrain(drain);
  multisink.HandleEntry(kExampleEntry);

  std::byte read_buffer[512];
  uint32_t drop_count = 0;
  Result<PeekedEntry> peeked_entry = drain.PeekEntry(read_buffer, drop_count);
  // ... Handle drop_count ...

  if (peeked_entry.ok()) {
    // Note: SendByteArray is not a provided utility function.
    Status send_status = SendByteArray(peeked_entry.value().entry());
    if (send_status.ok()) {
      drain.PopEntry(peeked_entry.value());
    } else {
      // ... Handle send error ...
    }
  }

Drop Counts
===========
The `PeekEntry` and `PopEntry` return two different drop counts, one for the
number of entries a drain was skipped forward for providing a small buffer or
draining too slow, and the other for entries that failed to be added to the
MultiSink.
