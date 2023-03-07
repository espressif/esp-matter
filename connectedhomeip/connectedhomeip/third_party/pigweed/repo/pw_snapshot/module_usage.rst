.. _module-pw_snapshot-module_usage:

============
Module Usage
============
Right now, pw_snapshot just dictates a *format*. That means there is no provided
system information collection integration, underlying storage, or transport
mechanism to fetch a snapshot from a device. These must be set up independently
by your project.

-------------------
Building a Snapshot
-------------------
Even though a Snapshot is just a proto message, the potential size of the proto
makes it important to consider the encoder.

Nanopb is a popular encoder for embedded devices, it's impractical to use
with the pw_snapshot proto. Nanopb works by generating in-memory structs that
represent the protobuf message. Repeated, optional, and variable-length fields
increase the size of the in-memory struct. The struct representation
of snapshot-like protos can quickly near 10KB in size. Allocating 10KB

Pigweed's pw_protobuf is a better choice as its design is centered around
incrementally writing a proto directly to the final wire format. If you only
write a few fields in a snapshot, you can do so with minimal memory overhead.

.. code-block:: cpp

  #include "pw_bytes/span.h"
  #include "pw_protobuf/encoder.h"
  #include "pw_snapshot_protos/snapshot.pwpb.h"
  #include "pw_status/status.h"
  #include "pw_stream/stream.h"

  pw::Status EncodeSnapshot(pw::stream::Writer& writer,
                            pw::ByteSpan submessage_encode_buffer,
                            const CrashInfo &crash_info) {
    // Create a snapshot proto encoder.
    pw::snapshot::Snapshot::StreamEncoder snapshot_encoder(
        writer, submessage_encode_buffer);
    {  // This scope is required to handle RAII behavior of the submessage.
      // Start writing the Metadata submessage.
      pw::snapshot::Metadata::StreamEncoder metadata_encoder =
          snapshot_encoder.GetMetadataEncoder();
      metadata_encoder.WriteReason(EncodeReasonLog(crash_info));
      metadata_encoder.WriteFatal(true);
      metadata_encoder.WriteProjectName(pw::as_bytes(pw::span("smart-shoe")));
      metadata_encoder.WriteDeviceName(
          pw::as_bytes(pw::span("smart-shoe-p1")));
    }
    return proto_encoder.status();
  }

-------------------
Custom Project Data
-------------------
There are two main ways to add custom project-specific data to a snapshot. Tags
are the simplest way to capture small snippets of information that require
no or minimal post-processing. For more complex data, it's usually more
practical to extend the Snapshot proto.

Tags
====
Adding a key/value pair to the tags map is straightforward when using
pw_protobuf.

.. code-block:: cpp

  {
    pw::Snapshot::TagsEntry::StreamEncoder tags_encoder =
        snapshot_encoder.GetTagsEncoder();
    tags_encoder.WriteKey("BtState");
    tags_encoder.WriteValue("connected");
  }

Extending the Proto
===================
Extending the Snapshot proto relies on proto behavior details that are explained
in the :ref:`Snapshot Proto Format<module-pw_snapshot-proto_format>`. Extending
the snapshot proto is as simple as defining a proto message that **only**
declares fields with numbers that are reserved by the Snapshot proto for
downstream projects. When encoding your snapshot, you can then write both the
upstream Snapshot proto and your project's custom extension proto message to the
same proto encoder.

The upstream snapshot tooling will ignore any project-specific proto data,
the proto data can be decoded a second time using a project-specific proto. At
that point, any handling logic of the project-specific data would have to be
done as part of project-specific tooling.

-------------------
Analyzing Snapshots
-------------------
Snapshots can be processed for analysis using the ``pw_snapshot.process`` python
tool. This tool turns a binary snapshot proto into human readable, actionable
information. As some snapshot fields may optionally be tokenized, a
pw_tokenizer database or ELF file with embedded pw_tokenizer tokens may
optionally be passed to the tool to detokenize applicable fields.

