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
 * This module provides session related queries.
 *
 * @module DB API: session related queries.
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')
const util = require('../util/util.js')

/**
 * Returns a promise that resolves into an array of objects containing 'sessionId', 'sessionKey' and 'creationTime'.
 *
 * @export
 * @param {*} db
 * @returns A promise of executing a query.
 */
async function getAllSessions(db) {
  let rows = await dbApi.dbAll(
    db,
    'SELECT SESSION_ID, SESSION_KEY, CREATION_TIME FROM SESSION',
    []
  )
  return rows.map(dbMapping.map.session)
}

/**
 * Sets the session dirty flag to false.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns A promise that resolves with the number of rows updated.
 */
async function setSessionClean(db, sessionId) {
  return dbApi.dbUpdate(
    db,
    'UPDATE SESSION SET DIRTY = ? WHERE SESSION_ID = ?',
    [0, sessionId]
  )
}
/**
 * Resolves with true or false, depending whether this session is dirty.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns A promise that resolves into true or false, reflecting session dirty state.
 */
async function getSessionDirtyFlag(db, sessionId) {
  let row = await dbApi.dbGet(
    db,
    'SELECT DIRTY FROM SESSION WHERE SESSION_ID = ?',
    [sessionId],
    false
  )
  if (row == null) {
    return undefined
  } else {
    return dbApi.fromDbBool(row.DIRTY)
  }
}
/**
 * Resolves w/ the session tied to a session id.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns A promise that resolves into a session
 */
async function getSessionFromSessionId(db, sessionId) {
  return dbApi
    .dbGet(
      db,
      'SELECT SESSION_ID, SESSION_KEY, CREATION_TIME FROM SESSION WHERE SESSION_ID = ?',
      [sessionId]
    )
    .then(dbMapping.map.session)
}

/**
 * Resolves into a session id, obtained from window id.
 *
 * @export
 * @param {*} db
 * @param {*} sessionKey
 * @returns A promise that resolves into an object containing sessionId, sessionKey and creationTime.
 */
async function getSessionInfoFromSessionKey(db, sessionKey) {
  return dbApi
    .dbGet(
      db,
      'SELECT SESSION_ID, SESSION_KEY, CREATION_TIME FROM SESSION WHERE SESSION_KEY = ?',
      [sessionKey]
    )
    .then(dbMapping.map.session)
}

/**
 * Returns a promise that will resolve into a sessionID created from a query.
 *
 * This method has essetially two different use cases:
 *   1.) When there is no sessionId yet (so sessionId argument is null), then this method is expected to either create a new session, or find a
 *       sessionId that is already associated with the given sessionKey.
 *
 *   2.) When a sessionId is passed, then the method simply updates the row with a given sessionId to contain sessionKey and windowId.
 *
 * In either case, the returned promise resolves with a sessionId.
 *
 * @export
 * @param {*} db
 * @param {*} userKey This is in essence the "session cookie id"
 * @param {*} sessionId If sessionId exists already, then it's passed in. If it doesn't then this is null.
 * @returns promise that resolves into a session id.
 */
async function ensureZapSessionId(db, userKey, sessionId = null) {
  if (sessionId == null) {
    // There is no sessionId from before, so we check if there is one mapped to userKey already
    let row = await dbApi.dbGet(
      db,
      'SELECT SESSION_ID FROM SESSION WHERE SESSION_KEY = ?',
      [userKey]
    )
    if (row == null) {
      return dbApi.dbInsert(
        db,
        'INSERT INTO SESSION (SESSION_KEY, CREATION_TIME) VALUES (?,?)',
        [userKey, Date.now()]
      )
    } else {
      return row.SESSION_ID
    }
  } else {
    // This is a case where we want to attach to a given sessionId.
    await dbApi.dbUpdate(
      db,
      'UPDATE SESSION SET SESSION_KEY = ? WHERE SESSION_ID = ?',
      [userKey, sessionId]
    )
    return sessionId
  }
}

/**
 * Returns a promise that will resolve into an existing userId and sessionId.
 * userId and sessionId that are passed as `options` are not
 * validated, they are trusted. If you pass both sessionId and
 * userId, this method will simply return them and do nothing.
 *
 * So don't use this method as "create if it doesn't exist" kind of
 * a method. The purpose is just to quickly ensure that an ID
 * is created when not passed.
 *
 * Returned promise resolves into an object with sessionId and userId.
 *
 * @export
 * @param {*} db
 * @param {*} userKey This is in essence the "session cookie id"
 * @param {*} sessionId If sessionId exists already, then it's passed in and linked to user.
 * @returns promise that resolves into an object with sessionId and userId and newSession.
 */
