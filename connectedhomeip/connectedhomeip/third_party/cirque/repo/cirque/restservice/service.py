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

import os
import atexit

from flask import Flask
from flask import jsonify
from flask import request
from flask import Response

from cirque.common.cirquelog import CirqueLog
from cirque.home.home import CirqueHome

app = Flask(__name__)
CirqueLog.setup_cirque_logger()
logger = CirqueLog.get_cirque_logger('service')

service_mode = os.environ.get('CIRQUE_DEBUG', 0)

homes = {}


@app.route('/create_home', methods=['POST'])
def create_home():
  home = CirqueHome()
  homes[home.home_id] = home
  return jsonify(home.create_home(request.json))


@app.route('/get_homes', methods=['GET'])
def get_homes():
  return jsonify(list(homes.keys()))


@app.route('/wifi_ssid_psk/<home_id>', methods=['GET'])
def get_wifi_ssid_psk(home_id):
  return jsonify(homes[home_id].get_wifiap_ssid_psk())


@app.route('/home_devices/<home_id>', methods=['GET'])
def get_home_devices(home_id):
  if home_id not in homes:
    return ''
  return jsonify(homes[home_id].get_home_devices())


@app.route('/device_state/<home_id>/<device_id>', methods=['GET'])
def get_device_state(home_id, device_id):
  if home_id not in homes:
    return ''
  return jsonify(homes[home_id].get_device_state(device_id))


@app.route('/device_cmd/<home_id>/<device_id>/<path:cmd>', methods=['GET'])
def execute_device_cmd(home_id, device_id, cmd):
  if home_id not in homes:
    return ''
  stream = request.args.get('stream')
  ret = homes[home_id].execute_device_cmd(cmd, device_id, stream == 'True')
  return Response(
      ret.output, mimetype='text/plain') if stream == 'True' else jsonify(
          dict({
              'return_code': str(ret.exit_code),
              'output': str(ret.output.decode())
          }))


@app.route('/stop_device/<home_id>/<device_id>', methods=['GET'])
def stop_device(home_id, device_id):
  if home_id not in homes:
    return ''
  return jsonify(homes[home_id].stop_device(device_id))


@app.route('/device_log/<home_id>/<device_id>', methods=['GET'])
def device_log(home_id, device_id):
  tail = request.args.get('tail', None)
  if tail is not None and tail.isdigit():
    tail = int(tail)
  else:
    tail = 'all'

  if home_id not in homes:
    return ''
  return homes[home_id].get_device_log(device_id, tail)


@app.route('/destroy_home/<home_id>', methods=['GET'])
def destroy_home(home_id):
  if home_id not in homes:
    return ''
  reply = jsonify(homes[home_id].destroy_home())
  del homes[home_id]
  return reply


@app.route('/')
def destroy_homes():
  global homes
  logger.info('removing all the homes..')
  temp_homes = list(homes.keys())
  for home_id in temp_homes:
    homes[home_id].destroy_home()
    del homes[home_id]
  del homes
  return ''


# becareful not to remove this part
if service_mode:
  atexit.register(destroy_homes)

if __name__ == '__main__':
  app.run()
