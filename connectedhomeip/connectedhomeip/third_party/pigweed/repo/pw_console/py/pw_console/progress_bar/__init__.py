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
"""Pigweed Console progress bar functions."""
from pw_console.progress_bar.progress_bar_state import TASKS_CONTEXTVAR
from pw_console.progress_bar.progress_bar_task_counter import (
    ProgressBarTaskCounter,
)

__all__ = [
    'start_progress',
    'update_progress',
]


def start_progress(task_name: str, total: int, hide_eta=False):
    progress_state = TASKS_CONTEXTVAR.get()

    progress_state.startup_progress_bar_impl()

    assert progress_state.instance is not None

    progress_state.tasks[task_name] = ProgressBarTaskCounter(
        name=task_name,
        total=total,
        prompt_toolkit_counter=progress_state.instance(
            range(total), label=task_name
        ),
    )
    ptc = progress_state.tasks[task_name].prompt_toolkit_counter
    ptc.hide_eta = hide_eta  # type: ignore


def update_progress(
    task_name: str, count=1, completed=False, canceled=False, new_total=None
):
    progress_state = TASKS_CONTEXTVAR.get()
    # The caller may not actually get canceled and will continue trying to
    # update after an interrupt.
    if task_name not in progress_state.tasks:
        return

    # Take one action
    if completed:
        progress_state.tasks[task_name].mark_completed()
    elif canceled:
        progress_state.tasks[task_name].mark_canceled()
    elif new_total:
        progress_state.tasks[task_name].set_new_total(new_total)
    else:
        progress_state.tasks[task_name].update(count)

    # Check if all tasks are complete
    if (
        progress_state.instance is not None
        and progress_state.all_tasks_complete
    ):
        if hasattr(progress_state.instance, '__exit__'):
            progress_state.instance.__exit__()
