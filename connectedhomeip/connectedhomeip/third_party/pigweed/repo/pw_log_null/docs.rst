.. _module-pw_log_null:

-----------
pw_log_null
-----------
``pw_log_null`` is a ``pw_log backend`` that ignores all ``pw_log`` statements.
The backend implements ``PW_LOG`` with an empty inline function. Using an empty
function ensures that the arguments are evaluated and their types are correct.
Since the function is inline in the header, the compiler will optimize out the
function call.

This backend can be used to completely disable ``pw_log``, which may be helpful
in certain development situations (e.g. to avoid circular dependencies).

.. tip::
  If you are concerned about the resource demands of logging, try tokenizing
  logs with :ref:`module-pw_tokenizer` and :ref:`module-pw_log_tokenized`
  instead of disabling logs completely. Tokenized logs provide exactly same
  information as plain text logs but use dramatically less resources.
