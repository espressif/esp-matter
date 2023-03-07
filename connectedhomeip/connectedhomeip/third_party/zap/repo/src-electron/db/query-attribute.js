/**
 *
 *    Copyright (c) 2021 Silicon Labs
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
 * This module provides queries related to attributes.
 *
 * @module DB API: attribute queries.
 */
const dbApi = require('./db-api.js')
const dbMapping = require('./db-mapping.js')

function attributeExportMapping(x) {
  return {
    id: x.ATTRIBUTE_ID,
    name: x.NAME,
    code: x.CODE,
    side: x.SIDE,
    type: x.TYPE,
    define: x.DEFINE,
    mfgCode: x.MANUFACTURER_CODE,
    clusterSide: x.SIDE,
    clusterName: x.CLUSTER_NAME,
    isClusterEnabled: x.ENABLED,
  }
}

/**
 * Promises to select all endpoint type attributes filtered by EndpointTypeRef and ClusterRef.
 *
 * @export
 * @param {*} db
 * @param {*} endpointTypeRef
 * @param {*} endpointTypeClusterRef
 * @returns Records of selected Endpoint Type Attributes.
 */
 async function selectEndpointTypeAttributesByEndpointTypeRefAndClusterRef(
  db,
  endpointTypeRef,
  endpointTypeClusterRef
){
  let rows = await dbApi.dbAll(
    db,
    `
    select 
      ENDPOINT_TYPE_ATTRIBUTE_ID,
      ENDPOINT_TYPE_REF,
      ENDPOINT_TYPE_CLUSTER_REF AS 'CLUSTER_REF',
      ATTRIBUTE_REF,
      INCLUDED,
      STORAGE_OPTION,
      SINGLETON,
      BOUNDED,
      DEFAULT_VALUE,
      INCLUDED_REPORTABLE,
      MIN_INTERVAL,
      MAX_INTERVAL,
      REPORTABLE_CHANGE
    from 
      ENDPOINT_TYPE_ATTRIBUTE
    where
      ENDPOINT_TYPE_REF = ? and ENDPOINT_TYPE_CLUSTER_REF = ?`,
    [endpointTypeRef,endpointTypeClusterRef]
  )
  return rows.map(dbMapping.map.endpointTypeAttribute)
}

/**
 * Promises to duplicate endpoint type attributes.
 *
 * @export
 * @param {*} db
 * @param {*} newEndpointTypeRef
 * @param {*} newEndpointTypeClusterRef
 * @param {*} attribute
 * @returns Promise duplicated endpoint type attribute's id.
 */
async function duplicateEndpointTypeAttribute(
  db,
  newEndpointTypeRef,
  newEndpointTypeClusterRef,
  attribute
){
  return await dbApi.dbInsert(
    db,
    `INSERT INTO ENDPOINT_TYPE_ATTRIBUTE (
      ENDPOINT_TYPE_REF,
      ENDPOINT_TYPE_CLUSTER_REF,
      ATTRIBUTE_REF,
      INCLUDED,
      STORAGE_OPTION,
      SINGLETON,
      BOUNDED,
      DEFAULT_VALUE,
      INCLUDED_REPORTABLE,
      MIN_INTERVAL,
      MAX_INTERVAL,
      REPORTABLE_CHANGE)
      VALUES (
        ?,
        ?,
        ?,
        ?,
        ?,
        ?,
        ?,
        ?,
        ?,
        ?,
        ?,
        ?
      )`,
    [ newEndpointTypeRef,
      newEndpointTypeClusterRef,
      attribute.attributeRef,
      attribute.included,
      attribute.storageOption,
      attribute.singleton,
      attribute.bounded,
      attribute.defaultValue,
      attribute.includedReportable,
      attribute.minInterval,
      attribute.maxInterval,
      attribute.reportableChange ]
  )
}


/**
 * Returns a promise of data for attributes inside an endpoint type.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} packageIds
 * @returns Promise that resolves with the attribute data.
 */
