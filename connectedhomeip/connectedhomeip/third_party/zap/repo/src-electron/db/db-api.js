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

/**
 * This module provides generic DB functions for performing SQL queries.
 *
 * @module JS API: low level database access
 */

const sqlite = require('sqlite3')
const fsp = require('fs').promises
const env = require('../util/env')
const util = require('../util/util.js')
const dbEnum = require('../../src-shared/db-enum.js')
const dbCache = require('./db-cache')
const dbMapping = require('./db-mapping.js')

// This is a SQLITE specific thing. With SQLITE databases,
// we can't have multiple transactions. So this mechanism
// here is handling this.
// If this code ever runs against a database engine that
// supports multiple transactions, this can all go away.
//
let inTransaction = false

function executeBeginTransaction(db, resolve, reject) {
  db.run('BEGIN TRANSACTION', [], function (err) {
    if (err) {
      env.logError('Failed to BEGIN TRANSACTION')
      reject(err)
    } else {
      env.logSql('Executed BEGIN TRANSACTION')
      resolve()
    }
  })
}

function delayBeginTransaction(db, resolve, reject) {
  let cnt = 0
  let interval = setInterval(() => {
    if (inTransaction) {
      cnt++
      if (cnt > 100) {
        reject('Waited for 10s for transaction to relinquish, but it did not.')
      }
    } else {
      clearInterval(interval)
      executeBeginTransaction(db, resolve, reject)
    }
  }, 100)
}

/**
 * Returns a promise to begin a transaction. The beginning of the
 * transaction will be delayed for up to 5 seconds, checking every
 * 1/10th of a second of previous transaction is already finished.
 *
 * After 5 seconds, the code gives up and rejects the promise.
 *
 * This is to allow simultaneous calls to this function, even though
 * SQLite does not allow for simultaneous transactions.
 *
 * So use transactions responsibly.
 *
 * @export
 * @param {*} db
 * @returns A promise that resolves without an argument and rejects with an error from BEGIN TRANSACTION query.
 */
async function dbBeginTransaction(db) {
  return new Promise((resolve, reject) => {
    if (inTransaction) {
      delayBeginTransaction(db, resolve, reject)
    } else {
      inTransaction = true
      executeBeginTransaction(db, resolve, reject)
    }
  })
}

/**
 * Returns a promise to execute a commit.
 *
 * @export
 * @param {*} db
 * @returns A promise that resolves without an argument or rejects with an error from COMMIT query.
 */
async function dbCommit(db) {
  return new Promise((resolve, reject) => {
    db.run('COMMIT', [], function (err) {
      if (err) {
        env.logError('Failed to COMMIT')
        reject(err)
      } else {
        env.logSql('Executed COMMIT')
        inTransaction = false
        resolve()
      }
    })
  })
}

/**
 * Returns a promise to execute a rollback of a transaction.
 *
 * @export
 * @param {*} db
 * @returns A promise that resolves without an argument or rejects with an error from ROLLBACK query.
 */
async function dbRollback(db) {
  return new Promise((resolve, reject) => {
    db.run('ROLLBACK', [], function (err) {
      if (err) {
        env.logError('Failed to ROLLBACK')
        reject(err)
      } else {
        env.logSql('Executed ROLLBACK')
        inTransaction = false
        resolve()
      }
    })
  })
}

/**
 * Returns a promise to execute a DELETE FROM query.
 *
 * @export
 * @param {*} db
 * @param {*} query
 * @param {*} args
 * @returns A promise that resolve with the number of delete rows, or rejects with an error from query.
 */
async function dbRemove(db, query, args) {
  return new Promise((resolve, reject) => {
    db.run(query, args, function (err) {
      if (err) {
        env.logError(`Failed remove: ${query}: ${args}`)
        reject(err)
      } else {
        env.logSql('Executed remove', query, args)
        resolve(this.changes)
      }
    })
  })
}

/**
 * Returns a promise to execute an update query.
 *
 * @export
 * @param {*} db
 * @param {*} query
 * @param {*} args
 * @returns A promise that resolves with a number of changed rows, or rejects with an error from the query.
 */
