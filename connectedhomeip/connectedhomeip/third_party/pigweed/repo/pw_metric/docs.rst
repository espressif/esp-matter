.. _module-pw_metric:

=========
pw_metric
=========

.. attention::

  This module is **not yet production ready**; ask us if you are interested in
  using it out or have ideas about how to improve it.

--------
Overview
--------
Pigweed's metric module is a **lightweight manual instrumentation system** for
tracking system health metrics like counts or set values. For example,
``pw_metric`` could help with tracking the number of I2C bus writes, or the
number of times a buffer was filled before it could drain in time, or safely
incrementing counters from ISRs.

Key features of ``pw_metric``:

- **Tokenized names** - Names are tokenized using the ``pw_tokenizer`` enabling
  long metric names that don't bloat your binary.

- **Tree structure** - Metrics can form a tree, enabling grouping of related
  metrics for clearer organization.

- **Per object collection** - Metrics and groups can live on object instances
  and be flexibly combined with metrics from other instances.

- **Global registration** - For legacy code bases or just because it's easier,
  ``pw_metric`` supports automatic aggregation of metrics. This is optional but
  convenient in many cases.

- **Simple design** - There are only two core data structures: ``Metric`` and
  ``Group``, which are both simple to understand and use. The only type of
  metric supported is ``uint32_t`` and ``float``. This module does not support
  complicated aggregations like running average or min/max.

Example: Instrumenting a single object
--------------------------------------
The below example illustrates what instrumenting a class with a metric group
and metrics might look like. In this case, the object's
``MySubsystem::metrics()`` member is not globally registered; the user is on
their own for combining this subsystem's metrics with others.

.. code::

  #include "pw_metric/metric.h"

  class MySubsystem {
   public:
    void DoSomething() {
      attempts_.Increment();
      if (ActionSucceeds()) {
        successes_.Increment();
      }
    }
    Group& metrics() { return metrics_; }

   private:
    PW_METRIC_GROUP(metrics_, "my_subsystem");
    PW_METRIC(metrics_, attempts_, "attempts", 0u);
    PW_METRIC(metrics_, successes_, "successes", 0u);
  };

The metrics subsystem has no canonical output format at this time, but a JSON
dump might look something like this:

.. code:: none

  {
    "my_subsystem" : {
      "successes" : 1000,
      "attempts" : 1200,
    }
  }

In this case, every instance of ``MySubsystem`` will have unique counters.

Example: Instrumenting a legacy codebase
----------------------------------------
A common situation in embedded development is **debugging legacy code** or code
which is hard to change; where it is perhaps impossible to plumb metrics
objects around with dependency injection. The alternative to plumbing metrics
is to register the metrics through a global mechanism. ``pw_metric`` supports
this use case. For example:

**Before instrumenting:**

.. code::

  // This code was passed down from generations of developers before; no one
  // knows what it does or how it works. But it needs to be fixed!
  void OldCodeThatDoesntWorkButWeDontKnowWhy() {
    if (some_variable) {
      DoSomething();
    } else {
      DoSomethingElse();
    }
  }

**After instrumenting:**

.. code::

  #include "pw_metric/global.h"
  #include "pw_metric/metric.h"

  PW_METRIC_GLOBAL(legacy_do_something, "legacy_do_something");
  PW_METRIC_GLOBAL(legacy_do_something_else, "legacy_do_something_else");

  // This code was passed down from generations of developers before; no one
  // knows what it does or how it works. But it needs to be fixed!
  void OldCodeThatDoesntWorkButWeDontKnowWhy() {
    if (some_variable) {
      legacy_do_something.Increment();
      DoSomething();
    } else {
      legacy_do_something_else.Increment();
      DoSomethingElse();
    }
  }

In this case, the developer merely had to add the metrics header, define some
metrics, and then start incrementing them. These metrics will be available
globally through the ``pw::metric::global_metrics`` object defined in
``pw_metric/global.h``.

Why not just use simple counter variables?
------------------------------------------
One might wonder what the point of leveraging a metric library is when it is
trivial to make some global variables and print them out. There are a few
reasons:

