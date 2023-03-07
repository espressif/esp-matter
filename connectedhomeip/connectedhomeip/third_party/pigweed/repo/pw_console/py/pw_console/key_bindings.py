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
# pylint: skip-file
"""Console key bindings."""
import logging
from typing import Dict, List

from prompt_toolkit.filters import (
    Condition,
    has_focus,
)
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.key_binding.bindings.focus import focus_next, focus_previous
from prompt_toolkit.key_binding.key_bindings import Binding


__all__ = ('create_key_bindings',)

_LOG = logging.getLogger(__package__)

DEFAULT_KEY_BINDINGS: Dict[str, List[str]] = {
    'global.open-user-guide': ['f1'],
    'global.open-menu-search': ['c-p'],
    'global.focus-previous-widget': ['c-left'],
    'global.focus-next-widget': ['c-right', 's-tab'],
    'global.exit-no-confirmation': ['c-x c-c'],
    'global.exit-with-confirmation': ['c-d'],
    'log-pane.shift-line-to-top': ['z t'],
    'log-pane.shift-line-to-center': ['z z'],
    'log-pane.toggle-follow': ['f'],
    'log-pane.toggle-wrap-lines': ['w'],
    'log-pane.toggle-table-view': ['t'],
    'log-pane.duplicate-log-pane': ['insert'],
    'log-pane.remove-duplicated-log-pane': ['delete'],
    'log-pane.clear-history': ['C'],
    'log-pane.toggle-follow': ['f'],
    'log-pane.toggle-web-browser': ['O'],
    'log-pane.move-cursor-up': ['up', 'k'],
    'log-pane.move-cursor-down': ['down', 'j'],
    'log-pane.visual-select-up': ['s-up'],
    'log-pane.visual-select-down': ['s-down'],
    'log-pane.visual-select-all': ['N', 'c-r'],
    'log-pane.deselect-cancel-search': ['c-c'],
    'log-pane.scroll-page-up': ['pageup'],
    'log-pane.scroll-page-down': ['pagedown'],
    'log-pane.scroll-to-top': ['g'],
    'log-pane.scroll-to-bottom': ['G'],
    'log-pane.save-copy': ['c-o'],
    'log-pane.search': ['/', 'c-f'],
    'log-pane.search-next-match': ['n', 'c-s', 'c-g'],
    'log-pane.search-previous-match': ['N', 'c-r'],
    'log-pane.search-apply-filter': ['escape c-f'],
    'log-pane.clear-filters': ['escape c-r'],
    'search-toolbar.toggle-column': ['c-t'],
    'search-toolbar.toggle-invert': ['c-v'],
    'search-toolbar.toggle-matcher': ['c-n'],
    'search-toolbar.cancel': ['escape', 'c-c', 'c-d'],
    'search-toolbar.create-filter': ['escape c-f'],
    'window-manager.move-pane-left': ['escape c-left'],  # Alt-Ctrl-
    'window-manager.move-pane-right': ['escape c-right'],  # Alt-Ctrl-
    # NOTE: c-up and c-down seem swapped in prompt-toolkit
    'window-manager.move-pane-down': ['escape c-up'],  # Alt-Ctrl-
    'window-manager.move-pane-up': ['escape c-down'],  # Alt-Ctrl-
    'window-manager.enlarge-pane': ['escape ='],  # Alt-= (mnemonic: Alt Plus)
    'window-manager.shrink-pane': [
        'escape -'
    ],  # Alt-minus (mnemonic: Alt Minus)
    'window-manager.shrink-split': ['escape ,'],  # Alt-, (mnemonic: Alt <)
    'window-manager.enlarge-split': ['escape .'],  # Alt-. (mnemonic: Alt >)
    'window-manager.focus-prev-pane': ['escape c-p'],  # Ctrl-Alt-p
    'window-manager.focus-next-pane': ['escape c-n'],  # Ctrl-Alt-n
    'window-manager.balance-window-panes': ['c-u'],
    'python-repl.copy-output-selection': ['c-c'],
    'python-repl.copy-all-output': ['escape c-c'],
    'python-repl.copy-clear-or-cancel': ['c-c'],
    'python-repl.paste-to-input': ['c-v'],
    'python-repl.history-search': ['c-r'],
    'python-repl.snippet-search': ['c-t'],
    'save-as-dialog.cancel': ['escape', 'c-c', 'c-d'],
    'quit-dialog.no': ['escape', 'n', 'c-c'],
    'quit-dialog.yes': ['y', 'c-d'],
    'command-runner.cancel': ['escape', 'c-c'],
    'command-runner.select-previous-item': ['up', 's-tab'],
    'command-runner.select-next-item': ['down', 'tab'],
    'help-window.close': ['q', 'f1', 'escape'],
    'help-window.copy-all': ['c-c'],
}


