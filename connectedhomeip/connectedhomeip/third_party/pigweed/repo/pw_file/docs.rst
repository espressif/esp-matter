.. _module-pw_file:

=======
pw_file
=======

.. attention::

  ``pw_file`` is under construction, and may see significant breaking API
  changes.

The ``pw_file`` module defines a service for file system-like interactions
between a client and server. FileSystem services may be backed by true file
systems, or by virtual file systems that provide a file-system like interface
with no true underlying file system.

pw_file does not define a protocol for file transfers.
:ref:`module-pw_transfer` provides a generalized mechanism for
performing file transfers, and is recommended to be used in tandem with pw_file.

-----------
RPC service
-----------
The FileSystem RPC service is oriented to allow direct interaction, and has no
sequenced protocol. Unlike FTP, all interactions are stateless. This service
also does not yet define any authentication mechanism, meaning that all clients
that can access a FileSystem service are granted equal permissions.

.. literalinclude:: file.proto
  :language: protobuf
  :lines: 14-

------------------------------
Flat FileSystem implementation
------------------------------
This module provides the ``FlatFileSystemService``, an optional implementation
of the FileSystem RPC service with a virtual interface that allows different
data storage implementations to expose logical files. As the name implies, the
file system is treated as a flat file system; it does not support any
directory-like interactions.

The ``FlatFileSystemService`` implementation requires a static, fixed list of
``Entry`` pointers. Each ``Entry`` represents a potential
file, and acts as an interface boundary that is backed by some kind of storage
mechanism (e.g. ``BlobStore``, ``PersistentBuffer``).

All ``Entry`` objects that should be enumerated by a
``FlatFileSystemService`` MUST be named, and names must be globally unique to
prevent ambiguity. Unnamed file entries will NOT be enumerated by a
``FlatFileSystemService``, and are considered empty/deleted files. It is valid
to have empty files that are enumerated with a name.

``FlatFileSystemService`` requires two buffers at construction: one buffer for
reading file names and another for encoding protobuf responses. The recommended
encoding buffer size for a particular maximum file name length can be calculated
with ``EncodingBufferSizeBytes``. For convenience, the
``FlatFileSystemServiceWithBuffer<kMaxFileNameLength>`` class is provided. That
class creates a ``FlatFileSystemService`` with a buffer automatically sized
based on the maximum file name length.
