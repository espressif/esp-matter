.. _module-pw_function:

-----------
pw_function
-----------
The function module provides a standard, general-purpose API for wrapping
callable objects.

.. note::
  This module is under construction and its API is not complete.

Overview
========

Basic usage
-----------
``pw_function`` defines the ``pw::Function`` class. A ``Function`` is a
move-only callable wrapper constructable from any callable object. Functions
are templated on the signature of the callable they store.

Functions implement the call operator --- invoking the object will forward to
the stored callable.

.. code-block:: c++

  int Add(int a, int b) { return a + b; }

  // Construct a Function object from a function pointer.
  pw::Function<int(int, int)> add_function(Add);

  // Invoke the function object.
  int result = add_function(3, 5);
  EXPECT_EQ(result, 8);

  // Construct a function from a lambda.
  pw::Function<int(int)> negate([](int value) { return -value; });
  EXPECT_EQ(negate(27), -27);

Functions are nullable. Invoking a null function triggers a runtime assert.

.. code-block:: c++

  // A function intialized without a callable is implicitly null.
  pw::Function<void()> null_function;

  // Null functions may also be explicitly created or set.
  pw::Function<void()> explicit_null_function(nullptr);

  pw::Function<void()> function([]() {});  // Valid (non-null) function.
  function = nullptr;  // Set to null, clearing the stored callable.

  // Functions are comparable to nullptr.
  if (function != nullptr) {
    function();
  }

``pw::Function``'s default constructor is ``constexpr``, so default-constructed
functions may be used in classes with ``constexpr`` constructors and in
``constinit`` expressions.

.. code-block:: c++

  class MyClass {
   public:
    // Default construction of a pw::Function is constexpr.
    constexpr MyClass() { ... }

    pw::Function<void(int)> my_function;
  };

  // pw::Function and classes that use it may be constant initialized.
  constinit MyClass instance;

Storage
-------
By default, a ``Function`` stores its callable inline within the object. The
inline storage size defaults to the size of two pointers, but is configurable
through the build system. The size of a ``Function`` object is equivalent to its
inline storage size.

The ``pw::InlineFunction`` alias is similar to ``pw::Function``, but is always
inlined. That is, even if dynamic allocation is enabled for ``pw::Function``  -
``pw::InlineFunction`` will fail to compile if the callable  is larger than the
inline storage size.

Attempting to construct a function from a callable larger than its inline size
is a compile-time error unless dynamic allocation is enabled.

.. admonition:: Inline storage size

  The default inline size of two pointers is sufficient to store most common
  callable objects, including function pointers, simple non-capturing and
  capturing lambdas, and lightweight custom classes.

.. code-block:: c++

  // The lambda is moved into the function's internal storage.
  pw::Function<int(int, int)> subtract([](int a, int b) { return a - b; });

  // Functions can be also be constructed from custom classes that implement
  // operator(). This particular object is large (8 ints of space).
  class MyCallable {
   public:
    int operator()(int value);

   private:
    int data_[8];
  };

  // Compiler error: sizeof(MyCallable) exceeds function's inline storage size.
  pw::Function<int(int)> function((MyCallable()));

.. admonition:: Dynamic allocation

  When ``PW_FUNCTION_ENABLE_DYNAMIC_ALLOCATION`` is enabled, a ``Function``
  will use dynamic allocation to store callables that exceed the inline size.
  When it is enabled but a compile-time check for the inlining is still required
  ``pw::InlineFunction`` can be used.

.. warning::
  If ``PW_FUNCTION_ENABLE_DYNAMIC_ALLOCATION`` is enabled then attempt to cast
  from ``pw::InlineFunction`` to a regular ``pw::Function`` will **ALWAYS**
  allocate memory.

API usage
=========

``pw::Function`` function parameters
------------------------------------
When implementing an API which takes a callback, a ``Function`` can be used in
place of a function pointer or equivalent callable.

.. code-block:: c++

  // Before:
  void DoTheThing(int arg, void (*callback)(int result));

  // After. Note that it is possible to have parameter names within the function
  // signature template for clarity.
  void DoTheThing(int arg, const pw::Function<void(int result)>& callback);

