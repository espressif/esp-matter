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

import os
import glob
import subprocess
from threading import Lock

from cirque.capabilities.basecapability import BaseCapability


class TrafficControlCapability(BaseCapability):

  def __init__(self, latencyMs=0, loss=0):
    self.latencyMs = latencyMs
    self.loss = loss

  @property
  def name(self):
    return "TrafficControl"

  @property
  def description(self):
    return {
        "latency": self.latencyMs,
        "loss": self.loss,
    }

  def get_docker_run_args(self, dockernode):
    return {"cap_add": ["NET_ADMIN"]}

  def enable_capability(self, docker_node):
    command = []
    if self.latencyMs > 0:
      command += ["delay", "{}ms".format(self.latencyMs)]
    if self.loss > 0:
      command += ["loss", "{}%".format(self.loss)]
    if command:
      docker_node.container.exec_run(
          " ".join(["tc", "qdisc", "add", "dev", "eth0", "root", "netem"] +
                   command))

  def disable_capability(self, docker_node):
    pass
