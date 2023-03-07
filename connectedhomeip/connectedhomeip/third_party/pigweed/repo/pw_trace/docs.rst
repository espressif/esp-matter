.. _module-pw_trace:

========
pw_trace
========
Pigweed's tracing module provides facilities for applications to trace
information about the execution of their application. The module is split into
two components:

1. The facade (this module) which is only a macro interface layer
2. The backend, provided elsewhere, implements the low level tracing.

------
Status
------
This module is currently in development, and is therefore still undergoing
significant changes.

Future work will add:

1. A Python library to generate trace files which can be viewed. (pwbug/205)
2. Add more examples with sample output. (pwbug/207)
3. Implement a trace backend module. (pwbug/260)

--------
Overview
--------
Traces provide a useful view which shows the flow of events through a system,
and can greatly assist in understanding complex software problems. These traces
can either show what tasks are running at any given time, or use added code
(similar to logging), to help annotate specific interesting flows.

Fundamentally, tracing is similar to logging in that it provides the developer
with a view into what the system was doing at a particular time. The fundamental
difference between logs and tracing is that logs present information as ad-hoc
human-readable strings and are better suited to providing the current state of
the system. Instead, traces capture sequences of events with precise timestamps,
and are therefore useful at understanding the flow of events in the system over
time.

The default backend for pw_trace is pw_trace_null, which disables tracing.

Compatibility
-------------
Most of the facade is compatible with C and C++, the only exception to this is
the Scope and Function tracing macros which are convenience wrappers only
available in C++.

pw_trace:null
-------------
``pw_trace_null`` is a ``pw_trace backend`` that ignores all ``pw_trace``
statements. The backend implements ``pw_trace`` with empty inline functions.
Using empty functions ensure that the arguments are evaluated and their types
are correct. Since the functions are inline in the header, the compiler will
optimize out the function call.

This backend can be used to completely disable ``pw_trace``.

Dependencies
-------------
``pw_preprocessor``

Example
-------

.. code-block:: cpp

  #define PW_TRACE_MODULE_NAME "Input"
  #include "pw_trace/trace.h"

  void SendButton() {
    PW_TRACE_FUNCTION()
    // do something
  }

  void InputLoop() {
    while(1) {
      auto event = WaitNewInputEvent()
      TRACE_SCOPE("Handle Event");  // measure until loop finished
      if (event == kNewButton){
        SendButton();
        PW_TRACE_END("button");  // Trace event was started in ButtonIsr
      } else {
        PW_TRACE_INSTANT("Unknown event");
      }
    }
  }

  void ButtonIsr() {
    PW_TRACE_START("button");
    SendNewInputEvent(kNewButton);
  }


------------
Trace macros
------------

The ``pw_trace`` public API provides three basic trace events:

- ``PW_TRACE_INSTANT`` - This is used to trace an instant event, which has no
  duration.
- ``PW_TRACE_START`` & ``PW_TRACE_END`` - Trace 'start' and 'end' events are
  paired together to measure the duration of an event.

These trace event macros all have the same arguments:

- *label* - Each of these trace events must have a label, which is a string
  describing the event. In addition to the required label, each of these traces
  can optionally provide a group label and trace id.
- *group_label* - The *optional* group label is used if many traces are all
  measuring the same thing and should be grouped together. This information will
  be used when visualizing the trace to ensure the information appears together.
- *trace_id* - The *optional* trace id is similar to the group_id, but instead
  groups events using a runtime value. This can be used if multiple trace flow
  might happen at the same time and need to be grouped together.
  For example, this could be used to trace data packets flowing through the
  system; when a new sample might arrive before the previous packet is finished
  processing. This would result in two start events occurring before the end
  event. By providing a trace id with a different value for each packet, these
  can be separated when decoding.

.. tip::

  All of these arguments must be the same for a *start* and *end* pair.

This results in 9 different trace calls:

.. cpp:function:: PW_TRACE_INSTANT(label)
.. cpp:function:: PW_TRACE_INSTANT(label, group_label)
.. cpp:function:: PW_TRACE_INSTANT(label, group_label, trace_id)
.. cpp:function:: PW_TRACE_START(label)
.. cpp:function:: PW_TRACE_START(label, group_label)
.. cpp:function:: PW_TRACE_START(label, group_label, trace_id)
.. cpp:function:: PW_TRACE_END(label)
.. cpp:function:: PW_TRACE_END(label, group_label)
.. cpp:function:: PW_TRACE_END(label, group_label, trace_id)

