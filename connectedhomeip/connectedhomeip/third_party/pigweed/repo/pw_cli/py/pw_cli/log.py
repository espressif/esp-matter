# Copyright 2020 The Pigweed Authors
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
"""Tools for configuring Python logging."""

import logging
from pathlib import Path
from typing import NamedTuple, Optional, Union, Iterator

from pw_cli.color import colors as pw_cli_colors
from pw_cli.env import pigweed_environment

# Log level used for captured output of a subprocess run through pw.
LOGLEVEL_STDOUT = 21

# Log level indicating a irrecoverable failure.
LOGLEVEL_FATAL = 70


class _LogLevel(NamedTuple):
    level: int
    color: str
    ascii: str
    emoji: str


# Shorten all the log levels to 3 characters for column-aligned logs.
# Color the logs using ANSI codes.
# fmt: off
_LOG_LEVELS = (
    _LogLevel(LOGLEVEL_FATAL,   'bold_red',     'FTL', '☠️ '),
    _LogLevel(logging.CRITICAL, 'bold_magenta', 'CRT', '‼️ '),
    _LogLevel(logging.ERROR,    'red',          'ERR', '❌'),
    _LogLevel(logging.WARNING,  'yellow',       'WRN', '⚠️ '),
    _LogLevel(logging.INFO,     'magenta',      'INF', 'ℹ️ '),
    _LogLevel(LOGLEVEL_STDOUT,  'cyan',         'OUT', '💬'),
    _LogLevel(logging.DEBUG,    'blue',         'DBG', '👾'),
)
# fmt: on

_LOG = logging.getLogger(__name__)
_STDERR_HANDLER = logging.StreamHandler()


def c_to_py_log_level(c_level: int) -> int:
    """Converts pw_log C log-level macros to Python logging levels."""
    return c_level * 10


def main() -> None:
    """Shows how logs look at various levels."""

    # Force the log level to make sure all logs are shown.
    _LOG.setLevel(logging.DEBUG)

    # Log one message for every log level.
    _LOG.log(LOGLEVEL_FATAL, 'An irrecoverable error has occurred!')
    _LOG.critical('Something important has happened!')
    _LOG.error('There was an error on our last operation')
    _LOG.warning('Looks like something is amiss; consider investigating')
    _LOG.info('The operation went as expected')
    _LOG.log(LOGLEVEL_STDOUT, 'Standard output of subprocess')
    _LOG.debug('Adding 1 to i')


def _setup_handler(
    handler: logging.Handler,
    formatter: logging.Formatter,
    level: Union[str, int],
    logger: logging.Logger,
) -> None:
    handler.setLevel(level)
    handler.setFormatter(formatter)
    logger.addHandler(handler)


def install(
    level: Union[str, int] = logging.INFO,
    use_color: Optional[bool] = None,
    hide_timestamp: bool = False,
    log_file: Optional[Union[str, Path]] = None,
    logger: Optional[logging.Logger] = None,
    debug_log: Optional[Union[str, Path]] = None,
) -> None:
    """Configures the system logger for the default pw command log format.

    If you have Python loggers separate from the root logger you can use
    `pw_cli.log.install` to get the Pigweed log formatting there too. For
    example: ::

        import logging

        import pw_cli.log

        pw_cli.log.install(
            level=logging.INFO,
            use_color=True,
            hide_timestamp=False,
            log_file=(Path.home() / 'logs.txt'),
            logger=logging.getLogger(__package__),
        )

    Args:
      level: The logging level to apply. Default: `logging.INFO`.
      use_color: When `True` include ANSI escape sequences to colorize log
          messages.
      hide_timestamp: When `True` omit timestamps from the log formatting.
      log_file: File to send logs into instead of the terminal.
      logger: Python Logger instance to install Pigweed formatting into.
          Defaults to the Python root logger: `logging.getLogger()`.
      debug_log: File to log to from all levels, regardless of chosen log level.
          Logs will go here in addition to the terminal.

    """
    if not logger:
        logger = logging.getLogger()

    colors = pw_cli_colors(use_color)

    env = pigweed_environment()
    if env.PW_SUBPROCESS or hide_timestamp:
        # If the logger is being run in the context of a pw subprocess, the
        # time and date are omitted (since pw_cli.process will provide them).
        timestamp_fmt = ''
    else:
        # This applies a gray background to the time to make the log lines
        # distinct from other input, in a way that's easier to see than plain
        # colored text.
        timestamp_fmt = colors.black_on_white('%(asctime)s') + ' '

    formatter = logging.Formatter(
        timestamp_fmt + '%(levelname)s %(message)s', '%Y%m%d %H:%M:%S'
    )

    # Set the log level on the root logger to NOTSET, so that all logs
    # propagated from child loggers are handled.
    logging.getLogger().setLevel(logging.NOTSET)

    # Always set up the stderr handler, even if it isn't used.
    _setup_handler(_STDERR_HANDLER, formatter, level, logger)

    if log_file:
        # Set utf-8 encoding for the log file. Encoding errors may come up on
        # Windows if the default system encoding is set to cp1250.
        _setup_handler(
            logging.FileHandler(log_file, encoding='utf-8'),
            formatter,
            level,
            logger,
        )
        # Since we're using a file, filter logs out of the stderr handler.
        _STDERR_HANDLER.setLevel(logging.CRITICAL + 1)

    if debug_log:
        # Set utf-8 encoding for the log file. Encoding errors may come up on
        # Windows if the default system encoding is set to cp1250.
        _setup_handler(
            logging.FileHandler(debug_log, encoding='utf-8'),
            formatter,
            logging.DEBUG,
            logger,
        )

    if env.PW_EMOJI:
        name_attr = 'emoji'
        colorize = lambda ll: str
    else:
        name_attr = 'ascii'
        colorize = lambda ll: getattr(colors, ll.color)

    for log_level in _LOG_LEVELS:
        name = getattr(log_level, name_attr)
        logging.addLevelName(log_level.level, colorize(log_level)(name))


def all_loggers() -> Iterator[logging.Logger]:
    """Iterates over all loggers known to Python logging."""
    manager = logging.getLogger().manager  # type: ignore[attr-defined]

    for logger_name in manager.loggerDict:  # pylint: disable=no-member
        yield logging.getLogger(logger_name)


def set_all_loggers_minimum_level(level: int) -> None:
    """Increases the log level to the specified value for all known loggers."""
    for logger in all_loggers():
        if logger.isEnabledFor(level - 1):
            logger.setLevel(level)


if __name__ == '__main__':
    install()
    main()