async function selectAllAttributeDetailsFromEnabledClusters(
  db,
  endpointsAndClusters,
  packageIds
) {
  let endpointTypeClusterRef = endpointsAndClusters
    .map((ep) => ep.endpointTypeClusterRef)
    .toString()
  return dbApi
    .dbAll(
      db,
      `
  SELECT
    ATTRIBUTE.ATTRIBUTE_ID,
    ATTRIBUTE.NAME,
    ATTRIBUTE.CODE,
    ATTRIBUTE.SIDE,
    ATTRIBUTE.TYPE,
    ATTRIBUTE.DEFINE,
    ATTRIBUTE.MANUFACTURER_CODE,
    ENDPOINT_TYPE_CLUSTER.SIDE,
    CLUSTER.NAME AS CLUSTER_NAME,
    ENDPOINT_TYPE_CLUSTER.ENABLED
  FROM ATTRIBUTE
  INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
  ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
  INNER JOIN CLUSTER
  ON ATTRIBUTE.CLUSTER_REF = CLUSTER.CLUSTER_ID
  INNER JOIN ENDPOINT_TYPE_CLUSTER
  ON CLUSTER.CLUSTER_ID = ENDPOINT_TYPE_CLUSTER.CLUSTER_REF
  WHERE ENDPOINT_TYPE_CLUSTER.CLUSTER_REF IN (${endpointTypeClusterRef})
  AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1
  AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)}) 
  GROUP BY ATTRIBUTE.NAME
        `
    )
    .then((rows) => rows.map(attributeExportMapping))
}

/**
 * Returns a promise of data for manufacturing/non-manufacturing specific attributes
 * inside an endpoint type.
 *
 * @param db
 * @param endpointTypeId
 * @param isManufacturingSpecific,
 * @param packageIds
 * @returns Promise that resolves with the manufacturing/non-manufacturing
 * specific attribute data.
 */
async function selectAttributeDetailsFromAllEndpointTypesAndClustersUtil(
  db,
  endpointsAndClusters,
  isManufacturingSpecific,
  packageIds
) {
  let endpointTypeIds = endpointsAndClusters
    .map((ep) => ep.endpointId)
    .toString()
  let endpointClusterIds = endpointsAndClusters
    .map((ep) => ep.endpointClusterId)
    .toString()
  return dbApi
    .dbAll(
      db,
      `
  SELECT
    ATTRIBUTE.ATTRIBUTE_ID,
    ATTRIBUTE.NAME,
    ATTRIBUTE.CODE,
    ATTRIBUTE.SIDE,
    ATTRIBUTE.TYPE,
    ATTRIBUTE.DEFINE,
    ATTRIBUTE.MANUFACTURER_CODE,
    ENDPOINT_TYPE_CLUSTER.SIDE,
    CLUSTER.NAME AS CLUSTER_NAME,
    ENDPOINT_TYPE_CLUSTER.ENABLED
  FROM
    ATTRIBUTE
  INNER JOIN
    ENDPOINT_TYPE_ATTRIBUTE
  ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
  INNER JOIN
    ENDPOINT_TYPE_CLUSTER
  ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
  INNER JOIN
    CLUSTER
  ON ATTRIBUTE.CLUSTER_REF = CLUSTER.CLUSTER_ID
  WHERE
    ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_REF IN (${endpointTypeIds})
    AND ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF in (${endpointClusterIds})
    AND ATTRIBUTE.MANUFACTURER_CODE IS ` +
        (isManufacturingSpecific ? `NOT ` : ``) +
        `NULL
    AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1
    AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)}) 
  GROUP BY ATTRIBUTE.NAME
        `
    )
    .then((rows) => rows.map(attributeExportMapping))
}

/**
 * Returns a promise of data for manufacturing specific attributes inside an endpoint type.
 *
 * @param db
 * @param endpointTypeId
 * @param packageIds
 * @returns Promise that resolves with the manufacturing specific attribute data.
 */
async function selectManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters(
  db,
  endpointsAndClusters,
  packageIds
) {
  return selectAttributeDetailsFromAllEndpointTypesAndClustersUtil(
    db,
    endpointsAndClusters,
    true,
    packageIds
  )
}

/**
 * Returns a promise of data for attributes with no manufacturing specific information inside an endpoint type.
 *
 * @param db
 * @param endpointTypeId
 * @param packageIds
 * @returns Promise that resolves with the non-manufacturing specific attribute data.
 */
async function selectNonManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters(
  db,
  endpointsAndClusters,
  packageIds
) {
  return selectAttributeDetailsFromAllEndpointTypesAndClustersUtil(
    db,
    endpointsAndClusters,
    false,
    packageIds
  )
}

