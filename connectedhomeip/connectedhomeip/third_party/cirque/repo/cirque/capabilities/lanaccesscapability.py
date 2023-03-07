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
from cirque.common.utils import manipulate_iptable_src_dst_rule


class LanAccessCapability(BaseCapability):

  def __init__(self, home_lan):
    self.__home_lan = home_lan

  @property
  def name(self):
    return 'LanAccess'

  def enable_capability(self, docker_node):
    node_address = docker_node.description['ipv4_addr']
    subnet = self.__home_lan.subnet
    manipulate_iptable_src_dst_rule(self.__home_lan.logger, node_address,
                                    subnet, 'ACCEPT')
    manipulate_iptable_src_dst_rule(self.__home_lan.logger, subnet,
                                    node_address, 'ACCEPT')

  def disable_capability(self, docker_node):
    node_address = docker_node.description['ipv4_addr']
    subnet = self.__home_lan.subnet
    manipulate_iptable_src_dst_rule(
        self.__home_lan.logger, node_address, subnet, 'ACCEPT', add=False)
    manipulate_iptable_src_dst_rule(
        self.__home_lan.logger, subnet, node_address, 'ACCEPT', add=False)
