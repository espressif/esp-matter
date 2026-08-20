## RainMaker Cluster

Attributes and commands for Rainmaker User-Node Association.


### 1. Revision History

The global ClusterRevision attribute value SHALL be the highest revision number in the table below.

| Revision | Description                                                                     |
|:--------:|:-------------------------------------------------------------------------------:|
| 1        | Initial cluster revision                                                        |
| 2        | Added challenge (writeable) attribute                                           |
| 3        | Sign device's own Matter node ID instead of arbitrary data and removed commands |


### 2. Classification

| Hierarchy | Role        | Scope    |
|:---------:|:-----------:|:--------:|
|Base       | Application | Endpoint |


### 3. Cluster ID

| ID         | Name      |
|:----------:|:---------:|
| 0x131BFC00 | Rainmaker |


### 4. Attributes

| ID     | Name              | Type   | Constraint | Quality | Default | Access | Conformance |
|:------:|:-----------------:|:------:|:----------:|:-------:|:-------:|:------:|:-----------:|
| 0x0000 | Status            | bool   | all        |         | FALSE   | RV     | D           |
| 0x0001 | RmakerNodeId      | string | max 40     |         | MS      | RV     | M           |
| 0x0002 | ChallengeResponse | string | max 150    |         | MS      | RV     | M           |
| 0x0003 | Challenge         | string | max 40     |         | MS      | RW VO  | M           |

#### 4.1. Status Attribute

This attribute SHALL indicates the Rainmaker User-Node Association status,
TRUE means the user and node are associated, and FALSE means otherwise.

#### 4.2. RmakerNodeId Attribute

This attribute SHALL contain the Rainmaker Node Id.

#### 4.3. ChallengeResponse Attribute

This attribute SHALL contain the DAC signature of the device's own Matter operational node ID.

Writing to the Challenge attribute triggers signing, the device always signs its own node ID.

#### 4.4. Challenge Attribute

Client SHOULD write to this attribute to trigger signing. The written value is ignored.
