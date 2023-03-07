.. _module-pw_log_rpc:

==========
pw_log_rpc
==========
An RPC-based logging solution for Pigweed with log filtering and log drops
reporting -- coming soon!

.. warning::
  This module is under construction and might change in the future.

-----------
RPC Logging
-----------

How to Use
==========
1. Set up RPC
-------------
Set up RPC for your target device. Basic deployments run RPC over a UART, with
HDLC on top for framing. See :ref:`module-pw_rpc` for details on how to enable
``pw_rpc``.

2. Set up tokenized logging (optional)
--------------------------------------
Set up the :ref:`module-pw_log_tokenized` log backend.

3. Connect the tokenized logging handler to the MultiSink
---------------------------------------------------------
Create a :ref:`MultiSink <module-pw_multisink>` instance to buffer log entries.
Then, make the log backend handler,
``pw_tokenizer_HandleEncodedMessageWithPayload``, encode log entries in the
``log::LogEntry`` format, and add them to the ``MultiSink``.

4. Create log drains and filters
--------------------------------
Create an ``RpcLogDrainMap`` with one ``RpcLogDrain`` for each RPC channel used
to stream logs. Optionally, create a ``FilterMap`` with ``Filter`` objects with
different IDs. Provide these map to the ``LogService`` and register the latter
with the application's RPC service. The ``RpcLogDrainMap`` provides a convenient
way to access and maintain each ``RpcLogDrain``. Attach each ``RpcLogDrain`` to
the ``MultiSink``. Optionally, set the ``RpcLogDrain`` callback to decide if a
log should be kept or dropped. This callback can be ``Filter::ShouldDropLog``.

5. Flush the log drains in the background
-----------------------------------------
Depending on the product's requirements, create a thread to flush all
``RpcLogDrain``\s or one thread per drain. The thread(s) must continuously call
``RpcLogDrain::Flush()`` to pull entries from the ``MultiSink`` and send them to
the log listeners. Alternatively, use ``RpcLogDrain::Trickle`` to control the
rate of log entries streamed. Optionally, set up a callback to notify the
thread(s) when a drain is open.

Logging over RPC diagrams
=========================

Sample RPC logs request
-----------------------
The log listener, e.g. a computer, requests logs via RPC. The log service
receives the request and sets up the corresponding ``RpcLogDrain`` to start the
log stream.

.. mermaid::

  graph TD
    computer[Computer]-->pw_rpc;
    pw_rpc-->log_service[LogService];
    log_service-->rpc_log_drain_pc[RpcLogDrain<br>streams to<br>computer];;

Sample logging over RPC
------------------------
Logs are streamed via RPC to a computer, and to another log listener. There can
also be internal log readers, i.e. ``MultiSink::Drain``\s, attached to the
``MultiSink``, such as a writer to persistent memory, for example.

.. mermaid::

  graph TD
    source1[Source 1]-->log_api[pw_log API];
    source2[Source 2]-->log_api;
    log_api-->log_backend[Log backend];
    log_backend-->multisink[MultiSink];
    multisink-->drain[MultiSink::Drain];
    multisink-->rpc_log_drain_pc[RpcLogDrain<br>streams to<br>computer];
    multisink-->rpc_log_drain_other[RpcLogDrain<br>streams to<br>other log listener];
    drain-->other_consumer[Other log consumer<br>e.g. persistent memory];
    rpc_log_drain_pc-->pw_rpc;
    rpc_log_drain_other-->pw_rpc;
    pw_rpc-->computer[Computer];
    pw_rpc-->other_listener[Other log<br>listener];

Components Overview
===================
LogEntry and LogEntries
-----------------------
RPC logging uses ``LogEntry`` to encapsulate each entry's data, such as level,
timestamp, and message. ``LogEntries`` can hold multiple instances of
``LogEntry`` to send more data using fewer transmissions. The ``LogEntries`` has
an optional field for the first message's sequence ID that corresponds to the
count of each ``LogEntry`` that passes the log filter and is sent. A client can
use this sequence ID and the number of messages in a ``LogEntries`` to figure
out if logs were dropped during transmission.

RPC log service
---------------
The ``LogService`` class is an RPC service that provides a way to request a log
stream sent via RPC and configure log filters. Thus, it helps avoid using a
different protocol for logs and RPCs over the same interface(s).
It requires a ``RpcLogDrainMap`` to assign stream writers and delegate the
log stream flushing to the user's preferred method, as well as a ``FilterMap``
to retrieve and modify filters.

