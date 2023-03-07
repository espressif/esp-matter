.. _module-pw_third_party_googletest:

==========
GoogleTest
==========
The ``$dir_pw_third_party/googletest/`` module provides various helpers to
optionally use full upstream GoogleTest/GoogleMock with
:ref:`module-pw_unit_test`.

----------------------------------------
Using upstream GoogleTest and GoogleMock
----------------------------------------
If you want to use the full upstream GoogleTest/GoogleMock, you must do the
following:

Submodule
=========
Add GoogleTest to your workspace with the following command.

.. code-block:: sh

  git submodule add https://github.com/google/googletest third_party/googletest

GN
==
* Set the GN var ``dir_pw_third_party_googletest`` to the location of the
  GoogleTest source. If you used the command above this will be
  ``//third_party/googletest``.
* Set the GN var ``pw_unit_test_MAIN = dir_pigweed + "/third_party/googletest:gmock_main"``.
* Set the GN var
  ``pw_unit_test_GOOGLETEST_BACKEND = "//third_party/googletest"``.

CMake
=====
* Set the ``dir_pw_third_party_googletest`` to the location of the
  GoogleTest source.
* Set the var ``pw_unit_test_MAIN`` to ``pw_third_party.googletest.gmock_main``.
* Set the var ``pw_unit_test_GOOGLETEST_BACKEND`` to
  ``pw_third_party.googletest``.

.. note::

  Not all unit tests build properly with upstream GoogleTest yet. This is a
  work in progress.
