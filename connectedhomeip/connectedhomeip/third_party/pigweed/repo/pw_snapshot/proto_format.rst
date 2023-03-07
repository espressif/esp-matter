.. _module-pw_snapshot-proto_format:

=====================
Snapshot Proto Format
=====================
The Snapshot proto format depends on proto handling properties that directly
inform how the proto should be used. If two proto messages use field numbers
in a mutually exclusive manner, they can be encoded to the same buffer, and
decoded independently without any errors. An example is illustrated below:

.. code-block::

  // This message uses field numbers 1, 2, and 3.
  message BasicLog {
    string message = 1;
    LogLevel level = 2;
    int64 timestamp = 3;
  }

  // This message uses field numbers 16 and 17, which are mutually exclusive
  // to the numbers used in BasicLog.
  message ExtendedLog {
    string file_name = 16;
    uint32 line_nubmer = 17;
  }

In the above example, a BasicLog and ExtendedLog can be encoded to the same
buffer and then be decoded without causing any problems. What breaks
this is when either of the two messages above are updated to use the same field
number for different types. This can be ameliorated by specifying reserved
field number ranges as shown below:

.. code-block::

  message BasicLog {
    string message = 1;
    LogLevel level = 2;
    int64 timestamp = 3;

    // ExtendedLog uses these field numbers. These field numbers should never
    // be used by BasicLog.
    reserved 16 to max;
  }

  message ExtendedLog {
    // BasicLog uses these field numbers. These field numbers should never
    // be used by ExtendedLog.
    reserved 1 to 15;

    string file_name = 16;
    uint32 line_nubmer = 17;
  }

This is exactly how the Snapshot proto is set up. While a SnapshotMetadata proto
message provides a good portion of the valuable snapshot contents, the larger
Snapshot message specifies field numbers and reserved field ranges to clarify
which field numbers are left to a project for declaring a mutually exclusive
project-specific proto message that can be used to augment the data natively
supported by the upstream proto.

--------------------
Module-Specific Data
--------------------
Some upstream Pigweed modules provide a ``Snapshot*`` message that overlays the
main Snapshot message. These messages share a field number with the Snapshot
proto, which lets modules provide a helper that populates a subset of the
Snapshot proto without explicitly depending on the Snapshot proto.

Example:
.. code-block::

  // snapshot.proto
  message Snapshot {
    ...
    // Information about allocated Thread.
    repeated pw.thread.Thread threads = 18;
  }

  // thread.proto

  // This message overlays the pw.snapshot.Snapshot proto. It's valid to encode
  // this message to the same sink that a Snapshot proto is being written to.
  message SnapshotThread {
    // Thread information.
    repeated pw.thread.Thread threads = 18;
  }

It is **critical** that the SnapshotThread message is in sync with the larger
Snapshot proto. If the type or field numbers are different, the proto decode
will fail. If the semantics are different, the decode will likely succeed but
might provide misleading information.
