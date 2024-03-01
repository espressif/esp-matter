## RainMaker Cluster

Attributes and commands for `Espressif Badge` User-Node Association.


### 1. Revision History

The global ClusterRevision attribute value SHALL be the highest revision number in the table below.

| Revision | Description                           |
|:--------:|:-------------------------------------:|
| 1        | Initial cluster revision              |


### 2. Classification

| Hierarchy | Role        | Scope    |
|:---------:|:-----------:|:--------:|
|Base       | Application | Endpoint |


### 3. Cluster ID

| ID         | Name           |
|:----------:|:--------------:|
| 0x131BFC03 | EspressifBadge |


### 4. Attributes

| ID     | Name              | Type   | Constraint | Quality | Default | Access | Conformance |
|:------:|:-----------------:|:------:|:----------:|:-------:|:-------:|:------:|:-----------:|
| 0x0000 | Name              | string | max 32     |         |         | RW     | D           |
| 0x0001 | CompanyName       | string | max 32     |         |         | RW     | M           |
| 0x0002 | Email             | string | max 32     |         |         | RW     | M           |
| 0x0003 | Contact           | string | max 32     |         |         | RW     | M           |
| 0x0004 | EventName         | string | max 32     |         |         | RW     | M           |

#### 4.1. Name Attribute

This attribute SHALL contain the name of the person wearing the badge.

#### 4.2. Company Name Attribute

This attribute SHALL contain the name of company of person wearing the badge.

#### 4.3. Email Attribute

This attribute SHALL contain Email.

#### 4.4. Contact Attribute

This attribute SHALL contain the contact number in the string format.

#### 4.5. Event Name Attribute

This attribute SHALL cotain the event name.


### 5. Commands

| ID   | Name          | Direction       | Response | Access | Conformance |
|:----:|:-------------:|:---------------:|:--------:|:------:|:-----------:|
| 0x00 | Configuration | client ⇒ server | Y        | O      | M           |

#### 5.1. Configuration Command

This command instructs the badge to display the Name, CompanyName, Email, Contact and EventName on epaper display.
- Prepare the command payload and <secret-key>

| ID     | Name              | Type   | Constraint | Quality | Default | Access | Conformance |
|:------:|:-----------------:|:------:|:----------:|:-------:|:-------:|:------:|:-----------:|
| 0x0000 | Name              | string | max 32     |         |         | RW     | M           |
| 0x0001 | CompanyName       | string | max 32     |         |         | RW     | M           |
| 0x0002 | Email             | string | max 32     |         |         | RW     | M           |
| 0x0003 | Contact           | string | max 32     |         |         | RW     | M           |
| 0x0004 | EventName         | string | max 32     |         |         | RW     | M           |

##### 5.1.1. Effect on Receipt

On receipt of Configuration command, a server SHALL set the attribute Name, CompanyName, Email, Contact and EventName. Additionally server SHALL display the Name, CompanyName, Email, Contact and EventName on epaper display.
