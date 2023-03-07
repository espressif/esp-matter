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

import {CompletionContext} from '@codemirror/autocomplete'
import {syntaxTree} from '@codemirror/language'
import {Device} from "pigweedjs";

const completePropertyAfter = ['PropertyName', '.', '?.']
const dontCompleteIn = [
  'TemplateString',
  'LineComment',
  'BlockComment',
  'VariableDefinition',
  'PropertyDefinition'
]
var objectPath = require("object-path");

export function completeFromGlobalScope(context: CompletionContext) {
  let nodeBefore = syntaxTree(context.state).resolveInner(context.pos, -1)

  if (
    completePropertyAfter.includes(nodeBefore.name) &&
    nodeBefore.parent?.name == 'MemberExpression'
  ) {
    let object = nodeBefore.parent.getChild('Expression')
    if (object?.name == 'VariableName') {
      let from = /\./.test(nodeBefore.name) ? nodeBefore.to : nodeBefore.from
      let variableName = context.state.sliceDoc(object.from, object.to)
      // @ts-ignore
      if (typeof window[variableName] == 'object') {
        // @ts-ignore
        return completeProperties(from, window[variableName])
      }
    }
    else if (object?.name == 'MemberExpression') {
      let from = /\./.test(nodeBefore.name) ? nodeBefore.to : nodeBefore.from
      let variableName = context.state.sliceDoc(object.from, object.to)
      let variable = resolveWindowVariable(variableName);
      // @ts-ignore
      if (typeof variable == 'object') {
        // @ts-ignore
        return completeProperties(from, variable, variableName)
      }
    }
  } else if (nodeBefore.name == 'VariableName') {
    return completeProperties(nodeBefore.from, window)
  } else if (context.explicit && !dontCompleteIn.includes(nodeBefore.name)) {
    return completeProperties(context.pos, window)
  }
  return null
}

function completeProperties(from: number, object: Object, variableName?: string) {
  let options = []
  for (let name in object) {
    // @ts-ignore
    if (object[name] instanceof Function && variableName) {
      debugger;
      options.push({
        label: name,
        // @ts-ignore
        detail: getFunctionDetailText(`${variableName}.${name}`),
        type: 'function'
      })
    }
    else {
      options.push({
        label: name,
        type: 'variable'
      })
    }

  }
  return {
    from,
    options,
    validFor: /^[\w$]*$/
  }
}

function resolveWindowVariable(variableName: string) {
  if (objectPath.has(window, variableName)) {
    return objectPath.get(window, variableName);
  }
}

function getFunctionDetailText(fullExpression: string): string {
  if (fullExpression.startsWith("device.rpcs.")) {
    fullExpression = fullExpression.replace("device.rpcs.", "");
  }
  const args = ((window as any).device as Device).getMethodArguments(fullExpression);
  if (args) {
    return `(${args.join(", ")})`;
  }
  return "";
}
