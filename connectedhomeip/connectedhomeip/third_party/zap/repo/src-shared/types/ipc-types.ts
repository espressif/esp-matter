/**
 *
 *    Copyright (c) 2020 Silicon Labs
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

import ipc from 'node-ipc'
import * as db from './db-types'

export interface Server {
  ipc: InstanceType<typeof ipc.IPC>
  serverStarted: boolean
}

export interface IpcEventHandlerContext {
  db: db.DbType | null
  socket?: any
  httpPort: number
}

export interface Client {
  ipc: InstanceType<typeof ipc.IPC>
  uuid: string
  connected: boolean
}
