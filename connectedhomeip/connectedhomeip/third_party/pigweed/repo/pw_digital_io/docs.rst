.. _module-pw_digital_io:

.. cpp:namespace-push:: pw::digital_io

=============
pw_digital_io
=============
.. warning::
   This module is under construction and may not be ready for use.

``pw_digital_io`` provides a set of interfaces for using General Purpose Input
and Output (GPIO) lines for simple Digital I/O. This module can either be used
directly by the application code or wrapped in a device driver for more complex
peripherals.

--------
Overview
--------
The interfaces provide an abstract concept of a **Digital IO line**. The
interfaces abstract away details about the hardware and platform-specific
drivers. A platform-specific backend is responsible for configuring lines and
providing an implementation of the interface that matches the capabilities and
intended usage of the line.

Example API usage:

.. code-block:: cpp

   using namespace pw::digital_io;

   Status UpdateLedFromSwitch(const DigitalIn& switch, DigitalOut& led) {
     PW_TRY_ASSIGN(const DigitalIo::State state, switch.GetState());
     return led.SetState(state);
   }

   Status ListenForButtonPress(DigitalInterrupt& button) {
     PW_TRY(button.SetInterruptHandler(Trigger::kActivatingEdge,
       [](State sampled_state) {
         // Handle the button press.
         // NOTE: this may run in an interrupt context!
       }));
     return button.EnableInterruptHandler();
   }

-------------------------
pw::digital_io Interfaces
-------------------------
There are 3 basic capabilities of a Digital IO line:

* Input - Get the state of the line.
* Output - Set the state of the line.
* Interrupt - Register a handler that is called when a trigger happens.

.. note:: **Capabilities** refer to how the line is intended to be used in a
   particular device given its actual physical wiring, rather than the
   theoretical capabilities of the hardware.

Additionally, all lines can be *enabled* and *disabled*:

* Enable - tell the hardware to apply power to an output line, connect any
  pull-up/down resistors, etc.
* Disable - tell the hardware to stop applying power and return the line to its
  default state. This may save power or allow some other component to drive a
  shared line.

.. note:: The initial state of a line is implementation-defined and may not
   match either the "enabled" or "disabled" state.  Users of the API who need
   to ensure the line is disabled (ex. output is not driving the line) should
   explicitly call ``Disable()``.

Functionality overview
======================
The following table summarizes the interfaces and their required functionality:

.. list-table::
   :header-rows: 1
   :stub-columns: 1

   * -
     - Interrupts Not Required
     - Interrupts Required
   * - Input/Output Not Required
     -
     - :cpp:class:`DigitalInterrupt`
   * - Input Required
     - :cpp:class:`DigitalIn`
     - :cpp:class:`DigitalInInterrupt`
   * - Output Required
     - :cpp:class:`DigitalOut`
     - :cpp:class:`DigitalOutInterrupt`
   * - Input/Output Required
     - :cpp:class:`DigitalInOut`
     - :cpp:class:`DigitalInOutInterrupt`

Synchronization requirements
============================
* An instance of a line has exclusive ownership of that line and may be used
  independently of other line objects without additional synchronization.
* Access to a single line instance must be synchronized at the application
  level. For example, by wrapping the line instance in ``pw::Borrowable``.
* Unless otherwise stated, the line interface must not be used from within an
  interrupt context.

------------
Design Notes
------------
The interfaces are intended to support many but not all use cases, and they do
not cover every possible type of functionality supported by the hardware. There
will be edge cases that require the backend to expose some additional (custom)
interfaces, or require the use of a lower-level API.

Examples of intended use cases:

* Do input and output on lines that have two logical states - active and
  inactive - regardless of the underlying hardware configuration.

  * Example: Read the state of a switch.
  * Example: Control a simple LED with on/off.
  * Example: Activate/deactivate power for a peripheral.
  * Example: Trigger reset of an I2C bus.

* Run code based on an external interrupt.

  * Example: Trigger when a hardware switch is flipped.
  * Example: Trigger when device is connected to external power.
  * Example: Handle data ready signals from peripherals connected to
    I2C/SPI/etc.

* Enable and disable lines as part of a high-level policy:

  * Example: For power management - disable lines to use less power.
  * Example: To support shared lines used for multiple purposes (ex. GPIO or
    I2C).

Examples of use cases we want to allow but don't explicitly support in the API:

* Software-controlled pull up/down resistors, high drive, polarity controls,
  etc.

  * It's up to the backend implementation to expose configuration for these
    settings.
  * Enabling a line should set it into the state that is configured in the
    backend.

* Level-triggered interrupts on RTOS platforms.

  * We explicitly support disabling the interrupt handler while in the context
    of the handler.
  * Otherwise, it's up to the backend to provide any additional level-trigger
    support.