async function ensureZapUserAndSession(
  db,
  userKey,
  sessionUuid,
  options = {
    sessionId: null,
    userId: null,
  }
) {
  if (options.sessionId != null && options.userId != null) {
    // if we're past both IDs, we simply return them back.
    return {
      sessionId: options.sessionId,
      userId: options.userId,
      newSession: false,
    }
  } else if (options.sessionId != null) {
    // we have a session, but not the user, so we create
    // the user and link the session with it.
    let user = await ensureUser(db, userKey)
    await linkSessionToUser(db, options.sessionId, user.userId)
    return {
      sessionId: options.sessionId,
      userId: user.userId,
      newSession: false,
    }
  } else if (options.userId != null) {
    // we have the user, but not the session, so we create the session,
    // and link it to the user.
    let sessionId = await ensureBlankSession(db, sessionUuid)
    await linkSessionToUser(db, sessionId, options.userId)
    return {
      sessionId: sessionId,
      userId: options.userId,
      newSession: true,
    }
  } else {
    // we have nothing, create both the user and the session.
    let user = await ensureUser(db, userKey)
    let sessionId = await ensureBlankSession(db, sessionUuid)
    await linkSessionToUser(db, sessionId, user.userId)
    return {
      sessionId: sessionId,
      userId: user.userId,
      newSession: true,
    }
  }
}

async function ensureBlankSession(db, uuid) {
  await dbApi.dbInsert(
    db,
    'INSERT OR IGNORE INTO SESSION (SESSION_KEY, CREATION_TIME, DIRTY) VALUES (?,?,?)',
    [uuid, Date.now(), 0]
  )
  const session = await getSessionInfoFromSessionKey(db, uuid)
  return session.sessionId
}

/**
 * When loading in a file, we start with a blank session.
 *
 * @export
 * @param {*} db
 */
async function createBlankSession(db, uuid = null) {
  let newUuid = uuid
  if (newUuid == null) newUuid = util.createUuid()

  return dbApi.dbInsert(
    db,
    'INSERT INTO SESSION (SESSION_KEY, CREATION_TIME, DIRTY) VALUES (?,?,?)',
    [newUuid, Date.now(), 0]
  )
}

/**
 * Returns all users.
 *
 * @param {*} db
 * @param {*} userId
 * @returns Promise that resolves into an array of sessions.
 */
async function getUsers(db) {
  let rows = await dbApi.dbAll(db, 'SELECT * from USER')
  return rows.map(dbMapping.map.user)
}

/**
 * Returns sessions for a given user.
 *
 * @param {*} db
 * @param {*} userId
 * @returns Promise that resolves into an array of sessions.
 */
async function getUsersSessions(db) {
  let allUsers = await getUsers(db)
  let sessionsPerUser = await Promise.all(
    allUsers.map((user) => getUserSessionsById(db, user.userId))
  )
  allUsers.forEach((user, i) => {
    user.sessions = sessionsPerUser[i]
  })
  return allUsers
}

/**
 * Returns sessions for a given user.
 *
 * @param {*} db
 * @param {*} userId
 * @returns Promise that resolves into an array of sessions.
 */
async function getUserSessionsById(db, userId) {
  let rows = await dbApi.dbAll(
    db,
    'SELECT SESSION_ID, SESSION_KEY, CREATION_TIME, DIRTY FROM SESSION WHERE USER_REF = ?',
    [userId]
  )
  return rows.map(dbMapping.map.session)
}

/**
 * Returns user with a given key, or null if none exists.
 *
 * @param {*} db
 * @param {*} userKey
 * @returns A promise of returned user.
 */
async function getUserByKey(db, userKey) {
  let row = await dbApi.dbGet(
    db,
    'SELECT USER_ID, USER_KEY, CREATION_TIME FROM USER WHERE USER_KEY = ?',
    [userKey]
  )
  return dbMapping.map.user(row)
}

/**
 * Creates a new user entry for a given user key if it doesn't exist, or returns
 * the existing user.
 *
 * @param {*} db
 * @param {*} userKey
 * @returns user object, containing userId, userKey and creationTime
 */
async function ensureUser(db, userKey) {
  await dbApi.dbInsert(
    db,
    'INSERT OR IGNORE INTO USER ( USER_KEY, CREATION_TIME ) VALUES (?,?)',
    [userKey, Date.now()]
  )
  return getUserByKey(db, userKey)
}

/**
 * Links an existing session with a user, given both IDs.
 *
 * @param {*} db
 * @param {*} sessionId
 * @param {*} userId
 * @returns promise that resolves into nothing
 */
async function linkSessionToUser(db, sessionId, userId) {
  return dbApi.dbUpdate(
    db,
    `UPDATE SESSION SET USER_REF = ? WHERE SESSION_ID = ?`,
    [userId, sessionId]
  )
}
/**
 * Promises to delete a session from the database, including all the rows that have the session as a foreign key.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns A promise of a removal of session.
 */
async function deleteSession(db, sessionId) {
  return dbApi.dbRemove(db, 'DELETE FROM SESSION WHERE SESSION_ID = ?', [
    sessionId,
  ])
}