/**
 * Returns a promise of data for attributes inside an endpoint type
 * that either have a default or a bounded attribute.
 *
 * @param {*} db
 * @param {*} endpointTypeId
 * @param {*} packageIds
 * @returns Promise that resolves with the attribute data.
 */
async function selectAttributeDetailsWithABoundFromEnabledClusters(
  db,
  endpointsAndClusters,
  packageIds
) {
  let endpointClusterIds = endpointsAndClusters
    .map((ep) => ep.endpointClusterId)
    .toString()
  let mapFunction = (x) => {
    return {
      id: x.ATTRIBUTE_ID,
      name: x.NAME,
      side: x.SIDE,
      clusterName: x.CLUSTER_NAME,
      attributeMinValue: x.MIN,
      attributeMaxValue: x.MAX,
      defaultValue: x.DEFAULT_VALUE,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
SELECT
  ATTRIBUTE.ATTRIBUTE_ID,
  ATTRIBUTE.NAME,
  ATTRIBUTE.SIDE,
  CLUSTER.NAME AS CLUSTER_NAME,
  ATTRIBUTE.MIN,
  ATTRIBUTE.MAX,
  ENDPOINT_TYPE_ATTRIBUTE.DEFAULT_VALUE
FROM
  ATTRIBUTE
INNER JOIN
  ENDPOINT_TYPE_ATTRIBUTE
ON
  ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
INNER JOIN
  ENDPOINT_TYPE_CLUSTER
ON
  ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
INNER JOIN
  CLUSTER
ON
  CLUSTER.CLUSTER_ID = ENDPOINT_TYPE_CLUSTER.CLUSTER_REF
INNER JOIN
  ATOMIC
ON
  ATOMIC.NAME = ATTRIBUTE.TYPE
WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF in (${endpointClusterIds})
  AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ENDPOINT_TYPE_ATTRIBUTE.BOUNDED !=0
  AND ENDPOINT_TYPE_CLUSTER.ENABLED=1
  AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)}) 
