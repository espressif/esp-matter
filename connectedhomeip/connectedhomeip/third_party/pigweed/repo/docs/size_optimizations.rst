.. _docs-size-optimizations:

==================
Size Optimizations
==================
This page contains recommendations for optimizing the size of embedded software
including its memory and code footprints.

These recommendations are subject to change as the C++ standard and compilers
evolve, and as the authors continue to gain more knowledge and experience in
this area. If you disagree with recommendations, please discuss them with the
Pigweed team, as we're always looking to improve the guide or correct any
inaccuracies.

---------------------------------
Compile Time Constant Expressions
---------------------------------
The use of `constexpr <https://en.cppreference.com/w/cpp/language/constexpr>`_
and soon with C++20
`consteval <https://en.cppreference.com/w/cpp/language/consteval>`_ can enable
you to evaluate the value of a function or variable more at compile-time rather
than only at run-time. This can often not only result in smaller sizes but also
often times more efficient, faster execution.

We highly encourage using this aspect of C++, however there is one caveat: be
careful in marking functions constexpr in APIs which cannot be easily changed
in the future unless you can prove that for all time and all platforms, the
computation can actually be done at compile time. This is because there is no
"mutable" escape hatch for constexpr.

See the :doc:`embedded_cpp_guide` for more detail.

---------
Templates
---------
The compiler implements templates by generating a separate version of the
function for each set of types it is instantiated with. This can increase code
size significantly.

Be careful when instantiating non-trivial template functions with multiple
types.

Consider splitting templated interfaces into multiple layers so that more of the
implementation can be shared between different instantiations. A more advanced
form is to share common logic internally by using default sentinel template
argument value and ergo instantation such as ``pw::Vector``'s
``size_t kMaxSize = vector_impl::kGeneric`` or ``pw::span``'s
``size_t Extent = dynamic_extent``.

-----------------
Virtual Functions
-----------------
Virtual functions provide for runtime polymorphism. Unless runtime polymorphism
is required, virtual functions should be avoided. Virtual functions require a
virtual table and a pointer to it in each instance, which all increases RAM
usage and requires extra instructions at each call site. Virtual functions can
also inhibit compiler optimizations, since the compiler may not be able to tell
which functions will actually be invoked. This can prevent linker garbage
collection, resulting in unused functions being linked into a binary.

When runtime polymorphism is required, virtual functions should be considered.
C alternatives, such as a struct of function pointers, could be used instead,
but these approaches may offer no performance advantage while sacrificing
flexibility and ease of use.

Only use virtual functions when runtime polymorphism is needed. Lastly try to
avoid templated virtual interfaces which can compound the cost by instantiating
many virtual tables.

Devirtualization
================
When you do use virtual functions, try to keep devirtualization in mind. You can
make it easier on the compiler and linker by declaring class definitions as
``final`` to improve the odds. This can help significantly depending on your
toolchain.

If you're interested in more details,
`this is an interesting deep dive <https://quuxplusone.github.io/blog/2021/02/15/devirtualization/>`_.

---------------------------------------------------------
Initialization, Constructors, Finalizers, and Destructors
---------------------------------------------------------
Constructors
============
Where possible consider making your constructors constexpr to reduce their
costs. This also enables global instances to be eligible for ``.data`` or if
all zeros for ``.bss`` section placement.

Static Destructors And Finalizers
=================================
For many embedded projects, cleaning up after the program is not a requirement,
meaning the exit functions including any finalizers registered through
``atexit``, ``at_quick_exit``, and static destructors can all be removed to
reduce the size.

The exact mechanics for disabling static destructors depends on your toolchain.

See the `Ignored Finalizer and Destructor Registration`_ section below for
further details regarding disabling registration of functions to be run at exit
via ``atexit`` and ``at_quick_exit``.

Clang
-----
With modern versions of Clang you can simply use ``-fno-C++-static-destructors``
and you are done.

GCC with newlib-nano
--------------------
With GCC this is more complicated. For example with GCC for ARM Cortex M devices
using ``newlib-nano`` you are forced to tackle the problem in two stages.

