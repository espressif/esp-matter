.. _module-pw_snapshot:

===========
pw_snapshot
===========

.. warning::

  This module is unstable and under active development. The snapshot proto
  format may see breaking changes as it stabilizes.

``pw_snapshot`` provides a storage format and associated tooling for capturing a
device's system state at a given point in time for analysis at a later time.
This is particularly useful for capturing information at crash time to provide
context to the cause of the crash. Outside of crash reporting, snapshots can be
used to debug anomalies that don't result in crashes by treating snapshotting as
a heavyweight alternative to tracing, logging-based dump commands, or other
on-demand system state capturing.


.. toctree::
  :maxdepth: 1

  setup
  module_usage
  proto_format
  design_discussion

------------------
Life of a Snapshot
------------------
A "snapshot" is just a `proto message
<https://cs.pigweed.dev/pigweed/+/HEAD:pw_snapshot/pw_snapshot_protos/snapshot.proto>`_
with many optional fields that describe a device's state at the time the
snapshot was captured. The serialized proto can then be stored and transfered
like a file so it can be analyzed at a later time.

#. **Snapshot capture triggered** - The device encounters a condition that
   indicates a snapshot should be captured. This could be through a crash
   handler, or through other developer-specified entry points.
#. **Device "pauses"** - In order to capture system state, the device must
   temporarily disable the thread scheduler and regular servicing of interrupts
   to prevent the system state from churning while it is captured.
#. **Snapshot captured** - The device collects information throughout the
   system through a project-provided snapshot collection logic flow. This data
   is stored as a serialized Snapshot proto message for later retrieval.
#. **Device resumes** - After a snapshot is stored, the device resumes normal
   execution. In a crash handler, the device will usually reboot instead of
   returning to normal execution.
#. **Snapshot retrieved from device** - During normal device operation, stored
   snapshots are retrieved from a device by a client that is interested in
   analyzing the snapshot, or forwarding it elsewhere to be analyzed.
#. **Snapshot analyzed** - Finally, analysis tooling is run on the captured
   snapshot proto to produce human readable dumps (akin to a crash report).
   Alternatively, the data can be ingested by a server to act as a cloud crash
   reporting endpoint. The structured form of a snapshot enables common
   cloud-based crash reporting needs like version filtering, crash signatures,
   de-duplication, and binary-matched symbolization.

While Pigweed provides libraries for each part of a snapshot's lifecycle, the
glue that puts all these pieces together is project specific. Please see the
section on :ref:`Setting up a Snapshot Pipeline<module-pw_snapshot-setup>` for
more information on how to bring up snapshot support for your project.
