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

from concurrent import futures

import sys
import atexit
import socket

import grpc
from grpc_status import rpc_status
from google.rpc import code_pb2, status_pb2

import google.protobuf.empty_pb2 as empty_pb2
import cirque.proto.capability_pb2 as capability_pb2
import cirque.proto.device_pb2 as device_pb2
import cirque.proto.service_pb2 as service_pb2
import cirque.proto.service_pb2_grpc as service_pb2_grpc

from cirque.common.cirquelog import CirqueLog
from cirque.home.home import CirqueHome

logger = None


def convert_to_device_pb(device):

  describs = dict()
  for describ in ('ipv4_addr', 'ssid', 'psk'):
    if describ in device['description']:
      describs[describ] = device['description'][describ]

  device_pb = device_pb2.CirqueDevice(
      device_specification=device_pb2.DeviceSpecification(
          device_type=device['type'],
          base_image=device['base_image'],
      ),
      device_id=device['id'],
      device_name=device['name'],
      device_description=device_pb2.DeviceDescription(**describs))
  if 'Weave' in device['capability']:
    device_pb.device_specification.weave_capability.CopyFrom(
        convert_weave_capability_to_pb(device['capability']['Weave']))
  if 'Thread' in device['capability']:
    device_pb.device_specification.thread_capability.CopyFrom(
        convert_thread_capability_to_pb(device['capability']['Thread']))
  if 'WiFi' in device['capability']:
    device_pb.device_specification.wifi_capability.CopyFrom(
        convert_wifi_capability_to_pb(device['capability']['WiFi']))
  if 'Xvnc' in device['capability']:
    device_pb.device_specification.xvnc_capability.CopyFrom(
        convert_xvnc_capability_to_pb(device['capability']['Xvnc']))
  if 'Interactive' in device['capability']:
    device_pb.device_specification.interactive_capability.CopyFrom(
        convert_interactive_capability_to_pb(
            device['capability']['Interactive']))
  if 'LanAccess' in device['capability']:
    device_pb.device_specification.lan_access_capability.CopyFrom(
        convert_lan_access_capability_to_pb(device['capability']['LanAccess']))
  if 'Mount' in device['capability']:
    device_pb.device_specification.mount_capability.CopyFrom(
        convert_mount_capability_to_pb(device['capability']['Mount']))
  if 'TrafficControl' in device['capability']:
    device_pb.device_specification.trafficcontrol_capability.CopyFrom(
        convert_trafficcontrol_capability_to_pb(
            device['capability']['TrafficControl']))

  return device_pb


def add_weave_capability_to_config(device_config, weave_capability):
  device_config['capability'].append('Weave')
  device_config['weave_config_file'] = \
      weave_capability.weave_certificate_path
  device_config['weave_config_target_path'] = \
      weave_capability.target_path


def convert_weave_capability_to_pb(capability_description):
  capability_pb = capability_pb2.WeaveCapability()
  capability_pb.weave_certificate_path = capability_description['config']
  capability_pb.target_path = capability_description['target_path']
  return capability_pb


def add_thread_capability_to_config(device_config, thread_capability):
  device_config['capability'].append('Thread')
  device_config['thread_petition'] = thread_capability.petition_id
  device_config['rcp_mode'] = thread_capability.rcp_mode
  if thread_capability.daemons:
    device_config['daemons'] = thread_capability.daemons


def convert_thread_capability_to_pb(capability_description):
  # TODO: detailed Thread capability info
  capability_pb = capability_pb2.ThreadCapability()
  return capability_pb


def add_wifi_capability_to_config(device_config, wifi_capability):
  device_config['capability'].append('WiFi')


def convert_wifi_capability_to_pb(capability_description):
  capability_pb = capability_pb2.WiFiCapability()
  return capability_pb


def add_xvnc_capability_to_config(device_config, xvnc_capability):
  device_config['capability'].append('Xvnc')
  device_config['xvnc_localhost'] = xvnc_capability.localhost
  device_config['docker_display_id'] = xvnc_capability.docker_display_id
  if xvnc_capability.display_id != 0:
    device_config['display_id'] = xvnc_capability.display_id