- **Metrics offload** - To make it easy to get metrics off-device by sharing
  the infrastructure for offloading.

- **Consistent format** - To get the metrics in a consistent format (e.g.
  protobuf or JSON) for analysis

- **Uncoordinated collection** - To provide a simple and reliable way for
  developers on a team to all collect metrics for their subsystems, without
  having to coordinate to offload. This could extend to code in libraries
  written by other teams.

- **Pre-boot or interrupt visibility** - Some of the most challenging bugs come
  from early system boot when not all system facilities are up (e.g. logging or
  UART). In those cases, metrics provide a low-overhead approach to understand
  what is happening. During early boot, metrics can be incremented, then after
  boot dumping the metrics provides insights into what happened. While basic
  counter variables can work in these contexts too, one still has to deal with
  the offloading problem; which the library handles.

---------------------
Metrics API reference
---------------------

The metrics API consists of just a few components:

- The core data structures ``pw::metric::Metric`` and ``pw::metric::Group``
- The macros for scoped metrics and groups ``PW_METRIC`` and
  ``PW_METRIC_GROUP``
- The macros for globally registered metrics and groups
  ``PW_METRIC_GLOBAL`` and ``PW_METRIC_GROUP_GLOBAL``
- The global groups and metrics list: ``pw::metric::global_groups`` and
  ``pw::metric::global_metrics``.

Metric
------
The ``pw::metric::Metric`` provides:

- A 31-bit tokenized name
- A 1-bit discriminator for int or float
- A 32-bit payload (int or float)
- A 32-bit next pointer (intrusive list)

The metric object is 12 bytes on 32-bit platforms.

.. cpp:class:: pw::metric::Metric

  .. cpp:function:: Increment(uint32_t amount = 0)

    Increment the metric by the given amount. Results in undefined behaviour if
    the metric is not of type int.

  .. cpp:function:: Set(uint32_t value)

    Set the metric to the given value. Results in undefined behaviour if the
    metric is not of type int.

  .. cpp:function:: Set(float value)

    Set the metric to the given value. Results in undefined behaviour if the
    metric is not of type float.

Group
-----
The ``pw::metric::Group`` object is simply:

- A name for the group
- A list of children groups
- A list of leaf metrics groups
- A 32-bit next pointer (intrusive list)

The group object is 16 bytes on 32-bit platforms.

.. cpp:class:: pw::metric::Group

  .. cpp:function:: Dump(int indent_level = 0)

    Recursively dump a metrics group to ``pw_log``. Produces output like:

    .. code:: none

      "$6doqFw==": {
        "$05OCZw==": {
          "$VpPfzg==": 1,
          "$LGPMBQ==": 1.000000,
          "$+iJvUg==": 5,
        }
        "$9hPNxw==": 65,
        "$oK7HmA==": 13,
        "$FCM4qQ==": 0,
      }

    Note the metric names are tokenized with base64. Decoding requires using
    the Pigweed detokenizer. With a detokenizing-enabled logger, you could get
    something like:

    .. code:: none

      "i2c_1": {
        "gyro": {
          "num_sampleses": 1,
          "init_time_us": 1.000000,
          "initialized": 5,
        }
        "bus_errors": 65,
        "transactions": 13,
        "bytes_sent": 0,
      }

Macros
------
The **macros are the primary mechanism for creating metrics**, and should be
used instead of directly constructing metrics or groups. The macros handle
tokenizing the metric and group names.