GROUP BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.MANUFACTURER_CODE, ATTRIBUTE.NAME, ATTRIBUTE.SIDE
        `
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * The enabled attributes details across all endpoints and clusters.
 * @param db
 * @param endpointsAndClusters
 * @param packageIds
 * @returns The enabled attributes details across all endpoints and clusters.
 */
async function selectAttributeDetailsFromEnabledClusters(
  db,
  endpointsAndClusters,
  packageIds
) {
  let endpointClusterIds = endpointsAndClusters
    .map((ep) => ep.endpointClusterId)
    .toString()
  let mapFunction = (x) => {
    return {
      id: x.ATTRIBUTE_ID,
      name: x.NAME,
      code: x.CODE,
      side: x.SIDE,
      type: x.TYPE,
      define: x.DEFINE,
      mfgCode: x.MANUFACTURER_CODE,
      isWritable: x.IS_WRITABLE,
      clusterId: x.CLUSTER_ID,
      clusterSide: x.CLUSTER_SIDE,
      clusterName: x.CLUSTER_NAME,
      clusterDefine: x.CLUSTER_DEFINE,
      clusterCode: x.CLUSTER_CODE,
      isAttributeBounded: x.BOUNDED,
      storageOption: x.STORAGE_OPTION,
      isSingleton: x.SINGLETON,
      defaultValue: x.DEFAULT_VALUE,
      attributeSize: x.ATOMIC_SIZE,
      clusterIndex: x.CLUSTER_INDEX,
      mfgAttributeCount: x.MANUFACTURING_SPECIFIC_ATTRIBUTE_COUNT,
      singletonAttributeSize: x.SINGLETON_ATTRIBUTE_SIZE,
      maxAttributeSize: x.MAX_ATTRIBUTE_SIZE,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
  SELECT
  ATTRIBUTE.ATTRIBUTE_ID,
    ATTRIBUTE.NAME,
    ATTRIBUTE.CODE,
    ATTRIBUTE.SIDE,
    ATTRIBUTE.TYPE,
    ATTRIBUTE.DEFINE,
    ATTRIBUTE.MANUFACTURER_CODE,
    ATTRIBUTE.IS_WRITABLE,
    CLUSTER.CLUSTER_ID AS CLUSTER_ID,
    ENDPOINT_TYPE_CLUSTER.SIDE AS CLUSTER_SIDE,
    CLUSTER.NAME AS CLUSTER_NAME,
    CLUSTER.DEFINE AS CLUSTER_DEFINE,
    CLUSTER.CODE AS CLUSTER_CODE,
    ENDPOINT_TYPE_ATTRIBUTE.BOUNDED,
    ENDPOINT_TYPE_ATTRIBUTE.STORAGE_OPTION,
    ENDPOINT_TYPE_ATTRIBUTE.SINGLETON,
    ENDPOINT_TYPE_ATTRIBUTE.DEFAULT_VALUE,
    CASE
      WHEN ATOMIC.IS_STRING=1 THEN 
        CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
             WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
             ELSE ATOMIC.ATOMIC_SIZE
             END
        WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
        ELSE ATOMIC.ATOMIC_SIZE
    END AS ATOMIC_SIZE,
    ROW_NUMBER() OVER (PARTITION BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ENDPOINT_TYPE_CLUSTER.SIDE ORDER BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE) CLUSTER_INDEX,
    COUNT (ATTRIBUTE.MANUFACTURER_CODE) OVER () AS MANUFACTURING_SPECIFIC_ATTRIBUTE_COUNT,
    SUM (CASE WHEN ENDPOINT_TYPE_ATTRIBUTE.SINGLETON=1 THEN 
          CASE WHEN ATOMIC.IS_STRING=1 THEN 
            CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
                 WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
                 ELSE ATOMIC.ATOMIC_SIZE
            END
          WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
          ELSE ATOMIC.ATOMIC_SIZE
          END
        ELSE 0 END) OVER () AS SINGLETON_ATTRIBUTE_SIZE,
    MAX(CASE WHEN ATOMIC.IS_STRING=1 THEN 
          CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
              WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
              ELSE ATOMIC.ATOMIC_SIZE
          END
        WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
        ELSE ATOMIC.ATOMIC_SIZE
        END) OVER () AS MAX_ATTRIBUTE_SIZE
  FROM ATTRIBUTE
  INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
  ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
  INNER JOIN ENDPOINT_TYPE_CLUSTER
  ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
  INNER JOIN CLUSTER
  ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
  INNER JOIN ATOMIC
  ON ATOMIC.NAME = ATTRIBUTE.TYPE
  WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF IN (${endpointClusterIds})
  AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ENDPOINT_TYPE_CLUSTER.ENABLED=1 AND ENDPOINT_TYPE_CLUSTER.SIDE=ATTRIBUTE.SIDE
  AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  GROUP BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE, ATTRIBUTE.SIDE
  ORDER BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ENDPOINT_TYPE_CLUSTER.SIDE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE
        `
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 *
 * @param db
 * @param endpointsAndClusters
 * @param packageIds
 * @returns
 * Default values for the attributes longer than a pointer,
 * in a form of a binary blob. All attribute values with size greater than 2 bytes.
 * Excluding 0 values and externally saved values
 * Union is used to get separate entries of attributes w.r.t to default, minimum
 * and maximum values
 */

