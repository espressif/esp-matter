# Copyright 2021 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Python logging helper fuctions."""

import copy
from datetime import datetime
import json
import logging
import tempfile
from typing import Any, Dict, Iterable, Iterator, Optional


def all_loggers() -> Iterator[logging.Logger]:
    """Iterates over all loggers known to Python logging."""
    manager = logging.getLogger().manager  # type: ignore[attr-defined]

    for logger_name in manager.loggerDict:  # pylint: disable=no-member
        yield logging.getLogger(logger_name)


def create_temp_log_file(
    prefix: Optional[str] = None, add_time: bool = True
) -> str:
    """Create a unique tempfile for saving logs.

    Example format: /tmp/pw_console_2021-05-04_151807_8hem6iyq
    """
    if not prefix:
        prefix = str(__package__)

    # Grab the current system timestamp as a string.
    isotime = datetime.now().isoformat(sep="_", timespec="seconds")
    # Timestamp string should not have colons in it.
    isotime = isotime.replace(":", "")

    if add_time:
        prefix += f"_{isotime}"

    log_file_name = None
    with tempfile.NamedTemporaryFile(
        prefix=f"{prefix}_", delete=False
    ) as log_file:
        log_file_name = log_file.name

    return log_file_name


def set_logging_last_resort_file_handler(
    file_name: Optional[str] = None,
) -> None:
    log_file = file_name if file_name else create_temp_log_file()
    logging.lastResort = logging.FileHandler(log_file)


def disable_stdout_handlers(logger: logging.Logger) -> None:
    """Remove all stdout and stdout & stderr logger handlers."""
    for handler in copy.copy(logger.handlers):
        # Must use type() check here since this returns True:
        #   isinstance(logging.FileHandler, logging.StreamHandler)
        # pylint: disable=unidiomatic-typecheck
        if type(handler) == logging.StreamHandler:
            logger.removeHandler(handler)
        # pylint: enable=unidiomatic-typecheck


def setup_python_logging(
    last_resort_filename: Optional[str] = None,
    loggers_with_no_propagation: Optional[Iterable[logging.Logger]] = None,
) -> None:
    """Disable log handlers for full screen prompt_toolkit applications."""
    if not loggers_with_no_propagation:
        loggers_with_no_propagation = []
    disable_stdout_handlers(logging.getLogger())

    if logging.lastResort is not None:
        set_logging_last_resort_file_handler(last_resort_filename)

    for logger in list(all_loggers()):
        # Prevent stdout handlers from corrupting the prompt_toolkit UI.
        disable_stdout_handlers(logger)
        if logger in loggers_with_no_propagation:
            continue
        # Make sure all known loggers propagate to the root logger.
        logger.propagate = True

    # Prevent these loggers from propagating to the root logger.
    hidden_host_loggers = [
        "pw_console",
        "pw_console.plugins",
        # prompt_toolkit triggered debug log messages
        "prompt_toolkit",
        "prompt_toolkit.buffer",
        "parso.python.diff",
        "parso.cache",
        "pw_console.serial_debug_logger",
    ]
    for logger_name in hidden_host_loggers:
        logging.getLogger(logger_name).propagate = False

    # Set asyncio log level to WARNING
    logging.getLogger("asyncio").setLevel(logging.WARNING)

    # Always set DEBUG level for serial debug.
    logging.getLogger("pw_console.serial_debug_logger").setLevel(logging.DEBUG)


def log_record_to_json(record: logging.LogRecord) -> str:
    log_dict: Dict[str, Any] = {}
    log_dict["message"] = record.getMessage()
    log_dict["levelno"] = record.levelno
    log_dict["levelname"] = record.levelname
    log_dict["args"] = record.args

    if hasattr(record, "extra_metadata_fields") and (
        record.extra_metadata_fields  # type: ignore
    ):
        fields = record.extra_metadata_fields  # type: ignore
        log_dict["fields"] = {}
        for key, value in fields.items():
            if key == "msg":
                log_dict["message"] = value
                continue

            log_dict["fields"][key] = str(value)

    return json.dumps(log_dict)


class JsonLogFormatter(logging.Formatter):
    """Json Python logging Formatter

    Use this formatter to log pw_console messages to a file in json
    format. Column values normally shown in table view will be populated in the
    'fields' key.

    Example log entry:

    .. code-block:: json

       {
         "message": "System init",
         "levelno": 20,
         "levelname": "INF",
         "args": [
           "0:00",
           "pw_system ",
           "System init"
         ],
         "fields": {
           "module": "pw_system",
           "file": "pw_system/init.cc",
           "timestamp": "0:00"
         }
       }

    Example usage:

    .. code-block:: python

       import logging
       import pw_console.python_logging

       _DEVICE_LOG = logging.getLogger('rpc_device')

       json_filehandler = logging.FileHandler('logs.json', encoding='utf-8')
       json_filehandler.setLevel(logging.DEBUG)
       json_filehandler.setFormatter(
           pw_console.python_logging.JsonLogFormatter())
       _DEVICE_LOG.addHandler(json_filehandler)

    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def format(self, record: logging.LogRecord) -> str:
        return log_record_to_json(record)
