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
"""
The label module defines a class to store and manipulate size reports.
"""

from collections import defaultdict
from dataclasses import dataclass
from typing import Iterable, Dict, Sequence, Tuple, List, Optional
import csv


@dataclass
class Label:
    """Return type of DataSourceMap generator."""

    name: str
    size: int
    capacity: Optional[int] = None
    exists_both: Optional[bool] = None
    parents: Tuple[str, ...] = ()

    def is_new(self) -> bool:
        return (not self.exists_both) and self.size > 0

    def is_del(self) -> bool:
        return (not self.exists_both) and self.size < 0


@dataclass
class LabelInfo:
    size: int = 0
    capacity: Optional[int] = None
    exists_both: Optional[bool] = None


class _LabelMap:
    """Private module to hold parent and child labels with their size."""

    _label_map: Dict[str, Dict[str, LabelInfo]]

    def __init__(self):
        self._label_map = defaultdict(lambda: defaultdict(LabelInfo))

    def remove(
        self, parent_label: str, child_label: Optional[str] = None
    ) -> None:
        """Delete entire parent label or the child label."""
        if child_label:
            del self._label_map[parent_label][child_label]
        else:
            del self._label_map[parent_label]

    def __getitem__(self, parent_label: str) -> Dict[str, LabelInfo]:
        """Indexing LabelMap using '[]' operators by specifying a label."""
        return self._label_map[parent_label]

    def __contains__(self, parent_label: str) -> bool:
        return parent_label in self._label_map

    def map_generator(self) -> Iterable[Tuple[str, Dict[str, LabelInfo]]]:
        for parent_label, label_dict in self._label_map.items():
            yield parent_label, label_dict


class _DataSource:
    """Private module to store a data source name with a _LabelMap."""

    def __init__(self, name: str):
        self._name = name
        self._ds_label_map = _LabelMap()

    def get_name(self) -> str:
        return self._name

    def add_label(
        self,
        parent_label: str,
        child_label: str,
        size: int,
        diff_exist: Optional[bool] = None,
    ) -> None:
        curr_label_info = self._ds_label_map[parent_label][child_label]
        curr_label_info.size += size
        if curr_label_info.exists_both is None:
            curr_label_info.exists_both = diff_exist

    def __getitem__(self, parent_label: str) -> Dict[str, LabelInfo]:
        return self._ds_label_map[parent_label]

    def __contains__(self, parent_label: str) -> bool:
        return parent_label in self._ds_label_map

    def label_map_generator(self) -> Iterable[Tuple[str, Dict[str, LabelInfo]]]:
        for parent_label, label_dict in self._ds_label_map.map_generator():
            yield parent_label, label_dict