async function dbUpdate(db, query, args) {
  return new Promise((resolve, reject) => {
    db.run(query, args, function (err) {
      if (err) {
        env.logError(`Failed update: ${query}: ${args}`)
        reject(err)
      } else {
        env.logSql('Executed update', query, args)
        resolve(this.changes)
      }
    })
  })
}

/**
 * Returns a promise to execute an insert query.
 *
 * @export
 * @param {*} db
 * @param {*} query
 * @param {*} args
 * @returns A promise that resolves with the rowid from the inserted row, or rejects with an error from the query.
 */
async function dbInsert(db, query, args) {
  return new Promise((resolve, reject) => {
    db.run(query, args, function (err) {
      if (err) {
        env.logError(`Failed insert: ${query}: ${args} : ${err}`)
        reject(err)
      } else {
        env.logSql('Executed insert', query, args)
        resolve(this.lastID)
      }
    })
  })
}

/**
 * Returns a promise to execute a query to perform a select that returns all rows that match a query.
 *
 * @export
 * @param {*} db
 * @param {*} query
 * @param {*} args
 * @returns A promise that resolves with the rows that got retrieved from the database, or rejects with an error from the query.
 */
async function dbAll(db, query, args) {
  return new Promise((resolve, reject) => {
    db.all(query, args, (err, rows) => {
      if (err) {
        env.logError(`Failed all: ${query}: ${args} : ${err}`)
        reject(err)
      } else {
        env.logSql('Executed all', query, args)
        resolve(rows)
      }
    })
  })
}

/**
 * Returns a promise to execute a query to perform a select that returns first row that matches a query.
 *
 * @export
 * @param {*} db
 * @param {*} query
 * @param {*} args
 * @returns A promise that resolves with a single row that got retrieved from the database, or rejects with an error from the query.
 */
async function dbGet(db, query, args, reportError = true) {
  return new Promise((resolve, reject) => {
    db.get(query, args, (err, row) => {
      if (err) {
        if (reportError) env.logError(`Failed get: ${query}: ${args} : ${err}`)
        reject(err)
      } else {
        env.logSql('Executed get', query, args)
        resolve(row)
      }
    })
  })
}

/**
 * Returns a promise to perform a prepared statement, using data from array for SQL parameters.
 * It resolves with an array of rows, containing the data, or rejects with an error.
 *
 * @param {*} db
 * @param {*} sql
 * @param {*} arrayOfArrays
 */
async function dbMultiSelect(db, sql, arrayOfArrays) {
  return new Promise((resolve, reject) => {
    env.logSql('Preparing select', sql, arrayOfArrays.length)
    let rows = []
    let statement = db.prepare(sql, function (err) {
      if (err) reject(err)
      for (const singleArray of arrayOfArrays) {
        statement.get(singleArray, (err2, row) => {
          if (err2) {
            reject(err2)
          } else {
            rows.push(row)
          }
        })
      }
      statement.finalize((err3) => {
        if (err3) {
          reject(err3)
        } else {
          resolve(rows)
        }
      })
    })
  })
}

/**
 * Returns a promise to perfom a prepared statement, using data from array for SQL parameters.
 * It resolves with an array of rowids, or rejects with an error.
 *
 * @export
 * @param {*} db
 * @param {*} sql
 * @param {*} arrayOfArrays
 * @returns A promise that resolves with the array of rowids for the rows that got inserted, or rejects with an error from the query.
 */
async function dbMultiInsert(db, sql, arrayOfArrays) {
  return new Promise((resolve, reject) => {
    env.logSql('Preparing insert', sql, arrayOfArrays.length)
    let lastIds = []
    let statement = db.prepare(sql, function (err) {
      if (err) reject('Error While preparing sql command: ' + sql + ', ' + err)
      for (const singleArray of arrayOfArrays) {
        statement.run(singleArray, (err2) => {
          if (err2)
            reject(
              'Error While running sql command: ' +
                sql +
                ', values: ' +
                singleArray +
                ', ' +
                err2
            )
          lastIds.push(this.lastID)
        })
      }
      statement.finalize((err3) => {
        if (err3)
          reject('Error While finalizing sql command: ' + sql + ', ' + err3)
        resolve(lastIds)
      })
    })
  })
}

