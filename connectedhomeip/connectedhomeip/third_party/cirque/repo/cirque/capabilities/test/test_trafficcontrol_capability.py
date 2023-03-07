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

import logging
import unittest
import re
import docker

import cirque.common.utils as utils

from cirque.common.cirquelog import CirqueLog
from cirque.capabilities.trafficcontrolcapability \
    import TrafficControlCapability
from cirque.nodes.dockernode import DockerNode
from cirque.nodes.wifiapnode import WiFiAPNode


class TestTrafficControl(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    CirqueLog.setup_cirque_logger(level=logging.DEBUG)
    docker_client = docker.from_env()

    cls.trafficcontrol_capability = TrafficControlCapability(
        latencyMs=200, loss=25)

    cls.node1 = DockerNode(
        docker_client,
        'generic_node_image',
        capabilities=[cls.trafficcontrol_capability])
    cls.node2 = DockerNode(docker_client, 'generic_node_image')
    kwargs = {'stdin_open': True, 'privileged': True}
    cls.node1.run(**kwargs)
    cls.node2.run(**kwargs)

  @classmethod
  def tearDownClass(cls):
    print('tearing down tests...')
    cls.node1.stop()
    cls.node2.stop()

  def test_001_ping(self):
    ret = self.node1.container.exec_run('ping -q -c 25 {}'.format(
        self.node2.description['ipv4_addr']))
    print('Ping command output: {}'.format(ret.output.decode()))
    self.assertEqual(ret.exit_code, 0)
    received = float(
        re.findall(rb'\s+(\d+)\sreceived', ret.output)[0].decode('utf-8'))
    print('Packets lost: {}/15'.format(25 - received))
    # The probablity that loss 0-1 packet or 13+ packet is ~1%
    self.assertTrue(received > 12 and received < 24)
    latency = float(
        re.findall(rb'\d+\.\d+/(\d+\.\d+)/\d+\.\d+/\d+\.\d+',
                   ret.output)[0].decode('utf-8'))
    print('Avg. Lantency: {}'.format(latency))
    self.assertTrue(latency > 199 and latency < 220)


if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestTrafficControl)
  unittest.TextTestRunner(verbosity=2).run(suite)
