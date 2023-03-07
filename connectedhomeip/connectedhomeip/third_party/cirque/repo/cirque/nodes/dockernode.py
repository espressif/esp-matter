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

from functools import reduce

import docker
from cirque.common.cirquelog import CirqueLog
from cirque.common.utils import sleep_time


class DockerNode:

  def __init__(self,
               docker_client,
               node_type,
               capabilities=None,
               base_image=None):
    self._client = docker_client
    self.node_type = node_type
    if base_image:
      self.image_name = base_image
    else:
      self.image_name = node_type
    self.container = None
    self.capabilities = [] if capabilities is None else capabilities
    self.logger = CirqueLog.get_cirque_logger(self.__class__.__name__)
    self.logger.info('Capabilites: {}'.format(
        [c.name for c in self.capabilities]))

  def run(self, **kwargs):

    def merge_capapblity_arg(arg0, arg1):
      for key, item in arg1.items():
        if key in arg0:
          self.logger.debug('{}: {} {}'.format(key, arg0[key], item))
          if isinstance(item, list):
            arg0[key] += item
          elif isinstance(item, dict):
            arg0[key].update(item)
          elif key == 'privileged':
            arg0[key] |= item
        else:
          arg0[key] = item
      return arg0

    capability_run_args = [
        capability.get_docker_run_args(self) for capability in self.capabilities
    ]
    capability_run_args = reduce(merge_capapblity_arg, capability_run_args,
                                 {'cap_add': ['SYS_TIME']})
    kwargs.update(capability_run_args)
    self.container = self._client.containers.run(
        self.image_name, detach=True, **kwargs)
    self.logger.info('starting container with image {} args={}'.format(
        self.image_name, kwargs))
    if self.container is None:
      self.logger.error(
          'failed to create container: {}, please check and try again..'.format(
              self.name))
    for capability in self.capabilities:
      capability.enable_capability(self)

  def stop(self):
    if hasattr(self, 'container') and self.container:
      for capability in self.capabilities:
        capability.disable_capability(self)
      self.container.stop(timeout=2)
    self.container = None

  def __del__(self):
    if hasattr(self, 'container') and self.container:
      self.stop()

  @property
  def id(self):
    if self.container is not None:
      return self.container.id
    return None

  @property
  def name(self):
    if self.container is not None:
      return self.container.name
    return None

  @property
  def type(self):
    return self.node_type

  @property
  def base_image(self):
    return self.image_name

  @property
  def description(self):
    inspection = self.inspect()
    network_info = inspection['NetworkSettings']['Networks']
    if network_info:
      network_name = next(iter(network_info.keys()))
      description = {
          'ipv4_addr': network_info[network_name]['IPAddress'],
      }
      if network_info[network_name].get('IPv6Gateway', None):
        description.update({
          'ipv6_addr': network_info[network_name]['GlobalIPv6Address'],
        })
    for capability in self.capabilities:
      description.update(capability.description)
    return description

  def get_container_pid(self):
    if self.container is None:
      return None
    return self.inspect()['State']['Pid']

  def get_device_log(self, tail='all'):
    if self.container is not None:
      return self.container.logs(tail=tail).decode()
    return ''

  def inspect(self):
    return self._client.api.inspect_container(self.container.id)