async function selectAttributeBoundDetails(
  db,
  endpointsAndClusters,
  packageIds
) {
  let endpointClusterIds = endpointsAndClusters
    .map((ep) => ep.endpointClusterId)
    .toString()
  let mapFunction = (x) => {
    return {
      id: x.ATTRIBUTE_ID,
      name: x.NAME,
      side: x.SIDE,
      clusterName: x.CLUSTER_NAME,
      clusterMfgCode: x.CLUSTER_MANUFACTURER_CODE,
      defaultValue: x.ATT_VALUE,
      attributeSize: x.ATOMIC_SIZE,
      attributeValueType: x.ATTRIBUTE_VALUE_TYPE,
      arrayIndex: x.ARRAY_INDEX,
      isString: x.IS_STRING,
    }
  }
  return dbApi
    .dbAll(
      db,
      `SELECT
      *, SUM(ATOMIC_SIZE) OVER (ORDER BY CLUSTER_MANUFACTURER_CODE, CLUSTER_NAME ROWS BETWEEN UNBOUNDED PRECEDING AND 1 PRECEDING) AS ARRAY_INDEX FROM (
  SELECT
    ATTRIBUTE.ATTRIBUTE_ID AS ATTRIBUTE_ID,
    ATTRIBUTE.NAME AS NAME,
    ATTRIBUTE.SIDE AS SIDE,
    CLUSTER.NAME AS CLUSTER_NAME,
    CLUSTER.MANUFACTURER_CODE AS CLUSTER_MANUFACTURER_CODE,
    ENDPOINT_TYPE_ATTRIBUTE.DEFAULT_VALUE AS ATT_VALUE,
    CASE
    WHEN ATOMIC.IS_STRING=1 THEN 
      CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
           WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
           ELSE ATOMIC.ATOMIC_SIZE
           END
      WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
      ELSE ATOMIC.ATOMIC_SIZE
    END AS ATOMIC_SIZE,
    'DEFAULT' as ATTRIBUTE_VALUE_TYPE,
    ATOMIC.IS_STRING AS IS_STRING
  FROM ATTRIBUTE
  INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
  ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
  INNER JOIN ENDPOINT_TYPE_CLUSTER
  ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
  INNER JOIN CLUSTER
  ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
  INNER JOIN ATOMIC
  ON ATOMIC.NAME = ATTRIBUTE.TYPE
  WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF in (${endpointClusterIds})
  AND ENDPOINT_TYPE_CLUSTER.SIDE = ATTRIBUTE.SIDE AND ENDPOINT_TYPE_CLUSTER.ENABLED=1
  AND (CASE
    WHEN ATOMIC.IS_STRING=1 THEN 
      CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
           WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
           ELSE ATOMIC.ATOMIC_SIZE
           END
      WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
      ELSE ATOMIC.ATOMIC_SIZE
    END) > 2 AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ATT_VALUE IS NOT NULL AND ATT_VALUE != "" AND REPLACE(ATT_VALUE, '0', '')!='x' AND REPLACE(ATT_VALUE, '0', '')!=''
    AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  GROUP BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE, ATTRIBUTE.SIDE
  UNION
  SELECT
  ATTRIBUTE.ATTRIBUTE_ID AS ATTRIBUTE_ID,
  ATTRIBUTE.NAME AS NAME,
  ATTRIBUTE.SIDE AS SIDE,
  CLUSTER.NAME AS CLUSTER_NAME,
  CLUSTER.MANUFACTURER_CODE AS CLUSTER_MANUFACTURER_CODE,
  ATTRIBUTE.MIN AS ATT_VALUE,
  CASE
  WHEN ATOMIC.IS_STRING=1 THEN 
    CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
         WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
         ELSE ATOMIC.ATOMIC_SIZE
         END
    WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
    ELSE ATOMIC.ATOMIC_SIZE
  END AS ATOMIC_SIZE,
  'MINIMUM' as ATTRIBUTE_VALUE_TYPE,
  ATOMIC.IS_STRING AS IS_STRING
FROM ATTRIBUTE
INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
INNER JOIN ENDPOINT_TYPE_CLUSTER
ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
INNER JOIN CLUSTER
ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
INNER JOIN ATOMIC
ON ATOMIC.NAME = ATTRIBUTE.TYPE
WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF in (${endpointClusterIds})
AND ENDPOINT_TYPE_CLUSTER.SIDE = ATTRIBUTE.SIDE AND ENDPOINT_TYPE_CLUSTER.ENABLED=1
AND (CASE
  WHEN ATOMIC.IS_STRING=1 THEN 
    CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
         WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
         ELSE ATOMIC.ATOMIC_SIZE
         END
    WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
    ELSE ATOMIC.ATOMIC_SIZE
  END) > 2 AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ATT_VALUE IS NOT NULL AND ATT_VALUE != "" AND ENDPOINT_TYPE_ATTRIBUTE.BOUNDED !=0 AND REPLACE(ATT_VALUE, '0', '')!='x' AND REPLACE(ATT_VALUE, '0', '')!=''
  AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
GROUP BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE, ATTRIBUTE.SIDE
UNION
  SELECT
  ATTRIBUTE.ATTRIBUTE_ID AS ATTRIBUTE_ID,
  ATTRIBUTE.NAME AS NAME,
  ATTRIBUTE.SIDE AS SIDE,
  CLUSTER.NAME AS CLUSTER_NAME,
  CLUSTER.MANUFACTURER_CODE AS CLUSTER_MANUFACTURER_CODE,
  ATTRIBUTE.MAX AS ATT_VALUE,
  CASE
  WHEN ATOMIC.IS_STRING=1 THEN 
    CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
         WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
         ELSE ATOMIC.ATOMIC_SIZE
         END
    WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
    ELSE ATOMIC.ATOMIC_SIZE
  END AS ATOMIC_SIZE,
  'MAXIMUM' as ATTRIBUTE_VALUE_TYPE,
  ATOMIC.IS_STRING AS IS_STRING
FROM ATTRIBUTE
INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
INNER JOIN ENDPOINT_TYPE_CLUSTER
ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
INNER JOIN CLUSTER
ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
INNER JOIN ATOMIC
ON ATOMIC.NAME = ATTRIBUTE.TYPE
WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF in (${endpointClusterIds})
AND ENDPOINT_TYPE_CLUSTER.SIDE = ATTRIBUTE.SIDE AND ENDPOINT_TYPE_CLUSTER.ENABLED=1
AND (CASE
  WHEN ATOMIC.IS_STRING=1 THEN 
    CASE WHEN ATOMIC.IS_LONG=0 THEN ATTRIBUTE.MAX_LENGTH+1
         WHEN ATOMIC.IS_LONG=1 THEN ATTRIBUTE.MAX_LENGTH+2
         ELSE ATOMIC.ATOMIC_SIZE
         END
    WHEN ATOMIC.ATOMIC_SIZE IS NULL THEN ATTRIBUTE.MAX_LENGTH
    ELSE ATOMIC.ATOMIC_SIZE
  END) > 2 AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ATT_VALUE IS NOT NULL AND ATT_VALUE != "" AND ENDPOINT_TYPE_ATTRIBUTE.BOUNDED !=0 AND REPLACE(ATT_VALUE, '0', '')!='x' AND REPLACE(ATT_VALUE, '0', '')!=''
  AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
GROUP BY CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE, ATTRIBUTE.SIDE )
        `
    )
    .then((rows) => rows.map(mapFunction))
}

