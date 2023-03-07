.. _module-pw_kvs:

======
pw_kvs
======
.. note::
  The documentation for this module is currently under construction.

``pw_kvs`` is Pigweed's Key Value Store (KVS) library. KVS is a flash-backed
persistent storage system with integrated wear-leveling that serves as a
relatively lightweight alternative to a file system.

-------------
KeyValueStore
-------------
The KVS system stores key and value data pairs. The key value pairs are stored
in `flash memory`_ as a `key-value entry`_ (KV entry) that consists of a
header/metadata, the key data, and value data. KV entries are accessed through
Put, Get, and Delete operations.

Each flash sector is written sequentially in an append-only manner, with each
following entry write being at a higher address than all of the previous entry
writes to that sector since erase. Once information (header, metadata, data,
etc) is written to flash, that information is not modified or cleared until a
full sector erase occurs as part of garbage collection.

Individual KV entries are contained within a single flash sector (do not cross
sector boundaries). Flash sectors can contain as many KV entries as fit in the
sector.

KVS does not store any data/metadata/state in flash beyond the KV entries. All
KVS system state can be derived from the stored KV entries. Current KVS system
state is determined at boot from flash-stored KV entries and then maintained in
ram by the KVS. The KVS is at all times in a valid state on-flash, so there are
no windows of vulnerability to unexpected power loss or crash. The old entry
for a key is maintained until the new entry for that key is written and
verified.

Each `key-value entry`_ has a unique transaction ID that is incremented for
each KVS update transaction. When determining system state from flash-stored KV
entries, the valid entry with the highest transaction ID is considered to be
the “current” entry of the key. All stored entries of the same key with lower
transaction ID are considered old or “stale”.

Updates/rewrites of a key that has been previously stored is done as a new KV
entry with an updated transaction ID and the new value for the key. The KVS
internal state is updated to reflect the new entry. The previously stored KV
entries for that key are not modified or removed from flash storage, until
garbage collection reclaims the “stale” entries.

`Garbage collection`_ is done by copying any currently valid KV entries in the
sector to be garbage collected to a different sector and then erasing the
sector.

Flash Memory
============
The flash storage used by KVS is comprised of two layers, FlashMemory and
FlashPartition.

FlashMemory is the lower level that manages the raw read/write/erase of the
flash memory device.

FlashPartition is a subset of a FlashMemory. A FlashMemory may have one or
multiple FlashPartitions that represent different parts of the FlashMemory -
such as partitions for KVS, OTA, snapshots/crashlogs, etc. Each FlashPartition
has its own separate logical address space starting from zero to size bytes of
the partition. FlashPartition logical address does not always map directly to
FlashMemory addresses due to partition encryption, sector headers, etc.

Writes to flash must have a start address that is a multiple of the flash
write alignment. Write size must also be a multiple of flash write alignment.
Write alignment varies by flash device and partition type. Reads from flash do
not have any address or size alignment requirement - reads always have a
minimum alignment of 1.

FlashPartitions may have a different alignment than the FlashMemory they are
part of, so long as the partition's alignment is a multiple of the alignment
for the FlashMemory.

Sectors are the minimum erase size for both FlashMemory and FlashPartition.
FlashPartitions may have a different logical sector size than the FlashMemory
they are part of. Partition logical sectors may be smaller due to partition
overhead (encryption, wear tracking, etc) or larger due to combining raw
sectors into larger logical sectors.

FlashPartition supports access via NonSeekableWriter and SeekableReader.

Size report
===========
The following size report showcases the memory usage of the KVS and
FlashPartition.

.. include:: kvs_size

Storage Allocation
==================
KVS requires more storage space than the size of the key-value data stored.
This is due to the always free sector required for garbage collection and the
"write and garbage collect later" approach KVS uses.

KVS works poorly with stored data being more than 75% of the available
storage. It works best with stored data being less than 50% of the available
storage. For applications that prefer/need to do garbage collection at
scheduled times or that write very heavily can benefit from additional flash
store space.

The flash storage used by KVS is multiplied by `redundancy`_ used. A redundancy
of 2 will use twice the storage.

Key-Value Entry
===============
Each key-value (KV) entry consists of a header/metadata, the key data, and
value data. Individual KV entries are contained within a single flash sector
(do not cross sector boundaries). Because of this the maximum KV entry size is
the partition sector size.

KV entries are appended as needed to sectors, with append operations spread
over time. Each individual KV entry is written completely as a single
high-level operation. KV entries are appended to a sector as long as space is
available for a given KV entry. Multiple sectors can be active for writing at
any time.

When a key is rewritten (writing a new KV entry of an existing key), the KV
entry is stored at a new location that may or may not be located in the same
sector as the previous entry for that key. The new entry uses a transaction
ID greater than the previous transaction ID. The previous KV entry for that key
remains unaltered “on-disk” but is considered “stale”. It is garbage collected
at some future time.

Redundancy
==========
KVS supports storing redundant copies of KV entries. For a given redundancy
level (N), N total copies of each KV entry are stored. Redundant copies are
always stored in different sectors. This protects against corruption or even
full sector loss in N-1 sectors without data loss.

Redundancy increases flash usage proportional to the redundancy level. The RAM
usage for KVS internal state has a small increase with redundancy.

Garbage Collection
==================
Storage space occupied by stale KV entries is reclaimed and made available
for reuse through a garbage collection process. The base garbage collection
operation is done to reclaim one sector at a time.

KVS always keeps at least one sector free at all times to ensure the ability to
garbage collect. This free sector is used to copy valid entries from the sector
to be garbage collected before erasing the sector to be garbage collected. The
always free sector is rotated as part of the KVS wear leveling.

Full Maintenance does garbage collection of all sectors except those that have
current valid KV entries.

Heavy Maintenance does garbage collection of all sectors, including removing
entries for deleted keys. Use strong caution when doing Heavy Maintenance as it
can, compared to Full Maintenance, result in a significant amount of moving
valid entries,

Garbage collection can be performed by request of higher level software or
automatically as needed to make space available to write new entries.

Flash wear management
=====================
Wear leveling is accomplished by cycling selection of the next sector to write
to. This cycling spreads flash wear across all free sectors so that no one
sector is prematurely worn out.

Wear leveling through cycling selection of next sector to write

* Location of new writes/rewrites of key-values will prefer sectors already
  in-use (partially filled), with new (blank) sectors used when no in-use
  sectors have large enough available space for the new write
* New (blank) sectors selected cycle sequentially between available free
  sectors
* Search for the first available sector, starting from current write sector + 1
  and wrap around to start at the end of partition.
* This spreads the erase/write cycles for heavily written/rewritten key-values
  across all free sectors, reducing wear on any single sector
* Erase count is not considered as part of the wear leveling decision making
  process
* Sectors with already written key-values that are not modified will remain in
  the original sector and not participate in wear-leveling, so long as the
  key-values in the sector remain unchanged

Configuration
=============
.. c:macro:: PW_KVS_MAX_FLASH_ALIGNMENT

  The maximum flash alignment supported.

.. c:macro:: PW_KVS_REMOVE_DELETED_KEYS_IN_HEAVY_MAINTENANCE

  Whether to remove deleted keys in heavy maintanence. This feature costs some
  code size (>1KB) and is only necessary if arbitrary key names are used.
  Without this feature, deleted key entries can fill the KVS, making it
  impossible to add more keys, even though most keys are deleted.
