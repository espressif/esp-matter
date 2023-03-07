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

import {useEffect, useState} from "react";
import {Device} from "pigweedjs";
import {EditorView} from "codemirror"
import {basicSetup} from "./basicSetup";
import {javascript, javascriptLanguage} from "@codemirror/lang-javascript"
import {placeholder} from "@codemirror/view";
import {oneDark} from "@codemirror/theme-one-dark";
import {keymap} from "@codemirror/view"
import {Extension} from "@codemirror/state"
import {completeFromGlobalScope} from "./autocomplete";
import LocalStorageArray from "./localStorageArray";
import "xterm/css/xterm.css";
import styles from "../../styles/repl.module.css";

const isSSR = () => typeof window === 'undefined';

interface ReplProps {
  device: Device | undefined
}

const globalJavaScriptCompletions = javascriptLanguage.data.of({
  autocomplete: completeFromGlobalScope
})

const createTerminal = async (container: HTMLElement) => {
  const {Terminal} = await import('xterm');
  const {FitAddon} = await import('xterm-addon-fit');
  const terminal = new Terminal({
    // cursorBlink: true,
    theme: {
      background: '#2c313a'
    }
  });
  terminal.open(container);

  const fitAddon = new FitAddon();
  terminal.loadAddon(fitAddon);
  fitAddon.fit();
  return terminal;
};

const createPlaceholderText = () => {
  var div = document.createElement('div');
  div.innerHTML = `Type code and hit Enter to run. See <b>[?]</b> for more info.`
  return div;
}

const createEditor = (container: HTMLElement, enterKeyMap: Extension) => {
  let view = new EditorView({
    extensions: [basicSetup, javascript(), placeholder(createPlaceholderText()), oneDark, globalJavaScriptCompletions, enterKeyMap],
    parent: container,
  });
  return view;
}

let currentCommandHistoryIndex = -1;
let historyStorage: LocalStorageArray;
if (typeof window !== 'undefined') {
  historyStorage = new LocalStorageArray();
}

export default function Repl({device}: ReplProps) {
  const [terminal, setTerminal] = useState<any>(null);
  const [codeEditor, setCodeEditor] = useState<EditorView | null>(null);

  useEffect(() => {
    let cleanupFns: {(): void; (): void;}[] = [];
    if (!terminal && !isSSR() && device) {
      const futureTerm = createTerminal(document.querySelector('#repl-log-container')!);
      futureTerm.then(async (term) => {
        cleanupFns.push(() => {
          term.dispose();
          setTerminal(null);
        });
        setTerminal(term);
      });

      return () => {
        cleanupFns.forEach(fn => fn());
      }
    }
    else if (terminal && !device) {
      terminal.dispose();
      setTerminal(null);
    }
  }, [device]);

  useEffect(() => {
    if (!terminal) return;
    const enterKeyMap = {
      key: "Enter",
      run(view: EditorView) {
        if (view.state.doc.toString().trim().length === 0) return true;
        try {
          // To run eval() in global scope, we do (1, eval) here.
          const cmdOutput = (1, eval)(view.state.doc.toString());
          // Check if eval returned a promise
          if (typeof cmdOutput === "object" && cmdOutput.then !== undefined) {
            cmdOutput
              .then((result: any) => {
                terminal.write(`Promise { ${result} }\r\n`);
              })
              .catch((e: any) => {
                if (e instanceof Error) {
                  terminal.write(`\x1b[31;1mUncaught (in promise) Error: ${e.message}\x1b[0m\r\n`)
                }
                else {
                  terminal.write(`\x1b[31;1mUncaught (in promise) ${e}\x1b[0m\r\n`)
                }
              });
          }
          else {
            terminal.write(cmdOutput + "\r\n");
          }
        }
        catch (e) {
          if (e instanceof Error) terminal.write(`\x1b[31;1m${e.message}\x1b[0m\r\n`)
        }

        currentCommandHistoryIndex = -1;
        historyStorage.unshift(view.state.doc.toString());

        // Clear text editor
        const transaction = view.state.update({changes: {from: 0, to: view.state.doc.length, insert: ""}});
        view.dispatch(transaction);
        return true;
      }
    };

    const upKeyMap = {
      key: "ArrowUp",
      run(view: EditorView) {
        currentCommandHistoryIndex++;
        if (historyStorage.data[currentCommandHistoryIndex]) {
          // set text editor
          const transaction = view.state.update({changes: {from: 0, to: view.state.doc.length, insert: historyStorage.data[currentCommandHistoryIndex]}});
          view.dispatch(transaction);
        }
        else {
          currentCommandHistoryIndex = historyStorage.data.length - 1;
        }
        return true;
      }
    };

    const downKeyMap = {
      key: "ArrowDown",
      run(view: EditorView) {
        currentCommandHistoryIndex--;
        if (currentCommandHistoryIndex <= -1) {
          currentCommandHistoryIndex = -1;
          const transaction = view.state.update({changes: {from: 0, to: view.state.doc.length, insert: ""}});
          view.dispatch(transaction);
        }
        else if (historyStorage.data[currentCommandHistoryIndex]) {
          // set text editor
          const transaction = view.state.update({changes: {from: 0, to: view.state.doc.length, insert: historyStorage.data[currentCommandHistoryIndex]}});
          view.dispatch(transaction);
        }
        return true;
      }
    };

    const keymaps = keymap.of([enterKeyMap, upKeyMap, downKeyMap]);
    let view = createEditor(document.querySelector('#repl-editor-container')!, keymaps);
    return () => view.destroy();
  }, [terminal]);

  return (
    <div className={styles.container}>
      <div id="repl-log-container" className={styles.logs}></div>
      <div className={styles.replWithCaret}>
        <div>
          <div className={styles.tooltip}>?
            <span className={styles.tooltiptext}>
              This REPL runs JavaScript.
              You can navigate previous commands using <span>Up</span> and <span>Down</span> arrow keys.
              <br /><br />
              Call device RPCs using <span>device.rpcs.*</span> API.
            </span>
          </div>
        <span className={styles.caret}>{`> `}</span>
        </div>
        <div id="repl-editor-container" className={styles.editor}></div>
      </div>
    </div>
  )
}