Modules
-------
In addition to these arguments, traces can be grouped into modules similar to
logging. This is done by redefining the ``PW_TRACE_MODULE_NAME``.

Flags
-----
Each trace event also has a flags field which can be used to provide additional
compile time trace information. Each trace macro has a matching macro which
allows specifying the flag:

.. cpp:function:: PW_TRACE_INSTANT_FLAG(flag, label)
.. cpp:function:: PW_TRACE_INSTANT_FLAG(flag, label, group_label)
.. cpp:function:: PW_TRACE_INSTANT_FLAG(flag, label, group_label, trace_id)
.. cpp:function:: PW_TRACE_START_FLAG(flag, label)
.. cpp:function:: PW_TRACE_START_FLAG(flag, label, group_label)
.. cpp:function:: PW_TRACE_START_FLAG(flag, label, group_label, trace_id)
.. cpp:function:: PW_TRACE_END_FLAG(flag, label)
.. cpp:function:: PW_TRACE_END_FLAG(flag, label, group_label)
.. cpp:function:: PW_TRACE_END_FLAG(flag, label, group_label, trace_id)

When not specified the flag uses the value of the macro ``PW_TRACE_FLAGS``.

Data
----
Each macro also has a variant which allows appending additional data:

.. cpp:function:: PW_TRACE_INSTANT_DATA(label, data_format_string, data, size)
.. cpp:function:: PW_TRACE_INSTANT_DATA(\
   label, group_label, data_format_string, data, size)
.. cpp:function:: PW_TRACE_INSTANT_DATA(\
   label, group_label, trace_id, data_format_string, data, size)
.. cpp:function:: PW_TRACE_START_DATA(label, data_format_string, data, size)
.. cpp:function:: PW_TRACE_START_DATA(\
   label, group_label, data_format_string, data, size)
.. cpp:function:: PW_TRACE_START_DATA(\
   label, group_label, trace_id, data_format_string, data, size)
.. cpp:function:: PW_TRACE_END_DATA(label, data_format_string, data, size)
.. cpp:function:: PW_TRACE_END_DATA(\
   label, group_label, data_format_string, data, size)
.. cpp:function:: PW_TRACE_END_DATA(\
   label, group_label, trace_id, data_format_string, data, size)

These macros require 3 additional arguments:

- *data_format_string* - A string which is used by the decoder to identify the
  data. This could for example either be printf style tokens, python struct
  packed fmt string or a custom label recognized by the decoder.
- *data* - A pointer to a buffer of arbitrary caller-provided data (void*).
- *size* - The size of the data (size_t).

Currently the included python tool supports a few different options for
*data_format_string*:

- *@pw_arg_label* - Uses the string in the data as the trace event label.
- *@pw_arg_group* - Uses the string in the data as the trace event group.
- *@pw_arg_counter* - Uses the data as a little endian integer value, and
  visualizes it as a counter value in the trace (on a graph).
- *@pw_py_struct_fmt:* - Interprets the string after the ":" as a python struct
  format string, and uses that format string to unpack the data elements. This
  can be used to either provide a single value type, or provide multiple
  different values with a variety of types. Options for format string types can
  be found here: https://docs.python.org/3/library/struct.html#format-characters

.. tip::

  It is ok for only one event of a start/end pair to contain data, as long the
  *label*, *group_label*, and *trace_id*, are all the same.

C++ Only Traces
---------------
Scope API measures durations until the object loses scope. This can for
example, provide a convenient method of tracing functions or loops.

.. cpp:function:: PW_TRACE_SCOPE(label)
.. cpp:function:: PW_TRACE_SCOPE(label, group_label)

Function API measures durations until the function returns. This is the only
macro which does not require a *label*, and instead uses the function name as the
label. It still can optionally be provided a *group_id*.

.. cpp:function:: PW_TRACE_FUNCTION()
.. cpp:function:: PW_TRACE_FUNCTION(group_label)