def convert_xvnc_capability_to_pb(capability_description):
  capability_pb = capability_pb2.XvncCapability()
  capability_pb.localhost = capability_description['localhost']
  capability_pb.display_id = capability_description['display_id']
  return capability_pb


def add_trafficcontrol_capability_to_config(device_config,
                                            trafficcontrol_capability):
  device_config['capability'].append('TrafficControl')
  device_config['traffic_control'] = {
      'latencyMs': trafficcontrol_capability.latency_ms,
      'loss': trafficcontrol_capability.loss_rate
  }


def convert_trafficcontrol_capability_to_pb(capability_description):
  capability_pb = capability_pb2.TrafficControlCapability()
  capability_pb.latency_ms = capability_description['latencyMs']
  capability_pb.loss_rate = capability_description['loss']
  return capability_pb


def add_interactive_capability_to_config(device_config, interactive_capability):
  device_config['capability'].append('Interactive')


def convert_interactive_capability_to_pb(capability_description):
  capability_pb = capability_pb2.InteractiveCapability()
  return capability_pb


def add_lan_access_capability_to_config(device_config, lan_access_capability):
  device_config['capability'].append('LanAccess')
  if lan_access_capability.internal_lan:
    device_config['docker_network'] = 'internal'


def convert_lan_access_capability_to_pb(capability_description):
  capability_pb = capability_pb2.LanAccessCapability()
  return capability_pb


def add_mount_capability_to_config(device_config, mount_capability):
  device_config['capability'].append('Mount')
  mount_pairs = [
      (p.host_path, p.target_path) for p in mount_capability.mount_pairs
  ]
  device_config['mount_pairs'] = mount_pairs


def convert_mount_capability_to_pb(capability_description):
  mount_pb = capability_pb2.MountCapability()
  for host_path, target_path in capability_description['mount_pairs']:
    mount_pb.mount_pairs.append(
        capability_pb2.MountPair(host_path=host_path, target_path=target_path))
  return mount_pb


cirque_service = None


def __exit_handler():
  global cirque_service
  for home in cirque_service.homes.values():
    home.destroy_home()