/**
 * Write logs to the session log.
 *
 * @param {*} db database connection
 * @param {*} sessionId session id to write log to
 * @param {*} logArray array of objects containing 'timestamp' and 'log'
 * @returns promise of a database insert.
 */
async function writeLog(db, sessionId, logArray) {
  return dbApi.dbMultiInsert(
    db,
    'INSERT INTO SESSION_LOG (SESSION_REF, TIMESTAMP, LOG) VALUES (?,?,?)',
    logArray.map((logEntry) => {
      return [sessionId, logEntry.timestamp, logEntry.log]
    })
  )
}

/**
 * Read all logs for the session.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns promise that resolves into an array of objects containing 'timestamp' and 'log'
 */
async function readLog(db, sessionId) {
  return dbApi
    .dbAll(
      db,
      'SELECT TIMESTAMP, LOG from SESSION_LOG WHERE SESSION_REF = ? ORDER BY TIMESTAMP',
      [sessionId]
    )
    .then((rows) => rows.map(dbMapping.map.sessionLog))
}

/**
 * Promises to update or insert a key/value pair in SESSION_KEY_VALUE table.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @param {*} key
 * @param {*} value
 * @returns A promise of creating or updating a row, resolves with the rowid of a new row.
 */
async function updateSessionKeyValue(db, sessionId, key, value) {
  return dbApi.dbInsert(
    db,
    'INSERT OR REPLACE INTO SESSION_KEY_VALUE (SESSION_REF, KEY, VALUE) VALUES (?,?,?)',
    [sessionId, key, value]
  )
}

/**
 * Promises to insert a key/value pair in SESSION_KEY_VALUE table. Ignore if value already exists.
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @param {*} key
 * @param {*} value
 * @returns A promise of creating or updating a row, resolves with the rowid of a new row.
 */
async function insertSessionKeyValue(db, sessionId, key, value) {
  return dbApi.dbInsert(
    db,
    'INSERT OR IGNORE INTO SESSION_KEY_VALUE (SESSION_REF, KEY, VALUE) VALUES (?,?,?)',
    [sessionId, key, value]
  )
}

/**
 * Promises to insert a whole batch of key/value pairs.
 * Any key/value inside object is loaded.
 *
 * @param {*} db
 * @param {*} session
 * @param {*} object
 * @returns Promise of multi-insert of all attributes inside object.
 */
async function insertSessionKeyValues(db, sessionId, object) {
  let args = []
  for (const [key, value] of Object.entries(object)) {
    args.push([sessionId, key, value])
  }
  return dbApi.dbMultiInsert(
    db,
    'INSERT OR REPLACE INTO SESSION_KEY_VALUE (SESSION_REF, KEY, VALUE) VALUES (?,?,?)',
    args
  )
}

/**
 * Retrieves a value of a single session key.
 *
 * @param {*} db
 * @param {*} sessionId
 * @returns A promise that resolves with a value or with 'undefined' if none is found.
 */
async function getSessionKeyValue(db, sessionId, key) {
  let row = await dbApi.dbGet(
    db,
    'SELECT VALUE FROM SESSION_KEY_VALUE WHERE SESSION_REF = ? AND KEY = ?',
    [sessionId, key]
  )
  if (row == null) {
    return undefined
  } else {
    return row.VALUE
  }
}

/**
 * Resolves to an array of objects that contain 'key' and 'value'
 *
 * @export
 * @param {*} db
 * @param {*} sessionId
 * @returns Promise to retrieve all session key values.
 */
async function getAllSessionKeyValues(db, sessionId) {
  let rows = await dbApi.dbAll(
    db,
    'SELECT KEY, VALUE FROM SESSION_KEY_VALUE WHERE SESSION_REF = ? ORDER BY KEY',
    [sessionId]
  )
  return rows.map((row) => {
    return {
      key: row.KEY,
      value: row.VALUE,
    }
  })
}

// exports
exports.getAllSessions = getAllSessions
exports.setSessionClean = setSessionClean
exports.getSessionDirtyFlag = getSessionDirtyFlag
exports.getSessionFromSessionId = getSessionFromSessionId
exports.getSessionInfoFromSessionKey = getSessionInfoFromSessionKey
exports.ensureZapSessionId = ensureZapSessionId
exports.ensureZapUserAndSession = ensureZapUserAndSession
exports.createBlankSession = createBlankSession
exports.deleteSession = deleteSession
exports.writeLog = writeLog
exports.readLog = readLog
exports.updateSessionKeyValue = updateSessionKeyValue
exports.insertSessionKeyValue = insertSessionKeyValue
exports.insertSessionKeyValues = insertSessionKeyValues
exports.getSessionKeyValue = getSessionKeyValue
exports.getAllSessionKeyValues = getAllSessionKeyValues
exports.ensureUser = ensureUser
exports.getUserSessionsById = getUserSessionsById
exports.getUsers = getUsers
exports.getUsersSessions = getUsersSessions