/**
 * The reportable attribute details per endpoint per clusters.
 * @param {*} db
 * @param {*} endpointsAndClusters
 * @param {*} packageIds
 * @returns * The reportable attribute details per endpoint per clusters.
 */
async function selectReportableAttributeDetailsFromEnabledClustersAndEndpoints(
  db,
  endpointsAndClusters,
  packageIds
) {
  let endpointClusterIds = endpointsAndClusters
    .map((ep) => ep.endpointClusterId)
    .toString()
  let mapFunction = (x) => {
    return {
      id: x.ATTRIBUTE_ID,
      name: x.NAME,
      code: x.CODE,
      side: x.SIDE,
      mfgCode: x.MANUFACTURER_CODE,
      clusterName: x.CLUSTER_NAME,
      clusterCode: x.CLUSTER_CODE,
      attributeReportableMinValue: x.MIN_INTERVAL,
      attributeReportableMaxValue: x.MAX_INTERVAL,
      attributeReportableChange: x.REPORTABLE_CHANGE,
      endpointIdentifier: x.ENDPOINT_IDENTIFIER,
    }
  }
  return dbApi
    .dbAll(
      db,
      `
  SELECT
    ATTRIBUTE.ATTRIBUTE_ID,
    ATTRIBUTE.NAME,
    ATTRIBUTE.CODE,
    ATTRIBUTE.SIDE,
    ATTRIBUTE.MANUFACTURER_CODE,
    CLUSTER.NAME AS CLUSTER_NAME,
    CLUSTER.CODE AS CLUSTER_CODE,
    ENDPOINT_TYPE_ATTRIBUTE.MIN_INTERVAL,
    ENDPOINT_TYPE_ATTRIBUTE.MAX_INTERVAL,
    ENDPOINT_TYPE_ATTRIBUTE.REPORTABLE_CHANGE,
    ENDPOINT.ENDPOINT_IDENTIFIER
  FROM ATTRIBUTE
  INNER JOIN ENDPOINT_TYPE_ATTRIBUTE
  ON ATTRIBUTE.ATTRIBUTE_ID = ENDPOINT_TYPE_ATTRIBUTE.ATTRIBUTE_REF
  INNER JOIN ENDPOINT_TYPE_CLUSTER
  ON ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF = ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_CLUSTER_ID
  INNER JOIN CLUSTER
  ON ENDPOINT_TYPE_CLUSTER.CLUSTER_REF = CLUSTER.CLUSTER_ID
  INNER JOIN ENDPOINT
  ON ENDPOINT_TYPE_CLUSTER.ENDPOINT_TYPE_REF = ENDPOINT.ENDPOINT_TYPE_REF
  INNER JOIN ATOMIC
  ON ATOMIC.NAME = ATTRIBUTE.TYPE
  WHERE ENDPOINT_TYPE_ATTRIBUTE.ENDPOINT_TYPE_CLUSTER_REF IN (${endpointClusterIds})
  AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED = 1 AND ENDPOINT_TYPE_CLUSTER.ENABLED=1 AND ENDPOINT_TYPE_CLUSTER.SIDE=ATTRIBUTE.SIDE
  AND ENDPOINT_TYPE_ATTRIBUTE.INCLUDED_REPORTABLE = 1
  AND ATTRIBUTE.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  GROUP BY CASE WHEN SINGLETON=0 THEN ENDPOINT.ENDPOINT_IDENTIFIER END, CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE, ATTRIBUTE.SIDE
  HAVING CASE WHEN SINGLETON=1 THEN ENDPOINT.ENDPOINT_IDENTIFIER = MIN(ENDPOINT.ENDPOINT_IDENTIFIER) ELSE SINGLETON=0 END
  ORDER BY ENDPOINT.ENDPOINT_IDENTIFIER, CLUSTER.MANUFACTURER_CODE, CLUSTER.CODE, ATTRIBUTE.CODE, ATTRIBUTE.MANUFACTURER_CODE
        `
    )
    .then((rows) => rows.map(mapFunction))
}