RpcLogDrain
-----------
An ``RpcLogDrain`` reads from the ``MultiSink`` instance that buffers logs, then
packs, and sends the retrieved log entries to the log listener. One
``RpcLogDrain`` is needed for each log listener. An ``RpcLogDrain`` needs a
thread to continuously call ``Flush()`` to maintain the log stream. A thread can
maintain multiple log streams, but it must not be the same thread used by the
RPC server, to avoid blocking it.

Each ``RpcLogDrain`` is identified by a known RPC channel ID and requires a
``rpc::RawServerWriter`` to write the packed multiple log entries. This writer
is assigned by the ``LogService::Listen`` RPC.

``RpcLogDrain``\s can also be provided an open RPC writer, to constantly stream
logs without the need to request them. This is useful in cases where the
connection to the client is dropped silently because the log stream can continue
when reconnected without the client requesting logs again if the error handling
is set to ``kIgnoreWriterErrors`` otherwise the writer will be closed.

An ``RpcLogDrain`` must be attached to a ``MultiSink`` containing multiple
``log::LogEntry``\s. When ``Flush`` is called, the drain acquires the
``rpc::RawServerWriter`` 's write buffer, grabs one ``log::LogEntry`` from the
multisink, encodes it into a ``log::LogEntries`` stream, and repeats the process
until the write buffer is full. Then the drain calls
``rpc::RawServerWriter::Write`` to flush the write buffer and repeats the
process until all the entries in the ``MultiSink`` are read or an error is
found.

The user must provide a buffer large enough for the largest entry in the
``MultiSink`` while also accounting for the interface's Maximum Transmission
Unit (MTU). If the ``RpcLogDrain`` finds a drop message count as it reads the
``MultiSink`` it will insert a message in the stream with the drop message
count in the log proto dropped optional field. The receiving end can display the
count with the logs if desired.

RpcLogDrainMap
--------------
Provides a convenient way to access all or a single ``RpcLogDrain`` by its RPC
channel ID.

RpcLogDrainThread
-----------------
The module includes a sample thread that flushes each drain sequentially.
``RpcLogDrainThread`` takes an encoding buffer span at construction.
``RpcLogDrainThreadWithBuffer`` takes a template parameter for the buffer size,
which must be large enough to fit at least one log entry.

Future work might replace this with enqueueing the flush work on a work queue.
The user can also choose to have different threads flushing individual
``RpcLogDrain``\s with different priorities.

When creating a ``RpcLogDrainThread``, the thread can be configured to
rate limit logs by introducing a limit to how many logs can be flushed from
each sink before a configurable sleep period begins to give the sinks time to
handle the flushed logs. For example, if the rate limiting is configured to 2
log bundles per flush with minimum delay of 100ms between flushes, the logging
thread will send at most 20 log bundles per second over each sink. Log bundle
size is dictated by the size of the encode buffer provided to the
RpcLogDrainThread.

Rate limiting is helpful in cases where transient bursts of high volumes of logs
cause transport buffers to saturate. By rate limiting the RPC log drain, the
transport buffers are given time to send data. As long as the average logging
rate is significantly less than the rate limit imposed by the
``RpcLogDrainThread``, the logging pipeline should be more resilient high
volume log bursts.

Rate limiting log drains is particularly helpful for systems that collect logs
to a multisink in bulk when communications aren't available (e.g. collecting
early boot logs until the logging thread starts). If a very full log buffer is
suddenly flushed to the sinks without rate limiting, it's possible to overwhelm
the output buffers if they don't have sufficient headroom.

.. note::
  Introducing a logging drain rate limit will increase logging latency, but
  usually not by much. It's important to tune the rate limit configuration to
  ensure it doesn't unnecessarily introduce a logging bottleneck or
  significantly increase latency.

Calling ``OpenUnrequestedLogStream()`` is a convenient way to set up a log
stream that is started without the need to receive an RCP request for logs.

The ``RpcLogDrainThread`` sets up a callback for each drain, to be notified when
a drain is opened and flushing must resume.

---------
Log Drops
---------
Unfortunately, logs can be dropped and not reach the destination. This module
expects to cover all cases and be able to notify the user of log drops when
possible. Logs can be dropped when

- They don't pass a filter. This is the expected behavior, so filtered logs will
  not be tracked as dropped logs.