First, there are the destructors for the static global objects. These can be
placed in the ``.fini_array`` and ``.fini`` input sections through the use of
the ``-fno-use-cxa-atexit`` GCC flag, assuming ``newlib-nano`` was configured
with ``HAVE_INITFINI_ARAY_SUPPORT``. The two input sections can then be
explicitly discarded in the linker script through the use of the special
``/DISCARD/`` output section:

.. code-block:: text

      /DISCARD/ : {
      /* The finalizers are never invoked when the target shuts down and ergo
       * can be discarded. These include C++ global static destructors and C
       * designated finalizers. */
      *(.fini_array);
      *(.fini);

Second, there are the destructors for the scoped static objects, frequently
referred to as Meyer's Singletons. With the Itanium ABI these use
``__cxa_atexit`` to register destruction on the fly. However, if
``-fno-use-cxa-atexit`` is used with GCC and ``newlib-nano`` these will appear
as ``__tcf_`` prefixed symbols, for example ``__tcf_0``.

There's `an interesting proposal (P1247R0) <http://wg21.link/p1247r0>`_ to
enable ``[[no_destroy]]`` attributes to C++ which would be tempting to use here.
Alas this is not an option yet. As mentioned in the proposal one way to remove
the destructors from these scoped statics is to wrap it in a templated wrapper
which uses placement new.

.. code-block:: cpp

  #include <type_traits>

  template <class T>
  class NoDestroy {
   public:
    template <class... Ts>
    NoDestroy(Ts&&... ts) {
      new (&static_) T(std::forward<Ts>(ts)...);
    }

    T& get() { return reinterpret_cast<T&>(static_); }

   private:
    std::aligned_storage_t<sizeof(T), alignof(T)> static_;
  };

This can then be used as follows to instantiate scoped statics where the
destructor will never be invoked and ergo will not be linked in.

.. code-block:: cpp

  Foo& GetFoo() {
    static NoDestroy<Foo> foo(foo_args);
    return foo.get();
  }

-------
Strings
-------

Tokenization
============
Instead of directly using strings and printf, consider using
:ref:`module-pw_tokenizer` to replace strings and printf-style formatted strings
with binary tokens during compilation. This can reduce the code size, memory
usage, I/O traffic, and even CPU utilization by replacing snprintf calls with
simple tokenization code.

Be careful when using string arguments with tokenization as these still result
in a string in your binary which is appended to your token at run time.

String Formatting
=================
The formatted output family of printf functions in ``<cstdio>`` are quite
expensive from a code size point of view and they often rely on malloc. Instead,
where tokenization cannot be used, consider using :ref:`module-pw_string`'s
utilities.

Removing all printf functions often saves more than 5KiB of code size on ARM
Cortex M devices using ``newlib-nano``.

Logging & Asserting
===================
Using tokenized backends for logging and asserting such as
:ref:`module-pw_log_tokenized` coupled with :ref:`module-pw_assert_log` can
drastically reduce the costs. However, even with this approach there remains a
callsite cost which can add up due to arguments and including metadata.

Try to avoid string arguments and reduce unnecessary extra arguments where
possible. And consider adjusting log levels to compile out debug or even info
logs as code stabilizes and matures.

Future Plans
------------
Going forward Pigweed is evaluating extra configuration options to do things
such as dropping log arguments for certain log levels and modules to give users
finer grained control in trading off diagnostic value and the size cost.

----------------------------------
Threading and Synchronization Cost
----------------------------------

Lighterweight Signaling Primatives
==================================
Consider using ``pw::sync::ThreadNotification`` instead of semaphores as they
can be implemented using more efficient RTOS specific signaling primitives. For
example on FreeRTOS they can be backed by direct task notifications which are
more than 10x smaller than semaphores while also being faster.

Threads and their stack sizes
=============================
Although synchronous APIs are incredibly portable and often easier to reason
about, it is often easy to forget the large stack cost this design paradigm
comes with. We highly recommend watermarking your stacks to reduce wasted
memory.

Our snapshot integration for RTOSes such as :ref:`module-pw_thread_freertos` and
:ref:`module-pw_thread_embos` come with built in support to report stack
watermarks for threads if enabled in the kernel.

In addition, consider using asynchronous design patterns such as Active Objects
which can use :ref:`module-pw_work_queue` or similar asynchronous dispatch work
queues to effectively permit the sharing of stack allocations.

Buffer Sizing
=============
We'd be remiss not to mention the sizing of the various buffers that may exist
in your application. You could consider watermarking them with
:ref:`module-pw_metric`. You may also be able to adjust their servicing interval
and priority, but do not forget to keep the ingress burst sizes and scheduling
jitter into account.

----------------------------
Standard C and C++ libraries
----------------------------
Toolchains are typically distributed with their preferred standard C library and
standard C++ library of choice for the target platform.

Although you do not always have a choice in what standard C library and what
standard C++ library is used or even how it's compiled, we recommend always
keeping an eye out for common sources of bloat.

Assert
======
The standard C library should provides the ``assert`` function or macro which
may be internally used even if your application does not invoke it directly.
Although this can be disabled through ``NDEBUG`` there typically is not a
portable way of replacing the ``assert(condition)`` implementation without
configuring and recompiling your standard C library.

However, you can consider replacing the implementation at link time with a
cheaper implementation. For example ``newlib-nano``, which comes with the
``GNU Arm Embedded Toolchain``, often has an expensive ``__assert_func``
implementation which uses ``fiprintf`` to print to ``stderr`` before invoking
``abort()``. This can be replaced with a simple ``PW_CRASH`` invocation which
can save several kilobytes in case ``fiprintf`` isn't used elsewhere.

One option to remove this bloat is to use ``--wrap`` at link time to replace
these implementations. As an example in GN you could replace it with the
following ``BUILD.gn`` file:

.. code-block:: text

  import("//build_overrides/pigweed.gni")

  import("$dir_pw_build/target_types.gni")

  # Wraps the function called by newlib's implementation of assert from stdlib.h.
  #
  # When using this, we suggest injecting :newlib_assert via pw_build_LINK_DEPS.
  config("wrap_newlib_assert") {
    ldflags = [ "-Wl,--wrap=__assert_func" ]
  }

  # Implements the function called by newlib's implementation of assert from
  # stdlib.h which invokes __assert_func unless NDEBUG is defined.
  pw_source_set("wrapped_newlib_assert") {
    sources = [ "wrapped_newlib_assert.cc" ]
    deps = [
      "$dir_pw_assert:check",
      "$dir_pw_preprocessor",
    ]
  }

And a ``wrapped_newlib_assert.cc`` source file implementing the wrapped assert
function:

.. code-block:: cpp

  #include "pw_assert/check.h"
  #include "pw_preprocessor/compiler.h"

  // This is defined by <cassert>
  extern "C" PW_NO_RETURN void __wrap___assert_func(const char*,
                                                    int,
                                                    const char*,
                                                    const char*) {
    PW_CRASH("libc assert() failure");
  }


Ignored Finalizer and Destructor Registration
=============================================
Even if no cleanup is done during shutdown for your target, shutdown functions
such as ``atexit``, ``at_quick_exit``, and ``__cxa_atexit`` can sometimes not be
linked out. This may be due to vendor code or perhaps using scoped statics, also
known as Meyer's Singletons.

The registration of these destructors and finalizers may include locks, malloc,
and more depending on your standard C library and its configuration.

One option to remove this bloat is to use ``--wrap`` at link time to replace
these implementations with ones which do nothing. As an example in GN you could
replace it with the following ``BUILD.gn`` file:

.. code-block:: text

  import("//build_overrides/pigweed.gni")

  import("$dir_pw_build/target_types.gni")

  config("wrap_atexit") {
    ldflags = [
      "-Wl,--wrap=atexit",
      "-Wl,--wrap=at_quick_exit",
      "-Wl,--wrap=__cxa_atexit",
    ]
  }

  # Implements atexit, at_quick_exit, and __cxa_atexit from stdlib.h with noop
  # versions for targets which do not cleanup during exit and quick_exit.
  #
  # This removes any dependencies which may exist in your existing libc.
  # Although this removes the ability for things such as Meyer's Singletons,
  # i.e. non-global statics, to register destruction function it does not permit
  # them to be garbage collected by the linker.
  pw_source_set("wrapped_noop_atexit") {
    sources = [ "wrapped_noop_atexit.cc" ]
  }

And a ``wrapped_noop_atexit.cc`` source file implementing the noop functions:

.. code-block:: cpp

  // These two are defined by <cstdlib>.
  extern "C" int __wrap_atexit(void (*)(void)) { return 0; }
  extern "C" int __wrap_at_quick_exit(void (*)(void)) { return 0; }

  // This function is part of the Itanium C++ ABI, there is no header which
  // provides this.
  extern "C" int __wrap___cxa_atexit(void (*)(void*), void*, void*) { return 0; }

Unexpected Bloat in Disabled STL Exceptions
===========================================
The GCC
`manual <https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_exceptions.html>`_
recommends using ``-fno-exceptions`` along with ``-fno-unwind-tables`` to
disable exceptions and any associated overhead. This should replace all throw
statements with calls to ``abort()``.

However, what we've noticed with the GCC and ``libstdc++`` is that there is a
risk that the STL will still throw exceptions when the application is compiled
with ``-fno-exceptions`` and there is no way for you to catch them. In theory,
this is not unsafe because the unhandled exception will invoke ``abort()`` via
``std::terminate()``. This can occur because the libraries such as
``libstdc++.a`` may not have been compiled with ``-fno-exceptions`` even though
your application is linked against it.

See
`this <https://blog.mozilla.org/nnethercote/2011/01/18/the-dangers-of-fno-exceptions/>`_
for more information.

Unfortunately there can be significant overhead surrounding these throw call
sites in the ``std::__throw_*`` helper functions. These implementations such as
``std::__throw_out_of_range_fmt(const char*, ...)`` and
their snprintf and ergo malloc dependencies can very quickly add up to many
kilobytes of unnecessary overhead.

One option to remove this bloat while also making sure that the exceptions will
actually result in an effective ``abort()`` is to use ``--wrap`` at link time to
replace these implementations with ones which simply call ``PW_CRASH``.

As an example in GN you could replace it with the following ``BUILD.gn`` file,
note that the mangled names must be used:

.. code-block:: text

  import("//build_overrides/pigweed.gni")

  import("$dir_pw_build/target_types.gni")

  # Wraps the std::__throw_* functions called by GNU ISO C++ Library regardless
  # of whether "-fno-exceptions" is specified.
  #
  # When using this, we suggest injecting :wrapped_libstdc++_functexcept via
  # pw_build_LINK_DEPS.
  config("wrap_libstdc++_functexcept") {
    ldflags = [
      "-Wl,--wrap=_ZSt21__throw_bad_exceptionv",
      "-Wl,--wrap=_ZSt17__throw_bad_allocv",
      "-Wl,--wrap=_ZSt16__throw_bad_castv",
      "-Wl,--wrap=_ZSt18__throw_bad_typeidv",
      "-Wl,--wrap=_ZSt19__throw_logic_errorPKc",
      "-Wl,--wrap=_ZSt20__throw_domain_errorPKc",
      "-Wl,--wrap=_ZSt24__throw_invalid_argumentPKc",
      "-Wl,--wrap=_ZSt20__throw_length_errorPKc",
      "-Wl,--wrap=_ZSt20__throw_out_of_rangePKc",
      "-Wl,--wrap=_ZSt24__throw_out_of_range_fmtPKcz",
      "-Wl,--wrap=_ZSt21__throw_runtime_errorPKc",
      "-Wl,--wrap=_ZSt19__throw_range_errorPKc",
      "-Wl,--wrap=_ZSt22__throw_overflow_errorPKc",
      "-Wl,--wrap=_ZSt23__throw_underflow_errorPKc",
      "-Wl,--wrap=_ZSt19__throw_ios_failurePKc",
      "-Wl,--wrap=_ZSt19__throw_ios_failurePKci",
      "-Wl,--wrap=_ZSt20__throw_system_errori",
      "-Wl,--wrap=_ZSt20__throw_future_errori",
      "-Wl,--wrap=_ZSt25__throw_bad_function_callv",
    ]
  }

  # Implements the std::__throw_* functions called by GNU ISO C++ Library
  # regardless of whether "-fno-exceptions" is specified with PW_CRASH.
  pw_source_set("wrapped_libstdc++_functexcept") {
    sources = [ "wrapped_libstdc++_functexcept.cc" ]
    deps = [
      "$dir_pw_assert:check",
      "$dir_pw_preprocessor",
    ]
  }

And a ``wrapped_libstdc++_functexcept.cc`` source file implementing each
wrapped and mangled ``std::__throw_*`` function:

.. code-block:: cpp

 #include "pw_assert/check.h"
 #include "pw_preprocessor/compiler.h"

 // These are all wrapped implementations of the throw functions provided by
 // libstdc++'s bits/functexcept.h which are not needed when "-fno-exceptions"
 // is used.

 // std::__throw_bad_exception(void)
 extern "C" PW_NO_RETURN void __wrap__ZSt21__throw_bad_exceptionv() {
   PW_CRASH("std::throw_bad_exception");
 }

 // std::__throw_bad_alloc(void)
 extern "C" PW_NO_RETURN void __wrap__ZSt17__throw_bad_allocv() {
   PW_CRASH("std::throw_bad_alloc");
 }

 // std::__throw_bad_cast(void)
 extern "C" PW_NO_RETURN void __wrap__ZSt16__throw_bad_castv() {
   PW_CRASH("std::throw_bad_cast");
 }

 // std::__throw_bad_typeid(void)
 extern "C" PW_NO_RETURN void __wrap__ZSt18__throw_bad_typeidv() {
   PW_CRASH("std::throw_bad_typeid");
 }

 // std::__throw_logic_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt19__throw_logic_errorPKc(const char*) {
   PW_CRASH("std::throw_logic_error");
 }

 // std::__throw_domain_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt20__throw_domain_errorPKc(const char*) {
   PW_CRASH("std::throw_domain_error");
 }

 // std::__throw_invalid_argument(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt24__throw_invalid_argumentPKc(
     const char*) {
   PW_CRASH("std::throw_invalid_argument");
 }

 // std::__throw_length_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt20__throw_length_errorPKc(const char*) {
   PW_CRASH("std::throw_length_error");
 }

 // std::__throw_out_of_range(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt20__throw_out_of_rangePKc(const char*) {
   PW_CRASH("std::throw_out_of_range");
 }

 // std::__throw_out_of_range_fmt(const char*, ...)
 extern "C" PW_NO_RETURN void __wrap__ZSt24__throw_out_of_range_fmtPKcz(
     const char*, ...) {
   PW_CRASH("std::throw_out_of_range");
 }

 // std::__throw_runtime_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt21__throw_runtime_errorPKc(
     const char*) {
   PW_CRASH("std::throw_runtime_error");
 }

 // std::__throw_range_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt19__throw_range_errorPKc(const char*) {
   PW_CRASH("std::throw_range_error");
 }

 // std::__throw_overflow_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt22__throw_overflow_errorPKc(
     const char*) {
   PW_CRASH("std::throw_overflow_error");
 }

 // std::__throw_underflow_error(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt23__throw_underflow_errorPKc(
     const char*) {
   PW_CRASH("std::throw_underflow_error");
 }

 // std::__throw_ios_failure(const char*)
 extern "C" PW_NO_RETURN void __wrap__ZSt19__throw_ios_failurePKc(const char*) {
   PW_CRASH("std::throw_ios_failure");
 }

 // std::__throw_ios_failure(const char*, int)
 extern "C" PW_NO_RETURN void __wrap__ZSt19__throw_ios_failurePKci(const char*,
                                                                   int) {
   PW_CRASH("std::throw_ios_failure");
 }

 // std::__throw_system_error(int)
 extern "C" PW_NO_RETURN void __wrap__ZSt20__throw_system_errori(int) {
   PW_CRASH("std::throw_system_error");
 }

 // std::__throw_future_error(int)
 extern "C" PW_NO_RETURN void __wrap__ZSt20__throw_future_errori(int) {
   PW_CRASH("std::throw_future_error");
 }

 // std::__throw_bad_function_call(void)
 extern "C" PW_NO_RETURN void __wrap__ZSt25__throw_bad_function_callv() {
   PW_CRASH("std::throw_bad_function_call");
 }