.. code-block:: sh

  # Example invocation, which dumps to stdout by default.
  $ python -m pw_snapshot.processor path/to/serialized_snapshot.bin


          ____ _       __    _____ _   _____    ____  _____ __  ______  ______
         / __ \ |     / /   / ___// | / /   |  / __ \/ ___// / / / __ \/_  __/
        / /_/ / | /| / /    \__ \/  |/ / /| | / /_/ /\__ \/ /_/ / / / / / /
       / ____/| |/ |/ /    ___/ / /|  / ___ |/ ____/___/ / __  / /_/ / / /
      /_/     |__/|__/____/____/_/ |_/_/  |_/_/    /____/_/ /_/\____/ /_/
                    /_____/


                              ▪▄▄▄ ▄▄▄· ▄▄▄▄▄ ▄▄▄· ▄ ·
                              █▄▄▄▐█ ▀█ • █▌ ▐█ ▀█ █
                              █ ▪ ▄█▀▀█   █. ▄█▀▀█ █
                              ▐▌ .▐█ ▪▐▌ ▪▐▌·▐█ ▪▐▌▐▌
                              ▀    ▀  ▀ ·  ▀  ▀  ▀ .▀▀

  Device crash cause:
      ../examples/example_rpc.cc: Assert failed: 1+1 == 42

  Project name:      gShoe
  Device:            GSHOE-QUANTUM_CORE-REV_0.1
  Device FW version: QUANTUM_CORE-0.1.325-e4a84b1a
  FW build UUID:     ad2d39258c1bc487f07ca7e04991a836fdf7d0a0
  Snapshot UUID:     8481bb12a162164f5c74855f6d94ea1a

  Thread State
    2 threads running, Main Stack (Handler Mode) active at the time of capture.
                       ~~~~~~~~~~~~~~~~~~~~~~~~~

  Thread (INTERRUPT_HANDLER): Main Stack (Handler Mode) <-- [ACTIVE]
  Est CPU usage: unknown
  Stack info
    Stack used:   0x2001b000 - 0x2001ae20 (480 bytes)
    Stack limits: 0x2001b000 - 0x???????? (size unknown)
  Raw Stack
  00caadde


  Thread (RUNNING): Idle
  Est CPU usage: unknown
  Stack info
    Stack used:   0x2001ac00 - 0x2001ab0c (244 bytes, 47.66%)
    Stack limits: 0x2001ac00 - 0x2001aa00 (512 bytes)

---------------------
Symbolizing Addresses
---------------------
The snapshot processor tool has built-in support for symbolization of some data
embedded into snapshots. Taking advantage of this requires the use of a
project-provided ``SymbolizerMatcher`` callback. This is used by the snapshot
processor to understand which ELF file should be used to symbolize which
snapshot in cases where a snapshot has related snapshots embedded inside of it.

Here's an example implementation that uses the device name:

.. code-block:: py

  # Given a firmware bundle directory, determine the ELF file associated with
  # the provided snapshot.
  def _snapshot_symbolizer_matcher(fw_bundle_dir: Path,
                                   snapshot: snapshot_pb2.Snapshot
      ) -> Symbolizer:
      metadata = MetadataProcessor(snapshot.metadata, DETOKENIZER)
      if metadata.device_name().startswith('GSHOE_MAIN_CORE'):
          return LlvmSymbolizer(fw_bundle_dir / 'main.elf')
      if metadata.device_name().startswith('GSHOE_SENSOR_CORE'):
          return LlvmSymbolizer(fw_bundle_dir / 'sensors.elf')
      return LlvmSymbolizer()


  # A project specific wrapper to decode snapshots that provides a detokenizer
  # and ElfMatcher.
  def decode_snapshots(snapshot: bytes, fw_bundle_dir: Path) -> str:

      # This is the actual ElfMatcher, which wraps the helper in a lambda that
      # captures the passed firmware artifacts directory.
      matcher: processor.SymbolizerMatcher = (
          lambda snapshot: _snapshot_symbolizer_matcher(
              fw_bundle_dir, snapshot))
      return processor.process_snapshots(snapshot, DETOKENIZER, matcher)

-------------
C++ Utilities
-------------

UUID utilities
==============
Snapshot UUIDs are used to uniquely identify snapshots. Pigweed strongly
recommends using randomly generated data as a snapshot UUID. The
more entropy and random bits, the lower the probability that two devices will
produce the same UUID for a snapshot. 16 bytes should be sufficient for most
projects, so this module provides ``UuidSpan`` and ``ConstUuidSpan`` types that
can be helpful for referring to UUID-sized byte spans.

Reading a snapshot's UUID
-------------------------
An in-memory snapshot's UUID may be read using ``ReadUuidFromSnapshot()``.

.. code-block:: cpp

  void NotifyNewSnapshot(ConstByteSpan snapshot) {
    std::array<std::byte, pw::snapshot::kUuidSizeBytes> uuid;
    pw::Result<pw::ConstByteSpan> result =
        pw::snapshot::ReadUuidFromSnapshot(snapshot, uuid);
    if (!result.ok()) {
      PW_LOG_ERROR("Failed to read UUID from new snapshot, error code %d",
                   static_cast<int>(result.status().code()));
      return;
    }
    LogNewSnapshotUuid(result.value());
  }