def create_key_bindings(console_app) -> KeyBindings:
    """Create custom key bindings.

    This starts with the key bindings, defined by `prompt-toolkit`, but adds the
    ones which are specific for the console_app. A console_app instance
    reference is passed in so key bind functions can access it.
    """

    key_bindings = KeyBindings()
    register = console_app.prefs.register_keybinding

    @register(
        'global.open-user-guide',
        key_bindings,
        filter=Condition(lambda: not console_app.modal_window_is_open()),
    )
    def show_help(event):
        """Toggle user guide window."""
        console_app.user_guide_window.toggle_display()

    # F2 is ptpython settings
    # F3 is ptpython history

    @register(
        'global.open-menu-search',
        key_bindings,
        filter=Condition(lambda: not console_app.modal_window_is_open()),
    )
    def show_command_runner(event):
        """Open command runner window."""
        console_app.open_command_runner_main_menu()

    @register('global.focus-previous-widget', key_bindings)
    def app_focus_previous(event):
        """Move focus to the previous widget."""
        focus_previous(event)

    @register('global.focus-next-widget', key_bindings)
    def app_focus_next(event):
        """Move focus to the next widget."""
        focus_next(event)

    # Bindings for when the ReplPane input field is in focus.
    # These are hidden from help window global keyboard shortcuts since the
    # method names end with `_hidden`.
    @register(
        'python-repl.copy-clear-or-cancel',
        key_bindings,
        filter=has_focus(console_app.pw_ptpython_repl),
    )
    def handle_ctrl_c_hidden(event):
        """Reset the python repl on Ctrl-c"""
        console_app.repl_pane.ctrl_c()

    @register('global.exit-no-confirmation', key_bindings)
    def quit_no_confirm(event):
        """Quit without confirmation."""
        event.app.exit()

    @register(
        'global.exit-with-confirmation',
        key_bindings,
        filter=console_app.pw_ptpython_repl.input_empty_if_in_focus_condition()
        | has_focus(console_app.quit_dialog),
    )
    def quit(event):
        """Quit with confirmation dialog."""
        # If the python repl is in focus and has text input then Ctrl-d will
        # delete forward characters instead.
        console_app.quit_dialog.open_dialog()

    @register(
        'python-repl.paste-to-input',
        key_bindings,
        filter=has_focus(console_app.pw_ptpython_repl),
    )
    def paste_into_repl(event):
        """Reset the python repl on Ctrl-c"""
        console_app.repl_pane.paste_system_clipboard_to_input_buffer()

    @register(
        'python-repl.history-search',
        key_bindings,
        filter=has_focus(console_app.pw_ptpython_repl),
    )
    def history_search(event):
        """Open the repl history search dialog."""
        console_app.open_command_runner_history()

    @register(
        'python-repl.snippet-search',
        key_bindings,
        filter=has_focus(console_app.pw_ptpython_repl),
    )
    def insert_snippet(event):
        """Open the repl snippet search dialog."""
        console_app.open_command_runner_snippets()

    @register(
        'python-repl.copy-all-output',
        key_bindings,
        filter=console_app.repl_pane.input_or_output_has_focus(),
    )
    def copy_repl_output_text(event):
        """Copy all Python output to the system clipboard."""
        console_app.repl_pane.copy_all_output_text()

    return key_bindings
