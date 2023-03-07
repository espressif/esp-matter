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
 *
 *
 * @jest-environment node
 */
const handlebars = require('handlebars')
const helperC = require('../src-electron/generator/helper-c.js')
const templateEngine = require('../src-electron/generator/template-engine.js')
const { timeout } = require('./test-util.js')

test(
  'handlebars: simple test',
  () => {
    let template = handlebars.compile('{{a}} {{b}} {{c}}!')
    let output = template({ a: 'Very', b: 'simple', c: 'test' })
    expect(output).toEqual('Very simple test!')
  },
  timeout.short()
)

test(
  'handlebars: comment test',
  () => {
    let template = handlebars.compile(
      '{{!-- some random comment --}}{{a}} {{b}} {{c}}!'
    )
    let output = template({ a: 'Very', b: 'simple', c: 'test' })
    expect(output).toEqual('Very simple test!')
  },
  timeout.short()
)

test(
  'handlebars: object test',
  () => {
    let template = handlebars.compile('{{in.a}} {{in.b}} {{in.c}}!')
    let output = template({ in: { a: 'Very', b: 'simple', c: 'test' } })
    expect(output).toEqual('Very simple test!')
  },
  timeout.short()
)

test(
  'handlebars: with test',
  () => {
    let template = handlebars.compile('{{#with in}}{{a}} {{b}} {{c}}!{{/with}}')
    let output = template({ in: { a: 'Very', b: 'simple', c: 'test' } })
    expect(output).toEqual('Very simple test!')
  },
  timeout.short()
)

test(
  'handlebars: each test',
  () => {
    let template = handlebars.compile('{{#each in}}{{this}} {{/each}}!')
    let output = template({ in: ['Very', 'simple', 'test'] })
    expect(output).toEqual('Very simple test !')
  },
  timeout.short()
)

test(
  'handlebars: partials',
  () => {
    handlebars.registerPartial('very_simple_test', 'Very simple test!')
    let template = handlebars.compile('{{> very_simple_test}}')
    let output = template()
    expect(output).toEqual('Very simple test!')
  },
  timeout.short()
)

test(
  'handlebars: helper',
  () => {
    handlebars.registerHelper(
      'supreme_leader',
      (name) => `His most evil excelency, Mr. ${name}`
    )
    let template = handlebars.compile(
      '{{#each list_of_lunatics}}{{supreme_leader this}} {{/each}}'
    )
    let output = template({
      list_of_lunatics: ['Stalin', 'Trotsky', 'Genghis Khan'],
    })
    expect(output).toEqual(
      'His most evil excelency, Mr. Stalin His most evil excelency, Mr. Trotsky His most evil excelency, Mr. Genghis Khan '
    )
  },
  timeout.short()
)

test(
  'handlebars: if helper',
  () => {
    let template = handlebars.compile(
      '{{#if flag}}Yes flag!{{else}}No flag!{{/if}}'
    )
    let output = template({ flag: true })
    expect(output).toEqual('Yes flag!')
    output = template({ flag: false })
    expect(output).toEqual('No flag!')
  },
  timeout.short()
)

test(
  'handlebars: using functions inside the passed input',
  () => {
    let template = handlebars.compile('{{fn}}')
    let output = template({
      fn: () => {
        let text = 'example text'
        let uc = text.toUpperCase()
        return `Got ${text}, returned ${uc}`
      },
    })
    expect(output).toEqual('Got example text, returned EXAMPLE TEXT')
  },
  timeout.short()
)

test(
  'handlebars: using helper to populate the context',
  () => {
    let template = handlebars.compile('{{#each custom_list}}{{value}}{{/each}}')
    let output = template({
      custom_list: () => {
        let list = []
        for (let i = 0; i < 10; i++) {
          list.push({ value: i })
        }
        return list
      },
    })
    expect(output).toEqual('0123456789')
  },
  timeout.short()
)

test(
  'handlebars: helper this processing',
  () => {
    handlebars.registerHelper('inc', function () {
      this.data++
      return this.data
    })
    let template = handlebars.compile('{{inc}}{{inc}}{{inc}}{{inc}}{{inc}}')
    let output = template({ data: 0 })
    expect(output).toEqual('12345')
  },
  timeout.short()
)

test(
  'handlebars: iterator',
  () => {
    handlebars.registerHelper('it', function (options) {
      let ret = this.prefix
      let context = this
      for (let x = 0; x < 10; x++) {
        context.thing = x
        ret = ret + options.fn(context)
      }
      ret = ret + this.postfix
      return ret
    })
    let template = handlebars.compile('{{#it}}{{thing}}{{/it}}')
    let output = template({ prefix: 'PRE:', postfix: ':ERP' })
    expect(output).toEqual('PRE:0123456789:ERP')
  },
  timeout.short()
)

test(
  'delimeter macros',
  () => {
    expect(helperC.as_delimited_macro('VerySimple')).toEqual('VERY_SIMPLE')
    expect(helperC.as_delimited_macro('Very_simple')).toEqual('VERY_SIMPLE')
    expect(helperC.as_delimited_macro('Very_Simple')).toEqual('VERY_SIMPLE')
    expect(helperC.as_delimited_macro('Very_123_Simple')).toEqual(
      'VERY_123_SIMPLE'
    )
    expect(helperC.as_delimited_macro('MfrDefGpdCmd0')).toEqual(
      'MFR_DEF_GPD_CMD0'
    )
  },
  timeout.short()
)