/**
 * Returns a promise that will resolve when the database in question is closed.
 * Rejects with an error if closing fails.
 *
 * @param {*} database
 * @returns A promise that resolves without an argument or rejects with error from the database closing.
 */
async function closeDatabase(database) {
  dbCache.clear()
  return new Promise((resolve, reject) => {
    env.logSql('About to close database.')
    database.close((err) => {
      if (err) return reject(err)
      env.logSql('Database is closed.')
      resolve()
    })
  })
}

/**
 * Imediatelly closes the database.
 *
 * @param {*} database
 */
function closeDatabaseSync(database) {
  dbCache.clear()
  env.logSql('About to close database.')
  database.close((err) => {
    if (err) console.log(`Database close error: ${err}`)
    env.logSql('Database is closed.')
  })
}

/**
 * Create in-memory database.
 *
 *  @returns Promise that resolve with the Db.
 */
async function initRamDatabase() {
  dbCache.clear()
  return new Promise((resolve, reject) => {
    let db = new sqlite.Database(':memory:', (err) => {
      if (err) {
        reject(err)
      } else {
        env.logSql(`Connected to the RAM database.`)
        resolve(db)
      }
    })
  })
}

/**
 * Returns a promise to initialize a database.
 *
 * @export
 * @param {*} sqlitePath
 * @returns A promise that resolves with the database object that got created, or rejects with an error if something went wrong.
 */
async function initDatabase(sqlitePath) {
  dbCache.clear()
  return new Promise((resolve, reject) => {
    let db = new sqlite.Database(sqlitePath, (err) => {
      if (err) {
        reject(err)
      } else {
        env.logSql(`Connected to the database at: ${sqlitePath}`)
        resolve(db)
      }
    })
  })
}

/**
 * Returns a promise to insert or replace a setting into the database.
 *
 * @param {*} db
 * @param {*} version
 * @returns  A promise that resolves with a rowid of created setting row or rejects with error if something goes wrong.
 */
async function insertOrReplaceSetting(db, category, key, value) {
  return dbInsert(
    db,
    'INSERT OR REPLACE INTO SETTING ( CATEGORY, KEY, VALUE ) VALUES ( ?, ?, ? )',
    [category, key, value]
  )
}

/**
 * Updates SETTING table with values selected
 *
 * @param {*} db
 * @param {*} rows
 * @returns  A promise that resolves with the SETTING table being repopulated
 */
async function updateSetting(db, rows) {
  for (let i = 0; i < rows.length; i++) {
    dbInsert(
      db,
      'INSERT OR REPLACE INTO SETTING ( CATEGORY, KEY, VALUE ) VALUES ( ?, ?, ? )',
      [rows[i].category, rows[i].key, rows[i].value]
    )
  }
}

/**
 * Returns a promise resolving the entire SETTING table
 *
 * @param {*} db
 * @returns  A promise resolving the entire SETTING table
 */
async function selectSettings(db) {
  let rows = []
  rows = await dbAll(db, 'SELECT CATEGORY,KEY,VALUE FROM SETTING')
  return rows.map(dbMapping.map.settings)
}

async function determineIfSchemaShouldLoad(db, context) {
  try {
    let row = await dbGet(
      db,
      'SELECT CRC FROM PACKAGE WHERE PATH = ?',
      [context.filePath],
      false
    )
    if (row == null) {
      context.mustLoad = true
    } else {
      context.mustLoad = row.CRC != context.crc
    }
    context.hasSchema = true
    return context
  } catch (err) {
    // Fall through, do nothing
    context.mustLoad = true
    context.hasSchema = false
    return context
  }
}

