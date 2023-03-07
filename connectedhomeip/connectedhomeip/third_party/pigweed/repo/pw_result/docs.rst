.. _module-pw_result:

=========
pw_result
=========
``pw::Result<T>`` is a class template for use in returning either a
``pw::Status`` error or an object of type ``T``.

.. inclusive-language: disable

``pw::Result<T>``'s implementation is closely based on Abseil's `StatusOr<T>
class <https://github.com/abseil/abseil-cpp/blob/master/absl/status/statusor.h>`_.
There are a few differences:

.. inclusive-language: enable

* ``pw::Result<T>`` uses ``pw::Status``, which is much less sophisticated than
  ``absl::Status``.
* ``pw::Result<T>``'s functions are ``constexpr`` and ``pw::Result<T>`` may be
  used in ``constexpr`` statements if ``T`` is trivially destructible.

-----
Usage
-----
Usage of ``pw::Result<T>`` is identical to Abseil's ``absl::StatusOr<T>``.
See Abseil's `documentation
<https://abseil.io/docs/cpp/guides/status#returning-a-status-or-a-value>`_ and
`usage tips <https://abseil.io/tips/181>`_ for guidance.

``pw::Result<T>`` is returned from a function that may return ``pw::OkStatus()``
and a value or an error status and no value. If ``ok()`` is true, the
``pw::Result<T>`` contains a valid ``T``. Otherwise, it does not contain a ``T``
and attempting to access the value is an error.

``pw::Result<T>`` can be used to directly access the contained type:

.. code-block:: cpp

  #include "pw_result/result.h"

  if (pw::Result<Foo> foo = TryCreateFoo(); foo.ok()) {
    foo->DoBar();
  }

``pw::Result`` is compatible with ``PW_TRY`` and ``PW_TRY_ASSIGN``, for example:

.. code-block:: cpp

  #include "pw_status/try.h"
  #include "pw_result/result.h"

  pw::Result<int> GetAnswer();  // Example function.

  pw::Status UseAnswer() {
    const pw::Result<int> answer = GetAnswer();
    if (!answer.ok()) {
      return answer.status();
    }
    if (answer.value() == 42) {
      WhatWasTheUltimateQuestion();
    }
    return pw::OkStatus();
  }

  pw::Status UseAnswerWithTry() {
    const pw::Result<int> answer = GetAnswer();
    PW_TRY(answer.status());
    if (answer.value() == 42) {
      WhatWasTheUltimateQuestion();
    }
    return pw::OkStatus();
  }

  pw::Status UseAnswerWithTryAssign() {
    PW_TRY_ASSIGN(const int answer, GetAnswer());
    if (answer == 42) {
      WhatWasTheUltimateQuestion();
    }
    return pw::OkStatus();
  }

.. warning::

  Be careful not to use larger types by value as this can quickly consume
  unnecessary stack.

.. warning::

  This module is experimental. Its impact on code size and stack usage has not
  yet been profiled. Use at your own risk.

Monadic Operations
==================
``pw::Result<T>`` also supports monadic operations, similar to the additions
made to ``std::optional<T>`` in C++23. These operations allow functions to be
applied to a ``pw::Result<T>`` that would perform additional computation.

These operations do not incur any additional FLASH or RAM cost compared to a
traditional if/else ladder, as can be seen in the `Size report`_.

.. code-block:: cpp

  // Without monads
  pw::Result<Image> GetCuteCat(const Image& img) {
     pw::Result<Image> cropped = CropToCat(img);
     if (!cropped.ok()) {
       return cropped.status();
     }
     pw::Result<Image> with_tie = AddBowTie(*cropped);
     if (!with_tie.ok()) {
       return with_tie.status();
     }
     pw::Result<Image> with_sparkles = MakeEyesSparkle(*with_tie);
     if (!with_sparkles.ok()) {
       return with_parkes.status();
     }
     return AddRainbow(MakeSmaller(*with_sparkles));
  }

  // With monads
  pw::Result<Image> GetCuteCat(const Image& img) {
    return CropToCat(img)
           .and_then(AddBoeTie)
           .and_then(MakeEyesSparkle)
           .transform(MakeSmaller)
           .transform(AddRainbow);
  }

