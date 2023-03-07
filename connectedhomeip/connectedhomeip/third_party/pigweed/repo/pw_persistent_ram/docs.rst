.. _module-pw_persistent_ram:

=================
pw_persistent_ram
=================
The ``pw_persistent_ram`` module contains utilities and containers for using
persistent RAM. By persistent RAM we are referring to memory which is not
initialized across reboots by the hardware nor bootloader(s). This memory may
decay or bit rot between reboots including brownouts, ergo integrity checking is
highly recommended.

.. Note::
  This is something that not all architectures and applications built on them
  support and requires hardware in the loop testing to verify it works as
  intended.

.. Warning::
  Do not treat the current containers provided in this module as stable storage
  primitives. We are still evaluating lighterweight checksums from a code size
  point of view. In other words, future updates to this module may result in a
  loss of persistent data across software updates.

------------------------
Persistent RAM Placement
------------------------
Persistent RAM is typically provided through specially carved out linker script
sections and/or memory ranges which are located in such a way that any
bootloaders and the application boot code do not clobber it.

1. If persistent linker sections are provided, we recommend using our section
   placement macro. For example imagine the persistent section name is called
   `.noinit`, then you could instantiate an object as such:

   .. code-block:: cpp

      #include "pw_persistent_ram/persistent.h"
      #include "pw_preprocessor/compiler.h"

      using pw::persistent_ram::Persistent;

      PW_PLACE_IN_SECTION(".noinit") Persistent<bool> persistent_bool;

2. If persistent memory ranges are provided, we recommend using a struct to wrap
   the different persisted objects. This then could be checked to fit in the
   provided memory range size, for example by asserting against variables
   provided through a linker script.

   .. code-block:: cpp

      #include "pw_assert/check.h"
      #include "pw_persistent_ram/persistent.h"

      // Provided for example through a linker script.
      extern "C" uint8_t __noinit_begin;
      extern "C" uint8_t __noinit_end;

      struct PersistentData {
        Persistent<bool> persistent_bool;
      };
      PersistentData& persistent_data =
        *reinterpret_cast<NoinitData*>(&__noinit_begin);

      void CheckPersistentDataSize() {
        PW_DCHECK_UINT_LE(sizeof(PersistentData),
                          __noinit_end - __noinit_begin,
                          "PersistentData overflowed the noinit memory range");
      }

-----------------------------------
Persistent RAM Lifecycle Management
-----------------------------------
In order for persistent RAM containers to be as useful as possible, any
invalidation of persistent RAM and the containers therein should be executed
before the global static C++ constructors, but after the BSS and data sections
are initialized in RAM.

The preferred way to clear Persistent RAM is to simply zero entire persistent
RAM sections and/or memory regions. Pigweed's persistent containers have picked
integrity checks which work with zeroed memory, meaning they do not hold a value
after zeroing. Alternatively containers can be individually cleared.

The boot sequence itself is tightly coupled to the number of persistent sections
and/or memory regions which exist in the final image, ergo this is something
which Pigweed cannot provide to the user directly. However, we do recommend
following some guidelines:

1. Do not instantiate regular types/objects in persistent RAM, ensure integrity
   checking is always used! This is a major risk with this technique and can
   lead to unexpected memory corruption.
2. Always instantiate persistent containers outside of the objects which depend
   on them and use dependency injection. This permits unit testing and avoids
   placement accidents of persistents and/or their users.
3. Always erase persistent RAM data after software updates unless the
   persistent storage containers are explicitly stored at fixed address and
   with a fixed layout. This prevents use of swapped objects or their members
   where the same integrity checks are used.
4. Consider zeroing persistent RAM to recover from crashes which may be induced
   by persistent RAM usage, for example by checking the reboot/crash reason.
5. Consider zeroing persistent RAM on cold boots to always start from a
   consistent state if persistence is only desired across warm reboots. This can
   create determinism from cold boots when using for example DRAM.
6. Consider an explicit persistent clear request which can be set before a warm
   reboot as a signal to zero all persistent RAM on the next boot to emulate
   persistent memory loss in a threadsafe manner.

---------------------------------
pw::persistent_ram::Persistent<T>
---------------------------------
The Persistent is a simple container for holding its templated value ``T`` with
CRC16 integrity checking. Note that a Persistent will be lost if a write/set
operation is interrupted or otherwise not completed, as it is not double
buffered.

The default constructor does nothing, meaning it will result in either invalid
state initially or a valid persisted value from a previous session.

The destructor does nothing, ergo it is okay if it is not executed during
shutdown.

Example: Storing an integer
---------------------------
A common use case of persistent data is to track boot counts, or effectively
how often the device has rebooted. This can be useful for monitoring how many
times the device rebooted and/or crashed. This can be easily accomplished using
the Persistent container.

