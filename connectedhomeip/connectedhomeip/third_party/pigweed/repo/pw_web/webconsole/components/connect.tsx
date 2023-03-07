// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

import Button from '@mui/material/Button';
import {Alert} from '@mui/material';
import {Device, WebSerial} from "pigweedjs";
import {useState} from 'react';
import DeviceFactory from "../common/device";
type WebSerialTransport = WebSerial.WebSerialTransport

interface LogProps {
  onConnection: (device: Device) => void
}

export default function BtnConnect({onConnection}: LogProps) {
  const [connected, setConnected] = useState(false);
  if (connected) return (<Alert severity="success">Connected!</Alert>)
  return (<Button onClick={async () => {
    const device = await DeviceFactory();
    await device.connect();
    setConnected(true);
    onConnection(device);
  }} variant="contained">Connect</Button>)
}