class DataSourceMap:
    """Module to store an array of DataSources and capacities.

    An organize way to store a hierachy of labels and their sizes.
    Includes a capacity array to hold regex patterns for applying
    capacities to matching label names.

    """

    _BASE_TOTAL_LABEL = 'total'

    @classmethod
    def from_bloaty_tsv(cls, raw_tsv: Iterable[str]) -> 'DataSourceMap':
        """Read in Bloaty TSV output and store in DataSourceMap."""
        reader = csv.reader(raw_tsv, delimiter='\t')
        top_row = next(reader)
        vmsize_index = top_row.index('vmsize')
        ds_map_tsv = cls(top_row[:vmsize_index])
        for row in reader:
            ds_map_tsv.insert_label_hierachy(
                row[:vmsize_index], int(row[vmsize_index])
            )
        return ds_map_tsv

    def __init__(self, data_sources_names: Iterable[str]):
        self._data_sources = list(
            _DataSource(name) for name in ['base', *data_sources_names]
        )
        self._capacity_array: List[Tuple[str, int]] = []

    def label_exists(
        self, ds_index: int, parent_label: str, child_label: str
    ) -> bool:
        return (parent_label in self._data_sources[ds_index]) and (
            child_label in self._data_sources[ds_index][parent_label]
        )

    def insert_label_hierachy(
        self,
        label_hierarchy: Iterable[str],
        size: int,
        diff_exist: Optional[bool] = None,
    ) -> None:
        """Insert a hierachy of labels with its size."""

        # Insert initial '__base__' data source that holds the
        # running total size.
        self._data_sources[0].add_label(
            '__base__', self._BASE_TOTAL_LABEL, size
        )
        complete_label_hierachy = [self._BASE_TOTAL_LABEL, *label_hierarchy]
        for index in range(len(complete_label_hierachy) - 1):
            if complete_label_hierachy[index]:
                self._data_sources[index + 1].add_label(
                    complete_label_hierachy[index],
                    complete_label_hierachy[index + 1],
                    size,
                    diff_exist,
                )

    def add_capacity(self, regex_pattern: str, capacity: int) -> None:
        """Insert regex pattern and capacity into dictionary."""
        self._capacity_array.append((regex_pattern, capacity))

    def diff(self, base: 'DataSourceMap') -> 'DiffDataSourceMap':
        """Calculate the difference between 2 DataSourceMaps."""
        diff_dsm = DiffDataSourceMap(self.get_ds_names())
        curr_parent = self._BASE_TOTAL_LABEL

        # Iterate through base labels at each datasource index.
        last_data_source = len(base.get_ds_names()) - 1
        parent_data_source_index = last_data_source + 1
        for b_label in base.labels(last_data_source):
            if last_data_source > 0:
                curr_parent = b_label.parents[-1]
            lb_hierachy_names = [*b_label.parents, b_label.name]

            # Check if label exists in target binary DataSourceMap.
            # Subtract base from target size and insert diff size
            # into DiffDataSourceMap.
            if self.label_exists(
                parent_data_source_index, curr_parent, b_label.name
            ):
                diff_size = (
                    self._data_sources[parent_data_source_index][curr_parent][
                        b_label.name
                    ].size
                ) - b_label.size

                if diff_size:
                    diff_dsm.insert_label_hierachy(
                        lb_hierachy_names, diff_size, True
                    )
                else:
                    diff_dsm.insert_label_hierachy(lb_hierachy_names, 0, True)

            # label is not present in target - insert with negative size
            else:
                diff_dsm.insert_label_hierachy(
                    lb_hierachy_names, -1 * b_label.size, False
                )

        # Iterate through all of target labels
        # to find labels new to target from base.
        for t_label in self.labels(last_data_source):
            if last_data_source > 0:
                curr_parent = t_label.parents[-1]

            # New addition to target
            if not base.label_exists(
                parent_data_source_index, curr_parent, t_label.name
            ):
                diff_dsm.insert_label_hierachy(
                    [*t_label.parents, f"{t_label.name}"], t_label.size, False
                )

        return diff_dsm

    def get_total_size(self) -> int:
        return self._data_sources[0]['__base__'][self._BASE_TOTAL_LABEL].size

    def get_ds_names(self) -> Tuple[str, ...]:
        """List of DataSource names for easy indexing and reference."""
        return tuple(
            data_source.get_name() for data_source in self._data_sources[1:]
        )

    def labels(self, ds_index: Optional[int] = None) -> Iterable[Label]:
        """Generator that yields a Label depending on specified data source.

        Args:
            ds_index: Integer index of target data source.

        Returns:
            Iterable Label objects.
        """
        ds_index = len(self._data_sources) if ds_index is None else ds_index + 2
        yield from self._per_data_source_generator(
            tuple(), self._data_sources[1:ds_index]
        )

    def _per_data_source_generator(
        self,
        parent_labels: Tuple[str, ...],
        data_sources: Sequence[_DataSource],
    ) -> Iterable[Label]:
        """Recursive generator to return Label based off parent labels."""
        for ds_index, curr_ds in enumerate(data_sources):
            for parent_label, label_map in curr_ds.label_map_generator():
                if not parent_labels:
                    curr_parent = self._BASE_TOTAL_LABEL
                else:
                    curr_parent = parent_labels[-1]
                if parent_label == curr_parent:
                    for child_label, label_info in label_map.items():
                        if len(data_sources) == 1:
                            yield Label(
                                child_label,
                                label_info.size,
                                parents=parent_labels,
                                exists_both=label_info.exists_both,
                            )
                        else:
                            yield from self._per_data_source_generator(
                                (*parent_labels, child_label),
                                data_sources[ds_index + 1 :],
                            )


class DiffDataSourceMap(DataSourceMap):
    """DataSourceMap that holds diff information."""

    def has_diff_sublabels(self, top_ds_label: str) -> bool:
        """Checks if first datasource is identical."""
        for label in self.labels():
            if label.size != 0:
                if (label.parents and (label.parents[0] == top_ds_label)) or (
                    label.name == top_ds_label
                ):
                    return True
        return False
