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
"""Functions to create checkboxes for menus and toolbars."""

import sys
from typing import Callable, Iterable, Optional, NamedTuple

from prompt_toolkit.formatted_text.base import OneStyleAndTextTuple
from prompt_toolkit.formatted_text import StyleAndTextTuples

_KEY_SEPARATOR = ' '
_CHECKED_BOX = '[✓]'

if sys.platform in ['win32']:
    _CHECKED_BOX = '[x]'


class ToolbarButton(NamedTuple):
    key: Optional[str] = None
    description: Optional[str] = 'Button'
    mouse_handler: Optional[Callable] = None
    is_checkbox: bool = False
    checked: Optional[Callable] = None


def to_checkbox(
    checked: bool,
    mouse_handler: Optional[Callable] = None,
    end: str = ' ',
    unchecked_style: str = 'class:checkbox',
    checked_style: str = 'class:checkbox-checked',
) -> OneStyleAndTextTuple:
    text = _CHECKED_BOX if checked else '[ ]'
    text += end
    style = checked_style if checked else unchecked_style
    if mouse_handler:
        return (style, text, mouse_handler)
    return (style, text)


def to_checkbox_text(checked: bool, end=' '):
    return to_checkbox(checked, end=end)[1]


def to_setting(
    checked: bool,
    text: str,
    active_style='class:toolbar-setting-active',
    inactive_style='',
    mouse_handler=None,
):
    """Apply a style to text if checked is True."""
    style = active_style if checked else inactive_style
    if mouse_handler:
        return (style, text, mouse_handler)
    return (style, text)


def to_checkbox_with_keybind_indicator(
    checked: bool,
    key: str,
    description: str,
    mouse_handler=None,
    base_style: str = '',
    **checkbox_kwargs,
):
    """Create a clickable keybind indicator with checkbox for toolbars."""
    if mouse_handler:
        return to_keybind_indicator(
            key,
            description,
            mouse_handler,
            leading_fragments=[
                to_checkbox(checked, mouse_handler, **checkbox_kwargs)
            ],
            base_style=base_style,
        )
    return to_keybind_indicator(
        key,
        description,
        leading_fragments=[to_checkbox(checked, **checkbox_kwargs)],
        base_style=base_style,
    )


def to_keybind_indicator(
    key: str,
    description: str,
    mouse_handler: Optional[Callable] = None,
    leading_fragments: Optional[Iterable] = None,
    middle_fragments: Optional[Iterable] = None,
    base_style: str = '',
    key_style: str = 'class:keybind',
    description_style: str = 'class:keyhelp',
):
    """Create a clickable keybind indicator for toolbars."""
    if base_style:
        base_style += ' '

    fragments: StyleAndTextTuples = []
    fragments.append((base_style + 'class:toolbar-button-decoration', ' '))

    def append_fragment_with_base_style(frag_list, fragment) -> None:
        if mouse_handler:
            frag_list.append(
                (base_style + fragment[0], fragment[1], mouse_handler)
            )
        else:
            frag_list.append((base_style + fragment[0], fragment[1]))

    # Add any starting fragments first
    if leading_fragments:
        for fragment in leading_fragments:
            append_fragment_with_base_style(fragments, fragment)

    # Function name
    if mouse_handler:
        fragments.append(
            (base_style + description_style, description, mouse_handler)
        )
    else:
        fragments.append((base_style + description_style, description))

    if middle_fragments:
        for fragment in middle_fragments:
            append_fragment_with_base_style(fragments, fragment)

    # Separator and keybind
    if key:
        if mouse_handler:
            fragments.append(
                (base_style + description_style, _KEY_SEPARATOR, mouse_handler)
            )
            fragments.append((base_style + key_style, key, mouse_handler))
        else:
            fragments.append((base_style + description_style, _KEY_SEPARATOR))
            fragments.append((base_style + key_style, key))

    fragments.append((base_style + 'class:toolbar-button-decoration', ' '))
    return fragments