``pw::Result<T>::and_then``
---------------------------
The ``pw::Result<T>::and_then`` member function will return the result of the
invocation of the provided function on the contained value if it exists.
Otherwise, returns the contained status in a ``pw::Result<U>``, which is the
return type of provided function.

.. code-block:: cpp

  // Expositional prototype of and_then:
  template <typename T>
  class Result {
    template <typename U>
    Result<U> and_then(Function<Result<U>(T)> func);
  };

  Result<Foo> CreateFoo();
  Result<Bar> CreateBarFromFoo(const Foo& foo);

  Result<Bar> bar = CreateFoo().and_then(CreateBarFromFoo);

``pw::Result<T>::or_else``
--------------------------
The ``pw::Result<T>::or_else`` member function will return ``*this`` if it
contains a value. Otherwise, it will return the result of the provided function.
The function must return a type convertible to a ``pw::Result<T>`` or ``void``.
This is particularly useful for handling errors.

.. code-block:: cpp

  // Expositional prototype of or_else:
  template <typename T>
  class Result {
    template <typename U>
      requires std::is_convertible_v<U, Result<T>>
    Result<T> or_else(Function<U(Status)> func);

    Result<T> or_else(Function<void(Status)> func);
  };

  // Without or_else:
  Result<Image> GetCuteCat(const Image& image) {
    Result<Image> cropped = CropToCat(image);
    if (!cropped.ok()) {
      PW_LOG_ERROR("Failed to crop cat: %d", cropped.status().code());
      return cropped.status();
    }
    return cropped;
  }

  // With or_else:
  Result<Image> GetCuteCat(const Image& image) {
    return CropToCat(image).or_else(
        [](Status s) { PW_LOG_ERROR("Failed to crop cat: %d", s.code()); });
  }

Another useful scenario for ``pw::Result<T>::or_else`` is providing a default
value that is expensive to compute. Typically, default values are provided by
using ``pw::Result<T>::value_or``, but that requires the default value to be
constructed regardless of whether we actually need it.

.. code-block:: cpp

  // With value_or:
  Image GetCuteCat(const Image& image) {
    // GenerateCuteCat() must execute regardless of the success of CropToCat
    return CropToCat(image).value_or(GenerateCuteCat());
  }

  // With or_else:
  Image GetCuteCat(const Image& image) {
    // GenerateCuteCat() only executes if CropToCat fails.
    return *CropToCat(image).or_else([](Status) { return GenerateCuteCat(); });
  }

``pw::Result<T>::transform``
----------------------------
The ``pw::Result<T>::transform`` member method will return a ``pw::Result<U>``
which contains the result of the invocation of the given function if ``*this``
contains a value. Otherwise, it returns a ``pw::Result<U>`` with the same
``pw::Status`` value as ``*this``.

The monadic methods for ``and_then`` and ``transform`` are fairly similar. The
primary difference is that ``and_then`` requires the provided function to return
a ``pw::Result``, whereas ``transform`` functions can return any type. Users
should be aware that if they provide a function that returns a ``pw::Result`` to
``transform``, this will return a ``pw::Result<pw::Result<U>>``.

.. code-block:: cpp

  // Expositional prototype of transform:
  template <typename T>
  class Result {
    template <typename U>
    Result<U> transform(Function<U(T)> func);
  };

  Result<int> ConvertStringToInteger(std::string_view);
  int MultiplyByTwo(int x);

  Result<int> x = ConvertStringToInteger("42")
                    .transform(MultiplyByTwo);

-----------
Size report
-----------
The table below showcases the difference in size between functions returning a
Status with an output pointer, and functions returning a Result, in various
situations.

Note that these are simplified examples which do not necessarily reflect the
usage of Result in real code. Make sure to always run your own size reports to
check if Result is suitable for you.

.. include:: result_size

------
Zephyr
------
To enable ``pw_result`` for Zephyr add ``CONFIG_PIGWEED_RESULT=y`` to the
project's configuration.
