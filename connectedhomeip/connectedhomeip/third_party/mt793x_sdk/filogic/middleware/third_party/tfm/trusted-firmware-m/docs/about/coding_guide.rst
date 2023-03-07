##############################
Yet another coding standard :)
##############################

.. Warning::

    Every rule has an exception so if you disagree or dislike then reach out!

The coding style of TF-M project is based on
`Linux coding style <https://www.kernel.org/doc/html/v4.10/process/coding-style.html>`__
but there are updates for domain specific conventions as listed below.

TF-M also reuses code from other SW projects, e.g. ``CMSIS_5``, which
means some areas of code may have different styles. We use common sense approach
and new code may inherit coding style from external projects but it needs to
remain within clear scope.

The guidance below is provided as a help. It isn't meant to be a definitive
list.

As implied in the :doc:`contributing guide </docs/processes/contributing>`
maintainers have the right to decide on what's acceptable in case of any
divergence.

.. Warning::

    Text files do not fall within these rules as they may require different formatting.``

****************
Consistent style
****************
The code needs to be consistent with itself, so if existing code in the file
violates listed coding style rules then it is better to follow existing style
in the file and not break consistency by following the rules listed here.

You may need to add a comment in the commit description to clarify this.

List of rules
=============

- Use 4 spaces indentation. No tabs.
- Use ``lower_case_with_underscore`` for filenames, variable and function names.
- Use standard types e.g. ``uint32_t``, ``uint16_t``, ``uint8_t``, ``int32_t``
  etc.
- Use ``uintptr_t`` type when representing addresses as numbers.
- Use static for all private functions and variables.
- Use void argument if your function doesn't contain any argument.
- Use C90 ``/* */`` for comments rather than C99 ``//``
- No trailing spaces in code.
- Max 80 characters length. Text files are exception as stated above.
- Open curly brace ``{`` at the same if/else/while/for/switch statement line.
- Use curly braces ``{ }`` for one line statement bodies also.
- Put open curly brace in the line below the function header.
- Order function parameters so that input params are before output params.
- Declare local variables at the beginning of the function.
- Define macros in all caps i.e. ``CAPITAL_WITH_UNDERSCORE``.
- Use typedefs **ONLY** for function prototype declarations.
- Type definitions in ``lower_case_with_underscore`` ended by ``_t``.
- Do not use typedef for other constructs as it obfuscates code.
- Do not use bitfields.
- Use static for all global variables to reduce the variable scope.
- Use enumeration for error codes to keep the code readable.
- Use descriptive variable and functions names.
- Put the correct license header at the beginning of the file.
- Keep the files (.h/.c) self-contained, i.e. put include dependencies in the
  file.
- Put appropriate header define guard in .h files: ``__HEADER_NAME__``.
  Any divergence from this rules should be clearly documented.
- In a .c file, #include it's own header file first.
- Document all the public functions in the header file only.
- Document all the private functions in the source file only.
- Add "extern C " definition for C++ support in the header files.
- In the switch statement, aligned cases with the switch keyword.
- For enums, use upper case letters with digits and underscore only.
- Do not code while eating.

--------------

*Copyright (c) 2018-2019, Arm Limited. All rights reserved.*
