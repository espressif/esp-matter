## RainMaker Cluster

Attributes and commands for Rainmaker User-Node Association.


### 1. Revision History

The global ClusterRevision attribute value SHALL be the highest revision number in the table below.

| Revision | Description                           |
|:--------:|:-------------------------------------:|
| 1        | Initial cluster revision              |
| 2        | Added challenge (writeable) attribute |


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

This attribute SHALL contain the signature of the data to be signed using the DAC key.
The data to be signed can either be written to the Challenge attribute or provided by invoking the SignData command.

#### 4.4. Challenge Attribute

Client SHOULD write the data to be signed to this attribute.


### 5. Commands

| ID   | Name          | Direction       | Response | Access | Conformance |
|:----:|:-------------:|:---------------:|:--------:|:------:|:-----------:|
| 0x00 | Configuration | client ⇒ server | Y        | O      | D           |
| 0x01 | SignData      | client ⇒ server | Y        | O      | M           |

#### 5.1. Configuration Command

This command initiates the Rainmaker User-Node Association.

In order to Invoke this command, client must have Rainmaker Node Id.
Please follow below steps for complete flow:

- Read the Rainmaker Node Id attribute.
- Trigger the User-Node association, by calling add-user API, or using rainmaker cli
    ```
    rainmaker.py test --addnode <rainmaker-node-id>

    # Output is in format <user-id> <secret-key>
    ```
- Prepare the command payload by concatinating <user-id>, "::", and <secret-key>

| ID   | Name             | Type   | Constraint | Quality | Default | Conformance |
|-----:|:----------------:|:------:|:----------:|:-------:|:-------:|:-----------:|
| 0x00 | ConfigurationArg | string | desc       |         |         | M           |

##### 5.1.1. Effect on Receipt

On receipt of this command, server SHALL perform the user node association with the received user-id.
It SHALL update the Status attribute as per the response of the API call.

#### 5.2 SignData Command

This command signs the data received in the argument with DAC key and stores the signature in ASN1 format in
ChallengeResponse attribute.

| ID   | Name        | Type   | Constraint | Quality | Default | Conformance |
|-----:|:-----------:|:------:|:----------:|:-------:|:-------:|:-----------:|
| 0x00 | SignDataArg | string | desc       |         |         | M           |
