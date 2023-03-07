.. _module-pw_log_string:

=============
pw_log_string
=============
``pw_log_string`` is a partial backend for ``pw_log``. This backend fowards the
``PW_LOG_*`` macros to the ``pw_log_string:handler`` facade which is backed by
a C API. ``pw_log_string:handler`` does not implement the full C API, leaving
projects to provide their own implementation of
``pw_log_string_HandleMessageVaList``. See ``pw_log_basic`` for a similar
``pw_log`` backend that also provides an implementation.

As this module passes the log message, file name, and module name as a string to
the handler function, it's relatively expensive and not well suited for
space-constrained devices. This module is oriented towards usage on a host
(e.g. a simulated device).

Note that ``pw_log_string:handler`` may be used even when it's not used
as the backend for ``pw_log`` via ``pw_log_string``. For example it can be
useful to mix tokenized and string based logging in case you have a C ABI where
tokenization can not be used on the other side.

---------------
Getting started
---------------
This module is extremely minimal to set up:

1. Implement ``pw_log_string_HandleMessageVaList()``
2. Set ``pw_log_BACKEND`` to ``"$dir_pw_log_string"``
3. Set ``pw_log_string_HANDLER_BACKEND`` to point to the source set that
   implements ``pw_log_string_HandleMessageVaList()``

What exactly ``pw_log_string_HandleMessageVaList()`` should do is entirely up to
the implementation. ``pw_log_basic``'s log handler is one example, but it's also
possible to encode as protobuf and send over a TCP port, write to a file, or
blink an LED to log as morse code.
