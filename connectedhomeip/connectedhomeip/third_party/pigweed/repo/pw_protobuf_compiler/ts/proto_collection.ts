// Copyright 2021 The Pigweed Authors
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

/** Tools for compiling and importing Javascript protos on the fly. */

import {Message} from 'google-protobuf';
import {DescriptorProto, FileDescriptorSet} from 'google-protobuf/google/protobuf/descriptor_pb';

export type MessageCreator = new () => Message;
class MessageMap extends Map<string, MessageCreator> {}
class MessageDescriptorMap extends Map<string, DescriptorProto> { }
export class ModuleMap extends Map<string, any> {}

/**
 * A wrapper class of protocol buffer modules to provide convenience methods.
 */
export class ProtoCollection {
  private messages: MessageMap = new MessageMap();
  private messageDescriptors: MessageDescriptorMap = new MessageDescriptorMap();

  constructor(
    readonly fileDescriptorSet: FileDescriptorSet,
    modules: ModuleMap
  ) {
    this.mapMessages(fileDescriptorSet, modules);
  }

  /**
   * Creates a map between message identifier "{packageName}.{messageName}"
   * and the Message class and also the associated DescriptorProto.
   */
  private mapMessages(set: FileDescriptorSet, mods: ModuleMap): void {
    for (const fileDescriptor of set.getFileList()) {
      const mod = mods.get(fileDescriptor.getName()!)!;
      for (const messageType of fileDescriptor.getMessageTypeList()) {
        const fullName =
          fileDescriptor.getPackage()! + '.' + messageType.getName();
        const message = mod[messageType.getName()!];
        this.messages.set(fullName, message);
        this.messageDescriptors.set(fullName, messageType);
      }
    }
  }

  /**
   * Finds the Message class referenced by the identifier.
   *
   *  @param identifier String identifier of the form
   *  "{packageName}.{messageName}" i.e: "pw.rpc.test.NewMessage".
   */
  getMessageCreator(identifier: string): MessageCreator | undefined {
    return this.messages.get(identifier);
  }

  /**
   * Finds the DescriptorProto referenced by the identifier.
   *
   *  @param identifier String identifier of the form
   *  "{packageName}.{messageName}" i.e: "pw.rpc.test.NewMessage".
   */
  getDescriptorProto(identifier: string): DescriptorProto | undefined {
    return this.messageDescriptors.get(identifier);
  }
}