.. cpp:function:: PW_METRIC(identifier, name, value)
.. cpp:function:: PW_METRIC(group, identifier, name, value)
.. cpp:function:: PW_METRIC_STATIC(identifier, name, value)
.. cpp:function:: PW_METRIC_STATIC(group, identifier, name, value)

  Declare a metric, optionally adding it to a group.

  - **identifier** - An identifier name for the created variable or member.
    For example: ``i2c_transactions`` might be used as a local or global
    metric; inside a class, could be named according to members
    (``i2c_transactions_`` for Google's C++ style).
  - **name** - The string name for the metric. This will be tokenized. There
    are no restrictions on the contents of the name; however, consider
    restricting these to be valid C++ identifiers to ease integration with
    other systems.
  - **value** - The initial value for the metric. Must be either a floating
    point value (e.g. ``3.2f``) or unsigned int (e.g. ``21u``).
  - **group** - A ``pw::metric::Group`` instance. If provided, the metric is
    added to the given group.

  The macro declares a variable or member named "name" with type
  ``pw::metric::Metric``, and works in three contexts: global, local, and
  member.

  If the `_STATIC` variant is used, the macro declares a variable with static
  storage. These can be used in function scopes, but not in classes.

  1. At global scope:

    .. code::

      PW_METRIC(foo, "foo", 15.5f);

      void MyFunc() {
        foo.Increment();
      }

  2. At local function or member function scope:

    .. code::

      void MyFunc() {
        PW_METRIC(foo, "foo", 15.5f);
        foo.Increment();
        // foo goes out of scope here; be careful!
      }

  3. At member level inside a class or struct:

    .. code::

      struct MyStructy {
        void DoSomething() {
          somethings.Increment();
        }
        // Every instance of MyStructy will have a separate somethings counter.
        PW_METRIC(somethings, "somethings", 0u);
      }

  You can also put a metric into a group with the macro. Metrics can belong to
  strictly one group, otherwise an assertion will fail. Example:

  .. code::

    PW_METRIC_GROUP(my_group, "my_group");
    PW_METRIC(my_group, foo, "foo", 0.2f);
    PW_METRIC(my_group, bar, "bar", 44000u);
    PW_METRIC(my_group, zap, "zap", 3.14f);

  .. tip::

    If you want a globally registered metric, see ``pw_metric/global.h``; in
    that contexts, metrics are globally registered without the need to
    centrally register in a single place.

.. cpp:function:: PW_METRIC_GROUP(identifier, name)
.. cpp:function:: PW_METRIC_GROUP(parent_group, identifier, name)
.. cpp:function:: PW_METRIC_GROUP_STATIC(identifier, name)
.. cpp:function:: PW_METRIC_GROUP_STATIC(parent_group, identifier, name)

  Declares a ``pw::metric::Group`` with name name; the name is tokenized.
  Works similar to ``PW_METRIC`` and can be used in the same contexts (global,
  local, and member). Optionally, the group can be added to a parent group.

  If the `_STATIC` variant is used, the macro declares a variable with static
  storage. These can be used in function scopes, but not in classes.

  Example:

  .. code::

    PW_METRIC_GROUP(my_group, "my_group");
    PW_METRIC(my_group, foo, "foo", 0.2f);
    PW_METRIC(my_group, bar, "bar", 44000u);
    PW_METRIC(my_group, zap, "zap", 3.14f);

.. cpp:function:: PW_METRIC_GLOBAL(identifier, name, value)

  Declare a ``pw::metric::Metric`` with name name, and register it in the
  global metrics list ``pw::metric::global_metrics``.

  Example:

  .. code::

    #include "pw_metric/metric.h"
    #include "pw_metric/global.h"

    // No need to coordinate collection of foo and bar; they're autoregistered.
    PW_METRIC_GLOBAL(foo, "foo", 0.2f);
    PW_METRIC_GLOBAL(bar, "bar", 44000u);

  Note that metrics defined with ``PW_METRIC_GLOBAL`` should never be added to
  groups defined with ``PW_METRIC_GROUP_GLOBAL``. Each metric can only belong
  to one group, and metrics defined with ``PW_METRIC_GLOBAL`` are
  pre-registered with the global metrics list.

  .. attention::

    Do not create ``PW_METRIC_GLOBAL`` instances anywhere other than global
    scope. Putting these on an instance (member context) would lead to dangling
    pointers and misery. Metrics are never deleted or unregistered!

.. cpp:function:: PW_METRIC_GROUP_GLOBAL(identifier, name, value)

  Declare a ``pw::metric::Group`` with name name, and register it in the
  global metric groups list ``pw::metric::global_groups``.

  Note that metrics created with ``PW_METRIC_GLOBAL`` should never be added to
  groups! Instead, just create a freestanding metric and register it into the
  global group (like in the example below).

  Example:

  .. code::

    #include "pw_metric/metric.h"
    #include "pw_metric/global.h"

    // No need to coordinate collection of this group; it's globally registered.
    PW_METRIC_GROUP_GLOBAL(leagcy_system, "legacy_system");
    PW_METRIC(leagcy_system, foo, "foo",0.2f);
    PW_METRIC(leagcy_system, bar, "bar",44000u);

  .. attention::

    Do not create ``PW_METRIC_GROUP_GLOBAL`` instances anywhere other than
    global scope. Putting these on an instance (member context) would lead to
    dangling pointers and misery. Metrics are never deleted or unregistered!

----------------------
Usage & Best Practices
----------------------
This library makes several tradeoffs to enable low memory use per-metric, and
one of those tradeoffs results in requiring care in constructing the metric
trees.

Use the Init() pattern for static objects with metrics
------------------------------------------------------
A common pattern in embedded systems is to allocate many objects globally, and
reduce reliance on dynamic allocation (or eschew malloc entirely). This leads
to a pattern where rich/large objects are statically constructed at global
scope, then interacted with via tasks or threads. For example, consider a
hypothetical global ``Uart`` object:

.. code::

  class Uart {
   public:
    Uart(span<std::byte> rx_buffer, span<std::byte> tx_buffer)
      : rx_buffer_(rx_buffer), tx_buffer_(tx_buffer) {}

    // Send/receive here...

   private:
    pw::span<std::byte> rx_buffer;
    pw::span<std::byte> tx_buffer;
  };

  std::array<std::byte, 512> uart_rx_buffer;
  std::array<std::byte, 512> uart_tx_buffer;
  Uart uart1(uart_rx_buffer, uart_tx_buffer);

Through the course of building a product, the team may want to add metrics to
the UART to for example gain insight into which operations are triggering lots
of data transfer. When adding metrics to the above imaginary UART object, one
might consider the following approach:

.. code::

  class Uart {
   public:
    Uart(span<std::byte> rx_buffer,
         span<std::byte> tx_buffer,
         Group& parent_metrics)
      : rx_buffer_(rx_buffer),
        tx_buffer_(tx_buffer) {
        // PROBLEM! parent_metrics may not be constructed if it's a reference
        // to a static global.
        parent_metrics.Add(tx_bytes_);
        parent_metrics.Add(rx_bytes_);
     }

    // Send/receive here which increment tx/rx_bytes.

   private:
    pw::span<std::byte> rx_buffer;
    pw::span<std::byte> tx_buffer;

    PW_METRIC(tx_bytes_, "tx_bytes", 0);
    PW_METRIC(rx_bytes_, "rx_bytes", 0);
  };

  PW_METRIC_GROUP(global_metrics, "/");
  PW_METRIC_GROUP(global_metrics, uart1_metrics, "uart1");

  std::array<std::byte, 512> uart_rx_buffer;
  std::array<std::byte, 512> uart_tx_buffer;
  Uart uart1(uart_rx_buffer,
             uart_tx_buffer,
             uart1_metrics);

However, this **is incorrect**, since the ``parent_metrics`` (pointing to
``uart1_metrics`` in this case) may not be constructed at the point of
``uart1`` getting constructed. Thankfully in the case of ``pw_metric`` this
will result in an assertion failure (or it will work correctly if the
constructors are called in a favorable order), so the problem will not go
unnoticed.  Instead, consider using the ``Init()`` pattern for static objects,
where references to dependencies may only be stored during construction, but no
methods on the dependencies are called.

Instead, the ``Init()`` approach separates global object construction into two
phases: The constructor where references are stored, and a ``Init()`` function
which is called after all static constructors have run. This approach works
correctly, even when the objects are allocated globally:

.. code::

  class Uart {
   public:
    // Note that metrics is not passed in here at all.
    Uart(span<std::byte> rx_buffer,
         span<std::byte> tx_buffer)
      : rx_buffer_(rx_buffer),
        tx_buffer_(tx_buffer) {}

     // Precondition: parent_metrics is already constructed.
     void Init(Group& parent_metrics) {
        parent_metrics.Add(tx_bytes_);
        parent_metrics.Add(rx_bytes_);
     }

    // Send/receive here which increment tx/rx_bytes.

   private:
    pw::span<std::byte> rx_buffer;
    pw::span<std::byte> tx_buffer;

    PW_METRIC(tx_bytes_, "tx_bytes", 0);
    PW_METRIC(rx_bytes_, "rx_bytes", 0);
  };

  PW_METRIC_GROUP(root_metrics, "/");
  PW_METRIC_GROUP(root_metrics, uart1_metrics, "uart1");

  std::array<std::byte, 512> uart_rx_buffer;
  std::array<std::byte, 512> uart_tx_buffer;
  Uart uart1(uart_rx_buffer,
             uart_tx_buffer);

  void main() {
    // uart1_metrics is guaranteed to be initialized by this point, so it is
    safe to pass it to Init().
    uart1.Init(uart1_metrics);
  }

.. attention::

  Be extra careful about **static global metric registration**. Consider using
  the ``Init()`` pattern.

Metric member order matters in objects
--------------------------------------
The order of declaring in-class groups and metrics matters if the metrics are
within a group declared inside the class. For example, the following class will
work fine:

.. code::

  #include "pw_metric/metric.h"

  class PowerSubsystem {
   public:
     Group& metrics() { return metrics_; }
     const Group& metrics() const { return metrics_; }

   private:
    PW_METRIC_GROUP(metrics_, "power");  // Note metrics_ declared first.
    PW_METRIC(metrics_, foo, "foo", 0.2f);
    PW_METRIC(metrics_, bar, "bar", 44000u);
  };

but the following one will not since the group is constructed after the metrics
(and will result in a compile error):

.. code::

  #include "pw_metric/metric.h"

  class PowerSubsystem {
   public:
     Group& metrics() { return metrics_; }
     const Group& metrics() const { return metrics_; }

   private:
    PW_METRIC(metrics_, foo, "foo", 0.2f);
    PW_METRIC(metrics_, bar, "bar", 44000u);
    PW_METRIC_GROUP(metrics_, "power");  // Error: metrics_ must be first.
  };

.. attention::

  Put **groups before metrics** when declaring metrics members inside classes.

Thread safety
-------------
``pw_metric`` has **no built-in synchronization for manipulating the tree**
structure. Users are expected to either rely on shared global mutex when
constructing the metric tree, or do the metric construction in a single thread
(e.g. a boot/init thread). The same applies for destruction, though we do not
advise destructing metrics or groups.

Individual metrics have atomic ``Increment()``, ``Set()``, and the value
accessors ``as_float()`` and ``as_int()`` which don't require separate
synchronization, and can be used from ISRs.

.. attention::

  **You must synchronize access to metrics**. ``pw_metrics`` does not
  internally synchronize access during construction. Metric Set/Increment are
  safe.

Lifecycle
---------
Metric objects are not designed to be destructed, and are expected to live for
the lifetime of the program or application. If you need dynamic
creation/destruction of metrics, ``pw_metric`` does not attempt to cover that
use case. Instead, ``pw_metric`` covers the case of products with two execution
phases:

1. A boot phase where the metric tree is created.
2. A run phase where metrics are collected. The tree structure is fixed.

Technically, it is possible to destruct metrics provided care is taken to
remove the given metric (or group) from the list it's contained in. However,
there are no helper functions for this, so be careful.

Below is an example that **is incorrect**. Don't do what follows!

.. code::

  #include "pw_metric/metric.h"

  void main() {
    PW_METRIC_GROUP(root, "/");
    {
      // BAD! The metrics have a different lifetime than the group.
      PW_METRIC(root, temperature, "temperature_f", 72.3f);
      PW_METRIC(root, humidity, "humidity_relative_percent", 33.2f);
    }
    // OOPS! root now has a linked list that points to the destructed
    // "humidity" object.
  }

.. attention::

  **Don't destruct metrics**. Metrics are designed to be registered /
  structured upfront, then manipulated during a device's active phase. They do
  not support destruction.

-----------------
Exporting metrics
-----------------
Collecting metrics on a device is not useful without a mechanism to export
those metrics for analysis and debugging. ``pw_metric`` offers optional RPC
service libraries (``:metric_service_nanopb`` based on nanopb, and
``:metric_service_pwpb`` based on pw_protobuf) that enable exporting a
user-supplied set of on-device metrics via RPC. This facility is intended to
function from the early stages of device bringup through production in the
field.

The metrics are fetched by calling the ``MetricService.Get`` RPC method, which
streams all registered metrics to the caller in batches (server streaming RPC).
Batching the returned metrics avoids requiring a large buffer or large RPC MTU.

The returned metric objects have flattened paths to the root. For example, the
returned metrics (post detokenization and jsonified) might look something like:

.. code:: none

  {
    "/i2c1/failed_txns": 17,
    "/i2c1/total_txns": 2013,
    "/i2c1/gyro/resets": 24,
    "/i2c1/gyro/hangs": 1,
    "/spi1/thermocouple/reads": 242,
    "/spi1/thermocouple/temp_celsius": 34.52,
  }

Note that there is no nesting of the groups; the nesting is implied from the
path.

RPC service setup
-----------------
To expose a ``MetricService`` in your application, do the following:

1. Define metrics around the system, and put them in a group or list of
   metrics. Easy choices include for example the ``global_groups`` and
   ``global_metrics`` variables; or creat your own.
2. Create an instance of ``pw::metric::MetricService``.
3. Register the service with your RPC server.

For example:

.. code::

   #include "pw_rpc/server.h"
   #include "pw_metric/metric.h"
   #include "pw_metric/global.h"
   #include "pw_metric/metric_service_nanopb.h"

   // Note: You must customize the RPC server setup; see pw_rpc.
   Channel channels[] = {
    Channel::Create<1>(&uart_output),
   };
   Server server(channels);

   // Metric service instance, pointing to the global metric objects.
   // This could also point to custom per-product or application objects.
   pw::metric::MetricService metric_service(
       pw::metric::global_metrics,
       pw::metric::global_groups);

   void RegisterServices() {
     server.RegisterService(metric_service);
     // Register other services here.
   }

   void main() {
     // ... system initialization ...

     RegisterServices();

     // ... start your applcation ...
   }

.. attention::

  Take care when exporting metrics. Ensure **appropriate access control** is in
  place. In some cases it may make sense to entirely disable metrics export for
  production builds. Although reading metrics via RPC won't influence the
  device, in some cases the metrics could expose sensitive information if
  product owners are not careful.

.. attention::

  **MetricService::Get is a synchronous RPC method**

  Calls to is ``MetricService::Get`` are blocking and will send all metrics
  immediately, even though it is a server-streaming RPC. This will work fine if
  the device doesn't have too many metrics, or doesn't have concurrent RPCs
  like logging, but could be a problem in some cases.

  We plan to offer an async version where the application is responsible for
  pumping the metrics into the streaming response. This gives flow control to
  the application.

-----------
Size report
-----------
The below size report shows the cost in code and memory for a few examples of
metrics. This does not include the RPC service.

.. include:: metric_size_report

.. attention::

  At time of writing, **the above sizes show an unexpectedly large flash
  impact**. We are investigating why GCC is inserting large global static
  constructors per group, when all the logic should be reused across objects.

-------------
Metric Parser
-------------
The metric_parser Python Module requests the system metrics via RPC, then parses the
response while detokenizing the group and metrics names, and returns the metrics
in a dictionary organized by group and value.

----------------
Design tradeoffs
----------------
There are many possible approaches to metrics collection and aggregation. We've
chosen some points on the tradeoff curve:

- **Atomic-sized metrics** - Using simple metric objects with just uint32/float
  enables atomic operations. While it might be nice to support larger types, it
  is more useful to have safe metrics increment from interrupt subroutines.

- **No aggregate metrics (yet)** - Aggregate metrics (e.g. average, max, min,
  histograms) are not supported, and must be built on top of the simple base
  metrics. By taking this route, we can considerably simplify the core metrics
  system and have aggregation logic in separate modules. Those modules can then
  feed into the metrics system - for example by creating multiple metrics for a
  single underlying metric. For example: "foo", "foo_max", "foo_min" and so on.

  The other problem with automatic aggregation is that what period the
  aggregation happens over is often important, and it can be hard to design
  this cleanly into the API. Instead, this responsibility is pushed to the user
  who must take more care.

  Note that we will add helpers for aggregated metrics.

- **No virtual metrics** - An alternate approach to the concrete Metric class
  in the current module is to have a virtual interface for metrics, and then
  allow those metrics to have their own storage. This is attractive but can
  lead to many vtables and excess memory use in simple one-metric use cases.

- **Linked list registration** - Using linked lists for registration is a
  tradeoff, accepting some memory overhead in exchange for flexibility. Other
  alternatives include a global table of metrics, which has the disadvantage of
  requiring centralizing the metrics -- an impossibility for middleware like
  Pigweed.

- **Synchronization** - The only synchronization guarantee provided by
  pw_metric is that increment and set are atomic. Other than that, users are on
  their own to synchonize metric collection and updating.

- **No fast metric lookup** - The current design does not make it fast to
  lookup a metric at runtime; instead, one must run a linear search of the tree
  to find the matching metric. In most non-dynamic use cases, this is fine in
  practice, and saves having a more involved hash table. Metric updates will be
  through direct member or variable accesses.

- **Relying on C++ static initialization** - In short, the convenience
  outweighs the cost and risk. Without static initializers, it would be
  impossible to automatically collect the metrics without post-processing the
  C++ code to find the metrics; a huge and debatably worthwhile approach. We
  have carefully analyzed the static initializer behaviour of Pigweed's
  IntrusiveList and are confident it is correct.

- **Both local & global support** - Potentially just one approach (the local or
  global one) could be offered, making the module less complex. However, we
  feel the additional complexity is worthwhile since there are legimitate use
  cases for both e.g. ``PW_METRIC`` and ``PW_METRIC_GLOBAL``. We'd prefer to
  have a well-tested upstream solution for these use cases rather than have
  customers re-implement one of these.

----------------
Roadmap & Status
----------------
- **String metric names** - ``pw_metric`` stores metric names as tokens. On one
  hand, this is great for production where having a compact binary is often a
  requirement to fit the application in the given part. However, in early
  development before flash is a constraint, string names are more convenient to
  work with since there is no need for host-side detokenization. We plan to add
  optional support for using supporting strings.

- **Aggregate metrics** - We plan to add support for aggregate metrics on top
  of the simple metric mechanism, either as another module or as additional
  functionality inside this one. Likely examples include min/max,

- **Selectively enable or disable metrics** - Currently the metrics are always
  enabled once included. In practice this is not ideal since many times only a
  few metrics are wanted in production, but having to strip all the metrics
  code is error prone. Instead, we will add support for controlling what
  metrics are enabled or disabled at compile time. This may rely on of C++20's
  support for zero-sized members to fully remove the cost.

- **Async RPC** - The current RPC service exports the metrics by streaming
  them to the client in batches. However, the current solution streams all the
  metrics to completion; this may block the RPC thread. In the future we will
  have an async solution where the user is in control of flow priority.

- **Timer integration** - We would like to add a stopwatch type mechanism to
  time multiple in-flight events.

- **C support** - In practice it's often useful or necessary to instrument
  C-only code. While it will be impossible to support the global registration
  system that the C++ version supports, we will figure out a solution to make
  instrumenting C code relatively smooth.

- **Global counter** - We may add a global metric counter to help detect cases
  where post-initialization metrics manipulations are done.

- **Proto structure** - It may be possible to directly map metrics to a custom
  proto structure, where instead of a name or token field, a tag field is
  provided. This could result in elegant export to an easily machine parsable
  and compact representation on the host. We may investigate this in the
  future.

- **Safer data structures** - At a cost of 4B per metric and 4B per group, it
  may be possible to make metric structure instantiation safe even in static
  constructors, and also make it safe to remove metrics dynamically. We will
  consider whether this tradeoff is the right one, since a 4B cost per metric
  is substantial on projects with many metrics.
