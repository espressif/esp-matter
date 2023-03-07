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
from cirque.connectivity.threadsimpipe import ThreadSimPipe
from cirque.common.cirquelog import CirqueLog


class ThreadCapability(BaseCapability):

  def __init__(self,
               node_id,
               petition_id,
               daemons=['wpantund', 'otbr-agent'],
               rcp=False):
    self.thread_endpoint = ThreadSimPipe(node_id, petition_id, rcp)
    self.thread_endpoint.open()
    self.logger = CirqueLog.get_cirque_logger(self.__class__.__name__)
    self.daemons = daemons
    for daemon in daemons:
      if daemon not in {'wpantund', 'otbr-agent'}:
        self.logger.warning(
            'using unkown thread daemon mode: {}'.format(daemon))

  @property
  def name(self):
    return 'Thread'

  def get_docker_run_args(self, dockernode):
    return {
        'devices': [
            '{}:/dev/ttyUSB0'.format(self.thread_endpoint.pipe_path_for_user)
        ],
        'volumes': [
            '{}:/dev/ttyUSB0'.format(self.thread_endpoint.pipe_path_for_user)
        ],
        'sysctls': {
            'net.ipv6.conf.all.disable_ipv6': 0,
            'net.ipv4.conf.all.forwarding': 1,
            'net.ipv6.conf.all.forwarding': 1,
        },
        'privileged': True,
        'tty': True,
    }

  def disable_capability(self, docker_node):
    for daemon in self.daemons:
      docker_node.container.exec_run('killall {}'.format(daemon))
    self.thread_endpoint.close()
    self.thread_endpoint = None
