#!/usr/bin/env python3
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
"""Tests for bloaty configuration tooling."""

import unittest
import os
import logging
import sys
from pw_bloat.label import DataSourceMap, Label

LIST_LABELS = [
    Label(name='main()', size=30, parents=tuple(['FLASH', '.code'])),
    Label(name='foo()', size=100, parents=tuple(['RAM', '.heap'])),
    Label(name='bar()', size=220, parents=tuple(['RAM', '.heap'])),
]

logger = logging.getLogger()
logger.level = logging.DEBUG
logger.addHandler(logging.StreamHandler(sys.stdout))


def get_test_map():
    pw_root = os.environ.get("PW ROOT")
    filename = f"{pw_root}/pigweed/pw_bloat/test_label.csv"
    with open(filename, 'r') as csvfile:
        ds_map = DataSourceMap.from_bloaty_tsv(csvfile)
        capacity_patterns = [("^__TEXT$", 459), ("^_", 920834)]
        for cap_pattern, cap_size in capacity_patterns:
            ds_map.add_capacity(cap_pattern, cap_size)
    return ds_map


class LabelStructTest(unittest.TestCase):
    """Testing class for the label structs."""

    def test_data_source_total_size(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        self.assertEqual(ds_map.get_total_size(), 0)

    def test_data_source_single_insert_total_size(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        ds_map.insert_label_hierachy(['FLASH', '.code', 'main()'], 30)
        self.assertEqual(ds_map.get_total_size(), 30)

    def test_data_source_multiple_insert_total_size(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        ds_map.insert_label_hierachy(['FLASH', '.code', 'main()'], 30)
        ds_map.insert_label_hierachy(['RAM', '.code', 'foo()'], 100)
        self.assertEqual(ds_map.get_total_size(), 130)

    def test_parsing_generator_three_datasource_names(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )
        list_labels_three = [*LIST_LABELS, Label(name='total', size=350)]
        for label_hiearchy in ds_map.labels():
            self.assertIn(label_hiearchy, list_labels_three)
        self.assertEqual(ds_map.get_total_size(), 350)

    def test_parsing_generator_two_datasource_names(self):
        ds_map = DataSourceMap(['a', 'b'])
        ds_label_list = [
            Label(name='main()', size=30, parents=tuple(['FLASH'])),
            Label(name='foo()', size=100, parents=tuple(['RAM'])),
            Label(name='bar()', size=220, parents=tuple(['RAM'])),
        ]
        for label in ds_label_list:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.name], label.size
            )
        list_labels_two = [*ds_label_list, Label(name='total', size=350)]
        for label_hiearchy in ds_map.labels():
            self.assertIn(label_hiearchy, list_labels_two)
        self.assertEqual(ds_map.get_total_size(), 350)

    def test_parsing_generator_specified_datasource_1(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )
        list_labels_ds_b = [
            Label(name='.code', size=30, parents=tuple(['FLASH'])),
            Label(name='.heap', size=320, parents=tuple(['RAM'])),
        ]
        list_labels_ds_b += [Label(name='total', size=350)]
        for label_hiearchy in ds_map.labels(1):
            self.assertIn(label_hiearchy, list_labels_ds_b)
        self.assertEqual(ds_map.get_total_size(), 350)

    def test_parsing_generator_specified_datasource_str_2(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )
        list_labels_ds_a = [
            Label(name='FLASH', size=30, parents=tuple([])),
            Label(name='RAM', size=320, parents=tuple([])),
        ]
        list_labels_ds_a += [Label(name='total', size=350)]
        for label_hiearchy in ds_map.labels(0):
            self.assertIn(label_hiearchy, list_labels_ds_a)
        self.assertEqual(ds_map.get_total_size(), 350)

    def test_parsing_generator_specified_datasource_int(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )
        list_labels_ds_a = [
            Label(name='FLASH', size=30, parents=tuple([])),
            Label(name='RAM', size=320, parents=tuple([])),
        ]
        list_labels_ds_a += [Label(name='total', size=350)]
        for label_hiearchy in ds_map.labels(0):
            self.assertIn(label_hiearchy, list_labels_ds_a)
        self.assertEqual(ds_map.get_total_size(), 350)

    def test_parsing_generator_specified_datasource_int_2(self):
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )
        list_labels_ds_b = [
            Label(name='.code', size=30, parents=tuple(['FLASH'])),
            Label(name='.heap', size=320, parents=tuple(['RAM'])),
        ]
        list_labels_ds_b += [Label(name='total', size=350)]
        for label_hiearchy in ds_map.labels(1):
            self.assertIn(label_hiearchy, list_labels_ds_b)
        self.assertEqual(ds_map.get_total_size(), 350)

    def test_diff_same_ds_labels_diff_sizes(self):
        """Same map with different sizes."""
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )

        ds_map2 = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map2.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name],
                label.size + 10,
            )

        list_labels_ds_b = [
            Label(
                name='main()',
                size=-10,
                exists_both=True,
                parents=tuple(['FLASH', '.code']),
            ),
            Label(
                name='foo()',
                size=-10,
                exists_both=True,
                parents=tuple(['RAM', '.heap']),
            ),
            Label(
                name='bar()',
                size=-10,
                exists_both=True,
                parents=tuple(['RAM', '.heap']),
            ),
        ]

        ds_map_diff = ds_map.diff(ds_map2)

        for label_hiearchy in ds_map_diff.labels():
            self.assertIn(label_hiearchy, list_labels_ds_b)

    def test_diff_missing_ds_labels_diff_sizes(self):
        """Different map with different sizes."""
        ds_map = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS:
            ds_map.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name], label.size
            )

        ds_map2 = DataSourceMap(['a', 'b', 'c'])
        for label in LIST_LABELS[:-1]:
            ds_map2.insert_label_hierachy(
                [label.parents[0], label.parents[1], label.name],
                label.size + 20,
            )
        ds_map2.insert_label_hierachy(
            [label.parents[0], label.parents[1], 'foobar()'], label.size + 20
        )

        ds_map2.insert_label_hierachy(["LOAD #5", 'random_load', 'func()'], 250)

        list_labels_ds_b = [
            Label(
                name='FLASH',
                size=20,
                capacity=None,
                exists_both=True,
                parents=(),
            ),
            Label(
                name='RAM',
                size=-80,
                capacity=None,
                exists_both=True,
                parents=(),
            ),
            Label(
                name='LOAD #5',
                size=250,
                capacity=None,
                exists_both=False,
                parents=(),
            ),
        ]

        ds_map_diff = ds_map2.diff(ds_map)

        for label_hiearchy in ds_map_diff.labels(0):
            self.assertIn(label_hiearchy, list_labels_ds_b)


if __name__ == '__main__':
    unittest.main()
