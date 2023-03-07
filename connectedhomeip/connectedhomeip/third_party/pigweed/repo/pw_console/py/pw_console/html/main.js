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

var VirtualizedList = window.VirtualizedList.default;
const rowHeight = 30;

function formatDate(dt) {
  function pad2(n) {
    return (n < 10 ? '0' : '') + n;
  }

  return dt.getFullYear() + pad2(dt.getMonth() + 1) + pad2(dt.getDate()) + ' ' +
      pad2(dt.getHours()) + ':' + pad2(dt.getMinutes()) + ':' +
      pad2(dt.getSeconds());
}

let data = [];
function clearLogs() {
  data = [{
    'message': 'Logs started',
    'levelno': 20,
    time: formatDate(new Date()),
    'levelname': '\u001b[35m\u001b[1mINF\u001b[0m',
    'args': [],
    'fields': {'module': '', 'file': '', 'timestamp': '', 'keys': ''}
  }];
}
clearLogs();

let nonAdditionalDataFields =
    ['_hosttime', 'levelname', 'levelno', 'args', 'fields', 'message', 'time'];
let additionalHeaders = [];
function updateHeadersFromData(data) {
  let dirty = false;
  Object.keys(data).forEach((columnName) => {
    if (nonAdditionalDataFields.indexOf(columnName) === -1 &&
        additionalHeaders.indexOf(columnName) === -1) {
      additionalHeaders.push(columnName);
      dirty = true;
    }
  });
  Object.keys(data.fields || {}).forEach((columnName) => {
    if (nonAdditionalDataFields.indexOf(columnName) === -1 &&
        additionalHeaders.indexOf(columnName) === -1) {
      additionalHeaders.push(columnName);
      dirty = true;
    }
  });

  const headerDOM = document.querySelector('.log-header');
  if (dirty) {
    headerDOM.innerHTML = `
      <span class="_hosttime">Time</span>
      <span class="level">Level</span>
      ${
        additionalHeaders
            .map((key) => `
        <span class="${key}">${key}</span>
      `).join('\n')}
      <span class="msg">Message</span>`
  }

  // Also update column widths to match actual row.
  const headerChildren = Array.from(headerDOM.children);

  const firstRow = document.querySelector('.log-container .log-entry');
  const firstRowChildren = Array.from(firstRow.children);
  headerChildren.forEach((col, index) => {
    if (firstRowChildren[index]) {
      col.setAttribute(
          'style',
          `width:${firstRowChildren[index].getBoundingClientRect().width}`);
      col.setAttribute('title', col.innerText);
    }
  })
}

function getUrlHashParameter(param) {
  var params = getUrlHashParameters();
  return params[param];
}

function getUrlHashParameters() {
  var sPageURL = window.location.hash;
  if (sPageURL)
    sPageURL = sPageURL.split('#')[1];
  var pairs = sPageURL.split('&');
  var object = {};
  pairs.forEach(function(pair, i) {
    pair = pair.split('=');
    if (pair[0] !== '')
      object[pair[0]] = pair[1];
  });
  return object;
}
let currentTheme = {};
let defaultLogStyleRule = 'color: #ffffff;';
let columnStyleRules = {};
let defaultColumnStyles = [];
let logLevelStyles = {};
const logLevelToString = {
  10: 'DBG',
  20: 'INF',
  21: 'OUT',
  30: 'WRN',
  40: 'ERR',
  50: 'CRT',
  70: 'FTL'
};

function setCurrentTheme(newTheme) {
  currentTheme = newTheme;
  defaultLogStyleRule = parseStyle(newTheme.default);
  document.querySelector('body').setAttribute('style', defaultLogStyleRule);
  // Apply default font styles to columns
  let styles = [];
  Object.keys(newTheme).forEach(key => {
    if (key.startsWith('log-table-column-')) {
      styles.push(newTheme[key]);
    }
    if (key.startsWith('log-level-')) {
      logLevelStyles[parseInt(key.replace('log-level-', ''))] =
          parseStyle(newTheme[key]);
    }
  });
  defaultColumnStyles = styles;
}

function parseStyle(rule) {
  const ruleList = rule.split(' ');
  let outputStyle = ruleList.map(fragment => {
    if (fragment.startsWith('bg:')) {
      return `background-color: ${fragment.replace('bg:', '')}`
    } else if (fragment === 'bold') {
      return `font-weight: bold`;
    } else if (fragment === 'underline') {
      return `text-decoration: underline`;
    } else if (fragment.startsWith('#')) {
      return `color: ${fragment}`;
    }
  });
  return outputStyle.join(';')
}

function applyStyling(data, applyColors = false) {
  let colIndex = 0;
  Object.keys(data).forEach(key => {
    if (columnStyleRules[key] && typeof data[key] === 'string') {
      Object.keys(columnStyleRules[key]).forEach(token => {
        data[key] = data[key].replaceAll(
            token,
            `<span
              style="${defaultLogStyleRule};${
                applyColors ? (defaultColumnStyles
                                   [colIndex % defaultColumnStyles.length]) :
                              ''};${parseStyle(columnStyleRules[key][token])};">
                ${token}
            </span>`);
      });
    } else if (key === 'fields') {
      data[key] = applyStyling(data.fields, true);
    }
    if (applyColors) {
      data[key] = `<span
      style="${
          parseStyle(
              defaultColumnStyles[colIndex % defaultColumnStyles.length])}">
        ${data[key]}
      </span>`;
    }
    colIndex++;
  });
  return data;
}

(function() {
const container = document.querySelector('.log-container');
const height = window.innerHeight - 50
let follow = true;
// Initialize our VirtualizedList
var virtualizedList = new VirtualizedList(container, {
  height,
  rowCount: data.length,
  rowHeight: rowHeight,
  estimatedRowHeight: rowHeight,
  renderRow: (index) => {
    const element = document.createElement('div');
    element.classList.add('log-entry');
    element.setAttribute('style', `height: ${rowHeight}px;`);
    const logData = data[index];
    element.innerHTML = `
        <span class="time">${logData.time}</span>
        <span class="level" style="${logLevelStyles[logData.levelno] || ''}">${
        logLevelToString[logData.levelno]}</span>
        ${
        additionalHeaders
            .map(
                (key) => `
          <span class="${key}">${
                    logData[key] || logData.fields[key] || ''}</span>
        `).join('\n')}
        <span class="msg">${logData.message}</span>
      `;
    return element;
  },
  initialIndex: 0,
  onScroll: (scrollTop, event) => {
    const offset =
        virtualizedList._sizeAndPositionManager.getUpdatedOffsetForIndex({
          containerSize: height,
          targetIndex: data.length - 1,
        });

    if (scrollTop < offset) {
      follow = false;
    } else {
      follow = true;
    }
  }
});

const port = getUrlHashParameter('ws')
const hostname = location.hostname || '127.0.0.1';
var ws = new WebSocket(`ws://${hostname}:${port}/`);
ws.onmessage = function(event) {
  let dataObj;
  try {
    dataObj = JSON.parse(event.data);
  } catch (e) {
  }
  if (!dataObj)
    return;

  if (dataObj.__pw_console_colors) {
    const colors = dataObj.__pw_console_colors;
    setCurrentTheme(colors.classes);
    if (colors.column_values) {
      columnStyleRules = {...colors.column_values};
    }
  } else {
    const currentData = {...dataObj, time: formatDate(new Date())};
    updateHeadersFromData(currentData);
    data.push(applyStyling(currentData));
    virtualizedList.setRowCount(data.length);
    if (follow) {
      virtualizedList.scrollToIndex(data.length - 1);
    }
  }
};
})();