async function updateCurrentSchemaCrc(db, context) {
  await dbInsert(
    db,
    'INSERT OR REPLACE INTO PACKAGE (PATH, CRC, TYPE) VALUES ( ?, ?, ? )',
    [context.filePath, context.crc, dbEnum.packageType.sqlSchema]
  )
  return context
}

async function performSchemaLoad(db, schemaContent) {
  return new Promise((resolve, reject) => {
    env.logSql('Loading schema.')
    db.serialize(() => {
      db.exec(schemaContent, (err) => {
        if (err) {
          env.logError('Failed to populate schema')
          env.logError(err)
          reject(err)
        }
        resolve()
      })
    })
  })
}

/**
 * Returns a promise to load schema into a blank database, and inserts a version to the settings table.j
 *
 * @export
 * @param {*} db
 * @param {*} schemaPath
 * @param {*} zapVersion
 * @returns A promise that resolves with the same db that got passed in, or rejects with an error.
 */
async function loadSchema(db, schemaPath, zapVersion, sqliteFile = null) {
  let schemaFileContent = await fsp.readFile(schemaPath, 'utf8')
  let rows = []
  let context = {
    filePath: schemaPath,
    data: schemaFileContent,
    crc: util.checksum(schemaFileContent),
  }
  await determineIfSchemaShouldLoad(db, context)
  if (context.mustLoad && context.hasSchema) {
    rows = await selectSettings(db)
    await closeDatabase(db)
    if (sqliteFile != null) util.createBackupFile(sqliteFile)
  }
  if (context.mustLoad && context.hasSchema) {
    if (sqliteFile == null) {
      db = await initRamDatabase()
    } else {
      db = await initDatabase(sqliteFile)
    }
  }
  if (context.mustLoad) {
    await performSchemaLoad(db, context.data)
    await updateCurrentSchemaCrc(db, context)
    await updateSetting(db, rows)
  }

  await insertOrReplaceSetting(db, 'APP', 'VERSION', zapVersion.version)
  if ('hash' in zapVersion) {
    await insertOrReplaceSetting(db, 'APP', 'HASH', zapVersion.hash)
  }
  if ('date' in zapVersion) {
    await insertOrReplaceSetting(db, 'APP', 'DATE', zapVersion.date)
  }
  return db
}

/**
 * Init database and load the schema.
 *
 * @param {*} sqliteFile
 * @param {*} schemaFile
 * @param {*} zapVersion
 * @returns Promise that resolves into the database object.
 */
async function initDatabaseAndLoadSchema(sqliteFile, schemaFile, zapVersion) {
  let db = await initDatabase(sqliteFile)
  return loadSchema(db, schemaFile, zapVersion, sqliteFile)
}

/**
 * Returns the data that should be stored into the DB column, from the passed JS boolean.
 *
 * @param {*} value
 * @returns Value to be stored into the database.
 */
function toDbBool(value) {
  return value ? 1 : 0
}

/**
 * Returns a true or false JS boolean from the value that was read in the database.
 *
 * @param {*} value
 * @return value to be used in JS after reading value from database.
 */
function fromDbBool(value) {
  return value == 1
}

/**
 *
 * @param {*} value
 * @returns Given value in the form of string
 */
function toInClause(value) {
  return value ? value.toString() : value
}

exports.dbBeginTransaction = dbBeginTransaction
exports.dbCommit = dbCommit
exports.dbRollback = dbRollback
exports.dbRemove = dbRemove
exports.dbUpdate = dbUpdate
exports.dbInsert = dbInsert
exports.dbAll = dbAll
exports.dbGet = dbGet
exports.dbMultiSelect = dbMultiSelect
exports.dbMultiInsert = dbMultiInsert
exports.closeDatabase = closeDatabase
exports.closeDatabaseSync = closeDatabaseSync
exports.initRamDatabase = initRamDatabase
exports.initDatabase = initDatabase
exports.loadSchema = loadSchema
exports.initDatabaseAndLoadSchema = initDatabaseAndLoadSchema
exports.toDbBool = toDbBool
exports.fromDbBool = fromDbBool
exports.toInClause = toInClause
