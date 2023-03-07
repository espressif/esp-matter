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


class WeaveCapability(BaseCapability):

  def __init__(self, cert_path, cert_target_path=None, daemon='nldaemon'):
    self.cert_path = cert_path
    if cert_target_path:
      self.cert_target_path = cert_target_path
    else:
      self.cert_target_path = \
          '/mnt/stateful_partition/nestlabs/device_provisioning.config'
    self.daemon = daemon

  @property
  def name(self):
    return 'Weave'

  def get_docker_run_args(self, docker_node):
    return {
        'volumes': ['{}:{}'.format(self.cert_path, self.cert_target_path)],
        'sysctls': {
            'net.ipv6.conf.all.disable_ipv6': 0,
            'net.ipv4.conf.all.forwarding': 1,
            'net.ipv6.conf.all.forwarding': 1,
        },
        'privileged': True,
    }

  def disable_capability(self, docker_node):
    docker_node.container.exec_run('killall {}'.format(self.daemon))

  @property
  def description(self):
    return {
        'config': self.cert_path,
        'target_path': self.cert_target_path,
    }
