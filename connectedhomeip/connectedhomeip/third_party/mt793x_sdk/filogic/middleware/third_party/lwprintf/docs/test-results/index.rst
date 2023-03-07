.. _test_results:

Test results
============

Library is put under several tests to ensure correct output format.
Results are underneath with information about number of passed and failed tests.

.. note::
    Majority of failed tests are linked to precision digits with floating-point based specifiers.
    This is considered as *OK* since failures are visible at higher number of precision digits,
    not affecting final results. Keep in mind that effective number of precision digits
    with ``float`` type is ``7`` and for ``double`` is ``15``.

With the exception to additional specifiers, supported only by *LwPRINTF* library,
all tests are compared against ``stdio printf`` library included in *Microsoft Visual Studio C/C++ compiler*.

.. literalinclude:: ../../tests/test_results.test
    :encoding: utf-16
    :linenos:
    :caption: Test results of the library

.. toctree::
    :maxdepth: 2