---------------------------------
Compiler and Linker Optimizations
---------------------------------

Compiler Optimization Options
=============================
Don't forget to configure your compiler to optimize for size if needed. With
Clang this is ``-Oz`` and with GCC this can be done via ``-Os``. The GN
toolchains provided through :ref:`module-pw_toolchain` which are optimized for
size are suffixed with ``*_size_optimized``.

Garbage collect function and data sections
==========================================
By default the linker will place all functions in an object within the same
linker "section" (e.g. ``.text``). With Clang and GCC you can use
``-ffunction-sections`` and ``-fdata-sections`` to use a unique "section" for
each object (e.g. ``.text.do_foo_function``). This permits you to pass
``--gc-sections`` to the linker to cull any unused sections which were not
referenced.

To see what sections were garbage collected you can pass ``--print-gc-sections``
to the linker so it prints out what was removed.

The GN toolchains provided through :ref:`module-pw_toolchain` are configured to
do this by default.

Function Inlining
=================
Don't forget to expose trivial functions such as member accessors as inline
definitions in the header. The compiler and linker can make the trade-off on
whether the function should be actually inlined or not based on your
optimization settings, however this at least gives it the option. Note that LTO
can inline functions which are not defined in headers.

We stand by the
`Google style guide <https://google.github.io/styleguide/cppguide.html#Inline_Functions>`_
to recommend considering this for simple functions which are 10 lines or less.

