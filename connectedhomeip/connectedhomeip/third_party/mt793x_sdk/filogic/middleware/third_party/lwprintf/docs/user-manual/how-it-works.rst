.. _how_it_works:

How it works
============

LwPRINTF library supports ``2`` different formatting output types:

* Write formatted data to user input array
* Directly print formatted characters by calling ``output_function`` for every formatted character in the input string

Text formatting is based on input format string followed by the data parameters.
It is mostly used to prepare numeric data types to human readable format.

.. note::
    LwPRINTF is open-source implementation of regular *stdio.h* library in C language.
    It implements only output functions, excluding input scanning features

Formatting functions take input *format string* followed by (optional) different data types.
Internal algorithm scans character by character to understand type of expected data user would like to have printed.

Every format specifier starts with letter ``%``, followed by optional set of flags, widths and other sets of characters.
Last part of every specifier is its type, that being type of format and data to display.

.. tip::
    To print number ``1234`` in human readable format, use specifier ``%d``.
    With default configuration, call ``lwprintf_printf("%d", 1234);`` and it will print ``"1234"``.

Check section :ref:`um_format_specifier` for list of all formats and data types

Character output function
*************************

API functions printing characters directly to the output stream (ex. :c:macro:`lwprintf_printf`),
require output function to be set during initialization procedure.

Output function is called by the API for every character to be printed/transmitted by the application.

.. note::
    Output function is set during initialization procedure. 
	If not set (set as ``NULL``), it is not possible to use API function which directly print
	characters to output stream. Application is then limited only to API functions
	that write formatted data to input buffer.

Notes to consider:

* Output function must return same character as it was used as an input parameter to consider successful print
* Output function will receive ``(int)'\0'`` character to indicate no more characters will follow in this API call
* Single output function may be used for different LwPRINTF instances

.. literalinclude:: ../examples_src/example_minimal.c
    :language: c
    :linenos:
    :caption: Absolute minimum example to support direct output

.. toctree::
    :maxdepth: 2