Compile time enabling/disabling
-------------------------------
Traces in a file can be enabled/disabled at compile time by defining through
the ``PW_TRACE_ENABLE`` macro.  A value of 0 causes traces to be disabled.
A non-zero value will enable traces.  While tracing defaults to enabled,
it is best practice to define ``PW_TRACE_ENABLE`` explicitly in files that
use tracing as the default may change in the future.

A good pattern is to have a module level configuration parameter for enabling
tracing and define ``PW_TRACE_ENABLE`` in terms of that at the top of each
of the module's files:


.. code-block:: cpp

  // Enable tracing based on pw_example module config parameter.
  #define PW_TRACE_ENABLE PW_EXAMPLE_TRACE_ENABLE


Additionally specific trace points (or sets of points) can be enabled/disabled
using the following pattern:

.. code-block:: cpp

  // Assuming tracing is disabled at the top of the file.

  // Enable specific trace.
  #undef PW_TRACE_ENABLE
  #define PW_TRACE_ENABLE 1
  PW_TRACE_INSTANT("important trace");

  // Set traces back to disabled.  PW_TRACE_ENABLE can not be left
  // undefined.
  #undef PW_TRACE_ENABLE
  #define PW_TRACE_ENABLE 0

-----------
Backend API
-----------
Each of the trace event macros get sent to one of two macros which are
implemented by the backend:

.. cpp:function:: PW_TRACE(event_type, flags, label, group_label, trace_id)
.. cpp:function:: PW_TRACE_DATA(event_type, flags, label, group_label, \
   trace_id, data_format_string, data, size)

The ``event_type`` value will be whatever the backend defined for that specific
trace type using the macros defined below.

The backend can optionally not define ``PW_TRACE_DATA`` to have those traces
disabled.

Trace types
-----------
Although there are only 3 basic trace types, each has 3 variants:

- Label only
- Label and group
- Label, group, and trace_id

This combination creates 9 different trace event types:

- *PW_TRACE_TYPE_INSTANT*: Instant trace, with only a label.
- *PW_TRACE_TYPE_DURATION_START*: Start trace, with only a label.
- *PW_TRACE_TYPE_DURATION_END*: End trace, with only a label.
- *PW_TRACE_TYPE_INSTANT_GROUP*: Instant trace, with a label and a group.
- *PW_TRACE_TYPE_DURATION_GROUP_START*: Start trace, with a label and a group.
- *PW_TRACE_TYPE_DURATION_GROUP_END*: End trace, with a label and a group.
- *PW_TRACE_TYPE_ASYNC_INSTANT*: Instant trace, with label, group, and trace_id
- *PW_TRACE_TYPE_ASYNC_START*: Start trace, with label, group, and trace_id.
- *PW_TRACE_TYPE_ASYNC_END*: End trace, with label, group, and trace_id.

The backend must define these macros to have them enabled. If any are left
undefined, any traces of that type are removed.

Defaults
--------
The backend can use these macros to change what the default value is if not
provided.

- *PW_TRACE_FLAGS_DEFAULT*: Default value if no flags are provided.
- *PW_TRACE_TRACE_ID_DEFAULT*: Default value if no trace_id provided.
- *PW_TRACE_GROUP_LABEL_DEFAULT*: Default value if no group_label provided.

----------
Sample App
----------
A sample application is provided in the examples folder. This code attempts to
provide examples of the multiple ways tracing can be used. Furthermore,
trace backends can include the sample app in their own examples to show how to
use other features.

The sample app contains 3 "fake" tasks, which are each in their own
`PW_TRACE_MODULE`.

- *Input*: Simulating a driver, which gets data periodically, and sends to
  *Processing* task.
- *Processing*: Has a work queue, which handles processing the jobs.
- *Kernel*: A simple work loop which demonstrates a possible integration of
  tracing with a RTOS/Kernel.

Jobs are intentionally made to have multiple stages of processing (simulated by
being re-added to the work-queue). This results in multiple jobs being handled
at the same time, the trace_id is used to separate these traces.

----------------------
Python Trace Generator
----------------------
The Python tool is still in early development, but currently it supports
generating a list of json lines from a list of trace events.

To view the trace, these lines can be saved to a file and loaded into
chrome://tracing.

Future work will look to add:

- Config options to customize output.
- A method of providing custom data formatters.
- Perfetto support.