.. code-block:: cpp

    #include "pw_persistent_ram/persistent.h"
    #include "pw_preprocessor/compiler.h"

    using pw::persistent_ram::Persistent;

    class BootCount {
     public:
      explicit BootCount(Persistent<uint16_t>& persistent_boot_count)
          : persistent_(persistent_boot_count) {
        if (!persistent_.has_value()) {
          persistent_ = 0;
        } else {
          persistent_ = persistent_.value() + 1;
        }
        boot_count_ = persistent_.value();
      }

      uint16_t GetBootCount() { return boot_count_; }

     private:
      Persistent<uint16_t>& persistent_;
      uint16_t boot_count_;
    };

    PW_PLACE_IN_SECTION(".noinit") Persistent<uint16_t> persistent_boot_count;
    BootCount boot_count(persistent_boot_count);

    int main() {
      const uint16_t boot_count = boot_count.GetBootCount();
      // ... rest of main
    }

Example: Storing larger objects
-------------------------------
Larger objects may be inefficient to copy back and forth due to the need for
a working copy. To work around this, you can get a Mutator handle that provides
direct access to the underlying object. As long as the Mutator is in scope, it
is invalid to access the underlying Persistent, but you'll be able to directly
modify the object in place. Once the Mutator goes out of scope, the Persistent
object's checksum is updated to reflect the changes.

.. code-block:: cpp

    #include "pw_persistent_ram/persistent.h"
    #include "pw_preprocessor/compiler.h"

    using pw::persistent_ram::Persistent;

    contexpr size_t kMaxReasonLength = 256;

    struct LastCrashInfo {
      uint32_t uptime_ms;
      uint32_t boot_id;
      char reason[kMaxReasonLength];
    }

    PW_PLACE_IN_SECTION(".noinit") Persistent<LastBootInfo> persistent_crash_info;

    void HandleCrash(const char* fmt, va_list args) {
      // Once this scope ends, we know the persistent object has been updated
      // to reflect changes.
      {
        auto& mutable_crash_info =
            persistent_crash_info.mutator(GetterAction::kReset);
        vsnprintf(mutable_crash_info->reason,
                  sizeof(mutable_crash_info->reason),
                  fmt,
                  args);
        mutable_crash_info->uptime_ms = system::GetUptimeMs();
        mutable_crash_info->boot_id = system::GetBootId();
      }
      // ...
    }

    int main() {
      if (persistent_crash_info.has_value()) {
        LogLastCrashInfo(persistent_crash_info.value());
        // Clear crash info once it has been dumped.
        persistent_crash_info.Invalidate();
      }

      // ... rest of main
    }

.. _module-pw_persistent_ram-persistent_buffer:

------------------------------------
pw::persistent_ram::PersistentBuffer
------------------------------------
The PersistentBuffer is a persistent storage container for variable-length
serialized data. Rather than allowing direct access to the underlying buffer for
random-access mutations, the PersistentBuffer is mutable through a
PersistentBufferWriter that implements the pw::stream::Writer interface. This
removes the potential for logical errors due to RAII or open()/close() semantics
as both the PersistentBuffer and PersistentBufferWriter can be used validly as
long as their access is serialized.

Example
-------
An example use case is emitting crash handler logs to a buffer for them to be
available after a the device reboots. Once the device reboots, the logs would be
emitted by the logging system. While this isn't always practical for plaintext
logs, tokenized logs are small enough for this to be useful.

.. code-block:: cpp

    #include "pw_persistent_ram/persistent_buffer.h"
    #include "pw_preprocessor/compiler.h"

    using pw::persistent_ram::PersistentBuffer;
    using pw::persistent_ram::PersistentBuffer::PersistentBufferWriter;

    PW_KEEP_IN_SECTION(".noinit") PersistentBuffer<2048> crash_logs;
    void CheckForCrashLogs() {
      if (crash_logs.has_value()) {
        // A function that dumps sequentially serialized logs using pw_log.
        DumpRawLogs(crash_logs.written_data());
        crash_logs.clear();
      }
    }

    void HandleCrash(CrashInfo* crash_info) {
      PersistentBufferWriter crash_log_writer = crash_logs.GetWriter();
      // Sets the pw::stream::Writer that pw_log should dump logs to.
      crash_log_writer.clear();
      SetLogSink(crash_log_writer);
      // Handle crash, calling PW_LOG to log useful info.
    }

    int main() {
      void CheckForCrashLogs();
      // ... rest of main
    }

Size Report
-----------
The following size report showcases the overhead for using Persistent. Note that
this is templating the Persistent only on a ``uint32_t``, ergo the cost without
pw_checksum's CRC16 is the approximate cost per type.

.. include:: persistent_size

Compatibility
-------------
* C++17

Dependencies
------------
* ``pw_checksum``
