# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import unittest
from unittest import mock

from cirque.capabilities.xvnccapability import XvncCapability


@mock.patch('subprocess.Popen')
class TestXvncCapability(unittest.TestCase):

  def __check_docker_run_args(self, docker_run_args, display_id,
                              docker_display_id):
    self.assertEqual(docker_run_args['environment'],
                     [f'DISPLAY=:{docker_display_id}'])

    src_path = f'/tmp/.X11-unix/X{display_id}'
    dest_path = f'/tmp/.X11-unix/X{docker_display_id}'
    self.assertEqual(docker_run_args['volumes'], [f'{src_path}:{dest_path}'])

  def test_auto_assigned_display_id(self, popen):
    xvnccapability = XvncCapability(display_id=0)

    docker_run_args = xvnccapability.get_docker_run_args(None)
    display_id = xvnccapability.description['display_id']
    self.__check_docker_run_args(docker_run_args, display_id, 0)

  def test_fixed_display_id(self, popen):
    xvnccapability = XvncCapability(display_id=2)

    docker_run_args = xvnccapability.get_docker_run_args(None)
    self.__check_docker_run_args(docker_run_args, 2, 0)

  def test_fixed_docker_display_id(self, popen):
    xvnccapability = XvncCapability(display_id=42, docker_display_id=13)

    docker_run_args = xvnccapability.get_docker_run_args(None)
    self.__check_docker_run_args(docker_run_args, 42, 13)


if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestXvncCapability)
  unittest.TextTestRunner(verbosity=2).run(suite)