- The drains are too slow to keep up. In this case, the ring buffer is full of
  undrained entries; when new logs come in, old entries are dropped. The log
  stream will contain a ``LogEntry`` message with the number of dropped logs.
  E.g.

      Dropped 15 logs due to slow reader

- There is an error creating or adding a new log entry, and the ring buffer is
  notified that the log had to be dropped. The log stream will contain a
  ``LogEntry`` message with the number of dropped logs.
  E.g.

      Dropped 15 logs due to slow reader

- A log entry is too large for the stack buffer. The log stream will contain
  an error message with the drop count. Provide a log buffer that fits the
  largest entry added to the MultiSink to avoid this error.
  E.g.

      Dropped 1 log due to stack buffer too small

- A log entry is too large for the outbound buffer. The log stream will contain
  an error message with the drop count. Provide a log buffer that fits the
  largest entry added to the MultiSink to avoid this error.
  E.g.

      Dropped 1 log due to outbound buffer too small

- There are detected errors transmitting log entries. The log stream will
  contain a ``LogEntry`` with an error message and the number of dropped logs
  the next time the stream is flushed only if the drain's error handling is set
  to close the stream on error.
  E.g.

      Dropped 10 logs due to writer error

- There are undetected errors transmitting or receiving log entries, such as an
  interface interruption. Clients can calculate the number of logs lost in
  transit using the sequence ID and number of entries in each stream packet.
  E.g.

      Dropped 50 logs due to transmission error

The drop count is combined when possible, and reported only when an entry, that
passes any filters, is going to be sent.

-------------
Log Filtering
-------------
A ``Filter`` anywhere in the path of a ``LogEntry`` proto, for example, in the
``PW_LOG*`` macro implementation, or in an ``RpcLogDrain`` if using RPC logging.
The log filtering service provides read and modify access to the ``Filter``\s
registered in the ``FilterMap``.

How to Use
==========
1. Set up RPC
-------------
Set up RPC for your target device. See :ref:`module-pw_rpc` for details.

2. Create ``Filter``\s
----------------------
Provide each ``Filter`` with its own container for the ``FilterRules`` as big as
the number of rules desired. These rules can be pre-poluated.

3. Create a ``FilterMap`` and ``FilterService``
-----------------------------------------------
Set up the ``FilterMap`` with the filters than can be modified with the
``FilterService``. Register the service with the RPC server.

4. Use RPCs to retrieve and modify filter rules
-----------------------------------------------

Components Overview
===================
Filter::Rule
------------
Contains a set of values that are compared against a log when set. All
conditions must be met for the rule to be met.

- ``action``: drops or keeps the log if the other conditions match.
  The rule is ignored when inactive.

- ``any_flags_set``: the condition is met if this value is 0 or the log has any
  of these flags set.

- ``level_greater_than_or_equal``: the condition is met when the log level is
  greater than or equal to this value.

- ``module_equals``: the condition is met if this byte array is empty, or the
  log module equals the contents of this byte array.

- ``thread_equals``: the condition is met if this byte array is empty or the
  log thread equals the contents of this byte array.

Filter
------
Encapsulates a collection of zero or more ``Filter::Rule``\s and has
an ID used to modify or retrieve its contents.

FilterMap
---------
Provides a convenient way to retrieve register filters by ID.

----------------------------
Logging with filters example
----------------------------
The following code shows a sample setup to defer the log handling to the
``RpcLogDrainThread`` to avoid having the log streaming block at the log
callsite.

main.cc
=======
.. code-block:: cpp

  #include "foo/log.h"
  #include "pw_log/log.h"
  #include "pw_thread/detached_thread.h"
  #include "pw_thread_stl/options.h"

  namespace {

  void RegisterServices() {
    pw::rpc::system_server::Server().RegisterService(foo::log::log_service);
    pw::rpc::system_server::Server().RegisterService(foo::log::filter_service);
  }
  }  // namespace

  int main() {
    PW_LOG_INFO("Deferred logging over RPC example");
    pw::rpc::system_server::Init();
    RegisterServices();
    pw::thread::DetachedThread(pw::thread::stl::Options(), foo::log::log_thread);
    pw::rpc::system_server::Start();
    return 0;
  }

foo/log.cc
==========
Example of a log backend implementation, where logs enter the ``MultiSink`` and
log drains and filters are set up.