Link Time Optimization (LTO)
============================
Consider using LTO to further reduce the size of the binary if needed. This
tends to be very effective at devirtualization.

Unfortunately, the aggressive inlining done by LTO can make it much more
difficult to triage bugs. In addition, it can significantly increase the total
build time.

On GCC and Clang this can be enabled by passing ``-flto`` to both the compiler
and the linker. In addition, on GCC ``-fdevirtualize-at-ltrans`` can be
optionally used to devirtualize more aggressively.

Disabling Scoped Static Initialization Locks
============================================
C++11 requires that scoped static objects are initialized in a thread-safe
manner. This also means that scoped statics, i.e. Meyer's Singletons, be
thread-safe. Unfortunately this rarely is the case on embedded targets. For
example with GCC on an ARM Cortex M device if you test for this you will
discover that instead the program crashes as reentrant initialization is
detected through the use of guard variables.

With GCC and Clang, ``-fno-threadsafe-statics`` can be used to remove the global
lock which often does not work for embedded targets. Note that this leaves the
guard variables in place which ensure that reentrant initialization continues to
crash.

Be careful when using this option in case you are relying on threadsafe
initialization of statics and the global locks were functional for your target.

Triaging Unexpectedly Linked In Functions
=========================================
Lastly as a tip if you cannot figure out why a function is being linked in you
can consider:

* Using ``--wrap`` with the linker to remove the implementation, resulting in a
  link failure which typically calls out which calling function can no longer be
  linked.
* With GCC, you can use ``-fcallgraph-info`` to visualize or otherwise inspect
  the callgraph to figure out who is calling what.
* Sometimes symbolizing the address can resolve what a function is for. For
  example if you are using ``newlib-nano`` along with ``-fno-use-cxa-atexit``,
  scoped static destructors are prefixed ``__tcf_*``. To figure out object these
  destructor functions are associated with, you can use ``llvm-symbolizer`` or
  ``addr2line`` and these will often print out the related object's name.