class CirqueService(service_pb2_grpc.CirqueServiceServicer):

  def __init__(self):
    self.homes = dict()
    self.__next_home_id = 0

  def CreateCirqueHome(self, _, context):
    home = CirqueHome(str(self.__next_home_id))
    self.__next_home_id += 1
    self.homes[home.home_id] = home
    return service_pb2.CreateCirqueHomeResponse(home_id=home.home_id)

  def CreateCirqueDevice(self, request, context):
    if request.home_id is None or request.home_id not in self.homes:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
      return service_pb2.CreateCirqueDeviceResponse()
    elif request.specification is None:
      context.abort_with_status(
          rpc_status.to_status(
              status_pb2.Status(code=code_pb2.INVALID_ARGUMENT)))
      return service_pb2.CreateCirqueDeviceResponse()
    else:
      specification = request.specification
      device_config = {
          'type': specification.device_type,
          'capability': [],
      }
      if specification.base_image:
        device_config['base_image'] = specification.base_image
      if specification.WhichOneof('optional_weave_capability'):
        add_weave_capability_to_config(device_config,
                                       specification.weave_capability)
      if specification.WhichOneof('optional_thread_capability'):
        add_thread_capability_to_config(device_config,
                                        specification.thread_capability)
      if specification.WhichOneof('optional_wifi_capability'):
        add_wifi_capability_to_config(device_config,
                                      specification.wifi_capability)
      if specification.WhichOneof('optional_xvnc_capability'):
        add_xvnc_capability_to_config(device_config,
                                      specification.xvnc_capability)
      if specification.WhichOneof('optional_interactive_capability'):
        add_interactive_capability_to_config(
            device_config, specification.interactive_capability)
      if specification.WhichOneof('optional_lan_access_capability'):
        add_lan_access_capability_to_config(device_config,
                                            specification.lan_access_capability)
      if specification.WhichOneof('optional_mount_capability'):
        add_mount_capability_to_config(device_config,
                                       specification.mount_capability)
      if specification.WhichOneof('optional_trafficcontrol_capability'):
        add_trafficcontrol_capability_to_config(
            device_config, specification.trafficcontrol_capability)

      device_id = self.homes[request.home_id].add_device(device_config)
      if device_id is None:
        context.abort_with_status(
            rpc_status.to_status(status_pb2.Status(code=code_pb2.INTERNAL)))
        return service_pb2.CreateCirqueDeviceResponse()
      else:
        device = self.homes[request.home_id].get_device_state(device_id)
        return service_pb2.CreateCirqueDeviceResponse(
            device=convert_to_device_pb(device))

  def ExecuteDeviceCommand(self, request, context):
    if request.home_id is None or \
       request.home_id not in self.homes or \
       request.device_id is None or \
       request.device_id not in self.homes[request.home_id].devices:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
      return service_pb2.ExecuteDeviceCommandResponse()

    ret = self.homes[request.home_id].execute_device_cmd(
        cmd=request.command,
        node_id=request.device_id,
        stream=request.streaming)

    if ret.exit_code != 0:
      context.abort_with_status(
          rpc_status.to_status(
              status_pb2.Status(code=code_pb2.ABORTED, message=ret.output)))
      return service_pb2.ExecuteDeviceCommandResponse()

    return service_pb2.ExecuteDeviceCommandResponse(output=ret.output)

  def ListCirqueHomes(self, request, context):
    return service_pb2.ListCirqueHomesResponse(home_id=[*self.homes])

  def ListCirqueHomeDevices(self, request, context):
    if request.home_id is None or request.home_id not in self.homes:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
      return service_pb2.ListCirqueHomeDevicesResponse()
    devices = self.homes[request.home_id].get_home_devices().values()
    device_pbs = [convert_to_device_pb(d) for d in devices]
    return service_pb2.ListCirqueHomeDevicesResponse(devices=device_pbs)

  def QueryCirqueDevice(self, request, context):
    if request.home_id is None or \
       request.home_id not in self.homes or \
       request.device_id is None or \
       request.device_id not in self.homes[request.home_id].devices:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
      return service_pb2.QueryCirqueDeviceResponse()
    device = self.homes[request.home_id].get_device_state(request.device_id)
    return service_pb2.QueryCirqueDeviceResponse(
        device=convert_to_device_pb(device))

  def StopCirqueDevice(self, request, context):
    if request.home_id is None or \
       request.home_id not in self.homes or \
       request.device_id is None or \
       request.device_id not in self.homes[request.home_id].devices:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
    else:
      self.homes[request.home_id].stop_device(request.device_id)
    return empty_pb2.Empty()

  def StopCirqueHome(self, request, context):
    if request.home_id is None or \
       request.home_id not in self.homes:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
    else:
      self.homes[request.home_id].destroy_home()
      del self.homes[request.home_id]
    return empty_pb2.Empty()

  def GetCirqueDeviceLog(self, request, context):
    if request.home_id is None or \
       request.home_id not in self.homes or \
       request.device_id is None or \
       request.device_id not in self.homes[request.home_id].devices:
      context.abort_with_status(
          rpc_status.to_status(status_pb2.Status(code=code_pb2.NOT_FOUND)))
      return service_pb2.GetCirqueDeviceLogResponse()
    tail = request.tail if request.tail is not None else 'all'
    log = self.homes[request.home_id].get_device_log(
        request.device_id, tail=tail)
    return service_pb2.GetCirqueDeviceLogResponse(log=log)


def serve(service_port=50051):
  global cirque_service
  cirque_service = CirqueService()
  atexit.register(__exit_handler)
  server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
  service_pb2_grpc.add_CirqueServiceServicer_to_server(cirque_service, server)
  logger.info('running grpc service on port: {}'.format(service_port))
  server.add_insecure_port('[::]:{}'.format(service_port))
  server.start()
  server.wait_for_termination()


def main(service_port):
  global logger
  CirqueLog.setup_cirque_logger()
  logger = CirqueLog.get_cirque_logger('grpc_service')
  serve(service_port)


if __name__ == '__main__':
  service_port = 50051
  if len(sys.argv) > 1 and sys.argv[1].isdigit():
    service_port = int(sys.argv[1])
    if service_port < 49151:
      raise PermissionError('please use port range in (49152 ~ 65535)')
  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    try:
      s.bind(('', service_port))
    except (OSError, PermissionError) as err:
      raise RuntimeError('{}:{!r}'.format(service_port, err))
  main(service_port)
