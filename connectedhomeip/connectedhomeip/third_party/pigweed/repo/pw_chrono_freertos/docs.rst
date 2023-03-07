.. _module-pw_chrono_freertos:

------------------
pw_chrono_freertos
------------------
``pw_chrono_freertos`` is a collection of ``pw_chrono`` backends that are
implemented using FreeRTOS.

.. warning::
  This module is still under construction, the API is not yet stable.

SystemClock backend
-------------------
The FreeRTOS based ``system_clock`` backend implements the
``pw_chrono:system_clock`` facade by using ``xTaskGetTickCountFromISR()`` and
``xTaskGetTickCount()`` based on the current context. An InterruptSpinLock is
used to manage overflows in a thread and interrupt safe manner to produce a
signed 64 bit timestamp.

The ``SystemClock::now()`` must be used more than once per overflow of the
native FreeRTOS ``xTaskGetTickCount*()`` overflow. Note that this duration may
vary if ``portSUPPRESS_TICKS_AND_SLEEP()``, ``vTaskStepTick()``, and/or
``xTaskCatchUpTicks()`` are used.

SystemTimer backend
-------------------
The FreeRTOS based ``system_timer`` backend implements the
``pw_chrono:system_timer`` facade by using FreeRTOS's Software Timers API.
``pw::chrono::SystemTimer`` instances use ``StaticTimer_t`` &
``xTimerCreateStatic``, dynamic memory allocation is never used. This means
that both ``#define configUSE_TIMERS 1`` and
``#define configSUPPORT_STATIC_ALLOCATION 1`` must be set in the FreeRTOS
configuration.

This also means that FreeRTOS's ``Timer Service Daemon Task`` is used and must
be configured appropriately by the user through ``configTIMER_TASK_PRIORITY``,
``configTIMER_QUEUE_LENGTH``, and ``configTIMER_TASK_STACK_DEPTH``.

Build targets
-------------
The GN build for ``pw_chrono_freertos`` has one target: ``system_clock``.
The ``system_clock`` target provides the
``pw_chrono_backend/system_clock_config.h`` and ``pw_chrono_freertos/config.h``
headers and the backend for the ``pw_chrono:system_clock``.