async function selectAttributeByCode(
  db,
  packageIds,
  clusterCode,
  attributeCode,
  manufacturerCode
) {
  if (clusterCode == null) {
    return selectGlobalAttributeByCode(
      db,
      packageIds,
      attributeCode,
      manufacturerCode
    )
  } else {
    return selectNonGlobalAttributeByCode(
      db,
      packageIds,
      clusterCode,
      attributeCode,
      manufacturerCode
    )
  }
}

async function selectNonGlobalAttributeByCode(
  db,
  packageIds,
  clusterCode,
  attributeCode,
  manufacturerCode
) {
  let manufacturerCondition
  let arg = [attributeCode, clusterCode]

  if (manufacturerCode == null || manufacturerCode == 0) {
    manufacturerCondition = 'C.MANUFACTURER_CODE IS NULL'
  } else {
    manufacturerCondition =
      '( C.MANUFACTURER_CODE IS NULL OR C.MANUFACTURER_CODE = ? )'
    arg.push(manufacturerCode)
  }
  return dbApi
    .dbGet(
      db,
      `
SELECT
  A.ATTRIBUTE_ID,
  A.CLUSTER_REF,
  A.CODE,
  A.MANUFACTURER_CODE,
  A.NAME,
  A.TYPE,
  A.SIDE,
  A.DEFINE,
  A.MIN,
  A.MAX,
  A.REPORT_MIN_INTERVAL,
  A.REPORT_MAX_INTERVAL,
  A.REPORTABLE_CHANGE,
  A.REPORTABLE_CHANGE_LENGTH,
  A.IS_WRITABLE,
  A.DEFAULT_VALUE,
  A.IS_OPTIONAL,
  A.REPORTING_POLICY,
  A.IS_NULLABLE,
  A.IS_SCENE_REQUIRED,
  A.ARRAY_TYPE,
  A.MUST_USE_TIMED_WRITE
FROM ATTRIBUTE AS A
INNER JOIN CLUSTER AS C
ON C.CLUSTER_ID = A.CLUSTER_REF
WHERE A.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  AND A.CODE = ?
  AND C.CODE = ?
  AND ${manufacturerCondition}`,
      arg
    )
    .then(dbMapping.map.attribute)
}

