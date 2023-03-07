.. _module-pw_chrono_threadx:

-----------------
pw_chrono_threadx
-----------------
``pw_chrono_threadx`` is a collection of ``pw_chrono`` backends that are
implemented using ThreadX.

.. warning::
  This module is under construction, not ready for use, and the documentation
  is incomplete.

SystemClock backend
-------------------
The ThreadX based ``system_clock`` backend implements the
``pw_chrono:system_clock`` facade by using ``tx_time_get()``. An
InterruptSpinLock is used to manage overflows in a thread and interrupt safe
manner to produce a signed 64 bit timestamp.

The ``SystemClock::now()`` must be used more than once per overflow of the
native ThreadX ``tx_time_get()`` overflow. Note that this duration may vary if
``tx_time_set()`` is used.

.. warning::
  Note that this is not compatible with TX_NO_TIMER as this disables
  ``tx_time_get()``.

Build targets
-------------
The GN build for ``pw_chrono_threadx`` has one target: ``system_clock``.
The ``system_clock`` target provides the
``pw_chrono_backend/system_clock_config.h`` and ``pw_chrono_threadx/config.h``
headers and the backend for the ``pw_chrono:system_clock``.