Examples of uses cases we explicitly don't plan to support:

* Using Digital IO to simulate serial interfaces like I2C (bit banging), or any
  use cases requiring exact timing and access to line voltage, clock controls,
  etc.
* Mode selection - controlling hardware multiplexing or logically switching from
  GPIO to I2C mode.

API decisions that have been deferred:

* Supporting operations on multiple lines in parallel - for example to simulate
  a memory register or other parallel interface.
* Helpers to support different patterns for interrupt handlers - running in the
  interrupt context, dispatching to a dedicated thread, using a pw_sync
  primitive, etc.

The following sub-sections discuss specific design decisions in detail.

States vs. voltage levels
=========================
Digital IO line values are represented as **active** and **inactive** states.
These states abstract away the actual electrical level and other physical
properties of the line. This allows applications to interact with Digital IO
lines across targets that may have different physical configurations. It is up
to the backend to provide a consistent definition of state.

Interrupt handling
==================
Interrupt handling is part of this API. The alternative was to have a separate
API for interrupts. We wanted to have a single object that refers to each line
and represents all the functionality that is available on the line.

Interrupt triggers are configured through the ``SetInterruptHandler`` method.
The type of trigger is tightly coupled to what the handler wants to do with that
trigger.

The handler is passed the latest known sampled state of the line. Otherwise
handlers running in an interrupt context cannot query the state of the line.

Class Hierarchy
===============
``pw_digital_io`` contains a 2-level hierarchy of classes.

* ``DigitalIoOptional`` acts as the base class and represents a line that does
  not guarantee any particular functionality is available.

  * This should be rarely used in APIs. Prefer to use one of the derived
    classes.
  * This class is never extended outside this module. Extend one of the derived
    classes.

* Derived classes represent a line with a particular combination of
  functionality.

  * Use a specific class in APIs to represent the requirements.
  * Extend the specific class that has the actual capabilities of the line.

In the future, we may support additional for classes that describe lines with
**optional** functionality. For example, ``DigitalInOptionalInterrupt`` could
describe a line that supports input and optionally supports interrupts.

When using any classes with optional functionality, including
``DigitalIoOptional``, you must check that a functionality is available using
the ``provides_*`` runtime flags. Calling a method that is not supported will
trigger ``PW_CRASH``.

We define the public API through non-virtual methods declared in
``DigitalIoOptional``. These methods delegate to private pure virtual methods.

Type Conversions
================
Conversions are provided between classes with compatible requirements. For
example:

.. code-block:: cpp

   DigitalInInterrupt& in_interrupt_line;
   DigitalIn& in_line = in_interrupt_line;

   DigitalInInterrupt* in_interrupt_line_ptr;
   DigitalIn* in_line_ptr = &in_interrupt_line_ptr->as<DigitalIn>();

Asynchronous APIs
=================
At present, ``pw_digital_io`` is synchronous. All the API calls are expected to
block until the operation is complete. This is desirable for simple GPIO chips
that are controlled through direct register access. However, this may be
undesirable for GPIO extenders controlled through I2C or another shared bus.

The API may be extended in the future to add asynchronous capabilities, or a
separate asynchronous API may be created.

Backend Implemention Notes
==========================
* Derived classes explicitly list the non-virtual methods as public or private
  depending on the supported set of functionality. For example, ``DigitalIn``
  declare ``GetState`` public and ``SetState`` private.
* Derived classes that exclude a particular functionality provide a private,
  final implementation of the unsupported virtual method that crashes if it is
  called. For example, ``DigitalIn`` implements ``DoSetState`` to trigger
  ``PW_CRASH``.
* Backend implementations provide real implementation for the remaining pure
  virtual functions of the class they extend.
* Classes that support optional functionality make the non-virtual optional
  methods public, but they do not provide an implementation for the pure virtual
  functions. These classes are never extended.
* Backend implementations **must** check preconditions for each operations. For
  example, check that the line is actually enabled before trying to get/set the
  state of the line. Otherwise return ``pw::Status::FailedPrecondition()``.
* Backends *may* leave the line in an uninitialized state after construction,
  but implementors are strongly encouraged to initialize the line to a known
  state.

  * If backends initialize the line, it must be initialized to the disabled
    state. i.e. the same state it would be in after calling ``Enable()``
    followed by ``Disable()``.
  * Calling ``Disable()`` on an uninitialized line must put it into the disabled
    state.

------------
Dependencies
------------
* :ref:`module-pw_assert`
* :ref:`module-pw_function`
* :ref:`module-pw_result`
* :ref:`module-pw_status`

.. cpp:namespace-pop::

Zephyr
======
To enable ``pw_digital_io`` for Zephyr add ``CONFIG_PIGWEED_DIGITAL_IO=y`` to
the project's configuration.
