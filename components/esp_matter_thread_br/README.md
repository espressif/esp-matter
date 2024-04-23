# Thread Border Router Cluster

The Thread Border Router (BR) Cluster offers an interface for managing the ESP Thread BR. It allows users to perform various tasks such as configuring the dataset of the Thread network that the BR will form or join, start or stop Thread network.

## 1. Cluster Identifiers

| Identifier | Name                     |
|------------|--------------------------|
| 0x131BFC02 | **Thread Border Router** |

## 2. Data Types

### 2.1 ThreadRoleEnum Type

This data type is derived from enum8.

| Value | Name     | Summary                                   | Conformance |
|-------|----------|-------------------------------------------|-------------|
| 0     | Disabled | The Thread is disabled                    | M           |
| 1     | Detached | The Node is detached to a Thread network  | M           |
| 2     | Child    | The Node acts as a Child Role             | M           |
| 3     | Router   | The Node acts as a Router Role            | M           |
| 4     | Leader   | The Node acts as a Leader Role            | M           |

## 2. Attributes

| ID     | Name              | Type           | Constranint | Quality | Default | Access | Conformance |
|--------|-------------------|----------------|-------------|---------|---------|--------|-------------|
| 0x0000 | **DatasetTlvs**   | octstr         |  max254     | N       |         | R V    | M           |
| 0x0001 | **Role**          | ThreadRoleEnum |             |         |         | R V    | M           |
| 0x0002 | **BorderAgentId** | octstr         |  16         | N       |         | R V    | M           |

### 2.1 DatasetTlvs Attribute

This attribute stores the dataset Tlvs of the Thread network that the BR will form or join. It will be updated after the ConfigureDatasetTlvs command is handled and the dataset is successfully committed.

### 2.2 Role Attribute

This attribute stores the Thread network role of the Thread BR.

### 2.3 BorderAgentId Attribute

This attribute stores the the randomly generated Border Agent ID. The typical use case of the ID is to be published in the MeshCoP mDNS service as the `id` TXT value for the client to identify this Border Router/Agent device.

## 3. Commands

| ID     | Name                     | Direction      | Response | Access | Conformance |
|--------|--------------------------|----------------|----------|--------|-------------|
| 0x0000 | **ConfigureDatasetTlvs** | client->server | Y        | A      | M           |
| 0x0001 | **StartThread**          | client->server | Y        | A      | M           |
| 0x0002 | **StopThread**           | client->server | Y        | A      | M           |


### 3.1 ConfigureDatasetTlvs Command

The ConfigureDatasetTlvs command allows the Thread BR to configure the dataset Tlvs of its Thread network. The DatasetTlvs Attribute will be updated after the dataset is commited.

The ConfigureDatasetTlvs command SHALL have the following data fields:

| ID | Name               | Type   | Constraint | Quality | Default | Comformance |
|----|--------------------|--------|------------|---------|---------|-------------|
| 0  | **DatasetTlvsStr** | string |  max508    |         |         | M           |

#### 3.1.1 DatasetTlvsStr Field

This field is the dataset tlvs string which will be conmmited.

### 3.2 StartThread Command

The StartThread command allows devices to form or join Thread network.

The StartThread command has no data field.

### 3.3 StopThread Command

The StopThread command allows devices to stop its Thread network.

The StopThread command has no data field.
