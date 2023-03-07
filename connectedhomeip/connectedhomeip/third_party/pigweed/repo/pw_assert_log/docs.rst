.. _module-pw_assert_log:

=============
pw_assert_log
=============

-----------------
pw_assert_BACKEND
-----------------
This assert backend implements the ``pw_assert:check`` facade, by routing the
``PW_CHECK()``/``PW_CRASH()`` macros into ``PW_LOG``  with the
``PW_LOG_LEVEL_FATAL`` log level. This is an easy way to tokenize your assert
messages, by using the ``pw_log_tokenized`` log backend for logging, then using
``pw_assert_log`` to route the tokenized messages into the tokenized log
handler.

To use this module:

1. Set your assert backend: ``pw_assert_BACKEND = dir_pw_assert_log``
2. Ensure your logging backend knows how to handle the assert failure flag

----------------------
pw_assert_LITE_BACKEND
----------------------
This assert backend implements the ``pw_assert:assert`` facade, by routing the
``PW_ASSERT()`` macros into ``PW_LOG`` with the ``PW_LOG_LEVEL_FATAL`` log
level. This is an easy way to forward your asserts to a native macro assert
API if it is already constexpr safe such as on Android.
