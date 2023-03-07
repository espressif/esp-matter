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
"""Module for size report ASCII tables from DataSourceMaps."""

import enum
from typing import (
    Iterable,
    Tuple,
    Union,
    Type,
    List,
    Optional,
    NamedTuple,
    cast,
)

from pw_bloat.label import DataSourceMap, DiffDataSourceMap, Label


class AsciiCharset(enum.Enum):
    """Set of ASCII characters for drawing tables."""

    TL = '+'
    TM = '+'
    TR = '+'
    ML = '+'
    MM = '+'
    MR = '+'
    BL = '+'
    BM = '+'
    BR = '+'
    V = '|'
    H = '-'
    HH = '='


class LineCharset(enum.Enum):
    """Set of line-drawing characters for tables."""

    TL = '┌'
    TM = '┬'
    TR = '┐'
    ML = '├'
    MM = '┼'
    MR = '┤'
    BL = '└'
    BM = '┴'
    BR = '┘'
    V = '│'
    H = '─'
    HH = '═'


class _Align(enum.Enum):
    CENTER = 0
    LEFT = 1
    RIGHT = 2


def get_label_status(curr_label: Label) -> str:
    if curr_label.is_new():
        return 'NEW'
    if curr_label.is_del():
        return 'DEL'
    return ''


def diff_sign_sizes(size: int, diff_mode: bool) -> str:
    if diff_mode:
        size_sign = '+' if size > 0 else ''
        return f"{size_sign}{size:,}"
    return f"{size:,}"


