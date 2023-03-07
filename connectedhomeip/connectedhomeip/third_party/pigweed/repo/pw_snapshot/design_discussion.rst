.. _module-pw_snapshot-design_discussion:

=================
Design Discussion
=================
There were a handful of key requirements going into the design of pw_snapshot:

* **Pre-established file format** - Building and maintaining tooling to support
  parsing binary snapshot data is a high maintenance burden that detracts from
  the appeal of a pre-existing widely known/supported format.
* **Incremental writing** - Needing to build an entire snapshot before
  committing it as a finished file is a big limitation on embedded devices where
  RAM is often very constrained. It is important that a snapshot can be built in
  smaller in-memory segments that can be committed incrementally to a larger
  sink (e.g. UART, off-chip flash).
* **Extensible** - Pigweed doesn't know everything users might want to capture
  in a snapshot. It's important that users have ways to include their own
  information into snapshots with minimal friction.
* **Relatively compact** - It's important that snapshots can contain useful
  information even when they are limited to a few hundred bytes in size.

Why Proto?
==========
Protobufs are widely used and supported across many languages and platforms.
This greatly reduces the encode/decode tooling maintenance introduced by using
custom or unstructured formats. While using a format like JSON provides
similarly wide tooling support, encoding the same information as a proto
significantly reduces the final file size.

While protobuffer messages aren't truly streamable (i.e. can be written without
any intermediate buffers) due to how message nesting works, a large message can
be incrementally written as long as there's enough buffer space for encoding the
largest single sub-message in the proto.

Why overlay multiple protos?
============================
Proto 2 supported a feature called "extensions" that explicitly allowed this
behavior. While proto 3 removed this feature, it doesn't disallow the old
behavior of serializing two 'overlayed' protos to the same data stream. Proto 3
recommends using an "Any" proto instead of extensions, as it is more explicit
and eliminates the issue of collisions in proto messages. Unfortunately, proto
'Any' messages introduce unacceptable overhead. For a single integer that would
encode to a few bytes using extensions, an Any submessage quickly expands to
tens of bytes.

pw_snapshot's proto format takes advantage of "extensions" from proto 2 without
explicitly relying on the feature. To reduce the risk of colissions and maximize
encoding efficiency, certain ranges are reserved to allow Pigweed to grow while
ensuring downstream customers have equivalent flexibility when using the
Snapshot proto format.

Why no file header?
===================
Right now it's assumed that anything that is storing or transferring a
serialized snapshot implicitly tracks its size (and a checksum, if desired).
While a container format might be introduced independently, pw_snapshot focuses
on treating an encoded snapshot as raw serialized proto data.