``pw::Function`` is movable, but not copyable, so APIs must accept
``pw::Function`` objects either by const reference (``const
pw::Function<void()>&``) or rvalue reference (``const pw::Function<void()>&&``).
If the ``pw::Function`` simply needs to be called, it should be passed by const
reference. If the ``pw::Function`` needs to be stored, it should be passed as an
rvalue reference and moved into a ``pw::Function`` variable as appropriate.

.. code-block:: c++

  // This function calls a pw::Function but doesn't store it, so it takes a
  // const reference.
  void CallTheCallback(const pw::Function<void(int)>& callback) {
    callback(123);
  }

  // This function move-assigns a pw::Function to another variable, so it takes
  // an rvalue reference.
  void StoreTheCallback(pw::Function<void(int)>&& callback) {
    stored_callback_ = std::move(callback);
  }

.. admonition:: Rules of thumb for passing a ``pw::Function`` to a function

   * **Pass by value**: Never.

     This results in unnecessary ``pw::Function`` instances and move operations.
   * **Pass by const reference** (``const pw::Function&``): When the
     ``pw::Function`` is only invoked.

     When a ``pw::Function`` is called or inspected, but not moved, take a const
     reference to avoid copies and support temporaries.
   * **Pass by rvalue reference** (``pw::Function&&``): When the
     ``pw::Function`` is moved.

     When the function takes ownership of the ``pw::Function`` object, always
     use an rvalue reference (``pw::Function<void()>&&``) instead of a mutable
     lvalue reference (``pw::Function<void()>&``). An rvalue reference forces
     the caller to ``std::move`` when passing a preexisting ``pw::Function``
     variable, which makes the transfer of ownership explicit. It is possible to
     move-assign from an lvalue reference, but this fails to make it obvious to
     the caller that the object is no longer valid.
   * **Pass by non-const reference** (``pw::Function&``): Rarely, when modifying
     a variable.

     Non-const references are only necessary when modifying an existing
     ``pw::Function`` variable. Use an rvalue reference instead if the
     ``pw::Function`` is moved into another variable.

Calling functions that use ``pw::Function``
-------------------------------------------
A ``pw::Function`` can be implicitly constructed from any callback object. When
calling an API that takes a ``pw::Function``, simply pass the callable object.
There is no need to create an intermediate ``pw::Function`` object.

.. code-block:: c++

  // Implicitly creates a pw::Function from a capturing lambda and calls it.
  CallTheCallback([this](int result) { result_ = result; });

  // Implicitly creates a pw::Function from a capturing lambda and stores it.
  StoreTheCallback([this](int result) { result_ = result; });

When working with an existing ``pw::Function`` variable, the variable can be
passed directly to functions that take a const reference. If the function takes
ownership of the ``pw::Function``, move the ``pw::Function`` variable at the
call site.

.. code-block:: c++

  // Accepts the pw::Function by const reference.
  CallTheCallback(my_function_);

  // Takes ownership of the pw::Function.
  void StoreTheCallback(std::move(my_function));

Use ``pw::Callback`` for one-shot functions
-------------------------------------------
``pw::Callback`` is a specialization of ``pw::Function`` that can only be called
once. After a ``pw::Callback`` is called, the target function is destroyed. A
``pw::Callback`` in the "already called" state has the same state as a
``pw::Callback`` that has been assigned to nullptr.

Size reports
============

Function class
--------------
The following size report compares an API using a ``pw::Function`` to a
traditional function pointer.

.. include:: function_size

Callable sizes
--------------
The table below demonstrates typical sizes of various callable types, which can
be used as a reference when sizing external buffers for ``Function`` objects.

.. include:: callable_size

Design
======
``pw::Function`` is an alias of
`fit::function <https://cs.opensource.google/fuchsia/fuchsia/+/main:sdk/lib/fit/include/lib/fit/function.h;drc=f66f54fca0c11a1168d790bcc3d8a5a3d940218d>`_
.


``pw::Callback`` is an alias of
`fit::callback <https://cs.opensource.google/fuchsia/fuchsia/+/main:sdk/lib/fit/include/lib/fit/function.h;drc=f66f54fca0c11a1168d790bcc3d8a5a3d940218d>`_
.

Zephyr
======
To enable ``pw_function` for Zephyr add ``CONFIG_PIGWEED_FUNCTION=y`` to the
project's configuration.
