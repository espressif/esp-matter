# Copyright 2020 Google LLC
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

from cirque.capabilities.basecapability import BaseCapability


class MountCapability(BaseCapability):

  def __init__(self, mount_pairs):
    self.mount_pairs = mount_pairs

  @property
  def name(self):
    return 'Mount'

  def get_docker_run_args(self, docker_node):
    return {
        'volumes': [
            '{}:{}'.format(host_path, target_path)
            for host_path, target_path in self.mount_pairs
        ],
        'privileged': True,
    }

  @property
  def description(self):
    return {
        'mount_pairs': self.mount_pairs,
    }
