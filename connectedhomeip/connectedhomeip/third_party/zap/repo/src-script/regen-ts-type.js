const sqlite3 = require('sqlite3')
const _ = require('lodash')
const fs = require('fs')
const path = require('path')

const db = new sqlite3.Database(':memory:')
const schema = path.resolve(__dirname, '../src-electron/db/zap-schema.sql')
const outputFile = path.resolve(__dirname, '../src-shared/types/db-types.ts')

// Utility function for generating TypeScript interface types from ZAP table schema

// sqlite3.verbose()

let sqlToTypescriptTypes = { integer: 'number', text: 'string' }

// load schema
db.serialize(async function () {
  let schemaFileContent = fs.readFileSync(schema, 'utf8')

  db.exec(schemaFileContent, (err) => {
    if (err) {
      console.log('Failed to populate schema')
      console.log(err)
    }
  })
})

function tableNames(db) {
  return dbAll(
    db,
    "SELECT name FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%'"
  ).then((tables) => tables.map((x) => x.name))
}

function tableDetails(db) {
  return tableNames(db).then((names) =>
    Promise.all(
      names.map((name) => dbAll(db, "PRAGMA table_info('" + name + "')"))
    )
  )
}

async function regenDatabaseTypes() {
  let output = [
    `/**
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

import * as sqlite from 'sqlite3'
export type DbType = sqlite.Database

// This file is generated via ./src-script/regen-ts-type.js.  Do not hand edit this file!
`,
  ]

  console.log('Generating TypeScript interfaces via zap-schema.sql')

  let names = await tableNames(db)
  let details = await tableDetails(db)
  names.forEach((name, index) => {
    output.push(`export interface Db${_.upperFirst(_.camelCase(name))}Type {`)
    details[index].sort(function (a, b) {
      let nameA = a.name.toUpperCase() // ignore upper and lowercase
      let nameB = b.name.toUpperCase() // ignore upper and lowercase
      if (nameA < nameB) {
        return -1
      }
      if (nameA > nameB) {
        return 1
      }

      // names must be equal
      return 0
    })

    details[index].forEach((r) => {
      let k = r.name
      let v = sqlToTypescriptTypes[r.type]
      if (r.name === `${name.toUpperCase()}_ID`) {
        k = 'id'
      } else if (
        r.name === `${name.toUpperCase().replace(/^PACKAGE_/, '')}_ID`
      ) {
        k = 'id'
      } else if (r.name === `PARENT_${name.toUpperCase()}_REF`) {
        k = 'parentId'
      } else if (name === 'CLUSTER' && r.name === `DESCRIPTION`) {
        k = 'caption'
      } else if (name === 'CLUSTER' && r.name === `NAME`) {
        k = 'label'
      }

      output.push(`  ${_.camelCase(k)}: ${v}`)
    })
    output.push(`}\n`)
  })

  fs.writeFile(outputFile, output.join('\n'), (err) => {
    if (err) {
      console.error(err)
      return
    }

    console.log(`Generated TypeScript interfaces: ${outputFile}`)
  })
}

async function dbAll(db, query, args) {
  return new Promise((resolve, reject) => {
    db.all(query, args, (err, rows) => {
      if (err) {
        console.log(`Failed all: ${query}: ${args} : ${err}`)
        reject(err)
      } else {
        // console.log(`Executed all: ${query}: ${args}`)
        resolve(rows)
      }
    })
  })
}

regenDatabaseTypes()
