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
"""Pigweed Console progress bar task state."""

from contextvars import ContextVar
import copy
from dataclasses import dataclass, field
import signal
from typing import Dict, Optional, Union

from prompt_toolkit.application import get_app_or_none
from prompt_toolkit.shortcuts import ProgressBar
from prompt_toolkit.shortcuts.progress_bar import formatters

from pw_console.progress_bar.progress_bar_impl import (
    IterationsPerSecondIfNotHidden,
    ProgressBarImpl,
    TextIfNotHidden,
    TimeLeftIfNotHidden,
)
from pw_console.progress_bar.progress_bar_task_counter import (
    ProgressBarTaskCounter,
)
from pw_console.style import generate_styles

CUSTOM_FORMATTERS = [
    formatters.Label(suffix=': '),
    formatters.Rainbow(
        formatters.Bar(start='|Pigw', end='|', sym_a='e', sym_b='d!', sym_c=' ')
    ),
    formatters.Text(' '),
    formatters.Progress(),
    formatters.Text(' ['),
    formatters.Percentage(),
    formatters.Text('] in '),
    formatters.TimeElapsed(),
    TextIfNotHidden(' ('),
    IterationsPerSecondIfNotHidden(),
    TextIfNotHidden('/s, eta: '),
    TimeLeftIfNotHidden(),
    TextIfNotHidden(')'),
]


def prompt_toolkit_app_running() -> bool:
    existing_app = get_app_or_none()
    if existing_app:
        return True
    return False


@dataclass
class ProgressBarState:
    """Pigweed Console wide state for all repl progress bars.

    An instance of this class is intended to be a global variable."""

    tasks: Dict[str, ProgressBarTaskCounter] = field(default_factory=dict)
    instance: Optional[Union[ProgressBar, ProgressBarImpl]] = None

    def _install_sigint_handler(self) -> None:
        """Add ctrl-c handling if not running inside pw_console"""

        def handle_sigint(_signum, _frame):
            # Shut down the ProgressBar prompt_toolkit application
            prog_bar = self.instance
            if prog_bar is not None and hasattr(prog_bar, '__exit__'):
                prog_bar.__exit__()
            raise KeyboardInterrupt

        signal.signal(signal.SIGINT, handle_sigint)

    def startup_progress_bar_impl(self):
        prog_bar = self.instance
        if not prog_bar:
            if prompt_toolkit_app_running():
                prog_bar = ProgressBarImpl(
                    style=get_app_or_none().style, formatters=CUSTOM_FORMATTERS
                )
            else:
                self._install_sigint_handler()
                prog_bar = ProgressBar(
                    style=generate_styles(), formatters=CUSTOM_FORMATTERS
                )
                # Start the ProgressBar prompt_toolkit application in a separate
                # thread.
                prog_bar.__enter__()
            self.instance = prog_bar
        return self.instance

    def cleanup_finished_tasks(self) -> None:
        for task_name in copy.copy(list(self.tasks.keys())):
            task = self.tasks[task_name]
            if task.completed or task.canceled:
                ptc = task.prompt_toolkit_counter
                self.tasks.pop(task_name, None)
                if (
                    self.instance
                    and self.instance.counters
                    and ptc in self.instance.counters
                ):
                    self.instance.counters.remove(ptc)

    @property
    def all_tasks_complete(self) -> bool:
        tasks_complete = [
            task.completed or task.canceled
            for _task_name, task in self.tasks.items()
        ]
        self.cleanup_finished_tasks()
        return all(tasks_complete)

    def cancel_all_tasks(self):
        self.tasks = {}
        if self.instance is not None:
            self.instance.counters = []

    def get_container(self):
        prog_bar = self.instance
        if prog_bar is not None and hasattr(prog_bar, '__pt_container__'):
            return prog_bar.__pt_container__()
        return None


TASKS_CONTEXTVAR = ContextVar(
    'pw_console_progress_bar_tasks', default=ProgressBarState()
)
