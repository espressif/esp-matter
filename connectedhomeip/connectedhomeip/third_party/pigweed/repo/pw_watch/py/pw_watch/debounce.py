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
"""Run an interruptable, cancellable function after debouncing run requests"""

import enum
import logging
import threading
from abc import ABC, abstractmethod

_LOG = logging.getLogger('pw_watch')


class DebouncedFunction(ABC):
    """Function to be run by Debouncer"""

    @abstractmethod
    def run(self) -> None:
        """Run the function"""

    @abstractmethod
    def cancel(self) -> bool:
        """Cancel an in-progress run of the function.
        Must be called from different thread than run().
        Returns true if run was successfully cancelled, false otherwise"""

    @abstractmethod
    def on_complete(self, cancelled: bool = False) -> None:
        """Called after run() finishes. If true, cancelled indicates
        cancel() was invoked during the last run()"""

    # Note: The debounce uses threads. Since there is no way to guarantee which
    # thread recieves a KeyboardInterrupt, it is necessary catch this event
    # in all debouncer threads and forward it to the user.
    @abstractmethod
    def on_keyboard_interrupt(self):
        """Called when keyboard interrupt is delivered to a debouncer thread"""


class State(enum.Enum):
    IDLE = 1  # ------- Transistions to: DEBOUNCING
    DEBOUNCING = 2  # - Transistions to: RUNNING
    RUNNING = 3  # ---- Transistions to: INTERRUPTED or COOLDOWN
    INTERRUPTED = 4  # - Transistions to: RERUN
    COOLDOWN = 5  # ---- Transistions to: IDLE
    RERUN = 6  # ------- Transistions to: IDLE (but triggers a press)


class Debouncer:
    """Run an interruptable, cancellable function with debouncing"""

    def __init__(self, function: DebouncedFunction) -> None:
        super().__init__()
        self.function = function

        self.state = State.IDLE

        self.debounce_seconds = 1
        self.debounce_timer = None

        self.cooldown_seconds = 1
        self.cooldown_timer = None

        self.rerun_event_description = ''

        self.lock = threading.Lock()

    def press(self, event_description: str = '') -> None:
        """Try to run the function for the class. If the function is recently
        started, this may push out the deadline for actually starting. If the
        function is already running, will interrupt the function"""
        with self.lock:
            self._press_unlocked(event_description)

    def _press_unlocked(self, event_description: str) -> None:
        _LOG.debug('Press - state = %s', str(self.state))
        if self.state == State.IDLE:
            if event_description:
                _LOG.info('%s', event_description)
            self._start_debounce_timer()
            self._transition(State.DEBOUNCING)

        elif self.state == State.DEBOUNCING:
            self._start_debounce_timer()

        elif self.state == State.RUNNING:
            # When the function is already running but we get an incoming
            # event, go into the INTERRUPTED state to signal that we should
            # re-try running afterwards.

            # Push an empty line to flush ongoing I/O in subprocess.
            _LOG.error('')

            # Surround the error message with newlines to make it stand out.
            _LOG.error('')
            _LOG.error('Event while running: %s', event_description)
            _LOG.error('')

            self.function.cancel()
            self._transition(State.INTERRUPTED)
            self.rerun_event_description = event_description

        elif self.state == State.INTERRUPTED:
            # Function is running but was already interrupted. Do nothing.
            _LOG.debug('Ignoring press - interrupted')

        elif self.state == State.COOLDOWN:
            # Function just finished and we are cooling down; so trigger rerun.
            _LOG.debug('Got event in cooldown; scheduling rerun')
            self._transition(State.RERUN)
            self.rerun_event_description = event_description

    def _transition(self, new_state: State) -> None:
        _LOG.debug('State: %s -> %s', self.state, new_state)
        self.state = new_state

    def _start_debounce_timer(self):
        assert self.lock.locked()
        if self.state == State.DEBOUNCING:
            self.debounce_timer.cancel()
        self.debounce_timer = threading.Timer(
            self.debounce_seconds, self._run_function
        )
        self.debounce_timer.start()

    # Called from debounce_timer thread.
    def _run_function(self):
        try:
            with self.lock:
                assert self.state == State.DEBOUNCING
                self.debounce_timer = None
                self._transition(State.RUNNING)

            # Must run the function without the lock held so further press()
            # calls don't deadlock.
            _LOG.debug('Running debounced function')
            self.function.run()

            _LOG.debug('Finished running debounced function')
            with self.lock:
                if self.state == State.RUNNING:
                    self.function.on_complete(cancelled=False)
                    self._transition(State.COOLDOWN)
                elif self.state == State.INTERRUPTED:
                    self.function.on_complete(cancelled=True)
                    self._transition(State.RERUN)
                self._start_cooldown_timer()
        # Ctrl-C on Unix generates KeyboardInterrupt
        # Ctrl-Z on Windows generates EOFError
        except (KeyboardInterrupt, EOFError):
            self.function.on_keyboard_interrupt()

    def _start_cooldown_timer(self):
        assert self.lock.locked()
        self.cooldown_timer = threading.Timer(
            self.cooldown_seconds, self._exit_cooldown
        )
        self.cooldown_timer.start()

    # Called from cooldown_timer thread.
    def _exit_cooldown(self):
        try:
            with self.lock:
                self.cooldown_timer = None
                rerun = self.state == State.RERUN
                self._transition(State.IDLE)

                # If we were in the RERUN state, then re-trigger the event.
                if rerun:
                    self._press_unlocked(
                        'Rerunning: ' + self.rerun_event_description
                    )

        # Ctrl-C on Unix generates KeyboardInterrupt
        # Ctrl-Z on Windows generates EOFError
        except (KeyboardInterrupt, EOFError):
            self.function.on_keyboard_interrupt()