async function selectGlobalAttributeByCode(
  db,
  packageIds,
  attributeCode,
  manufacturerCode
) {
  let manufacturerCondition
  let arg = [attributeCode]

  if (manufacturerCode == null || manufacturerCode == 0) {
    manufacturerCondition = 'A.MANUFACTURER_CODE IS NULL'
  } else {
    manufacturerCondition =
      '( A.MANUFACTURER_CODE IS NULL OR A.MANUFACTURER_CODE = ? )'
    arg.push(manufacturerCode)
  }
  return dbApi
    .dbGet(
      db,
      `
SELECT
  A.ATTRIBUTE_ID,
  A.CLUSTER_REF,
  A.CODE,
  A.MANUFACTURER_CODE,
  A.NAME,
  A.TYPE,
  A.SIDE,
  A.DEFINE,
  A.MIN,
  A.MAX,
  A.REPORT_MIN_INTERVAL,
  A.REPORT_MAX_INTERVAL,
  A.REPORTABLE_CHANGE,
  A.REPORTABLE_CHANGE_LENGTH,
  A.IS_WRITABLE,
  A.DEFAULT_VALUE,
  A.IS_OPTIONAL,
  A.REPORTING_POLICY,
  A.IS_NULLABLE,
  A.IS_SCENE_REQUIRED,
  A.ARRAY_TYPE,
  A.MUST_USE_TIMED_WRITE
FROM ATTRIBUTE AS A
WHERE A.PACKAGE_REF IN (${dbApi.toInClause(packageIds)})
  AND A.CODE = ?
  AND ${manufacturerCondition}`,
      arg
    )
    .then(dbMapping.map.attribute)
}

/**
 * Retrieves the global attribute data for a given attribute and cluster code.
 *
 * @param {*} db
 * @param {*} clusterRef
 * @param {*} attributeRef
 */
async function selectGlobalAttributeDefaults(db, clusterRef, attributeRef) {
  return dbApi
    .dbAll(
      db,
      `
SELECT
  GAD.DEFAULT_VALUE,
  GAB.BIT,
  GAB.VALUE,
  (SELECT NAME FROM TAG WHERE TAG_ID = GAB.TAG_REF) AS TAG
FROM
  GLOBAL_ATTRIBUTE_DEFAULT AS GAD
LEFT JOIN
  GLOBAL_ATTRIBUTE_BIT AS GAB
ON
  GAD.GLOBAL_ATTRIBUTE_DEFAULT_ID = GAB.GLOBAL_ATTRIBUTE_DEFAULT_REF
WHERE
  GAD.CLUSTER_REF = ?
  AND GAD.ATTRIBUTE_REF = ?
ORDER BY
  GAD.CLUSTER_REF, GAD.ATTRIBUTE_REF, GAB.BIT
`,
      [clusterRef, attributeRef]
    )
    .then((rows) =>
      rows.reduce((ac, row) => {
        if (!('default_value' in ac)) {
          ac.defaultValue = row.DEFAULT_VALUE
        }
        if (row.BIT != null) {
          if (!('featureBits' in ac)) {
            ac.featureBits = []
          }
          ac.featureBits.push({ bit: row.BIT, value: row.VALUE, tag: row.TAG })
        }
        return ac
      }, {})
    )
}

exports.selectAllAttributeDetailsFromEnabledClusters =
  selectAllAttributeDetailsFromEnabledClusters
exports.selectManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters =
  selectManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters
exports.selectNonManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters =
  selectNonManufacturerSpecificAttributeDetailsFromAllEndpointTypesAndClusters
exports.selectAttributeDetailsWithABoundFromEnabledClusters =
  selectAttributeDetailsWithABoundFromEnabledClusters
exports.selectAttributeDetailsFromEnabledClusters =
  selectAttributeDetailsFromEnabledClusters
exports.selectAttributeBoundDetails = selectAttributeBoundDetails
exports.selectReportableAttributeDetailsFromEnabledClustersAndEndpoints =
  selectReportableAttributeDetailsFromEnabledClustersAndEndpoints
exports.selectGlobalAttributeDefaults = selectGlobalAttributeDefaults
exports.selectAttributeByCode = selectAttributeByCode
exports.duplicateEndpointTypeAttribute = duplicateEndpointTypeAttribute
exports.selectEndpointTypeAttributesByEndpointTypeRefAndClusterRef = selectEndpointTypeAttributesByEndpointTypeRefAndClusterRef
