# Copyright 2022 The Pigweed Authors
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
"""Wrapper fuctions to add borders around prompt_toolkit containers."""

from typing import Callable, List, Optional, Union

from prompt_toolkit.layout import (
    AnyContainer,
    FormattedTextControl,
    HSplit,
    VSplit,
    Window,
)


def create_border(
    # pylint: disable=too-many-arguments
    content: AnyContainer,
    content_height: Optional[int] = None,
    title: str = '',
    border_style: Union[Callable[[], str], str] = '',
    base_style: Union[Callable[[], str], str] = '',
    top: bool = True,
    bottom: bool = True,
    left: bool = True,
    right: bool = True,
    horizontal_char: str = '━',
    vertical_char: str = '┃',
    top_left_char: str = '┏',
    top_right_char: str = '┓',
    bottom_left_char: str = '┗',
    bottom_right_char: str = '┛',
    left_margin_columns: int = 0,
    right_margin_columns: int = 0,
) -> HSplit:
    """Wrap any prompt_toolkit container in a border."""

    top_border_items: List[AnyContainer] = []
    if left:
        top_border_items.append(
            Window(width=1, height=1, char=top_left_char, style=border_style)
        )

    title_text = None
    if title:
        title_text = FormattedTextControl(
            [('', f'{horizontal_char}{horizontal_char} {title} ')]
        )

    top_border_items.append(
        Window(
            title_text,
            char=horizontal_char,
            # Expand width to max available space
            dont_extend_width=False,
            style=border_style,
        )
    )
    if right:
        top_border_items.append(
            Window(width=1, height=1, char=top_right_char, style=border_style)
        )

    content_items: List[AnyContainer] = []
    if left:
        content_items.append(
            Window(
                width=1,
                height=content_height,
                char=vertical_char,
                style=border_style,
            )
        )

    if left_margin_columns > 0:
        content_items.append(
            Window(
                width=left_margin_columns,
                height=content_height,
                char=' ',
                style=border_style,
            )
        )
    content_items.append(content)
    if right_margin_columns > 0:
        content_items.append(
            Window(
                width=right_margin_columns,
                height=content_height,
                char=' ',
                style=border_style,
            )
        )

    if right:
        content_items.append(
            Window(width=1, height=2, char=vertical_char, style=border_style)
        )

    bottom_border_items: List[AnyContainer] = []
    if left:
        bottom_border_items.append(
            Window(width=1, height=1, char=bottom_left_char)
        )
    bottom_border_items.append(
        Window(
            char=horizontal_char,
            # Expand width to max available space
            dont_extend_width=False,
        )
    )
    if right:
        bottom_border_items.append(
            Window(width=1, height=1, char=bottom_right_char)
        )

    rows: List[AnyContainer] = []
    if top:
        rows.append(VSplit(top_border_items, height=1, padding=0))
    rows.append(VSplit(content_items, height=content_height))
    if bottom:
        rows.append(
            VSplit(bottom_border_items, height=1, padding=0, style=border_style)
        )

    return HSplit(rows, style=base_style)