.. code-block:: cpp

  #include "foo/log.h"

  #include <array>
  #include <cstdint>

  #include "pw_chrono/system_clock.h"
  #include "pw_log/proto_utils.h"
  #include "pw_log_rpc/log_filter.h"
  #include "pw_log_rpc/log_filter_map.h"
  #include "pw_log_rpc/log_filter_service.h"
  #include "pw_log_rpc/log_service.h"
  #include "pw_log_rpc/rpc_log_drain.h"
  #include "pw_log_rpc/rpc_log_drain_map.h"
  #include "pw_log_rpc/rpc_log_drain_thread.h"
  #include "pw_rpc_system_server/rpc_server.h"
  #include "pw_sync/interrupt_spin_lock.h"
  #include "pw_sync/lock_annotations.h"
  #include "pw_sync/mutex.h"
  #include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"

  namespace foo::log {
  namespace {
  constexpr size_t kLogBufferSize = 5000;
  // Tokenized logs are typically 12-24 bytes.
  constexpr size_t kMaxMessageSize = 32;
  // kMaxLogEntrySize should be less than the MTU of the RPC channel output used
  // by the provided server writer.
  constexpr size_t kMaxLogEntrySize =
      pw::log_rpc::RpcLogDrain::kMinEntrySizeWithoutPayload + kMaxMessageSize;
  std::array<std::byte, kLogBufferSize> multisink_buffer;

  // To save RAM, share the mutex, since drains will be managed sequentially.
  pw::sync::Mutex shared_mutex;
  std::array<std::byte, kMaxEntrySize> client1_buffer
      PW_GUARDED_BY(shared_mutex);
  std::array<std::byte, kMaxEntrySize> client2_buffer
      PW_GUARDED_BY(shared_mutex);
  std::array<pw::log_rpc::RpcLogDrain, 2> drains = {
      pw::log_rpc::RpcLogDrain(
          1,
          client1_buffer,
          shared_mutex,
          RpcLogDrain::LogDrainErrorHandling::kIgnoreWriterErrors),
      pw::log_rpc::RpcLogDrain(
          2,
          client2_buffer,
          shared_mutex,
          RpcLogDrain::LogDrainErrorHandling::kIgnoreWriterErrors),
  };

  pw::sync::InterruptSpinLock log_encode_lock;
  std::array<std::byte, kMaxLogEntrySize> log_encode_buffer
      PW_GUARDED_BY(log_encode_lock);

  std::array<Filter::Rule, 2> logs_to_host_filter_rules;
  std::array<Filter::Rule, 2> logs_to_server_filter_rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = pw::log::FilterRule::Level::INFO_LEVEL,
      },
      {
          .action = Filter::Rule::Action::kDrop,
      },
  }};
  std::array<Filter, 2> filters{
      Filter(pw::as_bytes(pw::span("HOST", 4)), logs_to_host_filter_rules),
      Filter(pw::as_bytes(pw::span("WEB", 3)), logs_to_server_filter_rules),
  };
  pw::log_rpc::FilterMap filter_map(filters);

  extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
      pw_tokenizer_Payload metadata, const uint8_t message[], size_t size_bytes) {
    int64_t timestamp =
        pw::chrono::SystemClock::now().time_since_epoch().count();
    std::lock_guard lock(log_encode_lock);
    pw::Result<pw::ConstByteSpan> encoded_log_result =
      pw::log::EncodeTokenizedLog(
          metadata, message, size_bytes, timestamp, log_encode_buffer);

    if (!encoded_log_result.ok()) {
      GetMultiSink().HandleDropped();
      return;
    }
    GetMultiSink().HandleEntry(encoded_log_result.value());
  }
  }  // namespace

  pw::log_rpc::RpcLogDrainMap drain_map(drains);
  pw::log_rpc::RpcLogDrainThread log_thread(GetMultiSink(), drain_map);
  pw::log_rpc::LogService log_service(drain_map);
  pw::log_rpc::FilterService filter_service(filter_map);

  pw::multisink::MultiSink& GetMultiSink() {
    static pw::multisink::MultiSink multisink(multisink_buffer);
    return multisink;
  }
  }  // namespace foo::log

Logging in other source files
-----------------------------
To defer logging, other source files must simply include ``pw_log/log.h`` and
use the :ref:`module-pw_log` APIs, as long as the source set that includes
``foo/log.cc`` is setup as the log backend.
