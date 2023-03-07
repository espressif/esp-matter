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
"""Serializes an Environment into a JSON file."""

import json

# Disable super() warnings since this file must be Python 2 compatible.
# pylint: disable=super-with-arguments


class JSONVisitor(object):  # pylint: disable=useless-object-inheritance
    """Serializes an Environment into a JSON file."""

    def __init__(self, *args, **kwargs):
        super(JSONVisitor, self).__init__(*args, **kwargs)
        self._data = {}

    def serialize(self, env, outs):
        self._data = {
            'modify': {},
            'set': {},
        }

        env.accept(self)

        json.dump(self._data, outs, indent=4, separators=(',', ': '))
        outs.write('\n')
        self._data = {}

    def visit_set(self, set):  # pylint: disable=redefined-builtin
        self._data['set'][set.name] = set.value

    def visit_clear(self, clear):
        self._data['set'][clear.name] = None

    def _initialize_path_like_variable(self, name):
        default = {'append': [], 'prepend': [], 'remove': []}
        self._data['modify'].setdefault(name, default)

    def visit_remove(self, remove):
        self._initialize_path_like_variable(remove.name)
        self._data['modify'][remove.name]['remove'].append(remove.value)
        if remove.value in self._data['modify'][remove.name]['append']:
            self._data['modify'][remove.name]['append'].remove(remove.value)
        if remove.value in self._data['modify'][remove.name]['prepend']:
            self._data['modify'][remove.name]['prepend'].remove(remove.value)

    def visit_prepend(self, prepend):
        self._initialize_path_like_variable(prepend.name)
        self._data['modify'][prepend.name]['prepend'].append(prepend.value)
        if prepend.value in self._data['modify'][prepend.name]['remove']:
            self._data['modify'][prepend.name]['remove'].remove(prepend.value)

    def visit_append(self, append):
        self._initialize_path_like_variable(append.name)
        self._data['modify'][append.name]['append'].append(append.value)
        if append.value in self._data['modify'][append.name]['remove']:
            self._data['modify'][append.name]['remove'].remove(append.value)

    def visit_echo(self, echo):
        pass

    def visit_comment(self, comment):
        pass

    def visit_command(self, command):
        pass

    def visit_doctor(self, doctor):
        pass

    def visit_blank_line(self, blank_line):
        pass

    def visit_function(self, function):
        pass

    def visit_hash(self, hash):  # pylint: disable=redefined-builtin
        pass