class BloatTableOutput:
    """ASCII Table generator from DataSourceMap."""

    _RST_PADDING_WIDTH = 6
    _DEFAULT_MAX_WIDTH = 80

    class _LabelContent(NamedTuple):
        name: str
        size: int
        label_status: str

    def __init__(
        self,
        ds_map: Union[DiffDataSourceMap, DataSourceMap],
        col_max_width: int = _DEFAULT_MAX_WIDTH,
        charset: Union[Type[AsciiCharset], Type[LineCharset]] = AsciiCharset,
        rst_output: bool = False,
        diff_label: Optional[str] = None,
    ):
        self._data_source_map = ds_map
        self._cs = charset
        self._total_size = 0
        col_names = [*self._data_source_map.get_ds_names(), 'sizes']
        self._diff_mode = False
        self._diff_label = diff_label
        if isinstance(self._data_source_map, DiffDataSourceMap):
            col_names = ['diff', *col_names]
            self._diff_mode = True
        self._col_names = col_names
        self._additional_padding = 0
        self._ascii_table_rows: List[str] = []
        self._rst_output = rst_output
        self._total_divider = self._cs.HH.value
        if self._rst_output:
            self._total_divider = self._cs.H.value
            self._additional_padding = self._RST_PADDING_WIDTH

        self._col_widths = self._generate_col_width(col_max_width)

    def _generate_col_width(self, col_max_width: int) -> List[int]:
        """Find column width for all data sources and sizes."""
        max_len_size = 0
        diff_len_col_width = 0

        col_list = [
            len(ds_name) for ds_name in self._data_source_map.get_ds_names()
        ]
        for curr_label in self._data_source_map.labels():
            self._total_size += curr_label.size
            max_len_size = max(
                len(diff_sign_sizes(self._total_size, self._diff_mode)),
                len(diff_sign_sizes(curr_label.size, self._diff_mode)),
                max_len_size,
            )
            for index, parent_label in enumerate(
                [*curr_label.parents, curr_label.name]
            ):
                if len(parent_label) > col_max_width:
                    col_list[index] = col_max_width
                elif len(parent_label) > col_list[index]:
                    col_list[index] = len(parent_label)

        diff_same = 0
        if self._diff_mode:
            col_list = [len('Total'), *col_list]
            diff_same = len('(SAME)')
        col_list.append(max(max_len_size, len('sizes'), diff_same))

        if self._diff_label is not None:
            sum_all_col_names = sum(col_list)
            if sum_all_col_names < len(self._diff_label):
                diff_len_col_width = (
                    len(self._diff_label) - sum_all_col_names
                ) // len(self._col_names)

        return [
            (x + self._additional_padding + diff_len_col_width)
            for x in col_list
        ]

    def _diff_label_names(
        self,
        old_labels: Optional[Tuple[_LabelContent, ...]],
        new_labels: Tuple[_LabelContent, ...],
    ) -> Tuple[_LabelContent, ...]:
        """Return difference between arrays of labels."""

        if old_labels is None:
            return new_labels
        diff_list = []
        for (new_lb, old_lb) in zip(new_labels, old_labels):
            if (new_lb.name == old_lb.name) and (new_lb.size == old_lb.size):
                diff_list.append(self._LabelContent('', 0, ''))
            else:
                diff_list.append(new_lb)

        return tuple(diff_list)

    def _label_title_row(self) -> List[str]:
        label_rows = []
        label_cells = ''
        divider_cells = ''
        for width in self._col_widths:
            label_cells += ' ' * width + ' '
            divider_cells += (self._cs.H.value * width) + self._cs.H.value
        if self._diff_label is not None:
            label_cells = self._diff_label.center(len(label_cells[:-1]), ' ')
        label_rows.extend(
            [
                f"{self._cs.TL.value}{divider_cells[:-1]}{self._cs.TR.value}",
                f"{self._cs.V.value}{label_cells}{self._cs.V.value}",
                f"{self._cs.ML.value}{divider_cells[:-1]}{self._cs.MR.value}",
            ]
        )
        return label_rows

    def create_table(self) -> str:
        """Parse DataSourceMap to create ASCII table."""
        curr_lb_hierachy = None
        last_diff_name = ''
        if self._diff_mode:
            self._ascii_table_rows.extend([*self._label_title_row()])
        else:
            self._ascii_table_rows.extend(
                [self._create_border(True, self._cs.H.value)]
            )
        self._ascii_table_rows.extend([*self._create_title_row()])

        has_entries = False

        for curr_label in self._data_source_map.labels():
            if curr_label.size == 0:
                continue

            has_entries = True

            new_lb_hierachy = tuple(
                [
                    *self._get_ds_label_size(curr_label.parents),
                    self._LabelContent(
                        curr_label.name,
                        curr_label.size,
                        get_label_status(curr_label),
                    ),
                ]
            )
            diff_list = self._diff_label_names(
                curr_lb_hierachy, new_lb_hierachy
            )
            curr_lb_hierachy = new_lb_hierachy

            if curr_label.parents and curr_label.parents[0] == last_diff_name:
                continue
            if (
                self._diff_mode
                and diff_list[0].name
                and (
                    not cast(
                        DiffDataSourceMap, self._data_source_map
                    ).has_diff_sublabels(diff_list[0].name)
                )
            ):
                if (len(self._ascii_table_rows) > 5) and (
                    self._ascii_table_rows[-1][0] != '+'
                ):

                    self._ascii_table_rows.append(
                        self._row_divider(
                            len(self._col_names), self._cs.H.value
                        )
                    )
                self._ascii_table_rows.append(
                    self._create_same_label_row(1, diff_list[0].name)
                )

                last_diff_name = curr_label.parents[0]
            else:
                self._ascii_table_rows += self._create_diff_rows(diff_list)

        if self._rst_output and self._ascii_table_rows[-1][0] == '+':
            self._ascii_table_rows.pop()

        self._ascii_table_rows.extend(
            [*self._create_total_row(is_empty=not has_entries)]
        )

        return '\n'.join(self._ascii_table_rows) + '\n'

    def _create_same_label_row(self, col_index: int, label: str) -> str:
        label_row = ''
        for col in range(len(self._col_names) - 1):
            if col == col_index:
                curr_cell = self._create_cell(label, False, col, _Align.LEFT)
            else:
                curr_cell = self._create_cell('', False, col)
            label_row += curr_cell
        label_row += self._create_cell(
            "(SAME)", True, len(self._col_widths) - 1, _Align.RIGHT
        )
        return label_row

    def _get_ds_label_size(
        self, parent_labels: Tuple[str, ...]
    ) -> Iterable[_LabelContent]:
        """Produce label, size pairs from parent label names."""
        parent_label_sizes = []
        for index, target_label in enumerate(parent_labels):
            for curr_label in self._data_source_map.labels(index):
                if curr_label.name == target_label:
                    diff_label = get_label_status(curr_label)
                    parent_label_sizes.append(
                        self._LabelContent(
                            curr_label.name, curr_label.size, diff_label
                        )
                    )
                    break
        return parent_label_sizes

    def _create_total_row(self, is_empty: bool = False) -> Iterable[str]:
        complete_total_rows = []

        if self._diff_mode and is_empty:
            # When diffing two identical binaries, output a row indicating that
            # the two are the same.
            no_diff_row = ''
            for i in range(len(self._col_names)):
                if i == 0:
                    no_diff_row += self._create_cell(
                        'N/A', False, i, _Align.CENTER
                    )
                elif i == len(self._col_names) - 1:
                    no_diff_row += self._create_cell('0', True, i)
                else:
                    no_diff_row += self._create_cell(
                        '(same)', False, i, _Align.CENTER
                    )
            complete_total_rows.append(no_diff_row)

        complete_total_rows.append(
            self._row_divider(len(self._col_names), self._total_divider)
        )
        total_row = ''

        for i in range(len(self._col_names)):
            if i == 0:
                total_row += self._create_cell('Total', False, i, _Align.LEFT)
            elif i == len(self._col_names) - 1:
                total_size_str = diff_sign_sizes(
                    self._total_size, self._diff_mode
                )
                total_row += self._create_cell(total_size_str, True, i)
            else:
                total_row += self._create_cell('', False, i, _Align.CENTER)

        complete_total_rows.extend(
            [total_row, self._create_border(False, self._cs.H.value)]
        )
        return complete_total_rows

    def _create_diff_rows(
        self, diff_list: Tuple[_LabelContent, ...]
    ) -> Iterable[str]:
        """Create rows for each label according to its index in diff_list."""
        curr_row = ''
        diff_index = 0
        diff_rows = []
        for index, label_content in enumerate(diff_list):
            if label_content.name:
                if self._diff_mode:
                    curr_row += self._create_cell(
                        label_content.label_status, False, 0
                    )
                    diff_index = 1
                for cell_index in range(
                    diff_index, len(diff_list) + diff_index
                ):
                    if cell_index == index + diff_index:
                        if (
                            cell_index == diff_index
                            and len(self._ascii_table_rows) > 5
                            and not self._rst_output
                        ):
                            diff_rows.append(
                                self._row_divider(
                                    len(self._col_names), self._cs.H.value
                                )
                            )
                        if (
                            len(label_content.name) + self._additional_padding
                        ) > self._col_widths[cell_index]:
                            curr_row = self._multi_row_label(
                                label_content.name, cell_index
                            )
                            break
                        curr_row += self._create_cell(
                            label_content.name, False, cell_index, _Align.LEFT
                        )
                    else:
                        curr_row += self._create_cell('', False, cell_index)

                # Add size end of current row.
                curr_size = diff_sign_sizes(label_content.size, self._diff_mode)
                curr_row += self._create_cell(
                    curr_size, True, len(self._col_widths) - 1, _Align.RIGHT
                )
                diff_rows.append(curr_row)
                if self._rst_output:
                    diff_rows.append(
                        self._row_divider(
                            len(self._col_names), self._cs.H.value
                        )
                    )
                curr_row = ''

        return diff_rows

    def _create_cell(
        self,
        content: str,
        last_cell: bool,
        col_index: int,
        align: Optional[_Align] = _Align.RIGHT,
    ) -> str:
        v_border = self._cs.V.value
        if self._rst_output and content:
            content = content.replace('_', '\\_')
        pad_diff = self._col_widths[col_index] - len(content)
        padding = (pad_diff // 2) * ' '
        odd_pad = ' ' if pad_diff % 2 == 1 else ''
        string_cell = ''

        if align == _Align.CENTER:
            string_cell = f'{v_border}{odd_pad}{padding}{content}{padding}'
        elif align == _Align.LEFT:
            string_cell = f'{v_border}{content}{padding*2}{odd_pad}'
        elif align == _Align.RIGHT:
            string_cell = f'{v_border}{padding*2}{odd_pad}{content}'

        if last_cell:
            string_cell += self._cs.V.value
        return string_cell

    def _multi_row_label(self, content: str, target_col_index: int) -> str:
        """Split content name into multiple rows within correct column."""
        max_len = self._col_widths[target_col_index] - self._additional_padding
        split_content = '...'.join(
            content[max_len:][i : i + max_len - 3]
            for i in range(0, len(content[max_len:]), max_len - 3)
        )
        split_content = f"{content[:max_len]}...{split_content}"
        split_tab_content = [
            split_content[i : i + max_len]
            for i in range(0, len(split_content), max_len)
        ]
        multi_label = []
        curr_row = ''
        for index, cut_content in enumerate(split_tab_content):
            last_cell = False
            for blank_cell_index in range(len(self._col_names)):
                if blank_cell_index == target_col_index:
                    curr_row += self._create_cell(
                        cut_content, False, target_col_index, _Align.LEFT
                    )
                else:
                    if blank_cell_index == len(self._col_names) - 1:
                        if index == len(split_tab_content) - 1:
                            break
                        last_cell = True
                    curr_row += self._create_cell(
                        '', last_cell, blank_cell_index
                    )
            multi_label.append(curr_row)
            curr_row = ''

        return '\n'.join(multi_label)

    def _row_divider(self, col_num: int, h_div: str) -> str:
        l_border = ''
        r_border = ''
        row_div = ''
        for col in range(col_num):
            if col == 0:
                l_border = self._cs.ML.value
                r_border = ''
            elif col == (col_num - 1):
                l_border = self._cs.MM.value
                r_border = self._cs.MR.value
            else:
                l_border = self._cs.MM.value
                r_border = ''

            row_div += f"{l_border}{self._col_widths[col] * h_div}{r_border}"
        return row_div

    def _create_title_row(self) -> Iterable[str]:
        title_rows = []
        title_cells = ''
        last_cell = False
        for index, curr_name in enumerate(self._col_names):
            if index == len(self._col_names) - 1:
                last_cell = True
            title_cells += self._create_cell(
                curr_name, last_cell, index, _Align.CENTER
            )
        title_rows.extend(
            [
                title_cells,
                self._row_divider(len(self._col_names), self._cs.HH.value),
            ]
        )
        return title_rows

    def _create_border(self, top: bool, h_div: str):
        """Top or bottom borders of ASCII table."""
        row_div = ''
        for col in range(len(self._col_names)):
            if top:
                if col == 0:
                    l_div = self._cs.TL.value
                    r_div = ''
                elif col == (len(self._col_names) - 1):
                    l_div = self._cs.TM.value
                    r_div = self._cs.TR.value
                else:
                    l_div = self._cs.TM.value
                    r_div = ''
            else:
                if col == 0:
                    l_div = self._cs.BL.value
                    r_div = ''
                elif col == (len(self._col_names) - 1):
                    l_div = self._cs.BM.value
                    r_div = self._cs.BR.value
                else:
                    l_div = self._cs.BM.value
                    r_div = ''

            row_div += f"{l_div}{self._col_widths[col] * h_div}{r_div}"
        return row_div


class RstOutput:
    """Tabular output in ASCII format, which is also valid RST."""

    def __init__(
        self, ds_map: DataSourceMap, table_label: Optional[str] = None
    ):
        self._data_source_map = ds_map
        self._table_label = table_label
        self._diff_mode = False
        if isinstance(self._data_source_map, DiffDataSourceMap):
            self._diff_mode = True

    def create_table(self) -> str:
        """Initializes RST table and builds first row."""
        table_builder = [
            '\n.. list-table::',
            '   :widths: auto',
            '   :header-rows: 1\n',
        ]
        header_cols = ['Label', 'Segment', 'Delta']
        for i, col_name in enumerate(header_cols):
            list_space = '*' if i == 0 else ' '
            table_builder.append(f"   {list_space} - {col_name}")

        return '\n'.join(table_builder) + f'\n{self.add_report_row()}\n'

    def _label_status_unchanged(self, parent_lb_name: str) -> bool:
        """Determines if parent label has no status change in diff mode."""
        for curr_lb in self._data_source_map.labels():
            if curr_lb.size != 0:
                if (
                    curr_lb.parents and (parent_lb_name == curr_lb.parents[0])
                ) or (curr_lb.name == parent_lb_name):
                    if get_label_status(curr_lb) != '':
                        return False
        return True

    def add_report_row(self) -> str:
        """Add in new size report row with Label, Segment, and Delta.

        Returns:
            RST string that is the current row with a full symbols
            table breakdown of the corresponding segment.
        """
        table_rows = []
        curr_row = []
        curr_label_name = ''
        for parent_lb in self._data_source_map.labels(0):
            if parent_lb.size != 0:
                if (self._table_label is not None) and (
                    curr_label_name != self._table_label
                ):
                    curr_row.append(f'   * - {self._table_label} ')
                    curr_label_name = self._table_label
                else:
                    curr_row.append('   * -')
                curr_row.extend(
                    [
                        f'     - .. dropdown:: {parent_lb.name}',
                        '            :animate: fade-in\n',
                        '            .. list-table::',
                        '               :widths: auto\n',
                    ]
                )
                if self._label_status_unchanged(parent_lb.name):
                    skip_status = 1, '*'
                else:
                    skip_status = 0, ' '
                for curr_lb in self._data_source_map.labels():
                    if (curr_lb.size != 0) and (
                        (
                            curr_lb.parents
                            and (parent_lb.name == curr_lb.parents[0])
                        )
                        or (curr_lb.name == parent_lb.name)
                    ):
                        sign_size = diff_sign_sizes(
                            curr_lb.size, self._diff_mode
                        )
                        curr_status = get_label_status(curr_lb)
                        curr_name = curr_lb.name.replace('_', '\\_')
                        to_extend = [
                            f'               * - {curr_status}',
                            f'               {skip_status[1]} - {sign_size}',
                            f'                 - {curr_name}\n',
                        ][skip_status[0] :]
                        curr_row.extend(to_extend)
                curr_row.append(
                    f'     - {diff_sign_sizes(parent_lb.size, self._diff_mode)}'
                )
            table_rows.extend(curr_row)
            curr_row = []

        # No size difference.
        if len(table_rows) == 0:
            table_rows.extend(
                [f'\n   * - {self._table_label}', '     - (ALL)', '     - 0']
            )

        return '\n'.join(table_rows)
