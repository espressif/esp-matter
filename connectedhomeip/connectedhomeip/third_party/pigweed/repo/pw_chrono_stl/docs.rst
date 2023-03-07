.. _module-pw_chrono_stl:

-------------
pw_chrono_stl
-------------
``pw_chrono_stl`` is a collection of ``pw_chrono`` backends that are implemented
using STL's ``std::chrono`` library.

.. warning::
  This module is still under construction, the API is not yet stable.

SystemClock backend
-------------------
The STL based ``pw_chrono_stl:system_clock`` backend target implements the
``pw_chrono:system_clock`` facade by using the ``std::chrono::steady_clock``.
Note that the ``std::chrono::system_clock`` cannot be used as this is not always
a monotonic clock source.

See the documentation for ``pw_chrono`` for further details.

SystemTimer backend
-------------------
The STL based ``pw_chrono_stl:system_timer`` backend target implements the
``pw_chrono:system_timer`` facade by spawning a detached thread for every single
``InvokeAt()`` and ``InvokeAfter()`` call. This thread simply sleeps until the
desired ``expiration_deadline`` and invokes the user's ``ExpiryCallback`` if it
wasn't cancelled.

.. Warning::
  Although fully functional, the current implementation is NOT efficient!

See the documentation for ``pw_chrono`` for further details.

Build targets
-------------
The GN build for ``pw_chrono_stl`` has one target: ``system_clock``.
The ``system_clock`` target provides the
``pw_chrono_backend/system_clock_config.h`` and
``pw_chrono_backend/system_clock_inline.h`` headers and the backend for the
``pw_chrono:system_clock``.
